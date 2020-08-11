#ifndef routing 
#define routing

#include <openssl/md5.h>
#include <string>
#include <list>
#include <fstream>
#include <map>
#include <iostream>
#include <tuple>


#define us_int unsigned short int
#define HASH first 		// for readability when
#define CLUSTER second 	// accessing pairs

using namespace std;


std::string NETWORK_FILE = "network.txt";
us_int HIGHEST_ADDR = 2 ^ 10;



	/* ===========================
		  --- RoutingTable ---
		Zero-Hop Router for Node
	   ==========================*/
class RoutingTable {
public:

	// constructor
	RoutingTable(us_int _addr) {
		//routing_table.clear();
		address_space = 0;
		fstream myFile(NETWORK_FILE, ios::in);


		// Error checking; case when network.txt does not exist.
		if (!myFile.is_open()) {
			cout << "Error: network.txt not found.\nExiting program.\n\n";
			exit(1);
		}

		list<us_int> cluster;
		us_int maxKeyHash;

		us_int no = 1; // to store maxKey as well as node addresses.
		char c = '1';  // house keeping variables
		string s;	   // enables traversal of next line
		us_int len;    // for printing lists of sizes 'len's

		// loop until EOF is found
		while (myFile.peek() != EOF) {
			myFile >> maxKeyHash;

			// c is for housekeeping i.e. handling irrelevant.
			while (c != '[') {
				myFile >> c;
			}

			// gets addresses belonging to a cluster.
			while (c != ']') {
				myFile >> no;
				cluster.push_back(no);
				myFile >> c;
			}

			routing_table.emplace_back(maxKeyHash, cluster);

			cluster.clear();
			getline(myFile, s);

		}

	}

	// find_responsible_cluster(key)
	const pair<us_int, list<us_int>> & find_responsible_cluster(us_int key) {

		auto i = routing_table.cbegin(), end = routing_table.cend();
		for (; i != end; i++) {
			if (key <= i->HASH)
				return *i;
		}

		return *i;
	}


	// find_peers(address)
	const list<us_int> * find_peers(us_int addr) {

		auto i = routing_table.cbegin(), end = routing_table.cend();
		auto j = i->CLUSTER.cbegin(), jend = i->CLUSTER.cend();

		for (; i != end; i++) {
			j = i->CLUSTER.cbegin(), jend = i->CLUSTER.cend();

			for (; j != jend; j++)
				if (addr == *j)
					return & i->CLUSTER;
		}
		return 0;
	}

	// Hash Function
	// to be reviewed
	int hash_key(string key) {
		unsigned char store[128];
		unsigned char * hash = MD5((unsigned char*)key.c_str(), key.length(), store);
		return *hash;
	}

private:
	list<pair<us_int, list<us_int>>> routing_table; // to store routing table records
	us_int address_space;							// maximum keyspace that is going to review


	};

	#endif