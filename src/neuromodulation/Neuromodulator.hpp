#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // TODO PHASE 2: Implement real modulation
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Forward declarations for all neuromodulators
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;
class Curiosity;
class Novelty;
class PredictionError;

// Neuromodulator: Dopamine - reward and reinforcement learning signal
// Real implementation with reward prediction error and temporal difference learning
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling with temporal difference learning
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get prediction error
    float getPredictionError() const;
    
    // Get expected value
    void setExpectedValue(float value);
    float getExpectedValue() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neuromodulator: Acetylcholine - attention and memory consolidation
// Real implementation - affects sensory processing and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal new attention to novel stimuli
    void signalAttention(float attentionStrength);
    
    // Boost memory encoding and consolidation
    void signalMemoryBoost(float boostStrength);
    
    // Get current attention modulation level
    float getAttentionModulation() const;
    
    // Get current memory boost level
    float getMemoryBoost() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Apply attention modulation to neural processing
    void applyAttentionModulation();
    
    // Apply memory boost effects
    void applyMemoryBoost(float boostStrength);
};

// Neuromodulator: Norepinephrine - arousal and vigilance
// Real implementation - affects arousal, attention, and stress response
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal arousal changes
    void signalArousal(float arousalLevel);
    
    // Signal stress response
    void signalStress(float stressLevel);
    
    // Get current arousal level
    float getArousalLevel() const;
    
    // Get stress level
    float getStressLevel() const;
    
    // Get vigilance level
    float getVigilanceLevel() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Update internal neuromodulation state
    void updateInternalState();
    
    // Update neural excitability
    void updateNeuralExcitability(float arousalLevel);
    
    // Update memory encoding effects
    void updateMemoryEncoding(float stressLevel);
};

// Neuromodulator: Serotonin - mood, impulsivity, and social behavior
// Real implementation - affects mood regulation and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal mood changes
    void signalMood(float moodLevel);
    
    // Signal social status changes
    void signalSocialStatus(float socialStatus);
    
    // Get current mood level
    float getMoodLevel() const;
    
    // Get social status
    float getSocialStatus() const;
    
    // Get impulsivity level
    float getImpulsivityLevel() const;
    
    // Get reward sensitivity
    float getRewardSensitivity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Update internal neuromodulation state
    void updateInternalState();
    
    // Update social processing effects
    void updateSocialProcessing(float moodLevel);
    
    // Update reward sensitivity effects
    void updateRewardSensitivity(float socialStatus);
};

// Acetylcholine: Attention and memory consolidation (stub - to be replaced)
class AcetylcholineStub : public Neuromodulator {
public:
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Norepinephrine: Arousal and vigilance (stub - to be replaced)
class NorepinephrineStub : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Serotonin: Mood, impulsivity, and social behavior (stub - to be replaced)
class SerotoninStub : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

} // namespace nlm