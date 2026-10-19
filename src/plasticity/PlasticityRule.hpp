#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <memory>
#include <vector>
#include <string>

namespace nlm
{

// Base class for plasticity rules with comprehensive biological implementations
// Implements real synaptic plasticity mechanisms including STDP, Hebbian, reward-modulated,
// metaplasticity, and homeostatic plasticity rules

class PlasticityRule {
public:
    virtual ~PlasticityRule() = default;
    
    // Update synaptic weights based on pre/post synaptic activity
    // Pure virtual implementation requires concrete subclasses
    virtual void update(Synapse* synapse, 
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) = 0;
    
    // Apply weight change with bounds checking and homeostasis
    virtual void applyWeightChange(Synapse* synapse, SynapticWeight delta) = 0;
    
    // Get rule name for identification and logging
    virtual const char* getName() const = 0;
    
    // Check if rule is enabled for learning
    bool isEnabled() const;
    void setEnabled(bool enabled);
    
    // Get rule-specific parameters (to be overridden by subclasses)
    virtual float getLearningRate() const { return learningRate_; }
    virtual void setLearningRate(float rate) { learningRate_ = rate; }
    
    // Check if rule applies based on plasticity flags
    virtual bool shouldApply(const PlasticityFlags& flags) const;
    
    // Apply homeostatic scaling to maintain network stability
    virtual void applyHomeostaticScaling(float globalTrend, float targetRate);
    
    // Metaplasticity - modulate learning rate based on previous learning
    virtual void updateMetaplasticity(float previousLearning, TimestepDuration dt);
    
    // Calculate eligibility trace for reward-modulated learning
    virtual float calculateEligibilityTrace(float delta, float decayRate) const;
    
    // Clamp synaptic weight to biological bounds
    static SynapticWeight clampWeight(SynapticWeight weight, SynapticWeight min, SynapticWeight max);
    
    // Calculate synaptic change based on covariance of pre/post activity
    static float calculateCovarianceChange(float preRate, float postRate, 
                                           float preBaseline, float postBaseline,
                                           float learningRate, float timeConstant);
    
    // Calculate STDP timing-based change
    static float calculateSTDPChange(float preSpikeTime, float postSpikeTime,
                                    float ltpWeight, float ltdWeight,
                                    float timeConstant, float decaySpeed);
    
protected:
    PlasticityRule() : 
        enabled_(true),
        learningRate_(0.01f),
        eligibilityTrace_(0.0f),
        metaplasticityFactor_(1.0f),
        homeostaticallyScaled_(false),
        lastLearningMagnitude_(0.0f) {}
    
    // Member variables for plasticity state
    bool enabled_;
    float learningRate_;
    float eligibilityTrace_;
    float metaplasticityFactor_;
    bool homeostaticallyScaled_;
    float lastLearningMagnitude_;
    PlasticityFlags plasticityFlags_;
};

// Implementation of PlasticityRule member functions
inline bool PlasticityRule::isEnabled() const { return enabled_; }
inline void PlasticityRule::setEnabled(bool enabled) { enabled_ = enabled; }

inline bool PlasticityRule::shouldApply(const PlasticityFlags& flags) const {
    return enabled_ && 
           (flags.hebbian || flags.stdp || flags.reward_modulated || 
            flags.structural || flags.eligible);
}

inline void PlasticityRule::applyHomeostaticScaling(float globalTrend, float targetRate) {
    if (globalTrend > targetRate * 1.2f) {
        // Network is too active, scale down learning
        learningRate_ *= 0.95f;
        homeostaticallyScaled_ = true;
    } else if (globalTrend < targetRate * 0.8f) {
        // Network is too inactive, scale up learning
        learningRate_ *= 1.05f;
        homeostaticallyScaled_ = true;
    }
}

inline void PlasticityRule::updateMetaplasticity(float previousLearning, TimestepDuration dt) {
    // Metaplasticity adapts learning based on previous learning history
    // High previous learning reduces future learning (stability)
    // Low previous learning increases future learning (exploration)
    if (std::abs(previousLearning) > 0.01f) {
        metaplasticityFactor_ = 1.0f / (1.0f + std::abs(previousLearning) * 10.0f);
    }
    learningRate_ *= metaplasticityFactor_;
}

inline float PlasticityRule::calculateEligibilityTrace(float delta, float decayRate) const {
    // Eligibility trace accumulates recent weight changes for delayed reward modulation
    return eligibilityTrace_ * decayRate + delta;
}

inline PlasticityRule::SynapticWeight PlasticityRule::clampWeight(SynapticWeight weight, 
                                                                SynapticWeight min, 
                                                                SynapticWeight max) {
    return std::clamp(weight, min, max);
}

inline float PlasticityRule::calculateCovarianceChange(float preRate, float postRate,
                                                      float preBaseline, float postBaseline,
                                                      float learningRate, float timeConstant) {
    // Covariance rule: Δw = η × (⟨x·y⟩ - ⟨x⟩⟨y⟩)
    float covariance = (preRate * postRate) - (preBaseline * postBaseline);
    return learningRate * covariance * timeConstant;
}

inline float PlasticityRule::calculateSTDPChange(float preSpikeTime, float postSpikeTime,
                                                 float ltpWeight, float ltdWeight,
                                                 float timeConstant, float decaySpeed) {
    // Asymmetric STDP based on spike timing
    float deltaT = postSpikeTime - preSpikeTime;
    
    if (deltaT > 0) {
        // Pre before post: Long-term potentiation (LTP)
        return ltpWeight * std::exp(-deltaT / timeConstant);
    } else {
        // Post before pre: Long-term depression (LTD)
        return -ltdWeight * std::exp(deltaT / timeConstant);
    }
}

// Comprehensive Hebbian plasticity implementation with multiple learning rules
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
    
    // Parameters
    void setLearningRate(float rate) override;
    float getLearningRate() const override;
    void setBaselineThreshold(float threshold);
    float getBaselineThreshold() const;
    void setHomeostaticTarget(float target);
    float getHomeostaticTarget() const;
    
    // Hebbian rule variants
    void enableCovarianceRule(bool enable);
    void enableBCMTheory(bool enable);
    void enableAntiHebbian(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Anti-Hebbian learning: neurons that fire together, wire apart
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
    void setDepressionRate(float rate);
    float getDepressionRate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Bienenstock-Cooper-Munro (BCM) theory with sliding threshold
class BCM : public PlasticityRule {
public:
    BCM();
    ~BCM() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Parameters
    void setTheta(float theta);      // Sliding threshold
    void setSigma(float sigma);      // Threshold sensitivity
    void setLearningRate(float rate);
    float getTheta() const;
    float getSigma() const;
    float getLearningRate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Homeostatic plasticity to maintain network stability
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
    
    // Parameters
    void setTargetFiringRate(float target);
    void setPlasticityExponent(float exponent);
    void setLearningRate(float rate);
    float getTargetFiringRate() const;
    float getPlasticityExponent() const;
    float getLearningRate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Spike-Timing-Dependent Plasticity (STDP) with multiple variants
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
    void setLTDWeight(float weight);    // Long-term depression weight
    void setTimeConstant(float tau);    // STDP time constant (ms)
    void setNMDAThreshold(float threshold);  // NMDA receptor activation threshold
    void setDopamineModulation(float modulation);
    void setSpatialWindow(float radius);
    void configure(float ltpWeight, float ltdWeight, float tau, float nmdqaThreshold = 0.0f);
    
    // STDP variants
    void enableAsymmetricSTDP(bool enable);
    void enableSpatialSTDP(bool enable);
    void enableNMDASTDP(bool enable);
    void enableDopamineSTDP(bool enable);
    
    // Getters
    float getLTPWeight() const;
    float getLTDWeight() const;
    float getTimeConstant() const;
    float getNMDAThreshold() const;
    float getDopamineModulation() const;
    float getSpatialWindow() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Reward-modulated plasticity with prediction error
class RewardModulatedPlasticity : public PlasticityRule {
public:
    RewardModulatedPlasticity();
    ~RewardModulatedPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Reward parameters
    void setPredictionError(float error);
    void setDopamineModulation(float modulation);
    void setDelayCreditAssignment(float delay);
    void setAsymmetricSensitivity(bool enable);
    void setTrialBasedModulation(bool enable);
    
    float getPredictionError() const;
    float getDopamineModulation() const;
    float getDelayCreditAssignment() const;
    bool getAsymmetricSensitivity() const;
    bool getTrialBasedModulation() const;
    
    // Reward prediction error calculation
    void calculatePredictionError(float predictedReward, float actualReward);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neuromodulator-integrated plasticity
class NeuromodulatorPlasticity : public PlasticityRule {
public:
    NeuromodulatorPlasticity();
    ~NeuromodulatorPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Neuromodulator-specific parameters
    void setDopamineLevel(float level);
    void setAcetylcholineLevel(float level);
    void setNorepinephrineLevel(float level);
    void setSerotoninLevel(float level);
    
    void setDopamineModulation(float modulation);
    void setAcetylcholineModulation(float modulation);
    void setNorepinephrineModulation(float modulation);
    void setSerotoninModulation(float modulation);
    
    float getDopamineLevel() const;
    float getAcetylcholineLevel() const;
    float getNorepinephrineLevel() const;
    float getSerotoninLevel() const;
    
    float getDopamineModulation() const;
    float getAcetylcholineModulation() const;
    float getNorepinephrineModulation() const;
    float getSerotoninModulation() const;
    
    // Neuromodulator interaction effects
    void integrateDopamineModulation(float baselineLearning, float rewardPrediction);
    void integrateAcetylcholineModulation(float attention, float novelty);
    void integrateNorepinephrineModulation(float arousal, float stress);
    void integrateSerotoninModulation(float mood, float social);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Structural plasticity for synapse formation and elimination
class StructuralPlasticity : public PlasticityRule {
public:
    StructuralPlasticity();
    ~StructuralPlasticity() override;
    
    void update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) override;
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override;
    const char* getName() const override;
    
    // Structural parameters
    void setSynaptogenesisRate(float rate);
    void setPruningRate(float rate);
    void setMinWeightThreshold(float threshold);
    void setActivityThreshold(float threshold);
    void setMaxSynapsesPerNeuron(size_t max);
    void setMaxTotalSynapses(size_t max);
    void enableActivityDependency(bool enable);
    
    // Structural operations
    SynapseId createSynapse(Brain* brain, NeuronId source, NeuronId destination,
                           SynapticWeight weight);
    bool removeSynapse(Brain* brain, SynapseId synapse);
    NeuronId createNeuron(Brain* brain, NeuronType type);
    bool removeNeuron(Brain* brain, NeuronId neuron);
    
    // Getters
    float getSynaptogenesisRate() const;
    float getPruningRate() const;
    float getMinWeightThreshold() const;
    float getActivityThreshold() const;
    size_t getMaxSynapsesPerNeuron() const;
    size_t getMaxTotalSynapses() const;
    bool getActivityDependency() const;
    
    // Statistics
    size_t getTotalSynapsesCreated() const;
    size_t getTotalSynapsesPruned() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Complete plasticity system coordinator
class PlasticitySystem {
public:
    PlasticitySystem();
    ~PlasticitySystem();
    
    // Add plasticity rule to the system
    void addPlasticityRule(std::unique_ptr<PlasticityRule> rule);
    
    // Get specific rule types
    STDP* getSTDP() const;
    Hebbian* getHebbian() const;
    NeuromodulatorPlasticity* getNeuromodulatorPlasticity() const;
    RewardModulatedPlasticity* getRewardModulatedPlasticity() const;
    StructuralPlasticity* getStructuralPlasticity() const;
    
    // Get all rules
    const std::vector<std::unique_ptr<PlasticityRule>>& getAllRules() const;
    
    // System-wide plasticity update
    void updateAll(Brain* brain, SimulationStep currentStep, Timestamp currentTime);
    
    // Configuration
    void configureFromDefaults();
    void configureFromConfig(const class Config& config);
    
    // Statistics and diagnostics
    void logPlasticityState() const;
    float getSystemPlasticityLoad() const;
    float getNetworkStability() const;
    
    // Reset system
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm

#endif // PLASTICITYRULE_HPP

