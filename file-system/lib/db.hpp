#ifndef DB_HPP
#define DB_HPP

#include <bits/stdc++.h>
#include "json.hpp"

using namespace std;
using json = nlohmann::json;

json db;

void InitDb() {
  ifstream fin("db.json");
  fin >> db;
  fin.close();
}

void UpdateDb() {
  ofstream fout("db.json");
  fout << setw(2) << db << endl;
  fout.close();
}

#endif
