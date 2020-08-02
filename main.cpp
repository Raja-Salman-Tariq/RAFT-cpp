#include "node.h"

int main(){
	PersistantKeyValueStore p(9);
	Messenger m;
	Message mm;
	Scheduler s;
	RoutingTable t(100);
	RaftNode  n(100,p,s,m,t);
	n.showPeers();

	cout<<"thingie: "<<fopen("kuchar kew xD.txt", "r");

	return 0;
}