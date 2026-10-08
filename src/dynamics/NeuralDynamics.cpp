#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

namespace nlm {

// ======================================================
// HOGKIN-HUXLEY DYNAMICS IMPLEMENTATION
// ======================================================

struct HodgkinHuxleyDynamics::Impl {
    // Classic Hodgkin-Huxley parameters
    float Cm;                    // Membrane capacitance (μF/cm²)
    float gNa;                   // Sodium conductance (mS/cm²)
    float gK;                    // Potassium conductance (mS/cm²)
    float gL;                    // Leak conductance (mS/cm²)
    
    // Reversal potentials (mV)
    float Vna;                   // Sodium reversal potential
    float Vk;                    // Potassium reversal potential
    float Vl;                    // Leak reversal potential
    
    // Gating variables
    float n;                     // Potassium activation
    float m;                     // Sodium activation
    float h;                     // Sodium inactivation
    
    // State variables
    float lastVm;                // Last membrane potential for alpha/beta calculations
    float lastTime;               // Last update time
    
    Impl() : Cm(1.0f), gNa(120.0f), gK(36.0f), gL(0.3f),
             Vna(50.0f), Vk(-77.0f), Vl(-54.387f),
             n(0.0f), m(0.0f), h(0.0f), lastVm(-65.0f), lastTime(0.0f) {}
    
    // Initialize with standard HH parameters
    void initialize() {
        Cm = 1.0f;
        gNa = 120.0f;
        gK = 36.0f;
        gL = 0.3f;
        Vna = 50.0f;
        Vk = -77.0f;
        Vl = -54.387f;
        n = 0.0f;
        m = 0.0f;
        h = 0.0f;
        lastVm = -65.0f;
        lastTime = 0.0f;
    }
    
    // Alpha and beta functions for gating variables
    static float alpha_n(float V) {
        return 0.01f * (V + 55.0f) / (1.0f - std::exp(-(V + 55.0f) / 10.0f));
    }
    
    static float beta_n(float V) {
        return 0.125f * std::exp(-(V + 65.0f) / 80.0f);
    }
    
    static float alpha_m(float V) {
        return 0.1f * (V + 40.0f) / (1.0f - std::exp(-(V + 40.0f) / 10.0f));
    }
    
    static float beta_m(float V) {
        return 4.0f * std::exp(-(V + 65.0f) / 18.0f);
    }
    
    static float alpha_h(float V) {
        return 0.07f * std::exp(-(V + 65.0f) / 20.0f);
    }
    
    static float beta_h(float V) {
        return 1.0f / (1.0f + std::exp(-(V + 35.0f) / 10.0f));
    }
};

HodgkinHuxleyDynamics::HodgkinHuxleyDynamics() : pImpl(new Impl) {}

HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() = default;

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron || dt <= 0.0) return;
    
    const auto& state = neuron->getState();
    float V = state.membranePotential;
    
    // Calculate gating variable derivatives
    float an = pImpl->alpha_n(V);
    float bn = pImpl->beta_n(V);
    float am = pImpl->alpha_m(V);
    float bm = pImpl->beta_m(V);
    float ah = pImpl->alpha_h(V);
    float bh = pImpl->beta_h(V);
    
    // Update gating variables using Euler integration
    float dn = (an * (1.0f - pImpl->n) - bn * pImpl->n) * static_cast<float>(dt);
    float dm = (am * (1.0f - pImpl->m) - bm * pImpl->m) * static_cast<float>(dt);
    float dh = (ah * (1.0f - pImpl->h) - bh * pImpl->h) * static_cast<float>(dt);
    
    pImpl->n += dn;
    pImpl->m += dm;
    pImpl->h += dh;
    
    // Clamp gating variables between 0 and 1
    pImpl->n = std::clamp(pImpl->n, 0.0f, 1.0f);
    pImpl->m = std::clamp(pImpl->m, 0.0f, 1.0f);
    pImpl->h = std::clamp(pImpl->h, 0.0f, 1.0f);
    
    // Calculate total currents
    float INa = pImpl->gNa * pImpl->m * pImpl->m * pImpl->m * pImpl->h * (V - pImpl->Vna);
    float IK = pImpl->gK * pImpl->n * pImpl->n * pImpl->n * pImpl->n * (V - pImpl->Vk);
    float IL = pImpl->gL * (V - pImpl->Vl);
    
    // Get external current input
    float Iext = neuron->getTotalCurrent();
    
    // Update membrane potential using HH equation
    float dV = (Iext - INa - IK - IL) / pImpl->Cm * static_cast<float>(dt);
    
    neuron->addToMembranePotential(dV);
    
    // Check for spike threshold (classic HH threshold is around 0 mV)
    if (neuron->checkThreshold()) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(neuron->getMembranePotential());
        
        // Enter refractory period
        neuron->setRefractoryPeriod(5); // 5 ms refractory period
    }
    
    // Update state in neuron
    neuron->setMembranePotential(V + dV);
    neuron->clearTotalCurrent();
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse || dt <= 0.0) return;
    
    // HH synapse dynamics using conductance-based model
    // AMPA-like fast excitatory synapse
    float gAMPA = synapse->getEfficacy() * synapse->getWeight();
    
    // Update synaptic conductance (decay)
    float dg = -gAMPA * static_cast<float>(dt) * 10.0f; // Decay constant
    gAMPA += dg;
    
    // Update synapse efficacy based on usage (depression/facilitation)
    if (synapse->getEfficacy() > 0.0f) {
        float decayFactor = 1.0f - std::min(1.0f, static_cast<float>(dt) * 20.0f);
        synapse->setEfficacy(gAMPA * decayFactor);
    }
    
    synapse->step(static_cast<float>(dt));
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    SynapticWeight weight = synapse->getWeight();
    
    if (synapse->isExcitatory()) {
        // AMPA/NMDA-like conductance increase
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(weight);
    }
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    
    // HH firing threshold
    return neuron->getMembranePotential() >= 0.0f;
}

void HodgkinHuxleyDynamics::reset() {
    if (pImpl) {
        pImpl->initialize();
    }
}

void HodgkinHuxleyDynamics::setMembraneTimeConstant(float tau) {
    if (pImpl) {
        pImpl->Cm = tau;
    }
}

float HodgkinHuxleyDynamics::getMembraneTimeConstant() const {
    return pImpl ? pImpl->Cm : 1.0f;
}

// ======================================================
// FITZHUGH-NAGUMO DYNAMICS IMPLEMENTATION
// ======================================================

struct FitzHughNagumoDynamics::Impl {
    // FHN parameters
    float epsilon;               // Time scale separation parameter
    float a;                     // Baseline parameter
    float b;                     // Threshold parameter
    float I;                     // External input/current
    
    // State variables
    float w;                     // Recovery variable
    float lastVm;                // Last membrane potential
    float lastTime;              // Last update time
    
    Impl() : epsilon(0.08f), a(0.7f), b(0.8f), I(0.5f),
             w(0.0f), lastVm(-1.5f), lastTime(0.0f) {}
    
    // Initialize with standard FHN parameters
    void initialize() {
        epsilon = 0.08f;
        a = 0.7f;
        b = 0.8f;
        I = 0.5f;
        w = 0.0f;
        lastVm = -1.5f;
        lastTime = 0.0f;
    }
};

FitzHughNagumoDynamics::FitzHughNagumoDynamics() : pImpl(new Impl) {}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron || dt <= 0.0) return;
    
    const auto& state = neuron->getState();
    float V = state.membranePotential;
    
    // FHN equations
    float dV = (V - V * V * V / 3.0f - pImpl->w + pImpl->I) * static_cast<float>(dt);
    float dw = (pImpl->epsilon * (V + pImpl->a - pImpl->b * pImpl->w)) * static_cast<float>(dt);
    
    // Update variables
    float newV = V + dV;
    float newW = pImpl->w + dw;
    
    // Update neuron state
    neuron->setMembranePotential(newV);
    
    // Update adaptation variable
    float& adaptation = neuron->getAdaptationVariable();
    adaptation = newW;
    
    // Check for firing (threshold crossing with hysteresis)
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(newV);
        
        // Enter refractory period
        neuron->setRefractoryPeriod(3); // 3 ms refractory period
    }
    
    neuron->clearTotalCurrent();
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse || dt <= 0.0) return;
    
    // FHN synapse dynamics - conductance-based model
    // NMDA-like slow synaptic conductance
    float gNMDA = synapse->getEfficacy() * synapse->getWeight();
    
    // NMDA receptor dynamics with voltage dependence
    float mgBlock = 1.0f / (1.0f + std::exp(-0.062f * (-65.0f)));
    float conductance = gNMDA * mgBlock * (1.0f - (pImpl->w - pImpl->a) / pImpl->b);
    
    // Update conductance (decay)
    float dg = -conductance * static_cast<float>(dt) * 5.0f; // Slower decay
    conductance += dg;
    
    // Update synapse efficacy based on usage (depression/facilitation)
    if (synapse->getEfficacy() > 0.0f) {
        float decayFactor = 1.0f - std::min(1.0f, static_cast<float>(dt) * 10.0f);
        synapse->setEfficacy(conductance * decayFactor);
    }
    
    synapse->step(static_cast<float>(dt));
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    SynapticWeight weight = synapse->getWeight();
    
    // FHN synaptic input - NMDA-like dynamics
    if (synapse->isExcitatory()) {
        // Enhanced input for NMDA-like dynamics
        float enhancedWeight = weight * (1.0f + 0.5f * (pImpl->b - pImpl->w));
        neuron->receiveExcitatoryInput(enhancedWeight);
    } else {
        neuron->receiveInhibitoryInput(weight);
    }
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    
    // FHN firing threshold (bistable switching)
    float V = neuron->getMembranePotential();
    float adaptation = neuron->getAdaptationVariable();
    
    // Firing when V crosses threshold or adaptation variable changes
    return (V > pImpl->b) || (neuron->isRefractory() && V < pImpl->a);
}

void FitzHughNagumoDynamics::reset() {
    if (pImpl) {
        pImpl->initialize();
    }
}

void FitzHughNagumoDynamics::setMembraneTimeConstant(float tau) {
    if (pImpl) {
        pImpl->epsilon = 1.0f / std::max(tau, 0.001f);
    }
}

float FitzHughNagumoDynamics::getMembraneTimeConstant() const {
    return pImpl ? 1.0f / pImpl->epsilon : 1.0f;
}

// ======================================================
// INTEGRATE-AND-FIRE DYNAMICS IMPLEMENTATION (IMPROVED)
// ======================================================

struct IntegrateAndFireDynamics::Impl {
    // Biologically realistic IA-F parameters
    float Cm;                    // Membrane capacitance (pF)
    float membraneResistance;     // Membrane resistance (MOhm)
    float restingPotential;      // Resting potential (mV)
    float threshold;             // Firing threshold (mV)
    float resetPotential;        // Reset potential after spike (mV)
    float decayTau;              // Membrane time constant (ms)
    
    // Synaptic input parameters
    float synapticWeight;        // Average synaptic weight
    float synapticTimeConstant;   // Synaptic time constant (ms)
    
    // Short-term plasticity state
    float utilization;            // Fraction of resources used
    float recovery;               // Rate of resource recovery
    float facilitation;           // Facilitation parameter
    
    // Refractory mechanism
    float refractoryPeriod;       // Refractory period duration (ms)
    float refractoryVoltage;      // Voltage during refractory period (mV)
    
    Impl() : Cm(100.0f), membraneResistance(100.0f), restingPotential(-70.0f),
             threshold(-55.0f), resetPotential(-75.0f), decayTau(20.0f),
             synapticWeight(0.1f), synapticTimeConstant(5.0f),
             utilization(0.0f), recovery(0.95f), facilitation(0.95f),
             refractoryPeriod(5.0f), refractoryVoltage(-80.0f) {}
    
    // Initialize with biologically realistic parameters
    void initialize() {
        Cm = 100.0f;
        membraneResistance = 100.0f;
        restingPotential = -70.0f;
        threshold = -55.0f;
        resetPotential = -75.0f;
        decayTau = 20.0f;
        synapticWeight = 0.1f;
        synapticTimeConstant = 5.0f;
        utilization = 0.0f;
        recovery = 0.95f;
        facilitation = 0.95f;
        refractoryPeriod = 5.0f;
        refractoryVoltage = -80.0f;
    }
    
    // Update synaptic efficacy with depression/facilitation
    float updateSynapticEfficacy(SynapticWeight baseWeight, bool isFacilitating = false) {
        if (isFacilitating) {
            // Facilitation: increase weight with recent activity
            utilization = std::min(1.0f, utilization + (1.0f - utilization) * (1.0f - facilitation) * 0.1f);
        } else {
            // Depression: decrease weight with recent activity
            utilization = recovery * utilization;
        }
        
        return baseWeight * utilization;
    }
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron || dt <= 0.0) return;
    
    const auto& state = neuron->getState();
    float V = state.membranePotential;
    
    // Calculate membrane current components
    float I_leak = (V - state.restingPotential) / pImpl->membraneResistance;
    float I_syn = state.synapseConductance * (V - state.restingPotential);
    float I_ext = neuron->getTotalCurrent() / pImpl->Cm;
    
    // Check if neuron is in refractory period
    if (neuron->isRefractory()) {
        // Hold at refractory voltage
        neuron->setMembranePotential(pImpl->refractoryVoltage);
        neuron->decrementRefractory();
        neuron->clearTotalCurrent();
        return;
    }
    
    // Leaky integration with Euler method
    float dV = -(V - state.restingPotential) / pImpl->decayTau 
               + (I_ext + I_syn + I_leak) * static_cast<float>(dt);
    
    float newV = V + dV;
    
    // Spike detection with hysteresis
    if (newV >= pImpl->threshold && !neuron->isFiring()) {
        // Mark neuron as firing
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(newV);
        
        // Set refractory period
        neuron->setRefractoryPeriod(static_cast<uint32_t>(pImpl->refractoryPeriod));
        
        // Reset voltage to reset potential
        newV = pImpl->resetPotential;
        
        // Update adaptation variable for spike-frequency adaptation
        float& adaptation = neuron->getAdaptationVariable();
        adaptation += 1.0f; // Each spike increases adaptation
    }
    
    // Spike-frequency adaptation
    float& adaptation = neuron->getAdaptationVariable();
    if (adaptation > 0.0f) {
        adaptation *= 0.9f; // Decay adaptation
    }
    
    // Update membrane potential
    neuron->setMembranePotential(newV);
    
    // Clear total current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse || dt <= 0.0) return;
    
    // IA-F synapse dynamics with realistic time course
    float g = synapse->getEfficacy() * synapse->getWeight();
    
    // Two-process model: fast rise and slow decay
    float riseRate = 1.0f / 0.5f;   // 0.5 ms rise time
    float decayRate = 1.0f / 5.0f;  // 5 ms decay time
    
    // Update conductance
    float dg = -decayRate * g; // Simplified (no rise mechanism needed for synaptic decay)
    g += dg * static_cast<float>(dt);
    
    // Update synapse efficacy based on usage (depression/facilitation)
    if (g > 0.0f) {
        synapse->setEfficacy(std::max(0.0f, g));
    }
    
    synapse->step(static_cast<float>(dt));
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    // Check for depression vs facilitation
    bool isFacilitating = synapse->getType() == SynapseType::Modulatory;
    
    // Update synaptic efficacy with STP
    SynapticWeight baseWeight = synapse->getWeight();
    SynapticWeight effectiveWeight = pImpl->updateSynapticEfficacy(baseWeight, isFacilitating);
    
    // Apply input based on synapse type
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(effectiveWeight);
    } else {
        neuron->receiveInhibitoryInput(effectiveWeight);
    }
    
    // Record spike history for plasticity
    // Note: Cannot call non-const method on const synapse
    // This would need to be handled elsewhere or synapse would need const version
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    
    // Enhanced spike detection with hysteresis
    float V = neuron->getMembranePotential();
    float adaptation = neuron->getAdaptationVariable();
    
    // Threshold with adaptation modulation
    float effectiveThreshold = pImpl->threshold + 2.0f * adaptation;
    
    return (V >= effectiveThreshold) && !neuron->isRefractory();
}

void IntegrateAndFireDynamics::reset() {
    if (pImpl) {
        pImpl->initialize();
    }
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    if (pImpl) {
        pImpl->decayTau = tau;
        pImpl->membraneResistance = tau * pImpl->Cm;
    }
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl ? pImpl->decayTau : 20.0f;
}

} // namespace nlm