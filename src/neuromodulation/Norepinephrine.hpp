#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// NE signals novel/stimulating inputs and maintains alertness
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    // Disable copying
    Norepinephrine(const Norepinephrine&) = delete;
    Norepinephrine& operator=(const Norepinephrine&) = delete;
    
    // Disable moving
    Norepinephrine(Norepinephrine&&) = delete;
    Norepinephrine& operator=(Norepinephrine&&) = delete;
    
    // Get modulator name
    const char* getName() const override;
    
    // Get current concentration/level
    float getLevel() const override;
    void setLevel(float level) override;
    
    // Get NE-specific plasticity factor
    float getPlasticityFactor() const override;
    
    // Update neuromodulator state
    void update(TimestepDuration dt) override;
    
    // Signal novel or intense stimulus
    void signalNovelStimulus(float intensity);
    
    // Signal urgent alert
    void signalAlert(float urgency);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm