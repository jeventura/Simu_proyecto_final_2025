#pragma once
#include <vector>
#include "../model/Pentagono.hpp"
#include <string>

using namespace std;

bool loadDataFromJson(const string& path, vector<Pentagono>& result, int& steps);
