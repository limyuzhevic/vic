#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <cmath>

namespace nlm {

/**
 * Neuromodulator: Advanced neuromodulatory system
 * Real implementation with sophisticated dynamics and plasticity integration
 */
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (0.0 to 1.0)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Real implementation with sophisticated modulation rules
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state with real dynamics
    virtual void update(TimestepDuration dt, const Neuromodulator* coModulator = nullptr, 
                      float predictionError = 0.0f) = 0;
    
    // Get learning rate based on current level
    virtual float getLearningRate() const = 0;
    
    // Get attention modulation factor
    virtual float getAttentionModulation() const = 0;
    
    // Get memory consolidation strength
    virtual float getMemoryConsolidationStrength() const = 0;
    
protected:
    Neuromodulator() = default;
};

/**
 * Dopamine: Advanced reward and reinforcement learning system
 * Real implementation with expectation-based learning and prediction error signaling
 */
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt, const Neuromodulator* coModulator = nullptr, 
                float predictionError = 0.0f) override;
    
    // Advanced reward signaling with eligibility traces
    void signalReward(float reward, float prediction = 0.0f);
    void signalRewardPredictionError(float error);
    void signalUncertainty(float uncertainty);
    
    // Set reward expectations and temporal difference learning
    void setExpectedReward(float reward);
    void updateExpectedReward(float actualReward, float discount = 0.9f);
    
    // Get reward-related metrics
    float getExpectedReward() const;
    float getPredictionError() const;
    float getTemporalDifference() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * Acetylcholine: Advanced attention and memory consolidation system
 * Real implementation with working memory modulation and attention control
 */
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt, const Neuromodulator* coModulator = nullptr, 
                float predictionError = 0.0f) override;
    
    // Attention control for working memory
    void signalAttention(float attentionStrength);
    void signalWorkingMemoryLoad(float load);
    void signalMemoryUpdate(const std::vector<float>& memoryTraces);
    
    // Get attention-related metrics
    float getAttentionStrength() const;
    float getWorkingMemoryCapacity() const;
    float getMemoryStability() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * Norepinephrine: Advanced arousal and vigilance system
 * Real implementation with adaptive stress response and cognitive control
 */
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt, const Neuromodulator* coModulator = nullptr, 
                float predictionError = 0.0f) override;
    
    // Arousal and vigilance control
    void signalStress(float stressLevel);
    void signalCognitiveLoad(float load);
    void signalPerformanceFeedback(float performance);
    
    // Get vigilance-related metrics
    float getVigilanceLevel() const;
    float getStressLevel() const;
    float getCognitiveControl() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

/**
 * Serotonin: Advanced mood and behavioral regulation system
 * Real implementation with behavioral flexibility and social behavior control
 */
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt, const Neuromodulator* coModulator = nullptr, 
                float predictionError = 0.0f) override;
    
    // Behavioral regulation
    void signalMood(float mood);
    void signalSocialContext(float socialFactor);
    void signalBehavioralFlexibility(float flexibility);
    
    // Get behavioral metrics
    float getMoodLevel() const;
    float getSocialBehaviorStrength() const;
    float getBehavioralFlexibility() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

