#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for plasticity rules
// Provides common functionality used by all plasticity rules
class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule name
    virtual const char* getName() const = 0;
    
    // Get learning rate
    virtual float getLearningRate() const = 0;
    
    // Set learning rate
    virtual void setLearningRate(float rate) = 0;
    
    // Set weight bounds
    virtual void setWeightBounds(float minWeight, float maxWeight) = 0;
    
    // Set weight change threshold
    virtual void setWeightChangeThreshold(float threshold) = 0;
    
    // Check if rule is enabled
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
protected:
    PlasticityRule() : enabled_(true) {}
    
private:
    struct Impl {
        float learningRate;           // Overall learning rate
        float minWeight;              // Minimum synaptic weight
        float maxWeight;              // Maximum synaptic weight
        float weightChangeThreshold;  // Threshold for applying weight changes
        float adaptationFactor;       // Rate of adaptation to previous changes
        
        Impl() : learningRate(0.01f), minWeight(-1.0f), maxWeight(1.0f),
                 weightChangeThreshold(0.001f), adaptationFactor(0.9f) {}
    };
    Impl* pImpl;
    bool enabled_;
};

} // namespace nlm