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
    
    // Attention and memory effects
    void signalFocus(float novelty, float importance);
    void signalMemoryEncoding(float encodingQuality);
    
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
    
    // Arousal and cognitive effects
    void signalArousal(float stimulusIntensity, bool unexpected);
    void signalWorkingMemoryLoad(float load);
    
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
    
    // Behavioral and emotional effects
    void signalMood(float moodChange);
    void signalBehavioralInhibition(float inhibitionLevel);
    void signalSleepNeed(float sleepPressure);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Integration helper functions for neuromodulation effects
void applyMemoryEffects(
    std::vector<std::unique_ptr<NeuralWorkingMemory>>& workingMemory,
    std::vector<std::unique_ptr<NeuralEpisodicMemory>>& episodicMemory,
    const Dopamine& dopamine,
    const Acetylcholine& ach,
    const Norepinephrine& ne,
    const Serotonin& serotonin,
    float timestep
);

void applyAttentionEffects(
    std::vector<std::unique_ptr<AttentionalSelection>>& attentionSystems,
    const Dopamine& dopamine,
    const Acetylcholine& ach,
    const Norepinephrine& ne,
    const Serotonin& serotonin,
    float timestep
);

void applyDevelopmentEffects(
    std::vector<std::unique_ptr<DevelopmentSystem>>& developmentSystems,
    const Dopamine& dopamine,
    const Norepinephrine& ne,
    const Serotonin& serotonin,
    float timestep
);

void applyPredictionEffects(
    std::vector<std::unique_ptr<PredictionSystem>>& predictionSystems,
    const Dopamine& dopamine,
    const Acetylcholine& ach,
    float timestep
);

} // namespace nlm
