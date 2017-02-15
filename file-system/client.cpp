#include <bits/stdc++.h>
#include <zmqpp/zmqpp.hpp>
#include "lib/json.hpp"
#include "lib/file.hpp"
#include "lib/constants.hpp"

using namespace std;
using namespace zmqpp;
using json = nlohmann::json;

string GetName(socket& s) {
  regex r("[a-zA-Z]\\w*");
  string name;

  bool ok;
  do {
    cout << "Username: ";
    getline(cin, name);
    json req;
    req["type"] = NAME_REQ;
    req["user"] = name;

    message m;
    m << req.dump();
    s.send(m);

    cout << "Waiting response" << endl;
    message ans;
    s.receive(ans);
    string _res;
    ans >> _res;
    json res = json::parse(_res);

    ok = res["res"] == "OK";
    if (!ok) {
      cout << "Invalid name" << endl << endl;
    }
  } while(!ok);

  return name;
}

int GetOption(string& opt) {
  stringstream ss(opt);
  int o;
  ss >> o;

  if (!ss)
    return -1;
  return o;
}

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

  // Pause();
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

bool ExecuteOpt(int opt, string &user, socket& s) {
  switch (opt) {
    case LS_REQ:
      ListFiles(user, s);
      break;
    case GET_REQ:
      GetFileFromServer(user, s);
      break;
    case SEND_REQ:
      SendFileToServer(user, s);
      break;
    case RM_REQ:
      RemoveFile(user, s);
      break;
    case 0:
      cout << endl << "Bye bye!!" << endl;
      return false;
      break;
    default:
      cout << "Invalid option" << endl;
      break;
  }

  return true;
}

bool PrintMenu(string &user, socket& s) {
  cout << endl << endl;
  cout << "Please enter your choice :)" << endl;
  cout << "1. List your files" << endl;
  cout << "2. Get a file" << endl;
  cout << "3. Upload a file" << endl;
  cout << "4. Delete a file" << endl;
  cout << "0. Exit" << endl;
  cout << "> ";

  string _opt;
  cin >> _opt;
  int opt = GetOption(_opt);
  cout << endl;
  return ExecuteOpt(opt, user, s);
}

int main(int argc, const char *argv[]) {
  string ip = "localhost", port = "5555";
  context ctx;
  socket s(ctx, socket_type::req);
  s.connect("tcp://" + ip + ":" + port);

  string user = GetName(s);

  cout << endl;
  cout << "Welcome " << user << " to SuperFancy FileSystem (SFFS)" << endl;
  bool cont;
  do {
    cont = PrintMenu(user, s);
  } while(cont);

  return 0;
}
