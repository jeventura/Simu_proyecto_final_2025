#include "JsonLoader.hpp"
#include <fstream>
#include "../utils/json.hpp"

using namespace std;
using json = nlohmann::json;

bool loadDataFromJson(const string& path, vector<Pentagono>& result, int& steps) {
    ifstream file(path);
    if (!file.is_open()) return false;

    json j;
    file >> j;

    try {
        if (j.contains("pentagonos") && j.contains("pasos")) {
            result = j["pentagonos"].get<vector<Pentagono>>();
            steps = j["pasos"].get<int>();
            return true;
        }
    } catch (...) {
        return false;
    }

    return false;
}