#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Phase 5: Full neuromodulation system implementation

#include <string>

namespace nlm {

class Neuromodulator {
public:
    Neuromodulator(const std::string& name);
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    const char* getName() const { return name_.c_str(); }
    std::string getNameString() const { return name_; }
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(double dt) = 0;
    
    // Reset to baseline
    virtual void reset() = 0;
    
protected:
    std::string name_;
};

// Dopamine: Reward and reinforcement learning signal
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(double dt) override;
    void reset() override;
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(double dt) override;
    void reset() override;
    
    // Attention modulation
    void focusOnModality(int modality);
    void enhanceMemory(const std::vector<float>& memory);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(double dt) override;
    void reset() override;
    
    // Arousal responses
    void triggerFightResponse();
    void triggerFlightResponse();
    void increaseAlertness(float amount);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(double dt) override;
    void reset() override;
    
    // Mood regulation
    void increaseHappiness(float amount);
    void reduceImpulsivity(float amount);
    void enhanceSocialBonding();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
