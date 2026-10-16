#pragma once

#include "Neuromodulator.hpp"
#include <vector>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

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
    void signalStress(float stressLevel);
    void enhanceMemoryConsolidation(const std::vector<float>& trace);
    void increaseVigilance(float baselineInhibition);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm