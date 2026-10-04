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
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Attention-specific methods
    float getAttentionGain() const;
    float getWorkingMemoryCapacity() const;
    float getSignalToNoiseRatio() const;
    
    // Memory-specific methods
    float getMemoryEncodingEfficiency() const;
    float getMemoryConsolidationStrength() const;
    float getMemoryRetrievalAccuracy() const;
    
    // Signal-based processing
    void signalAttention(float attentionalDemand);
    void signalMemory(float memoryLoad);
    void signalEncoding(float encodingRate);
    
    // Reset
    void reset();
    
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
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Arousal-specific methods
    float getNeuralGain() const;
    float getAlertnessLevel() const;
    float getResponsiveness() const;
    
    // Stress-response methods
    float getStressSensitivity() const;
    float getAnxietyLevel() const;
    
    // Signal-based processing
    void signalArousal(float arousalLevel);
    void signalStress(float stressLevel);
    void signalAlertness(float alertness);
    
    // Reset
    void reset();
    
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
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Decision-making methods
    float getDecisionBias() const;
    float getImpulsivityLevel() const;
    float getRewardProcessing() const;
    
    // Social behavior methods
    float getSocialCohesion() const;
    float getSocialRecognition() const;
    float getAnxietyLevel() const;
    
    // Sleep/wake cycle methods
    float getSleepReadiness() const;
    float getWakeDrive() const;
    
    // Signal-based processing
    void signalMood(float moodLevel);
    void signalMotivation(float motivationLevel);
    void signalSocialBehavior(float socialActivity);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
