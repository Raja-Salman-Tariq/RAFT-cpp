#ifndef handlers_present
#define handlers_present

#include <future>
#include "message_module.h"
#include "utils.h"

// ...scheduler and async not fully implemented...
// file incomplete...




/*################################################################################
#                                HANDLER FUNCTIONS                             #
################################################################################
"""
handle_REQUEST_VOTE :   Handles a REQUEST_VOTE message
    Input(s):
        msg     :   REQUEST_VOTE message
    Output(s):
        None
    Side Effect(s):
        Will Accept or Reject the REQUEST_VOTE with a VOTE and update the
        internal state accordingly.
"""*/
RaftNode r;
void handle_REQUEST_VOTE(Message m, RaftNode& this) {
    
        std::list<std::string> _args={"term", "last_log_index", "last_log_term"};
        if (!m.has_args(_args)){
            cout<<"Error : From call to handle_REQUEST_VOTE() in handlers.h module; Message does not have required arguments.\n";
            return;
        }

        debug_log("RECV: \n"+m.toStr(), this.addr);
        test_log("RECV: \n"+m.toStr(), this.addr);

        // # UNPACK MSG

        us_int candidate_id=m.getSrc(),
        term=stoi(m.getVal("term")),
        last_log_index=stoi(m.getVal("last_log_index")),
        last_log_term=stoi(m.getVal("last_log_term"));

        // the following 2 lines are commented and differ from py code
        // we need not copy them since we can directly access private data
        // members due to the friend keyword in RaftNode class

        /*our_last_log_index = _our_last_log_index;
          our_last_log_term = _our_last_log_term;*/

        /*#---------------------------------------------------------------------------
        # Conditions for Acceptance
        #---------------------------------------------------------------------------
        # If we are out of date, then become follower*/

        if(this.current_term < term)
            this.step_down(term);


        // C1: They are at a higher term
        bool c1= this.current_term<= term,

        // C2: We have not voted yet
        c2= this.voted_for==0,

        // C3: Their log is more up to date
        c3a= last_log_term >= this.last_log_term(),
        c3b= ((last_log_term != this.last_log_term()) || (last_log_index >= this.last_log_index())),
        c3 = (c3a && c3b);

        debug_log("C1: "+to_string(c1)+" | "+
                  "C2: "+to_string(c2)+" | "+
                  "C3: "+to_string(c3)+" | "+
                  "C3a: "+to_string(c3a)+" | "
                  "C3b: "+to_string(c3b)
            ,addr);

        if (c1 && c2 && c3){
            debug_log("RaftNode "+to_string(addr)+": ACCEPT Vote from "+to_string(candidate_id)
                ,addr)

        voted_for=candidate_id;
        send_VOTE(candidate_id, true);
        set_election_timer();
        }   

        else{
            debug_log("RaftNode "+toStr(addr)+": REJECT Vote from "+to_string(candidate_id)
                , addr);
            send_VOTE(candidate_id, false);
        }
    
}

void send_VOTE(us_int dst, bool accept){
/*    """
    Tell <dst> that I <accept>
    """
    #self.debug_log("Raft", self.addr, "VOTE for", dst)*/

    cout<<"***********************************\n"
        <<"NEEDS TO BE IMPLEMENTED (send_VOTE() func)\n"
        <<"***********************************";

}



#endif