#ifndef node_present
#define node_present

#include <iostream>
#include <string>
#include "routing_module.h"
#include "key_value_module.h"
#include "log_module.h"
#include "async_module.h"
#include <unistd.h>
#include <functional>
#include "message_module.h"




#define us_int unsigned short int
using namespace std;


//struct Scheduler {};

struct transactions {};


class RaftNode {
public:
	/*		the variables begining with an underscore '_' are
			object initialization paremeters. They get copied
			into the class data members (not begin
			with an underscore).

			The RaftNode function below initializes the class

			Particularly, a member initialization list has
			been used, where to copy parameters (P) into data
			members (DM), we use the following syntax:

			: DM1(P1), DM2(P2), ... , DMn(Pn)
	*/
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

		//cout<<peers.size();
		peers.remove(addr);
		replication_running=0;
		//cout<<peers.size();
	}

	//RaftNode(){cout<<"Creatus est\n";}

	void showPeers() {
		auto i = peers.cbegin(), end = peers.cend();
		cout << "\nPeers: ";

		for (; i != end; i++) {
			cout << *i << ", ";
		}
		printf("\n\n");
	}

	void set_role_follower() {
		role = "FOLLOWER";
	}

	void set_role_candidate() {
		role = "CANDIDATE";
	}

	void set_role_leader() {
		role = "LEADER";
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
	void* candidacy;


	// Leader
	us_int last_request_id;
	list<transactions> pending_transactions;
	void* replication_running;
	us_int next_index;		// Next log index per peer
	us_int match_index; 	// Successfully replicated log index per peer
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
public:
	void step_down(us_int new_term) {
		test_log("RaftNode " + to_string(addr) + ": Stepping Down as LEADER", addr);
		current_term = new_term;
		become_follower();
		set_election_timer();
	}

	void become_follower() {
		debug_log("RaftNode " + to_string(addr) + ": BECOME FOLLOWER", addr);
		test_log("RaftNode " + to_string(addr) + ": BECOME FOLLOWER", addr);

		role = "FOLLOWER";
		next_index = 0,
		match_index = 0,
		voted_for = 0;

		if (replication_running){
			cancelled_operations.push_back(replication_running);
			replication_running=0;
		}

		if (candidacy) {
			cancelled_operations.push_back(candidacy);
			candidacy=0;
		} 
		
		clear_heartbeat_timer();
	}

	void clear_heartbeat_timer() {
		if (heartbeat_timer)
			cancelled_operations.push_back(heartbeat_timer); //heartbeat_timer.cancel();// make heartbeat timer a ptr to future obj maybe ?
			heartbeat_timer = 0;
	}

	void set_election_timer() {
		if (election_timer) {
			cancelled_operations.push_back(election_timer);
			election_timer=0;
		}
		auto fut=(std::async(std::launch::async,[this](){timeout();}));
		election_timer=(void*)&(fut);
	}

	void become_candidate(){
		debug_log("RaftNode "+to_string(addr)+": BECOME CANDIDATE", addr);
		test_log("RaftNode"+to_string(addr)+": BECOME CANDIDATE", addr);
		clear_heartbeat_timer();

		leader=0;
		role="CANDIDATE";
		set_election_timer();
	}

	void get_vote_from_node(const us_int & peer){
		auto condition= match_type_src;
		

	}

	bool request_votes_and_get_majority(){
		string s="RaftNode"+to_string(addr)+": requesting votes from peers:";
		for (auto peer : peers)
			s+=" "+(to_string(peer))+",";
		//need to delete last trailing ',';
		debug_log(s, addr);

		list<void*> vote_request_tasks;

		for (auto& peer: peers){
			auto fut=(async(std::launch::async, [this,&peer](){get_vote_from_node(peer);}));
			vote_request_tasks.push_back(&fut);
		}

		int y_votes=0;

		for (auto & request_task : vote_request_tasks){

		}
			
		return 1;
	}

	void start_candidacy(){
//		cout<<"Voila ! Nodef: "<<addr;
		debug_log("RaftNode "+to_string(addr)+": Starts Candidacy", addr);
		bool election_result=0;
		current_term+=1;
		become_candidate();
		voted_for=addr;

		election_result=request_votes_and_get_majority();
	}


	void timeout(){
		float time_to_sleep= ELECTION_TIMEOUT_MIN + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(ELECTION_TIMEOUT_MAX-ELECTION_TIMEOUT_MIN)));
		sleep(time_to_sleep);
		if (candidacy)
			cancelled_operations.push_back(candidacy);

		if (role!="LEADER")
			candidacy=(void*) 1;

		auto fut=std::async(std::launch::async,[this](){start_candidacy();});
		candidacy=(void *) &fut;
	}
/*
	void start_replication() { 
		if (replication_running)
			scheduler.job_promises.at(replication_running)->set_value(0);

		replication_running = async(launch::async, replicate_log_to_peers);

		//set_heartbeat_timer();
	}

	void replicate_log_to_peers() {
		bool norm=false;
		try {
			list<auto> replicators;
			string s = "Waiting on Replicators...",
			       s_done = "Replication Done.";

			for (auto peer : peers)
				replicators.emplace(async(launch::async, replicate_log_to_node, peer));
			debug_log(s, addr);

			for (auto awaiter : replicators)
				awaiter.get();
			norm=true;
		}
		catch (auto e){}

		if (!norm)
			cout<<"Error : Exception encourtered in call to replicate_log_to_peers() in node.h.";
	}
/*
	void replicate_log_to_node(){
			bool accept=false;

			void* condition;

			try{
				while (!accept){
					condition++;
				}
			}
			catch (int e) {}
	}*/

	/*"""
	set_election_timer :   Sets the Election Timer
	Input(s):
	    None
	Output(s):
	    None
	Side Effect(s):
	    Sets the Election Timer for Node
	    When Timer Expires, the node will begin an Election as a Candidate
	"""
	def set_election_timer(self):
	"""
	Start an election timeout timer.
	    If we were a candidate for an election, we time out the election and start a new election timeout.
	"""
	#self.debug_log("Raft", self.addr, "restarting election timeout")
	if(self.election_timer != None):
	    # print("cancelinng election timer")
	    self.election_timer.cancel()

	async def timeout():
	    time_to_sleep = uniform(ELECTION_TIMEOUT_MIN, ELECTION_TIMEOUT_MAX)
	    await asyncio.sleep(time_to_sleep)
	    if self.candidacy != None:
	        self.candidacy.cancel()
	    # Can't figure out why this isn't stopping so let's just not try and elect if we are the main guy.
	    if not self.role == Role.LEADER:
	        self.candidacy = asyncio.create_task(self.start_candidacy())
	self.election_timer = asyncio.create_task(timeout())
	*/
	/*

	"""
	become_follower :   Becomes a Follower
	Input(s):
	    None
	Output(s):
	    None
	Side Effect(s):
	    Sets Role to Follower
	    Clears Next Index
	    Clears Match Index
	    Clears Voted For
	    If the Node was Replicating, Stop it
	    If the Node has a Candidacy, Stop it
	    Stop the Heatbeat Timer
	"""
	def become_follower(self):
	self.debug_log("RaftNode ", self.addr, ": BECOME FOLLOWER")
	self.test_log("RaftNode", self.addr, ": BECOME FOLLOWER")
	self.role = Role.FOLLOWER
	self.next_index = None
	self.match_index = None
	self.voted_for = None

	if(self.replication_running != None):
	    self.replication_running.cancel()
	if(self.candidacy != None):
	    self.candidacy.cancel()
	self.clear_heartbeat_timer()*/

public:

	friend void handle_REQUEST_VOTE(Message m, RaftNode& node);



};
#endif