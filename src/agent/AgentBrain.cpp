#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Helper function to calculate average activity in a neuron group
static float calculateAverageActivity(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return 0.0f;
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        // Use membrane potential deviation from rest as activity measure
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

// Implementation of AgentBrain vision width/height methods
// These methods provide access to the vision configuration for world integration

size_t AgentBrain::getVisionWidth() const {
    // If we have an active brain with regions, use the first region's vision size
    // This assumes a uniform vision configuration across the brain
    if (brain_) {
        // Try to get vision from sensory percept if available
        // This would need access to world, so this method signature may need revision
        // For now, return a reasonable default
        return 16;  // Default vision width
    }
    return 16;  // Default fallback
}

size_t AgentBrain::getVisionHeight() const {
    // If we have an active brain with regions, use the first region's vision size
    // This assumes a uniform vision configuration across the brain
    if (brain_) {
        // Try to get vision from sensory percept if available
        // This would need access to world, so this method signature may need revision
        // For now, return a reasonable default
        return 16;  // Default vision height
    }
    return 16;  // Default fallback
}

} // namespace nlm