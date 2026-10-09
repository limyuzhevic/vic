#pragma once

#include "../brain/Synapse.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <functional>

namespace nlm {

/**
 * Custom Plasticity Rule implementation
 * User-defined STDP and Hebbian learning rules
 */
class CustomPlasticityRuleImpl {
public:
    struct PlasticityConfig {
        CustomPlasticityRule::RuleType type;
        float learningRate;
        float stdpTauPre;   // Pre-synaptic spike trace decay
        float stdpTauPost;  // Post-synaptic spike trace decay
        float hebbianAlpha; // Hebbian learning rate
        float homeostaticTarget;  // Target firing rate
        float homeostaticRate;    // Homeostatic learning rate
        float neuromodulatorWeight; // Weight for neuromodulator influence
        bool enableDepression;     // Enable LTD in STDP
        bool enableHebbian;        // Enable Hebbian component
        std::string timeVaryingFunction; // For time-varying rules
        std::map<std::string, float> customParams; // Custom parameters
    };
    
    CustomPlasticityRuleImpl(const PlasticityConfig& config);
    ~CustomPlasticityRuleImpl() = default;
    
    // Update synaptic weight based on pre and post spike times
    void update(Synapse* synapse,
                const std::vector<float>& preSpikeTimes,
                const std::vector<float>& postSpikeTimes,
                float dt);
    
    // Get the plasticity type
    CustomPlasticityRule::RuleType getType() const { return config_.type; }
    
    // Set the plasticity type
    void setType(CustomPlasticityRule::RuleType type) { config_.type = type; }
    
    // Get parameters
    const PlasticityConfig& getParams() const { return config_; }
    
    // Set parameters
    void setParams(const PlasticityConfig& params) { config_ = params; }
    
    // Calculate weight change based on rule type
    float calculateWeightChange(Synapse* synapse,
                                const std::vector<float>& preSpikeTimes,
                                const std::vector<float>& postSpikeTimes,
                                float dt) const;
    
private:
    PlasticityConfig config_;
    
    // Rule-specific implementations
    float applySTDP(const std::vector<float>& preSpikeTimes,
                    const std::vector<float>& postSpikeTimes,
                    float dt) const;
    
    float applyHebbian(const std::vector<float>& preSpikeTimes,
                       const std::vector<float>& postSpikeTimes,
                       float dt) const;
    
    float applyBCM(const std::vector<float>& postSpikeTimes,
                   float dt) const;
    
    float applyOja(const std::vector<float>& preSpikeTimes,
                   const std::vector<float>& postSpikeTimes,
                   float dt) const;
    
    float applyTimeVarying(const std::vector<float>& preSpikeTimes,
                           const std::vector<float>& postSpikeTimes,
                           float dt) const;
};

/**
 * Dynamic STDP implementation
 * Time-varying spike-timing dependent plasticity
 */
class DynamicSTDPImpl {
public:
    struct DynamicSTDPConfig {
        float baseLTP;                     // Base long-term potentiation rate
        float baseLTD;                     // Base long-term depression rate
        float timeVaryingFactor;           // Time-varying modulation factor
        float neuromodulatorSensitivity;    // Sensitivity to neuromodulators
        float activityHistorySize;         // Size of activity history
        bool enableMetaplasticity;        // Enable metaplastic changes
        float metaplasticRate;             // Metaplastic learning rate
        std::string timeFunction;          // Time function for LTP/LTD modulation
    };
    
    DynamicSTDPImpl(const DynamicSTDPConfig& config);
    ~DynamicSTDPImpl() = default;
    
    // Update STDP based on time-varying factors
    void update(Synapse* synapse,
                const std::vector<float>& preSpikeTimes,
                const std::vector<float>& postSpikeTimes,
                float dt,
                float neuromodulatorLevel = 1.0f);
    
    // Get current STDP configuration
    const DynamicSTDPConfig& getConfig() const { return config_; }
    
    // Set STDP configuration
    void setConfig(const DynamicSTDPConfig& config) { config_ = config; }
    
    // Calculate time-varying modulation factor
    float getTimeVaryingModulation(float timeSinceLastUpdate) const;
    
    // Get activity-dependent plasticity factor
    float getActivityDependentFactor(const std::vector<float>& recentActivity) const;
    
private:
    DynamicSTDPConfig config_;
    float lastUpdateTime_;
    float accumulatedActivity_;
    std::vector<float> activityHistory_;
    
    // Time function implementations
    float exponentialTimeFunction(float t, float tau) const;
    float gaussianTimeFunction(float t, float sigma) const;
    float linearTimeFunction(float t) const;
};

/**
 * Serotonin neuromodulation
 * Complex serotonin signaling dynamics
 */
class SerotoninImpl {
public:
    struct SerotoninConfig {
        float baseline;                     // Baseline serotonin level
        float peak;                         // Peak serotonin level
        float decayRate;                    // Decay rate
        float synthesisRate;                // Synthesis rate
        float releaseRate;                  // Release rate
        float reuptakeRate;                 // Reuptake rate
        float autoreceptorSensitivity;      // Sensitivity to serotonin levels
        float neuromodulatorStrength;        // Strength of neuromodulatory effects
        float tonicMode;                    // Tonic firing mode
        float phasicMode;                   // Phasic firing mode
        float rewardPredictionErrorWeight;   // Weight for reward prediction error
        float punishmentWeight;              // Weight for punishment signals
        std::string modulationFunction;     // Modulation function
    };
    
    SerotoninImpl(const SerotoninConfig& config);
    ~SerotoninImpl() = default;
    
    // Update serotonin dynamics
    void update(float dt);
    
    // Signal reward
    void signalReward(float reward);
    
    // Signal punishment
    void signalPunishment(float punishment);
    
    // Signal reward prediction error
    void signalRewardPredictionError(float error);
    
    // Get current serotonin level
    float getLevel() const { return level_; }
    
    // Set serotonin level
    void setLevel(float level) { level_ = std::clamp(level, 0.0f, peak_); }
    
    // Get serotonin configuration
    const SerotoninConfig& getConfig() const { return config_; }
    
    // Set serotonin configuration
    void setConfig(const SerotoninConfig& config) { config_ = config; }
    
    // Calculate neuromodulatory effect on neural excitability
    float getExcitabilityEffect() const;
    
    // Calculate neuromodulatory effect on plasticity
    float getPlasticityEffect() const;
    
    // Calculate neuromodulatory effect on memory
    float getMemoryEffect() const;
    
private:
    SerotoninConfig config_;
    float level_;                           // Current serotonin level
    float burstLevel_;                      // Burst level
    float tonicLevel_;                      // Tonic level
    float recentActivity_;                  // Recent activity for dynamic regulation
    float rewardPredictionError_;           // Accumulated reward prediction error
    float punishmentAccumulator_;           // Accumulated punishment signals
    
    // State tracking
    enum class SerotoninState {
        Tonic,
        Phasic,
        Burst,
        Depression,
        Recovery
    };
    
    SerotoninState state_;
    float stateTime_;                       // Time spent in current state
    
    // Dynamic regulation
    float getModulationFactor(float time) const;
    void updateState(float time, float activity);
};

/**
 * Custom reward function implementation
 * Complex reward shaping beyond simple scalar values
 */
class CustomRewardImpl {
public:
    struct RewardConfig {
        std::string functionType;            // Type of reward function
        float weight;                        // Weight in combined reward
        bool enableGradient;                 // Whether to compute reward gradients
        std::string complexity;              // Complexity level (simple, medium, complex)
        std::vector<std::string> inputFeatures; // Features to include
        bool useTemporalAbstraction;         // Whether to use temporal abstraction
        float temporalDiscount;              // Temporal discount factor
        bool enableIntrinsicMotivation;      // Whether to include intrinsic motivation
        float intrinsicWeight;               // Weight for intrinsic motivation
        std::string intrinsicFunction;       // Intrinsic motivation function
        bool enableExtrinsicMotivation;      // Whether to include extrinsic motivation
        float extrinsicWeight;               // Weight for extrinsic motivation
        std::map<std::string, float> extrinsicWeights; // Weights for different extrinsic factors
        std::string explorationBonus;        // Exploration bonus function
        bool enableMetadata;                 // Whether to include metadata in reward
    };
    
    CustomRewardImpl(const RewardConfig& config);
    ~CustomRewardImpl() = default;
    
    // Compute reward based on state
    float compute(const std::vector<float>& state);
    
    // Compute gradient of reward w.r.t. state
    std::vector<float> computeGradient(const std::vector<float>& state);
    
    // Set reward configuration
    void setConfig(const RewardConfig& config) { config_ = config; }
    
    // Get reward configuration
    const RewardConfig& getConfig() const { return config_; }
    
    // Get reward statistics
    struct RewardStats {
        float currentReward;                 // Current reward
        float averageReward;                 // Average reward over last N steps
        float maxReward;                     // Maximum reward
        float minReward;                     // Minimum reward
        float stdDeviation;                  // Standard deviation of reward
        float temporalVariation;             // Variation over time
        float explorationBonus;              // Exploration bonus contribution
        float intrinsicMotivation;           // Intrinsic motivation contribution
        float extrinsicMotivation;           // Extrinsic motivation contribution
    };
    
    RewardStats getStats() const;
    
    // Reset statistics
    void resetStats();
    
private:
    RewardConfig config_;
    RewardStats stats_;
    
    // Reward function implementations
    float computeSimpleReward(const std::vector<float>& state) const;
    float computeComplexReward(const std::vector<float>& state) const;
    float computeIntrinsicallyMotivatedReward(const std::vector<float>& state) const;
    float computeExtrinsicallyMotivatedReward(const std::vector<float>& state) const;
    
    // Gradient computations
    std::vector<float> computeSimpleRewardGradient(const std::vector<float>& state) const;
    std::vector<float> computeComplexRewardGradient(const std::vector<float>& state) const;
    
    // Temporal abstraction
    float computeTemporalAbstraction(const std::vector<float>& state) const;
    float computeTemporalDiscount(float reward, float time) const;
    
    // Exploration bonus
    float computeExplorationBonus(const std::vector<float>& state) const;
    
    // Metadata
    std::vector<float> extractMetadata(const std::vector<float>& state) const;
};

} // namespace nlm
