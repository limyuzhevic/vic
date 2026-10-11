#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

struct Norepinephrine::Impl;

class Norepinephrine : public Neuromodulator {
    struct Impl;
    Impl* pImpl;
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    void reset() override;
    
    // Increase arousal and alertness
    void increaseArousal(float level);
    void enhanceSensoryProcessing(float enhancement);
    void setFocusLevel(float level);
};

} // namespace nlm