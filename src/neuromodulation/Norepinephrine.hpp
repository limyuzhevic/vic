#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

/**
 * @class Norepinephrine
 * @brief Arousal and vigilance neuromodulator
 * 
 * Norepinephrine (NE) regulates arousal, attention, and stress responses.
 * It enhances signal-to-noise ratios in neural processing and modulates
 * cognitive functions including working memory and executive function.
 * 
 * @details Uses PImpl idiom for forward declaration to avoid circular
 * dependencies. Provides arousal modulation capabilities.
 */
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
};

} // namespace nlm
