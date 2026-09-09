#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Implements biologically realistic neuromodulation with phase 6 integration

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (0.0 to 1.0 normalized)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns plasticity factor (1.0 = baseline plasticity)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on biological dynamics
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Implements phasic and tonic dopamine signaling with prediction error integration
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling (phasic burst)
    void signalReward(float reward);
    
    // Reward prediction error signaling (error-driven learning)
    void signalRewardPredictionError(float error);
    
    // Get baseline level for tonic signaling
    float getBaseline() const { return pImpl->baseline; }
    void setBaseline(float baseline) { pImpl->baseline = baseline; }
    
    // Check if dopamine is above threshold for learning
    bool isLearningThreshold() const { return pImpl->level > 0.3f; }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Implements attentional modulation and memory strengthening
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override { return pImpl->level; }
    void setLevel(float level) override { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return 0.8f + 0.4f * pImpl->level; }
    void update(TimestepDuration dt) override;
    
    // Attention modulation (enhances working memory)
    void signalAttention(float attentionLevel);
    
    // Memory consolidation (strengthens episodic memory)
    void consolidateMemory(float consolidationStrength);
    
    // Get attention-related statistics
    float getAttentionLevel() const { return pImpl->level; }
    const std::vector<float>& getAttentionHistory() const { return pImpl->history; }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Implements alertness and behavioral activation
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override { return pImpl->level; }
    void setLevel(float level) override { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return 0.5f + 0.8f * pImpl->level; }
    void update(TimestepDuration dt) override;
    
    // Arousal signaling (increases alertness)
    void signalArousal(float arousalLevel);
    
    // Vigilance modulation (improves stimulus discrimination)
    void signalVigilance(float vigilanceLevel);
    
    // Get arousal-related statistics
    float getArousalLevel() const { return pImpl->level; }
    const std::vector<float>& getArousalHistory() const { return pImpl->history; }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Implements mood regulation and behavioral inhibition
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override { return pImpl->level; }
    void setLevel(float level) override { pImpl->level = std::clamp(level, 0.0f, 1.0f); }
    float getPlasticityFactor() const override { return 1.2f - 0.8f * pImpl->level; }
    void update(TimestepDuration dt) override;
    
    // Mood signaling (affects motivation)
    void signalMood(float moodLevel);
    
    // Behavioral inhibition (reduces impulsive actions)
    void signalInhibition(float inhibitionLevel);
    
    // Get mood-related statistics
    float getMoodLevel() const { return pImpl->level; }
    const std::vector<float>& getMoodHistory() const { return pImpl->history; }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
