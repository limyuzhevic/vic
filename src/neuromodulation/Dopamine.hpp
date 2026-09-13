#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/ConceptFormation.hpp"
#include <vector>
#include <memory>
#include <functional>
#include <string>
#include <algorithm>

namespace nlm {

// Dopamine: Enhanced with full neuromodulation capabilities
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Enhanced dopamine functions for full integration
    void enhanceWorkingMemory(NeuralWorkingMemory* workingMemory, float factor = 1.0f);
    void enhanceEpisodicMemory(NeuralEpisodicMemory* episodicMemory, float consolidation = 1.0f);
    void modulateAttention(class AttentionalSelection* attention, float bias = 1.0f);
    void modulateConceptFormation(ConceptFormation* conceptFormation, float threshold = 1.0f);
    
    // Get enhanced modulation factors
    float getMemoryConsolidationFactor() const;
    float getAttentionalModulation() const;
    float getConceptModulation() const;
    
    // Reward prediction interaction
    void interactWithRewardPrediction(float predictionError, float expectedValue);
    
    // Integration with other neuromodulators
    void integrateWithCuriosity(float curiosityLevel);
    void integrateWithNovelty(float noveltyLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm