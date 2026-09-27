#pragma once

#include "OptimizedWorkingMemory.hpp"
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <memory>
#include <mutex>
#include <chrono>
#include <cmath>

namespace nlm {

class OptimizedWorkingMemory {
public:
    struct MemoryTrace {
        NeuronId neuronId;
        float activation;
        Timestamp timestamp;
        float decayFactor;
        bool active;
        float competitionScore;
        
        MemoryTrace() : neuronId(), activation(0.0f), timestamp(0.0), 
                        decayFactor(0.99f), active(false), competitionScore(0.0f) {}
        MemoryTrace(NeuronId id, float act, Timestamp ts)
            : neuronId(id), activation(act), timestamp(ts), 
              decayFactor(0.99f), active(true), competitionScore(0.0f) {}
    };

    OptimizedWorkingMemory();
    ~OptimizedWorkingMemory();

    // Disable copying, enable moving
    OptimizedWorkingMemory(const OptimizedWorkingMemory&) = delete;
    OptimizedWorkingMemory& operator=(const OptimizedWorkingMemory&) = delete;
    OptimizedWorkingMemory(OptimizedWorkingMemory&&) noexcept;
    OptimizedWorkingMemory& operator=(OptimizedWorkingMemory&&) noexcept;

    // Core memory operations
    void storeToNeuron(NeuronId neuronId, float activation);
    void storeTrace(const MemoryTrace& trace);
    float getNeuronActivation(NeuronId neuronId) const;
    const MemoryTrace* getTrace(NeuronId neuronId) const;
    std::vector<NeuronId> getMemoryNeurons() const;
    std::vector<float> getMemoryActivations() const;

    // Optimized updates
    void update(TimestepDuration dt);
    void decayMemory(TimestepDuration dt);
    void resetCompetitionScores();

    // Memory management
    void clear();
    void setCapacity(size_t capacity);
    size_t getCapacity() const;
    size_t getActiveTraceCount() const;

    // Optimization
    void enableCompetition(bool enable);
    void setCompetitionStrength(float strength);
    void enableEfficientStorage(bool enable);
    void optimizeAccessPattern(const std::vector<NeuronId>& accessPattern);

    // Statistics
    float getAverageActivation() const;
    float getTotalMemoryLoad() const;
    std::string getStatistics() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
