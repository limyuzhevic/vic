#pragma once

#include "../../../src/core/Types/Types.hpp"
#include "../../../src/core/Random/Random.hpp"
#include "../../../src/brain/Brain.hpp"
#include "../../../src/brain/Synapse.hpp"
#include <string>
#include <vector>

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// PLACEHOLDER - Phase 2

class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get dopamine level
    float getLevel() const;
    
    // Get plasticity modulation factor
    float getPlasticityFactor() const;
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get reward prediction error
    float getRewardPredictionError() const;
    
    // Update dopamine state
    void update(TimestepDuration dt);
    
    // Configure dopamine system parameters
    void configure(float baseline, float peak, float decayRate, float releaseRate);
    
    // Modulate neuronal excitability
    void modifyNeuronalExcitability(float factor);
    
    // Enhance plasticity
    void enhancePlasticity(float strength);
    
    // Modify attention
    void modifyAttention(float focus);
    
    // Signal reward
    void signalReward(float reward);
    
    // Signal prediction error
    void signalPredictionError(float error);
    
    // Enhance STDP
    void enhanceSTDP(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes, TimestepDuration dt);
    
    // Enhance Hebbian learning
    void enhanceHebbian(Synapse* synapse, const std::vector<Timestamp>& preSpikes,
                       const std::vector<Timestamp>& postSpikes, TimestepDuration dt);
    
    // Enhance structural plasticity
    void enhanceStructuralPlasticity(Brain* brain, RandomGenerator& rng, float activityThreshold);
    
    // Update prediction integration
    void updatePredictionIntegration(float predictionError, float reward);
    
    // Get enhanced neuromodulation factors
    float getEnhancedSTDPFactor() const;
    float getEnhancedHebbianFactor() const;
    float getEnhancedStructuralFactor() const;
    float getPredictionErrorWeight() const;
    float getRewardPredictionWeight() const;
    
    // Reset system
    void reset();
    
    // Get modulator name
    const char* getName() const;
    
    // Set level with clamping
    void setLevel(float level);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm