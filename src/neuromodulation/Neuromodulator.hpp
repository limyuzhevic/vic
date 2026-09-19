#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Core implementation for Phase 6 integration

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

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
    
    // Configure from config
    virtual void configure(const class Config& config) {}
    
protected:
    Neuromodulator() = default;
    
    // Baseline levels and decay
    float baseline_ = 0.0f;
    float decayRate_ = 0.1f;
};

// Dopamine: Reward and reinforcement learning signal
// Core implementation for reward prediction error signaling
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void configure(const class Config& config) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Core implementation for attentional modulation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention signaling
    void signalAttention(float attention);
    void signalLearning(float learningSignal);
    void configure(const class Config& config) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
// Core implementation for arousal and stress response
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal signaling
    void signalArousal(float arousal);
    void signalAlertness(float alertness);
    void configure(const class Config& config) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Core implementation for mood modulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood signaling
    void signalMood(float mood);
    void signalImpulse(float impulse);
    void configure(const class Config& config) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Novelty: Novelty detection and surprise signaling
// Core implementation for detecting novel stimuli
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Novelty signaling
    void signalNovelty(float novelty);
    void configure(const class Config& config) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Curiosity: Exploration motivation and goal seeking
// Core implementation for exploration behavior
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Exploration signaling
    void signalNovelty(float novelty);
    void signalExplorationOpportunity(float opportunity);
    void setExplorationBias(float bias);
    float getExplorationBias() const;
    void configure(const class Config& config) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Prediction Error: Prediction error signaling
// Core implementation for prediction error-based learning
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Prediction error signaling
    void signalPredictionError(float error);
    void configure(const class Config& config) override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
