#include "NeuralDynamics.hpp"
#include <cmath>
#include <stdexcept>

namespace nlm {

// Helper functions for integration
static float alpha_m_func(float V) { return 0.1f * (V + 40.0f) / (1.0f - std::exp(-(V + 40.0f) / 10.0f)); }
static float beta_m_func(float V) { return 4.0f * std::exp(-(V + 65.0f) / 18.0f); }
static float alpha_h_func(float V) { return 0.07f * std::exp(-(V + 65.0f) / 20.0f); }
static float beta_h_func(float V) { return 1.0f / (1.0f + std::exp(-(V + 35.0f) / 10.0f)); }
static float alpha_n_func(float V) { return 0.01f * (V + 55.0f) / (1.0f - std::exp(-(V + 55.0f) / 10.0f)); }
static float beta_n_func(float V) { return 0.125f * std::exp(-(V + 65.0f) / 80.0f); }

// Hodgkin-Huxley implementation
struct HodgkinHuxleyDynamics::Impl {
    // HH parameters (classical values from Hodgkin & Huxley 1952)
    float Cm;           // Membrane capacitance (μF/cm²)
    float gNa_max;      // Max sodium conductance (mS/cm²)
    float gK_max;       // Max potassium conductance (mS/cm²)
    float gL;           // Leak conductance (mS/cm²)
    
    // Reversal potentials (mV)
    float ENa;          // Sodium reversal potential (+50 mV)
    float EK;           // Potassium reversal potential (-77 mV)
    float EL;           // Leak reversal potential (-54.387 mV)
    
    // Integration settings
    IntegrationMethod integrationMethod;
    
    // State variables
    float m, h, n;       // Gating variables (0-1)
    float V_rest;        // Resting potential (-70 mV)
    bool spikeDetected;  // Spike detection flag
    float lastSpikeTime; // Time of last spike (ms)
    float spikeThreshold; // Spike detection threshold (-55 mV)
    
    // Refractory state
    uint32_t refractoryRemaining;
    float adaptationVariable;
    
    // Synaptic parameters
    float synapticReversalPotential;
    
    Impl() : 
        Cm(1.0f), gNa_max(120.0f), gK_max(36.0f), gL(0.3f),
        ENa(50.0f), EK(-77.0f), EL(-54.387f),
        integrationMethod(HH::IntegrationMethod::Euler),
        m(0.05f), h(0.6f), n(0.32f), V_rest(-70.0f),
        spikeDetected(false), lastSpikeTime(-1000.0f), spikeThreshold(-55.0f),
        refractoryRemaining(0), adaptationVariable(0.0f), synapticReversalPotential(0.0f) {}
    
    float m_inf(float V) const { return alpha_m_func(V) / (alpha_m_func(V) + beta_m_func(V)); }
    float h_inf(float V) const { return alpha_h_func(V) / (alpha_h_func(V) + beta_h_func(V)); }
    float n_inf(float V) const { return alpha_n_func(V) / (alpha_n_func(V) + beta_n_func(V)); }
    
    float tau_m(float V) const { return 1.0f / (alpha_m_func(V) + beta_m_func(V)); }
    float tau_h(float V) const { return 1.0f / (alpha_h_func(V) + beta_h_func(V)); }
    float tau_n(float V) const { return 1.0f / (alpha_n_func(V) + beta_n_func(V)); }
    
    void updateGatingVariables(float V, float dt) {
        m += (m_inf(V) - m) / tau_m(V) * dt;
        h += (h_inf(V) - h) / tau_h(V) * dt;
        n += (n_inf(V) - n) / tau_n(V) * dt;
        
        m = std::max(0.0f, std::min(1.0f, m));
        h = std::max(0.0f, std::min(1.0f, h));
        n = std::max(0.0f, std::min(1.0f, n));
    }
    
    float computeMembraneCurrent(float V, float I_ext = 0.0f) const {
        float I_Na = gNa_max * m * m * m * h * (V - ENa);
        float I_K = gK_max * n * n * n * (V - EK);
        float I_L = gL * (V - EL);
        return I_ext - I_Na - I_K - I_L;
    }
    
    float updateMembranePotential(float V, float I_ext, float dt) {
        float I_total = computeMembraneCurrent(V, I_ext);
        
        switch (integrationMethod) {
            case HH::IntegrationMethod::Euler:
                return V + (-I_total / Cm) * dt;
            case HH::IntegrationMethod::RungeKutta4:
                return rungeKuttaStep(V, I_ext, Cm, gNa_max, gK_max, gL, ENa, EK, EL);
            default:
                return V + (-I_total / Cm) * dt;
        }
    }
    
    bool detectSpike(float V) {
        if (V >= spikeThreshold && !spikeDetected) {
            spikeDetected = true;
            return true;
        } else if (V < spikeThreshold) {
            spikeDetected = false;
        }
        return false;
    }
    
    float computeResetPotential() const {
        return EL;
    }
};

// FitzHugh-Nagumo implementation
struct FitzHughNagumoDynamics::Impl {
    // FHN parameters
    float a;           // Recovery variable scaling factor (typically 0.7-0.8)
    float b;           // Cubic nullcline parameter (controls excitability)
    float tau;         // Time constant for recovery variable (ms)
    float V_rest;      // Resting potential (mV)
    
    // Integration settings
    IntegrationMethod integrationMethod;
    ExcitabilityType excitabilityType;
    
    // State variables
    float V;           // Membrane potential (mV)
    float w;           // Recovery variable (-2 to 2)
    bool spikeDetected; // Spike detection flag
    float lastSpikeTime; // Time of last spike (ms)
    float threshold;    // Spike detection threshold (mV)
    
    // Refractory state
    uint32_t refractoryRemaining;
    
    Impl() : 
        a(0.7f), b(0.7f), tau(12.5f), V_rest(-65.0f),
        integrationMethod(HH::IntegrationMethod::Euler),
        excitabilityType(ExcitabilityType::TypeII),
        V(-65.0f), w(0.0f), spikeDetected(false), lastSpikeTime(-1000.0f), threshold(-40.0f),
        refractoryRemaining(0) {}
    
    void updateFhnState(float dt) {
        // FHN equations: dV/dt = (V - V^3/3 - w + I_ext)/tau, dw/dt = (a*V + b - c*w)/tau
        float dV = (V - V * V * V / 3.0f - w + externalCurrent) / tau;
        float dw = (a * V + b - c * w) / recoveryTimeConstant;
        
        V += dV * dt;
        w += dw * dt;
        
        w = std::max(-2.0f, std::min(2.0f, w));
        
        bool spikeChanged = detectSpike(V);
        if (spikeChanged) {
            lastSpikeTime = dt;
        }
    }
    
    bool detectSpike(float V) {
        if (V >= threshold && !spikeDetected) {
            spikeDetected = true;
            return true;
        } else if (V < threshold) {
            spikeDetected = false;
        }
        return false;
    }
};

// Enhanced IntegrateAndFire implementation
struct IntegrateAndFireDynamics::Impl {
    float tau_m;           // Membrane time constant (ms)
    float R;               // Membrane resistance (MOhm)
    float Cm;              // Membrane capacitance (nF)
    
    uint32_t refractoryPeriod;
    uint32_t refractoryRemaining;
    
    float threshold;       // Base threshold (mV)
    float adaptationVariable;
    float adaptationRate;
    
    float g_exc_max;       // Max excitatory conductance (nS)
    float g_inh_max;       // Max inhibitory conductance (nS)
    float E_exc;           // Excitatory reversal potential (mV)
    float E_inh;           // Inhibitory reversal potential (mV)
    
    IntegrationMethod integrationMethod;
    
    Impl() : 
        tau_m(20.0f), R(10.0f), Cm(0.25f),
        refractoryPeriod(5), refractoryRemaining(0),
        threshold(-55.0f), adaptationVariable(0.0f), adaptationRate(0.1f),
        g_exc_max(1.0f), g_inh_max(0.5f), E_exc(0.0f), E_inh(-70.0f),
        integrationMethod(HH::IntegrationMethod::Euler) {}
};

// Constructor implementations
HodgkinHuxleyDynamics::HodgkinHuxleyDynamics() : pImpl(new Impl) {}
HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() = default;

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    float V = neuron->getMembranePotential();
    float I_ext = neuron->getTotalCurrent();
    float dt_f = static_cast<float>(dt);
    
    pImpl->updateGatingVariables(V, dt_f);
    
    float V_new = pImpl->updateMembranePotential(V, I_ext, dt_f);
    
    if (pImpl->detectSpike(V_new)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);
        neuron->setMembranePotential(pImpl->computeResetPotential());
        pImpl->updateGatingVariables(pImpl->computeResetPotential(), dt_f);
        neuron->setRefractoryPeriod(5);
    } else {
        neuron->setMembranePotential(V_new);
    }
    
    if (neuron->isRefractory()) {
        if (pImpl->refractoryRemaining > 0) pImpl->refractoryRemaining--;
        neuron->setMembranePotential(pImpl->computeResetPotential());
    }
    
    neuron->clearTotalCurrent();
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    synapse->step(0.0);
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    float weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight));
    }
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void HodgkinHuxleyDynamics::reset() {
    float V_rest = pImpl->computeResetPotential();
    pImpl->m = pImpl->m_inf(V_rest);
    pImpl->h = pImpl->h_inf(V_rest);
    pImpl->n = pImpl->n_inf(V_rest);
    pImpl->spikeDetected = false;
    pImpl->refractoryRemaining = 0;
    pImpl->adaptationVariable = 0.0f;
}

void HodgkinHuxleyDynamics::setMembraneCapacitance(float cm) { pImpl->Cm = cm; }
float HodgkinHuxleyDynamics::getMembraneCapacitance() const { return pImpl->Cm; }

void HodgkinHuxleyDynamics::setMaxSodiumConductance(float gNa) { pImpl->gNa_max = gNa; }
float HodgkinHuxleyDynamics::getMaxSodiumConductance() const { return pImpl->gNa_max; }

void HodgkinHuxleyDynamics::setMaxPotassiumConductance(float gK) { pImpl->gK_max = gK; }
float HodgkinHuxleyDynamics::getMaxPotassiumConductance() const { return pImpl->gK_max; }

void HodgkinHuxleyDynamics::setMaxLeakConductance(float gL) { pImpl->gL = gL; }
float HodgkinHuxleyDynamics::getMaxLeakConductance() const { return pImpl->gL; }

void HodgkinHuxleyDynamics::setSodiumReversalPotential(float ENa) { pImpl->ENa = ENa; }
float HodgkinHuxleyDynamics::getSodiumReversalPotential() const { return pImpl->ENa; }

void HodgkinHuxleyDynamics::setPotassiumReversalPotential(float EK) { pImpl->EK = EK; }
float HodgkinHuxleyDynamics::getPotassiumReversalPotential() const { return pImpl->EK; }

void HodgkinHuxleyDynamics::setLeakReversalPotential(float EL) { pImpl->EL = EL; }
float HodgkinHuxleyDynamics::getLeakReversalPotential() const { return pImpl->EL; }

void HodgkinHuxleyDynamics::setGatingVariableParameters(float a, float b, float c, float d) {
    pImpl->alpha_m = a; pImpl->beta_m = b;
    pImpl->alpha_h = c; pImpl->beta_h = d;
    pImpl->alpha_n = 0.01f; pImpl->beta_n = 0.125f;
}

void HodgkinHuxleyDynamics::setIntegrationMethod(HodgkinHuxleyDynamics::IntegrationMethod method) { pImpl->integrationMethod = method; }
void HodgkinHuxleyDynamics::setSpikeThreshold(float threshold) { pImpl->spikeThreshold = threshold; }
float HodgkinHuxleyDynamics::getSpikeThreshold() const { return pImpl->spikeThreshold; }
bool HodgkinHuxleyDynamics::hasRecentSpike() const { return pImpl->spikeDetected; }
float HodgkinHuxleyDynamics::getLastSpikeTime() const { return pImpl->lastSpikeTime; }
void HodgkinHuxleyDynamics::clearRecentSpikes() { pImpl->spikeDetected = false; }

// FitzHughNagumo implementation
FitzHughNagumoDynamics::FitzHughNagumoDynamics() : pImpl(new Impl) {}
FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    float dt_f = static_cast<float>(dt);
    float I_ext = neuron->getTotalCurrent();
    
    pImpl->externalCurrent = I_ext;
    pImpl->updateFhnState(dt_f);
    
    neuron->setMembranePotential(pImpl->V);
    
    if (pImpl->detectSpike(pImpl->V)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);
        neuron->setMembranePotential(pImpl->V_rest);
    }
    
    if (neuron->isRefractory()) {
        if (pImpl->refractoryRemaining > 0) pImpl->refractoryRemaining--;
        neuron->setMembranePotential(pImpl->V_rest);
    }
    
    neuron->clearTotalCurrent();
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    synapse->step(0.0);
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    float weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight));
    }
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void FitzHughNagumoDynamics::reset() {
    pImpl->V = pImpl->V_rest;
    pImpl->w = 0.0f;
    pImpl->spikeDetected = false;
    pImpl->refractoryRemaining = 0;
}

void FitzHughNagumoDynamics::setCubicParameter(float b) { pImpl->b = b; }
float FitzHughNagumoDynamics::getCubicParameter() const { return pImpl->b; }

void FitzHughNagumoDynamics::setRecoveryTimeConstant(float tau) { pImpl->tau = tau; }
float FitzHughNagumoDynamics::getRecoveryTimeConstant() const { return pImpl->tau; }

void FitzHughNagumoDynamics::setThreshold(float threshold) { pImpl->threshold = threshold; }
float FitzHughNagumoDynamics::getThreshold() const { return pImpl->threshold; }

void FitzHughNagumoDynamics::setMembraneTimeConstant(float tau) { pImpl->tau = tau; }
float FitzHughNagumoDynamics::getMembraneTimeConstant() const { return pImpl->tau; }

void FitzHughNagumoDynamics::setExternalCurrent(float Iext) { pImpl->externalCurrent = Iext; }
float FitzHughNagumoDynamics::getExternalCurrent() const { return pImpl->externalCurrent; }

void FitzHughNagumoDynamics::setIntegrationMethod(FitzHughNagumoDynamics::IntegrationMethod method) { pImpl->integrationMethod = method; }
void FitzHughNagumoDynamics::setExcitabilityType(ExcitabilityType type) { pImpl->excitabilityType = type; }
FitzHughNagumoDynamics::ExcitabilityType FitzHughNagumoDynamics::getExcitabilityType() const { return pImpl->excitabilityType; }

bool FitzHughNagumoDynamics::hasRecentSpike() const { return pImpl->spikeDetected; }
float FitzHughNagumoDynamics::getLastSpikeTime() const { return pImpl->lastSpikeTime; }
void FitzHughNagumoDynamics::clearRecentSpikes() { pImpl->spikeDetected = false; }

// Enhanced IntegrateAndFire implementation
IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}
IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    if (!neuron) return;
    
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    float I_ext = neuron->getTotalCurrent();
    float dt_f = static_cast<float>(dt);
    
    float I_syn = state.synapseConductance * (V - (state.synapseConductance > 0 ? 0.0f : -70.0f));
    float I_total = I_ext + I_syn;
    
    float adaptationIncrement = pImpl->adaptationVariable * pImpl->adaptationRate;
    float threshold = pImpl->threshold + adaptationIncrement;
    
    if (pImpl->integrationMethod == IntegrateAndFireDynamics::IntegrationMethod::Euler) {
        float dV = (-(V - state.restingPotential) / pImpl->tau_m + I_total / pImpl->Cm) * dt_f;
        neuron->setMembranePotential(V + dV);
    } else {
        float V_pred = V + (-(V - state.restingPotential) / pImpl->tau_m + I_total / pImpl->Cm) * dt_f;
        neuron->setMembranePotential(V_pred);
    }
    
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);
        pImpl->adaptationVariable += 0.1f;
        neuron->setMembranePotential(state.resetPotential);
        neuron->setRefractoryPeriod(pImpl->refractoryPeriod);
    } else {
        pImpl->adaptationVariable *= 0.9f;
        pImpl->adaptationVariable = std::max(0.0f, pImpl->adaptationVariable);
    }
    
    if (neuron->isRefractory()) {
        if (pImpl->refractoryRemaining > 0) pImpl->refractoryRemaining--;
        neuron->setMembranePotential(state.resetPotential);
    }
    
    float conductanceChange = I_total * dt_f / pImpl->g_exc_max;
    neuron->getState().synapseConductance += conductanceChange;
    neuron->getState().synapseConductance = std::max(0.0f, std::min(1.0f, neuron->getState().synapseConductance));
    
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    if (!synapse) return;
    
    float V = synapse->getSourceNeuron() ? synapse->getSourceNeuron()->getMembranePotential() : 0.0f;
    float weight = synapse->getWeight();
    
    float g_syn = weight * (synapse->isExcitatory() ? pImpl->g_exc_max : pImpl->g_inh_max);
    
    float efficacyChange = g_syn * dt / (synapse->isExcitatory() ? 10.0f : 5.0f);
    synapse->setEfficacy(synapse->getEfficacy() + efficacyChange);
    synapse->setEfficacy(std::max(0.0f, std::min(1.0f, synapse->getEfficacy())));
    
    synapse->decayEligibilityTrace(0.01f);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    if (!neuron || !synapse) return;
    
    float weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    
    float effectiveWeight = weight * efficacy;
    
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(effectiveWeight * pImpl->E_exc);
    } else {
        neuron->receiveInhibitoryInput(effectiveWeight * std::abs(pImpl->E_inh));
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    if (!neuron) return false;
    
    float V = neuron->getMembranePotential();
    float baseThreshold = neuron->getThreshold();
    float adaptationIncrement = pImpl->adaptationVariable * pImpl->adaptationRate;
    
    return V >= (baseThreshold + adaptationIncrement);
}

void IntegrateAndFireDynamics::reset() {
    pImpl->adaptationVariable = 0.0f;
    pImpl->refractoryRemaining = 0;
    pImpl->Cm = 0.25f;
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) { pImpl->tau_m = tau; }
float IntegrateAndFireDynamics::getMembraneTimeConstant() const { return pImpl->tau_m; }

} // namespace nlm