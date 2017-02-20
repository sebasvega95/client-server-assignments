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

  bool first_time = true, finished;
  int cur_pos = 0;
  do {
    json req;
    req["type"] = GET_REQ;
    req["user"] = user;
    req["filename"] = filename;
    req["curPos"] = cur_pos;

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
      bool save = SaveFileBase64(filename, file, first_time);
      first_time = false;
      finished = res["finished"];
      cur_pos = res["curPos"];
      cout << "Downloading... " << cur_pos / (double) res["fileSize"] << endl;
      if (!save) {
        cout << "Error saving file" << endl;
      } else {
        cout << "File received succesfully" << endl;
      }
    } else {
      cout << res["res"] << endl;
      break;
    }
  } while (!finished);
}

void SendFileToServer(string& user, socket& s) {
  cout << "Enter filename: ";
  string filename;
  cin >> filename;

  string server_response = "File created!";
  int cur_pos = 0, file_size = FileSize(filename);
  bool finished;
  do {
    json open_file = ReadFileBase64(filename, cur_pos);
    if (open_file["error"] != nullptr) {
      cout << open_file["message"] << endl;
      return;
    }
    string file = open_file["file"];

    json req;
    req["type"] = SEND_REQ;
    req["user"] = user;
    req["filename"] = basename(filename.c_str());
    req["file"] = file;
    req["firstTime"] = (cur_pos == 0);

    cur_pos = open_file["curPos"];
    finished = open_file["finished"];

    message m;
    m << req.dump();
    s.send(m);
    message ans;
    s.receive(ans);
    string _res;
    ans >> _res;
    json res = json::parse(_res);

    if (res["res"] != "OK") {
      server_response = res["res"];
      break;
    } else {
      cout << "Uploading... " << (double) cur_pos / file_size << endl;
    }
  } while (!finished);

  cout << server_response << endl;
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
