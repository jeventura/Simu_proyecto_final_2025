#pragma once
#include <vector>
#include "../model/Pentagono.hpp"
#include <string>

bool loadPentagonosFromJson(const std::string& path, std::vector<Pentagono>& result);
