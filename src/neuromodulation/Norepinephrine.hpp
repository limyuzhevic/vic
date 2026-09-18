#pragma once

#include "../core/Types/Types.hpp"

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
    void signalEvent(float magnitude = 1.0f) override;
    void reset() override;
    float getBaselineLevel() const override;
    float getPeakLevel() const override;
    
    // Arousal modulation
    void signalStress(float stressLevel);
    void signalNovelty(float noveltyLevel);
    
    // Cognitive enhancement
    void enhanceSignalToNoiseRatio(float enhancement);
    void modulateLearningRate(float rateChange);
    
    // Get arousal-related statistics
    float getArousalLevel() const;
    float getVigilanceLevel() const;
    float getStressLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
