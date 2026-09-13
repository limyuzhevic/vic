#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Abstract base for neuromodulatory signals
// Modern neuroscience shows neuromodulators affect global brain state
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Returns factor (1.0 = normal plasticity, 0.5 = reduced, 2.0 = enhanced)
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on brain activity
    // dt = time delta since last update
    virtual void update(double dt) = 0;
    
    // Reset neuromodulator to baseline state
    virtual void reset() {}
    
protected:
    Neuromodulator() : level_(0.0f) {}
    
    float level_;
};

// Dopamine: Reward prediction error and reinforcement
// Core to reinforcement learning and motivation
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override { return "Dopamine"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = level; }
    
    // Implement abstract methods
    float getPlasticityFactor() const override;
    void update(double dt) override;
    void reset() override;
    
    // Reward signaling methods
    void signalReward(float reward);
    void signalRewardPredictionError(float predictionError);
    void modulatePlasticityBasedOnReward(float reward);
    
    // Get reward history for learning
    float getAverageRecentReward() const { return averageReward_; }
    float getTotalRewardHistory() const { return totalRewardHistory_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Reward tracking
    float averageReward_;
    float totalRewardHistory_;
    int rewardCount_;
    double lastRewardTime_;
};

// Acetylcholine: Attention modulation and memory consolidation
// Enhances signal-to-noise ratio in cortical processing
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "Acetylcholine (ACh)"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = level; }
    
    // Implement abstract methods
    float getPlasticityFactor() const override;
    void update(double dt) override;
    void reset() override;
    
    // Attention control
    void enhanceSensoryProcessing(double attentionWeight);
    void facilitateMemoryConsolidation(double memoryStrength);
    void modulateAlertnessLevel(float alertness);
    
    // Get attentional state
    float getAttentionLevel() const { return attentionLevel_; }
    float getMemoryConsolidationRate() const { return memoryConsolidationRate_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Attention and memory state
    float attentionLevel_;
    float memoryConsolidationRate_;
    double lastAttentionUpdateTime_;
};

// Norepinephrine: Arousal, vigilance, and stress response
// prepares brain for action, enhances focus
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "Norepinephrine (NE)"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = level; }
    
    // Implement abstract methods
    float getPlasticityFactor() const override;
    void update(double dt) override;
    void reset() override;
    
    // Arousal and vigilance control
    void increase alertnessLevel(float level);
    void enhanceWorkingMemoryCapacity(float capacityBoost);
    void prepareForActionSelection(float urgency);
    
    // Get arousal state
    float getArousalLevel() const { return arousalLevel_; }
    float getVigilanceLevel() const { return vigilanceLevel_; }
    float getWorkingMemoryBoost() const { return workingMemoryBoost_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Arousal and cognitive control state
    float arousalLevel_;
    float vigilanceLevel_;
    float workingMemoryBoost_;
    double lastArousalUpdateTime_;
};

// Serotonin: Mood regulation, impulsivity, and social behavior
// modulates emotional responses and social decision making
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "Serotonin (5-HT)"; }
    float getLevel() const override { return level_; }
    void setLevel(float level) override { level_ = level; }
    
    // Implement abstract methods
    float getPlasticityFactor() const override;
    void update(double dt) override;
    void reset() override;
    
    // Emotional and social regulation
    void modulateMoodState(float moodLevel);
    void decrease impulsivityLevel(float reduction);
    void influenceSocialDecisionMaking(float socialWeight);
    void regulateSleepWakeCycle(float sleepPressure);
    
    // Get serotonin state
    float getMoodLevel() const { return moodLevel_; }
    float getImpulsivityLevel() const { return impulsivityLevel_; }
    float getSocialBehaviorWeight() const { return socialBehaviorWeight_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Mood and social state
    float moodLevel_;
    float impulsivityLevel_;
    float socialBehaviorWeight_;
    double lastSerotoninUpdateTime_;
};

// Neuromodulator collection for easy access to all systems
class NeuromodulatorSystem {
public:
    NeuromodulatorSystem();
    ~NeuromodulatorSystem();
    
    // Initialize all neuromodulators
    void initialize();
    
    // Update all neuromodulators
    void update(double dt);
    
    // Get specific neuromodulators
    Dopamine* getDopamine() { return dopamine_.get(); }
    const Dopamine* getDopamine() const { return dopamine_.get(); }
    
    Acetylcholine* getAcetylcholine() { return acetylcholine_.get(); }
    const Acetylcholine* getAcetylcholine() const { return acetylcholine_.get(); }
    
    Norepinephrine* getNorepinephrine() { return norepinephrine_.get(); }
    const Norepinephrine* getNorepinephrine() const { return norepinephrine_.get(); }
    
    Serotonin* getSerotonin() { return serotonin_.get(); }
    const Serotonin* getSerotonin() const { return serotonin_.get(); }
    
    // Overall neuromodulation level (average of all)
    float getOverallModulationLevel() const;
    
    // Reset all neuromodulators
    void resetAll();
    
private:
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Acetylcholine> acetylcholine_;
    std::unique_ptr<Norepinephrine> norepinephrine_;
    std::unique_ptr<Serotonin> serotonin_;
};

} // namespace nlm
