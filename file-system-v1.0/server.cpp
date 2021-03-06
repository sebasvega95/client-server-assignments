#include <bits/stdc++.h>
#include <zmqpp/zmqpp.hpp>
#include "lib/json.hpp"
#include "lib/base64.hpp"
#include "lib/constants.hpp"
#include "lib/db.hpp"
#include "lib/server-opts.hpp"

using namespace std;
using namespace zmqpp;
using json = nlohmann::json;

void Serve(json& req, socket& s) {
  int opt = req["type"];
  string user = req["user"];

  switch (opt) {
    case NAME_REQ:
      InitUser(user, s);
      break;
    case LS_REQ:
      ListFiles(user, s);
      break;
    case GET_REQ: {
      string filename = req["filename"];
      SendFileToClient(user, filename, s);
      break;
    }
    case SEND_REQ: {
      string filename = req["filename"];
      string file = req["file"];
      GetFileFromClient(user, filename, file, s);
      break;
    }
    case RM_REQ: {
      string filename = req["filename"];
      RemoveFile(user, filename, s);
      break;
    }
    default:
      break;
  }
}

void GetReq(socket& s) {
  message m;
  s.receive(m);
  string _req;
  m >> _req;
  json req = json::parse(_req);
  cout << "Received request " << req["type"] << endl;

  Serve(req, s);
}

int main(int argc, const char *argv[]) {
  InitDb();
  cout << "Init DB " << db << endl;

  context ctx;
  socket s(ctx, socket_type::rep);
  s.bind("tcp://*:5555");

  cout << "Waiting for requests" << endl;
  while (true) {
    GetReq(s);
  }

  return 0;
}
