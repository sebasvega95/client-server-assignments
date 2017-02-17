#ifndef FILE_HPP
#define FILE_HPP

#include <bits/stdc++.h>
#include "constants.hpp"
#include "base64.hpp"
#define FILE_NOT_FOUND "@"

using namespace std;

class ReadFileBase64 {
private:
  string filename;
  int cur_pos;
  bool finished;
public:
  ReadFileBase64(string _filename) : filename(_filename) {}

  string operator()() {
    ifstream fin(filename, ios::binary);
    if (!fin) {
      return FILE_NOT_FOUND;
    }

    char *buffer = new char[CHUNK_SIZE];
    fin.seekg(cur_pos);
    fin.read(buffer, CHUNK_SIZE);
    cur_pos += CHUNK_SIZE;
    finished = (bool) fin;

    Base64 b64;
    string out;
    b64.Encode(buffer, &out);

    return out;
  }

  bool is_finished() {
    return finished;
  }
};

class SaveFileBase64 {
private:
  string filename;
  bool is_first;
  public:
    SaveFileBase64(string _filename) : filename(_filename), is_first(true) {}

    void operator()() {
      Base64 b64;
      string out;
      b64.Decode(file, &out);
      ofstream fout(filename);
      if (!fout) {
        return false;
      }

      fout << out;
      fout.close();
      return true;
    }
};

// string ReadFileBase64(string& filename) {
//     ifstream fin(filename, ios::binary);
//     if (!fin) {
//         return FILE_NOT_FOUND;
//     }
//
//     stringstream oss;
//     oss << fin.rdbuf();
//     fin.close();
//
//     Base64 b64;
//     string in = oss.str(), out;
//     b64.Encode(in, &out);
//
//     return out;
// }

// bool SaveFileBase64(string& filename, string& file) {
//     Base64 b64;
//     string out;
//     b64.Decode(file, &out);
//     ofstream fout(filename);
//     if (!fout) {
//         return false;
//     }
//
//     fout << out;
//     fout.close();
//     return true;
// }

#endif
