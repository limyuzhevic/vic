#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>
#include <deque>

namespace nlm {

// NeuromodulatorState: Tracks phasic vs tonic activity for neuromodulators
struct NeuromodulatorState {
    float tonicLevel;           // Background, baseline level
    float phasicLevel;          // Transient bursts in response to events
    float recentPhasicActivity; // Recent phasic activity for integration
    float adaptiveBaseline;     // Adapts over time based on average activity
    float baselineAdaptation;  // Rate of baseline adaptation
    float phasicAdaptation;     // Rate of phasic adaptation
    
    NeuromodulatorState()
        : tonicLevel(0.0f), phasicLevel(0.0f), recentPhasicActivity(0.0f),
          adaptiveBaseline(0.0f), baselineAdaptation(0.1f), phasicAdaptation(1.0f) {}
};

// Neuromodulator: Abstract base for neuromodulatory signals with phasic/tonic dynamics
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Get phasic and tonic components separately
    virtual float getPhasicLevel() const = 0;
    virtual float getTonicLevel() const = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state with phasic/tonic dynamics
    virtual void update(TimestepDuration dt) = 0;
    
    // Phasic response to events
    virtual void phasicResponse(float magnitude) = 0;
    
    // Get current state for monitoring
    virtual const NeuromodulatorState& getState() const = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward prediction error signal with phasic/tonic dynamics
// Implements real dopamine signaling based on reward and prediction error
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPhasicLevel() const override;
    float getTonicLevel() const override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void phasicResponse(float magnitude) override;
    const NeuromodulatorState& getState() const override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error, float predictedReward);
    
    // Get prediction error signal
    float getPredictionError() const { return predictionError_; }
    void setPredictionError(float error) { predictionError_ = error; }
    
    // Configure dopamine dynamics
    void setPhasicResponseRate(float rate) { pImpl->phasicResponseRate_ = rate; }
    void setTonicDecayRate(float rate) { pImpl->tonicDecayRate_ = rate; }
    void setBaselineAdaptation(float rate) { pImpl->state_.baselineAdaptation = rate; }
    void setPhasicAdaptation(float rate) { pImpl->state_.phasicAdaptation = rate; }
    
    // Reset to baseline
    void reset() { pImpl->state_.phasicLevel = 0.0f; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    NeuromodulatorState state_;
    float predictionError_;
    std::deque<float> phasicHistory_;
    static constexpr size_t PHASIC_HISTORY_SIZE = 10;
};

// Acetylcholine: Attention and memory consolidation signal
// Mediates attention, working memory, and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPhasicLevel() const override;
    float getTonicLevel() const override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void phasicResponse(float magnitude) override;
    const NeuromodulatorState& getState() const override;
    
    // Attention modulation
    void increaseAttention(float amount);
    void decreaseAttention(float amount);
    
    // Memory consolidation
    void triggerMemoryConsolidation();
    
    // Set attention parameters
    void setAttentionSensitivity(float sensitivity) { pImpl->attentionSensitivity_ = sensitivity; }
    void setMemoryConsolidationRate(float rate) { pImpl->memoryConsolidationRate_ = rate; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    NeuromodulatorState state_;
    std::deque<float> attentionHistory_;
    static constexpr size_t ATTENTION_HISTORY_SIZE = 5;
};

// Norepinephrine: Arousal and vigilance signal
// Modulates arousal, vigilance, and stress response
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPhasicLevel() const override;
    float getTonicLevel() const override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void phasicResponse(float magnitude) override;
    const NeuromodulatorState& getState() const override;
    
    // Arousal modulation
    void increaseArousal(float amount);
    void decreaseArousal(float amount);
    
    // Stress response
    void triggerStressResponse(float intensity);
    
    // Set arousal parameters
    void setArousalSensitivity(float sensitivity) { pImpl->arousalSensitivity_ = sensitivity; }
    void setStressSensitivity(float sensitivity) { pImpl->stressSensitivity_ = sensitivity; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    NeuromodulatorState state_;
    std::deque<float> arousalHistory_;
    static constexpr size_t AROUSAL_HISTORY_SIZE = 5;
};

// Serotonin: Mood, impulsivity, and social behavior signal
// Regulates mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPhasicLevel() const override;
    float getTonicLevel() const override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void phasicResponse(float magnitude) override;
    const NeuromodulatorState& getState() const override;
    
    // Mood modulation
    void increaseMood(float amount);
    void decreaseMood(float amount);
    
    // Impulse control
    void increaseImpulseControl(float amount);
    void decreaseImpulseControl(float amount);
    
    // Social behavior modulation
    void increaseSocialBehavior(float amount);
    void decreaseSocialBehavior(float amount);
    
    // Set serotonin parameters
    void setMoodSensitivity(float sensitivity) { pImpl->moodSensitivity_ = sensitivity; }
    void setImpulseControlSensitivity(float sensitivity) { pImpl->impulseControlSensitivity_ = sensitivity; }
    void setSocialBehaviorSensitivity(float sensitivity) { pImpl->socialBehaviorSensitivity_ = sensitivity; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    NeuromodulatorState state_;
    std::deque<float> moodHistory_;
    static constexpr size_t MOOD_HISTORY_SIZE = 5;
};

} // namespace nlm
