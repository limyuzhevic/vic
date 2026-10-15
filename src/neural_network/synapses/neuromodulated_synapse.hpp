#pragma once

#include "synapse_base.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

/**
 * Neuromodulated Synapse
 * 
 * Synapse with dynamic neuromodulatory control:
 * - Neuromodulatory concentration effects
 * - Cross-modality interactions
 * - Dynamic gain modulation
 * - State-dependent plasticity
 * - Synaptic switching
 * 
 * Implements real neuromodulatory effects on synaptic transmission
 */
class NeuromodulatedSynapse : public EnhancedSynapse {
public:
    NeuromodulatedSynapse(SynapseId id, NeuronId source, NeuronId destination);
    ~NeuromodulatedSynapse() override;

    // ===== NEUROMODULATORS =====

    // Neuromodulatory signals
    enum class NeuromodulatorType {
        DOPAMINE,
        ACETYLCHOLINE,
        NORADRENALINE,
        SEROTONIN,
        ENDORPHIN,
        NOCICEPTIN,
        HISTAMINE,
        OREXIN,
        COTICOSTERONE,
        NEUROPEPTIDE_Y
    };

    // Add neuromodulatory signal
    void addNeuromodulator(NeuromodulatorType type, std::shared_ptr<Neuromodulator> modulator);

    // Get neuromodulator
    std::shared_ptr<Neuromodulator> getNeuromodulator(NeuromodulatorType type) const;

    // Get all neuromodulators
    const std::map<NeuromodulatorType, std::shared_ptr<Neuromodulator>>& getNeuromodulators() const {
        return neuromodulators_;
    }

    // ===== NEUROMODULATORY EFFECTS =====

    // Neuromodulatory effects on synaptic transmission
    struct NeuromodulatoryEffects {
        // Weight modulation
        float weightModulation;        // Multiplier for synaptic weight
        float baselineWeight;          // Baseline weight without modulation
        
        // Release probability
        float releaseProbability;      // Probability of neurotransmitter release
        float facilitation;           // Facilitation factor
        float depression;             // Depression factor
        
        // Reversal potential
        float reversalPotential;       // Synaptic reversal potential
        float drivingForce;            // Driving force for current
        
        // Time constants
        float riseTime;                // Synaptic rise time (ms)
        float decayTime;               // Synaptic decay time (ms)
        
        // Dynamics
        float activation;              // Activation level (0-1)
        float inactivation;            // Inactivation level (0-1)
    };

    // Calculate current neuromodulatory effects
    NeuromodulatoryEffects calculateEffects() const;

    // ===== STATE-DEPENDENT PLASTICITY =====

    // State-dependent plasticity
    struct StateDependentPlasticity {
        float stateWeight;             // Weight based on synaptic state
        float stateDependency;         // How much state affects plasticity
        float thresholdState;          // State threshold
        float saturationState;         // Saturation state level
        bool plasticWhenActive;        // Whether synapse is plastic when active
    };

    void setStateDependentPlasticity(const StateDependentPlasticity& params) { 
        stateDependentPlasticity_ = params; 
    }

    // Determine if synapse is in plastic state
    bool isPlasticState() const;

    // ===== DYNAMIC GAIN MODULATION =====

    // Dynamic gain modulation
    struct DynamicGain {
        float gain;                     // Current gain
        float targetGain;               // Target gain
        float gainLearningRate;         // Rate of gain adaptation
        float gainTimeConstant;         // Time constant for gain dynamics
        float saturationGain;           // Maximum gain
        float baselineGain;             // Baseline gain
    };

    void setDynamicGain(const DynamicGain& params) { 
        dynamicGain_ = params; 
    }

    // Update dynamic gain
    void updateDynamicGain(float dt);

    // ===== SYNAPTIC SWITCHING =====

    // Synaptic switching between different states
    enum class SynapticState {
        EXCITATORY,
        INHIBITORY,
        MODULATORY,
        INERT,
        REVERSIBLE,
        SWITCHING,
    };

    SynapticState getSynapticState() const { return synapticState_; }
    void setSynapticState(SynapticState state);

    // Switching parameters
    struct SwitchingParameters {
        float switchThreshold;          // Threshold for switching
        float switchLearningRate;       // Rate of state switching
        float switchTimeConstant;       // Time constant for switching
        float statePersistence;         // Persistence of current state
        float stateWeightRange;         // Weight range for state
    };

    void setSwitchingParameters(const SwitchingParameters& params) { 
        switchingParams_ = params; 
    }

    // ===== CROSS-MODALITY INTERACTIONS =====

    // Cross-modality interactions
    struct CrossModalityInteraction {
        float rewardSensory;            // Reward + sensory interaction
        float rewardCognitive;          // Reward + cognitive interaction
        float sensoryCognitive;         // Sensory + cognitive interaction
        float stressReward;             // Stress + reward interaction
        float noveltyReward;            // Novelty + reward interaction
    };

    void setCrossModalityInteraction(const CrossModalityInteraction& interaction) { 
        crossModalityInteraction_ = interaction; 
    }

    // Apply cross-modality interactions
    void applyCrossModalityInteractions(float reward, float sensory, 
                                       float cognitive, float stress, 
                                       float novelty, float dt);

    // ===== STATISTICAL DYNAMICS =====

    // Statistical dynamics of synaptic transmission
    struct StatisticalDynamics {
        float meanWeight;               // Mean synaptic weight
        float weightVariance;           // Weight variance
        float weightSkewness;           // Weight skewness
        float weightKurtosis;           // Weight kurtosis
        float transmissionProbability;  // Transmission probability
        float releaseVariance;          // Release probability variance
    };

    StatisticalDynamics getStatisticalDynamics() const { return statisticalDynamics_; }

    // Update statistical dynamics
    void updateStatisticalDynamics(float weightChange, bool formed, bool pruned, float dt);

    // ===== PLASTICITY MODULATION =====

    // Neuromodulatory plasticity modulation
    struct PlasticityModulation {
        float baselinePlasticity;       // Baseline plasticity rate
        float neuromodulatoryPlasticity; // Neuromodulatory influence on plasticity
        float activityDependentPlasticity; // Activity-dependent plasticity
        float stateDependentPlasticity;  // State-dependent plasticity
        float interactionPlasticity;    // Interaction-based plasticity
    };

    void setPlasticityModulation(const PlasticityModulation& params) { 
        plasticityModulation_ = params; 
    }

    // Calculate overall plasticity factor
    float calculatePlasticityFactor() const;

    // ===== COHERENCE PLASTICITY =====

    // Coherence-based plasticity (temporal coherence)
    struct CoherencePlasticity {
        float prePostCoherence;         // Pre-post spike coherence
        float coherenceThreshold;       // Coherence threshold
        float coherenceLearningRate;    // Learning rate for coherence
        float coherenceTimeConstant;    // Coherence time constant
        float coherenceWeight;          // Coherence weight in plasticity
    };

    void setCoherencePlasticity(const CoherencePlasticity& params) { 
        coherencePlasticity_ = params; 
    }

    // Calculate coherence
    float calculateSpikeCoherence(float preSpikeTime, float postSpikeTime) const;

    // ===== REVERSIBLE PLASTICITY =====

    // Reversible synaptic changes
    struct ReversiblePlasticity {
        float reversibleWeight;         // Reversible component of weight
        float irreversibleWeight;       // Irreversible component
        float reversibleRate;           // Rate of reversible changes
        float irreversibleRate;         // Rate of irreversible changes
        float reversibilityThreshold;   // Threshold for reversibility
        float persistenceTime;          // Persistence time constant
    };

    void setReversiblePlasticity(const ReversiblePlasticity& params) { 
        reversiblePlasticity_ = params; 
    }

    // Split weight into reversible and irreversible components
    void splitWeight(float& reversibleWeight, float& irreversibleWeight) const;

    // ===== STATISTICS =====

    // Neuromodulated synapse statistics
    struct NeuromodulatedSynapseStats {
        float averageModulation;        // Average neuromodulatory effect
        float stateTransitionRate;      // Rate of state transitions
        float plasticityEfficiency;     // Efficiency of plasticity
        float gainStability;            // Stability of gain modulation
        float coherenceStrength;        // Strength of spike coherence
        float crossModalityIntegration; // Integration of cross-modality signals
    };

    NeuromodulatedSynapseStats getStats() const { return stats_; }

    // Update statistics
    void updateStats(const NeuromodulatoryEffects& effects, bool stateChanged,
                    float coherence, float crossModality, float dt);

    // ===== UTILITY =====

    // Get current neuromodulatory level
    float getNeuromodulatoryLevel(NeuromodulatorType type) const;

    // Apply neuromodulatory effect
    void applyNeuromodulatoryEffect(NeuromodulatorType type, float effect);

    // Get effective synaptic weight with all modulations
    float getEffectiveWeightWithModulations() const;

    // ===== OVERRIDE =====

    // Override EnhancedSynapse::step
    void step(Timestamp currentTime) override {
        stepEnhanced(currentTime, 1.0f); // Default dt = 1.0ms
    }

    // Main enhanced step function
    bool stepEnhanced(Timestamp currentTime, TimestepDuration dt);

    // Apply all neuromodulatory effects
    void applyAllNeuromodulatoryEffects();

    // Update synaptic state
    void updateSynapticState(TimestepDuration dt);

    // Calculate state-dependent factors
    float calculateStateDependentFactors() const;

private:
    // Neuromodulators
    std::map<NeuromodulatorType, std::shared_ptr<Neuromodulator>> neuromodulators_;

    // State-dependent plasticity
    StateDependentPlasticity stateDependentPlasticity_;

    // Dynamic gain
    DynamicGain dynamicGain_;

    // Synaptic state
    SynapticState synapticState_;
    float stateChangeTimer_;

    // Cross-modality interactions
    CrossModalityInteraction crossModalityInteraction_;

    // Statistical dynamics
    StatisticalDynamics statisticalDynamics_;

    // Plasticity modulation
    PlasticityModulation plasticityModulation_;

    // Coherence plasticity
    CoherencePlasticity coherencePlasticity_;

    // Reversible plasticity
    ReversiblePlasticity reversiblePlasticity_;

    // Switching parameters
    SwitchingParameters switchingParams_;

    // Statistics
    NeuromodulatedSynapseStats stats_;

    // Helper methods
    NeuromodulatoryEffects calculateNeuromodulatoryEffects() const;
    float calculateDynamicGainFactor() const;
    bool checkStateTransitionConditions() const;
    float calculateCrossModalityFactor(float reward, float sensory, 
                                      float cognitive, float stress, float novelty) const;
    void updateStatisticalDynamics(float weightChange, bool formed, bool pruned, float dt);
    void splitWeightComponents(float currentWeight, float& reversible,
                               float& irreversible) const;
    float calculateCoherenceEffect(float preSpikeTime, float postSpikeTime, 
                                   float dt) const;
};

} // namespace nlm
