#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Acetylcholine: Attention and memory consolidation
// Enhanced with cholinergic dynamics, working memory modulation, and cortical excitability

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getBaselineLevel() const override;
    void setTargetLevel(float target) override;
    bool isActive() const override;
    void reset() override;
    
    // Attention modulation
    float getAttentionLevel() const;
    void setAttentionLevel(float level);
    
    // Working memory enhancement
    void enhanceWorkingMemory(float boostFactor);
    float getWorkingMemoryModulation() const;
    
    // Cortical excitability
    float getCorticalExcitability() const;
    void setCorticalExcitability(float factor);
    
    // Memory consolidation
    void consolidateMemory(float consolidationStrength);
    float getMemoryConsolidationFactor() const;
    
    // Prefrontal cortex modulation
    float getPFCModulation() const;
    void setPFCModulation(float modulation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
