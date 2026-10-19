#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Dopamine: Reward and reinforcement learning signal
// Enhanced with biological accuracy

class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Phasic and tonic components
    void signalPhasicBurst(float magnitude);
    void signalTonicBaseline(float baseline);
    float getPhasicComponent() const;
    float getTonicComponent() const;
    
    // Receptor subtypes (D1, D2)
    float getD1ReceptorEffect() const;
    float getD2ReceptorEffect() const;
    void setD1ReceptorSensitivity(float sensitivity);
    void setD2ReceptorSensitivity(float sensitivity);
    
    // Saturation and depletion effects
    void applySaturation();
    void applyDepletion(float depletionRate, TimestepDuration dt);
    bool isSaturated() const;
    float getDepletionState() const;
    
    // Asymmetric learning rate modulation
    float getLearningRateModulation(float predictionError) const;
    
    // Neuromodulator interaction methods
    NeuromodulatorInteraction getInteractionWith(const Neuromodulator& other) const;
    float getSynergyMultiplier(const Neuromodulator& other) const;
    float getAntagonismMultiplier(const Neuromodulator& other) const;
    
    // State tracking
    float getHistoryAverage(unsigned int window) const;
    float getPeakLevel() const;
    float getTimeAboveThreshold(float threshold) const;
    
    // Biological range management
    bool isWithinBiologicalRange(float level) const;
    float getMinLevel() const;
    float getMaxLevel() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm