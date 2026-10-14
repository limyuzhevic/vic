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
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return pImpl->level; }
    void setLevel(float level) override { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return pImpl->plasticityFactor; }
    void update(TimestepDuration dt) override;
    
    // Attention modulation
    float getAttentionBoost() const { return pImpl->attentionBoost; }
    
    // Memory consolidation
    float getMemoryConsolidationBoost() const { return pImpl->memoryConsolidationBoost; }
    
    // Reward prediction modulation
    void signalReward(float reward) { pImpl->level = std::min(pImpl->peak, pImpl->level + reward * pImpl->releaseRate); }
    
    // Reward prediction error
    void signalRewardPredictionError(float error) { pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate); }
    
private:
    struct Impl {
        float level;
        float baseline;
        float peak;
        float decayRate;
        float releaseRate;
        float plasticityFactor;
        float attentionBoost;
        float memoryConsolidationBoost;
        
        Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
                 plasticityFactor(1.0f), attentionBoost(0.0f), memoryConsolidationBoost(0.0f) {}
    };
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return pImpl->level; }
    void setLevel(float level) override { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return pImpl->plasticityFactor; }
    void update(TimestepDuration dt) override;
    
    // Arousal level
    float getArousalLevel() const { return pImpl->arousalLevel; }
    
    // Vigilance
    float getVigilance() const { return pImpl->vigilance; }
    
    // Stress response
    void signalStress(float stress) { pImpl->level = std::min(pImpl->peak, pImpl->level + stress * pImpl->releaseRate); }
    
    // Reward prediction error
    void signalRewardPredictionError(float error) { pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate); }
    
private:
    struct Impl {
        float level;
        float baseline;
        float peak;
        float decayRate;
        float releaseRate;
        float plasticityFactor;
        float arousalLevel;
        float vigilance;
        
        Impl() : level(0.0f), baseline(0.0f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
                 plasticityFactor(1.0f), arousalLevel(0.0f), vigilance(0.5f) {}
    };
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return pImpl->level; }
    void setLevel(float level) override { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return pImpl->plasticityFactor; }
    void update(TimestepDuration dt) override;
    
    // Mood state
    float getMoodLevel() const { return pImpl->moodLevel; }
    
    // Impulsivity
    float getImpulsivity() const { return pImpl->impulsivity; }
    
    // Social behavior
    float getSocialMotivator() const { return pImpl->socialMotivator; }
    
    // Reward prediction error
    void signalRewardPredictionError(float error) { pImpl->level = std::max(0.0f, pImpl->level + error * pImpl->releaseRate); }
    
    // Punishment
    void signalPunishment(float punishment) { pImpl->level = std::max(0.0f, pImpl->level - punishment * pImpl->decayRate); }
    
private:
    struct Impl {
        float level;
        float baseline;
        float peak;
        float decayRate;
        float releaseRate;
        float plasticityFactor;
        float moodLevel;
        float impulsivity;
        float socialMotivator;
        
        Impl() : level(0.5f), baseline(0.5f), peak(1.0f), decayRate(0.1f), releaseRate(1.0f),
                 plasticityFactor(1.0f), moodLevel(0.5f), impulsivity(0.5f), socialMotivator(0.5f) {}
    };
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
