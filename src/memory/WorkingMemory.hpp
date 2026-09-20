#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>
#include <string>

namespace nlm {

// Working memory: Temporary storage for active neural traces
// Used for maintaining information during current processing
class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Set capacity
    void setCapacity(size_t capacity);
    
    // Add a trace (information chunk)
    void addTrace(const std::vector<float>& trace);
    
    // Get traces by index or type
    std::vector<float> getTrace(size_t index) const;
    std::vector<std::vector<float>> getAllTraces() const;
    
    // Remove traces (with decay)
    void removeTrace(size_t index);
    void decayAllTraces(float decayRate);
    
    // Query state
    size_t getTraceCount() const;
    size_t getActiveTraceCount() const;
    size_t getCapacity() const;
    
    // Integration with neural processing
    void update(float timeStep);
    void updateWithNeurons(const std::vector<Neuron*>& neurons);
    
    // Competition and selection
    size_t selectMostActiveTrace();
    void suppressTraces(size_t keepIndex, float suppressionStrength);
    
    // Connection to episodic memory
    void recordEpisode(size_t episodeId, const std::vector<std::vector<float>>& traces);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm