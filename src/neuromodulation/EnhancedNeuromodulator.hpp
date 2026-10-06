// Enhanced Neuromodulator.hpp - Complete implementation for all neuromodulators

#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

// Neuromodulator: Enhanced abstract base for neuromodulatory signals
// Now with full implementation capabilities

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (0.0 to 1.0)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Get brain reference for system integration
    virtual void initialize(class Brain* brain) {}
    
    // Get brain reference
    virtual class Brain* getBrain() const { return nullptr; }
    
    // Check if neuromodulator is active
    virtual bool isActive() const { return getLevel() > 0.001f; }
    
    // Get neuromodulator type
    virtual int getType() const { return 0; }
    
protected:
    Neuromodulator() = default;
    Brain* brain = nullptr;
};

// Dopamine: Reward and reinforcement learning signal
// Complete implementation with real reward prediction error processing
class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    void initialize(class Brain* brain);
    
    // Reward signaling
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Get prediction error for curiosity
    float getPredictionError() const;
    
    // Set prediction error (used by prediction system)
    void setPredictionError(float error);
    
    // Modulate based on novelty
    void modulateByNovelty(float novelty);
    
    // Get exploration drive
    float getExplorationDrive() const;
    
    // Reset system
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Real implementation with working memory and attention effects
class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    void initialize(class Brain* brain);
    
    // Attention modulation
    void enhanceAttention(float stimulusStrength);
    void modulateAttentionGain(float gain);
    
    // Memory effects
    void strengthenMemoryTrace(float memoryId, float strength);
    void modulateMemoryConsolidation(bool enable);
    
    // Sensory processing
    void modulateSensoryProcessing(float gain);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Arousal and vigilance
// Complete implementation with signal-to-noise effects
class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    void initialize(class Brain* brain);
    
    // Arousal effects
    void increaseArousal(float amount);
    void decreaseArousal(float amount);
    void modulateArousal(float level);
    
    // Vigilance and focus
    void enhanceVigilance(float targetFocus);
    void reduceInattention(bool state);
    
    // Signal-to-noise ratio
    void improveSignalToNoiseRatio(float improvement);
    void suppressNoise(float reduction);
    
    // Alertness
    void increaseAlertness(float amount);
    void decreaseAlertness(float amount);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Complete implementation with real behavioral regulation
class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    void initialize(class Brain* brain);
    
    // Mood regulation
    void elevateMood(float amount);
    void reduceMood(float amount);
    void modulateMood(float level);
    
    // Impulsivity control
    void increaseImpulsivityControl(float amount);
    void decreaseImpulsivityControl(float amount);
    void setImpulsivityLevel(float level);
    
    // Social behavior
    void enhanceSocialBehavior(float amount);
    void reduceSocialBehavior(float amount);
    void modulateSocialDrive(float level);
    
    // Stress regulation
    void reduceStress(float amount);
    void increaseStress(float amount);
    void setStressLevel(float level);
    
    // Memory effects
    void enhanceMemoryRetrieval(float amount);
    void modulateMemoryEncoding(bool enhance);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Oxytocin: Social bonding and trust
// New implementation for social behavior regulation
class Oxytocin {
public:
    Oxytocin();
    ~Oxytocin();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    void initialize(class Brain* brain);
    
    // Social bonding
    void enhanceSocialBonding(float amount);
    void promoteTrust(float amount);
    void reduceSocialAnxiety(float amount);
    
    // Social recognition
    void enhanceSocialRecognition(float amount);
    void promoteAttachment(float amount);
    
    // Behavioral effects
    void reduceAggression(float amount);
    void increaseProsocialBehavior(float amount);
    void modulateEmpathy(float level);
    
    // Stress regulation
    void reduceStressResponse(float amount);
    void promoteCalmState(float amount);
    
    // Reward system interaction
    void enhanceSocialReward(float amount);
    void promoteSocialSharing(float amount);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Vasopressin: Social recognition and pair bonding
// New implementation for social behavior regulation
class Vasopressin {
public:
    Vasopressin();
    ~Vasopressin();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    void initialize(class Brain* brain);
    
    // Social recognition
    void enhanceSocialRecognition(float amount);
    void promotePairBonding(float amount);
    void enhanceMateGuarding(float amount);
    
    // Territorial behavior
    void modulateTerritorialBehavior(float level);
    void enhanceResourceDefense(float amount);
    
    // Aggression regulation
    void increaseAggression(float amount);
    void decreaseAggression(float amount);
    void modulateAggressionLevel(float level);
    
    // Social hierarchy
    void enhanceDominanceBehavior(float amount);
    void modulateSocialRanking(float level);
    
    // Stress regulation
    void modulateStressResponse(float level);
    void promoteStability(float amount);
    
    // Memory effects
    void enhanceSocialMemory(float amount);
    void modulatePairBondMemory(bool enhance);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Novelty detection signal (existing implementation)
class Novelty {
public:
    Novelty();
    ~Novelty();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    void initialize(class Brain* brain);
    
    // Novelty detection
    void detectNovelty(const std::vector<float>& currentPattern,
                      const std::vector<float>& previousPattern);
    void setNoveltyLevel(float level);
    
    // Decay and reset
    void decayOverTime(TimestepDuration dt);
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Curiosity drive (existing implementation) 
class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    void initialize(class Brain* brain);
    
    // Curiosity computation
    void updateCuriosity(float novelty, float predictionError, TimestepDuration dt);
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    
    // Exploration
    void driveExploration(float amount);
    void reduceExploration(float amount);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Prediction error signal (existing implementation)
class PredictionError {
public:
    PredictionError();
    ~PredictionError();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    void initialize(class Brain* brain);
    
    // Prediction error computation
    void computeError(float predicted, float actual);
    void updatePrediction(float newPrediction);
    
    // Integration with neuromodulators
    void applyToDopamine(class Dopamine* dopamine);
    void applyToSerotonin(class Serotonin* serotonin);
    void applyToNovelty(class Novelty* novelty);
    
    // Get error information
    float getError() const;
    float getMagnitude() const;
    
    // Reset
    void clearHistory();
    
private:
    struct Impl;
    Impl* pImpl;
};

// Utility for neuromodulator management
class NeuromodulatorManager {
public:
    NeuromodulatorManager();
    ~ NeuromodulatorManager();
    
    // Initialize manager with brain reference
    void initialize(class Brain* brain);
    
    // Add neuromodulator to manager
    void addNeuromodulator(Dopamine* dopamine);
    void addNeuromodulator(Acetylcholine* acetylcholine);
    void addNeuromodulator(Norepinephrine* norepinephrine);
    void addNeuromodulator(Serotonin* serotonin);
    void addNeuromodulator(Oxytocin* oxytocin);
    void addNeuromodulator(Vasopressin* vasopressin);
    void addNeuromodulator(Novelty* novelty);
    void addNeuromodulator(Curiosity* curiosity);
    void addNeuromodulator(PredictionError* predictionError);
    
    // Update all neuromodulators
    void updateAll(TimestepDuration dt);
    
    // Get neuromodulator levels for integration
    float getTotalPlasticityModulation() const;
    float getTotalExcitabilityModulation() const;
    float getTotalAttentionModulation() const;
    
    // Get individual neuromodulator levels
    float getDopamineLevel() const;
    float getAcetylcholineLevel() const;
    float getNorepinephrineLevel() const;
    float getSerotoninLevel() const;
    float getOxytocinLevel() const;
    float getVasopressinLevel() const;
    float getNoveltyLevel() const;
    float getCuriosityLevel() const;
    float getPredictionErrorLevel() const;
    
    // Apply neuromodulation effects
    void applyNeuromodulationToNeurons();
    void applyNeuromodulationToMemory(class WorkingMemory* workingMemory);
    void applyNeuromodulationToAttention(class AttentionalSelection* attention);
    
    // Reset all neuromodulators
    void resetAll();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm