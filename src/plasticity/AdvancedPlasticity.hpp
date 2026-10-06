#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Homeostatic Plasticity Mechanisms
// 
// Implements multiple forms of homeostatic regulation to maintain neural stability
// and prevent runaway excitation/inhibition.
// 
// Biological inspiration:
// - Synaptic scaling (Turrigiano, 2008)
// - Intrinsic plasticity (M unaffected, 2010)
// - Neuromodulator-driven homeostasis (Bremner et al., 2017)
// - Activity-dependent structural plasticity (Schmidt et al., 2011)
// 
// Key principles:
// 1. Maintain overall firing rate within optimal range
// 2. Prevent pathological synchronization
// 3. Enable adaptive responses to changing conditions
// 4. Implement multiple timescales of regulation

class HomeostaticPlasticity {
public:
    HomeostaticPlasticity();
    ~HomeostaticPlasticity();
    
    // Disable copying, enable moving
    HomeostaticPlasticity(const HomeostaticPlasticity&) = delete;
    HomeostaticPlasticity& operator=(const HomeostaticPlasticity&) = delete;
    HomeostaticPlasticity(HomeostaticPlasticity&&) noexcept;
    HomeostaticPlasticity& operator=(HomeostaticPlasticity&&) noexcept;
    
    // Configuration parameters
    void setTargetFiringRate(float targetHz);
    void setHomeostaticGain(float gain);
    void setTimeConstant(float tauMs);
    void setSynapticScalingThreshold(float threshold);
    void setIntrinsicPlasticityThreshold(float threshold);
    
    // Get current state
    float getTargetFiringRate() const;
    float getCurrentFiringRate() const;
    float getHomeostaticError() const;
    float getSynapticScalingFactor() const;
    
    // Update homeostatic mechanisms
    void update(float currentFiringRate, TimestepDuration dt);
    
    // Apply homeostatic plasticity to a synapse
    // Called from synapse::step() during plasticity update
    void applyToSynapse(class Synapse* synapse, SynapticWeight currentWeight,
                       float recentActivity, TimestepDuration dt);
    
    // Apply synaptic scaling to all synapses in a network
    void applySynapticScaling(const std::vector<class Synapse*>& synapses,
                              float globalScalingFactor);
    
    // Apply intrinsic plasticity to neurons
    void applyIntrinsicPlasticity(class Neuron* neuron, float neuronActivity,
                                 TimestepDuration dt);
    
    // Reset homeostatic state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng);
    
    // Get name
    const char* getName() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced Reward-Modulated Plasticity
// 
// Extends traditional reward-modulated plasticity with:
// - Multiple eligibility trace mechanisms
// - Neuromodulator-specific weight changes
// - Reward prediction error integration
// - Credit assignment for temporally extended actions
// 
// Biological inspiration:
// - Dopamine prediction error signals (Schultz, 2016)
// - Neuromodulatory gating of plasticity (Yagishita et al., 2014)
// - Trace-based eligibility (Steiner & Sejnowski, 2013)
// - Reward timing and delay modulation (Britten et al., 1996)

class RewardModulatedPlasticity {
public:
    RewardModulatedPlasticity();
    ~RewardModulatedPlasticity();
    
    // Disable copying, enable moving
    RewardModulatedPlasticity(const RewardModulatedPlasticity&) = delete;
    RewardModulatedPlasticity& operator=(const RewardModulatedPlasticity&) = delete;
    RewardModulatedPlasticity(RewardModulatedPlasticity&&) noexcept;
    RewardModulatedPlasticity& operator=(RewardModulatedPlasticity&&) noexcept;
    
    // Configuration parameters
    void setLearningRate(float rate);
    void setPredictionErrorGain(float gain);
    void setTraceDecayRate(float decayRate);
    void setRewardDelay(float delayMs);
    void setAlternativeLearningRate(float rate);
    void setTemporalDifferenceLambda(float lambda);
    
    // Get current state
    float getLearningRate() const;
    float getPredictionErrorGain() const;
    float getTraceDecayRate() const;
    float getExpectedReward() const;
    float getPredictionError() const;
    
    // Update with reward prediction error
    void update(float reward, float predictedReward, TimestepDuration dt);
    
    // Compute eligibility trace contribution from synaptic activity
    void computeEligibilityTrace(class Synapse* synapse, float synapticActivity,
                                Timestamp spikeTime, TimestepDuration dt);
    
    // Apply reward-modulated weight change
    void applyToSynapse(class Synapse* synapse, float neuromodulatorLevel,
                       TimestepDuration dt);
    
    // Add prediction error to a synapse (TD learning)
    void addPredictionError(class Synapse* synapse, float predictionError,
                           TimestepDuration dt);
    
    // Compute alternative learning rate (curiosity-driven)
    float computeAlternativeLearningRate(float novelty, float curiosity,
                                         TimestepDuration dt);
    
    // Reset reward-modulated state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng);
    
    // Get name
    const char* getName() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Meta-Plasticity System
// 
// Implements plasticity that regulates plasticity itself
// - Adapts learning rates based on experience
// - Prevents overfitting to specific patterns
// - Enables exploration-exploitation balance
// - Implements experience-dependent synaptic refinement
// 
// Biological inspiration:
// - Metaplasticity (soft synaptic consolidation) (Abraham & Lee, 1997)
// - Experience-dependent synaptic pruning (Kornblith & Markram, 2010)
// - Learning rate adaptation (Surmi et al., 2022)
// - Memory allocation for new information (Goswami et al., 2022)

class MetaPlasticity {
public:
    MetaPlasticity();
    ~MetaPlasticity();
    
    // Disable copying, enable moving
    MetaPlasticity(const MetaPlasticity&) = delete;
    MetaPlasticity& operator=(const MetaPlasticity&) = delete;
    MetaPlasticity(MetaPlasticity&&) noexcept;
    MetaPlasticity& operator=(MetaPlasticity&&) noexcept;
    
    // Configuration parameters
    void setPlasticityLearningRate(float rate);
    void setStabilityFactor(float factor);
    void setComplexityTarget(float target);
    void setMemoryAllocationRate(float rate);
    void setConsolidationStrength(float strength);
    
    // Get current state
    float getPlasticityLearningRate() const;
    float getStabilityFactor() const;
    float getComplexityScore() const;
    float getMemoryUtilization() const;
    
    // Update metaplastic state
    void update(float learningProgress, float networkComplexity,
                TimestepDuration dt);
    
    // Modify plasticity rules based on experience
    void adaptPlasticityRules(class STDP* stdp, class Hebbian* hebbian,
                             class Synapse* synapse, TimestepDuration dt);
    
    // Implement soft synaptic consolidation
    void consolidateMemory(class Synapse* synapse, float consolidationSignal,
                          TimestepDuration dt);
    
    // Allocate memory for new patterns
    void allocateNewMemory(class Synapse* synapse, float novelty,
                           TimestepDuration dt);
    
    // Compute synaptic complexity score
    float computeSynapticComplexity(const std::vector<class Synapse*>& synapses);
    
    // Reset metaplastic state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng);
    
    // Get name
    const char* getName() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Synaptic Scaling and Global Homeostasis
// 
// Implements network-wide mechanisms for synaptic balance
// - Global synaptic scaling (Turrigiano, 2008)
// - Structural plasticity for network optimization
// - Activity-dependent synaptic reorganization
// - Network-level homeostatic control

class SynapticScaling {
public:
    SynapticScaling();
    ~SynapticScaling();
    
    // Disable copying, enable moving
    SynapticScaling(const SynapticScaling&) = delete;
    SynapticScaling& operator=(const SynapticScaling&) = delete;
    SynapticScaling(SynapticScaling&&) noexcept;
    SynapticScaling& operator=(SynapticScaling&&) noexcept;
    
    // Configuration parameters
    void setScalingGain(float gain);
    void setTargetExcitationInhibitionRatio(float ratio);
    void setStructuralPlasticityEnabled(bool enabled);
    void setOptimizationInterval(float intervalMs);
    void setNetworkStabilityThreshold(float threshold);
    
    // Get current state
    float getScalingGain() const;
    float getCurrentEIRatio() const;
    float getNetworkStability() const;
    float getOptimizationProgress() const;
    
    // Update scaling mechanisms
    void update(float currentEIRatio, const std::vector<class Synapse*>& synapses,
                TimestepDuration dt);
    
    // Apply global synaptic scaling
    void applyGlobalScaling(const std::vector<class Synapse*>& synapses,
                           float scalingFactor, TimestepDuration dt);
    
    // Apply structural plasticity
    void applyStructuralPlasticity(class StructuralPlasticity* sp,
                                  float networkActivity, TimestepDuration dt);
    
    // Reorganize synaptic connections
    void reorganizeConnections(const std::vector<class Synapse*>& synapses,
                               const class Brain* brain, TimestepDuration dt);
    
    // Optimize network topology
    void optimizeNetworkTopology(const std::vector<class Synapse*>& synapses,
                                 TimestepDuration dt);
    
    // Reset scaling state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng);
    
    // Get name
    const char* getName() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Combined Advanced Plasticity Manager
// 
// Orchestrates all advanced plasticity mechanisms
// Provides unified interface for advanced plasticity control
// Integrates with existing plasticity systems (STDP, Hebbian, Structural)

class AdvancedPlasticity {
public:
    AdvancedPlasticity();
    ~AdvancedPlasticity();
    
    // Disable copying, enable moving
    AdvancedPlasticity(const AdvancedPlasticity&) = delete;
    AdvancedPlasticity& operator=(const AdvancedPlasticity&) = delete;
    AdvancedPlasticity(AdvancedPlasticity&&) noexcept;
    AdvancedPlasticity& operator=(AdvancedPlasticity&&) noexcept;
    
    // Configuration
    void enableHomeostaticPlasticity(bool enabled);
    void enableRewardModulatedPlasticity(bool enabled);
    void enableMetaPlasticity(bool enabled);
    void enableSynapticScaling(bool enabled);
    
    // Set parameters for each mechanism
    void setHomeostaticTarget(float targetHz);
    void setRewardLearningRate(float rate);
    void setMetaPlasticityLearningRate(float rate);
    void setSynapticScalingGain(float gain);
    
    // Update all plasticity mechanisms
    void update(float currentFiringRate, float reward, float predictionError,
                float novelty, float curiosity, TimestepDuration dt);
    
    // Apply advanced plasticity to synapses
    void applyToSynapse(class Synapse* synapse, float neuralActivity,
                       float neuromodulatorLevel, float rewardError,
                       TimestepDuration dt);
    
    // Apply advanced plasticity to brain
    void applyToBrain(class Brain* brain, TimestepDuration dt);
    
    // Get system state
    bool isHomeostaticPlasticityEnabled() const;
    bool isRewardModulatedPlasticityEnabled() const;
    bool isMetaPlasticityEnabled() const;
    bool isSynapticScalingEnabled() const;
    
    float getNetworkStability() const;
    float getLearningEfficiency() const;
    float getAdaptabilityScore() const;
    
    // Reset all mechanisms
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng);
    
    // Get name
    const char* getName() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
