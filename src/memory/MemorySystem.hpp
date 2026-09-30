#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>
#include <string>

namespace nlm {

// Abstract base class for all memory systems
// Provides unified interface for different memory modalities
class MemorySystem {
public:
    virtual ~MemorySystem() = default;
    
    // Initialize memory system with brain reference
    virtual void initialize(Brain* brain) = 0;
    
    // Clear all memory content
    virtual void clear() = 0;
    
    // Check if memory system is initialized
    virtual bool isInitialized() const = 0;
    
    // Get memory system name/type
    virtual const char* getType() const = 0;
    
    // Get system-specific statistics
    virtual std::string getStats() const = 0;
    
    // Specialized virtual methods for each memory type
    virtual void update(float dt) {}  // For working memory dynamics
    virtual void store(const std::vector<float>& pattern, float strength = 1.0f) {}  // For storage
    virtual std::vector<float> retrieve() const { return std::vector<float>(); }  // For retrieval
    virtual bool contains(NeuronId neuron) const { return false; }  // For content checking
    
    // Common memory operations
    virtual void setCapacity(size_t capacity) {}
    virtual size_t getCapacity() const { return 0; }
    
    // Decay/update rate
    virtual void setDecayRate(float rate) {}
    virtual float getDecayRate() const { return 0.0f; }
    
    // Competition/reinforcement
    virtual void strengthenMemory(float factor) {}
    virtual void runCompetition() {}
};

} // namespace nlm
