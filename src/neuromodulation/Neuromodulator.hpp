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
    
    // Initialize with brain reference
    virtual void initialize(class Brain* brain) {}
    
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
    
    // Get neuromodulatory effects on cognitive functions
    virtual float getAttentionModulator() const { return 1.0f; }
    virtual float getMemoryModulator() const { return 1.0f; }
    virtual float getWorkingMemoryModulator() const { return 1.0f; }
    virtual float getLearningModulator() const { return 1.0f; }
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Affects attention, memory consolidation, working memory, cognitive processes
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
    
    // Cognitive effects
    float getAttentionModulator() const;
    float getMemoryModulator() const;
    float getWorkingMemoryModulator() const;
    
    // Set parameters
    void setBaseline(float baseline);
    void setPeak(float peak);
    void setReleaseRate(float rate);
    void setAttentionGain(float gain);
    void setMemoryGain(float gain);
    void setWorkingMemoryGain(float gain);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Memory consolidation effects
    float getMemoryConsolidationRate() const;
    float getAttentionGain() const;
    float getLearningRate() const;
    
    // Set parameters
    void setBaseline(float baseline);
    void setPeak(float peak);
    void setDecayRate(float rate);
    void setMemoryGain(float gain);
    void setAttentionStrength(float strength);
    void setLearningModulator(float modulator);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and vigilance effects
    float getArousalLevel() const;
    float getVigilanceLevel() const;
    float getStressLevel() const;
    
    // Set parameters
    void setBaseline(float baseline);
    void setPeak(float peak);
    void setDecayRate(float rate);
    void setArousalGain(float gain);
    void setVigilanceGain(float gain);
    void setStressModulator(float modulator);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood and behavioral effects
    float getMoodLevel() const;
    float getImpulsivityModulator() const;
    float getSocialBehaviorModulator() const;
    
    // Set parameters
    void setBaseline(float baseline);
    void setPeak(float peak);
    void setDecayRate(float rate);
    void setMoodGain(float gain);
    void setImpulsivityModulator(float modulator);
    void setSocialBehaviorGain(float gain);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
