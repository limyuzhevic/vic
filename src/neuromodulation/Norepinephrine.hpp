#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal modulation
    void signalArousal(float arousalLevel);
    void increaseVigilance(float intensity);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm