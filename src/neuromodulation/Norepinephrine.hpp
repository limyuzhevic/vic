#pragma once

#include "../core/Types/Types.hpp"
#include <algorithm>

namespace nlm {

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and alertness
    void increaseArousal(float intensity);
    void enhanceVigilance(float focus);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
