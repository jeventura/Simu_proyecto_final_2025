#pragma once
#include <vector>
#include "../model/Pentagono.hpp"
#include <string>

using namespace std;

bool loadPentagonosFromJson(const string& path, vector<Pentagono>& result);
