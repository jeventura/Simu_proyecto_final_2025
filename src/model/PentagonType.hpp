#pragma once

using namespace std;

// 0: Start, 1: Finish, 2: Wall, 3: Free, 4: Electric Wall, 5: Switch

enum class PentagonType {
    START, FINISH, WALL, FREE, ELECTRIC_WALL, SWITCH
};

int returnPentagonTypeAsInt(PentagonType type) {
    return static_cast<underlying_type<PentagonType>::type>(type);
}