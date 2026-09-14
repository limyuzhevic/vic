#pragma once

#include "Neuromodulator.hpp"
#include <boost/optional.hpp>
#include <memory>
#include <vector>
#include <algorithm>
#include <cmath>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Biologically plausible model based on locus coeruleus norepinephrine system
// Implementing stress response, alertness modulation, and vigilance control

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and vigilance control
    void increaseArousal(float amount);
    void decreaseArousal(float amount);
    void triggerVigilance(boost::optional<float> duration = boost::none);
    void resetVigilance();
    
    // Stress and adaptation responses
    void triggerStressResponse(float intensity);
    void adaptToStress(float adaptationRate);
    
    // State access
    float getArousalLevel() const;
    float getVigilanceLevel() const;
    float getLocusCoeruleusActivity() const;
    float getStressLevel() const;
    float getAdaptationLevel() const;
    float getSustainedAttention() const;
    
    // Enhanced neuromodulation effects
    void enhanceSensoryProcessing(std::vector<float>& sensoryInput, float gainFactor);
    void modulate alertness(float current arousal);
    void maintainVigilance(std::vector<float>& neuralActivity, float threshold);
    void resetNeuralGain(std::vector<float>& neuralActivity);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm