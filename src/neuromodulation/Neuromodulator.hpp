#pragma once

#include "../core/Types/Types.hpp"
#include "../core/ErrorHandling/ErrorHandling.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Enhanced with comprehensive error handling and validation
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level with validation
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Enhanced with error handling
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state with error handling
    virtual void update(TimestepDuration dt) = 0;
    
    // Enhanced error handling utilities
    void validateLevel(float level) const;
    void validateTimestep(TimestepDuration dt) const;
    
    // Error context for debugging
    virtual void setErrorContext(const std::string& context) { errorContext_ = context; }
    const std::string& getErrorContext() const { return errorContext_; }
    
    // Check if neuromodulator is initialized
    virtual bool isInitialized() const { return isInitialized_; }
    
    // Reset neuromodulator state
    virtual void reset() { isInitialized_ = false; }
    
protected:
    Neuromodulator() : isInitialized_(false), errorContext_("") {}
    
    bool isInitialized_;
    std::string errorContext_;
};

// Enhanced Dopamine: Reward and reinforcement learning signal
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
    
    // Reward signaling with validation
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Enhanced error handling
    void validateReward(float reward) const;
    void validatePredictionError(float error) const;
    
    // Reset method override
    void reset() override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Enhanced Acetylcholine: Attention and memory consolidation
// PLACEHOLDER - Phase 2
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override { return "ACh"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Memory consolidation methods with validation
    void consolidateMemory(const std::vector<NeuronId>& neurons, float strength);
    void strengthenSynapticConnections(const std::vector<SynapseId>& synapses);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Enhanced Norepinephrine: Arousal and vigilance
// PLACEHOLDER - Phase 2
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal and vigilance methods
    void increaseArousal(float intensity);
    void decreaseVigilance(float factor);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Enhanced Serotonin: Mood, impulsivity, and social behavior
// PLACEHOLDER - Phase 2
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override { return "5-HT"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood and behavior modulation methods
    void modulateMood(float moodLevel);
    void regulateImpulsivity(float level);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
