#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Advanced neuromodulation system with temporal dynamics and plasticity integration

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
    
    // Get additional modulator-specific information
    virtual float getPredictionError() const { return 0.0f; }
    virtual float getExpectedValue() const { return 0.0f; }
    virtual float getTemporalDerivative() const { return 0.0f; }
    
    // Reset modulator state
    virtual void reset() {}
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Advanced implementation with phasic/tonic dynamics and eligibility traces
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
    
    // Accessor methods
    float getPredictionError() const override;
    float getExpectedValue() const override;
    float getPhasicLevel() const;
    float getTonicLevel() const;
    float getRefractoriness() const;
    const std::vector<float>& getEligibilityTrace() const;
    
    // Configuration and control
    void setRefractoriness(float refractoriness);
    void setTraceDecayRate(float decayRate);
    void updateEligibilityTrace(float traceValue);
    void decayEligibilityTrace(float decayFactor);
    void reset() override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Enhanced with temporal dynamics and learning modulation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Memory consolidation
    void consolidateMemory(const std::vector<float>& memoryTrace);
    void updateAttention(float sensoryInput);
    float getAttentionLevel() const;
    float getMemoryConsolidationLevel() const;
    
    // Configuration
    void setAttentionGain(float gain);
    void setMemoryDecayRate(float decayRate);
    void reset() override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal, vigilance, and stress response
// Advanced implementation with adaptive dynamics
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Stress and arousal modulation
    void signalStress(float stressLevel);
    void signalNovelty(float noveltyLevel);
    float getArousalLevel() const;
    float getVigilanceLevel() const;
    float getStressLevel() const;
    
    // Configuration
    void setArousalBaseline(float baseline);
    void setVigilanceThreshold(float threshold);
    void setStressSensitivity(float sensitivity);
    void reset() override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood regulation, impulsivity control, and social behavior
// Enhanced with behavioral control and decision modulation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Behavioral control
    void signalSocialFeedback(float socialReward);
    void signalImbalance(float behavioralBias);
    float getMoodLevel() const;
    float getImpulseControlLevel() const;
    float getSocialBehaviorLevel() const;
    
    // Configuration
    void setMoodBaseline(float baseline);
    void setImpulseThreshold(float threshold);
    void setSocialSensitivity(float sensitivity);
    void reset() override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// NeuromodulatorManager: Manages all neuromodulators and their interactions
class NeuromodulatorManager {
public:
    NeuromodulatorManager();
    ~NeuromodulatorManager();
    
    // Initialize all neuromodulators
    void initialize(class Brain* brain);
    
    // Update all neuromodulators
    void updateAll(TimestepDuration dt);
    
    // Add neuromodulator to the system
    void addModulator(Neuromodulator* modulator);
    
    // Get specific neuromodulator by name
    Neuromodulator* getModulator(const std::string& name);
    const Neuromodulator* getModulator(const std::string& name) const;
    
    // Get composite neuromodulation level
    float getCompositeNeuromodulationLevel() const;
    
    // Get neuromodulator levels for decision making
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionError() const;
    float getNeuromodulationLevel() const;
    
    // Enable/disable specific neuromodulators
    void enableModulator(const std::string& name, bool enable);
    bool isModulatorEnabled(const std::string& name) const;
    
    // Reset all neuromodulators
    void resetAll();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm