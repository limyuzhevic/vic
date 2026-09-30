#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

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
    float getArousalLevel() const;
    void signalAlertness();
    void signalStress();
    
    // Learning enhancement
    float getLearningEnhancement() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm