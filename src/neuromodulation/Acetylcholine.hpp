#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../cognition/ConceptFormation.hpp"
#include <vector>
#include <memory>
#include <algorithm>
#include <cmath>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention modulation
    void modulateAttention(class AttentionalSelection* attention, float bias = 1.0f);
    
    // Memory consolidation
    void enhanceMemoryConsolidation(NeuralEpisodicMemory* episodicMemory, float strength = 1.0f);
    void enhanceWorkingMemory(NeuralWorkingMemory* workingMemory, float factor = 1.0f);
    
    // Signal acetylcholine events
    void signalAttentionEvent();
    void signalMemoryEvent();
    
    // Integration with other systems
    void integrateWithDopamine(float dopamineLevel);
    void integrateWithPredictionError(float predictionError);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
