#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <cmath>
#include <unordered_map>

namespace nlm {

// Forward declaration
class Neuron;

// Advanced synapse types
enum class AdvancedSynapseType : uint8_t {
    Excitatory,
    Inhibitory,
    Modulatory,
    Electrical,           // Electrical synapse (gap junction)
    Neuromodulatory,      // Modulated by neuromodulators
    Hebbian,              // Hebbian learning synapse
    NonHebbian,           // Non-Hebbian synapse
    DynamicPlasticity,    // Dynamic plasticity synapse
    Homeostatic,          // Homeostatic regulation synapse
    RewardModulated,      // Reward-modulated learning synapse
    PredictionError,      // Prediction error signaling synapse
    Structural,           // Structural plasticity synapse
    ActivityDependent,    // Activity-dependent plasticity synapse
    StateDependent,       // State-dependent plasticity synapse
    BCM,                  // Bienenstock-Cooper-Munro sliding threshold
    Oja,                  // Oja's rule
    Sanger,               // Sanger's competitive learning
    NormalizedCut,        // Normalized cut learning
    LayeredPerception,    // Layered perception
    Autoencoder,          // Autoencoder learning
    Adversarial,           // Adversarial learning
    MetaLearning,         // Meta-learning
    TransferLearning,     // Transfer learning
    ContinualLearning,    // Continual learning
    LifelongLearning,     // Lifelong learning
    OnlineLearning,       // Online learning
    BatchLearning,        // Batch learning
    Reinforcement,        // Reinforcement learning
    CuriosityDriven,      // Curiosity-driven learning
    IntrinsicMotivation,  // Intrinsic motivation
    ExtrinsicMotivation,  // Extrinsic motivation
    SocialLearning,       // Social learning
    CollaborativeLearning, // Collaborative learning
    MultiTaskLearning,    // Multi-task learning
};

// Advanced plasticity rules
enum class AdvancedPlasticityRule : uint8_t {
    STDP,                 // Spike-Timing-Dependent Plasticity
    Hebbian,              // Hebbian learning (OUA)
    AntiHebbian,           // Anti-Hebbian learning
    BCM,                  // Bienenstock–Cooper–Munro
    Oja,                  // Oja's rule
    Sanger,               // Sanger's competitive learning
    NormalizedCut,        // Normalized cut learning
    LayeredPerception,    // Layered perception
    Autoencoder,          // Autoencoder learning
    Adversarial,           // Adversarial learning
    MetaLearning,         // Meta-learning
    TransferLearning,     // Transfer learning
    ContinualLearning,    // Continual learning
    LifelongLearning,     // Lifelong learning
    OnlineLearning,       // Online learning
    BatchLearning,        // Batch learning
    Reinforcement,        // Reinforcement learning
    CuriosityDriven,      // Curiosity-driven learning
    IntrinsicMotivation,  // Intrinsic motivation
    ExtrinsicMotivation,  // Extrinsic motivation
    SocialLearning,       // Social learning
    CollaborativeLearning, // Collaborative learning
    MultiTaskLearning,    // Multi-task learning
};

// Synaptic current types
enum class SynapticCurrentType : uint8_t {
    AMPA,                 // Alpha-amino-3-hydroxy-5-methyl-4-isoxazole propionic acid
    NMDA,                 // N-methyl-D-aspartate
    GABA_A,               // GABA_A receptor
    GABA_B,               // GABA_B receptor
    Acetylcholine,        // Muscarinic and nicotinic receptors
    Serotonin,           // Serotonergic receptors
    Dopamine,             // Dopaminergic receptors
    Norepinephrine,       // Noradrenergic receptors
    Glutamate,            // Glutamatergic receptors
    Glycine,               // Glycinergic receptors
    ATP,                  // Purinergic receptors
    VIP,                  // Vasoactive intestinal peptide
    SubstanceP,           // Substance P receptors
    Enkephalin,           // Endogenous opioid peptide
};

// Advanced synapse configuration
struct AdvancedSynapseConfig {
    // Basic properties
    AdvancedSynapseType type;
    SynapticCurrentType currentType;
    float weight;
    float weightLearningRate;
    float weightDecayRate;
    float weightClippingMax;
    float weightClippingMin;
    
    // Temporal dynamics
    float riseTime;
    float decayTime;
    float timeConstant;
    float integrationTime;
    
    // Activity-dependent properties
    float baselineActivity;
    float maxActivity;
    float activityThreshold;
    float activitySensitivity;
    
    // Plasticity properties
    AdvancedPlasticityRule plasticityRule;
    float plasticityRate;
    float eligibilityTraceDecay;
    float stabilizationThreshold;
    
    // Neuromodulation properties
    float neuromodulationStrength;
    float neuromodulationTimeConstant;
    float neuromodulationThreshold;
    bool isNeuromodulated;
    
    // Structural properties
    bool enableStructuralPlasticity;
    float structuralGrowthRate;
    float structuralPruningRate;
    float structuralStability;
    
    // State properties
    float currentEfficacy;
    float eligibilityTrace;
    float lastPreSpikeTime;
    float lastPostSpikeTime;
    float shortTermDepression;
    float shortTermFacilitation;
    
    // Historical properties
    std::vector<float> weightHistory;
    std::vector<float> activityHistory;
    std::vector<float> efficacyHistory;
    
    AdvancedSynapseConfig()
        : type(AdvancedSynapseType::Excitatory)
        , currentType(SynapticCurrentType::AMPA)
        , weight(1.0f)
        , weightLearningRate(0.01f)
        , weightDecayRate(0.0001f)
        , weightClippingMax(1.0f)
        , weightClippingMin(-1.0f)
        , riseTime(0.1f)
        , decayTime(5.0f)
        , timeConstant(1.0f)
        , integrationTime(0.5f)
        , baselineActivity(0.1f)
        , maxActivity(1.0f)
        , activityThreshold(0.5f)
        , activitySensitivity(1.0f)
        , plasticityRule(AdvancedPlasticityRule::STDP)
        , plasticityRate(0.001f)
        , eligibilityTraceDecay(0.1f)
        , stabilizationThreshold(0.01f)
        , neuromodulationStrength(1.0f)
        , neuromodulationTimeConstant(10.0f)
        , neuromodulationThreshold(0.5f)
        , isNeuromodulated(false)
        , enableStructuralPlasticity(false)
        , structuralGrowthRate(0.0001f)
        , structuralPruningRate(0.00001f)
        , structuralStability(0.9f)
        , currentEfficacy(1.0f)
        , eligibilityTrace(0.0f)
        , lastPreSpikeTime(-1.0f)
        , lastPostSpikeTime(-1.0f)
        , shortTermDepression(1.0f)
        , shortTermFacilitation(0.0f) {}
};

// Synapse representing a connection between neurons
// Implements advanced synaptic transmission with delays and plasticity

class Synapse {
public:
    // Create synapse with IDs
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity
    SynapseId getId() const;
    
    // Connection
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    
    // Weight management
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    void applyPlasticityChange(float change);  // For advanced plasticity rules
    
    // Delay (in simulation steps)
    Delay getDelay() const;
    void setDelay(Delay delay);
    
    // Type
    SynapseType getType() const;
    void setType(SynapseType type);
    AdvancedSynapseType getAdvancedType() const;
    void setAdvancedType(AdvancedSynapseType type);
    
    // Current type
    SynapticCurrentType getCurrentType() const;
    void setCurrentType(SynapticCurrentType type);
    
    // Excitatory/Inhibitory
    bool isExcitatory() const;
    bool isInhibitory() const;
    bool isModulatory() const;
    
    // Activity history for plasticity
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    void clearHistory();
    
    // Plasticity flags
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    
    // Enable/disable plasticity rules
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Advanced plasticity configuration
    const AdvancedSynapseConfig& getAdvancedConfig() const;
    AdvancedSynapseConfig& getAdvancedConfig();
    void configureAdvanced(const AdvancedSynapseConfig& config);
    void updateAdvancedConfig();  // Update based on current state
    
    // Eligibility trace (for reward-modulated learning)
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    float getEfficacy() const;
    void setEfficacy(float efficacy);
    void updateEfficacy(float preActivity, float postActivity);  // Activity-dependent
    
    // Advanced synaptic dynamics
    void updateTemporalDynamics(Timestamp currentTime);  // Short-term plasticity
    void updatePlasticity(Timestamp currentTime, const AdvancedSynapseConfig& config);  // Plasticity rules
    void updateNeuromodulation(float neuromodulationSignal);  // Neuromodulation
    void updateStructuralPlasticity(float globalGrowthFactor);  // Structural changes
    
    // BCM theory support (sliding threshold)
    void updateSlidingThreshold(float avgPostActivity, float beta);  // BCM sliding threshold
    float getSlidingThreshold() const;
    
    // Oja's rule support
    void applyOjasRule(const std::vector<float>& neuralActivities, float learningRate);  // Oja's rule
    
    // Sanger's competitive learning support
    void applySangersRule(const std::vector<float>& neuralActivities, float learningRate);  // Sanger's rule
    
    // Adversarial learning support
    void applyAdversarialLearning(float reward, float cost);  // Adversarial learning
    
    // Meta-learning support
    void updateMetaLearningParameters(float metaGradient);  // Meta-learning
    
    // Transfer learning support
    void adaptToNewTask(const AdvancedSynapseConfig& newTaskConfig);  // Transfer learning
    
    // Continual learning support
    void preventCatastrophicForgettting(const std::vector<float>& newActivities);  // Continual learning
    
    // Online learning support
    void updateOnlineLearning(float onlineGradient);  // Online learning
    
    // Reward-modulated learning support
    void updateRewardModulatedLearning(float rewardPredictionError, float neuromodulationStrength);  // Reward-modulated
    
    // Curiosity-driven learning support
    void updateCuriosityDrivenLearning(float curiositySignal, float novelty);  // Curiosity-driven
    
    // Intrinsic motivation support
    void updateIntrinsicMotivation(float intrinsicDrive, float explorationValue);  // Intrinsic motivation
    
    // Extrinsic motivation support
    void updateExtrinsicMotivation(float extrinsicReward, float punishment);  // Extrinsic motivation
    
    // Social learning support
    void updateSocialLearning(float socialInfluence, float peerCohesion);  // Social learning
    
    // Collaborative learning support
    void updateCollaborativeLearning(float collaborationSignal, float groupPerformance);  // Collaborative
    
    // Multi-task learning support
    void updateMultiTaskLearning(const std::vector<float>& taskRewards, float taskSharingFactor);  // Multi-task
    
    // State-dependent plasticity support
    void updateStateDependentPlasticity(float neuralState, float environmentalContext);  // State-dependent
    
    // Update synapse for one simulation step
    // TODO PHASE 2: Implement advanced synaptic dynamics
    void step(Timestamp currentTime);
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng, const AdvancedSynapseConfig& config = AdvancedSynapseConfig());
    
    // Get advanced plasticity rule for this synapse
    AdvancedPlasticityRule getCurrentPlasticityRule() const;
    
    // Check if synapse can undergo plastic changes
    bool isPlastic() const;
    
    // Get synaptic current contribution based on type and efficacy
    SynapticWeight getCurrentContribution() const;
    
    // Update synaptic resources (for different receptor types)
    void updateReceptorResources(float timeStep);
    
    // Get receptor saturation level
    float getReceptorSaturation() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm