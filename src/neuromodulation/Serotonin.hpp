#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior neuromodulator
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt, float novelty = 0.0f, 
                float predictionError = 0.0f, float reward = 0.0f) override;
    float getExcitabilityModulator() const override;
    float getAttentionModulator() const override;
    float getMemoryModulator() const override;
    
    // Mood regulation
    void regulateMood(float moodLevel);
    
    // Impulse control
    void controlImpulsivity(float strength);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm