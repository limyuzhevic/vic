#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Real implementation based on computational neuroscience principles

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Real implementation: Different neuromodulators affect plasticity differently
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    // Real implementation: Real neuromodulators have dynamics, timescales, and interactions
    virtual void update(TimestepDuration dt) = 0;
    
    // Get signal history for analysis
    virtual const std::vector<float>& getHistory() const { return history_; }
    virtual void clearHistory() { history_.clear(); }
    
    // Check if neuromodulator is active
    virtual bool isActive() const { return getLevel() > 0.0f; }
    
    // Get response time constant (time to reach new level)
    virtual float getResponseTime() const { return responseTime_; }
    virtual void setResponseTime(float time) { responseTime_ = std::max(0.01f, time); }
    
protected:
    Neuromodulator() : responseTime_(1.0f) {}
    
    // Add a sample to history (should be called by derived classes)
    void addSample(float level) {
        history_.push_back(level);
        // Keep history manageable (last 1000 samples)
        if (history_.size() > 1000) {
            history_.erase(history_.begin());
        }
    }
    
private:
    std::vector<float> history_;
    float responseTime_;
};

// Dopamine: Reward and reinforcement learning signal
// Real implementation based on computational reward models
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling mechanisms
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    void signalUnexpectedOutcome(float outcome);
    
    // Learning parameter modulation
    void setLearningRateModulation(float modulation) { learningRateMod_ = std::clamp(modulation, 0.0f, 2.0f); }
    float getLearningRateModulation() const { return learningRateMod_; }
    
    // Phasic vs tonic components
    void setPhasicComponent(bool enable) { phasicEnabled_ = enable; }
    bool isPhasicEnabled() const { return phasicEnabled_; }
    
    // Sustained response to rewards
    void setSustainedResponse(float duration) { sustainedDuration_ = std::max(0.0f, duration); }
    
private:
    struct Impl;
    Impl* pImpl;
    
    float learningRateMod_;
    bool phasicEnabled_;
    float sustainedDuration_;
};

// Acetylcholine: Attention and memory consolidation
// Real implementation based on attentional gain modulation
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
    void increaseAttention(float amount);
    void decreaseAttention(float amount);
    bool isAttentionElevated() const;
    
    // Memory effects
    float getMemoryBoost() const;
    float getMemoryDecayRate() const;
    
    // Cognitive state effects
    void setCognitiveLoad(float load);
    float getCognitiveLoad() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Novelty: Change detection and exploratory behavior
// Real implementation based on novelty detection systems
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Novelty detection
    void detectNovelty(const std::vector<float>& input);
    void resetNoveltyDetection();
    
    // Learning enhancement
    float getLearningEnhancement() const;
    float getExplorationBias() const;
    
    // Surprise signals
    bool isSurprise() const;
    void recordSurprise(float surpriseValue);
    
private:
    struct Impl;
    Impl* pImpl;
};

// PredictronError: Prediction error signals for learning
// Real implementation based on temporal difference learning
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Learning signals
    void signalPredictionError(float error, float confidence);
    void setExpectedValue(float expected);
    float getExpectedValue() const;
    
    // Temporal difference learning
    void integratePredictionError(float value, float discount = 0.9f);
    
    // Error accumulation for learning
    float getErrorAccumulator() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Curiosity: Intrinsic motivation and exploration drive
// Real implementation based on curiosity-driven learning
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Exploration signals
    void increaseExploration(float amount);
    void decreaseExploration(float amount);
    bool isExplorationMode() const;
    
    // Intrinsic motivation
    float getIntrinsicMotivation() const;
    float getNoveltySensitivity() const;
    
    // Goal-directed exploration
    void setTargetStriving(float striving);
    float getTargetStriving() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return 0.0f; }
    void setLevel(float level) override {}
    float getPlasticityFactor() const override { return 1.0f; }
    void update(TimestepDuration dt) override {}
};

} // namespace nlm
