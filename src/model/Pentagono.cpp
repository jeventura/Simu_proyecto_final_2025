#include "Pentagono.hpp"

void from_json(const json& json, Pentagono& pentagon) {
    json["base_index"].get_to(pentagon.base_index);
    json["vertice1"].get_to(pentagon.vertice1);
    json["vertice2"].get_to(pentagon.vertice2);
}