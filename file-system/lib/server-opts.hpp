#ifndef SERVER_OPTS_HPP
#define SERVER_OPTS_HPP

#include <bits/stdc++.h>
#include <zmqpp/zmqpp.hpp>
#include "db.hpp"
#include "file.hpp"
#include "json.hpp"

using namespace std;
using namespace zmqpp;
using json = nlohmann::json;

void Send(json& res, socket& s) {
  message ans;
  ans << res.dump();
  s.send(ans);
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

  Send(res, s);
}

void ListFiles(string& user, socket &s) {
  json res;

  if (db[user] != nullptr) {
    res["res"] = "OK";
    res["data"] = db[user];
  } else {
    res["res"] = "User does not exist";
  }

  Send(res, s);
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

  Send(res, s);
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

  Send(res, s);
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

  Send(res, s);
}

#endif
