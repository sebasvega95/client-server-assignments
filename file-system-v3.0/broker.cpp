#include <iostream>
#include <regex>
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
json servers_index, servers_dir;

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

size_t GetServerPriority(size_t load, size_t disk_space) {
  return (size_t) (0.5 * load + 0.5 * disk_space);
}

void InitUser(string& user, socket &s) {
  json res;

  if (db["users"][user] != nullptr) {
    system(("mkdir fs/" + user).c_str());
    res["res"] = "OK";
  } else {
    regex r("[a-zA-Z]\\w*");
    if (regex_match(user, r)) {
      db["users"][user] = {};
      system(("mkdir fs/" + user).c_str());
      res["res"] = "OK";
      UpdateDb();
    } else {
      res["res"] = "Invalid name";
    }
  }

  Send(res, s);
}

void HandleClient(json& req, socket& s) {
  string user = req["user"];
  if (req["type"] == LS_REQ) {
    ListFiles(user, s);
  } else if (req["type"] == SEND_REQ) {
    int best_index = server_queue.minIndex();
    string server = servers_dir[best_index];
    json res;
    res["res"] = "OK";
    res["server"] = server;
    Send(res, s);
  } else if (req["type"] == NAME_REQ) {
    InitUser(user, s);
  } else {
    string filename = req["filename"];
    json res;
    if (db["users"][user][filename] != nullptr) {
      string server = db["users"][user][filename];
      res["res"] = "OK";
      res["server"] = server;
    } else {
      res["res"] = "Couldn't locate server";
    }
    Send(res, s);
  }
}

void InitServer(string &server, size_t disk_space, socket &s) {
  int priority = GetServerPriority(0, disk_space);
  server_queue.insert(num_servers, priority);
  servers_index[server] = num_servers;
  servers_dir[num_servers] = server;
  cout << "Init server " << server << " with idx " << num_servers << endl;
  num_servers++;

  json res;
  res["res"] = "OK";
  Send(res, s);
}

void UpdateServerPriority(string &server, size_t load, size_t disk_space) {
  size_t priority = GetServerPriority(load, disk_space);
  server_queue.changeKey(servers_index[server], priority);
}

void HandleServer(json& req, socket& s) {
  if (req["type"] == INIT_SERVER_REQ) {
    string server = req["dir"];
    size_t disk_space = req["disk"];
    InitServer(server, disk_space, s);
  } else { // UPDATE_SERVER_REQ
    json req = Receive(s);
    string server = req["dir"];
    size_t load = req["load"];
    size_t disk_space = req["disk"];
    UpdateServerPriority(server, load, disk_space);
  }
}

void GetReq(socket& s) {
  json req = Receive(s);
  cout << req.dump(2) << endl;
  if (req["user"] != nullptr) { // It's a client
    cout << "Received request " << req["type"] << " from user " << req["user"] << endl;
    HandleClient(req, s);
  } else { // req["dir"] so it's a server
    cout << "Received request " << req["type"] << " from server " << req["dir"] << endl;
    HandleServer(req, s);
  }
}

int main(int argc, char *argv[]) {
  InitDb();
  cout << "Init DB " << db << endl;
  num_servers = 0;
  servers_index = {};
  servers_dir = {};

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
