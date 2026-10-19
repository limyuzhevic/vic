#include "Types.hpp"
#include <stdexcept>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <iostream>

namespace nlm {

// Simple type validation functions
bool validateNeuronId(const NeuronId& id) {
    return id.value != UINT64_MAX;
}

bool validateSynapseId(const SynapseId& id) {
    return id.value != UINT64_MAX;
}

bool validateRegionId(const RegionId& id) {
    return id.value != UINT64_MAX;
}

bool validatePopulationId(const PopulationId& id) {
    return id.value != UINT64_MAX;
}

// Range validation function
bool validateRange(double value, double min, double max) {
    return value >= min && value <= max;
}

bool validateRange(float value, float min, float max) {
    return value >= min && value <= max;
}

bool validateRange(uint64_t value, uint64_t min, uint64_t max) {
    return value >= min && value <= max;
}

// Enum validation
bool validateNeuronType(NeuronType type) {
    return type == NeuronType::Excitatory || type == NeuronType::Inhibitory ||
           type == NeuronType::Modulatory || type == NeuronType::Sensory ||
           type == NeuronType::Motor || type == NeuronType::Internal;
}

// Type conversion functions
std::string neuronTypeToString(NeuronType type) {
    switch (type) {
        case NeuronType::Excitatory: return "Excitatory";
        case NeuronType::Inhibitory: return "Inhibitory";
        case NeuronType::Modulatory: return "Modulatory";
        case NeuronType::Sensory: return "Sensory";
        case NeuronType::Motor: return "Motor";
        case NeuronType::Internal: return "Internal";
        default: return "Unknown";
    }
}

NeuronType stringToNeuronType(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
    
    if (lower == "excitatory") return NeuronType::Excitatory;
    if (lower == "inhibitory") return NeuronType::Inhibitory;
    if (lower == "modulatory") return NeuronType::Modulatory;
    if (lower == "sensory") return NeuronType::Sensory;
    if (lower == "motor") return NeuronType::Motor;
    if (lower == "internal") return NeuronType::Internal;
    
    return NeuronType::Internal;
}

// Serialization functions
std::string serializeNeuronId(const NeuronId& id) {
    return std::to_string(id.value);
}

NeuronId deserializeNeuronId(const std::string& str) {
    try {
        return NeuronId(std::stoull(str));
    } catch (...) {
        return INVALID_NEURON_ID;
    }
}

// Constants
constexpr float PI = 3.1415926535f;
constexpr float E = 2.718281828f;
constexpr float EPSILON = 1e-6f;
constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;
constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;

// Math functions
double clamp(double value, double min, double max) {
    return std::max(min, std::min(max, value));
}

float clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

double lerp(double a, double b, double t) {
    return a + t * (b - a);
}

float lerp(float a, float b, float t) {
    return a + t * (b - a);
}

bool nearlyEqual(double a, double b, double epsilon) {
    return std::abs(a - b) <= epsilon * std::max(std::abs(a), std::abs(b));
}

bool nearlyEqual(float a, float b, float epsilon) {
    return std::abs(a - b) <= epsilon * std::max(std::abs(a), std::abs(b));
}

// Global initialization function
void initializeNLMTypes() {
    std::cout << "[NLM TYPES] Initializing NLM type system..." << std::endl;
}

// Global cleanup function
void cleanupNLMTypes() {
    std::cout << "[NLM TYPES] Cleaning up NLM type system..." << std::endl;
}

// Explicit template instantiations for commonly used types
namespace {
    struct TypeValidationInstantiations {
        TypeValidationInstantiations() {
            validateNeuronId(NeuronId(0));
            validateSynapseId(SynapseId(0));
            validateRange(5.0, 0.0, 10.0);
            validateRange(5.0f, 0.0f, 10.0f);
            validateNeuronType(NeuronType::Excitatory);
        }
    } validationInstantiations;
    
    struct TypeConversionInstantiations {
        TypeConversionInstantiations() {
            neuronTypeToString(NeuronType::Excitatory);
            stringToNeuronType("excitatory");
        }
    } conversionInstantiations;
    
} // namespace

} // namespace nlm