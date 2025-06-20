#pragma once
#include "../utils/json.hpp"

using namespace std;
using json = nlohmann::json;

struct Pentagono {
    int base_index;
    int vertice1;
    int vertice2;
    int type;

    bool operator<(const Pentagono& other) const {
        return tie(base_index, vertice1, vertice2, type) < tie(other.base_index, other.vertice1, other.vertice2, other.type);
    }
};

void from_json(const json& json, Pentagono& pentagono);