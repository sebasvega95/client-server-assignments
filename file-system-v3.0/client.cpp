#include <iostream>
#include <string>
#include <libgen.h>
#include <zmqpp/zmqpp.hpp>
#include "lib/dispatcher.hpp"
#include "lib/client-opts.hpp"
#include "lib/constants.hpp"
#include "lib/json.hpp"

using namespace std;
using namespace zmqpp;
using json = nlohmann::json;

int GetOption(string &opt) {
  stringstream ss(opt);
  int o;
  ss >> o;

  if (!ss)
    return -1;
  return o;
}

bool ExecuteOpt(int opt, string &user, socket &broker_socket) {
  context ctx;
  socket server_socket(ctx, socket_type::req);
  string filename;

  if (opt != LS_REQ  && opt != 0) {
    cout << "Enter filename: ";
    cin >> filename;

    json req;
    char _fn[256];
    strcpy(_fn, filename.c_str());
    req["filename"] = basename(_fn);
    req["type"] = opt;
    req["user"] = user;

    Send(req, broker_socket);
    json res = Receive(broker_socket);
    if (res["res"] == "OK") {
      string server_dir = res["server"];
      server_socket.connect("tcp://" + server_dir);
      cout << "Connected to server " << endl;
    } else {
      cout << res["res"] << endl;
    }
  }

  switch (opt) {
    case LS_REQ:
      ListFiles(user, broker_socket);
      break;
    case GET_REQ:
      GetFileFromServer(user, filename, server_socket);
      break;
    case SEND_REQ:
      SendFileToServer(user, filename, server_socket);
      break;
    case RM_REQ:
      RemoveFile(user, filename, server_socket);
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

bool PrintMenu(string &user, socket &broker_socket) {
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
  return ExecuteOpt(opt, user, broker_socket);
}

string GetCredentials(int type, socket &broker_socket) {
  string name, password;

  do {
    cout << "Username: ";
    getline(cin, name);
    cout << endl << "Password: ";
    getline(cin, password);

    if (ValidateCredentials(name, password, type, broker_socket)) {
      break;
    }
  } while(true);

  return name;
}

string HomeMenu(socket &brocker_socket) {
  string user = "";
  do {
    cout << endl << endl;
    cout << "1. Login" << endl;
    cout << "2. Signup" << endl;
    cout << "0. Exit" << endl;
    string _opt;
    cin >> _opt;
    int opt = GetOption(_opt);
    cout << endl;

    switch (opt) {
      case 1 : {
        user = GetCredentials(LOGIN_OPT, brocker_socket);
      }
      case 2: {
        user = GetCredentials(SIGNUP_OPT, brocker_socket);
      }
      case 0: {
        cout << endl << "Bye bye!!" << endl;
        user = "";
        break;
      }
      default: {
        cout << "Invalid option" << endl;
        continue;
      }

      if (user != "")
        break;
      else
        cout << "Invalid credentials" << endl;
    }
  } while (true);
  return user;
}

int main(int argc, const char *argv[]) {
  if (argc != 2) {
    cout << "Usage: " << argv[0] << " <broker-ip>" << endl;
  } else {
    string ip = argv[1], port = "5555";
    context ctx;
    socket broker_socket(ctx, socket_type::req);
    broker_socket.connect("tcp://" + ip + ":" + port);

    string user = HomeMenu(broker_socket);
    if (user == "") {
       return 0;
    }

    cout << endl;
    cout << "Welcome " << user << " to SuperFancy FileSystem (SFFS)" << endl;
    bool cont;
    do {
      cont = PrintMenu(user, broker_socket);
    } while(cont);
  }

  return 0;
}
