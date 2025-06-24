#include "PentagonType.hpp"

int returnPentagonTypeAsInt(PentagonType type) {
    return static_cast<underlying_type<PentagonType>::type>(type);
}