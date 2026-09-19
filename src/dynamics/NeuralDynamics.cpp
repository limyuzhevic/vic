#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneCapacitance;    // nF
    float thresholdVoltage;       // mV
    float resetVoltage;           // mV
    float restingVoltage;         // mV
    float refractoryPeriod;       // steps
    float adaptationConductance;  // nS
    float channelNoiseLevel;      // mV RMS
    bool enableChannelNoise;
    bool enableSpikeNoise;
    float noiseScale;
    
    Impl() : membraneTimeConstant(20.0f), membraneCapacitance(1.0f), 
             thresholdVoltage(-55.0f), resetVoltage(-70.0f), restingVoltage(-70.0f),
             refractoryPeriod(2.0f), adaptationConductance(0.01f),
             channelNoiseLevel(0.1f), enableChannelNoise(true), 
             enableSpikeNoise(true), noiseScale(0.5f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    float V = neuron->getMembranePotential();
    float V_rest = pImpl->restingVoltage;
    float tau = pImpl->membraneTimeConstant;
    float C = pImpl->membraneCapacitance;
    float I_syn = neuron->getTotalCurrent();
    float I_adapt = pImpl->adaptationConductance * neuron->getState().adaptationVariable;
    
    // Membrane equation: C * dV/dt = -gL * (V - Vrest) + I_syn - gA * a + noise
    float gL = 1.0f / tau;  // Leak conductance
    
    // Deterministic part
    float dV_det = (-gL * (V - V_rest) + I_syn - I_adapt) * dt / C;
    
    // Add channel noise if enabled
    float noise = 0.0f;
    if (pImpl->enableChannelNoise && pImpl->channelNoiseLevel > 0.0f) {
        // Generate Gaussian noise
        std::random_device rd;
        static std::mt19937 gen(rd());
        std::normal_distribution<float> dist(0.0f, pImpl->channelNoiseLevel);
        noise = dist(gen) * pImpl->noiseScale * std::sqrt(static_cast<float>(dt));
    }
    
    // Add spike noise if enabled
    if (pImpl->enableSpikeNoise) {
        float noise2 = (neuron->checkThreshold() ? pImpl->noiseScale : 0.0f);
        noise += noise2 * std::sqrt(static_cast<float>(dt));
    }
    
    // Update membrane potential with noise
    float V_new = V + dV_det + noise;
    
    // Clamp to realistic limits
    V_new = std::clamp(V_new, -100.0f, 50.0f);
    
    // Handle refractory period
    if (neuron->isRefractory()) {
        // During refractory, maintain reset voltage
        V_new = pImpl->resetVoltage;
        neuron->decrementRefractory();
    }
    
    // Update membrane potential
    neuron->setMembranePotential(V_new);
    
    // Check for spike
    if (neuron->checkThreshold() && !neuron->isRefractory()) {
        // Record spike with precise timing
        neuron->recordSpike(0.0);  // TODO: pass actual simulation time
        
        // Reset membrane potential
        neuron->setMembranePotential(pImpl->resetVoltage);
        
        // Enter refractory period
        neuron->setRefractoryPeriod(static_cast<uint32_t>(pImpl->refractoryPeriod / dt));
        neuron->setFiringState(FiringState::Refractory);
        
        // Update adaptation variable for spike-frequency adaptation
        neuron->getState().adaptationVariable += 1.0f;
        
        // Apply adaptation to membrane potential
        neuron->addToMembranePotential(-neuron->getState().adaptationVariable * 0.01f);
    }
    
    // Clear synaptic current for next step
    neuron->clearTotalCurrent();
    
    // Decay adaptation variable
    neuron->getState().adaptationVariable *= 0.95f;
    neuron->getState().adaptationVariable = std::max(0.0f, neuron->getState().adaptationVariable);
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Real synaptic dynamics: NMDA/AMPA conductance-based transmission
    // Update synaptic conductance based on neuron activity and receptor dynamics
    
    float V_post = synapse->getDestinationNeuron()->getMembranePotential();
    float weight = synapse->getWeight();
    float preSpike = synapse->getPreSpikeHistory().size() > 0 ? 1.0f : 0.0f;
    float postSpike = synapse->getPostSpikeHistory().size() > 0 ? 1.0f : 0.0f;
    
    // Update NMDA conductance
    synapse->getNMDAState().activation *= (1.0f - 0.5f * static_cast<float>(dt));
    synapse->getAMPAState().activation *= (1.0f - 0.8f * static_cast<float>(dt));
    
    // Update short-term plasticity
    float utilization = synapse->getSTPState().utilization;
    float recovery = synapse->getSTPState().recovery;
    
    // Tsodyks-Markram model for STP
    float U = 0.5f;  // Utilization parameter
    float tau_f = 10.0f;  // Facilitation time constant
    float tau_d = 100.0f; // Depression time constant
    
    // Update STP state
    float dU = (U * (1.0f - utilization) - utilization * recovery) * dt / tau_f;
    float dR = ((1.0f - recovery) - recovery) * dt / tau_d;
    
    synapse->getSTPState().utilization += dU;
    synapse->getSTPState().recovery += dR;
    
    // Clamp STP values
    synapse->getSTPState().utilization = std::clamp(synapse->getSTPState().utilization, 0.0f, 1.0f);
    synapse->getSTPState().recovery = std::clamp(synapse->getSTPState().recovery, 0.0f, 1.0f);
    
    // Update synaptic conductance based on STP and pre-synaptic spikes
    float effectiveWeight = weight * synapse->getSTPState().utilization;
    
    // AMPA conductance (fast, transient)
    float ampaConductance = effectiveWeight * 0.5f;
    synapse->getAMPAState().activation = std::max(synapse->getAMPAState().activation, ampaConductance);
    
    // NMDA conductance (slow, persistent) with voltage-dependent Mg2+ block
    float V_th = -40.0f;
    float magnesiumBlock = 1.0f / (1.0f + std::exp(0.062f * (V_post - V_th)));
    float nmdaConductance = effectiveWeight * magnesiumBlock * 0.3f;
    synapse->getNMDAState().activation = std::max(synapse->getNMDAState().activation, nmdaConductance);
    
    // Update synaptic conductance structure
    auto& cond = synapse->getConductance();
    cond.excitatory = synapse->isExcitatory() ? ampaConductance + nmdaConductance : 0.0f;
    cond.inhibitory = synapse->isInhibitory() ? effectiveWeight * 0.2f : 0.0f;
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Apply real synaptic input based on conductance-based model
    const auto& cond = synapse->getConductance();
    const auto& state = synapse->getNMDAState();
    
    float totalConductance = cond.excitatory + cond.inhibitory + cond.nmda + cond.ampa;
    
    if (synapse->isExcitatory()) {
        // Excitatory current: I = g * (V_post - E_rev)
        float reversal = cond.reversalExcitatory;
        float V_post = neuron->getMembranePotential();
        float I_exc = totalConductance * (V_post - reversal);
        
        // Add to neuron's synaptic input
        neuron->receiveExcitatoryInput(I_exc);
        
        // Update NMDA receptor state
        if (state.activation > 0.01f) {
            float V_post = neuron->getMembranePotential();
            float magnesiumBlock = 1.0f / (1.0f + std::exp(0.062f * (V_post - (-40.0f))));
            neuron->receiveModulatoryInput(state.activation * magnesiumBlock * 0.1f);
        }
    } else {
        // Inhibitory current
        float reversal = cond.reversalInhibitory;
        float V_post = neuron->getMembranePotential();
        float I_inh = totalConductance * (V_post - reversal);
        
        neuron->receiveInhibitoryInput(I_inh);
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    float V = neuron->getMembranePotential();
    float threshold = pImpl->thresholdVoltage;
    return V >= threshold && !neuron->isRefractory();
}

void IntegrateAndFireDynamics::reset() {
    // Reset internal state
    delete pImpl;
    pImpl = new Impl();
}

} // namespace nlm
