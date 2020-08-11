#ifndef handlers_present
define handlers_present

#include <future>
#include "message_module.h"
#include "utils.h"
#include "node.h"
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

        // function incomplete...
        // ...scheduler and async not fully implemented...
    
}
#endif