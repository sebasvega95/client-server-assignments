#include <iostream>
#include <string>
#include <zmqpp/zmqpp.hpp>
#include "lib/base64.hpp"
#include "lib/constants.hpp"
#include "lib/db.hpp"
#include "lib/file.hpp"
#include "lib/json.hpp"
#include "lib/server-opts.hpp"

using namespace std;
using namespace zmqpp;
using json = nlohmann::json;

void RespondToReq(json& req, socket &client_socket, socket &broker_socket) {
  int opt = req["type"];
  string user = req["user"];

  switch (opt) {
    case NAME_REQ:
      InitUser(user, client_socket, broker_socket);
      break;
    case LS_REQ:
      ListFiles(user, client_socket, broker_socket);
      break;
    case GET_REQ: {
      string filename = req["filename"];
      int cur_pos = req["curPos"];
      SendFileToClient(user, filename, cur_pos, client_socket, broker_socket);
      break;
    }
    case SEND_REQ: {
      string filename = req["filename"];
      string file = req["file"];
      bool first_time = req["firstTime"];
      GetFileFromClient(user, filename, file, first_time, client_socket, broker_socket);
      break;
    }
    case RM_REQ: {
      string filename = req["filename"];
      RemoveFile(user, filename, client_socket, broker_socket);
      break;
    }
    default:
      break;
  }
}

void GetReq(socket &client_socket, socket &broker_socket) {
  message m;
  client_socket.receive(m);
  string _req;
  m >> _req;
  json req = json::parse(_req);
  cout << "Received request " << req["type"] << " from " << req["user"] << endl;

  RespondToReq(req, client_socket, broker_socket);
}

void Serve(socket &broker_socket) {
  context ctx;
  socket client_socket(ctx, socket_type::rep);
  cout << "Waiting for requests" << endl;
  while (true) {
    GetReq(client_socket, broker_socket);
  }
}

void ConnectToBroker(string &dir, socket &broker_socket) {
  json req;
  req["type"] = INIT_SERVER_REQ;
  req["dir"] = dir;
  req["disk"] = GetDiskSpace();
  Send(req, broker_socket);

  json res = Receive(broker_socket);
  if (res["res"] == "OK") {
    cout << "Connected to broker" << endl;
    Serve(broker_socket);
  } else {
    cout << "Couldn't connect to broker" << endl;
    exit(EXIT_FAILURE);
  }
}

int main(int argc, const char *argv[]) {
  if (argc != 3) {
    cout << "Usage: " << argv[0] << " <broker-ip> <server-ip:port>" << endl;
  } else {
    string broker_ip = argv[1], broker_port = "5555", dir = argv[2];
    context ctx;
    socket broker_socket(ctx, socket_type::req);
    broker_socket.connect("tcp://" + broker_ip + ":" + broker_port);

    ConnectToBroker(dir, broker_socket);
  }

  return 0;
}
