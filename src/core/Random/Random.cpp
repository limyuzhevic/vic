#include "Random.hpp"

namespace nlm {

uint32_t RandomGenerator::randomBit() {
    return uniformInt(0u, 1u);
}

void RandomGenerator::fillBytes(void* data, size_t size) {
    uint8_t* bytes = static_cast<uint8_t*>(data);
    for (size_t i = 0; i < size; ++i) {
        bytes[i] = static_cast<uint8_t>(uniformInt(0u, 255u));
    }
}

} // namespace nlm
