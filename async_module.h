#ifndef async_present
#define async_present
#include <list>
#include "message_module.h"
#include <future>

struct Job {
	Job(bool (* _activCond)(Message&), void* _future = 0):
		activation_condition(_activCond), future(_future) {	}


	bool (*activation_condition)(Message&);
	void* future;

	bool operator == (const Job& j) {return (future == j.future);}
};


struct Handler {
	static us_int u_id;

	Handler(bool (* _activCond)(Message&), void (* _handleMeth)(void *)):
		activation_condition(_activCond), handle_method(_handleMeth)
	{
		u_id++; /*unique identifier*/
		c_id = u_id; //current u_id;
	}

	bool (*activation_condition)(Message&);
	void (* handle_method)(void*);
	us_int c_id;
};

us_int Handler:: u_id = 0;

class Scheduler {
public:
	Scheduler(list<Job>* _jobs = 0, list<Handler>* _handlers = 0) {

		if (_jobs)
			for (auto& j : *_jobs)
				jobs.emplace(j.future, j);

		if (_handlers)
			for (auto & h : *_handlers)
				handlers.emplace(h.c_id, h);

	}

	std::map<void*, promise<bool> *> job_promises;
	//list<std::promise<bool>> po;

	std::map<void*, Job> jobs;
	std::map<us_int, Handler> handlers;


	void deliver_message(Message m) {

		for (auto& j : jobs) {
			if (j.second.activation_condition(m)) {
				if (j.second.future) {
					try {
						job_promises.at(j.first)->set_value(1); // should have maps of promises for jobs and hands
					}
					catch (std::out_of_range& o){
						printf("Error : In call to deliver_message, async_module.h ; promise not made for job. \n");
					}
					jobs.erase(j.first);
				}
			}

		}

		for (auto& h : handlers) {
			if (h.second.activation_condition(m))
				std::async(std::launch::async, ((h.second.handle_method)), (void*)&m); // save the future ??
		}
	}


	void* get_message_future(bool (*activation_condition)(Message&)){
		Job j(activation_condition);
		register_job(j);
	}

	void register_job(Job& j) {
//		std::promise<bool> p;

		jobs.emplace(j.future, j);
//		job_promises.emplace(j.future, new promise<bool>);	// ******** DELETE THIS *************
	}


	void register_handler(Handler h) {
		handlers.emplace(h.c_id, h);
	}

	friend class RaftNode;
}; 
#endif