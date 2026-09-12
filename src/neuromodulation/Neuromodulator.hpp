#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Represents biological neuromodulators that regulate brain function

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (0.0 to 1.0)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to neural excitability
    // Higher level = more modulation effect
    virtual float getNeuralModulationFactor() const = 0;
    
    // Apply neuromodulatory effect on plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on brain activity
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Mediates reward prediction error and modulates neural plasticity

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getNeuralModulationFactor() const override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get prediction error component
    float getPredictionError() const;
    
    // Set reward prediction target
    void setPredictionTarget(float target);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Modulates attention strength and facilitates memory consolidation

class Acetylcholine : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = level; }
    float getNeuralModulationFactor() const override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
private:
    float level_ = 0.0f;
};

// Norepinephrine: Arousal and vigilance
// Modulates neural arousal and response gain

class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = level; }
    float getNeuralModulationFactor() const override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // NE enhances attention and alertness
    void increaseArousal(float amount);
    void decreaseArousal(float amount);
    
private:
    float level_ = 0.0f;
};

// Serotonin: Mood, impulsivity, and social behavior
// Regulates mood and social behavior

class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = level; }
    float getNeuralModulationFactor() const override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // 5-HT modulates impulsivity and patience
    void increasePatience(float amount);
    void decreasePatience(float amount);
    
private:
    float level_ = 0.0f;
};

} // namespace nlm
