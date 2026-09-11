#pragma once

#include "../core/Types/Types.hpp"
#include "Neuromodulator.hpp"

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
    void reset() override;
    
    // Arousal and vigilance modulation
    void increaseArousal(float arousalLevel);
    float getArousal() const { return arousal_; }
    
    // Signal-to-noise ratio enhancement
    float enhanceSignalToNoiseRatio(float baseSignal, float noise) const;
    void enhanceLearning(float learningRate) { learningRate_ = learningRate; }
    
    // Apply noradrenergic effects
    void applyToBrain(Brain* brain) const override;
    
private:
    struct Impl;
    Impl* pImpl;
    float arousal_;
    float learningRate_;
    std::vector<float> signalNoiseHistory_;
};

} // namespace nlm