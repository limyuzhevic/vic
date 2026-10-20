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
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention modulation
    void signalAttentionShift(float priority);
    
    // Memory consolidation
    void signalMemoryConsolidation(float consolidationSignal);
    
    // Get attention gain
    float getAttentionGain() const;
    
    // Get memory consolidation rate
    float getMemoryConsolidationRate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal modulation
    void signalArousal(float arousal);
    
    // Stress response
    void signalStress(float stressLevel);
    
    // Get arousal level
    float getArousalLevel() const;
    
    // Get focus width (high NE = narrow focus)
    float getFocusWidth() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood modulation
    void signalMoodChange(float moodDelta);
    
    // Social behavior
    void signalSocialFeedback(float socialValue);
    
    // Get mood level
    float getMoodLevel() const;
    
    // Get impulse control (high 5-HT = better control)
    float getImpulseControl() const;
    
    // Get social behavior strength
    float getSocialBehaviorStrength() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Curiosity: Exploration motivation
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Curiosity signaling
    void signalNovelty(float noveltyLevel);
    void signalPredictionError(float predictionError);
    
    // Get curiosity level
    float getCuriosityLevel() const;
    
    // Get exploration drive
    float getExplorationDrive() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Novelty: Novelty detection
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
    void signalSensoryNovelty(float novelty);
    
    // Get novelty level
    float getNoveltyLevel() const;
    
    // Get prediction confidence
    float getPredictionConfidence() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// PredictionError: Prediction error signal
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
    void signalError(float error);
    void signalPredictionConfidence(float confidence);
    
    // Get error level
    float getErrorLevel() const;
    
    // Get confidence level
    float getConfidenceLevel() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
