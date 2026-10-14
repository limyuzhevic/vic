#pragma once

#include "../core/Types/Types.hpp"
#include <unordered_map>

namespace nlm {

// Serotonin: Mood, impulsivity, and decision making
// Implements mood-related plasticity and learning rate modulation

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get 5-HT level
    float getLevel() const;
    void setLevel(float level);
    
    // Apply reward processing modulation
    void modulateRewardProcessing(float rewardValue, float* learningRate);
    
    // Apply impulsivity control
    void modulateImpulsivity(float decisionUrgency, float* patienceThreshold);
    
    // Apply mood-related plasticity effects
    void applyMoodRelatedPlasticity(NeuronId source, NeuronId target, 
                                   SynapticWeight& weight, float rewardHistory);
    
    // Affect learning rates
    float getLearningRateModulation() const;
    float getPlasticityModulation() const;
    
    // Update serotonin system parameters
    void updateSerotoninParameters(float baseline, float rewardGain, float moodSensitivity);
    
    // Reset
    void reset();
    
    // Get serotonin enhancement factor for neurons
    float getSerotoninGain(NeuronId neuron) const;
    
    // Apply mood effects to memory consolidation
    void applyMoodEffectsToMemory(NeuronId neuron, float moodState, float* consolidationStrength);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm