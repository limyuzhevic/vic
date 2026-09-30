#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// Phase 6: Enhanced neuromodulation with memory, attention, and cognitive effects

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // Enhanced: Modulates memory consolidation, attention, and cognitive control
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
protected:
    Neuromodulator() = default;
};

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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
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
    
    // Attention modulation
    void signalAttention(float attentionSignal);
    float getAttentionLevel() const;
    
    // Memory consolidation
    void initiateMemoryConsolidation();
    float getMemoryConsolidationFactor() const;
    
    // Sensory processing enhancement
    void enhanceSensoryProcessing(float enhancementFactor);
    float getSensoryEnhancement() const;
    
    // Sleep/wake regulation
    void setWakeLevel(float wake) { setLevel(wake); }
    float getWakeLevel() const { return getLevel(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
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
    
    // Arousal modulation
    void signalArousal(float arousalSignal);
    float getArousalLevel() const;
    
    // Vigilance enhancement
    void increaseVigilance(float vigilanceIncrement);
    float getVigilanceLevel() const;
    
    // Stress response
    void signalStress(float stressLevel);
    float getStressLevel() const;
    
    // Rapid eye movement (REM) sleep regulation
    void enterREMState();
    void exitREMState();
    bool isInREMState() const;
    
    // Cognitive control enhancement
    void enhanceCognitiveControl(float controlStrength);
    float getCognitiveControlLevel() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
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
    
    // Social behavior modulation
    void signalSocialReward(float socialValue);
    void signalSocialPunishment(float socialCost);
    
    // Impulsivity control
    float getImpulsivityLevel() const;
    void setImpulsivityLevel(float level);
    
    // Mood modulation
    void setMood(float mood) { setLevel(mood); }
    float getMood() const { return getLevel(); }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Novelty: Novelty detection for curiosity-driven exploration
class Novelty : public Neuromodulator {
public:
    Novelty();
    ~Novelty() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Novelty detection
    void detectNovelty(const Observation& observation, const Observation& previousObservation);
    void detectNovelty(const std::vector<float>& currentPattern, const std::vector<float>& previousPattern);
    
    // History tracking
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Curiosity: Exploration motivation combining novelty and prediction error
class Curiosity : public Neuromodulator {
public:
    Curiosity();
    ~Curiosity() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Curiosity update
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Exploration drive
    float getExplorationDrive() const;
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// PredictionError: Error signal for learning and prediction
class PredictionError : public Neuromodulator {
public:
    PredictionError();
    ~PredictionError() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Error computation
    void computeError(float predicted, float actual);
    void updatePrediction(float newPrediction);
    
    // History
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Magnitude
    float getMagnitude() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
