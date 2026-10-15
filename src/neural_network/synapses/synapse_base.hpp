#pragma once

#include "../brain/Synapse.hpp"
#include "../core/Types/Types.hpp"
#include <vector>
#include <string>

namespace nlm {

/**
 * Enhanced Synapse - Advanced synaptic model with plasticity and dynamics
 * 
 * Extends the basic Synapse class with:
 * - Advanced plasticity rules (STDP, Hebbian, homeostatic)
 * - Neuromodulatory effects
 * - Short-term plasticity
 * - Eligibility traces
 * - Structural plasticity
 * 
 * Maintains backward compatibility with existing Synapse interface
 */
class EnhancedSynapse : public Synapse {
public:
    EnhancedSynapse(SynapseId id, NeuronId source, NeuronId destination);
    ~EnhancedSynapse() override;

    // ===== ADVANCED PLASTICITY =====

    // Multi-factor plasticity rule
    struct MultiFactorPlasticity {
        float weight;              // Synaptic weight contribution
        float hebbian;            // Hebbian component
        float stdp;               // STDP component
        float reward;             // Reward-modulated component
        float homeostatic;        // Homeostatic component
        float neuromodulatory;     // Neuromodulatory component
    };

    // Multi-factor plasticity parameters
    struct MultiFactorParameters {
        // STDP components
        float stdp_ltp;           // LTP weight change (positive)
        float stdp_ltd;           // LTD weight change (negative)
        float stdp_tau;           // STDP time constant (ms)
        
        // Hebbian components
        float hebbian_learning_rate;    // Hebbian learning rate
        float hebbian_threshold;       // Hebbian threshold
        
        // Reward-modulated components
        float reward_decay;         // Reward trace decay
        float reward_threshold;     // Reward threshold
        
        // Homeostatic components
        float homeostatic_rate;    // Homeostatic adjustment rate
        float homeostatic_target;  // Target firing rate
        float homeostatic_strength;// Homeostatic strength
        
        // Neuromodulatory components
        float neuromodulation_gain; // Neuromodulation gain
        float neuromodulation_tau;  // Neuromodulation time constant
    };

    void setMultiFactorParameters(const MultiFactorParameters& params) { 
        multiFactorParams_ = params; 
    }

    // Calculate plasticity contribution
    MultiFactorPlasticity calculatePlasticity(float preSpikeTime, float postSpikeTime,
                                              float rewardSignal, float neuromodulator) const;

    // ===== META-PLASTICITY =====

    // Plasticity of plasticity (meta-plasticity)
    struct MetaPlasticity {
        float plasticityRate;      // Rate of plasticity change
        float plasticityLevel;     // Current plasticity level
        float plasticityTarget;    // Target plasticity level
        float metaPlasticityTau;   // Meta-plasticity time constant
        float metaLearningRate;    // Rate of meta-plasticity change
    };

    void setMetaPlasticityParams(const MetaPlasticity& params) { 
        metaPlasticityParams_ = params; 
    }

    // Update meta-plasticity
    void updateMetaPlasticity(float deltaPlasticity, TimestepDuration dt);

    // ===== NEUROMODULATORY EFFECTS =====

    // Neuromodulatory influence on plasticity and efficacy
    struct NeuromodulatoryEffects {
        float dopamine;            // Dopamine concentration (0-1)
        float acetylcholine;       // Acetylcholine concentration (0-1)
        float norepinephrine;      // Norepinephrine concentration (0-1)
        float serotonin;           // Serotonin concentration (0-1)
        float cortisol;             // Cortisol concentration (0-1)
    };

    void setNeuromodulatoryEffects(const NeuromodulatoryEffects& effects) { 
        neuromodEffects_ = effects; 
    }

    // Apply neuromodulatory effects
    void applyNeuromodulatoryEffects();

    // ===== SHORT-TERM PLASTICITY =====

    // Short-term plasticity models
    enum class ShortTermPlasticityType {
        DEPRESSION,        // Synaptic depression
        FACILITATION,      // Synaptic facilitation
        BOTH,              // Combined depression and facilitation
        REVERSIBLE,        // Reversible dynamics
    };

    struct ShortTermPlasticity {
        ShortTermPlasticityType type; // Type of STP
        float depressionRate;        // Depression rate (1/ms)
        float recoveryRate;          // Recovery rate (1/ms)
        float facilitationRate;      // Facilitation rate (1/ms)
        float baselineDepression;    // Baseline depression level
        float baselineFacilitation;  // Baseline facilitation level
        float u;                     // Utilization parameter (0-1)
        float x;                     // Facilitation variable (0-1)
        float u_bar;                 // Maximum utilization
    };

    void setShortTermPlasticity(const ShortTermPlasticity& stp) { 
        stp_ = stp; 
    }

    // Update short-term plasticity
    void updateShortTermPlasticity(bool preSpike, float dt);

    // Get effective synaptic weight (including STP)
    float getEffectiveWeight() const;

    // ===== ELIGIBILITY TRACES =====

    // Eligibility traces for reward-modulated learning
    struct EligibilityTrace {
        float value;                // Current trace value
        float decayRate;            // Trace decay rate
        float learningRate;         // Learning rate multiplier
        std::vector<float> history; // Trace history
        size_t maxHistory;          // Maximum history length
    };

    void setEligibilityTraceParams(float decayRate, float learningRate, size_t maxHistory = 100);

    // Update eligibility trace
    void updateEligibilityTrace(bool preSpike, bool postSpike, float reward, float dt);

    // Get eligibility trace value
    float getEligibilityTrace() const { return eligibilityTrace_.value; }

    // ===== STRUCTURAL PLASTICITY =====

    // Structural plasticity for synapse formation/pruning
    struct StructuralPlasticity {
        float formationRate;        // Rate of new synapse formation
        float pruningRate;          // Rate of synapse pruning
        float stability;            // Synapse stability factor
        float noiseLevel;           // Formation/pruning noise
        float threshold;            // Formation threshold
    };

    void setStructuralPlasticityParams(const StructuralPlasticity& params) { 
        structuralPlasticity_ = params; 
    }

    // Determine if synapse should be formed
    bool shouldFormSynapse(float connectionProbability, float dt) const;

    // Determine if synapse should be pruned
    bool shouldPruneSynapse(float stability, float dt) const;

    // ===== NOISE AND VARIABILITY =====

    // Synaptic transmission noise
    struct SynapticNoise {
        float weightNoise;          // Weight noise standard deviation
        float releaseNoise;         // Release probability noise
        float delayNoise;           // Transmission delay noise
        float reversalNoise;        // Reversal potential noise
    };

    void setSynapticNoise(const SynapticNoise& noise) { 
        synapticNoise_ = noise; 
    }

    // Generate stochastic synaptic transmission
    float generateStochasticTransmission(float baseWeight) const;

    // ===== CROSS-MODALITY INTERACTIONS =====

    // Cross-modality interactions (e.g., reward + sensory)
    struct CrossModalityInteraction {
        float rewardSensory;        // Reward + sensory interaction
        float rewardCognitive;      // Reward + cognitive interaction
        float sensoryCognitive;     // Sensory + cognitive interaction
        float rewardLearning;       // Reward-modulated learning
    };

    void setCrossModalityInteraction(const CrossModalityInteraction& interaction) { 
        crossModalityInteraction_ = interaction; 
    }

    // Apply cross-modality interactions
    void applyCrossModalityInteractions(float reward, float sensory, 
                                       float cognitive, float dt);

    // ===== STATISTICS =====

    // Synaptic statistics
    struct SynapseStats {
        float averageWeight;        // Average synaptic weight
        float weightVariance;       // Weight variance
        float plasticChanges;       // Number of plastic changes
        float stpChanges;           // Number of STP changes
        float eligibilityTraceMax;  // Maximum eligibility trace
        float formationEvents;      // Number of formation events
        float pruningEvents;        // Number of pruning events
    };

    SynapseStats getStats() const { return stats_; }

    // Update statistics
    void updateStats(float weightChange, float stpChange, float eligibilityChange,
                     bool formed, bool pruned);

    // ===== UTILITY =====

    // Normalize synaptic weight
    void normalizeWeight(float minWeight = 0.0f, float maxWeight = 1.0f);

    // Get plasticity factor (0-1 scale)
    float getPlasticityFactor() const;

    // ===== COMPATIBILITY =====

    // Override Synapse::step for enhanced dynamics
    void step(Timestamp currentTime) override {
        stepEnhanced(currentTime, 1.0f); // Default dt = 1.0ms
    }

    // Main enhanced step function
    bool stepEnhanced(Timestamp currentTime, TimestepDuration dt);

    // Apply all plasticity rules
    void applyAllPlasticityRules(float preSpikeTime, float postSpikeTime,
                                float reward, Timestamp currentTime);

private:
    // Plasticity parameters
    MultiFactorParameters multiFactorParams_;
    MetaPlasticity metaPlasticityParams_;
    float metaPlasticityLevel_;

    // Neuromodulation
    NeuromodulatoryEffects neuromodEffects_;

    // Short-term plasticity
    ShortTermPlasticity stp_;

    // Eligibility traces
    EligibilityTrace eligibilityTrace_;

    // Structural plasticity
    StructuralPlasticity structuralPlasticity_;

    // Noise parameters
    SynapticNoise synapticNoise_;

    // Cross-modality interactions
    CrossModalityInteraction crossModalityInteraction_;

    // Statistics
    SynapseStats stats_;

    // Helper methods
    float calculateSTDPContribution(float preTime, float postTime) const;
    float calculateHebbianContribution() const;
    float calculateHomeostaticContribution() const;
    float calculateRewardContribution(float reward) const;
    float calculateNeuromodulatoryContribution() const;
    void updateMetaPlasticityLevel(float delta, TimestepDuration dt);
    float generateStochasticWeight(float baseWeight) const;
};

} // namespace nlm
