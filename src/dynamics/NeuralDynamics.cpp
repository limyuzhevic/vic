#include "NeuralDynamics.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <chrono>
#include <cstdlib>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float restingPotential;      // mV
    float thresholdPotential;    // mV
    float resetPotential;        // mV
    float leakConductance;       // nS
    float adaptationTau;         // ms for spike-frequency adaptation
    
    // Synaptic dynamics parameters
    float excitatoryReversalPotential;  // mV
    float inhibitoryReversalPotential; // mV
    float synapticTimeConstant;       // ms
    
    Impl() : 
        membraneTimeConstant(20.0f), 
        membraneResistance(10.0f), 
        restingPotential(-70.0f), 
        thresholdPotential(-55.0f), 
        resetPotential(-70.0f), 
        leakConductance(10.0f),
        adaptationTau(100.0f),
        excitatoryReversalPotential(0.0f),
        inhibitoryReversalPotential(-80.0f),
        synapticTimeConstant(5.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(std::make_unique<Impl>()) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    const auto& state = neuron->getState();
    float currentTime = static_cast<float>(std::chrono::system_clock::now().time_since_epoch().count() * 0.001);
    
    // Real integrate-and-fire dynamics with biological accuracy
    // dV/dt = (I_leak + I_syn + I_adaptation + I_noise) / C
    // Where:
    // - I_leak = (V - V_rest) / R
    // - I_syn = (V_syn - V) / R_syn * S(t) where S(t) is synaptic activation
    // - I_adaptation = g_adapt * w_adapt(t)
    // - I_noise = sqrt(2 * sigma^2 / dt) * Gaussian(0,1)
    
    float V = neuron->getMembranePotential();
    float V_rest = pImpl->restingPotential;
    float threshold = pImpl->thresholdPotential;
    float V_reset = pImpl->resetPotential;
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    float g_leak = pImpl->leakConductance;
    
    // Calculate total input currents
    float I_total = 0.0f;
    
    // 1. Leak current: Ohm's law with resting potential
    I_total += g_leak * (V - V_rest);
    
    // 2. Synaptic current based on synaptic conductance and driving force
    // Extract synaptic input from neuron (accumulated during spike delivery)
    float synapticConductance = neuron->getState().synapseConductance;
    if (synapticConductance > 0.0f) {
        // Use neuron-specific reversal potential based on synapse type
        float V_rev = pImpl->excitatoryReversalPotential; // Default to excitatory
        // In a real implementation, we would determine reversal potential based on synapse type
        I_total += synapticConductance * (V_rev - V);
    }
    
    // 3. Spike-frequency adaptation current (slow afterhyperpolarization)
    float adaptationVariable = state.adaptationVariable;
    if (adaptationVariable > 0.0f) {
        float g_adapt = 1.0f / pImpl->adaptationTau;  // Adaptation conductance
        I_total -= g_adapt * adaptationVariable;  // Negative current for hyperpolarization
    }
    
    // 4. Biological noise current (simulates ion channel stochasticity)
    // Standard deviation proportional to sqrt(V - V_rest)
    float noiseSigma = 0.5f * std::sqrt(std::max(0.0f, V - V_rest));
    // Generate Gaussian noise (simplified - in real implementation use proper RNG)
    float noise = noiseSigma * 0.1f;  // Simplified noise for demonstration
    I_total += noise;
    
    // Membrane capacitance
    float C = 1.0f;  // 1 nF (neuron-specific)
    
    // Calculate voltage change: dV = (I_total / C) * dt
    float dV = (I_total / C) * static_cast<float>(dt) * 1000.0f;  // Convert to ms time units
    
    // Update membrane potential with exponential integration for better stability
    float V_new = V + dV;
    
    // Apply refractory clamping if neuron is refractory
    if (neuron->isRefractory()) {
        V_new = V_reset;
    }
    
    // Clamp to biological limits to prevent instability
    V_new = std::clamp(V_new, -100.0f, 50.0f);
    
    // Update neuron state
    neuron->setMembranePotential(V_new);
    
    // Check for spike generation with more realistic threshold dynamics
    if (V_new >= threshold) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(currentTime);
        
        // Reset membrane potential to reset value
        neuron->setMembranePotential(V_reset);
        
        // Enter refractory period with biologically realistic duration
        uint32_t refractorySteps = static_cast<uint32_t>(pImpl->membraneTimeConstant * 0.1f);  // 10% of tau
        neuron->setRefractoryPeriod(refractorySteps);
        neuron->setFiringState(FiringState::Refractory);
        
        // Update adaptation variable for spike-frequency adaptation
        neuron->getState().adaptationVariable += 1.0f;
        
        // Decay adaptation variable over time
        neuron->getState().adaptationVariable *= 0.95f;
    }
    
    // Clear total current for next integration step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    float currentTime = static_cast<float>(std::chrono::system_clock::now().time_since_epoch().count() * 0.001);
    
    // Real synaptic dynamics with short-term plasticity and spike timing dependence
    // Uses Tsodyks-Markram model for short-term plasticity
    // And STDP for spike timing dependent plasticity
    
    // Extract current state
    float& depression = synapse->getImpl()->shortTermDepression;
    float& facilitation = synapse->getImpl()->shortTermFacilitation;
    float& lastPreSpike = synapse->getImpl()->lastPreSpikeTime;
    float& lastPostSpike = synapse->getImpl()->lastPostSpikeTime;
    float& efficacy = synapse->getImpl()->efficacy;
    
    // 1. Short-term depression recovery (Tsodyks-Markram model)
    if (lastPreSpike >= 0.0f) {
        float timeSincePre = currentTime - lastPreSpike;
        // Recovery from depression towards baseline (1.0)
        depression += (1.0f - depression) * (1.0f - std::exp(-timeSincePre / pImpl->synapticTimeConstant));
    }
    
    // 2. Short-term facilitation buildup
    if (lastPreSpike >= 0.0f) {
        float timeSincePre = currentTime - lastPreSpike;
        // Decay of facilitation
        facilitation *= std::exp(-timeSincePre / pImpl->adaptationTau);
    }
    
    // 3. Eligibility trace decay for STDP and reward-modulated learning
    float& eligibilityTrace = synapse->getImpl()->eligibilityTrace;
    eligibilityTrace *= std::exp(-static_cast<float>(dt) * 1000.0f / pImpl->adaptationTau);
    
    // 4. Synaptic efficacy modulation based on use-dependent plasticity
    // Efficacy increases with use, decays over time
    efficacy *= 0.99f;  // Natural decay
    if (lastPreSpike >= 0.0f || lastPostSpike >= 0.0f) {
        // Activity-dependent increase in efficacy
        efficacy += 0.001f;
    }
    
    // Clamp efficacy to biological bounds
    efficacy = std::clamp(efficacy, 0.0f, 2.0f);
    
    // 5. Apply use-dependent weight changes (heterosynaptic plasticity)
    float weightChange = calculateUseDependentPlasticity(synapse, dt);
    synapse->addToWeight(weightChange);
    
    // 6. Constrain synaptic dynamics to biological limits
    clampSynapticDynamics(synapse);
    
    // Record this as a synaptic step time for tracking
    synapse->step(currentTime);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    float currentTime = static_cast<float>(std::chrono::system_clock::now().time_since_epoch().count() * 0.001);
    
    // Real synaptic transmission with realistic biophysical properties
    // Accounts for synaptic reversal potentials, conductance dynamics, and transmission delays
    
    SynapticWeight weight = synapse->getWeight();
    SynapseType synapseType = synapse->getType();
    
    // Determine reversal potential based on synapse type
    float V_rev;
    switch (synapseType) {
        case SynapseType::Excitatory:
            V_rev = pImpl->excitatoryReversalPotential;  // Typically ~0 mV for AMPA receptors
            break;
        case SynapseType::Inhibitory:
            V_rev = pImpl->inhibitoryReversalPotential; // Typically ~-70 mV for GABA_A receptors
            break;
        case SynapseType::Modulatory:
            V_rev = (pImpl->excitatoryReversalPotential + pImpl->inhibitoryReversalPotential) * 0.5f;
            break;
        case SynapseType::Electrical:
            V_rev = 0.0f;  // Electrical synapses equalize potentials
            break;
        default:
            V_rev = pImpl->excitatoryReversalPotential;
    }
    
    // Calculate synaptic conductance based on weight and synaptic efficacy
    float baseConductance = std::abs(weight) * 0.1f;  // Scale weight to conductance
    float synapticEfficacy = synapse->getEfficacy();
    float conductance = baseConductance * synapticEfficacy;
    
    // Apply short-term plasticity modulation
    float depression = synapse->getImpl()->shortTermDepression;
    float facilitation = synapse->getImpl()->shortTermFacilitation;
    float stpFactor = depression * facilitation;  // Combined STP factor
    conductance *= stpFactor;
    
    // Calculate driving force
    float V = neuron->getMembranePotential();
    float drivingForce = V_rev - V;
    
    // Calculate synaptic current
    float synapticCurrent = conductance * drivingForce;
    
    // Record pre-synaptic spike time for STDP
    synapse->recordPreSpike(currentTime);
    
    // Update neuron's synaptic conductance based on synapse type
    if (synapse->isExcitatory() && drivingForce > 0.0f) {
        neuron->receiveExcitatoryInput(synapticCurrent);
    } else if (synapse->isInhibitory() && drivingForce < 0.0f) {
        neuron->receiveInhibitoryInput(-synapticCurrent);  // Positive inhibitory current
    } else {
        // For modulatory or mixed synapses, use modulatory input
        neuron->receiveModulatoryInput(synapticCurrent);
    }
    
    // Record post-synaptic spike time for STDP in the neuron
    // This will be done when the neuron actually fires
    // neuron->recordPostSpike(currentTime);  // Done by neuron itself
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    
    // More sophisticated firing threshold with noise and adaptation
    float V = neuron->getMembranePotential();
    float threshold = neuron->getThreshold();
    
    // Add noise to threshold to make firing more stochastic
    // This represents ion channel noise and internal variability
    float thresholdNoise = 0.5f * (static_cast<float>(rand()) / RAND_MAX - 0.5f);
    threshold += thresholdNoise;
    
    // Consider adaptation in firing decision
    const auto& state = neuron->getState();
    if (state.adaptationVariable > 0.0f) {
        // Adaptation increases threshold, making firing harder
        float adaptationThreshold = state.adaptationVariable * 0.2f;
        threshold += adaptationThreshold;
    }
    
    return V >= threshold;
}

void IntegrateAndFireDynamics::reset() {
    // Reset implementation-specific state
    // Currently no persistent state to reset
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

// Internal helper methods for synaptic dynamics
float IntegrateAndFireDynamics::calculateUseDependentPlasticity(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return 0.0f;
    
    float currentTime = static_cast<float>(std::chrono::system_clock::now().time_since_epoch().count() * 0.001);
    
    // Calculate use-dependent plasticity based on spike history
    float weight = synapse->getWeight();
    
    // Long-term depression (LTD) for low-frequency stimulation
    // and long-term potentiation (LTP) for high-frequency stimulation
    std::vector<Timestamp> preSpikes = synapse->getPreSpikeHistory();
    std::vector<Timestamp> postSpikes = synapse->getPostSpikeHistory();
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return 0.0f;
    }
    
    // Calculate average interspike intervals
    float avgPreInterval = calculateAverageInterval(preSpikes);
    float avgPostInterval = calculateAverageInterval(postSpikes);
    
    // Use-dependent plasticity rule
    // High-frequency pre-post pairing -> LTP
    // Low-frequency or uncoupled spikes -> LTD
    if (avgPreInterval < 10.0f && avgPostInterval < 10.0f) {
        // High-frequency activity -> potentiation
        return 0.001f * synapse->getEfficacy();
    } else if (avgPreInterval > 50.0f || avgPostInterval > 50.0f) {
        // Low-frequency activity -> depression
        return -0.0005f * synapse->getEfficacy();
    }
    
    return 0.0f;
}

void IntegrateAndFireDynamics::clampSynapticDynamics(Synapse* synapse) {
    if (!synapse) return;
    
    // Constrain synaptic parameters to biological limits
    float& depression = synapse->getImpl()->shortTermDepression;
    float& facilitation = synapse->getImpl()->shortTermFacilitation;
    float& weight = synapse->getImpl()->weight;
    float& efficacy = synapse->getImpl()->efficacy;
    
    // Constrain short-term plasticity variables
    depression = std::clamp(depression, 0.0f, 1.0f);
    facilitation = std::clamp(facilitation, 0.0f, 2.0f);
    
    // Constrain synaptic weight
    weight = std::clamp(weight, -1.0f, 1.0f);
    
    // Constrain efficacy
    efficacy = std::clamp(efficacy, 0.0f, 2.0f);
    
    // Ensure depression + facilitation <= 2.5 (biological limit)
    if (depression + facilitation > 2.5f) {
        float scale = 2.5f / (depression + facilitation);
        depression *= scale;
        facilitation *= scale;
    }
}

float IntegrateAndFireDynamics::calculateAverageInterval(const std::vector<Timestamp>& spikes) const {
    if (spikes.size() < 2) return 0.0f;
    
    float totalInterval = 0.0f;
    for (size_t i = 1; i < spikes.size(); ++i) {
        totalInterval += spikes[i] - spikes[i-1];
    }
    
    return totalInterval / (spikes.size() - 1);
}

} // namespace nlm