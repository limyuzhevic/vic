#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include <string>
#include <cmath>

namespace nlm {

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
    void modulateAttention(float attentionStrength);
    
    // Memory consolidation
    void consolidateWorkingMemory(const Brain* brain);
    void enhanceSensoryMemoryEncoding(const Brain* brain);
    
    // Learning modulation
    void facilitateLongTermPotentiation(const Brain* brain);
    void modulateLearningRate(float baseRate, float novelty, float reward);
    
    // State management
    void reset() override;
    float getAttentionModulation() const;
    float getMemoryConsolidationStrength() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm