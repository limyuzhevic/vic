#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// 
// Neuromodulators regulate neural excitability and plasticity to:
// - Modulate attention and working memory stability
// - Control memory consolidation and retrieval
// - Adjust exploration vs exploitation balance
// - Influence development and maturation processes

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns factor that multiplies base plasticity rates
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state (called each simulation step)
    virtual void update(TimestepDuration dt) = 0;
    
    // Get attention modulation level (for attention systems)
    virtual float getAttentionModulation() const { return 1.0f; }
    
    // Get memory consolidation signal (for memory systems)
    virtual float getMemoryConsolidationSignal() const { return 0.0f; }
    
    // Get exploration motivation (for curiosity systems)
    virtual float getExplorationMotivation() const { return 0.0f; }
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Modulates: Learning rates, plasticity, motivation, attention

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
    
    // Get attention modulation based on reward prediction
    float getAttentionModulation() const override;
    
    // Get memory consolidation signal for rewarding events
    float getMemoryConsolidationSignal() const override;
    
    // Get exploration motivation based on prediction error
    float getExplorationMotivation() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Modulates: Working memory, attention, memory encoding, plasticity

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
    void setAttentionLevel(float level) { attentionLevel_ = level; }
    float getAttentionLevel() const { return attentionLevel_; }
    
    // Memory encoding
    void incrementEncodingScore(float score);
    float getEncodingScore() const { return encodingScore_; }
    
    // Working memory stability
    void stabilizeWorkingMemory(float stability);
    
    // Get attention modulation
    float getAttentionModulation() const override;
    
    // Get memory consolidation signal
    float getMemoryConsolidationSignal() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    float attentionLevel_;
    float encodingScore_;
};

// Norepinephrine: Arousal and vigilance
// Modulates: Alertness, response to novel stimuli, cognitive flexibility

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal level
    void setArousalLevel(float level) { arousalLevel_ = level; }
    float getArousalLevel() const { return arousalLevel_; }
    
    // Novelty sensitivity
    void setNoveltySensitivity(float sensitivity) { noveltySensitivity_ = sensitivity; }
    float getNoveltySensitivity() const { return noveltySensitivity_; }
    
    // Cognitive flexibility
    void incrementCognitiveFlexibility(float amount);
    float getCognitiveFlexibility() const { return cognitiveFlexibility_; }
    
    // Get exploration motivation based on arousal
    float getExplorationMotivation() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    float arousalLevel_;
    float noveltySensitivity_;
    float cognitiveFlexibility_;
};

// Serotonin: Mood, impulsivity, and social behavior
// Modulates: Behavioral flexibility, social behavior, mood effects on plasticity

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood level
    void setMoodLevel(float level) { moodLevel_ = level; }
    float getMoodLevel() const { return moodLevel_; }
    
    // Impulsivity
    void setImpulsivity(float level) { impulsivity_ = level; }
    float getImpulsivity() const { return impulsivity_; }
    
    // Social behavior strength
    void setSocialBehaviorStrength(float strength) { socialStrength_ = strength; }
    float getSocialBehaviorStrength() const { return socialStrength_; }
    
    // Get exploration motivation based on mood
    float getExplorationMotivation() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    float moodLevel_;
    float impulsivity_;
    float socialStrength_;
};

} // namespace nlm
