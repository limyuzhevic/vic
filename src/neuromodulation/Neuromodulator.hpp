#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace nlm {

// Forward declarations
class NeuromodulationEngine;
class PlasticityController;
class NeuromodulatorNetwork;
class NeuromodulatorDynamics;

// Enhanced neuromodulator configuration
struct NeuromodulatorConfig {
    float baselineLevel;              // Baseline concentration
    float maxLevel;                   // Maximum concentration
    float minLevel;                   // Minimum concentration
    float synthesisRate;               // Synthesis rate
    float decayRate;                  // Decay rate
    float homeostasisTarget;          // Homeostatic target level
    float adaptationSpeed;             // Adaptation speed
    float noiseLevel;                  // Noise level
    float plasticityModulation;        // Plasticity modulation strength
    float learningRate;                 // Learning rate
    float timeConstant;                 // Time constant for dynamics
    float saturation;                   // Saturation level
    bool enableAdaptation;             // Enable adaptation
    bool enableHomeostasis;            // Enable homeostasis
    bool enableNoise;                   // Enable noise
    bool enablePlasticityModulation;    // Enable plasticity modulation
    
    NeuromodulatorConfig()
        : baselineLevel(0.5f), maxLevel(1.0f), minLevel(0.0f), synthesisRate(0.1f),
          decayRate(0.1f), homeostasisTarget(0.5f), adaptationSpeed(0.01f),
          noiseLevel(0.0f), plasticityModulation(1.0f), learningRate(0.001f),
          timeConstant(0.5f), saturation(1.0f), enableAdaptation(true),
          enableHomeostasis(true), enableNoise(true), enablePlasticityModulation(true) {}
};

// Neuromodulator state for advanced dynamics
struct NeuromodulatorState {
    float currentLevel;                // Current concentration
    float targetLevel;                 // Target concentration
    float productionRate;              // Production rate
    float releaseRate;                 // Release rate
    float reuptakeRate;                // Reuptake rate
    float degradationRate;             // Degradation rate
    float receptorSaturation;          // Receptor saturation
    float intracellularStorage;        // Intracellular storage
    float homeostasisValue;            // Homeostatic set point
    float adaptationFactor;            // Adaptation factor
    float learningWeight;              // Learning weight
    float temporalTrace;               // Temporal trace
    float eligibilityTrace;            // Eligibility trace
    float predictionErrorSensitivity;  // Sensitivity to prediction errors
    float rewardSensitivity;           // Sensitivity to rewards
    float noveltySensitivity;          // Sensitivity to novelty
    float curiositySensitivity;        // Sensitivity to curiosity
    float uncertaintyEstimate;         // Estimate of uncertainty
    float confidenceValue;              // Confidence in predictions
    float informationGain;              // Information gain
    float explorationBonus;            // Exploration bonus
    float exploitationBonus;           // Exploitation bonus
    float predictionValue;             // Current prediction
    float predictionError;             // Prediction error
    float rewardPrediction;            // Predicted reward
    float actualReward;                // Actual received reward
    float noveltyValue;                 // Novelty value
    float curiosityValue;              // Curiosity value
    float temporalDerivative;           // Temporal derivative
    float spectralDensity;              // Spectral density
    float coherence;                    // Coherence
    float stationarity;                 // Stationarity
    float predictability;              // Predictability
    
    NeuromodulatorState()
        : currentLevel(0.5f), targetLevel(0.5f), productionRate(0.0f),
          releaseRate(1.0f), reuptakeRate(0.05f), degradationRate(0.01f),
          receptorSaturation(0.0f), intracellularStorage(0.0f),
          homeostasisValue(0.5f), adaptationFactor(1.0f), learningWeight(0.01f),
          temporalTrace(0.0f), eligibilityTrace(0.0f), predictionErrorSensitivity(1.0f),
          rewardSensitivity(1.0f), noveltySensitivity(1.0f), curiositySensitivity(1.0f),
          uncertaintyEstimate(0.0f), confidenceValue(0.5f), informationGain(0.0f),
          explorationBonus(0.0f), exploitationBonus(0.0f), predictionValue(0.0f),
          predictionError(0.0f), rewardPrediction(0.0f), actualReward(0.0f),
          noveltyValue(0.0f), curiosityValue(0.0f), temporalDerivative(0.0f),
          spectralDensity(1.0f), coherence(0.0f), stationarity(0.5f),
          predictability(0.5f) {}
};

// Advanced neuromodulator interface with comprehensive control
class Neuromodulator {
public:
    Neuromodulator(const NeuromodulatorConfig& config);
    virtual ~Neuromodulator();
    
    // Disable copying, enable moving
    Neuromodulator(const Neuromodulator&) = delete;
    Neuromodulator& operator=(const Neuromodulator&) = delete;
    Neuromodulator(Neuromodulator&&) noexcept;
    Neuromodulator& operator=(Neuromodulator&&) noexcept;
    
    // Configuration
    void setConfig(const NeuromodulatorConfig& config);
    const NeuromodulatorConfig& getConfig() const { return config_; }
    
    // State management
    void updateState(const NeuromodulatorState& state);
    NeuromodulatorState getState() const { return state_; }
    void setState(const NeuromodulatorState& state) { state_ = state; }
    
    // Core neuromodulator functions
    virtual const char* getName() const = 0;
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    virtual float getPlasticityFactor() const = 0;
    
    // Signal generation
    virtual void update(TimestepDuration dt) = 0;
    virtual void synthesizeSignal(float intensity, float duration) = 0;
    virtual void modulateSignal(float modulation) = 0;
    
    // Learning and adaptation
    virtual void learnFromError(float error) = 0;
    virtual void adaptToEnvironment(float environmentSignal) = 0;
    virtual void maintainHomeostasis() = 0;
    
    // Integration with other systems
    virtual void integrateWithBrain(std::shared_ptr<Brain> brain) = 0;
    virtual void integrateWithPlasticity(std::shared_ptr<PlasticityController> plasticity) = 0;
    virtual void integrateWithNetwork(std::shared_ptr<NeuromodulatorNetwork> network) = 0;
    virtual void integrateWithDynamics(std::shared_ptr<NeuromodulatorDynamics> dynamics) = 0;
    
    // Advanced neuromodulator functions
    virtual void applyNeuromodulation(float& synapticWeight, const Neuromodulator& other) const = 0;
    virtual void modulateLearningRate(float& learningRate, const Neuromodulator& other) const = 0;
    virtual void modulateAttention(float& attention, const Neuromodulator& other) const = 0;
    virtual void modulateMemoryConsolidation(float& memoryStrength, const Neuromodulator& other) const = 0;
    virtual void modulateMotorOutput(float& motorOutput, const Neuromodulator& other) const = 0;
    
    // Signal processing
    virtual float computeSignalQuality() const = 0;
    virtual float computeSignalReliability() const = 0;
    virtual float computeSignalComplexity() const = 0;
    virtual float computeSignalNovelty() const = 0;
    
    // Real-time control
    virtual void startSignalGeneration() = 0;
    virtual void stopSignalGeneration() = 0;
    virtual bool isSignalGenerationActive() const = 0;
    virtual float getSignalGenerationFrequency() const = 0;
    
    // Safety and constraints
    virtual bool validateSafetyConstraints() const = 0;
    virtual void enforceSafetyConstraints() = 0;
    virtual bool checkNumericalStability() const = 0;
    virtual void enforceHomeostaticBounds() = 0;
    
    // Export/import
    virtual void exportState(std::ostream& stream) const = 0;
    virtual void importState(std::istream& stream) = 0;
    
    // Performance metrics
    virtual float getSignalEfficiency() const = 0;
    virtual float getSignalReliability() const = 0;
    virtual float getAdaptationSpeed() const = 0;
    virtual float getLearningRate() const = 0;
    virtual float getTotalInfluence() const = 0;
    
protected:
    NeuromodulatorConfig config_;
    NeuromodulatorState state_;
    
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<PlasticityController> plasticity_;
    std::shared_ptr<NeuromodulatorNetwork> network_;
    std::shared_ptr<NeuromodulatorDynamics> dynamics_;
    
    // Protected helper methods
    void updateLevel(TimestepDuration dt);
    void applyHomeostasis();
    void adaptToChanges();
    void applyNoise();
    void enforceBounds();
    float computeTemporalDerivative(float current, float previous, TimestepDuration dt) const;
    float computeSpectralDensity(const std::vector<float>& signal) const;
    float computeCoherence(const std::vector<float>& signal1, const std::vector<float>& signal2) const;
};

// Enhanced Dopamine with advanced learning mechanisms
class AdvancedDopamine : public Neuromodulator {
public:
    AdvancedDopamine(const NeuromodulatorConfig& config);
    ~AdvancedDopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void synthesizeSignal(float intensity, float duration) override;
    void modulateSignal(float modulation) override;
    
    // Reward and learning functions
    void signalReward(float reward) override;
    void signalRewardPredictionError(float error) override;
    void learnFromError(float error) override;
    void adaptToEnvironment(float environmentSignal) override;
    
    // Advanced dopamine functions
    void updateEligibilityTraces(TimestepDuration dt);
    void computeTemporalDifference(float predictedValue, float actualValue, float reward, float nextPredictedValue);
    void updateMultiStepPredictions(const std::vector<float>& futureRewards);
    void modulatePlasticity(float& synapticWeight, const Neuromodulator& other) const override;
    void modulateLearningRate(float& learningRate, const Neuromodulator& other) const override;
    void modulateAttention(float& attention, const Neuromodulator& other) const override;
    void modulateMemoryConsolidation(float& memoryStrength, const Neuromodulator& other) const override;
    void modulateMotorOutput(float& motorOutput, const Neuromodulator& other) const override;
    
    // Performance metrics
    float computeSignalQuality() const override;
    float computeSignalReliability() const override;
    float computeSignalComplexity() const override;
    float computeSignalNovelty() const override;
    
    // Real-time control
    void startSignalGeneration() override;
    void stopSignalGeneration() override;
    bool isSignalGenerationActive() const override;
    float getSignalGenerationFrequency() const override;
    
    // Safety and constraints
    bool validateSafetyConstraints() const override;
    void enforceSafetyConstraints() override;
    bool checkNumericalStability() const override;
    void enforceHomeostaticBounds() override;
    
    // Export/import
    void exportState(std::ostream& stream) const override;
    void importState(std::istream& stream) override;
    
    // Performance metrics
    float getSignalEfficiency() const override;
    float getSignalReliability() const override;
    float getAdaptationSpeed() const override;
    float getLearningRate() const override;
    float getTotalInfluence() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine with advanced attention modulation
class AdvancedAcetylcholine : public Neuromodulator {
public:
    AdvancedAcetylcholine(const NeuromodulatorConfig& config);
    ~AdvancedAcetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void synthesizeSignal(float intensity, float duration) override;
    void modulateSignal(float modulation) override;
    
    // Learning and adaptation
    void learnFromError(float error) override;
    void adaptToEnvironment(float environmentSignal) override;
    
    // Advanced acetylcholine functions
    void updateAttentionWeights(float& attention, const Neuromodulator& other) const;
    void updateMemoryConsolidation(float& memoryStrength, const Neuromodulator& other) const;
    void updateSensoryProcessing(float& sensoryProcessing, const Neuromodulator& other) const;
    void updateCognitiveFunctions(float& cognitiveFunction, const Neuromodulator& other) const;
    
    // Performance metrics
    float computeSignalQuality() const override;
    float computeSignalReliability() const override;
    float computeSignalComplexity() const override;
    float computeSignalNovelty() const override;
    
    // Real-time control
    void startSignalGeneration() override;
    void stopSignalGeneration() override;
    bool isSignalGenerationActive() const override;
    float getSignalGenerationFrequency() const override;
    
    // Safety and constraints
    bool validateSafetyConstraints() const override;
    void enforceSafetyConstraints() override;
    bool checkNumericalStability() const override;
    void enforceHomeostaticBounds() override;
    
    // Export/import
    void exportState(std::ostream& stream) const override;
    void importState(std::istream& stream) override;
    
    // Performance metrics
    float getSignalEfficiency() const override;
    float getSignalReliability() const override;
    float getAdaptationSpeed() const override;
    float getLearningRate() const override;
    float getTotalInfluence() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine with advanced arousal modulation
class AdvancedNorepinephrine : public Neuromodulator {
public:
    AdvancedNorepinephrine(const NeuromodulatorConfig& config);
    ~AdvancedNorepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void synthesizeSignal(float intensity, float duration) override;
    void modulateSignal(float modulation) override;
    
    // Learning and adaptation
    void learnFromError(float error) override;
    void adaptToEnvironment(float environmentSignal) override;
    
    // Advanced norepinephrine functions
    void updateArousalLevel(float& arousal, const Neuromodulator& other) const;
    void updateVigilance(float& vigilance, const Neuromodulator& other) const;
    void updatePerformanceMonitoring(float& performance, const Neuromodulator& other) const;
    void updateAlertness(float& alertness, const Neuromodulator& other) const;
    
    // Performance metrics
    float computeSignalQuality() const override;
    float computeSignalReliability() const override;
    float computeSignalComplexity() const override;
    float computeSignalNovelty() const override;
    
    // Real-time control
    void startSignalGeneration() override;
    void stopSignalGeneration() override;
    bool isSignalGenerationActive() const override;
    float getSignalGenerationFrequency() const override;
    
    // Safety and constraints
    bool validateSafetyConstraints() const override;
    void enforceSafetyConstraints() override;
    bool checkNumericalStability() const override;
    void enforceHomeostaticBounds() override;
    
    // Export/import
    void exportState(std::ostream& stream) const override;
    void importState(std::istream& stream) override;
    
    // Performance metrics
    float getSignalEfficiency() const override;
    float getSignalReliability() const override;
    float getAdaptationSpeed() const override;
    float getLearningRate() const override;
    float getTotalInfluence() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin with advanced mood modulation
class AdvancedSerotonin : public Neuromodulator {
public:
    AdvancedSerotonin(const NeuromodulatorConfig& config);
    ~AdvancedSerotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void synthesizeSignal(float intensity, float duration) override;
    void modulateSignal(float modulation) override;
    
    // Learning and adaptation
    void learnFromError(float error) override;
    void adaptToEnvironment(float environmentSignal) override;
    
    // Advanced serotonin functions
    void updateMood(float& mood, const Neuromodulator& other) const;
    void updateImpulsivity(float& impulsivity, const Neuromodulator& other) const;
    void updateSocialBehavior(float& socialBehavior, const Neuromodulator& other) const;
    void updateEmotionRegulation(float& emotionRegulation, const Neuromodulator& other) const;
    
    // Performance metrics
    float computeSignalQuality() const override;
    float computeSignalReliability() const override;
    float computeSignalComplexity() const override;
    float computeSignalNovelty() const override;
    
    // Real-time control
    void startSignalGeneration() override;
    void stopSignalGeneration() override;
    bool isSignalGenerationActive() const override;
    float getSignalGenerationFrequency() const override;
    
    // Safety and constraints
    bool validateSafetyConstraints() const override;
    void enforceSafetyConstraints() override;
    bool checkNumericalStability() const override;
    void enforceHomeostaticBounds() override;
    
    // Export/import
    void exportState(std::ostream& stream) const override;
    void importState(std::istream& stream) override;
    
    // Performance metrics
    float getSignalEfficiency() const override;
    float getSignalReliability() const override;
    float getAdaptationSpeed() const override;
    float getLearningRate() const override;
    float getTotalInfluence() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
