#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
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
    
    // Reset neuromodulator to baseline
    virtual void reset() = 0;
    
    // Signal effect to brain
    virtual void signal(Brain* brain) = 0;
    
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
    
    // Reset neuromodulator to baseline
    void reset() override;
    
    // Signal effect to brain
    void signal(Brain* brain) override {
        boost::shared_ptr<Brain> brainPtr(brain);
        // Apply dopamine effects
    }
    
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
    
    // Signal attention to the brain
    void signalAttention(boost::shared_ptr<Brain> brain);
    
    // Reset neuromodulator to baseline
    void reset() override;
    
    // Signal effect to brain
    void signal(Brain* brain) override {
        boost::shared_ptr<Brain> brainPtr(brain);
        signalAttention(brainPtr);
    }
    
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
    
    // Signal arousal to the brain
    void signalArousal(float intensity, boost::shared_ptr<Brain> brain);
    
    // Reset neuromodulator to baseline
    void reset() override;
    
    // Signal effect to brain
    void signal(Brain* brain) override {
        boost::shared_ptr<Brain> brainPtr(brain);
        signalArousal(1.0f, brainPtr);
    }
    
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
    
    // Signal mood change to the brain
    void signalMoodChange(float moodChange, boost::shared_ptr<Brain> brain);
    
    // Reset neuromodulator to baseline
    void reset() override;
    
    // Signal effect to brain
    void signal(Brain* brain) override {
        boost::shared_ptr<Brain> brainPtr(brain);
        signalMoodChange(0.5f, brainPtr);
    }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm