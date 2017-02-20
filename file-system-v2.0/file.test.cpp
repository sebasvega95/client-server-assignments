#include <bits/stdc++.h>
#include "lib/file.hpp"
#include "lib/json.hpp"

using namespace std;
using json = nlohmann::json;

int main(int argc, char const *argv[]) {
  string ifname = "test-files/smw.mp3", ofname = "p.mp3", file;
  int cur_pos = 0;
  bool finished = false;
  bool first_time = true;
  int file_size = FileSize(ifname);

  do {
    json res = ReadFileBase64(ifname, cur_pos);
    finished = res["finished"];
    file = res["file"];
    cur_pos = res["curPos"];

    SaveFileBase64(ofname, file, first_time);
    cout << cur_pos << "/" << file_size << endl;
    first_time = false;
  } while (!finished);

  cout << "Done" << endl;

  return 0;
}
