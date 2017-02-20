#ifndef FILE_HPP
#define FILE_HPP

#include <bits/stdc++.h>
#include "constants.hpp"
#include "base64.hpp"
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

int FileSize(string& filename) {
  ifstream fin(filename, ios::binary);
  fin.seekg(0, fin.end);
  int length = fin.tellg();
  fin.close();

  return length;
}

json ReadFileBase64(string& filename, int cur_pos) {
  FILE *pFile  = fopen(filename.c_str(), "rb");
  char* buffer = (char*) malloc(CHUNK_SIZE * sizeof(char));
  json response;

  if (buffer == NULL) {
    cout << "mem :(" << endl;
    response = {
      {"error", true},
      {"message", "Memory error"}
    };
  } else if (pFile == NULL) {
    cout << "file :(" << endl;
    response = {
      {"error", true},
      {"message", "File not found"}
    };
  } else {
    fseek(pFile, cur_pos, SEEK_SET);
    int read = fread(buffer, 1, CHUNK_SIZE, pFile);
    cur_pos += read;
    bool finished = (bool) feof(pFile);

    Base64 b64;
    string out;
    b64.Encode(string(buffer, read), &out);
    response = {
      {"file", out},
      {"curPos", cur_pos},
      {"finished", finished}
    };

    fclose(pFile);
    free(buffer);
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
