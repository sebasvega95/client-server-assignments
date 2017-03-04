#include <iostream>
#include <zmqpp/zmqpp.hpp>
#include "lib/constants.hpp"
#include "lib/db.hpp"
#include "lib/dispatcher.hpp"
#include "lib/file.hpp"
#include "lib/json.hpp"
#include "lib/min-priority-queue.hpp"

using namespace std;
using namespace zmqpp;
using json = nlohmann::json;

MinIndexedPQ server_queue(MAX_NUM_SERVERS);
int num_servers;
json servers;

void ListFiles(string& user, socket &s) {
  json res;

  if (db["users"][user] != nullptr) {
    res["res"] = "OK";
    res["data"] = db["users"][user];
  } else {
    res["res"] = "User does not exist";
  }

  Send(res, s);
}

int GetServerPriority(int load, int disk_space) {
  return (int) (0.5 * load + 0.5 * disk_space);
}

void HandleClient(json& req, socket& s) {
  if (req["type"] == LS_REQ) {
    string user = req["user"];
    ListFiles(user, s);
  } else {

  }
}

void InitServer(json &req, socket &s) {
  string dir = req["dir"];
  int disk_space = req["disk"];
  int priority = GetServerPriority(0, disk_space);
  server_queue.insert(num_servers, priority);
  num_servers++;

  json res;
  res["res"] = "OK";
  Send(res, s);
}

void HandleServer(json& req, socket& s) {
  if (req["type"] == INIT_SERVER_REQ) {
    InitServer(req, s);
    cout << "Server " << req["dir"] << " started" << endl;
  } else { // Update

  }
}

void GetReq(socket& s) {
  json req = Receive(s);
  cout << req.dump(2) << endl;
  if (req["user"] != nullptr) {
    cout << "Received request " << req["type"] << " from user " << req["user"] << endl;
    HandleClient(req, s);
  } else {
    cout << "Received request " << req["type"] << " from server " << req["dir"] << endl;
    HandleServer(req, s);
  }
}

int main(int argc, char *argv[]) {
  InitDb();
  cout << "Init DB " << db << endl;
  num_servers = 0;
  servers = {};

  context ctx;
  socket s(ctx, socket_type::rep);
  s.bind("tcp://*:5555");
  cout << "Init socket" << endl;

  cout << "Waiting for requests" << endl;
  while (true) {
    GetReq(s);
  }
  return 0;
}
