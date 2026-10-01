#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <string>
#include <vector>

namespace nlm {

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
// Apply neuromodulatory effect to plasticity
     // TODO PHASE 2: Implement real modulation - see Neuromodulator.cpp for complete implementation
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Forward declarations for all neuromodulators
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;

// Dopamine: Reward and reinforcement learning signal
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
    
    // Initialize with brain reference for system integration
    void initialize(Brain* brain);
    
    // Set plasticity scaling factor
    void setPlasticityScaling(float scaling) { pImpl->stdpScaling = scaling; }
    
    // Get last reward prediction error
    float getLastRewardPredictionError() const { return pImpl->lastRewardPredictionError; }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Attention signaling
    void signalAttention(float salience);
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Set attention parameters
    void setAttentionGain(float gain) { pImpl->attentionGain = gain; }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Stress response signaling
    void signalStress(float stressorLevel);
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Set arousal parameters
    void setArousalLevel(float level) { pImpl->arousalLevel = level; }
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood signaling
    void signalMoodChange(float moodDelta);
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Set mood parameters
    void setMoodLevel(float level) { pImpl->moodLevel = level; }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
