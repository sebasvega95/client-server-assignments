#ifndef FILE_HPP
#define FILE_HPP

#include <bits/stdc++.h>
#include "constants.hpp"
#include "base64.hpp"
#include "json.hpp"
#define FILE_NOT_FOUND "@"

using namespace std;
using json = nlohmann::json;

// class ReadFileBase64 {
// private:
//   string filename;
//   int cur_pos;
//   bool finished;
// public:
//   ReadFileBase64(string _filename) : filename(_filename) {}
//
//   string operator()() {
//     ifstream fin(filename, ios::binary);
//     if (!fin) {
//       return FILE_NOT_FOUND;
//     }
//
//     char *buffer = new char[CHUNK_SIZE];
//     fin.seekg(cur_pos);
//     fin.read(buffer, CHUNK_SIZE);
//     cur_pos += CHUNK_SIZE;
//     finished = (bool) fin;
//
//     Base64 b64;
//     string out;
//     b64.Encode(buffer, &out);
//
//     return out;
//   }
//
//   bool is_finished() {
//     return finished;
//   }
// };

// class SaveFileBase64 {
// private:
//   string filename;
//   bool is_first;
//   public:
//     SaveFileBase64(string _filename) : filename(_filename), is_first(true) {}
//
//     void operator()() {
//       Base64 b64;
//       string out;
//       b64.Decode(file, &out);
//       ofstream fout(filename);
//       if (!fout) {
//         return false;
//       }
//
//       fout << out;
//       fout.close();
//       return true;
//     }
// };

json ReadFileBase64(string& filename, int cur_pos) {
  ifstream fin(filename, ios::binary);

  json response;
  if (!fin) {
    response = {
      {"error", true},
      {"message", "File not found"}
    };
  } else {
    char *buffer = new char[CHUNK_SIZE];
    fin.seekg(cur_pos);
    fin.read(buffer, CHUNK_SIZE);
    cur_pos += CHUNK_SIZE;
    bool finished = (bool) fin;

    Base64 b64;
    string out;
    b64.Encode(buffer, &out);

    response = {
      {"file", out},
      {"curPos", cur_pos},
      {"finished", finished}
    };
  }

  return response;
}

bool SaveFileBase64(string& filename, string& file, bool first_time) {
  Base64 b64;
  string out;
  b64.Decode(file, &out);
  ofstream fout;

  if (first_time) {
    fout.open(filename);
  } else {
    fout.open(filename, ios::app);
  }

  if (!fout) {
    return false;
  }
  fout << out;
  fout.close();
  return true;
}

#endif
