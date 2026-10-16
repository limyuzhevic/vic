#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Abstract base class for all brain systems
class BrainSystem {
public:
    virtual ~BrainSystem() = default;
    
    // Get system name
    virtual const char* getName() const = 0;
    
    // Get system version
    virtual const char* getVersion() const { return "1.0"; }
    
    // Initialize system with brain reference
    // This will be used during setup phase
    virtual void initialize(class Brain* brain) {}
    
    // Update system (called each simulation step)
    virtual void update(TimestepDuration dt) {}
    
    // Reset system state
    virtual void reset() {}
    
    // Check if system is initialized
    virtual bool isInitialized() const { return isInitialized_; }
    
    // Set initialization state
    void setInitialized(bool initialized) { isInitialized_ = initialized; }
    
    // Get system ID for registration
    virtual uint32_t getSystemId() const { return 0; }
    
protected:
    BrainSystem() : isInitialized_(false) {}
    
private:
    bool isInitialized_;
};

// Abstract base class for neuromodulation systems
class NeuromodulationSystem : public virtual BrainSystem {
public:
    virtual ~NeuromodulationSystem() = default;
    
    // Apply neuromodulatory effects
    virtual void applyModulation() = 0;
    
    // Get current modulation level
    virtual float getModulationLevel() const = 0;
};

// Abstract base class for memory systems
class MemorySystem : public virtual BrainSystem {
public:
    virtual ~MemorySystem() = default;
    
    // Store information
    virtual void store(const std::vector<float>& data, float strength = 1.0f) = 0;
    
    // Retrieve information
    virtual std::vector<float> retrieve() const = 0;
    
    // Clear stored information
    virtual void clear() = 0;
    
    // Get number of stored items
    virtual size_t getItemCount() const = 0;
};

// Abstract base class for cognitive systems
class CognitiveSystem : public virtual BrainSystem {
public:
    virtual ~CognitiveSystem() = default;
    
    // Process input
    virtual void process(const std::vector<float>& input) = 0;
    
    // Get output
    virtual std::vector<float> getOutput() const = 0;
    
    // Get confidence in output
    virtual float getConfidence() const = 0;
};

// Abstract base class for prediction systems
class PredictionSystemBase : public virtual BrainSystem {
public:
    virtual ~PredictionSystemBase() = default;
    
    // Make prediction
    virtual std::unique_ptr<class SensoryInput> predict(const class SensoryInput& input) = 0;
    
    // Update prediction based on actual observation
    virtual void updatePrediction(const class SensoryInput& predicted, const class SensoryInput& actual) = 0;
    
    // Get prediction error
    virtual float getPredictionError() const = 0;
    
    // Get prediction confidence
    virtual float getConfidence() const = 0;
};

// Abstract base class for development systems
class DevelopmentSystemBase : public virtual BrainSystem {
public:
    virtual ~DevelopmentSystemBase() = default;
    
    // Update developmental state
    virtual void updateDevelopment(Brain* brain, SimulationStep step) = 0;
    
    // Get current developmental stage
    virtual DevelopmentalStage getStage() const = 0;
    
    // Set developmental stage
    virtual void setStage(DevelopmentalStage stage) = 0;
    
    // Check if critical period is active
    virtual bool isCriticalPeriod() const = 0;
};

// Brain system factory function type
using SystemFactory = std::function<std::unique_ptr<BrainSystem>(Brain* brain)>;

} // namespace nlm
