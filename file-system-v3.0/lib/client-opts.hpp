#ifndef CLIENT_OPTS_HPP
#define CLIENT_OPTS_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <libgen.h>
#include <regex>
#include <zmqpp/zmqpp.hpp>
#include "constants.hpp"
#include "dispatcher.hpp"
#include "file.hpp"
#include "json.hpp"

using namespace std;
using namespace zmqpp;
using json = nlohmann::json;


bool ValidateCredentials(string &user, string &passwd, int type, socket &broker_socket) {
  json req;
  req["type"] = type;
  req["user"] = user;
  req["password"] = passwd;

  Send(req, broker_socket);

  cout << "Waiting authentication" << endl;
  json res = Receive(broker_socket);
  if (res["res"] != "OK") {
    cout << res["res"] << endl;
  }
  return res["res"] == "OK";
}

void ListFiles(string &user, socket &s) {
  json req;
  req["type"] = LS_REQ;
  req["user"] = user;

  Send(req, s);

  json res = Receive(s);

  if (res["res"] == "OK") {
    cout << "Your files are:" << endl;
    for (json::iterator it = res["data"].begin(); it != res["data"].end(); ++it) {
      cout << "* " << it.key() << endl;
    }
  } else {
    cout << res["res"] << endl;
  }
}

void GetFileFromServer(string &user, string &filename, socket &s) {
  bool first_time = true, finished;
  int cur_pos = 0;
  do {
    json req;
    req["type"] = GET_REQ;
    req["user"] = user;
    req["filename"] = filename;
    req["curPos"] = cur_pos;

    Send(req, s);

    json res = Receive(s);

    if (res["res"] == "OK") {
      string file = res["file"];
      bool save = SaveFileBase64(filename, file, first_time);
      first_time = false;
      finished = res["finished"];
      cur_pos = res["curPos"];
      if (!save) {
        cout << "Error saving file" << endl;
      } else {
        cout << "Downloading... " << cur_pos / (double) res["fileSize"] * 100 << "%" << endl;
      }
    } else {
      cout << res["res"] << endl;
      break;
    }
  } while (!finished);
}

void SendFileToServer(string &user, string &filename, socket &s) {
  string server_response = "File created!";
  int cur_pos = 0;
  size_t file_size = GetFileSize(filename);
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
    char _fn[256];
    strcpy(_fn, filename.c_str());
    req["filename"] = basename(_fn);
    req["file"] = file;
    req["firstTime"] = (cur_pos == 0);
    req["filesize"] = file_size;
    cur_pos = open_file["curPos"];
    finished = open_file["finished"];

    Send(req, s);

    json res = Receive(s);

    if (res["res"] != "OK") {
      server_response = res["res"];
      break;
    } else {
      cout << "Uploading... " << (double) cur_pos / file_size * 100 << "%" << endl;
    }
  } while (!finished);

  cout << server_response << endl;
}

void RemoveFile(string &user, string &filename, socket &s) {
  json req;
  req["type"] = RM_REQ;
  req["user"] = user;
  req["filename"] = filename;

  Send(req, s);

  json res = Receive(s);

  if (res["res"] == "OK") {
    cout << "File removed!" << endl;
  } else {
    cout << res["res"] << endl;
  }
}

#endif
