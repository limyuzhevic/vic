#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Enhanced Neuromodulator: Base class with additional interfaces for attention and memory
// Extends the basic neuromodulator interface to support cognitive functions

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
    
    // Cognitive modulation interfaces (ENHANCED)
    virtual float getAttentionalGain() const { return 1.0f; }
    virtual float getMemoryTraceStrength() const { return 1.0f; }
    virtual float getLearningRate() const { return 1.0f; }
    virtual void modulateAttention(float& gain) {}
    virtual void modulateMemory(float& strength) {}
    
    // Specialized neuromodulators with cognitive effects
    virtual bool supportsAttention() const { return false; }
    virtual bool supportsMemory() const { return false; }
    virtual bool supportsLearning() const { return false; }
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal (ENHANCED)
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
    
    // Enhanced cognitive interfaces
    float getAttentionalGain() const override;
    float getLearningRate() const override;
    void modulateAttention(float& gain) override;
    void modulateLearning(float& rate) override;
    
    bool supportsAttention() const override { return true; }
    bool supportsLearning() const override { return true; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Acetylcholine: Attention and memory consolidation (ENHANCED)
class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced cognitive interfaces
    float getAttentionalGain() const override;
    float getMemoryTraceStrength() const override;
    void modulateAttention(float& gain) override;
    void modulateMemory(float& strength) override;
    
    bool supportsAttention() const override { return true; }
    bool supportsMemory() const override { return true; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance (ENHANCED)
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced cognitive interfaces
    float getAttentionalGain() const override;
    float getLearningRate() const override;
    void modulateAttention(float& gain) override;
    void modulateLearning(float& rate) override;
    
    bool supportsAttention() const override { return true; }
    bool supportsLearning() const override { return true; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior (ENHANCED)
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Enhanced cognitive interfaces
    float getAttentionalGain() const override;
    float getLearningRate() const override;
    void modulateAttention(float& gain) override;
    void modulateLearning(float& rate) override;
    
    bool supportsAttention() const override { return true; }
    bool supportsLearning() const override { return true; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Novelty detector with attentional capture (ENHANCED)
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Novelty-specific methods
    void detectNovelty(float inputValue, float previousValue);
    float getAttentionalCapture() const;
    float getMemoryTraceStrength() const;
    void reset();
    
    // Enhanced cognitive interfaces
    float getAttentionalGain() const override;
    bool supportsAttention() const override { return true; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// PredictionError with learning modulation (ENHANCED)
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Prediction error methods
    void initialize(class Brain* brain);
    void computeError(float predicted, float actual);
    void updatePrediction(float newPrediction);
    float getMagnitude() const;
    void reset();
    
    // Enhanced cognitive interfaces
    float getLearningRate() const override;
    void modulateLearning(float& rate) override;
    bool supportsLearning() const override { return true; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Curiosity system with attention and memory integration (ENHANCED)
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Curiosity methods
    void initialize(class Brain* brain);
    void update(float novelty, float predictionError, TimestepDuration dt);
    float getExplorationDrive() const;
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    void reset();
    
    // Enhanced cognitive interfaces
    float getAttentionalGain() const override;
    float getLearningRate() const override;
    void modulateAttention(float& gain) override;
    void modulateLearning(float& rate) override;
    
    bool supportsAttention() const override { return true; }
    bool supportsLearning() const override { return true; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Reward system with neuromodulatory integration (ENHANCED)
class Reward : public Neuromodulator {
public:
    Reward();
    ~Reward() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward methods
    void add(float delta);
    float computeReward(const class Observation& observation) const;
    void reset();
    
    // Enhanced cognitive interfaces
    float getLearningRate() const override;
    void modulateLearning(float& rate) override;
    bool supportsLearning() const override { return true; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced Neuromodulation Manager for integrating all neuromodulators
class NeuromodulationManager {
public:
    NeuromodulationManager();
    ~NeuromodulationManager();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Update all neuromodulators
    void update(TimestepDuration dt, const class Observation& observation);
    
    // Get combined neuromodulatory effects
    float getCombinedAttentionalGain() const;
    float getCombinedMemoryStrength() const;
    float getCombinedLearningRate() const;
    
    // Add/remove neuromodulators
    void addNeuromodulator(std::unique_ptr<Neuromodulator> neuromodulator);
    void removeNeuromodulator(const char* name);
    Neuromodulator* getNeuromodulator(const char* name) const;
    
    // Get neuromodulator by type
    Dopamine* getDopamine() const;
    Acetylcholine* getAcetylcholine() const;
    Norepinephrine* getNorepinephrine() const;
    Serotonin* getSerotonin() const;
    Novelty* getNovelty() const;
    PredictionError* getPredictionError() const;
    Curiosity* getCuriosity() const;
    Reward* getReward() const;
    
    // Get all neuromodulators
    std::vector<Neuromodulator*> getAllNeuromodulators() const;
    
    // Reset all neuromodulators
    void resetAll();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm