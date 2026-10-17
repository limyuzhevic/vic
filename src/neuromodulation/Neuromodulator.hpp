#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Enhanced implementation with real neuromodulation effects

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
    
    // Reset neuromodulator state
    virtual void reset() {}
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Enhanced implementation with reward prediction error
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    Dopamine(const Dopamine&) = default;
    Dopamine& operator=(const Dopamine&) = default;
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void reset() override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Reward prediction error components
    float getExpectedReward() const;
    float getPredictionError() const;
    float getEligibilityTrace() const;
    float getExplorationBoost() const;
    
    // Configuration and tuning
    void setBaseline(float baseline);
    void setLearningRate(float rate);
    void setRewardRange(float range);
    void setExplorationEnabled(bool enabled);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override = default;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention mechanisms
    void signalNovelty(float novelty);
    void signalAttention(bool focus);
    void setAttentionThreshold(float threshold);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override = default;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal mechanisms
    void signalArousal(float level, float stress);
    void setArousalThreshold(float threshold);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override = default;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood and social mechanisms
    void signalSocialReward(float socialValue);
    void signalSocialThreat(float threat);
    void setSocialThreshold(float threshold);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neuromodulation system coordinator
class NeuromodulationSystem {
public:
    NeuromodulationSystem();
    ~NeuromodulationSystem();
    
    // Add neuromodulator to system
    void addNeuromodulator(std::unique_ptr<Neuromodulator> neuromodulator);
    
    // Update all neuromodulators
    void update(TimestepDuration dt);
    
    // Get neuromodulator by name
    Neuromodulator* getNeuromodulator(const std::string& name);
    
    // Apply all neuromodulation effects to brain
    void applyToBrain(class Brain* brain);
    
    // Reset all neuromodulators
    void reset();
    
    // Get system state
    std::vector<std::string> getNeuromodulatorNames() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
