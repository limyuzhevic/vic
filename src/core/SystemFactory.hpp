#pragma once

#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Abstract base class for all memory systems
class MemorySystem {
public:
    virtual ~MemorySystem() = default;
    
    // Initialize the memory system with brain reference
    virtual void initialize(class Brain* brain) = 0;
    
    // Clear all memory content
    virtual void clear() = 0;
    
    // Get system name for logging and identification
    virtual std::string getSystemName() const = 0;
    
    // Get current system capacity
    virtual size_t getCapacity() const = 0;
    
    // Set system capacity
    virtual void setCapacity(size_t capacity) = 0;
    
    // Check if system is initialized
    virtual bool isInitialized() const { return brain_ != nullptr; }
    
protected:
    class Brain* brain_ = nullptr;
};

// Abstract base class for prediction systems
class PredictionSystemBase {
public:
    virtual ~PredictionSystemBase() = default;
    
    // Initialize prediction system
    virtual void initialize(class Brain* brain) = 0;
    
    // Update prediction system
    virtual void update(class Brain* brain, SimulationStep step, Timestamp time) = 0;
    
    // Apply sensory input to prediction
    virtual void applySensoryInput(const class SensoryInput& input) = 0;
    
    // Get prediction accuracy
    virtual float getPredictionAccuracy() const = 0;
    
    // Get prediction error
    virtual float getPredictionError() const = 0;
};

// Abstract base class for cognitive systems
class CognitiveSystem {
public:
    virtual ~CognitiveSystem() = default;
    
    // Initialize cognitive system
    virtual void initialize(class Brain* brain) = 0;
    
    // Update cognitive system
    virtual void update(class Brain* brain, TimestepDuration dt) = 0;
    
    // Get system name
    virtual std::string getSystemName() const = 0;
    
    // Check if system is ready for use
    virtual bool isReady() const = 0;
};

// Abstract base class for neuromodulation systems
class NeuromodulatorBase {
public:
    virtual ~NeuromodulatorBase() = default;
    
    // Initialize neuromodulator
    virtual void initialize(class Brain* brain) = 0;
    
    // Update neuromodulator
    virtual void update(TimestepDuration dt) = 0;
    
    // Get neuromodulator level
    virtual float getLevel() const = 0;
    
    // Get neuromodulator type for identification
    virtual std::string getType() const = 0;
    
    // Reset neuromodulator state
    virtual void reset() = 0;
};

// Abstract base class for plasticity rules
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Configure plasticity parameters
    virtual void configure(float param1, float param2, float param3) = 0;
    
    // Update synapse based on spike history
    virtual void update(class Synapse* synapse, 
                       const std::vector<SpikeEvent>& preSpikes,
                       const std::vector<SpikeEvent>& postSpikes,
                       TimestepDuration dt) = 0;
    
    // Get plasticity rule name
    virtual std::string getRuleName() const = 0;
};

// Factory class for creating systems
class SystemFactory {
public:
    static std::unique_ptr<MemorySystem> createMemorySystem(const std::string& type);
    static std::unique_ptr<PredictionSystemBase> createPredictionSystem(const std::string& type);
    static std::unique_ptr<CognitiveSystem> createCognitiveSystem(const std::string& type);
    static std::unique_ptr<NeuromodulatorBase> createNeuromodulator(const std::string& type);
    static std::unique_ptr<PlasticityRule> createPlasticityRule(const std::string& type);
};

} // namespace nlm