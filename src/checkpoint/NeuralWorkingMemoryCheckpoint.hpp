// NeuralWorkingMemory checkpoint data structures

#pragma once

#include <vector>
#include "../core/Types/Types.hpp"

namespace nlm {

// NeuralWorkingMemory checkpoint state
struct NeuralWorkingMemoryCheckpoint {
    // Memory traces
    std::vector<NeuronId> memoryNeurons;
    std::vector<float> memoryActivations;
    std::vector<SimulationStep> memoryTimestamps;
    
    // Active traces
    std::vector<size_t> activeTraces;
    
    // Recurrent connections for maintenance
    std::vector<std::pair<NeuronId, NeuronId>> recurrentConnections;
    
    // Winner neurons
    std::vector<NeuronId> winners;
    
    // Capacity and parameters
    size_t capacity;
    float decayRate;
    
    // Neuromodulation factors
    float neuromodulationFactor;
    
    NeuralWorkingMemoryCheckpoint()
        : capacity(0), decayRate(0.0f), neuromodulationFactor(1.0f) {}
};

// NeuralWorkingMemory save/load interface
class NeuralWorkingMemorySaverLoader {
public:
    virtual ~NeuralWorkingMemorySaverLoader() = default;
    
    // Save working memory state
    virtual bool save(const NeuralWorkingMemoryCheckpoint& checkpoint) const = 0;
    
    // Load working memory state
    virtual bool load(NeuralWorkingMemoryCheckpoint& checkpoint) = 0;
    
    // Create empty checkpoint
    virtual NeuralWorkingMemoryCheckpoint createEmptyCheckpoint() const = 0;
};

} // namespace nlm
