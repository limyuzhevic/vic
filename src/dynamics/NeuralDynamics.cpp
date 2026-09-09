#include "NeuralDynamics.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// ==================== ADVANCED LIF DYNAMICS ====================

struct AdvancedLIFDynamics {
    // Neuronal biophysics parameters
    struct NeuronParameters {
        float V_rest;             // Resting potential (mV)
        float V_th;               // Threshold potential (mV)
        float V_reset;            // Reset potential (mV)
        float V_peak;             // Peak potential during spike (mV)
        float tau_m;              // Membrane time constant (ms)
        float g_L;                // Leak conductance (nS)
        float C_m;                // Membrane capacitance (pF)
        float inputResistance;    // Input resistance (MOhm)
        float adaptationConductance; // Adaptation conductance (nS)
        float adaptationTimeConstant; // Adaptation time constant (ms)
        float calciumTimeConstant;    // Calcium dynamics time constant (ms)
        float noiseLevel;          // Synaptic noise standard deviation (mV)
        float noiseCorrelation;    // Noise correlation coefficient
        float RMP;                // Resting membrane potential (mV)
        
        // Neuron type specific parameters
        NeuronType neuronType;     // Regular spiking, bursting, adapting, etc.
        float firingRateMax;       // Maximum firing rate (Hz)
        float minimumISI;          // Minimum interspike interval (ms)
        
        NeuronParameters()
            : V_rest(-70.0f), V_th(-55.0f), V_reset(-75.0f), V_peak(40.0f)
            , tau_m(20.0f), g_L(10.0f), C_m(200.0f), inputResistance(1000.0f)
            , adaptationConductance(0.0f), adaptationTimeConstant(100.0f)
            , calciumTimeConstant(50.0f), noiseLevel(1.0f), noiseCorrelation(0.0f)
            , RMP(-70.0f), neuronType(NeuronType::Excitatory), firingRateMax(200.0f)
            , minimumISI(5.0f) {}
    };
    
    // Short-term plasticity states
    struct STPState {
        float u;                  // Utilization (fraction of resources used)
        float x;                  // Resource availability (fraction)
        float tau_f;              // Facilitation time constant (ms)
        float tau_d;              // Depression time constant (ms)
        float U;                  // Baseline utilization probability
        float A;                  // Scaling factor for dynamics
        
        STPState() : u(0.0f), x(1.0f), tau_f(100.0f), tau_d(800.0f), U(0.05f), A(1.0f) {}
    };
    
    // Synaptic reversal potentials
    static constexpr float E_exc = 0.0f;     // Excitatory reversal potential (mV)
    static constexpr float E_inh = -75.0f;   // Inhibitory reversal potential (mV)
    static constexpr float E_mod = -50.0f;   // Modulatory reversal potential (mV)
    
    // ==================== MAIN IMPLEMENTATION ====================
    
    static void updateMembranePotential(Neuron* neuron, NeuronParameters& params, 
                                        TimestepDuration dt, RandomGenerator& rng) {
        // Get current state
        auto& state = neuron->getState();
        float V = state.membranePotential;
        
        // Calculate total synaptic current
        float I_syn = 0.0f;
        if (state.synapseConductance > 0.0f) {
            // Weighted sum of synaptic conductances
            float totalConductance = state.synapseConductance;
            
            // Estimate reversal potential based on net excitation/inhibition
            float netExcitation = 1.0f; // Simplified - could be based on specific synapse types
            float E_syn = netExcitation > 0 ? E_exc : (netExcitation < 0 ? E_inh : E_mod);
            
            // Ohm's law: I = g * (V - E)
            I_syn = totalConductance * (E_syn - V);
        }
        
        // Add external injected current
        float I_inj = neuron->getTotalCurrent();
        
        // Calculate noise input (stochastic neural noise)
        float noise = rng.normal(0.0f, params.noiseLevel);
        
        // Membrane equation: C*dV/dt = -gL*(V-VL) + I_syn + I_inj + noise
        // Convert to Euler integration
        float dV_leak = -params.g_L * (V - params.V_rest);
        float dV_total = (I_syn + I_inj + noise + dV_leak) * dt / params.C_m;
        
        // Update membrane potential
        float V_new = V + dV_total;
        
        // Apply adaptation current (spike-frequency adaptation)
        if (state.refractoryRemaining == 0 && state.adaptationVariable > 0.0f) {
            // Adaptation current increases with each spike and decays exponentially
            float tau_adapt = params.adaptationTimeConstant;
            float decay = std::exp(-dt / tau_adapt);
            float adaptChange = -state.adaptationVariable * (1.0f - decay);
            V_new += adaptChange;
        }
        
        // Apply calcium dynamics for plasticity
        if (neuron->getLastSpikeTime() >= 0.0f) {
            // Simplified calcium dynamics
            float Ca = state.adaptationVariable; // Using adaptation variable as proxy
            float Ca_decay = std::exp(-dt / params.calciumTimeConstant);
            float Ca_influx = params.firingRateMax * dt; // Simplified calcium influx per spike
            state.adaptationVariable = Ca * Ca_decay + Ca_influx;
        }
        
        // Clamp to reasonable bounds
        V_new = std::clamp(V_new, params.V_reset, params.V_peak);
        neuron->setMembranePotential(V_new);
        
        // Update firing rate estimate
        state.firingRate = calculateFiringRate(V_new, params.firingRateMax);
    }
    
    static float calculateFiringRate(float V, float maxRate) {
        // Convert voltage to firing rate (sigmoidal relationship)
        if (V <= -60.0f) return 0.0f;
        if (V >= 40.0f) return maxRate;
        
        float normalizedV = (V + 60.0f) / 100.0f; // Normalize to [0,1]
        normalizedV = std::clamp(normalizedV, 0.0f, 1.0f);
        
        // Sigmoidal rate-voltage relationship
        return maxRate * (1.0f / (1.0f + std::exp(-10.0f * (normalizedV - 0.5f))));
    }
    
    static void handleSpike(Neuron* neuron, const NeuronParameters& params, 
                           Timestamp currentTime, RandomGenerator& rng) {
        auto& state = neuron->getState();
        
        // Record spike with precise timing
        neuron->recordSpike(currentTime);
        
        // Set firing state
        neuron->setFiringState(FiringState::Active);
        
        // Implement spike-frequency adaptation
        // Increase adaptation variable (w) after each spike
        float adaptationIncrement = params.adaptationConductance * 
                                  rng.uniformReal(0.8f, 1.2f); // Biological variability
        state.adaptationVariable += adaptationIncrement;
        
        // Reset membrane potential to peak value for spike generation
        neuron->setMembranePotential(params.V_peak);
        
        // Calculate refractory period duration
        // Base refractory period + adaptation-dependent prolongation
        float baseRefractory = 2.0f;  // ms
        float adaptRefractory = std::min(10.0f, state.adaptationVariable * 0.5f); // ms
        uint32_t refractorySteps = static_cast<uint32_t>(std::ceil((baseRefractory + adaptRefractory) / 1.0f)); // Assuming dt = 1ms
        
        neuron->setRefractoryPeriod(refractorySteps);
        
        // Update neuron type specific properties
        switch (params.neuronType) {
            case NeuronType::RegularSpiking:
                // Regular spiking neurons have fixed refractory period
                break;
            case NeuronType::Bursting:
                // Bursty neurons have shorter initial refractory
                neuron->setRefractoryPeriod(static_cast<uint32_t>(std::ceil(1.0f / 1.0f)));
                break;
            case NeuronType::Adapting:
                // Adapting neurons have longer refractory due to adaptation
                neuron->setRefractoryPeriod(refractorySteps + 2);
                break;
            default:
                // Standard refractory handling
                break;
        }
        
        // Update spike history for STDP
        for (SynapseHandle handle : neuron->getOutgoingSynapses()) {
            // This would be handled by the SpikeSystem
        }
    }
    
    static void updateShortTermPlasticity(Synapse* synapse, STPState& stp, 
                                         TimestepDuration dt, RandomGenerator& rng) {
        // Short-term plasticity dynamics (Tsodyks-Markram model)
        float dtMs = static_cast<float>(dt) * 1000.0f; // Convert to ms
        
        // Depression: x decays with time constant tau_d
        float dx_dT = (1.0f - stp.x) / stp.tau_d;
        float dx = dx_dT * dtMs;
        stp.x = stp.x + dx;
        
        // Facilitation: u increases with spikes and decays with tau_f
        float du_dT = (stp.U * (1.0f - stp.u) - stp.u / stp.tau_f);
        float du = du_dT * dtMs;
        stp.u = stp.u + du;
        
        // Clamp to reasonable ranges
        stp.u = std::clamp(stp.u, 0.0f, 1.0f);
        stp.x = std::clamp(stp.x, 0.0f, 1.0f);
    }
    
    static SynapticWeight calculateEffectiveWeight(Synapse* synapse, 
                                                  const STPState& stp,
                                                  const NeuronParameters& preParams,
                                                  const NeuronParameters& postParams) {
        // Calculate effective synaptic weight including short-term plasticity
        float weight = synapse->getWeight();
        
        // Apply short-term plasticity modulation
        float stpFactor = stp.u * stp.x;
        
        // Apply neuron-specific scaling based on input resistance
        float inputResistScaling = std::pow(postParams.inputResistance / 1000.0f, 0.5f); // Normalization
        
        // Apply activity-dependent depression/facilitation
        float efficacy = synapse->getEfficacy();
        
        return weight * stpFactor * inputResistScaling * efficacy;
    }
    
    static void updateSynapticInput(Synapse* synapse, Neuron* neuron,
                                  const STPState& stp,
                                  Timestamp currentTime, TimestepDuration dt,
                                  RandomGenerator& rng) {
        if (synapse->getDelay() == 0) {
            // Direct synaptic transmission (no delay)
            SynapticWeight effectiveWeight = calculateEffectiveWeight(synapse, stp, 
                                                                    *synapse->getSourceNeuron()->getParameters(),
                                                                    *neuron->getParameters());
            
            if (synapse->isExcitatory()) {
                neuron->receiveExcitatoryInput(static_cast<MembranePotential>(effectiveWeight));
            } else if (synapse->isInhibitory()) {
                neuron->receiveInhibitoryInput(static_cast<MembranePotential>(std::abs(effectiveWeight)));
            } else {
                neuron->receiveModulatoryInput(static_cast<MembranePotential>(effectiveWeight));
            }
        } else {
            // Delayed synaptic transmission
            // This would be handled by the SpikeSystem for proper timing
        }
        
        // Update STP state
        updateShortTermPlasticity(synapse, const_cast<STPState&>(stp), dt, rng);
    }
    
    static bool shouldFire(const Neuron* neuron, const NeuronParameters& params, Timestamp currentTime) {
        const auto& state = neuron->getState();
        
        // Standard threshold crossing
        if (state.refractoryRemaining == 0 && state.membranePotential >= params.V_th) {
            return true;
        }
        
        // Additional firing conditions for different neuron types
        switch (params.neuronType) {
            case NeuronType::Bursting:
                // Bursty neurons can fire with lower threshold during burst
                if (state.refractoryRemaining == 0 && state.membranePotential >= params.V_th - 5.0f) {
                    // Check if we're in a burst pattern
                    if (state.lastSpikeTime >= 0.0f && 
                        (currentTime - state.lastSpikeTime) < 10.0f) {
                        return true;
                    }
                }
                break;
            case NeuronType::Adapting:
                // Adapting neurons have adaptive threshold
                float adaptiveThreshold = params.V_th + state.adaptationVariable * 0.5f;
                if (state.refractoryRemaining == 0 && state.membranePotential >= adaptiveThreshold) {
                    return true;
                }
                break;
            default:
                break;
        }
        
        return false;
    }
    
    static void resetNeuronState(Neuron* neuron, const NeuronParameters& params) {
        auto& state = neuron->getState();
        
        // Reset membrane potential
        neuron->setMembranePotential(params.V_reset);
        
        // Reset adaptation variable with some recovery
        state.adaptationVariable *= 0.1f; // Partial recovery
        
        // Reset firing state
        neuron->setFiringState(FiringState::Resting);
        
        // Clear total current
        neuron->clearTotalCurrent();
    }
};

// ==================== INTEGRATE-AND-FIRE DYNAMICS CLASS ============

struct IntegrateAndFireDynamics::Impl {
    // Main LIF dynamics engine
    AdvancedLIFDynamics::NeuronParameters defaultParameters;
    
    // Random number generator for biological noise
    RandomGenerator rng;
    
    // Synaptic plasticity mechanisms
    float stdpLearningRate;    // Spike timing dependent plasticity rate
    float hebbianLearningRate; // Hebbian plasticity rate
    
    // Short-term plasticity defaults
    AdvancedLIFDynamics::STPState defaultSTP;
    
    Impl() 
        : defaultParameters()
        , rng(42)  // Fixed seed for reproducibility
        , stdpLearningRate(0.01f)
        , hebbianLearningRate(0.005f)
        , defaultSTP() {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    const auto& state = neuron->getState();
    
    // Use default parameters if no neuron-specific configuration exists
    AdvancedLIFDynamics::NeuronParameters params = pImpl->defaultParameters;
    
    // Calculate total membrane potential change using advanced dynamics
    AdvancedLIFDynamics::updateMembranePotential(neuron, params, dt, pImpl->rng);
    
    // Check for spike generation with advanced logic
    Timestamp currentTime = 0.0; // In real implementation, would come from simulation clock
    if (AdvancedLIFDynamics::shouldFire(neuron, params, currentTime)) {
        // Handle spike event
        AdvancedLIFDynamics::handleSpike(neuron, params, currentTime, pImpl->rng);
        
        // Apply spike timing dependent plasticity
        // This will be handled by the Brain's plasticity system
        neuron->setFiringState(FiringState::Active);
    }
    
    // Apply refractory potential reset if in refractory period
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(params.V_reset);
    }
    
    // Clear total injected current for next timestep
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // TODO PHASE 2: Implement real synaptic dynamics
    // PLACEHOLDER: Synapse decay
    synapse->step(0.0);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // TODO PHASE 2: Implement real synaptic input
    // PLACEHOLDER: Simple additive input
    SynapticWeight weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight));
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void IntegrateAndFireDynamics::reset() {
    // Reset all internal state
    if (pImpl) {
        // Reset random number generator seed for reproducibility
        pImpl->rng.seed(42);
    }
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    if (pImpl) {
        pImpl->defaultParameters.tau_m = tau;
    }
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl ? pImpl->defaultParameters.tau_m : 20.0f;
}

void IntegrateAndFireDynamics::setMembraneResistance(float R) {
    if (pImpl) {
        pImpl->defaultParameters.inputResistance = R;
    }
}

float IntegrateAndFireDynamics::getMembraneResistance() const {
    return pImpl ? pImpl->defaultParameters.inputResistance : 1000.0f;
}

void IntegrateAndFireDynamics::configureNeuron(const std::map<std::string, float>& parameters) {
    if (!pImpl) return;
    
    // Configure neuron parameters based on key-value pairs
    for (const auto& param : parameters) {
        if (param.first == "V_rest") pImpl->defaultParameters.V_rest = param.second;
        else if (param.first == "V_th") pImpl->defaultParameters.V_th = param.second;
        else if (param.first == "V_reset") pImpl->defaultParameters.V_reset = param.second;
        else if (param.first == "V_peak") pImpl->defaultParameters.V_peak = param.second;
        else if (param.first == "tau_m") pImpl->defaultParameters.tau_m = param.second;
        else if (param.first == "g_L") pImpl->defaultParameters.g_L = param.second;
        else if (param.first == "C_m") pImpl->defaultParameters.C_m = param.second;
        else if (param.first == "inputResistance") pImpl->defaultParameters.inputResistance = param.second;
        else if (param.first == "adaptationConductance") pImpl->defaultParameters.adaptationConductance = param.second;
        else if (param.first == "adaptationTimeConstant") pImpl->defaultParameters.adaptationTimeConstant = param.second;
        else if (param.first == "calciumTimeConstant") pImpl->defaultParameters.calciumTimeConstant = param.second;
        else if (param.first == "noiseLevel") pImpl->defaultParameters.noiseLevel = param.second;
        else if (param.first == "noiseCorrelation") pImpl->defaultParameters.noiseCorrelation = param.second;
    }
}

void IntegrateAndFireDynamics::setSTDPParameters(float ltpWeight, float ltdWeight, float tau) {
    // STDP parameters are stored in the plasticity manager
    // This interface provides forward compatibility
}

void IntegrateAndFireDynamics::setHebbianParameters(float learningRate) {
    // Hebbian parameters are stored in the plasticity manager
    // This interface provides forward compatibility
}

} // namespace nlm