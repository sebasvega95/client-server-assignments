#ifndef DISPATCHER_HPP
#define DISPATCHER_HPP

#include <zmqpp/zmqpp.hpp>
#include "json.hpp"

using namespace std;
using namespace zmqpp;
using json = nlohmann::json;

void Send(json& msg, socket& s) {
  message ans;
  ans << msg.dump();
  s.send(ans);
}

json Receive(socket &s) {
  message m;
  s.receive(m);
  string msg;
  m >> msg;
  return json::parse(msg);
}

#endif
