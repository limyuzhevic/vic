#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (0.0 to 1.0)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity (0.0 to 2.0, 1.0 = baseline)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on brain inputs
    virtual void update(TimestepDuration dt, float predictionError = 0.0f,
                      float reward = 0.0f, float novelty = 0.0f) = 0;
    
    // Reset neuromodulator to baseline state
    virtual void reset() = 0;
    
    // Get neuromodulator type
    virtual NeuromodulatorType getType() const = 0;
protected:
    Neuromodulator() = default;
};

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt, float predictionError = 0.0f,
               float reward = 0.0f, float novelty = 0.0f) override;
    void reset() override;
    NeuromodulatorType getType() const override { return NeuromodulatorType::Acetylcholine; }
    
    // Attention modulation
    void modulateAttention(float attentionLevel);
    float getAttentionLevel() const;
    
    // Memory consolidation effects
    void consolidateMemory(float consolidationStrength);
    float getMemoryConsolidation() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Dopamine: Reward and reinforcement learning signal
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override { return "DA"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt, float predictionError = 0.0f,
               float reward = 0.0f, float novelty = 0.0f) override;
    void reset() override;
    NeuromodulatorType getType() const override { return NeuromodulatorType::Dopamine; }
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void signalPredit(float prediction);
    
    // Learning rate modulation
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Burst firing mode
    bool isBurstMode() const;
    void setBurstMode(bool enable);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt, float predictionError = 0.0f,
               float reward = 0.0f, float novelty = 0.0f) override;
    void reset() override;
    NeuromodulatorType getType() const override { return NeuromodulatorType::Norepinephrine; }
    
    // Arousal and vigilance
    void increaseArousal(float amount);
    void decreaseArousal(float amount);
    float getArousalLevel() const;
    
    // Focus and attention
    void setFocusLevel(float focus);
    float getFocusLevel() const;
    
    // Stress response
    void triggerStress(float stressLevel);
    bool isStressed() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt, float predictionError = 0.0f,
               float reward = 0.0f, float novelty = 0.0f) override;
    void reset() override;
    NeuromodulatorType getType() const override { return NeuromodulatorType::Serotonin; }
    
    // Mood regulation
    void setMood(MoodType mood);
    MoodType getMood() const;
    
    // Impulse control
    void modulateImpulsivity(float level);
    float getImpulsivityLevel() const;
    
    // Social behavior
    void setSocialBehavior(float socialFactor);
    float getSocialBehavior() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
