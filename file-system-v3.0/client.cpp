#include <bits/stdc++.h>
#include <zmqpp/zmqpp.hpp>
#include "lib/client-opts.hpp"
#include "lib/constants.hpp"
#include "lib/json.hpp"

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
  if (argc != 2) {
    cout << "Usage: " << argv[0] << " <server-ip>" << endl;
  } else {
    string ip = argv[1], port = "5555";
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
  }

  return 0;
}
