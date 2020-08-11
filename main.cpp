#include "node.h"
//#include <coroutine>
#include <zmq.h>
#include <iostream>
#include <vector>

using namespace std;

struct gib{

	gib(int i){x+=i, s+="\n";}

	int x=37;
	string s="jhon";

	void pr(){printf("%s, %d, \n", s.c_str(), x);}
};

void sendingLol(Message* m){
	printf("\n\n**************\nMsg sent:\nType:%s\nSrc%d\nDst:%d\nArgs:-\n", 
		m->getType().c_str(), m->getSrc(), m->getDst());
	 m->showArgs();
}

int main(){
	PersistantKeyValueStore p(9);
	Messenger m(100, &sendingLol);
	Scheduler s;
	RoutingTable t(100);
	RaftNode  n(100,p,s,m,t);
	cout<<(25789+3334);
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


	/*
	std::string str="GET";
	std:: string haha="haha", hurhur="hurhur", hehe="heehee", hoohoo="hoooohohohohoho", teehee="teehee", naga="", tir="t";
	m.send_message({str,100,200,{{haha,hurhur},{hehe,hoohoo},{teehee,naga},{naga,tir}, {tir,tir}}});

	//std::map<std::string,std::string> map={{haha,hurhur},{hehe,hoohoo}};
	*/


	//cout<<"thingie: "<<fopen("kuchar kew xD.txt", "r");



	void* context=zmq_ctx_new();

	list<gib> lol={1};

	list<gib>* loll=&lol;

//	auto i=lol.cbegin();
	for (auto& i : *loll)
		i.pr();

	cout<<"Empty ?";

	n.step_down(13);

	cout<<"Complt";

	return 0;
}