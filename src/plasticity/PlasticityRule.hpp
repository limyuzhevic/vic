#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Abstract base class for plasticity rules
// PLACEHOLDER - Phase 2 will implement real plasticity rules

class LearningRateAdapter;

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    // TODO PHASE 2: Implement real plasticity
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule name
    virtual const char* getName() const = 0;
    
    // Check if rule is enabled
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // Set learning rate adapter for adaptive learning rates
    void setLearningRateAdapter(LearningRateAdapter* adapter);
    
    // Get current learning rate from adapter or default
    float getCurrentLearningRate() const;
    
    // Update learning rate based on synaptic history
    void updateLearningRate(Synapse* synapse, const std::vector<SynapticWeight>& weightHistory,
                           const std::vector<float>& stabilityHistory,
                           const std::vector<float>& performanceHistory,
                           TimestepDuration dt);
    
protected:
    PlasticityRule();
    
private:
    bool enabled_;
    LearningRateAdapter* learningRateAdapter_;
};

// HebbianRule constructor was missing, adding it back
class HebbianRule : public PlasticityRule {
public:
    HebbianRule();
    ~HebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian rule: decrease weight when neurons fire together
// PLACEHOLDER - Phase 2
class AntiHebbianRule : public PlasticityRule {
public:
    AntiHebbianRule();
    ~AntiHebbianRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Bienenstock-Cooper-Munro (BCM) rule with synaptic scaling
// PLACEHOLDER - Phase 2
class BCMRule : public PlasticityRule {
public:
    BCMRule();
    ~BCMRule() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // BCM parameters
    void setTheta(float theta);    // Sliding threshold
    float getTheta() const;
    void setLearningRate(float rate);    // Base learning rate
    float getLearningRate() const;
    void setSynapticScaling(float scaling);    // Synaptic weight scaling
    float getSynapticScaling() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
