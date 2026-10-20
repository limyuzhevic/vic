#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

namespace nlm {

// Neuromodulator: Abstract base for neuromodulatory signals
// PLACEHOLDER - Phase 2 will implement real neuromodulation effects

class Neuromodulator {
public:
    virtual ~Neuromodulator() = default;
    
    // Get modulator name
    virtual const char* getName() const = 0;
    
    // Get current concentration/level
    virtual float getLevel() const = 0;
    virtual void setLevel(float level) = 0;
    
    // Apply neuromodulatory effect to plasticity
    // TODO PHASE 2: Implement real modulation
    virtual float getPlasticityFactor() const = 0;
    
    // Update neuromodulator state
    virtual void update(TimestepDuration dt) = 0;
    
    // Additional advanced features
    virtual float getModulatoryEffect(NeuronType neuronType) const { return 1.0f; }
    virtual bool isActive() const { return getLevel() > 0.01f; }
    virtual void reset() { setLevel(0.0f); }
    
protected:
    Neuromodulator() = default;
};

// Advanced neuromodulator: Enhanced version with more complex dynamics
class AdvancedNeuromodulator : public Neuromodulator {
public:
    struct NeuromodulatorDynamics {
        float baseline;           // Baseline concentration
        float peakLevel;          // Maximum achievable level
        float decayRate;          // Natural decay rate
        float releaseRate;        // Release rate on stimulus
        float adaptationRate;     // Rate of adaptation to baseline
        float historyDecay;       // Rate of history decay
        float timeConstant;       // Time constant for dynamics
        float saturationLevel;    // Maximum concentration
        
        NeuromodulatorDynamics()
            : baseline(0.0f), peakLevel(1.0f), decayRate(0.1f), releaseRate(1.0f)
            , adaptationRate(0.01f), historyDecay(0.05f), timeConstant(10.0f)
            , saturationLevel(2.0f) {}
    };
    
    struct NeuromodulatorInteraction {
        std::string partnerModulator;
        float weight;                    // Interaction strength (-1 to 1)
        float interactionType;            // 0: linear, 1: sigmoidal, 2: exponential
        std::function<float(float, float)> customInteraction;  // Custom function
        
        NeuromodulatorInteraction()
            : weight(0.0f), interactionType(0), customInteraction(nullptr) {}
    };
    
    AdvancedNeuromodulator(const std::string& name = "Advanced");
    ~AdvancedNeuromodulator() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Advanced neuromodulation features
    float getModulatoryEffect(NeuronType neuronType) const override;
    bool isActive() const override;
    void reset() override;
    
    // Complex dynamics
    void signalEvent(float magnitude, float duration = 1.0f);
    void applyInfluence(float influence, float timeConstant = 1.0f);
    float getHistoryAverage(size_t windowSize = 100) const;
    
    // Multi-factor integration
    void addPartnerNeuromodulator(AdvancedNeuromodulator* partner, float weight);
    float getCombinedEffect() const;
    
    // Configuration
    void configureDynamics(const NeuromodulatorDynamics& dynamics);
    void addInteraction(const NeuromodulatorInteraction& interaction);
    
    // State management
    struct NeuromodulatorState {
        float currentLevel;
        float baselineLevel;
        float targetLevel;
        float historySum;
        size_t historyCount;
        float timeInCurrentState;
        float lastEventTime;
        std::unordered_map<std::string, float> partnerEffects;
        
        NeuromodulatorState()
            : currentLevel(0.0f), baselineLevel(0.0f), targetLevel(0.0f)
            , historySum(0.0f), historyCount(0), timeInCurrentState(0.0f)
            , lastEventTime(0.0f) {}
    };
    
    const NeuromodulatorState& getState() const { return state_; }
    
private:
    struct Impl;
    Impl* pImpl;
    NeuromodulatorState state_;
};

// Multi-factor neuromodulation: Combines multiple neuromodulators
class MultiFactorNeuromodulation {
public:
    MultiFactorNeuromodulation();
    ~MultiFactorNeuromodulation();
    
    // Add neuromodulator factor
    void addFactor(const std::string& name, Neuromodulator* factor, float weight = 1.0f);
    
    // Get combined neuromodulatory effect
    float getCombinedEffect() const;
    
    // Get individual factor effects
    std::unordered_map<std::string, float> getIndividualEffects() const;
    
    // Update all factors
    void update(TimestepDuration dt);
    
    // Configure interaction rules between factors
    void addInteractionRule(const std::string& factor1, const std::string& factor2,
                          const std::function<float(float, float)>& interactionFunc);
    
    // Get interaction matrix
    std::unordered_map<std::string, std::unordered_map<std::string, float>>
    getInteractionMatrix() const;
    
    // Get neuromodulation statistics
    struct NeuromodulationStats {
        float totalEffect;
        float maxEffect;
        float minEffect;
        float averageEffect;
        std::unordered_map<std::string, float> factorEffects;
        std::unordered_map<std::string, float> interactionEffects;
        float temporalCoherence;  // How synchronized factors are over time
        float spatialCoherence;   // How effects distribute across neurons
        std::unordered_map<std::string, float> customMetrics;
    };
    
    NeuromodulationStats getStats() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Stress and arousal modulation system
class StressArousalSystem {
public:
    enum class StressLevel {
        Relaxed,    // Low stress, high focus
        Moderate,   // Balanced state
        Stressed,   // High stress, reduced flexibility
        Overwhelmed // Extreme stress, impaired function
    };
    
    struct StressArousalParameters {
        float stressThreshold;      // Threshold for stress activation
        float recoveryRate;         // Rate of stress recovery
        float adaptationTime;       // Time to adapt to stress level
        float homeostasisTarget;    // Target homeostasis level
        float plasticityModulation; // How stress affects plasticity
        float arousalBaseline;     // Baseline arousal level
        float stressExponent;       // Exponent for stress-intensity relationship
        float recoveryExponent;     // Exponent for recovery relationship
        
        StressArousalParameters()
            : stressThreshold(0.5f), recoveryRate(0.1f), adaptationTime(10.0f)
            , homeostasisTarget(0.5f), plasticityModulation(0.8f), arousalBaseline(0.3f)
            , stressExponent(2.0f), recoveryExponent(1.5f) {}
    };
    
    StressArousalSystem();
    ~StressArousalSystem();
    
    // Update stress level based on simulation events
    void update(float rewardPredictionError, float novelty, float memoryLoad,
               float cognitiveLoad = 0.0f, float socialInput = 0.0f);
    
    // Get current stress level
    StressLevel getStressLevel() const;
    
    // Get stress intensity (0-1)
    float getStressIntensity() const;
    
    // Get current arousal level
    float getArousalLevel() const;
    
    // Apply stress effects to brain dynamics
    void applyStressEffects(Brain* brain, TimestepDuration dt);
    
    // Configure stress response parameters
    void configureParameters(const StressArousalParameters& params);
    void setStressThreshold(float threshold);
    void setRecoveryRate(float rate);
    void setAdaptationTime(float time);
    void setHomeostasisTarget(float target);
    
    // Get stress response curves
    float getStressPlasticityModulation(float stressLevel) const;
    float getStressAttentionModulation(float stressLevel) const;
    float getStressMemoryModulation(float stressLevel) const;
    
    // Get stress system statistics
    struct StressStats {
        StressLevel currentLevel;
        float intensity;
        float arousal;
        float homeostasisDeviation;
        float timeInCurrentLevel;
        size_t stressEventsCount;
        float averageStressLevel;
        std::vector<float> stressHistory;
        std::unordered_map<std::string, float> stressSources;
        std::unordered_map<std::string, float> recoveryMetrics;
    };
    
    StressStats getStats() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Dopamine: Enhanced version with advanced dynamics
class AdvancedDopamine : public Neuromodulator {
public:
    struct DopamineDynamics {
        float baseline;              // Baseline dopamine level
        float phasicGain;            // Gain for phasic signals
        float tonicLevel;            // Tonic firing rate
        float predictionErrorGain;   // Gain for prediction errors
        float rewardMagnitudeGain;   // Gain for reward magnitude
        float actionSelectionGain;   // Gain for action selection
        float learningRate;          // Learning rate modulation
        float eligibilityTraceDecay; // Decay rate of eligibility traces
        
        DopamineDynamics()
            : baseline(0.0f), phasicGain(1.0f), tonicLevel(0.1f)
            , predictionErrorGain(1.0f), rewardMagnitudeGain(1.0f)
            , actionSelectionGain(1.0f), learningRate(0.01f), eligibilityTraceDecay(0.1f) {}
    };
    
    struct RewardPrediction {
        float predictedReward;
        float actualReward;
        float predictionError;
        float confidence;
        float temporalDifference;
        float creditAssignment;  // How credit is assigned across time
        
        RewardPrediction()
            : predictedReward(0.0f), actualReward(0.0f), predictionError(0.0f)
            , confidence(1.0f), temporalDifference(0.0f), creditAssignment(0.0f) {}
    };
    
    AdvancedDopamine();
    ~AdvancedDopamine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Advanced dopamine functions
    void signalReward(float reward, float predictionError = 0.0f);
    void signalRewardPredictionError(float predictionError);
    void updateRewardPrediction(float actualReward);
    void applyActionSelectionModulation(ActionType action, float value);
    void updateEligibilityTraces(float decayFactor = 1.0f);
    
    // Configuration
    void configureDynamics(const DopamineDynamics& dynamics);
    void setPrediction(float predictedReward);
    void setActionValues(const std::unordered_map<ActionType, float>& actionValues);
    
    // Statistics
    RewardPrediction getCurrentPrediction() const;
    float getActionValue(ActionType action) const;
    float getTemporalDifference() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Acetylcholine: Enhanced attention and memory modulation
class AdvancedAcetylcholine : public Neuromodulator {
public:
    struct AChDynamics {
        float baseline;              // Baseline acetylcholine level
        float attentionGain;         // Gain for attentional processes
        float encodingGain;          // Gain for memory encoding
        float retrievalGain;         // Gain for memory retrieval
        float noveltySensitivity;    // Sensitivity to novel stimuli
        float vigilance;             // Level of vigilance/alertness
        float cholinergicReactivity;  // Reactivity to unexpected events
        float workspaceBias;          // Bias toward working memory vs long-term memory
        
        AChDynamics()
            : baseline(0.0f), attentionGain(1.0f), encodingGain(1.0f), retrievalGain(1.0f)
            , noveltySensitivity(0.5f), vigilance(0.5f), cholinergicReactivity(0.5f)
            , workspaceBias(0.5f) {}
    };
    
    AdvancedAcetylcholine();
    ~AdvancedAcetylcholine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Advanced acetylcholine functions
    void signalNovelty(float noveltyLevel);
    void signalAttentionShift(float fromNeuron, float toNeuron, float strength);
    void enhanceEncoding(const std::vector<float>& pattern, float strength = 1.0f);
    void boostMemoryRetrieval(const std::vector<float>& pattern);
    void applyVigilance(float vigilanceLevel);
    
    // Configuration
    void configureDynamics(const AChDynamics& dynamics);
    void setAttentionFocus(const std::vector<NeuronId>& neurons);
    void setNoveltyThreshold(float threshold);
    void setWorkspaceBias(float bias);  // 0.0 = LTM, 1.0 = WM
    
private:
    struct Impl;
    Impl* pImpl;
};

// Norepinephrine: Enhanced arousal and locus coeruleus function
class AdvancedNorepinephrine : public Neuromodulator {
public:
    struct NEDynamics {
        float baseline;              // Baseline norepinephrine level
        float arousalGain;           // Gain for arousal processes
        float signalReliability;      // Reliability of sensory signals
        float responseGain;          // Response gain for unexpected events
        float sleepWakeRegulation;   // Regulation of sleep-wake cycle
        float stressResponse;        // Stress response amplification
        float cognitiveFocus;        // Level of cognitive focus
        float thalamocorticalGain;    // Gain for thalamocortical communication
        
        NEDynamics()
            : baseline(0.0f), arousalGain(1.0f), signalReliability(1.0f)
            , responseGain(1.0f), sleepWakeRegulation(0.5f), stressResponse(0.5f)
            , cognitiveFocus(0.5f), thalamocorticalGain(1.0f) {}
    };
    
    AdvancedNorepinephrine();
    ~AdvancedNorepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Advanced norepinephrine functions
    void signalUnexpectedEvent(float intensity, float salience = 1.0f);
    void regulateSleepWakeCycle(float sleepPressure, float homeostaticNeed);
    void applyStressAmplification(float stressLevel);
    void enhanceCognitiveFocus(float focusLevel);
    void modulateThalamocorticalActivity(float gain);
    
    // Configuration
    void configureDynamics(const NEDynamics& dynamics);
    void setSignalReliability(float reliability);
    void setResponseGain(float gain);
    void setSleepWakeBalance(float sleepNeed, float wakeNeed);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Serotonin: Enhanced mood and behavioral regulation
class AdvancedSerotonin : public Neuromodulator {
public:
    struct SerotoninDynamics {
        float baseline;              // Baseline serotonin level
        float behavioralInhibition;  // Level of behavioral inhibition
        float impulsivityControl;    // Control over impulsive actions
        float punishmentSensitivity; // Sensitivity to punishment
        float rewardAvailability;    // Perceived reward availability
        float socialBehaviorModulation; // Modulation of social behavior
        float emotionalStability;    // Emotional stability
        float decisionThreshold;     // Decision threshold level
        
        SerotoninDynamics()
            : baseline(0.0f), behavioralInhibition(0.5f), impulsivityControl(0.5f)
            , punishmentSensitivity(0.5f), rewardAvailability(0.5f), socialBehaviorModulation(0.5f)
            , emotionalStability(0.5f), decisionThreshold(0.5f) {}
    };
    
    AdvancedSerotonin();
    ~AdvancedSerotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Advanced serotonin functions
    void signalPunishment(float intensity);
    void modulateBehavioralInhibition(float level);
    void regulateImpulsivity(float level);
    void modifyRewardAvailability(float availability);
    void adjustSocialBehavior(float level);
    void stabilizeEmotion(float stability);
    void adjustDecisionThreshold(float threshold);
    
    // Configuration
    void configureDynamics(const SerotoninDynamics& dynamics);
    void setPunishmentHistory(float history);
    void setRewardHistory(float history);
    void setSocialInput(float socialInput);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Neuromodulator network: Interactions between all neuromodulators
class NeuromodulatorNetwork {
public:
    NeuromodulatorNetwork();
    ~NeuromodulatorNetwork();
    
    // Add neuromodulator to network
    void addNeuromodulator(Neuromodulator* modulator);
    
    // Get combined effect on neural population
    float getCombinedEffect(const std::vector<NeuronId>& neurons) const;
    
    // Update entire network
    void update(TimestepDuration dt);
    
    // Configure network interactions
    void addConnection(Neuromodulator* modulator1, Neuromodulator* modulator2,
                      float weight, float interactionType = 0.0f);
    void removeConnection(Neuromodulator* modulator1, Neuromodulator* modulator2);
    
    // Get network statistics
    struct NetworkStats {
        size_t totalModulators;
        size_t totalConnections;
        float globalEffect;
        float synchronization;  // How synchronized modulators are
        std::unordered_map<std::string, float> modulatorEffects;
        std::vector<std::pair<std::string, std::string>> topConnections;
        float networkCoherence;
        float emergentBehavior;  // Complex behaviors arising from interactions
        std::unordered_map<std::string, float> customMetrics;
    };
    
    NetworkStats getStats() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

#endif // NLM_NEUROMODULATOR_HPP
