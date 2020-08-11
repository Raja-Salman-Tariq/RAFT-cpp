#ifndef handlers_present
    define handlers_present

#include <future>
#include "message_module.h"
#include "utils.h"
#include "node.h"




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

        //our_last_log_index = _our_last_log_index;
        //our_last_log_term = _our_last_log_term;

        /*#---------------------------------------------------------------------------
        # Conditions for Acceptance
        #---------------------------------------------------------------------------
        # If we are out of date, then become follower*/

        if(this.current_term < term)
            this.step_down(term);
    

    /*[ =, *this](us_int & addr, us_int & _our_last_log_index, us_int & _our_last_log_term ) {
        printf("Stuff:%d, %d, %d, \n", addr, _our_last_log_index, this->latest_term);
    }*/
}
/*




    #Condition 1: They are at a higher Term
    c1 = self.current_term <= term
    #Condition 2: We have not voted yet
    c2 = self.voted_for == None
    #Condition 3: Their Log is more up to date
    c3a = last_log_term >= our_last_log_term
    c3b = (last_log_term != our_last_log_term or last_log_index >= our_last_log_index)
    c3 = all([c3a, c3b])
    self.debug_log("C1: ", c1, " | ",
                   "C2: ", c2, " | ",
                   "C3: ", c3, " | ",
                   "C3a: ", c3a, " | ",
                   "C3b: ", c3b)
    accept = all([c1, c2, c3])
    #---------------------------------------------------------------------------
    #We ACCEPT them
    if(accept):
        self.debug_log("RaftNode ", self.addr,
                  ": ACCEPT Vote from ", candidate_id)
        self.voted_for = candidate_id
        self.send_VOTE(candidate_id, True)
        self.set_election_timer()
    #We REJECT them
    else:
        self.debug_log("RaftNode ", self.addr,
                  ": REJECT Vote from ", candidate_id)
        self.send_VOTE(candidate_id, False)
#-------------------------------------------------------------------------------*//*
"""
handle_APPEND_ENTRIES :   Handles a APPEND_ENTRIES message
    Input(s):
        msg     :   APPEND_ENTRIES message
    Output(s):
        None
    Side Effect(s):
        Will Accept or Reject Vote APPEND ENTRIES with a APPEND_ENTRIES_REPLY
        and update the internal state accordingly.
"""
async def handle_APPEND_ENTRIES(self, msg: Message):
    assert(msg.has_args(['term','prev_log_index', 'prev_log_term',
                         'entries', 'leader_commit']))
    self.debug_log("RECV:", msg)
    self.test_log("RECV:", msg)
    src = msg.src
    term = msg.args['term']
    prev_log_index = msg.args['prev_log_index']
    prev_log_term = msg.args['prev_log_term']
    entries = [LogEntry(*e) for e in msg.args['entries']]
    leader_commit = msg.args['leader_commit']
    #--------------------------------------------------------------------------#
    #                           Manage State                                   #
    #--------------------------------------------------------------------------#
    # State Management
    if(self.current_term < term):
        self.step_down(term)
    #If CANDIDATE: Become Follower
    elif(self.role == Role.CANDIDATE):
        self.become_follower()
        self.set_election_timer()
    #If CANDIDATE: Become Follower
    else:
        self.set_election_timer()
    #--------------------------------------------------------------------------#
    #                              Condition(s)                                #
    #--------------------------------------------------------------------------#
    #1. Current Term <= Term of Leader
    c1 = self.current_term <= term
    #2. Logs Matching
    c2a = prev_log_term == None
    c2b = self.log.term_at_index(prev_log_index) == prev_log_term
    c2 = any([c2a, c2b])
    #--------------------------------------------------------------------------#
    #                               Handle REPLICATION                         #
    #--------------------------------------------------------------------------#
    accept = all([c1, c2])
    if(accept):
        self.leader = src
        self.debug_log("RaftNode", self.addr, "ACCEPT APPEND_ENTRIES")
        our_log_index = prev_log_index + 1
        for their_index in range(0, len(entries)):
            #Their Log Data
            their_entry = entries[their_index]
            their_term  = their_entry.term
            #Our Log Data
            our_index   = our_log_index + their_index
            our_term    = self.log.term_at_index(our_index)

            #If Match, continue
            if(our_term == their_term):
                continue
            #If Not Matching, Replace and Append
            else:
                new_entries = entries[their_index:]
                self.log.replace_log_at_and_after_index(our_index, new_entries)
                break
        #UPDATE COMMIT INDEX
        if leader_commit > self.commit_index:
            new_commit = min(leader_commit, self.log.latest_index())
            self.commit_entries(new_commit)
        if(self.pending_transactions != None):
            for transaction in self.pending_transactions:
                transaction.set_result(False)
    else:
        self.debug_log("RaftNode", self.addr, "REJECT APPEND_ENTRIES")
    #--------------------------------------------------------------------------#
    #                           RESPOND TO MESSAGE                             #
    #--------------------------------------------------------------------------#
    self.send_APPEND_ENTRIES_REPLY(src, accept)
#-------------------------------------------------------------------------------
"""
handle_GET :   Handles a GET message from Broker
    Input(s):
        msg     :   GET message
    Output(s):
        None
    Side Effect(s):
        1. This Node will find the responsible Cluster
        2. If this Node is In that Responsible Cluster it will
            a. If Leader Handle it itself
            b. If Follower FORWARD it to Leader
            c. If no Leader in Cluster Fail it
        3. If this Node is NOT in the Responsible Cluster it will:
            a.  FORWARD that Message to the Responsible Cluster and wait for the
                leader to FORWARD_REPLY.
"""
async def handle_GET(self, msg: Message):
    assert(msg.has_args(['id', 'key']))
    self.debug_log("RECV:", msg)
    self.test_log("RECV:", msg)
    req_id = msg.args['id']
    key = msg.args['key']

    #2. Find Responsible Cluster
    router_entry = self.routing_table.find_responsible_cluster(key)
    hash    = router_entry['hash']
    cluster = router_entry['cluster']

    #3. Check if We are Responsible
    responsible = self.routing_table.address_space == hash
    self.debug_log("NODE ", self.addr, " | ",
              "Cluster: ", cluster, "|",
              "Responsible", responsible)
    #---------------------------------------------------------------------------
    # 4a. We are Responsible
    if(responsible):
        response = None
        while(response == None):
            if(self.role == Role.LEADER):
                self.debug_log("RaftNode ", self.addr, " Handling GET")
                asyncio.create_task(self.GET(msg))
                return
            elif(self.leader != None):
                self.debug_log("RaftNode ", self.addr, " Forward to LEADER")
                condition = match_type_src(MessageType.FORWARD_REPLY, self.leader)
                future = self.scheduler.get_message_future(condition)
                self.send_FORWARD(self.leader, msg)
                try:
                    response = await asyncio.wait_for(future, timeout = 5)
                except asyncio.TimeoutError:
                    response = None

                if(response != None):
                    assert(response.has_args(['msg']))
                    self.test_log("RECV:", response)
                    response = response.args['msg']
                    response = Message(*response)
                    self.test_log("SEND:", response)
                    self.messenger.send_message(response)
                    return
            else:
                self.send_GET_RESPONSE_ERROR_TO_BROKER(req_id, key, value)
                return
    #---------------------------------------------------------------------------
    # 4b. We are NOT Responsible
    else:
        self.debug_log("RaftNode ", self.addr, " Sending to Different Cluster")
        self.test_log("Routing to Cluster: ", cluster)
        forwaders = [asyncio.create_task(self.forward_message(node, msg)) for node in cluster]
        responses = asyncio.wait(forwaders, return_when=asyncio.FIRST_COMPLETED)
        (finished, unfinished) = await responses
        for t in forwaders:
            t.cancel()
        return
#-------------------------------------------------------------------------------
"""
handle_SET :   Handles a SET message from Broker
    Input(s):
        msg     :   SET message
    Output(s):
        None
    Side Effect(s):
        1. This Node will find the responsible Cluster
        2. If this Node is In that Responsible Cluster it will
            a. If Leader Handle it itself
            b. If Follower FORWARD it to Leader
            c. If no Leader in Cluster Fail it
        3. If this Node is NOT in the Responsible Cluster it will:
            a.  FORWARD that Message to the Responsible Cluster and wait for the
                leader to FORWARD_REPLY.
"""
async def handle_SET(self, msg: Message):
    assert(msg.has_args(['id', 'key', 'value']))
    self.debug_log("RECV:", msg)
    #1. Unpack Message
    req_id = msg.args['id']
    key = msg.args['key']
    value = msg.args['value']

    #2. Find Responsible Cluster
    router_entry = self.routing_table.find_responsible_cluster(key)
    hash    = router_entry['hash']
    cluster = router_entry['cluster']

    #3. Check if We are Responsible
    responsible = self.routing_table.address_space == hash
    self.debug_log("NODE ", self.addr, " | ",
              "Cluster: ", cluster, "|",
              "Responsible", responsible)
    #---------------------------------------------------------------------------
    # 4a. We are Responsible
    if(responsible):
        response = None
        while(response == None):
            if(self.role == Role.LEADER):
                self.debug_log("RaftNode ", self.addr, " Handling SET")
                asyncio.create_task(self.SET(msg))
                return
            elif(self.leader != None):
                self.debug_log("RaftNode ", self.addr, " Forward to LEADER")
                condition = match_type_src(MessageType.FORWARD_REPLY, self.leader)
                future = self.scheduler.get_message_future(condition)
                self.send_FORWARD(self.leader, msg)
                try:
                    response = await asyncio.wait_for(future, timeout = 5)
                except asyncio.TimeoutError:
                    response = None

                if(response != None):
                    assert(response.has_args(['msg']))
                    self.test_log("RECV: ", response)
                    response = response.args['msg']
                    response = Message(*response)
                    self.test_log("SEND: ", response)
                    self.messenger.send_message(response)
                    return
            else:
                self.send_SET_RESPONSE_ERROR_TO_BROKER(req_id, key, value)
                return
    #---------------------------------------------------------------------------
    # 4b. We are NOT Responsible
    else:
        self.debug_log("RaftNode ", self.addr, " Sending to Different Cluster")
        self.test_log("Routing to Cluster: ", cluster)
        forwaders = [asyncio.create_task(self.forward_message(node, msg)) for node in cluster]
        responses = asyncio.wait(forwaders, return_when=asyncio.FIRST_COMPLETED)
        (finished, unfinished) = await responses
        for t in forwaders:
            t.cancel()
        return
#-------------------------------------------------------------------------------
"""
handle_FORWARD :   Handles a FORWARD message from another Node
    Input(s):
        msg     :   FORWARD message
    Output(s):
        None
    Side Effect(s):
        1. If we are a leader and the Message is a GET or SET, we will handle it
        2. Otherwise we will ignore it
        CROSS NODE TRANSACTIONS EVENTUALLY RESOLVE
"""
async def handle_FORWARD(self, msg : Message):
    self.debug_log("RECV: ", msg)
    self.test_log("RECV: ", msg)
    assert(msg.has_args(['msg']))
    src = msg.src
    msg = msg.args['msg']
    msg = Message(*msg)
    if(self.role == Role.LEADER):
        if(msg.has_type(MessageType.GET)):
            asyncio.create_task(self.GET(msg, direct = False, src = src))
        elif(msg.has_type(MessageType.SET)):
            asyncio.create_task(self.SET(msg, direct = False, src = src))
    return
#-------------------------------------------------------------------------------
"""
forward_message :   Forwards a Message Periodically to a Node
    Input(s):
        msg     :   Message to be sent in the FORWARD message
    Output(s):
        None
    Side Effect(s):
        1.  Every 5 Seconds, send a Forward Message to the Node and wait for a
            response.
"""
async def forward_message(self, dst, msg : Message):
    response = None
    try:
        while(response == None):
            condition = match_type_src(MessageType.FORWARD_REPLY, dst)
            future = self.scheduler.get_message_future(condition)
            self.send_FORWARD(dst, msg)
            try:
                response = await asyncio.wait_for(future, timeout = 5)
            except asyncio.TimeoutError:
                response = None
            self.debug_log("RECIEVED RESPONSE: ", response)
            if(response != None):
                assert(response.has_args(['msg']))
                forwarded_message = response.args['msg']
                forwarded_message = Message(*forwarded_message)
                if(forwarded_message.args['id'] == msg.args['id']):
                    self.test_log("RECV: ", response)
                    self.test_log("SEND: ", forwarded_message)
                    self.messenger.send_message(forwarded_message)
                    return
                response = None
    except CancelledError:
        if(response != None):
            response.cancel()
        self.debug_log("Cancelled Forward: ", dst)
        return
#-------------------------------------------------------------------------------
"""
debug_log :   Logs a message to Debug File for node
    Input(s):
        *args     :   Items to Log
    Output(s):
        None
    Side Effect(s):
        log to screen with CYAN
        log to <node_id>_debug_log.txt
"""
def debug_log(self, *args):
    if debug:
        print(colorama.Fore.CYAN + " ", *args, end= colorama.Style.RESET_ALL +"\n")
        if (debug_to_file):
            fname = '%s/%d/%d_debug_log.txt' % (NODE_DATA, self.addr, self.addr)
            print(*args, file=open(fname, 'a'))
#-------------------------------------------------------------------------------
"""
debug_log :   Logs a message to Test File for node
    Input(s):
        *args     :   Items to Log
    Output(s):
        None
    Side Effect(s):
        log to screen with YELLOW
        log to <node_id>_test_log.txt
"""
def test_log(self, *args):
    if test_to_screen:
        print(colorama.Fore.YELLOW + " ", *args, end= colorama.Style.RESET_ALL +"\n")
    if test_to_file:
        fname = '%s/%d/%d_test_log.txt' % (NODE_DATA, self.addr, self.addr)
        print(*args, file=open(fname, 'a'))
*/
#endif