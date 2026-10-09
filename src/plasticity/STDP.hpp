#pragma once

#include "PlasticityRule.hpp"

namespace nlm {

// Spike-Timing-Dependent Plasticity
// Real implementation based on spike-timing correlation

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

// Reward-modulated STDP (R-STDP)
// STDP weights are modulated by reward prediction error
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
    
    // Reward parameters
    void setRewardScale(float scale);           // Scale factor for reward influence
    float getRewardScale() const;
    void setBaselineWeight(float baseline);     // Baseline weight for reward modulation
    float getBaselineWeight() const;
    void setInhibitionStrength(float strength); // Inhibition strength for reward signals
    float getInhibitionStrength() const;
    
    // Eligibility trace parameters
    void setTraceDecay(float decayRate);        // Rate of eligibility trace decay
    float getTraceDecay() const;
    void setTraceIncrement(float increment);    // Increment per weight change
    float getTraceIncrement() const;
    
    // Update from reward signal
    void applyRewardSignal(float reward);
    
    // Get current eligibility trace for a synapse
    float getEligibilityTrace(Synapse* synapse) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Spike-timing dependent plasticity with triplet interactions
// Implements both pre-post and post-pre triplet rules
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
    
    // Triplet parameters
    void setAPlus(float aPlus);          // Post-before-post potentiation (A+)
    float getAPlus() const;
    void setAMinus(float aMinus);        // Post-before-post depression (A-)
    float getAMinus() const;
    void setBPlus(float bPlus);          // Pre-before-post potentiation (B+)
    float getBPlus() const;
    void setBMinus(float bMinus);        // Pre-before-post depression (B-)
    float getBMinus() const;
    void setTauPlus(float tauPlus);      // Time constant for positive spikes (ms)
    float getTauPlus() const;
    void setTauMinus(float tauMinus);    // Time constant for negative spikes (ms)
    float getTauMinus() const;
    
    // Triplet window parameters
    void setTripletWindow(float window); // Time window for triplet detection (ms)
    float getTripletWindow() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Homeostatic plasticity rule
// Regulates overall synaptic strength to maintain stability
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
    void setTargetMeanRate(float targetRate);    // Target firing rate (Hz)
    float getTargetMeanRate() const;
    void setPlasticityThreshold(float threshold); // Threshold for homeostatic regulation
    float getPlasticityThreshold() const;
    void setAdaptationRate(float rate);          // Rate of homeostatic adaptation
    float getAdaptationRate() const;
    void setTimeConstant(float tau);             // Time constant for homeostatic processes
    float getTimeConstant() const;
    
    // Update from neural activity statistics
    void updateFromActivity(float meanRate, float variance);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Anti-Hebbian plasticity rule
// Decreases weight when neurons fire together (opposite of Hebbian)
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
    
    // Anti-Hebbian parameters
    void setAntiHebbianStrength(float strength); // Strength of anti-Hebbian effect
    float getAntiHebbianStrength() const;
    void setAntiHebbianThreshold(float threshold); // Threshold for anti-Hebbian activation
    float getAntiHebbianThreshold() const;
    
    // Competitive learning parameters
    void setCompetitionFactor(float factor);     // Factor for competitive interactions
    float getCompetitionFactor() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Metaplasticity rule
// Regulates the plasticity itself (plasticity of plasticity)
class Metaplasticity : public PlasticityRule {
public:
    Metaplasticity();
    ~Metaplasticity() override;
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Metaplasticity parameters
    void setPlasticityLearningRate(float rate); // Rate of plasticity change
    float getPlasticityLearningRate() const;
    void setPlasticityTarget(float target);     // Target plasticity level
    float getPlasticityTarget() const;
    void setPlasticityExponent(float exponent); // Exponent for plasticity regulation
    float getPlasticityExponent() const;
    
    // Current plasticity state
    float getPlasticityState() const;
    void resetPlasticityState();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
