#include <assert.h>
#include "node.h"


class NetworkNode {

	NetworkNode(auto node_name, auto router_ep, auto peer_names, auto debug, string pub_endpoint) {
		//=================================
		// 		    ---- ZMQ ----
		//=================================

		// create context
		void* context = zmq_ctx_new();
		bool connected = false;

		// RECEIVE SOCKET: subscribe for messages

		void* sub_sock = zmq_socket(context, ZMQ_SUB);
		assert(sub_sock);
		bool not_connected = zmq_connect(sub_sock, pub_endpoint.c_str());
		assert(!not_connected);

		zmq_setsockopt(sub_sock, ZMQ_SUBSCRIBE,node_name.c_str, node_name.length());
	

		// Create handler for SUB socket
		void* sub=zmq_socket(context, ZMQ_STREAM);
		

		//on_recv(sub, handle);
	}

	void handle(){cout<<"ok";}
};
/*
    """
    handle :   Handles a Message sent to this node
        Input(s):
            msg_frames :   Message Frames
        Ouput(s):
            None
        Side Effect(s):
            Delivers Message to the RaftNode
    """
    def handle(self, msg_frames):
        # in the event of a mismatch, format a nice string with msg_frames in
        # the raw, for debug purposes
        assert len(msg_frames) == 3, ((
            "Multipart ZMQ message had wrong length. "
            "Full message contents:\n{}").format(msg_frames))
        #-----------------------------------------------------------------------
        #1. Unpack Message Frame
        if (msg_frames[0].decode('ascii') != self.name):
            return
        assert msg_frames[0].decode('ascii') == self.name
        # Second field is the empty delimiter
        msg = json.loads(msg_frames[2])
        self.log_debug("Received " + str(msg_frames))

        #-----------------------------------------------------------------------
        #2a. Proccess Startup Hello Message
        if msg['type'] == 'hello':
            if not self.connected:
                # Send helloResponse
                self.connected = True
                self.send_to_broker({'type': 'helloResponse', 'source': self.name})
                self.log("Node is running")
                # Run startup processes
                asyncio.create_task(self.raft_node.startup())
        #-----------------------------------------------------------------------
        #2a. Proccess Actual Messages
        else:
            src = msg.pop('src', '-1')
            dst = msg.pop('destination', '-1')
            message_type = msg.pop('type', None)
            debug_log(message_type," SRC: ",src," DST:",dst)
            assert message_type != None
            self.async_scheduler.deliver_message(Message(message_type, int(src), int(dst), msg))
    #---------------------------------------------------------------------------
    """*/