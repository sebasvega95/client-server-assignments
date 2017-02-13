#include <bits/stdc++.h>
#include <zmqpp/zmqpp.hpp>
#include "lib/json.hpp"
#include "lib/base64.hpp"

#define NAME_REQ 0
#define LS_REQ 1
#define GET_REQ 2
#define SEND_REQ 3
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

  if (db[user] != nullptr) {
    res["res"] = "OK";
  } else {
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

void ListFiles(string& user, socket &s) {
  json res;

  if (db[user] != nullptr) {
    res["res"] = "OK";
    res["data"] = db[user];
  } else {
    res["res"] = "User does not exist";
  }

  message ans;
  ans << res.dump();
  s.send(ans);
}

void SendFileToClient(string &user, socket& s) {
  // TODO: Get files from server
}

void GetFileFromClient(string &user, string& filename, string& file, socket& s) {
  json res;

  if (db[user] == nullptr) {
    res["res"] = "User does not exist";
  } else {
    Base64 b64;
    string out;
    b64.Decode(file, &out);

    ofstream fout("fs/" + user + "/" + filename);
    if (!fout) {
      res["res"] = "Error writing file";
    } else {
      fout << out;
      db[user].push_back(filename);
      res["res"] = "OK";
      UpdateDb();
    }
    fout.close();
  }

  message ans;
  ans << res.dump();
  s.send(ans);
}

void Serve(json &req, socket& s) {
  int opt = req["type"];
  string user = req["user"];

  switch (opt) {
    case NAME_REQ:
      InitUser(user, s);
      break;
    case LS_REQ:
      ListFiles(user, s);
      break;
    case GET_REQ:
      break;
    case SEND_REQ: {
      string filename = req["filename"];
      string file = req["file"];
      GetFileFromClient(user, filename, file, s);
      break;
    }
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
  cout << "Received: " << req.dump(2) << endl;

  Serve(req, s);
}

int main(int argc, const char *argv[]) {
  InitDb();
  cout << "Init DB" << db << endl;

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
