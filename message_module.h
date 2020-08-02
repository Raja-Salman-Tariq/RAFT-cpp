#include <iostream>
#include <list>
#include <zmq.h>
#include <map>

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

class Message{
public:

	Message(std::string _type, us_int _src, us_int _dst, std::map<std::string, std::string> _args):
					type(_type), src(_src), dst(_dst), args(_args){}

	Message(const Message & m):type(m.type), src(m.src), dst(m.dst), args(m.args){} // was not mandatory

	bool has_type(std::string _type){return !type.compare(_type); }
	bool has_args(std::list<std::string> _args){
		if (_args.size()!=args.size())
			return 0;

		auto i=_args.cbegin(), end=_args.cbegin();
		auto mapEnd=args.end();

		for (; i!=end; i++)
			if (mapEnd==args.find(*i))
				return 0;

		return 1;
	}

	const std::string& getType() {return type;}
	const us_int& getSrc() {return src;}
	const us_int& getDst() {return dst;}
	const std::map<std::string, std::string>& getArgs(){return args;}

	void showArgs(){
		auto i=args.cbegin(), end=args.cend();

		for(; i!=end; i++)
			printf("Key: %s, Val: %s\n", i->key, i->val);
	}

private:
	std::string type;
	us_int src;
	us_int dst;
	std::map<std::string, std::string> args;

};

/*
"""
match_type    :   Creates a Lambda for a Message Matching some Type
    Input(s):
        type    :   Type that Message must match
    Output(s):
        def match(msg : Message):
            return msg.has_type(type)
"""
def match_type(type : MessageType):
	return lambda msg: msg.has_type(type)
	*/
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

class Messenger{
public:
	Messenger(us_int _addr, void (* fun_ptr)(Message *)): addr(_addr), send_to_broker(fun_ptr){}

	void send_message(Message msg){
		send_to_broker(&msg);
	}

private:
	us_int addr;
	void (* send_to_broker)(Message*);

};