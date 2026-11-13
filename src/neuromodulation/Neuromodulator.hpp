#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Implements realistic dopamine dynamics with phasic and tonic firing
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling with temporal dynamics
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get prediction error signal
    float getPredictionError() const { return predictionError_; }
    
    // Get expected value
    float getExpectedValue() const { return expectedValue_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    float predictionError_;
    float expectedValue_;
};

// Acetylcholine: Attention and memory consolidation
// Implements attention modulation and memory strengthening
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention and memory signaling
    void signalAttention(float attention);
    void signalMemoryConsolidation(float strength);
    
    // Get attention signal
    float getAttentionSignal() const { return attentionSignal_; }
    
    // Get memory consolidation signal
    float getMemoryConsolidationSignal() const { return memoryConsolidationSignal_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    float attentionSignal_;
    float memoryConsolidationSignal_;
};

// Norepinephrine: Arousal and vigilance
// Implements arousal modulation and stress response
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and stress signaling
    void signalArousal(float arousal);
    void signalStress(float stress);
    
    // Get arousal signal
    float getArousalSignal() const { return arousalSignal_; }
    
    // Get stress signal
    float getStressSignal() const { return stressSignal_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    float arousalSignal_;
    float stressSignal_;
};

// Serotonin: Mood, impulsivity, and social behavior
// Implements mood regulation and social behavior modulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood and social signaling
    void signalMood(float mood);
    void signalAnxiety(float anxiety);
    
    // Get mood signal
    float getMoodSignal() const { return moodSignal_; }
    
    // Get anxiety signal
    float getAnxietySignal() const { return anxietySignal_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    float moodSignal_;
    float anxietySignal_;
};

// NeuromodulationManager: Coordinates all neuromodulators and their integration
// Manages the interaction between different neuromodulatory systems
class NeuromodulationManager {
public:
    NeuromodulationManager();
    ~NeuromodulationManager();
    
    // Initialize with brain reference for integration
    void initialize(class Brain* brain);
    
    // Update all neuromodulators and compute integrated signals
    void update(float dt);
    
    // Signal events to neuromodulators
    void signalReward(float reward, float predictionError);
    void signalNovelty(float novelty);
    void signalAttention(float attention);
    
    // Get individual neuromodulators
    Dopamine* getDopamine() const;
    Acetylcholine* getAcetylcholine() const;
    Norepinephrine* getNorepinephrine() const;
    Serotonin* getSerotonin() const;
    
    // Get integrated neuromodulation strength
    float getOverallNeuromodulationStrength() const;
    
    // Reset all neuromodulators
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    class Brain* brain_;
    
    // Compute integrated neuromodulation signals
    void computeIntegratedSignals();
    
    // Neuromodulation signals
    float predictionError_;
    float noveltySignal_;
    float rewardSignal_;
    float attentionSignal_;
};

} // namespace nlm
