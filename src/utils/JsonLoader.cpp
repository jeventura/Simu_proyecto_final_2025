#include "JsonLoader.hpp"
#include <fstream>
#include "../utils/json.hpp"

using namespace std;
using json = nlohmann::json;

bool loadPentagonosFromJson(const string& path, vector<Pentagono>& result) {
    ifstream file(path);
    if (!file.is_open()) return false;

    json j;
    file >> j;

    try {
        if (j.contains("pentagonos")) {
            result = j["pentagonos"].get<vector<Pentagono>>();
            return true;
        }
    } catch (...) {
        return false;
    }

    return false;
}