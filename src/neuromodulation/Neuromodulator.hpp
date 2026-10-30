#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Now implements real neuromodulation effects with biological dynamics

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Real implementation with biological accuracy
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state with biological dynamics
    virtual void update(TimestepDuration dt) = 0;
    
    // Get baseline level for homeostatic regulation
    virtual float getBaselineLevel() const = 0;
    
    // Set target level for regulated neuromodulation
    virtual void setTargetLevel(float target) = 0;
    
    // Check if neuromodulator is active and modulating
    virtual bool isActive() const = 0;
    
    // Reset to baseline state
    virtual void reset() = 0;
    
protected:
    Neuromodulator() = default;
};
// Dopamine: Reward and reinforcement learning signal
// Enhanced with biological accuracy including release dynamics and receptor modulation

class Dopamine : public Neuromodulator {
public:
    Dopamine();
    ~Dopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getBaselineLevel() const override;
    void setTargetLevel(float target) override;
    bool isActive() const override;
    void reset() override;
    
    // Reward signaling with dopamine burst-pause dynamics
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Motor excitation modulation
    float getMotorExcitationFactor() const;
    void setMotorExcitationFactor(float factor);
    
    // Learning rate modulation
    void setLearningRate(float rate);
    float getLearningRate() const;
    
    // Receptor dynamics (D1, D2, D3, D4, D5)
    void setReceptorAffinity(float d1, float d2);
    float getD1ReceptorEffect() const;
    float getD2ReceptorEffect() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
// Acetylcholine: Attention and memory consolidation
// Enhanced with cholinergic dynamics, working memory modulation, and cortical excitability

class Acetylcholine : public Neuromodulator {
public:
    Acetylcholine();
    ~Acetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getBaselineLevel() const override;
    void setTargetLevel(float target) override;
    bool isActive() const override;
    void reset() override;
    
    // Attention modulation
    float getAttentionLevel() const;
    void setAttentionLevel(float level);
    
    // Working memory enhancement
    void enhanceWorkingMemory(float boostFactor);
    float getWorkingMemoryModulation() const;
    
    // Cortical excitability
    float getCorticalExcitability() const;
    void setCorticalExcitability(float factor);
    
    // Memory consolidation
    void consolidateMemory(float consolidationStrength);
    float getMemoryConsolidationFactor() const;
    
    // Prefrontal cortex modulation
    float getPFCModulation() const;
    void setPFCModulation(float modulation);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
// Norepinephrine: Arousal and vigilance
// Enhanced with locus coeruleus dynamics, attention modulation, and stress response

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getBaselineLevel() const override;
    void setTargetLevel(float target) override;
    bool isActive() const override;
    void reset() override;
    
    // Arousal level
    float getArousalLevel() const;
    void setArousalLevel(float level);
    
    // Vigilance modulation
    float getVigilanceFactor() const;
    void setVigilanceFactor(float factor);
    
    // Stress response
    void signalStress(float stressLevel);
    float getStressResponse() const;
    
    // Locus coeruleus dynamics
    void setLocusCoeruleusGain(float gain);
    float getLocusCoeruleusSensitivity() const;
    
    // Fight-or-flight response
    float getFightOrFlightLevel() const;
    void updateFightOrFlight(float threatLevel, TimestepDuration dt);
    
    // Sleep-wake cycle modulation
    void setWakeDrive(float drive);
    float getWakeDrive() const;
    void setSleepDrive(float drive);
    float getSleepDrive() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};
// Serotonin: Mood, impulsivity, and social behavior
// Enhanced with raphe nucleus dynamics, mood regulation, and social behavior modulation

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getBaselineLevel() const override;
    void setTargetLevel(float target) override;
    bool isActive() const override;
    void reset() override;
    
    // Mood regulation
    float getMoodLevel() const;
    void setMoodLevel(float mood);
    
    // Impulsivity modulation
    float getImpulsivityLevel() const;
    void setImpulsivityLevel(float level);
    
    // Social behavior modulation
    float getSocialBehaviorModulation() const;
    void setSocialBehaviorModulation(float modulation);
    
    // Raphe nucleus dynamics
    void setRapheNucleusActivity(float activity);
    float getRapheNucleusActivity() const;
    
    // Anxiety regulation
    float getAnxietyLevel() const;
    void setAnxietyLevel(float level);
    
    // Reward processing
    void processReward(float rewardValue);
    float getRewardProcessingModulation() const;
    
    // Sleep regulation
    void setSeroSleepModulation(float modulation);
    float getSeroSleepModulation() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced reward system with prediction error and temporal difference learning

class EnhancedReward : public Neuromodulator {
public:
    EnhancedReward();
    ~EnhancedReward() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    float getBaselineLevel() const override;
    void setTargetLevel(float target) override;
    bool isActive() const override;
    void reset() override;
    
    // Temporal difference learning
    void updatePrediction(float newPrediction, float reward, TimestepDuration dt);
    float getPredictionError() const;
    float getTemporalDifference() const;
    
    // Reward prediction
    void setPredictedValue(float value);
    float getPredictedValue() const;
    void setActualValue(float value);
    float getActualValue() const;
    
    // Reward history for learning
    void recordReward(float reward);
    float getRecentRewardAverage(int window) const;
    
    // Reward discounting
    void setDiscountFactor(float factor);
    float getDiscountFactor() const;
    
    // Reward sensitivity
    void setRewardSensitivity(float sensitivity);
    float getRewardSensitivity() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
