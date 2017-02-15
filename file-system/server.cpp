#include <bits/stdc++.h>
#include <zmqpp/zmqpp.hpp>
#include "lib/json.hpp"
#include "lib/base64.hpp"
#include "lib/file.hpp"
#include "lib/constants.hpp"

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

void SendFileToClient(string& user, string& filename, socket& s) {
  json res;

  if (db[user] == nullptr) {
    res["res"] = "User does not exist";
  } else {
    vector<string> _f = db[user];
    if (find(_f.begin(), _f.end(), filename) == _f.end()) {
      res["res"] = "File does not exist";
    } else {
      string _filename = "fs/" + user + "/" + filename;
      string file = ReadFileBase64(_filename);
      if (file == FILE_NOT_FOUND) {
        res["res"] = "File does not exist, but it's in DB";
      } else {
        res["res"] = "OK";
        res["file"] = file;
      }
    }
  }

  message ans;
  ans << res.dump();
  s.send(ans);
}

void GetFileFromClient(string& user, string& filename, string& file, socket& s) {
  json res;

  if (db[user] == nullptr) {
    res["res"] = "User does not exist";
  } else {
    string _filename = "fs/" + user + "/" + filename;
    bool save = SaveFileBase64(_filename, file);

    if (!save) {
      res["res"] = "Error writing file";
    } else {
      db[user].push_back(filename);
      res["res"] = "OK";
      UpdateDb();
    }
  }

  message ans;
  ans << res.dump();
  s.send(ans);
}

void RemoveFile(string& user, string& filename ,socket& s) {
  json res;

  if (db[user] == nullptr) {
    res["res"] = "User does not exist";
  } else {
    vector<string> _f = db[user];
    if (find(_f.begin(), _f.end(), filename) == _f.end()) {
      res["res"] = "File does not exist";
    } else {
      string _filename = "fs/" + user + "/" + filename;
      int removed = remove(_filename.c_str());
      if (removed != 0) {
        res["res"] = "Error removing file!";
      } else {
        db[user].erase(remove(db[user].begin(), db[user].end(), filename), db[user].end());
        UpdateDb();
        res["res"] = "File removed!";
      }
    }
  }

  message ans;
  ans << res.dump();
  s.send(ans);
}

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
  cout << "Init DB" << db << endl;

  context ctx;
  socket s(ctx, socket_type::rep);
  s.bind("tcp://*:5555");

  cout << "Waiting for requests" << endl;
  while (true) {
    GetReq(s);
  }

  return 0;
}
