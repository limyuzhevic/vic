#pragma once

#include "neuron_base.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

/**
 * Adaptive Exponential Integrate-and-Fire (AdEx) Neuron
 * 
 * Advanced neuron model with:
 * - Adaptive exponential subthreshold dynamics
 * - Spike-triggered adaptation
 * - Resonateable properties
 * - Stochastic dynamics
 * 
 * Reference: Teh et al. (2008) "A Framework for Neural Dynamics Simulation"
 */
class AdaptiveNeuron : public EnhancedNeuron {
public:
    AdaptiveNeuron(NeuronId id);
    ~AdaptiveNeuron() override;

    // ===== ADAPTIVE EXPONENTIAL DYNAMICS =====

    // AdEx model parameters
    struct AdExParameters {
        // Subthreshold dynamics
        float delta_v;     // Slope of threshold at spike initiation (mV/ms)
        float tau_mem;     // Membrane time constant (ms)
        float C_m;         // Membrane capacitance (pF)
        float I_e;         // Constant input current (pA)
        
        // Spike-triggered adaptation
        float a;           // Subthreshold adaptation (pA)
        float b;           // Spike-triggered adaptation (pA)
        float tau_w;       // Adaptation time constant (ms)
        
        // Resonance properties
        float g_L;         // Leak conductance (nS)
        float E_L;         // Leak reversal potential (mV)
        float tau_syn;     // Synaptic time constant (ms)
    };

    void setAdExParameters(const AdExParameters& params) { adexParams_ = params; }
    const AdExParameters& getAdExParameters() const { return adexParams_; }

    // ===== RESONANT PROPERTIES =====

    // Hodgkin-Huxley ion channel parameters
    struct IonChannelParameters {
        // Sodium channel
        float g_Na;        // Maximum sodium conductance (mS/cm²)
        float E_Na;        // Sodium reversal potential (mV)
        float m_inf;       // Steady-state activation
        float tau_m;       // Time constant for m
        float h_inf;       // Steady-state inactivation
        float tau_h;       // Time constant for h
        
        // Potassium channel
        float g_K;         // Maximum potassium conductance (mS/cm²)
        float E_K;         // Potassium reversal potential (mV)
        float n_inf;       // Steady-state activation
        float tau_n;       // Time constant for n
    };

    void setIonChannelParameters(const IonChannelParameters& params) { 
        ionChannelParams_ = params; 
    }

    // ===== STOCHASTIC DYNAMICS =====

    // Stochastic current injection parameters
    struct StochasticParameters {
        float sigma;        // Noise standard deviation
        float dt;          // Time step for stochastic process
        float correlation;  // Correlation with other neurons
    };

    void setStochasticParameters(const StochasticParameters& params) { 
        stochasticParams_ = params; 
    }

    // ===== META-PLASTICITY =====

    // Plasticity of plasticity (meta-plasticity) parameters
    struct MetaPlasticityParameters {
        float plasticityRate;     // Rate of plasticity change
        float baselinePlasticity; // Baseline plasticity level
        float metaTau;            // Meta-plasticity time constant
        float metaThreshold;      // Threshold for meta-plasticity
    };

    void setMetaPlasticityParameters(const MetaPlasticityParameters& params) { 
        metaPlasticityParams_ = params; 
    }

    // Current meta-plasticity value
    float getMetaPlasticity() const { return metaPlasticityLevel_; }
    void setMetaPlasticity(float level) { metaPlasticityLevel_ = level; }

    // ===== HOMEOSTATIC PLASTICITY =====

    // Homeostatic plasticity parameters
    struct HomeostaticPlasticityParameters {
        float targetRate;          // Target firing rate (Hz)
        float learningRate;        // Plasticity learning rate
        float tau;                 // Time constant for homeostatic adjustment
        float baselineWeight;      // Baseline synaptic weight
        float weightRange;         // Weight range [0, range]
    };

    void setHomeostaticPlasticityParameters(const HomeostaticPlasticityParameters& params) { 
        homeostaticParams_ = params; 
    }

    // Current firing rate for homeostatic plasticity
    float getFiringRate() const { return firingRate_; }
    void setFiringRate(float rate) { firingRate_ = rate; }

    // ===== NEUROMODULATORY EFFECTS =====

    // Neuromodulatory influence on plasticity
    struct Neuromodulation {
        float dopamine;    // Dopamine level (0-1)
        float acetylcholine;  // Acetylcholine level (0-1)
        float norepinephrine; // Norepinephrine level (0-1)
        float serotonin;   // Serotonin level (0-1)
    };

    void setNeuromodulation(const Neuromodulation& mod) { neuromod_ = mod; }
    const Neuromodulation& getNeuromodulation() const { return neuromod_; }

    // ===== UPDATE METHODS =====

    // Update AdEx dynamics
    void updateAdExDynamics(TimestepDuration dt);

    // Update ion channels
    void updateIonChannels(TimestepDuration dt);

    // Update stochastic process
    void updateStochasticProcess(TimestepDuration dt);

    // Update meta-plasticity
    void updateMetaPlasticity(TimestepDuration dt);

    // Update homeostatic plasticity
    void updateHomeostaticPlasticity(TimestepDuration dt);

    // Calculate synaptic current
    float calculateSynapticCurrent() const;

    // ===== ENHANCED STEP =====

    // Main update function
    bool stepAdvanced(Timestamp currentTime, TimestepDuration dt);

    // ===== BACKWARD COMPATIBILITY =====

    // Override EnhancedNeuron::step
    void step(Timestamp currentTime) override {
        stepAdvanced(currentTime, 1.0f); // Default dt = 1.0ms
    }

private:
    // State variables
    float v_m;            // Membrane potential (mV)
    float w;              // Adaptation current (pA)
    float u;              // Recovery variable
    
    // Ion channel state
    float m, h, n;        // Gating variables
    
    // Stochastic components
    float noise;          // Stochastic current
    float deterministic;  // Deterministic input
    
    // Plasticity variables
    float metaPlasticityLevel_;
    float homeostaticError_;
    
    // Rate calculation
    float firingRate_;

    // Helper methods
    void initializeIonChannels();
    float getMembraneCurrent() const;
    float getAdaptationCurrent() const;
    bool checkThreshold(const AdExParameters& params) const;
    void updateGatingVariables(TimestepDuration dt);
};

} // namespace nlm
