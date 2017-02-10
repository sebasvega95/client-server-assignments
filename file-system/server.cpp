#include <bits/stdc++.h>
#include <zmqpp/zmqpp.hpp>
#include "lib/json.hpp"
#include "lib/base64.hpp"

#define NAME_REQ 0
#define LS_REQ 1
#define RD_REQ 2
#define CR_REQ 3
#define RM_REQ 4

using namespace std;
using namespace zmqpp;
using json = nlohmann::json;

json db;

void InitDb() {
  ifstream fin("db.json");
  fin >> db;
  fin.close();
}

void UpdateDb() {
  ofstream fout("db.json");
  fout << setw(2) << db << endl;
  fout.close();
}

void InitUser(string& user, socket &s) {
  json res;
  cout << res.dump(2) << endl;

  if (db[user]) {
    res["res"] = "OK";
  } else {
    string user;
    regex r("[a-zA-Z]\\w*");
    if (regex_match(user, r)) {
      db[user] = json::array();
      system(("mkdir fs/" + user).c_str());
      res["res"] = "OK";
      UpdateDb();
    } else {
      res["res"] = "Invalid name";
    }
  }

  message ans;
  ans << res.dump();
  s.send(ans);
}

void Serve(int opt, string& user, socket& s) {
  switch (opt) {
    case NAME_REQ:
      InitUser(user, s);
      break;
    case LS_REQ:
      break;
    case RD_REQ:
      break;
    case CR_REQ:
      break;
    case RM_REQ:
     break;
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
  cout << req.dump(2) << endl;

  int opt = req["type"];
  string user = req["user"];
  Serve(opt, user, s);
}

int main(int argc, const char *argv[]) {
  cout << "Init DB" << endl;
  InitDb();

  context ctx;
  socket s(ctx, socket_type::rep);
  s.bind("tcp://*:5555");

  cout << "Waiting for requests" << endl;
  while (true) {
    GetReq(s);
  }

  // cout << "Waiting for message..." << endl;
  // message m;
  // s.receive(m);
  // cout << "Message received!" << endl;
  //
  // string text;
  // m >> text;
  // auto r = json::parse(text);
  //
  // Base64 b64;
  // string in = r["file"], out;
  // b64.Decode(in, &out);
  //
  // string filename = r["name"];
  // ofstream fout("fs/" + filename);
  // fout << out;
  // fout.close();
  //
  // cout << "File written!" << endl;

  return 0;
}
