#include "node.h"

void sendingLol(Message* m){
	printf("\n\n**************\nMsg sent:\nType:%s\nSrc%d\nDst:%d\nArgs:-", 
		m->getType(), m->getSrc(), m->getDst());
	 m->showArgs();
}

int main(){
	PersistantKeyValueStore p(9);
	Messenger m(100, &sendingLol);
	Scheduler s;
	RoutingTable t(100);
	RaftNode  n(100,p,s,m,t);
	n.showPeers();

	list<int> la={};
	list<int> lb={0,-1,-2, -3,-4,-5,-6,-7};

	auto insertAt=la.cbegin(), insertFrom=lb.cbegin(), insertTill=lb.cbegin();
	cout<<"\nCbeg === cnd ?? "<<(insertAt==la.cend())<<endl;

	advance(insertFrom,3);
	advance(insertTill,4);

	la.insert(insertAt, insertFrom, insertTill	);

	auto i=la.cbegin(), end=la.cend();

	for(; i!=end; i++)
		cout<<*i<<", ";



	//cout<<"thingie: "<<fopen("kuchar kew xD.txt", "r");

	return 0;
}