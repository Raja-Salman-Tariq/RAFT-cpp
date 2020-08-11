#ifndef message_mod_present
	#define message_mod_present
#include <iostream>
#include <list>
#include <zmq.h>
#include <map>
#include "node.h"



#define us_int	unsigned short int
#define key first
#define val second




/*
//-------------------------------------------------------------------------------
//
//MessageType :   MessageTypes that we support
//

    #SET/GET
    GET = 'get'
    GET_RESPONSE_TO_BROKER = 'getResponse'

    SET = 'set'
    SET_RESPONSE_TO_BROKER = 'setResponse'

    #VOTING
    REQUEST_VOTE = 'REQUEST_VOTE'
    VOTE = 'VOTE'

    #APPEND ENTRIES
    APPEND_ENTRIES = 'APPEND_ENTRIES'
    APPEND_ENTRIES_REPLY = 'APPEND_ENTRIES_REPLY'

    #FORWARD
    FORWARD = 'FORWARD'
    FORWARD_REPLY = 'FORWARD_REPLY'
#-------------------------------------------------------------------------------
*/

class Message {
public:

	Message(std::string _type, us_int _src, us_int _dst, std::map<std::string, std::string> _args):
		type(_type), src(_src), dst(_dst), args(_args) {
		cout << type << "<- that was type\n";
	}

	Message(const Message & m): type(m.type), src(m.src), dst(m.dst), args(m.args) {
		std::cout << type << "<- that was type cpy cnstr\n";
	} // was not mandatory

	bool has_type(std::string _type) {return !type.compare(_type); }
	bool has_args(std::list<std::string> _args) {
		if (_args.size() != args.size())
			return 0;

		auto i = _args.cbegin(), end = _args.cbegin();
		auto mapEnd = args.end();

		for (; i != end; i++)
			if (mapEnd == args.find(*i))
				return 0;

		return 1;
	}

	const std::string& getType() {cout << "returning type..." << type << endl; return type;}
	const us_int& getSrc() {return src;}
	const us_int& getDst() {return dst;}
	const std::map<std::string, std::string>& getArgs() {return args;}
	const std::string& getVal(std::string key){return (args.find(key))->val;}

	void showArgs() {
		auto i = args.cbegin(), end = args.cend();

		for (; i != end; i++)
			printf("Key: %s, Val: %s\n", i->key.c_str(), i->val.c_str());
	}

	std::string& toStr() {

		if (msgAsStr == "") {

			auto i = args.cbegin(), end = args.cend();

			std::string s = "";
			s += ("Src | Dst | Type = " + std::to_string(src) + " | " + std::to_string(dst) + " | " + type + "\nArguments (below):\n");
			for (; i != end; i++)
				s += ("Key: " + i->key + ", Val: " + i->val + "\n");

		}

		return msgAsStr;
	}


	std::string type;
	us_int src;
	us_int dst;
	std::map<std::string, std::string> args;

	std::string msgAsStr = "";


	bool isFun(std::string s){
		cout<<"\n IS FUN ! \n"<<s;
		showArgs();
		if (s=="s")
			return 0;
		return 1;
	}

};


/*
bool match_type(Message &m, std::string type){
	return m.has_type(type);
}
*/



auto match_timothy(Message m, std::string s){
	return [&m, &s](){return m.isFun(s);};
}

auto match_type(Message m, std::string type) {
	return [&m, &type](){return m.has_type(type);};
}

auto match_type_src(Message m, std::string _type, us_int src){
	return [&m, &_type, &src](){return (m.has_type(_type)&&(m.src==src));};
}
/*
#-------------------------------------------------------------------------------
"""
match_type_src    :   Creates a Lambda for a Message Matching some Type and SRC
Input(s):
    type    :   Type that Message must match
    src     :   Source that the message must match
Output(s):
    def match(msg : Message, src : int):
        return msg.has_type(type) and msg.src == src
"""
def match_type_src(type : MessageType, src : int):
return lambda msg: msg.has_type(type) and msg.src == src
#-------------------------------------------------------------------------------
*/

class Messenger {
public:
	Messenger(us_int _addr, void (* fun_ptr)(Message *)): addr(_addr), send_to_broker(fun_ptr) {}

	void send_message(Message msg) {
		send_to_broker(&msg);
	}

private:
	us_int addr;
	void (* send_to_broker)(Message*);

};
#endif