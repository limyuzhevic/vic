#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

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

// Dopamine: Reward and reinforcement learning signal
// PLACEHOLDER - Phase 2
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
    Impl* pImpl;
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
    
    // Attentional signaling
    void signalAttention(float attentionStrength, float memoryPriority);
    
    // Memory consolidation
    void consolidateMemory(const std::vector<float>& memoryPattern, float strength);
    
    // Getters for specialized functions
    float getAttentionScore() const;
    float getMemoryPriority() const;
    const std::vector<float>& getMemoryTrace() const;
    
private:
    struct Impl;
    Impl* pImpl;
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
    
    // Arousal signaling
    void signalArousal(float intensity, float novelty);
    
    // Stress response
    void signalStress(float stressor, float duration);
    
    // Getters for specialized functions
    float getArousalLevel() const;
    float getVigilanceScore() const;
    float getStressLevel() const;
    float getAlertness() const;
    float getExplorationBias() const;
    
private:
    struct Impl;
    Impl* pImpl;
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
    
    // Reward signaling (social and non-social)
    void signalReward(float reward, float socialContext);
    
    // Social behavior signaling
    void signalSocialEvent(float socialValue, float groupCohesion);
    
    // Stress response
    void signalStress(float stressLevel);
    
    // Getters for specialized functions
    float getMoodScore() const;
    float getSatietyLevel() const;
    float getImpulsivity() const;
    float getSocialCohesion() const;
    float getAggression() const;
    
    const std::vector<float>& getMoodHistory() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
