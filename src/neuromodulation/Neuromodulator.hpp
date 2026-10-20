#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Phase 6: Implemented real neuromodulation system with:
// - Dopamine: Reward prediction, motivation, attention
// - Acetylcholine: Attention enhancement, memory consolidation
// - Norepinephrine: Arousal, vigilance, fight-flight response
// - Serotonin: Mood regulation, impulsivity, social behavior
// - Novelty: Novelty detection for curiosity-driven exploration
// - Curiosity: Exploration motivation based on novelty and prediction error
// - PredictionError: Reward prediction error signaling
// - Reward: Real reward computation from environment and agent state

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns a factor that modulates plasticity rates (typically 0.1-2.0x)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state for current timestep
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
    void signalMotivation(float motivation);
    void signalAttention(float attention);
    
    // Get current state
    float getMotivationLevel() const;
    float getRewardSensitivity() const;
    
    // Configure parameters
    void setBaseline(float baseline);
    void setReleaseRate(float rate);
    void setDecayRate(float rate);
    
private:
    struct Impl;
    Impl* pImpl;
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
    void update(TimestepDuration dt) override;
    
    // Signaling methods
    void signalAttention(float attention);
    void signalMemory(float memoryStrength);
    
    // Get boost values
    float getAttentionBoost() const;
    float getMemoryBoost() const;
    
    // Configure parameters
    void setAttentionBoost(float boost);
    void setMemoryBoost(float boost);
    
private:
    struct Impl;
    Impl* pImpl;
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
    void update(TimestepDuration dt) override;
    
    // Signaling methods
    void signalArousal(float arousal);
    void signalVigilance(float vigilance);
    void signalFightFlight(float intensity);
    
    // Get state
    float getArousal() const;
    float getVigilance() const;
    float getFightFlightMod() const;
    float getAttentionGain() const;
    float getMemoryConsolidation() const;
    
    // Configure parameters
    void setArousal(float arousal);
    void setVigilance(float vigilance);
    void setFightFlightMod(float mod);
    void setAttentionGain(float gain);
    void setMemoryConsolidation(float consolidation);
    
private:
    struct Impl;
    Impl* pImpl;
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
    void update(TimestepDuration dt) override;
    
    // Signaling methods
    void signalMood(float mood);
    void signalSocial(float socialStrength);
    
    // Get state
    float getMoodEffect() const;
    float getImpulsivityMod() const;
    float getSocialBehaviorMod() const;
    
    // Configure parameters
    void setImpulsivityMod(float mod);
    void setSocialBehaviorMod(float mod);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Reward signal for reinforcement learning
class Reward : public Neuromodulator {
public:
    Reward();
    ~Reward() override;
    
    // Get current reward value
    float getValue() const;
    void setValue(float value);
    
    // Accumulate reward
    void add(float delta);
    
    // Reset accumulated reward
    void reset();
    
    // Compute reward from environment state
    float computeReward(const Observation& observation) const;
    
    // Compute reward from environment state and agent
    float computeReward(const Observation& observation, const AgentBody& agent) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
