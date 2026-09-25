#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Forward declaration to avoid circular dependencies
class Brain;

// Observation class for reward computation
class Observation {
public:
    float value;
    
    Observation() : value(0.0f) {}
    explicit Observation(float val) : value(val) {}
    
    float getValue() const { return value; }
    void setValue(float val) { value = val; }
};

} // namespace nlm
