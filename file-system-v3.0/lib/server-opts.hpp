#ifndef SERVER_OPTS_HPP
#define SERVER_OPTS_HPP

#include <string>
#include <zmqpp/zmqpp.hpp>
#include "dispatcher.hpp"
#include "file.hpp"
#include "json.hpp"

using namespace std;
using namespace zmqpp;
using json = nlohmann::json;

size_t cur_load; // Current load for this server
string dir;

void SendFileToClient(string &user, string &filename, int cur_pos, socket &client_socket, socket &broker_socket) {
  json res;
  string _filename;

  _filename = "fs/" + user + "/" + filename;
  json file = ReadFileBase64(_filename, cur_pos);
  if (file["error"] != nullptr) {
    res["res"] = file["message"];
  } else {
    res["res"] = "OK";
    res["curPos"] = file["curPos"];
    res["file"] = file["file"];
    res["fileSize"] = GetFileSize(_filename);
    res["finished"] = file["finished"];
  }
  Send(res, client_socket);
}

void GetFileFromClient(string& user, string& filename, string& file, bool first_time, socket& client_socket, socket &broker_socket) {
  json res;
  string _filename = "fs/" + user + "/" + filename;
  system(("mkdir fs/" + user).c_str());
  bool save = SaveFileBase64(_filename, file, first_time);

  if (!save) {
    res["res"] = "Error writing file";
  } else {
    res["res"] = "OK";
  }

  Send(res, client_socket);
}

void RemoveFile(string& user, string& filename, socket& client_socket, socket &broker_socket) {
  json res;
  string _filename = "fs/" + user + "/" + filename;
  int removed = remove(_filename.c_str());
  if (removed != 0) {
    res["res"] = "Error removing file!";
  } else {
    res["res"] = "OK";
  }

  Send(res, client_socket);
}

#endif
