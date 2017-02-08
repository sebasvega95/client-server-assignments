#include <bits/stdc++.h>
#include <zmqpp/zmqpp.hpp>
#include "json.hpp"
#include "base64.hpp"

using namespace std;
using namespace zmqpp;
using json = nlohmann::json;

int main(int argc, char const *argv[]) {
  context ctx;
  socket s(ctx, socket_type::req);
  s.connect("tcp://localhost:5555");

  json load;
  load["user"] = "aerendon";
  load["name"] = "copy.png";


  ifstream fin("mushroom.png", ios::binary);
  stringstream oss;
  oss << fin.rdbuf();

  Base64 b64;
  string in = oss.str(), out;
  b64.Encode(in, &out);
  load["file"] = out;

  message m;
  m << load.dump();
  s.send(m);
  fin.close();

  return 0;
}
