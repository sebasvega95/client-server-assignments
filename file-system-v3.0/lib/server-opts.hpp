#ifndef SERVER_OPTS_HPP
#define SERVER_OPTS_HPP

#include <string>
#include <regex>
#include <vector>
#include <zmqpp/zmqpp.hpp>
#include "db.hpp"
#include "dispatcher.hpp"
#include "file.hpp"
#include "json.hpp"

using namespace std;
using namespace zmqpp;
using json = nlohmann::json;

void InitUser(string& user, socket &client_socket, socket &broker_socket) {
  json res;

  if (db["users"][user] != nullptr) {
    int err = system(("mkdir fs/" + user).c_str());
    if (err == -1) {
      res["res"] = "Filesystem error";
    } else {
      res["res"] = "OK";
    };
  } else {
    regex r("[a-zA-Z]\\w*");
    if (regex_match(user, r)) {
      db["users"][user] = json::array();
      int err = system(("mkdir fs/" + user).c_str());
      if (err == -1) {
        res["res"] = "Filesystem error";
      } else {
        res["res"] = "OK";
        UpdateDb();
      }
    } else {
      res["res"] = "Invalid name";
    }
  }

  Send(res, client_socket);
}

// TODO Remove this one, it's the broker's job
void ListFiles(string& user, socket &client_socket, socket &broker_socket) {
  json res;

  if (db["users"][user] != nullptr) {
    res["res"] = "OK";
    res["data"] = db["users"][user];
  } else {
    res["res"] = "User does not exist";
  }

  Send(res, client_socket);
}

void SendFileToClient(string &user, string &filename, int cur_pos, socket &client_socket, socket &broker_socket) {
  json res;
  string _filename;

  if (db["users"][user] == nullptr) {
    res["res"] = "User does not exist";
  } else {
    vector<string> _f = db["users"][user];
    if (find(_f.begin(), _f.end(), filename) == _f.end()) {
      res["res"] = "File does not exist";
    } else {
      _filename = "fs/" + user + "/" + filename;
      json file = ReadFileBase64(_filename, cur_pos);
      if (file["error"] != nullptr) {
        res["res"] = file["message"];
      } else {
        res["res"] = "OK";
        res["curPos"] = file["curPos"];
        res["file"] = file["file"];
        res["fileSize"] = FileSize(_filename);
        res["finished"] = file["finished"];
      }
    }
  }

  Send(res, client_socket);
}

void GetFileFromClient(string& user, string& filename, string& file, bool first_time, socket& client_socket, socket &broker_socket) {
  json res;

  if (db["users"][user] == nullptr) {
    res["res"] = "User does not exist";
  } else {
    string _filename = "fs/" + user + "/" + filename;
    bool save = SaveFileBase64(_filename, file, first_time);

    if (!save) {
      res["res"] = "Error writing file";
    } else {
      vector<string> _f = db["users"][user];
      if (find(_f.begin(), _f.end(), filename) == _f.end())
        db["users"][user].push_back(filename);
      res["res"] = "OK";
      UpdateDb();
    }
  }

  Send(res, client_socket);
}

void RemoveFile(string& user, string& filename, socket& client_socket, socket &broker_socket) {
  json res;

  if (db["users"][user] == nullptr) {
    res["res"] = "User does not exist";
  } else {
    vector<string> _f = db["users"][user];
    if (find(_f.begin(), _f.end(), filename) == _f.end()) {
      res["res"] = "File does not exist";
    } else {
      string _filename = "fs/" + user + "/" + filename;
      int removed = remove(_filename.c_str());
      if (removed != 0) {
        res["res"] = "Error removing file!";
      } else {
        db["users"][user].erase(remove(db["users"][user].begin(), db["users"][user].end(), filename), db["users"][user].end());
        UpdateDb();
        res["res"] = "File removed!";
      }
    }
  }

  Send(res, client_socket);
}

#endif
