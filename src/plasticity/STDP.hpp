#pragma once

#include "PlasticityRule.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

// STDP with proper spike-timing windows (A+ and A- exponential decays)
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
    
    // STDP parameters - now biologically realistic
    void setLTPWeight(float weight);    // A+ for potentiation (~10-20ms window)
    float getLTPWeight() const;
    void setLTDWeight(float weight);    // A- for depression (~30-50ms window)
    float getLTDWeight() const;
    void setTimeConstantPlus(float tau);    // τ+ for potentiation time constant
    float getTimeConstantPlus() const;
    void setTimeConstantMinus(float tau);   // τ- for depression time constant
    float getTimeConstantMinus() const;
    void setTauPlus(float tauPlus);         // Alternative parameter name
    float getTauPlus() const;
    void setTauMinus(float tauMinus);        // Alternative parameter name
    float getTauMinus() const;
    void setHomeostaticFactor(float factor); // Homeostatic scaling factor
    float getHomeostaticFactor() const;
    
    // Update parameters from config
    void configure(float ltpWeight, float ltdWeight, float tauPlus, float tauMinus,
                   float homeostaticFactor = 1.0f);
    
    // Eligibility trace management
    void updateEligibilityTrace(Synapse* synapse, float delta, float decayRate = 0.1f);
    float getTraceValue(const Synapse* synapse) const;
    
    // Triplet STDP parameters
    void setTripletA1(float a1);      // A1 parameter for triplet STDP
    void setTripletA2(float a2);      // A2 parameter for triplet STDP
    void setTripletTau1(float tau1);  // τ1 for triplet STDP
    void setTripletTau2(float tau2);  // τ2 for triplet STDP
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward-modulated STDP (R-STDP) with dopamine integration
// Integrates spike timing with reward prediction errors for credit assignment
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
    
    // Dopamine modulation parameters
    void setDopamineLevel(float level);
    float getDopamineLevel() const;
    void setLearningRateModulation(float mod);
    float getLearningRateModulation() const;
    void setRewardPredictionError(float rpe);
    float getRewardPredictionError() const;
    
    // Credit assignment window
    void setCreditAssignmentWindow(float window);
    float getCreditAssignmentWindow() const;
    
    // Eligibility trace dynamics
    void updateEligibilityTrace(Synapse* synapse, float delta, float dt);
    void decayTraces(float decayRate);
    float getTraceValue(const Synapse* synapse) const;
    
    // Configure from dopamine system
    void configure(float ltpWeight, float ltdWeight, float tauPlus, float tauMinus,
                   float dopamineLevel = 0.0f, float rpe = 0.0f);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Triplet STDP (Spike-Timing Dependent Plasticity) with complex learning rules
// Captures interactions between 3 spike events (pre-post, post-pre, etc.)
// More biologically realistic than pairwise STDP
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
    
    // Triplet STDP parameters
    void setA1(float a1);        // A1: pre-before-post triple contribution
    void setA2(float a2);        // A2: post-before-pre triple contribution
    void setA3(float a3);        // A3: post-post-pre triple contribution
    void setB1(float b1);        // B1: pre-post-post triple contribution
    void setB2(float b2);        // B2: post-pre-pre triple contribution
    
    // Time constants for different spike triplets
    void setTau1(float tau1);    // τ1: fast window (~10-20ms)
    void setTau2(float tau2);    // τ2: slow window (~100-200ms)
    void setTau3(float tau3);    // τ3: very slow window (~500-1000ms)
    
    // Weight bounds and constraints
    void setMinWeight(float min); // Minimum synaptic weight
    void setMaxWeight(float max); // Maximum synaptic weight
    
    // Triplet interaction strengths
    void setTripletInteractionStrength(float strength); // Overall triplet interaction scaling
    void setSynapticEligibilityTrace(float trace); // Baseline eligibility trace
    
    // Configure all parameters
    void configure(float a1, float a2, float a3, float b1, float b2,
                   float tau1, float tau2, float tau3, float strength = 1.0f);
    
    // Get statistics
    float getTripletSum(const std::vector<Timestamp>& spikes, float tau) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Homeostatic Plasticity Rule
// Maintains overall network stability by preventing runaway excitation/inhibition
// Implements synaptic scaling, firing rate homeostasis, and metaplasticity
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
    
    // Homeostatic control parameters
    void setTargetFiringRate(float target);       // Target average firing rate (Hz)
    void setHomeostaticLearningRate(float rate);  // How fast homeostasis acts
    void setMetaplasticityRate(float rate);       // Learning rate adaptation
    void setScalingExponent(float exp);           // Power law scaling exponent
    void setActivityHistorySize(size_t size);     // Number of time steps to average over
    
    // Synaptic scaling parameters
    void setScalingMinWeight(float min);          // Minimum weight for scaling
    void setScalingMaxWeight(float max);          // Maximum weight for scaling
    void setScalingTarget(float target);          // Target synaptic weight range
    
    // Configuration
    void configure(float targetRate, float learningRate = 0.001f);
    
    // Activity estimation
    float estimateNetworkActivity(Synapse* synapse, const std::vector<Timestamp>& spikes) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian Learning for Inhibitory Connections
// Implements "neurons that fire together, wire away" principle
// Primarily affects inhibitory synapses to maintain balance
class AntiHebbian : public PlasticityRule {
public:
    AntiHebbian();
    ~AntiHebbian() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setLearningRate(float rate);
    float getLearningRate() const;
    void setDepressionStrength(float strength);   // Maximum depression magnitude
    float getDepressionStrength() const;
    void setMinimumWeight(float min);             // Most negative weight possible
    float getMinimumWeight() const;
    void setMaximumWeight(float max);             // Least negative weight possible
    float getMaximumWeight() const;
    
    // Anti-Hebbian specific parameters
    void setCoactivityThreshold(float threshold);  // Activity level to trigger depression
    float getCoactivityThreshold() const;
    void setAntiHebbianLearningRate(float rate);   // Strength of anti-Hebbian effect
    float getAntiHebbianLearningRate() const;
    
    void configure(float learningRate, float depressionStrength = 0.1f);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm