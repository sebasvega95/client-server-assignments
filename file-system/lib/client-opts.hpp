#ifndef CLIENT_OPTS_HPP
#define CLIENT_OPTS_HPP

#include <bits/stdc++.h>
#include <zmqpp/zmqpp.hpp>
#include "constants.hpp"
#include "file.hpp"
#include "json.hpp"

using namespace std;
using namespace zmqpp;
using json = nlohmann::json;

void ListFiles(string &user, socket& s) {
  json req;
  req["type"] = LS_REQ;
  req["user"] = user;

  message m;
  m << req.dump();
  s.send(m);

  message ans;
  s.receive(ans);
  string _res;
  ans >> _res;
  json res = json::parse(_res);

  if (res["res"] == "OK") {
    cout << "Your files are:" << endl;
    for (auto &f : res["data"]) {
      cout << f << endl;
    }
  } else {
    cout << res["res"] << endl;
  }
}

void GetFileFromServer(string &user, socket& s) {
  cout << "Enter filename: ";
  string filename;
  cin >> filename;

  json req;
  req["type"] = GET_REQ;
  req["user"] = user;
  req["filename"] = filename;

  message m;
  m << req.dump();
  s.send(m);

  message ans;
  s.receive(ans);
  string _res;
  ans >> _res;
  json res = json::parse(_res);

  if (res["res"] == "OK") {
    string file = res["file"];
    bool save = SaveFileBase64(filename, file);
    if (!save) {
      cout << "Error saving file" << endl;
    } else {
      cout << "File received succesfully" << endl;
    }
  } else {
    cout << res["res"] << endl;
  }
}

void SendFileToServer(string& user, socket& s) {
  cout << "Enter filename: ";
  string filename;
  cin >> filename;

  string file = ReadFileBase64(filename);
  if (file == FILE_NOT_FOUND) {
    cout << "File not found" << endl;
    return;
  }

  json req;
  req["type"] = SEND_REQ;
  req["user"] = user;
  req["filename"] = basename(filename.c_str());
  req["file"] = file;

  message m;
  m << req.dump();
  s.send(m);

  message ans;
  s.receive(ans);
  string _res;
  ans >> _res;
  json res = json::parse(_res);

  if (res["res"] == "OK") {
    cout << "File created!" << endl;
  } else {
    cout << res["res"] << endl;
  }

}

void RemoveFile(string& user, socket& s) {
  cout << "Enter filename: ";
  string filename;
  cin >> filename;

  json req;
  req["type"] = RM_REQ;
  req["user"] = user;
  req["filename"] = filename;

  message m;
  m << req.dump();
  s.send(m);

  message ans;
  s.receive(ans);
  string _res;
  ans >> _res;
  json res = json::parse(_res);

  if (res["res"] == "OK") {
    cout << "File removed!" << endl;
  } else {
    cout << res["res"] << endl;
  }
}

#endif
