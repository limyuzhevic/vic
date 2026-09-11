#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for all synaptic plasticity rules
// Defines the interface for synaptic learning and weight modification
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    // Called on every simulation step to apply learning rules
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change directly to synapse
    // Alternative method for direct weight modification
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule name for identification and logging
    virtual const char* getName() const = 0;
    
    // Check if rule is enabled for plasticity updates
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    bool enabled_;
};

// Type aliases for backward compatibility and convenience
using ClassicHebbian = Hebbian;      // Traditional implementation
using ModernHebbian = HebbianRule;   // Modern rule-based interface

// ============================================================================
// HEBBIAN PLASTICITY RULE
// ============================================================================

// Hebbian learning implementation: "neurons that fire together, wire together"
// Implements covariance-based learning with correlation window
// Enhanced with better parameter validation and error handling
class Hebbian : public PlasticityRule {
public:
    Hebbian();
    ~Hebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Core Hebbian parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setMaxWeight(float maxWeight);
    float getMaxWeight() const;
    void setMinWeight(float minWeight);
    float getMinWeight() const;
    
    // Convenience method for common configuration
    void configure(float learningRate, float maxWeight = 1.0f, float minWeight = -1.0f);
    
private:
    struct Impl;
    Impl* pImpl;
};

// ============================================================================
// STDP (SPIKE-TIMING-DEPENDENT PLASTICITY)
// ============================================================================

// Spike-Timing-Dependent Plasticity (STDP)
// Classic temporal learning rule from biological systems
// Implements precise spike timing-dependent weight modification
// Enhanced with better bounds checking and error handling
class STDP : public PlasticityRule {
public:
    STDP();
    ~STDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // STDP parameters
    void setLTPWeight(float weight);    // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // Long-term depression weight
    float getLTDWeight() const;
    void setTimeConstant(float tau);    // STDP time constant (ms)
    float getTimeConstant() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tau);
    
private:
    struct Impl;
    Impl* pImpl;
};

}} // namespace nlm