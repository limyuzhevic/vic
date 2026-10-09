#include "NeuralDynamics.hpp"
#include <cmath>
#include <random>

namespace nlm {

// Helper functions for rate constant calculations
static float alpha_m(float V, float temperature = 6.0f) {
    float V_shifted = V + 40.0f;
    float temp_factor = std::pow(3.0f, (temperature - 6.0f) / 10.0f);
    return temp_factor * (V_shifted) / (1.0f - std::exp(-V_shifted / 10.0f));
}

static float beta_m(float V, float temperature = 6.0f) {
    float V_shifted = V + 40.0f;
    float temp_factor = std::pow(3.0f, (temperature - 6.0f) / 10.0f);
    return temp_factor * 4.0f * std::exp(-V_shifted / 18.0f);
}

static float alpha_h(float V, float temperature = 6.0f) {
    float V_shifted = V + 60.0f;
    float temp_factor = std::pow(3.0f, (temperature - 6.0f) / 10.0f);
    return temp_factor * 0.055f * std::exp(-V_shifted / 80.0f);
}

static float beta_h(float V, float temperature = 6.0f) {
    float V_shifted = V + 60.0f;
    float temp_factor = std::pow(3.0f, (temperature - 6.0f) / 10.0f);
    return temp_factor * 0.94f * std::exp(V_shifted / 24.0f);
}

static float alpha_n(float V, float temperature = 6.0f) {
    float V_shifted = V + 50.0f;
    float temp_factor = std::pow(3.0f, (temperature - 6.0f) / 10.0f);
    return temp_factor * (V_shifted) / (1.0f - std::exp(-V_shifted / 10.0f));
}

static float beta_n(float V, float temperature = 6.0f) {
    float V_shifted = V + 50.0f;
    float temp_factor = std::pow(3.0f, (temperature - 6.0f) / 10.0f);
    return temp_factor * 0.125f * std::exp(-V_shifted / 80.0f);
}

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float capacitance;            // pF
    float restingPotential;      // mV
    float threshold;              // mV
    float resetPotential;         // mV
    float leakConductance;        // nS
    float adaptationVariable;     // nA
    float adaptationTimeConstant; // ms
    
    // Stochasticity
    bool enableNoise;
    float noiseLevel;
    std::mt19937 rng;
    std::normal_distribution<float> noiseDist;
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(100.0f), capacitance(1.0f),
             restingPotential(-70.0f), threshold(-55.0f), resetPotential(-70.0f),
             leakConductance(10.0f), adaptationVariable(0.0f), adaptationTimeConstant(100.0f),
             enableNoise(false), noiseLevel(0.1f), rng(std::random_device{}()), noiseDist(0.0f, 1.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float I = neuron->getTotalCurrent();
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    float C = pImpl->capacitance;
    float gL = pImpl->leakConductance;
    float w = pImpl->adaptationVariable;
    
    // Leaky integrate-and-fire with adaptive threshold
    float dV = dt * (-(V - V_rest) / (R * C) + I / C - w / C + gL * (V_rest - V) / C);
    
    // Add stochastic noise if enabled
    if (pImpl->enableNoise) {
        float noise = pImpl->noiseDist(pImpl->rng) * pImpl->noiseLevel;
        dV += noise * std::sqrt(dt);
    }
    
    V += dV;
    neuron->setMembranePotential(V);
    
    // Adaptive threshold due to spike-frequency adaptation
    float adaptationCurrent = w;
    neuron->setMembranePotential(V - adaptationCurrent * dt);
    
    // Update adaptation variable
    pImpl->adaptationVariable += dt * (0.01f * (0.1f - w));
    
    // Check for firing
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);
        
        // Reset membrane potential and increase adaptation
        neuron->setMembranePotential(state.resetPotential);
        pImpl->adaptationVariable += 1.0f;  // Spike-induced adaptation
    }
    
    // Refractory mechanism
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Conductance-based synapse with exponential decay
    float weight = synapse->getWeight();
    float conductance = std::abs(weight) * synapse->getEfficacy();
    
    // Update synapse efficacy (short-term plasticity)
    if (synapse->getPlasticityFlags().stdp) {
        // Simple STDP update would go here
        // For now, use a simple exponential decay
        synapse->setEfficacy(synapse->getEfficacy() * std::exp(-dt / pImpl->membraneTimeConstant));
    }
    
    synapse->step(0.0);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    SynapticWeight weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    
    // Apply conductance-based input
    float effectiveWeight = weight * efficacy;
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(effectiveWeight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(effectiveWeight));
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void IntegrateAndFireDynamics::reset() {
    if (pImpl) {
        pImpl->adaptationVariable = 0.0f;
    }
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

// Hodgkin-Huxley Dynamics Implementation
struct HodgkinHuxleyDynamics::Impl {
    // Ion channel conductances (mS/cm^2)
    float gNa;      // Sodium conductance
    float gK;       // Potassium conductance  
    float gL;       // Leak conductance
    
    // Reversal potentials (mV)
    float ENa;      // Sodium reversal potential
    float EK;       // Potassium reversal potential
    float EL;       // Leak reversal potential
    
    // Rate constants (ms^-1)
    float alpha_m;   // Sodium activation rate
    float beta_m;    // Sodium deactivation rate
    float alpha_h;   // Sodium inactivation rate
    float beta_h;    // Sodium deinactivation rate
    float alpha_n;   // Potassium activation rate
    float beta_n;    // Potassium deactivation rate
    
    // Gating variables
    float m;         // Sodium activation (0-1)
    float h;         // Sodium inactivation (0-1)
    float n;         // Potassium activation (0-1)
    
    // Temperature factor
    float temperature;  // Celsius
    
    // Numerical stability
    float stabilityThreshold;
    float maxStepSize;
    
    // Stochasticity
    bool enableStochasticity;
    float noiseLevel;
    std::mt19937 rng;
    std::normal_distribution<float> noiseDist;
    
    Impl() : gNa(120.0f), gK(36.0f), gL(0.3f),
             ENa(50.0f), EK(-77.0f), EL(-54.387f),
             temperature(6.0f), stabilityThreshold(1e-6f), maxStepSize(0.01f),
             enableStochasticity(false), noiseLevel(0.1f),
             rng(std::random_device{}()), noiseDist(0.0f, 1.0f) {
        m = alpha_m(0.0f, temperature) / (alpha_m(0.0f, temperature) + beta_m(0.0f, temperature));
        h = alpha_h(0.0f, temperature) / (alpha_h(0.0f, temperature) + beta_h(0.0f, temperature));
        n = alpha_n(0.0f, temperature) / (alpha_n(0.0f, temperature) + beta_n(0.0f, temperature));
    }
};

HodgkinHuxleyDynamics::HodgkinHuxleyDynamics() : pImpl(new Impl) {}

HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() = default;

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float I = neuron->getTotalCurrent();
    
    // Calculate rate constants with temperature correction
    float temp_factor = std::pow(3.0f, (pImpl->temperature - 6.0f) / 10.0f);
    
    float am = temp_factor * (V + 40.0f) / (1.0f - std::exp(-(V + 40.0f) / 10.0f));
    float bm = temp_factor * 4.0f * std::exp(-(V + 40.0f) / 18.0f);
    float ah = temp_factor * 0.055f * std::exp(-(V + 60.0f) / 80.0f);
    float bh = temp_factor * 0.94f * std::exp((V + 60.0f) / 24.0f);
    float an = temp_factor * (V + 50.0f) / (1.0f - std::exp(-(V + 50.0f) / 10.0f));
    float bn = temp_factor * 0.125f * std::exp(-(V + 50.0f) / 80.0f);
    
    // Update gating variables using Euler method
    float dm = (am * (1.0f - pImpl->m) - bm * pImpl->m) * dt;
    float dh = (ah * (1.0f - pImpl->h) - bh * pImpl->h) * dt;
    float dn = (an * (1.0f - pImpl->n) - bn * pImpl->n) * dt;
    
    // Numerical stability check
    if (std::abs(dm) > pImpl->maxStepSize) dm = (dm > 0.0f) ? pImpl->maxStepSize : -pImpl->maxStepSize;
    if (std::abs(dh) > pImpl->maxStepSize) dh = (dh > 0.0f) ? pImpl->maxStepSize : -pImpl->maxStepSize;
    if (std::abs(dn) > pImpl->maxStepSize) dn = (dn > 0.0f) ? pImpl->maxStepSize : -pImpl->maxStepSize;
    
    // Update gating variables
    pImpl->m += dm;
    pImpl->h += dh;
    pImpl->n += dn;
    
    // Clamp to [0, 1] range
    pImpl->m = std::max(0.0f, std::min(1.0f, pImpl->m));
    pImpl->h = std::max(0.0f, std::min(1.0f, pImpl->h));
    pImpl->n = std::max(0.0f, std::min(1.0f, pImpl->n));
    
    // Add stochastic noise if enabled
    if (pImpl->enableStochasticity) {
        float noise = pImpl->noiseDist(pImpl->rng) * pImpl->noiseLevel * std::sqrt(dt);
        pImpl->m = std::max(0.0f, std::min(1.0f, pImpl->m + noise));
        pImpl->h = std::max(0.0f, std::min(1.0f, pImpl->h + noise));
        pImpl->n = std::max(0.0f, std::min(1.0f, pImpl->n + noise));
    }
    
    // Calculate ionic currents
    float INa = pImpl->gNa * std::pow(pImpl->m, 3.0f) * pImpl->h * (V - pImpl->ENa);
    float IK = pImpl->gK * std::pow(pImpl->n, 4.0f) * (V - pImpl->EK);
    float IL = pImpl->gL * (V - pImpl->EL);
    
    // Calculate membrane potential derivative
    float dV = (I - INa - IK - IL) / 1.0f;  // Assuming C = 1.0 μF/cm²
    
    // Update membrane potential
    V += dV * dt;
    neuron->setMembranePotential(V);
    
    // Check for firing (threshold is around 30 mV in HH model)
    if (V >= 30.0f && neuron->getFiringState() != FiringState::Active) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);
        
        // Spike initiated, set voltage to ~50 mV (after-potential)
        neuron->setMembranePotential(V);
    }
    
    // Refractory mechanism
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
        // Reset gating variables to steady-state values at reset potential
        pImpl->m = alpha_m(state.resetPotential, pImpl->temperature) / 
                   (alpha_m(state.resetPotential, pImpl->temperature) + beta_m(state.resetPotential, pImpl->temperature));
        pImpl->h = alpha_h(state.resetPotential, pImpl->temperature) / 
                   (alpha_h(state.resetPotential, pImpl->temperature) + beta_h(state.resetPotential, pImpl->temperature));
        pImpl->n = alpha_n(state.resetPotential, pImpl->temperature) / 
                   (alpha_n(state.resetPotential, pImpl->temperature) + beta_n(state.resetPotential, pImpl->temperature));
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Conductance-based synapse with voltage-dependent dynamics
    float weight = synapse->getWeight();
    
    // Synaptic conductance dynamics
    float conductance = synapse->getEfficacy();
    float decay_rate = 1.0f / (pImpl->gK * dt);  // Time constant based on potassium
    
    // Update conductance (exponential decay)
    float new_conductance = conductance * std::exp(-dt / decay_rate);
    synapse->setEfficacy(new_conductance);
    
    // Apply synaptic input to postsynaptic neuron (would need neuron reference)
    // For now, just step the synapse
    synapse->step(0.0);
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    SynapticWeight weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    
    // Calculate synaptic conductance change
    float conductance_change = weight * efficacy;
    
    if (synapse->isExcitatory()) {
        // Excitatory: increase sodium conductance
        // In HH model, excitatory input affects sodium channels
        neuron->receiveExcitatoryInput(conductance_change);
    } else {
        // Inhibitory: increase potassium conductance
        neuron->receiveInhibitoryInput(std::abs(conductance_change));
    }
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= 30.0f;
}

void HodgkinHuxleyDynamics::reset() {
    if (pImpl) {
        pImpl->m = alpha_m(0.0f, pImpl->temperature) / (alpha_m(0.0f, pImpl->temperature) + beta_m(0.0f, pImpl->temperature));
        pImpl->h = alpha_h(0.0f, pImpl->temperature) / (alpha_h(0.0f, pImpl->temperature) + beta_h(0.0f, pImpl->temperature));
        pImpl->n = alpha_n(0.0f, pImpl->temperature) / (alpha_n(0.0f, pImpl->temperature) + beta_n(0.0f, pImpl->temperature));
    }
}

void HodgkinHuxleyDynamics::loadConfig(const Config& config) {
    if (pImpl) {
        if (config.has("HH_TEMPERATURE")) {
            pImpl->temperature = config.getOr<float>("HH_TEMPERATURE", 6.0f);
        }
        if (config.has("HH_GNa")) {
            pImpl->gNa = config.getOr<float>("HH_GNa", 120.0f);
        }
        if (config.has("HH_GK")) {
            pImpl->gK = config.getOr<float>("HH_GK", 36.0f);
        }
        if (config.has("HH_GL")) {
            pImpl->gL = config.getOr<float>("HH_GL", 0.3f);
        }
        if (config.has("HH_ENa")) {
            pImpl->ENa = config.getOr<float>("HH_ENa", 50.0f);
        }
        if (config.has("HH_EK")) {
            pImpl->EK = config.getOr<float>("HH_EK", -77.0f);
        }
        if (config.has("HH_EL")) {
            pImpl->EL = config.getOr<float>("HH_EL", -54.387f);
        }
        if (config.has("HH_ENABLE_NOISE")) {
            pImpl->enableStochasticity = config.getOr<bool>("HH_ENABLE_NOISE", false);
        }
        if (config.has("HH_NOISE_LEVEL")) {
            pImpl->noiseLevel = config.getOr<float>("HH_NOISE_LEVEL", 0.1f);
        }
    }
}

const char* HodgkinHuxleyDynamics::getName() const {
    return "HodgkinHuxley";
}

void HodgkinHuxleyDynamics::setSodiumConductance(float gNa) {
    if (pImpl) pImpl->gNa = gNa;
}

void HodgkinHuxleyDynamics::setPotassiumConductance(float gK) {
    if (pImpl) pImpl->gK = gK;
}

void HodgkinHuxleyDynamics::setLeakConductance(float gL) {
    if (pImpl) pImpl->gL = gL;
}

void HodgkinHuxleyDynamics::setTemperature(float temperature) {
    pImpl->temperature = temperature;
    // Recompute gating variables with new temperature
    float am = alpha_m(0.0f, temperature);
    float bm = beta_m(0.0f, temperature);
    float ah = alpha_h(0.0f, temperature);
    float bh = beta_h(0.0f, temperature);
    float an = alpha_n(0.0f, temperature);
    float bn = beta_n(0.0f, temperature);
    
    pImpl->m = am / (am + bm);
    pImpl->h = ah / (ah + bh);
    pImpl->n = an / (an + bn);
}

float HodgkinHuxleyDynamics::getSodiumConductance() const {
    return pImpl ? pImpl->gNa : 0.0f;
}

float HodgkinHuxleyDynamics::getPotassiumConductance() const {
    return pImpl ? pImpl->gK : 0.0f;
}

float HodgkinHuxleyDynamics::getLeakConductance() const {
    return pImpl ? pImpl->gL : 0.0f;
}

float HodgkinHuxleyDynamics::getTemperature() const {
    return pImpl ? pImpl->temperature : 0.0f;
}

// FitzHugh-Nagumo Dynamics Implementation
struct FitzHughNagumoDynamics::Impl {
    // FHN model parameters
    float threshold;           // Threshold for excitation (mV)
    float gradient;            // Gradient parameter (0.7-0.8 for excitable)
    float baseline;            // Baseline parameter (0.7 for excitable)
    float recoveryTime;        // Recovery time constant
    
    // Numerical parameters
    float stabilityThreshold;  // Numerical stability threshold
    float maxStepSize;         // Maximum integration step size
    
    // Stochasticity
    bool enableStochasticity;
    float noiseLevel;
    std::mt19937 rng;
    std::normal_distribution<float> noiseDist;
    
    Impl() : threshold(-0.5f), gradient(0.8f), baseline(0.7f), recoveryTime(12.5f),
             stabilityThreshold(1e-6f), maxStepSize(0.01f),
             enableStochasticity(false), noiseLevel(0.1f),
             rng(std::random_device{}()), noiseDist(0.0f, 1.0f) {}
};

FitzHughNagumoDynamics::FitzHughNagumoDynamics() : pImpl(new Impl) {}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const auto& state = neuron->getState();
    
    float V = neuron->getMembranePotential();
    float I = neuron->getTotalCurrent();
    
    // FHN equations:
    // dv/dt = (v - v^3/3 - w + I)
    // dw/dt = (v + a - b*w)/tau
    
    float dv = (V - V*V*V/3.0f - state.adaptationVariable + I) * dt;
    float dw = (V + pImpl->baseline - pImpl->gradient * state.adaptationVariable) / pImpl->recoveryTime * dt;
    
    // Numerical stability check
    if (std::abs(dv) > pImpl->maxStepSize) dv = (dv > 0.0f) ? pImpl->maxStepSize : -pImpl->maxStepSize;
    if (std::abs(dw) > pImpl->maxStepSize) dw = (dw > 0.0f) ? pImpl->maxStepSize : -pImpl->maxStepSize;
    
    // Update voltage
    V += dv;
    neuron->setMembranePotential(V);
    
    // Update recovery variable
    state.adaptationVariable += dw;
    
    // Add stochastic noise if enabled
    if (pImpl->enableStochasticity) {
        float noise = pImpl->noiseDist(pImpl->rng) * pImpl->noiseLevel * std::sqrt(dt);
        V += noise;
        neuron->setMembranePotential(V);
    }
    
    // Check for firing (spike when voltage crosses threshold)
    if (V > pImpl->threshold && neuron->getFiringState() != FiringState::Active) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);
        
        // Reset after spike
        neuron->setMembranePotential(-2.0f);  // Reset below threshold
    }
    
    // Refractory mechanism
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
    }
    
    // Clear current for next step
    neuron->clearTotalCurrent();
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Conductance-based synapse with FHN-style dynamics
    float weight = synapse->getWeight();
    
    // Synaptic efficacy with FHN-like dynamics (bang-bang or threshold behavior)
    float efficacy = synapse->getEfficacy();
    
    // Update efficacy with thresholded dynamics
    if (efficacy > 0.5f) {
        // Decay towards equilibrium
        efficacy *= std::exp(-dt / pImpl->recoveryTime);
    } else if (efficacy < 0.5f) {
        // Grow towards equilibrium
        efficacy += (0.5f - efficacy) * std::exp(-dt / pImpl->recoveryTime);
    }
    
    synapse->setEfficacy(efficacy);
    synapse->step(0.0);
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    SynapticWeight weight = synapse->getWeight();
    float efficacy = synapse->getEfficacy();
    
    // Calculate synaptic input using FHN-style threshold behavior
    float effectiveWeight = weight * efficacy;
    
    if (effectiveWeight > 0.1f) {  // Threshold-based activation
        if (synapse->isExcitatory()) {
            neuron->receiveExcitatoryInput(effectiveWeight * 2.0f);  // Amplified
        } else {
            neuron->receiveInhibitoryInput(std::abs(effectiveWeight) * 2.0f);  // Amplified
        }
    }
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() > pImpl->threshold;
}

void FitzHughNagumoDynamics::reset() {
    if (pImpl) {
        // Reset to resting state
    }
}

void FitzHughNagumoDynamics::loadConfig(const Config& config) {
    if (pImpl) {
        if (config.has("FHN_THRESHOLD")) {
            pImpl->threshold = config.getOr<float>("FHN_THRESHOLD", -0.5f);
        }
        if (config.has("FHN_GRADIENT")) {
            pImpl->gradient = config.getOr<float>("FHN_GRADIENT", 0.8f);
        }
        if (config.has("FHN_BASELINE")) {
            pImpl->baseline = config.getOr<float>("FHN_BASELINE", 0.7f);
        }
        if (config.has("FHN_RECOVERY_TIME")) {
            pImpl->recoveryTime = config.getOr<float>("FHN_RECOVERY_TIME", 12.5f);
        }
        if (config.has("FHN_ENABLE_NOISE")) {
            pImpl->enableStochasticity = config.getOr<bool>("FHN_ENABLE_NOISE", false);
        }
        if (config.has("FHN_NOISE_LEVEL")) {
            pImpl->noiseLevel = config.getOr<float>("FHN_NOISE_LEVEL", 0.1f);
        }
    }
}

const char* FitzHughNagumoDynamics::getName() const {
    return "FitzHughNagumo";
}

void FitzHughNagumoDynamics::setThreshold(float threshold) {
    pImpl->threshold = threshold;
}

void FitzHughNagumoDynamics::setGradient(float gradient) {
    pImpl->gradient = gradient;
}

void FitzHughNagumoDynamics::setBaseline(float baseline) {
    pImpl->baseline = baseline;
}

void FitzHughNagumoDynamics::setRecoveryTime(float recoveryTime) {
    pImpl->recoveryTime = recoveryTime;
}

float FitzHughNagumoDynamics::getThreshold() const {
    return pImpl ? pImpl->threshold : -0.5f;
}

float FitzHughNagumoDynamics::getGradient() const {
    return pImpl ? pImpl->gradient : 0.8f;
}

float FitzHughNagumoDynamics::getBaseline() const {
    return pImpl ? pImpl->baseline : 0.7f;
}

float FitzHughNagumoDynamics::getRecoveryTime() const {
    return pImpl ? pImpl->recoveryTime : 12.5f;
}

} // namespace nlm

