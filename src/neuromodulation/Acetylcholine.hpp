#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Acetylcholine: Attention and memory consolidation neuromodulator
// Implemented: Modulates attention and working memory consolidation

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced attention mechanism
    void increaseAttention(float amount);
    void decreaseAttention(float amount);
    float getAttentionStrength() const;
    
    // Memory effects
    void enhanceMemoryConsolidation(float strength);
    
    // Enhanced attention and memory integration
    void signalAttentionDemand(float demand);
    void signalMemoryLoad(float load);
    void signalLearningEvent(float strength);
    
    // Get internal states
    float getAttentionStrength() const;
    float getMemoryConsolidationStrength() const;
    float getWorkingMemoryBoost() const;
    float getSustainedAttention() const;
    float getCognitiveLoad() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm