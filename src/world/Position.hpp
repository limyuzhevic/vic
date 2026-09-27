#pragma once

namespace nlm {

// Position structure for world objects
struct Position {
    float x = 0.0f;
    float y = 0.0f;
    
    Position() = default;
    Position(float x, float y) : x(x), y(y) {}
    
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
    
    bool operator!=(const Position& other) const {
        return !(*this == other);
    }
};

} // namespace nlm
