#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// TODO PHASE 2
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal signaling
    void signalArousal(float strength);
    void increaseVigilance(float increase);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm