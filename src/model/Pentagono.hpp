#pragma once
#include "../utils/json.hpp"

using json = nlohmann::json;

struct Pentagono {
    int base_index;
    int vertice1;
    int vertice2;
    int type;
};

void from_json(const json& json, Pentagono& pentagono);