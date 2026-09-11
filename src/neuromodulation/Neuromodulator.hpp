#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// The real implementations for ACh, NE, and 5-HT are in their own files

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
    
    // Initialize neuromodulator with brain reference
    virtual void initialize(class Brain* brain) {}
    
    // Reset neuromodulator state
    virtual void reset() {}
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine();
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    void initialize(Brain* brain);
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    void initialize(Brain* brain);
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    void enhanceAttention(float signalStrength);
    void consolidateMemory(float memoryStrength);
    void enhanceFocus(float focusSignal);
    
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    void initialize(Brain* brain);
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    void signalAlert(float alertStrength);
    void increaseArousal(float arousalSignal);
    
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin();
    
    void initialize(Brain* brain);
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    void enhanceMood(float moodBoost);
    void reduceImpulsivity(float controlSignal);
    void promoteSocialBehavior(float socialBond);
    
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm