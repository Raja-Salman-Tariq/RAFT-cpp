#include <iostream>
#include <string>
#include "routing_module.h"
#include "message_module.h"
#include "key_value_module.h"

#define us_int unsigned short int
using namespace std;


struct Scheduler {};


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
		peers(*routing_table.find_peers(addr)),
		role(_role)

	{
		//cout<<peers.size();
		peers.remove(addr);
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
	us_int addr;
	PersistantKeyValueStore persistent_storage;
	Scheduler scheduler;
	Messenger messenger;
	RoutingTable & routing_table;
	list<us_int> peers;
	string role;

	us_int current_term;
	us_int voted_for;

};