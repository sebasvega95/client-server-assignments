#include <bits/stdc++.h>
#include <zmqpp/zmqpp.hpp>
#include "lib/json.hpp"
#include "lib/base64.hpp"

#define NAME_REQ 0
#define LS_REQ 1
#define RD_REQ 2
#define CR_REQ 3
#define RM_REQ 4

using namespace std;
using namespace zmqpp;
using json = nlohmann::json;

void Pause() {
  cout << "Press any key to continue...";
  cin.ignore().get();
}

string GetName(socket& s) {
  regex r("[a-zA-Z]\\w*");
  string name;

  bool ok;
  do {
    system("clear");
    cout << "Username: ";
    getline(cin, name);
    json req;
    req["type"] = NAME_REQ;
    req["user"] = name;

    message m;
    m << req.dump();
    s.send(m);

    message ans;
    s.receive(ans);
    string _res;
    ans >> _res;
    json res = json::parse(_res);
    cout << res.dump(2) << endl;
    ok = res["res"] == "OK";
    if (!ok) {
      cout << "Invalid name" << endl;
      Pause();
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
  json load;
  load["type"] = LS_REQ;
  load["user"] = user;

  message m;
  m << load.dump();
  s.send(m);

  message ans;
  s.receive(ans);
  string _res;
  ans >> _res;
  json res = json::parse(_res);

  for (auto &f : res["data"]) {
    cout << f << endl;
  }
}

bool ExecuteOpt(int opt, string &user, socket& s) {
  switch (opt) {
    case LS_REQ:
      // ListFiles(user, s);
      break;
    case RD_REQ:
      break;
    case CR_REQ:
      break;
    case RM_REQ:
     break;
    case 0:
      return false;
      break;
    default:
      cout << "Invalid option" << endl;
      Pause();
      break;
  }

  return true;
}

bool PrintMenu(string &user, socket& s) {
  system("clear");

  cout << "Welcome " << user << " to SuperFancy FileSystem (SFFS)" << endl;
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
  return ExecuteOpt(opt, user, s);
}

int main(int argc, const char *argv[]) {
  string ip = "localhost", port = "5555";
  context ctx;
  socket s(ctx, socket_type::req);
  s.connect("tcp://" + ip + ":" + port);

  string user = GetName(s);

  bool cont;
  do {
    cont = PrintMenu(user, s);
  } while(cont);

  // context ctx;
  // socket s(ctx, socket_type::req);
  // s.connect("tcp://localhost:5555");
  //
  // json load;
  // load["user"] = "leiver";
  // load["name"] = "copy.mp3";
  //
  // ifstream fin("fs/Super Mario World Music - Title Theme.mp3", ios::binary);
  // if (!fin) {
  //   cout << "Mucha loca" << endl;
  //   return 0;
  // }
  // stringstream oss;
  // oss << fin.rdbuf();
  // fin.close();
  //
  // Base64 b64;
  // string in = oss.str(), out;
  // b64.Encode(in, &out);
  // load["file"] = out;
  //
  // message m;
  // m << load.dump();
  // s.send(m);

  return 0;
}
