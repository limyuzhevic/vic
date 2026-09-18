#pragma once

#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <string>

namespace nlm {

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Enhance attention and memory functions
    void enhanceAttention(float amount);
    void boostMemoryConsolidation();
    
    // Get signal-to-noise ratio and memory boost modifiers
    float getSignalToNoiseRatio() const;
    float getWorkingMemoryBoost() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm