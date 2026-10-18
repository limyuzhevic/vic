#pragma once

#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"
#include "Neuromodulator.hpp"

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Full implementation with biological realism

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get modulator name
    const char* getName() const override;
    
    // Get current concentration/level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Apply neuromodulatory effect to plasticity
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
    // Acetylcholine-specific functions
    
    // Compute attentional gain based on task demands
    // Enhances signaling in attended neural populations
    void computeAttentionalGain(const std::vector<float>& taskDemands,
                               std::vector<float>& gainFactors);
    
    // Enhance working memory retention during high ACh
    void enhanceWorkingMemory(const std::vector<float>& memoryItems,
                             std::vector<float>& retention)
    
    // Boost plasticity during novelty learning
    void boostPlasticity(const std::vector<float>& noveltySignals,
                        const std::vector<float>& errorSignals,
                        float& plasticityModulator);
    
    // Modulate burst firing in attention-related neurons
    void modulateBurstFiring(const std::vector<float>& neuronActivity,
                             std::vector<float>& burstModulation);
    
    // Generate phasic response to novel stimuli
    void generatePhasicResponse(const std::vector<float>& stimulusFeatures,
                               const class Novelty& novelty);
    
    // Apply tonic background level
    void applyTonicLevel(float baselineLevel);
    
    // Decay between phasic events
    void applyDecay(float decayRate, TimestepDuration dt);
    
    // Integrate with other systems
    
    // Work with dopamine for reward-attention balance
    void integrateWithDopamine(const class Dopamine& dopamine,
                               float& attentionRewardBalance);
    
    // Work with novelty for exploration-exploitation
    void integrateWithNovelty(const class Novelty& novelty,
                              std::vector<float>& explorationExploitation);
    
    // Influence prediction system uncertainty
    void integrateWithPredictionSystem(const class PredictionSystem& prediction,
                                       float& predictionUncertainty);
    
    // Reset neuromodulator state
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm