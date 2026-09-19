#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Provides common interface for all neuromodulators with level management,
// plasticity modulation, and temporal dynamics implementation

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name (abbreviation)
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (0.0 = baseline, 1.0 = peak)
    virtual float getLevel() const = 0;
    
    // Set level with clamping to [0.0, 1.0] range
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to synaptic plasticity
    // Returns a factor (typically 0.0-2.0) that scales synaptic modification
    // Higher values increase plasticity, lower values decrease it
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state based on temporal dynamics
    // Decay toward baseline, integrate phasic signals, update internal state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Locus coeruleus-noradrenergic system signaling reward prediction errors
class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling for reinforcement learning
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Integration methods for brain system coordination
    void integrateWithCuriosity(float curiosityLevel);
    void integrateWithNovelty(float noveltyLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation neuromodulator
// PLACEHOLDER - Phase 2 implementation
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance neuromodulator
// PLACEHOLDER - Phase 2 implementation
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood regulation and social behavior neuromodulator
// PLACEHOLDER - Phase 2 implementation
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Neuromodulation coordination: Integration hub for all neuromodulators
class NeuromodulationManager {
public:
    NeuromodulationManager();
    ~NeuromodulationManager();
    
    // Initialize with brain reference for system coordination
    void initialize(class Brain* brain);
    
    // Update all neuromodulators and compute integrated effects
    void updateAll(TimestepDuration dt);
    
    // Signal processing from brain systems
    void processReward(float reward);
    void processPredictionError(float error);
    void processNovelty(float novelty);
    void processAttention(float salience, float focus);
    void processSocialFeedback(float feedback);
    void processImpulsivity(float level);
    void processStress(float stressLevel);
    
    // Get neuromodulator levels for monitoring
    float getAChLevel() const;
    float getNELevel() const;
    float get5HTLevel() const;
    float getDALevel() const;
    
    // Get integrated plasticity modulation factor
    float getIntegratedPlasticityFactor() const;
    
    // Reset all neuromodulators to baseline
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
