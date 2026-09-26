// ============================================================================
// NEURON GROUP DISTRIBUTION UTILITIES IMPLEMENTATION
// ============================================================================

#include "Utilities.hpp"

namespace nlm {

// ============================================================================
// GROUP DISTRIBUTION UTILITIES IMPLEMENTATION
// ============================================================================

std::vector<size_t> distributeToGroups(size_t neuronCount, size_t groupCount, size_t startIndex) {
    std::vector<size_t> distribution(groupCount, 0);
    if (groupCount == 0) return distribution;
    
    size_t baseCount = neuronCount / groupCount;
    size_t remainder = neuronCount % groupCount;
    
    for (size_t i = 0; i < groupCount; ++i) {
        distribution[i] = baseCount + (i < remainder ? 1 : 0);
    }
    
    // Apply start offset if specified
    if (startIndex > 0 && startIndex < groupCount) {
        std::rotate(distribution.begin(), distribution.begin() + startIndex, distribution.end());
    }
    
    return distribution;
}

std::vector<size_t> distributeSensoryNeurons(size_t totalNeurons) {
    return distributeToGroups(totalNeurons, INTERNAL_SIGNALS_COUNT);
}

std::vector<size_t> distributeMotorNeurons(size_t totalNeurons) {
    return distributeToGroups(totalNeurons, MOTOR_ACTION_GROUPS);
}

} // namespace nlm
