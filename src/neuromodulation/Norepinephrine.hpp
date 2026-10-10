#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// TODO PHASE 2: Implement real neuromodulation
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal signaling
    void signalNovelty(float noveltySignal);
    void signalArousal(float arousalLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm