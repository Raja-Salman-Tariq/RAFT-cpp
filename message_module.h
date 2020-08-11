#ifndef message_mod_present // These first 2 and last line at end
#define message_mod_present // of file guard against circular depndncy
#include <iostream>
#include <list>
#include <zmq.h>
#include <map>
#include "node.h"



#define us_int	unsigned short int // we shall use this instead of int, etc.

#define key first  // these are for readability when
#define val second // accessing pairs



class Message {
public:

	// constructor
	Message(std::string _type, us_int _src, us_int _dst, std::map<std::string, std::string> _args):
		type(_type), src(_src), dst(_dst), args(_args) {
	}

	// copy constructor
	Message(const Message & m): type(m.type), src(m.src), dst(m.dst), args(m.args) {
	} // was not mandatory since not handling heap memory


	/* ===========================
		--- MEMBER FUNCTIONS ---
	   ==========================*/

	// Checks if message type matches the given type
	bool has_type(std::string _type) {return !type.compare(_type); }
	
	// Checks if a message has all given arguments
	bool has_args(std::list<std::string> _args) {
		// checks size of given list of arguments against the
		// number of arguments actually in message
		if (_args.size() != args.size())
			return 0;

		// traverse all arguments
		auto i = _args.cbegin(), end = _args.cbegin();
		auto mapEnd = args.end();

		for (; i != end; i++)
			if (mapEnd == args.find(*i)) // argument not found
				return 0;

		return 1; // all args found
	}
	//-----------------------------------------------
	

	// Helper function; converts the entire message to a
	// string for printing, handling, debugging, etc
	std::string& toStr() {

		// to handle multiple calls, i.e
		if (msgAsStr == "") { // if Message does not exist as a string (as in
							  // msgAsStr==""==null string), then convert to string
			auto i = args.cbegin(), end = args.cend();

			std::string s = "";
			s += ("Src | Dst | Type = " + std::to_string(src) + " | " + std::to_string(dst) + " | " + type + "\nArguments (below):\n");
			for (; i != end; i++)
				s += ("Key: " + i->key + ", Val: " + i->val + "\n");

			msgAsStr=s; // save message as string as internal variable
		}

		return msgAsStr; // else if message exists as string, then return it
	}
	//-----------------------------------------------


	/* =============================
		 --- GETTER FUNCTIONS ---
	     	mainly for debugging
	     		unimporant
	   ===========================*/
	const std::string& getType() {cout << "returning type..." << type << endl; return type;}
	const us_int& getSrc() {return src;}
	const us_int& getDst() {return dst;}
	const std::map<std::string, std::string>& getArgs() {return args;}
	const std::string& getVal(std::string key){return (args.find(key))->val;}

	// debugging function to print values to console.
	void showArgs() {
		auto i = args.cbegin(), end = args.cend();

		for (; i != end; i++)
			printf("Key: %s, Val: %s\n", i->key.c_str(), i->val.c_str());
	}


	std::string type;
	us_int src;
	us_int dst;
	std::map<std::string, std::string> args;

	std::string msgAsStr = "";


	// Debugging function; to be ignored/ removed
	/*
	bool isFun(std::string s){
		cout<<"\n IS FUN ! \n"<<s;
		showArgs();
		if (s=="s")
			return 0;
		return 1;
	}*/

};

// Completed work intended to use references rather than cpy
// 	to be ignored
/*
bool match_type(Message &m, std::string type){
	return m.has_type(type);
}
*/


// returns lambda to match type
auto match_type(Message m, std::string type) {
	return [&m, &type](){return m.has_type(type);};
}


// returns lambda to match type and src
auto match_type_src(Message m, std::string _type, us_int src){
	return [&m, &_type, &src](){return (m.has_type(_type)&&(m.src==src));};
}





class Messenger {
public:

	// constructor
	Messenger(us_int _addr, void (* fun_ptr)(Message *)): addr(_addr), send_to_broker(fun_ptr) {}

	void send_message(Message msg) { // send message to scheduler
		send_to_broker(&msg);
	}

private:
	us_int addr;
	void (* send_to_broker)(Message*); // meant to store lambda for scheduler
										// ...scheduler and async not fully implemented...

};
#endif // to prevent circular dependancy