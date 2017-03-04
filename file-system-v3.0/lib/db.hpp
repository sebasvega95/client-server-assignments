#ifndef DB_HPP
#define DB_HPP

#include <fstream>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

json db;

void InitDb() {
  ifstream fin("db.json");
  if (!fin) {
    ofstream fout("db.json");
    fout << R"({"users":{}})" << endl;
    fout.close();

    int err;
    err = system("rm -rf fs");
    err = system("mkdir fs");
    if (err == -1) {
      cout << "Couldn't init DB" << endl;
      exit(EXIT_FAILURE);
    }
    db = json::object();
  } else {
    fin >> db;
  }
  fin.close();
}

void UpdateDb() {
  ofstream fout("db.json");
  fout << setw(2) << db << endl;
  fout.close();
}

#endif
