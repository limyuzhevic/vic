#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Norepinephrine: Arousal and vigilance neuromodulator
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt, float novelty = 0.0f, 
                float predictionError = 0.0f, float reward = 0.0f) override;
    float getExcitabilityModulator() const override;
    float getAttentionModulator() const override;
    float getMemoryModulator() const override;
    
    // Arousal signaling
    void increaseArousal(float strength);
    
    // Stress response
    void signalStress(float intensity);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm