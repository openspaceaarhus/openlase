#include <zmq.hpp>
#include <string>
#include <iostream>
#include "message.h"

using namespace std;

int main(int argc, char *argv[]) {
  if (argc != 3) {
    cout << "usage ./" << argv[0] << " playerno action" << endl;
    return EXIT_FAILURE;
  }
  //  Prepare our context and socket
  zmq::context_t context (1);
  zmq::socket_t socket (context, ZMQ_REQ);

  int playerno = atoi(argv[1]);
  int action = atoi(argv[2]);;
  
  cout << "Connecting to gameish server server…" << endl;
  socket.connect ("tcp://localhost:5555");
  auto msg  = message { playerno, action};
  
  zmq::message_t request (sizeof(message));
  memcpy ((void *) request.data(), static_cast<void*>(&msg), sizeof(message));
  socket.send (request);
  cout << "sending: " << msg.playerno << " ----- " << msg.action << endl;

  //  Get the reply.
  zmq::message_t reply;
  socket.recv (&reply);
  cout << "Received World " << reply.data() <<  endl;
  return 0;
}
