#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

/**
 * @class Acetylcholine
 * @brief Attention and memory consolidation neuromodulator
 * 
 * Acetylcholine (ACh) modulates attention, arousal, and memory consolidation.
 * It enhances neural responses to salient stimuli and facilitates learning
 * through cholinergic mechanisms.
 * 
 * @details Uses PImpl idiom for forward declaration to avoid circular
 * dependencies. Provides attention modulation capabilities.
 */
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
};

} // namespace nlm
