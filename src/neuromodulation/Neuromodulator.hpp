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
    
    // Disable copying
    Dopamine(const Dopamine&) = delete;
    Dopamine& operator=(const Dopamine&) = delete;
    
    // Disable moving
    Dopamine(Dopamine&&) = delete;
    Dopamine& operator=(Dopamine&&) = delete;
    
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
// ACh signals attention-grabbing stimuli and enhances memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    // Disable copying
    Acetylcholine(const Acetylcholine&) = delete;
    Acetylcholine& operator=(const Acetylcholine&) = delete;
    
    // Disable moving
    Acetylcholine(Acetylcholine&&) = delete;
    Acetylcholine& operator=(Acetylcholine&&) = delete;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal attention-grabbing stimulus
    void signalAttention(float strength);
    
    // Signal memory-relevant event
    void signalMemoryEvent(float importance);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// NE signals novel/stimulating inputs and maintains alertness
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    // Disable copying
    Norepinephrine(const Norepinephrine&) = delete;
    Norepinephrine& operator=(const Norepinephrine&) = delete;
    
    // Disable moving
    Norepinephrine(Norepinephrine&&) = delete;
    Norepinephrine& operator=(Norepinephrine&&) = delete;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal novel or intense stimulus
    void signalNovelStimulus(float intensity);
    
    // Signal urgent alert
    void signalAlert(float urgency);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// 5-HT signals reward and social interactions, affects mood and impulsivity
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    // Disable copying
    Serotonin(const Serotonin&) = delete;
    Serotonin& operator=(const Serotonin&) = delete;
    
    // Disable moving
    Serotonin(Serotonin&&) = delete;
    Serotonin& operator=(Serotonin&&) = delete;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal reward event
    void signalReward(float intensity);
    
    // Signal social interaction
    void signalSocialEvent(float value);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
