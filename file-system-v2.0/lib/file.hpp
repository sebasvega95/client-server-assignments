#ifndef FILE_HPP
#define FILE_HPP

#include <bits/stdc++.h>
#include "base64.hpp"
#define FILE_NOT_FOUND "@"

using namespace std;

string ReadFileBase64(string& filename) {
    ifstream fin(filename, ios::binary);
    if (!fin) {
        return FILE_NOT_FOUND;
    }

    stringstream oss;
    oss << fin.rdbuf();
    fin.close();

    Base64 b64;
    string in = oss.str(), out;
    b64.Encode(in, &out);

    return out;
}

bool SaveFileBase64(string& filename, string& file) {
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

#endif
