#pragma once

#include "PlasticityRule.hpp"
#include "../../core/Types/Types.hpp"
#include "../../brain/Synapse.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

namespace nlm {

// Enhanced STDP with biological realism and neuromodulation support
// This version includes:
// - Eligibility traces for delayed reward signals
// - Dopamine modulation of STDP parameters
// - Homeostatic plasticity mechanisms
// - Triplet spike interactions

class EnhancedSTDP : public PlasticityRule {
public:
    EnhancedSTDP();
    ~EnhancedSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Enhanced STDP parameters
    void setLTPWeight(float weight);           // Long-term potentiation weight
    float getLTPWeight() const;
    void setLTDWeight(float weight);           // Long-term depression weight  
    float getLTDWeight() const;
    void setTimeConstant(float tau);           // STDP time constant (ms)
    float getTimeConstant() const;
    
    // Neuromodulation parameters
    void setDopamineGain(float gain);          // Dopamine modulation strength
    float getDopamineGain() const;
    void setCuriosityGain(float gain);         // Curiosity modulation strength
    float getCuriosityGain() const;
    
    // Eligibility trace parameters
    void setEligibilityTimeConstant(float tau);  // Trace decay time
    float getEligibilityTimeConstant() const;
    void setMaxEligibilityTrace(float maxTrace);  // Maximum trace value
    float getMaxEligibilityTrace() const;
    
    // Homeostatic plasticity
    void setHomeostaticTarget(float target);   // Target firing rate
    float getHomeostaticTarget() const;
    void setHomeostaticLearningRate(float rate); // Learning rate for homeostasis
    float getHomeostaticLearningRate() const;
    
    // Configure all parameters
    void configure(float ltpWeight, float ltdWeight, float tau,
                   float eligibilityTau, float maxTrace,
                   float dopamineGain, float curiosityGain,
                   float homeostaticTarget, float homeostaticRate);
    
    // Update neuromodulatory signals
    void updateNeuromodulation(float dopamine, float curiosity, float novelty);
    
    // Get current eligibility trace
    float getEligibilityTrace(Synapse* synapse) const;
    
    // Reset eligibility trace for synapse
    void resetEligibilityTrace(Synapse* synapse);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-Modulated STDP (R-STDP)
// Combines STDP with reward prediction error signals
class RewardModulatedSTDP : public PlasticityRule {
public:
    RewardModulatedSTDP();
    ~RewardModulatedSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // R-STDP parameters
    void setBaseSTDP(float ltpWeight, float ltdWeight, float tau);
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Reward integration
    void setRewardScale(float scale);
    float getRewardScale() const;
    void setPredictionError(float error);
    float getPredictionError() const;
    
    void updateReward(float reward, float predictionError);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Homeostatic Plasticity
// Maintains network stability by scaling synaptic strengths
class HomeostaticPlasticity : public PlasticityRule {
public:
    HomeostaticPlasticity();
    ~HomeostaticPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Homeostatic parameters
    void setTargetFiringRate(float rate);      // Desired firing rate
    float getTargetFiringRate() const;
    void setLearningRate(float rate);          // Learning rate for homeostasis
    float getLearningRate() const;
    void setStabilizationFactor(float factor); // How strongly to stabilize
    float getStabilizationFactor() const;
    
    // Track network activity
    void updateNetworkStats(size_t totalSpikes, size_t totalNeurons);
    float getCurrentFiringRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Triplet STDP
// Incorporates spike-triplet interactions for more realistic learning
class TripletSTDP : public PlasticityRule {
public:
    TripletSTDP();
    ~TripletSTDP() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Triplet interaction parameters
    void setA1LTP(float weight);               // First spike LTP amplitude
    void setA2LTP(float weight);               // Second spike LTP amplitude
    void setB1LTD(float weight);               // First spike LTD amplitude
    void setB2LTD(float weight);               // Second spike LTD amplitude
    void setTauPlus(float tau);                // LTP time constant
    void setTauMinus(float tau);               // LTD time constant
    
    // Configuration helper
    void configure(float a1LTP, float a2LTP, float b1LTD, float b2LTD,
                   float tauPlus, float tauMinus);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

