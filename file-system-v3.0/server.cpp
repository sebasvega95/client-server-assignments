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

void UpdateServerPriority(socket &broker_socket) {
  json req;
  req["type"] = "update";
  req["load"] = cur_load;
  req["disk"] = GetDiskSpace();
  Send(req, broker_socket);

  json res = Receive(broker_socket);
  if (res["res"] == "OK") {
    cout << "Broker state updated" << endl;
  }
}

void RespondToReq(json& req, socket &client_socket, socket &broker_socket) {
  int opt = req["type"];
  string user = req["user"];

  switch (opt) {
    case NAME_REQ:
      InitUser(user, client_socket, broker_socket);
      break;
    // case LS_REQ:
    //   ListFiles(user, client_socket, broker_socket);
    //   break;
    case GET_REQ: { // Client wants to get a file
      string filename = req["filename"];
      int cur_pos = req["curPos"];
      int file_size = req["fileSize"];
      cur_load += file_size;
      UpdateServerPriority(broker_socket);
      SendFileToClient(user, filename, cur_pos, client_socket, broker_socket);
      cur_load -= file_size;
      UpdateServerPriority(broker_socket);
      break;
    }
    case SEND_REQ: { // Client sends a file
      string filename = req["filename"];
      string file = req["file"];
      bool first_time = req["firstTime"];
      int file_size = GetFileSize(filename);
      cur_load += file_size;
      UpdateServerPriority(broker_socket);
      GetFileFromClient(user, filename, file, first_time, client_socket, broker_socket);
      cur_load -= file_size;
      UpdateServerPriority(broker_socket);
      break;
    }
    case RM_REQ: { // Client removes a file
      string filename = req["filename"];
      RemoveFile(user, filename, client_socket, broker_socket);
      UpdateServerPriority(broker_socket);
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

void Serve(string &port, socket &broker_socket) {
  cur_load = 0;
  context ctx;
  socket client_socket(ctx, socket_type::rep);
  client_socket.bind("tcp://*:" + port);
  cout << "Waiting for requests" << endl;

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
    Serve(port, broker_socket);
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
