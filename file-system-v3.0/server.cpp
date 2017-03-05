#include <iostream>
#include <string>
#include <zmqpp/zmqpp.hpp>
#include "lib/base64.hpp"
#include "lib/constants.hpp"
#include "lib/file.hpp"
#include "lib/json.hpp"
#include "lib/server-opts.hpp"

using namespace std;
using namespace zmqpp;
using json = nlohmann::json;

void UpdateServerPriority(socket &broker_socket) {
  json req;
  req["type"] = UPDATE_SERVER_REQ;
  req["dir"] = dir;
  req["load"] = cur_load;
  req["disk"] = GetDiskSpace();
  Send(req, broker_socket);
  json res = Receive(broker_socket);
  if (res["res"] == "OK") {
    cout << "Broker state updated with load=" << cur_load << " and disk=" << req["disk"] << endl;
  } else {
    cout << res["res"] << endl;
  }
}

void RespondToReq(json& req, socket &client_socket, socket &broker_socket) {
  int opt = req["type"];
  string user = req["user"];
  string filename = req["filename"];
  switch (opt) {
    case GET_REQ: { // Client wants to get a file
      int cur_pos = req["curPos"];
      size_t file_size = GetFileSize(filename);
      cur_load += file_size;
      UpdateServerPriority(broker_socket);
      SendFileToClient(user, filename, cur_pos, client_socket, broker_socket);
      cur_load -= file_size;
      UpdateServerPriority(broker_socket);
      break;
    }
    case SEND_REQ: { // Client sends a file
      string file = req["file"];
      bool first_time = req["firstTime"];
      size_t file_size = req["filesize"];
      cur_load += file_size;
      UpdateServerPriority(broker_socket);
      GetFileFromClient(user, filename, file, first_time, client_socket, broker_socket);
      cur_load -= file_size;
      UpdateServerPriority(broker_socket);
      break;
    }
    case RM_REQ: { // Client removes a file
      RemoveFile(user, filename, client_socket, broker_socket);
      UpdateServerPriority(broker_socket);
      break;
    }
    default:
      break;
  }
}

void GetReq(socket &client_socket, socket &broker_socket) {
  json req = Receive(client_socket);
  cout << "Received request " << req["type"] << " from " << req["user"] << endl;

  RespondToReq(req, client_socket, broker_socket);
}

void Serve(string &ip, string &port, socket &broker_socket) {
  cur_load = 0;
  dir = ip + ":" + port;
  context ctx;
  socket client_socket(ctx, socket_type::rep);
  client_socket.bind("tcp://*:" + port);
  cout << "Waiting for clint requests" << endl;

  while (true) {
    GetReq(client_socket, broker_socket);
  }
}

void ConnectToBroker(string &ip, string &port, socket &broker_socket) {
  json req;
  req["type"] = INIT_SERVER_REQ;
  req["dir"] = ip + ":" + port;
  req["disk"] = GetDiskSpace();
  Send(req, broker_socket);

  json res = Receive(broker_socket);
  if (res["res"] == "OK") {
    cout << "Connected to broker" << endl;
    Serve(ip, port, broker_socket);
  } else {
    cout << "Couldn't connect to broker" << endl;
    exit(EXIT_FAILURE);
  }
}

int main(int argc, const char *argv[]) {
  if (argc != 4) {
    cout << "Usage: " << argv[0] << " <broker-ip> <server-ip> <server-port>" << endl;
  } else {
    string broker_ip = argv[1], broker_port = "5555";
    string ip = argv[2], port = argv[3];
    context ctx;
    socket broker_socket(ctx, socket_type::req);
    broker_socket.connect("tcp://" + broker_ip + ":" + broker_port);

    ConnectToBroker(ip, port, broker_socket);
  }

  return 0;
}
