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
// ENHANCED - Phase 6: Full neuromodulation system
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
    void signalPredictionError(float error);
    
    // Configuration
    void setBaseline(float baseline);
    void setTonicLevel(float tonic);
    void setBurstDuration(float duration);
    
    // Integration modifiers
    void setWorkingMemoryModulation(float modulation);
    void setEpisodicMemoryModulation(float modulation);
    void setConceptFormationModulation(float modulation);
    void setAttentionModulation(float modulation);
    
    // Query methods
    bool isBurstMode() const;
    float getExcitabilityModulation() const;
    float getWorkingMemoryModulation() const;
    float getEpisodicMemoryModulation() const;
    float getConceptFormationModulation() const;
    float getAttentionModulation() const;
    float getPredictionErrorResponse() const;
    float getRewardResponse() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// ENHANCED - Phase 6: Full neuromodulation system
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signals
    void signalAttention(float attention);
    void signalMemory(float memory);
    
    // Configuration
    void setBaseline(float baseline);
    void setTonicLevel(float tonic);
    
    // Integration modifiers
    void setAttentionWeight(float weight);
    void setMemoryWeight(float weight);
    
    // Query methods
    bool isBurstMode() const;
    float getAttentionWeight() const;
    float getMemoryWeight() const;
    
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// ENHANCED - Phase 6: Full neuromodulation system
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signals
    void signalArousal(float arousal);
    void signalVigilance(float vigilance);
    
    // Configuration
    void setBaseline(float baseline);
    void setTonicLevel(float tonic);
    
    // Integration modifiers
    void setArousalWeight(float weight);
    void setVigilanceWeight(float weight);
    
    // Query methods
    bool isBurstMode() const;
    float getArousalWeight() const;
    float getVigilanceWeight() const;
    
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// ENHANCED - Phase 6: Full neuromodulation system
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signals
    void signalMood(float mood);
    void signalSocial(float social);
    
    // Configuration
    void setBaseline(float baseline);
    void setTonicLevel(float tonic);
    
    // Integration modifiers
    void setMoodWeight(float weight);
    void setImpulsivityWeight(float weight);
    
    // Query methods
    bool isBurstMode() const;
    float getMoodWeight() const;
    float getImpulsivityWeight() const;
    
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
