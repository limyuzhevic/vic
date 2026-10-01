#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Implements biologically plausible neuromodulation effects

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (0.0 to 1.0 normalized)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Get current plasticity factor (multiplier for plasticity rates)
    virtual float getPlasticityFactor() const = 0;
    
    // Get baseline level (homeostatic set point)
    virtual float getBaselineLevel() const = 0;
    
    // Get peak level (maximum achievable)
    virtual float getPeakLevel() const = 0;
    
    // Apply neuromodulatory effect to plasticity weights
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on neural activity and environment
    virtual void update(TimestepDuration dt, float predictionError = 0.0f, 
                       float reward = 0.0f, float novelty = 0.0f) = 0;
    
    // Check if neuromodulator is active (level above threshold)
    virtual bool isActive() const = 0;
    
    // Reset neuromodulator to baseline state
    virtual void reset() = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward prediction error, reinforcement learning signal
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    float getBaselineLevel() const override;
    float getPeakLevel() const override;
    bool isActive() const override;
    void reset() override;
    void update(TimestepDuration dt, float predictionError = 0.0f, 
                float reward = 0.0f, float novelty = 0.0f) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get prediction error (last computed)
    float getLastPredictionError() const;
    
    // Set learning rate for dopamine system
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Set time constant for dopamine dynamics
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
    // Enable/disable dopamine modulation
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention, learning rate modulation, memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    float getBaselineLevel() const override;
    float getPeakLevel() const override;
    bool isActive() const override;
    void reset() override;
    void update(TimestepDuration dt, float predictionError = 0.0f, 
                float reward = 0.0f, float novelty = 0.0f) override;
    
    // Attention signaling
    void signalAttention(float attention);
    void signalSensoryInput(float inputStrength);
    
    // Get attention level (derived from ACh level)
    float getAttentionLevel() const;
    
    // Set cholinergic time constants
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal, vigilance, locus coeruleus signal
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    float getBaselineLevel() const override;
    float getPeakLevel() const override;
    bool isActive() const override;
    void reset() override;
    void update(TimestepDuration dt, float predictionError = 0.0f, 
                float reward = 0.0f, float novelty = 0.0f) override;
    
    // Arousal signaling
    void signalArousal(float arousal);
    void signalStress(float stress);
    
    // Get arousal level (derived from NE level)
    float getArousalLevel() const;
    
    // Set norepinephrine dynamics parameters
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    void setAdaptationRate(float rate);
    float getAdaptationRate() const;
    
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity control, social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    float getBaselineLevel() const override;
    float getPeakLevel() const override;
    bool isActive() const override;
    void reset() override;
    void update(TimestepDuration dt, float predictionError = 0.0f, 
                float reward = 0.0f, float novelty = 0.0f) override;
    
    // Mood signaling
    void signalMood(float mood);
    void signalSocialContext(float context);
    
    // Get mood level (derived from 5-HT level)
    float getMoodLevel() const;
    
    // Get impulsivity level (inversely related to 5-HT)
    float getImpulsivityLevel() const;
    
    // Set serotonin dynamics parameters
    void setTimeConstant(float tau);
    float getTimeConstant() const;
    void setInhibitionFactor(float factor);
    float getInhibitionFactor() const;
    
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Neuromodulation manager for coordinating multiple neuromodulators
class NeuromodulationSystem {
public:
    NeuromodulationSystem();
    ~NeuromodulationSystem();
    
    // Add a neuromodulator to the system
    void addNeuromodulator(std::shared_ptr<Neuromodulator> neuromodulator);
    
    // Get a neuromodulator by name
    std::shared_ptr<Neuromodulator> getNeuromodulator(const std::string& name) const;
    
    // Get all neuromodulators
    const std::vector<std::shared_ptr<Neuromodulator>>& getAllNeuromodulators() const;
    
    // Update all neuromodulators based on neural activity
    void update(TimestepDuration dt, float predictionError = 0.0f, 
                float reward = 0.0f, float novelty = 0.0f);
    
    // Reset all neuromodulators
    void reset();
    
    // Calculate combined plasticity factor from all neuromodulators
    float getCombinedPlasticityFactor() const;
    
    // Set neuromodulation system enabled/disabled
    void setEnabled(bool enabled);
    bool isEnabled() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
