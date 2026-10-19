#pragma once

#include "../core/Types/Types.hpp"
#include <vector>

namespace nlm {

// Working memory: temporary active storage of information
// This implementation provides persistent storage of neuron-value associations
// with capacity limits and decay mechanisms for biological plausibility

class WorkingMemory {
public:
    WorkingMemory();
    ~WorkingMemory();
    
    // Store an item in working memory
    // If neuron already exists, its value is updated
    // If capacity is reached, oldest items may be evicted based on age/decay
    void store(NeuronId neuron, float value);
    
    // Retrieve the value associated with a neuron
    // Returns 0.0f if neuron is not in memory
    float retrieve(NeuronId neuron) const;
    
    // Check if a neuron is currently in working memory
    bool contains(NeuronId neuron) const;
    
    // Clear all items from working memory
    void clear();
    
    // Capacity management
    size_t getCapacity() const;
    size_t getCurrentSize() const;
    
    // Apply decay to all items (biologically inspired forgetting)
    void decay(float decayRate);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
