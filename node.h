#ifndef node_present
#define node_present

#include <iostream>
#include <string>
#include <unistd.h>
#include <functional>
#include <zmq.h>
#include "message_module.h"
#include "routing_module.h"
#include "key_value_module.h"
#include "log_module.h"
#include "async_module.h"
//#include "handlers.h"




#define us_int unsigned short int
using namespace std;


struct transactions {};


class RaftNode {
public:
	/*		the variables begining with an underscore '_' are
			object initialization paremeters. They get copied
			into the class data members (not begining
			with an underscore).

			The RaftNode "function" below initializes the class
			It is the constructor.

			Particularly, a member initialization list has
			been used, where to copy parameters (P) into data
			members (DM), we use the following syntax:

			: DM1(P1), DM2(P2), ... , DMn(Pn)
	*/

	// constructor
	RaftNode(	us_int _addr,
	            PersistantKeyValueStore _persistent_storage,
	            Scheduler _scheduler,
	            Messenger _messenger,
	            RoutingTable & _routing_table,
	            string _role = "")  :

		addr(_addr),
		persistent_storage(_persistent_storage),
		scheduler(_scheduler),
		messenger(_messenger),
		routing_table(_routing_table),
		peers(*_routing_table.find_peers(addr)),
		role(_role)

	{

		peers.remove(addr);
		replication_running = 0;

		//h.emplace(match_type("REQUEST_VOTE"),&handle_REQUEST_VOTE);
		// add handlers here
		string logStr="Starting Node: "+to_string(addr)+" Peers: ";
		for (auto& peer : peers)
			logStr+=to_string(peer)+" ";
		debug_log(logStr,addr);


	}

private:

	// Network Topology
	us_int addr;
	list<us_int> peers;
	us_int leader;


	// Modules
	Messenger messenger;
	Log log;
	PersistantKeyValueStore persistent_storage;
	RoutingTable & routing_table;
	Scheduler scheduler;


	// State Machine
	string role;


	// Follower
	us_int current_term;
	us_int voted_for;
	void* election_timer;	// ** CAUTION ** verify data type


	// Candidate
	void* candidacy;		// please refer to README for
	// reasoning for using void*


	// Leader
	us_int last_request_id;
	list<transactions> pending_transactions;
	void* replication_running;
	map<us_int, us_int> next_index;		// Next log index per peer
	map<us_int, us_int> match_index; 	// Successfully replicated log index per peer
	us_int commit_index;	// Indexes considered committed
	us_int last_applied;	// Last command in DB
	void* heartbeat_timer;

	// Housekeeping
	list<void*> cancelled_operations;

	// Timeouts
	us_int ELECTION_TIMEOUT_MIN = 8,
	       ELECTION_TIMEOUT_MAX = 10,
	       HEARTBEAT_TIMEOUT_MIN = 4,
	       HEARTBEAT_TIMEOUT_MAX = 5;

	// Handlers
	list<Handler> h;


public:

	/* ===========================
		--- MEMBER FUNCTIONS ---
	   ==========================*/


	// setter
	void set_role_follower() {
		role = "FOLLOWER";
	}

	// setter
	void set_role_candidate() {
		role = "CANDIDATE";
	}

	// setter
	void set_role_leader() {
		role = "LEADER";
	}


	/* ===========================
		--- MEMBER FUNCTIONS ---
			as in python cod

	   Any significant alteration
	   	 mentioned as comment(s)
	   	 	 within functions
	   ==========================*/

	void step_down(us_int new_term) {
		test_log("RaftNode " + to_string(addr) + ": Stepping Down as LEADER", addr);
		current_term = new_term;
		become_follower();
		set_election_timer();
	}

	void become_follower() {
		// instead of passing entire node object, logging functions just
		//	pass node number
		debug_log("RaftNode " + to_string(addr) + ": BECOME FOLLOWER", addr);
		test_log("RaftNode " + to_string(addr) + ": BECOME FOLLOWER", addr);

		role = "FOLLOWER";
		/*
		next_index = 0,		// with us_int, the system uses 0 for no/null values
		match_index = 0,*/

		// the 2 lines above commented; followers may not have match and next indexes
		// but instead of setting them as empty here, we can instead do so when
		// becoming leader, and avoid handling them. Access to these shall be restricted
		// via role.

		voted_for = 0;

		if (replication_running) {
			cancelled_operations.push_back(replication_running);
			replication_running = 0;	// a null value assigned to show absence
			// refer to READMD
		}

		if (candidacy) {
			cancelled_operations.push_back(candidacy);
			candidacy = 0;
		}

		clear_heartbeat_timer();
	}

	void clear_heartbeat_timer() {
		if (heartbeat_timer)
			cancelled_operations.push_back(heartbeat_timer); //heartbeat_timer.cancel();// make heartbeat timer a ptr to future obj maybe ?
		heartbeat_timer = 0;
	}

	void set_heartbeat_timer() {
		if (heartbeat_timer)
			cancelled_operations.push_back(heartbeat_timer);
		auto fut = async(std::launch::async, [this]() {heartbeat_timeout();});
		heartbeat_timer = &fut;
	}

	void set_election_timer() {
		if (election_timer) {
			cancelled_operations.push_back(election_timer);
			election_timer = 0;
		}
		auto fut = (std::async(std::launch::async, [this]() {timeout();})); // to asynchronously call functions

		election_timer = (void*) & (fut);
	}

	void become_candidate() {
		debug_log("RaftNode " + to_string(addr) + ": BECOME CANDIDATE", addr);
		test_log("RaftNode" + to_string(addr) + ": BECOME CANDIDATE", addr);
		clear_heartbeat_timer();

		leader = 0;
		role = "CANDIDATE";
		set_election_timer();
	}


	void become_leader() {
		debug_log("Node " + to_string(addr) + ": BECOME LEADER", addr);
		test_log("RaftNode " + to_string(addr) + ": BECOME LEADER", addr);

		match_index.clear();
		next_index.clear();

		for (auto& p : peers) {
			match_index.emplace(p, 0);
			next_index.emplace(p, log.latest_index() + 1);
		}

		clear_election_timer();
		start_replication();
		//start_heartbeat(); //-- func def not found


	}

	void clear_election_timer() {
		if (election_timer) {
			cancelled_operations.push_back(election_timer);
			election_timer = 0;
		}
	}

	void start_replication() {
		if (replication_running) {
			cancelled_operations.push_back(replication_running);
		}

		auto fut = async(std::launch::async, [this]() {replicate_log_to_peers();});
		replication_running = &fut;
		set_heartbeat_timer();
	}

	//======================================================
	// Incomplete functions;
	// ...scheduler and async not fully implemented...
	// if any one of these is completed, the rest should cascade
	//	in terms of syntax, logic, understanding...

	void replicate_log_to_peers() {}
	void get_vote_from_node(const us_int & peer) {
		auto condition = match_type_src;
	}
	//======================================================


	bool request_votes_and_get_majority() {
		string s = "RaftNode" + to_string(addr) + ": requesting votes from peers:";
		for (auto peer : peers)
			s += " " + (to_string(peer)) + ",";
		//need to delete last trailing ',';
		debug_log(s, addr);

		list<void*> vote_request_tasks;

		for (auto& peer : peers) {
			auto fut = (async(std::launch::async, [this, &peer]() {get_vote_from_node(peer);}));
			vote_request_tasks.push_back(&fut);
		}

		int y_votes = 0;

		// Incomplete function
		// ...scheduler and async not fully implemented...
		for (auto & request_task : vote_request_tasks) {
			// simply casting request_task to a future ptr
			// and then calling the future::get() SHOULD
			// suffice to complete the implementation till
			// this loop. HOWEVER, it has not been tested
			// and may have an issue in casting and calling
			// as futures are "temporary"

			// Additionally, even if this works, it may not
			// provide truly async functionality, and may
			// be large asynchronous mostly, but not always
			// depending on backend implementation of async
		}

		return 1;
	}

	void start_candidacy() {
		debug_log("RaftNode " + to_string(addr) + ": Starts Candidacy", addr);
		bool election_result = 0;
		current_term += 1;
		become_candidate();
		voted_for = addr;

		election_result = request_votes_and_get_majority();
	}


	void timeout() {
		float time_to_sleep = ELECTION_TIMEOUT_MIN + static_cast <float> (rand()) / ( static_cast <float> (RAND_MAX / (ELECTION_TIMEOUT_MAX - ELECTION_TIMEOUT_MIN)));

		sleep(time_to_sleep); // this is likely blocking sleep, but async SHOULD
		// be able to work around this by default...

		if (candidacy)
			cancelled_operations.push_back(candidacy);

		if (role != "LEADER")
			candidacy = (void*) 1;

		auto fut = std::async(std::launch::async, [this]() {start_candidacy();});
		candidacy = (void *) &fut;
	}

	void heartbeat_timeout() {
		float time_to_sleep = HEARTBEAT_TIMEOUT_MIN + static_cast <float> (rand()) / ( static_cast <float> (RAND_MAX / (HEARTBEAT_TIMEOUT_MAX - HEARTBEAT_TIMEOUT_MIN)));
		sleep(time_to_sleep);

		auto task = async(std::launch::async, [this]() {heartbeat();});
		task.get();
	}

	void heartbeat() {
		if (role == "LEADER")
			start_replication();
	}

	void send_to_broker(map<map<string, string>, map<string, string>> d) {
		//	req.send_json(d);
		cout << "*******************************************\n"
		     << "NEEDS TO BE IMPLEMENTED\n"
		     << "*******************************************";
	}

	// helper function; unimportant
	// useful for debugging, may be ignored
	void showPeers() {
		auto i = peers.cbegin(), end = peers.cend();
		cout << "\nPeers: ";

		for (; i != end; i++) {
			cout << *i << ", ";
		}
		printf("\n\n");
	}


	// to allow RaftNode private members be accessible without getters
	// by external function(s) -- may be ommitted in final version
	friend void handle_REQUEST_VOTE(Message m, RaftNode& node);



};
#endif