// Core types interface
#ifndef NLM_CORE_TYPES_HPP
#define NLM_CORE_TYPES_HPP

#include <cstdint>
#include <cmath>
#include <limits>
#include <algorithm>

namespace nlm {
namespace core {

// Basic numeric types
using byte = uint8_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using float32 = float;
using float64 = double;

// Neural connectivity types
using ConnectionWeight = float64;
using NeuronID = uint32;
using RegionID = uint16;
using SynapseID = uint32;
using PopulationID = uint16;

// Time units
using TimeSeconds = float64;
using TimeMilliseconds = float64;
using TimeDays = float64;

// Probability and percentage
using Probability = float64;
using Percentage = float64;

// Neural state types
enum class NeuronState {
    RESTING,
    DEPOLARIZING,
    SPIKING,
    REFRACTORY,
    SYNAPTIC,
    SENSORY,  // ADDED: Missing NeuronState enum value
    MOTOR    // ADDED: Missing NeuronState enum value
};

// Connection strength enum
enum class ConnectionStrength {
    WEAK,
    MODERATE,
    STRONG,
    EXCITATORY,
    INHIBITORY
};

// Direction vector
struct Vector3D {
    float64 x, y, z;
    
    Vector3D() : x(0), y(0), z(0) {}
    Vector3D(float64 x, float64 y, float64 z) : x(x), y(y), z(z) {}
    
    float64 magnitude() const { return std::sqrt(x*x + y*y + z*z); }
    Vector3D normalize() const {
        float64 mag = magnitude();
        return Vector3D(x/mag, y/mag, z/mag);
    }
    
    Vector3D operator+(const Vector3D& other) const {
        return Vector3D(x + other.x, y + other.y, z + other.z);
    }
    Vector3D operator-(const Vector3D& other) const {
        return Vector3D(x - other.x, y - other.y, z - other.z);
    }
    Vector3D operator*(float64 scalar) const {
        return Vector3D(x * scalar, y * scalar, z * scalar);
    }
    Vector3D operator/(float64 scalar) const {
        return Vector3D(x / scalar, y / scalar, z / scalar);
    }
};

// Color type
struct Color {
    float64 r, g, b, a;
    
    Color() : r(0), g(0), b(0), a(1) {}
    Color(float64 r, float64 g, float64 b, float64 a = 1.0) : r(r), g(g), b(b), a(a) {}
};

// Timestamp type
using Timestamp = uint64_t;

// Config options
struct ConfigOptions {
    bool validationEnabled = true;
    bool profilingEnabled = false;
    uint32 maxConnections = 1000;
    uint32 maxPopulations = 100;
    float64 connectionWeightRange = 2.0;
    float64 learningRate = 0.01;
    float64 discountFactor = 0.99;
};

} // namespace core
} // namespace nlm

#endif // NLM_CORE_TYPES_HPP
