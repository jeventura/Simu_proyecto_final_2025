#pragma once
#include "../model/PentagonType.hpp"
#include "../model/Pentagono.hpp"

using namespace std;

vector<int> findShortestPath(
  int start, int end,
  const vector<vector<int>>& adj,
  const vector<Pentagono>& pentagonos,
  const map<Pentagono,bool>& switches
);
