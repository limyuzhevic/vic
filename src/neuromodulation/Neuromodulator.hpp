#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// Neuromodulator interaction type enum
enum class NeuromodulatorInteraction {
    Synergistic,
    Antagonistic,
    Neutral,
    Additive
};

// Neuromodulator: Abstract base for neuromodulatory signals
// Base class with real neuromodulation effects
class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level (nanomolar range for biological accuracy)
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Neuromodulator interaction methods
    virtual NeuromodulatorInteraction getInteractionWith(const Neuromodulator& other) const = 0;
    virtual float getSynergyMultiplier(const Neuromodulator& other) const = 0;
    virtual float getAntagonismMultiplier(const Neuromodulator& other) const = 0;
    
    // State tracking for temporal dynamics
    virtual float getHistoryAverage(unsigned int window) const = 0;
    virtual float getPeakLevel() const = 0;
    virtual float getTimeAboveThreshold(float threshold) const = 0;
    
    // Biological range management
    virtual bool isWithinBiologicalRange(float level) const = 0;
    virtual float getMinLevel() const = 0;
    virtual float getMaxLevel() const = 0;
    
protected:
    Neuromodulator() = default;
};

// Dopamine: Reward and reinforcement learning signal
// Enhanced with biological accuracy
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
    
    // Phasic and tonic components
    void signalPhasicBurst(float magnitude);
    void signalTonicBaseline(float baseline);
    float getPhasicComponent() const;
    float getTonicComponent() const;
    
    // Receptor subtypes (D1, D2)
    float getD1ReceptorEffect() const;
    float getD2ReceptorEffect() const;
    void setD1ReceptorSensitivity(float sensitivity);
    void setD2ReceptorSensitivity(float sensitivity);
    
    // Saturation and depletion effects
    void applySaturation();
    void applyDepletion(float depletionRate);
    bool isSaturated() const;
    float getDepletionState() const;
    
    // Asymmetric learning rate modulation
    float getLearningRateModulation(float predictionError) const;
    
    // Neuromodulator interaction methods
    NeuromodulatorInteraction getInteractionWith(const Neuromodulator& other) const override;
    float getSynergyMultiplier(const Neuromodulator& other) const override;
    float getAntagonismMultiplier(const Neuromodulator& other) const override;
    
    // State tracking
    float getHistoryAverage(unsigned int window) const override;
    float getPeakLevel() const override;
    float getTimeAboveThreshold(float threshold) const override;
    
    // Biological range management
    bool isWithinBiologicalRange(float level) const override;
    float getMinLevel() const override;
    float getMaxLevel() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Acetylcholine: Attention and memory consolidation
// Enhanced with biological accuracy
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
    void modulateAttention(float attentionStrength);
    float getAttentionModulation() const;
    
    // Memory consolidation
    void enhanceMemoryConsolidation(float memoryStrength);
    float getMemoryEnhancement() const;
    
    // Theta rhythm coupling
    void setThetaFrequency(float frequency);
    float getThetaFrequency() const;
    void phaseWithTheta(float phaseOffset);
    
    // Learning rate modulation
    float getLearningRateModulation() const;
    
    // Sensory processing enhancement
    float enhanceSensoryProcessing(float inputStrength) const;
    
    // Neuromodulator interaction methods
    NeuromodulatorInteraction getInteractionWith(const Neuromodulator& other) const override;
    float getSynergyMultiplier(const Neuromodulator& other) const override;
    float getAntagonismMultiplier(const Neuromodulator& other) const override;
    
    // State tracking
    float getHistoryAverage(unsigned int window) const override;
    float getPeakLevel() const override;
    float getTimeAboveThreshold(float threshold) const override;
    
    // Biological range management
    bool isWithinBiologicalRange(float level) const override;
    float getMinLevel() const override;
    float getMaxLevel() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Norepinephrine: Arousal and vigilance
// Enhanced with biological accuracy
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Locus coeruleus firing dynamics
    void setLocusCoeruleusFiringRate(float rate);
    float getLocusCoeruleusFiringRate() const;
    void updateFiringDynamics(TimestepDuration dt);
    
    // Stress and fatigue modulation
    void applyStress(float stressLevel);
    void applyFatigue(float fatigueLevel);
    float getStressModulation() const;
    float getFatigueModulation() const;
    
    // Arousal and vigilance
    float getArousalLevel() const;
    float getVigilanceLevel() const;
    
    // Attention shifting mechanisms
    void shiftAttention(float shiftAmount);
    float getAttentionShift() const;
    
    // Neuromodulator interaction methods
    NeuromodulatorInteraction getInteractionWith(const Neuromodulator& other) const override;
    float getSynergyMultiplier(const Neuromodulator& other) const override;
    float getAntagonismMultiplier(const Neuromodulator& other) const override;
    
    // State tracking
    float getHistoryAverage(unsigned int window) const override;
    float getPeakLevel() const override;
    float getTimeAboveThreshold(float threshold) const override;
    
    // Biological range management
    bool isWithinBiologicalRange(float level) const override;
    float getMinLevel() const override;
    float getMaxLevel() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Serotonin: Mood, impulsivity, and social behavior
// Enhanced with biological accuracy
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Mood modulation
    void modulateMood(float moodLevel);
    float getMoodModulation() const;
    
    // Decody switching mechanisms
    void switchDecodyState();
    bool isDecodySwitching() const;
    
    // Impulsivity control
    float getImpulsivityControl() const;
    void applyImpulsivityControl(float controlStrength);
    
    // Social behavior enhancement
    float enhanceSocialBehavior(float socialInput) const;
    
    // Neuromodulator interaction methods
    NeuromodulatorInteraction getInteractionWith(const Neuromodulator& other) const override;
    float getSynergyMultiplier(const Neuromodulator& other) const override;
    float getAntagonismMultiplier(const Neuromodulator& other) const override;
    
    // State tracking
    float getHistoryAverage(unsigned int window) const override;
    float getPeakLevel() const override;
    float getTimeAboveThreshold(float threshold) const override;
    
    // Biological range management
    bool isWithinBiologicalRange(float level) const override;
    float getMinLevel() const override;
    float getMaxLevel() const override;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Novelty detection system
class Novelty {
public:
    Novelty();
    ~Novelty();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get novelty level
    float getLevel() const;
    void setLevel(float level);
    
    // Detect novelty from observation
    void detectNovelty(const class Observation& observation, 
                       const class Observation& previousObservation);
    
    // Detect novelty from sensory input pattern
    void detectNovelty(const std::vector<float>& currentPattern,
                       const std::vector<float>& previousPattern);
    
    // Decay novelty over time
    void update(TimestepDuration dt);
    
    // Get novelty history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Get novel object detection and feature detection
    float getNovelObjectDetection() const;
    const std::vector<float>& getFeatureDetection() const;
    
    // Habituation and sensory adaptation mechanisms
    void applyHabituation(float adaptationRate);
    void applySensoryAdaptation(float adaptationRate);
    
    // Novelty weighting for curiosity and exploration
    float getNoveltyWeight() const;
    void setNoveltyWeight(float weight);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Curiosity system
class Curiosity {
public:
    Curiosity();
    ~Curiosity();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get curiosity level
    float getLevel() const;
    
    // Update curiosity based on novelty and prediction error
    void update(float novelty, float predictionError, TimestepDuration dt);
    
    // Get exploration drive (same as level)
    float getExplorationDrive() const;
    
    // Set curiosity parameters
    void setNoveltyWeight(float weight);
    void setPredictionErrorWeight(float weight);
    
    // Reset
    void reset();
    
    // Adaptive curiosity modulation
    void adaptModulation(float environmentComplexity);
    
    // Exploration-exploitation balance
    float getExplorationExploitationBalance() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Prediction error system
class PredictionError {
public:
    PredictionError();
    ~PredictionError();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get prediction error
    float getError() const;
    
    // Compute prediction error from predicted vs actual
    void computeError(float predicted, float actual);
    
    // Update prediction
    void updatePrediction(float newPrediction);
    
    // Get prediction error magnitude
    float getMagnitude() const;
    
    // Get temporal dynamics of prediction error
    float getTemporalErrorTrend() const;
    float getErrorVariance() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Reward system
class Reward {
public:
    Reward();
    ~Reward();
    
    // Get current reward value
    float getValue() const;
    void setValue(float value);
    
    // Accumulate reward
    void add(float delta);
    
    // Reset accumulated reward
    void reset();
    
    // Compute reward from environment state
    float computeReward(const class Observation& observation) const;
    
    // Reward history
    const std::vector<float>& getHistory() const;
    void clearHistory();
    
    // Reward prediction
    void updatePrediction(float prediction);
    float getPredictionError() const;
    
    // Reward temporal dynamics
    float getRewardRate() const;
    float getRewardHistoryAverage(unsigned int window) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neuromodulator integration with Brain
class NeuromodulatorIntegration {
public:
    NeuromodulatorIntegration();
    ~NeuromodulatorIntegration();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Apply neuromodulator effects to neural excitability
    void applyToNeuralExcitability(class NeuralRegion* region);
    
    // Apply neuromodulator effects to synaptic plasticity
    void applyToSynapticPlasticity(class Synapse* synapse);
    
    // Add neuromodulator state to Brain checkpoints
    void checkpointNeuromodulatorState();
    
    // Include neuromodulator feedback loops
    void updateFeedbackLoops();
    
    // Get current neuromodulator levels
    float getDopamineLevel() const;
    float getAcetylcholineLevel() const;
    float getNorepinephrineLevel() const;
    float getSerotoninLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionErrorLevel() const;
    
    // Update all neuromodulators
    void updateAll(TimestepDuration dt);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
