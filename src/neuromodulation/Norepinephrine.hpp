#pragma once

#include "../brain/Brain.hpp"
#include "../core/Types/Types.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
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
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Enhance neural excitability and processing
    void enhanceExcitability(float amount);
    void processNovelInformation(const Observation& observation);
    
    // Get arousal and novelty sensitivity modifiers
    float getArousalLevel() const;
    float getNoveltySensitivity() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm