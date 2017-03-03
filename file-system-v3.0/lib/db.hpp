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
    fout << "{}" << endl;
    fout.close();

    system("rm -rf fs");
    system("mkdir fs");
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
