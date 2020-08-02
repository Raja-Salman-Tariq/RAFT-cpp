#include <openssl/md5.h>
#include <string>
#include <list>
#include <fstream>
#include <map>
#include <iostream>
#include <tuple>


#define us_int unsigned short int
#define HASH first
#define CLUSTER second

using namespace std;


std::string NETWORK_FILE = "network.txt";
us_int HIGHEST_ADDR = 2 ^ 10;
/*
#-------------------------------------------------------------------------------
"""
RoutingTable    :   Zero-Hop Router for Node
"""
*/

class RoutingTable {


private:
	list<pair<us_int, list<us_int>>> routing_table;
	us_int address_space;

public:
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
		//cluster.clear();
		us_int maxKeyHash;

		us_int no = 1; // to store maxKey as well as node addresses.
		char c = '1';  // house keeping variables
		string s;	   // enables traversal of next line
		us_int len;    // for printing lists of sizes 'len's

		//cout << "ok...";
		//myFile >> maxKeyHash;


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
				//cout << "next reads " << no << " and " << c << endl;
			}

			routing_table.emplace_back(maxKeyHash, cluster);
/*
			len = routing_table.size();
			cout << "\n\n*********prints************\n";
			for (auto i = routing_table.cbegin(); i != routing_table.cend(); i++) {
				cout << "Hash:" << i->HASH << ", Cluster: [";
				for (auto j = i->CLUSTER.cbegin(); j != i->CLUSTER.cend(); j++)
					printf(" %d,", *j);
				cout << "]\n";
			}
*/
			//cin >> len;
			cluster.clear();
			getline(myFile, s);
			//printf("printing remainder %s and cluster size %d, and eof % and file % ", s, cluster.size());

		}

	}

	const pair<us_int, list<us_int>> & find_responsible_cluster(us_int key) {

		auto i = routing_table.cbegin(), end = routing_table.cend();
		for (; i != end; i++) {
			if (key <= i->HASH)
				return *i;
		}

		return *i;
	}

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

	/*  """


		/*
		"""
		hash_key    :   Hashes Key with md5, then % the Highest Address
		    Input(s):
		        key :   Key to Hash
		    Output(s):
		        Hash of Key
		"""
		def hash_key(key: str):
		    """
		    Hash the key
		    """
		    return int(md5(key.encode()).hexdigest(), 16) % HIGHEST_ADDR

		*/

	int hash_key(string key) {
		unsigned char store[128];
		unsigned char * hash = MD5((unsigned char*)key.c_str(), key.length(), store);

/*
		for (int i = 0; i < 16; i++)
			printf("%02x", hash[i] );

		cout << "\nhash:" << *((unsigned int *)(hash + 1)) << endl;*/

		return *hash;
	}



	};