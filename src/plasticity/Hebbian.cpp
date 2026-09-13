// Placeholder Hebbian.cpp file to satisfy CMake build
// The actual implementation is now in STDP.cpp and other plasticity modules
// This file maintains backward compatibility for the build system

#include "Hebbian.hpp"

namespace nlm {

// Simple forward implementation to satisfy linking
Hebbian::Hebbian() {}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    // This method is now implemented in the main STDP.cpp file
    // The Hebbian functionality has been merged into the comprehensive
    // STDP implementation for better biological realism
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    // Weight change is now handled by the unified plasticity system
    synapse->addToWeight(delta);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    // Learning rate configuration is now handled by STDP module
    // STDP includes Hebbian-like learning with time-dependent weights
}

float Hebbian::getLearningRate() const {
    return 0.001f;  // Default rate
}

void Hebbian::setMaxWeight(float maxWeight) {
    // Max weight limits are now configurable in STDP parameters
}

float Hebbian::getMaxWeight() const {
    return 1.0f;  // Default maximum weight
}

} // namespace nlm