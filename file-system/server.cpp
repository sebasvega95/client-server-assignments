#include <bits/stdc++.h>
#include <zmqpp/zmqpp.hpp>
#include "json.hpp"
#include "base64.hpp"

using namespace std;
using namespace zmqpp;
using json = nlohmann::json;

int main(int argc, char const *argv[]) {
  context ctx;
  socket s(ctx, socket_type::rep);
  s.bind("tcp://*:5555");

  cout << "Waiting for message..." << endl;
  message m;
  s.receive(m);
  cout << "Message received!" << endl;

  string text;
  m >> text;
  cout << text << endl;
  auto r = json::parse(text);

  Base64 b64;
  string in = r["file"], out;
  b64.Decode(in, &out);

  string filename = r["name"];
  ofstream fout(filename);
  fout << out;
  fout.close();

  cout << "File written!" << endl;

  return 0;
}
