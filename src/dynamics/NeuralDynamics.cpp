// Fixed NeuralDynamics implementation

#include "NeuralDynamics.hpp"
#include <cmath>
#include <random>
#include <algorithm>
#include <limits>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    // Biophysical parameters
    float membraneCapacitance;  // Membrane capacitance (pF)
    float membraneResistance;   // Membrane resistance (MOhm)
    float membraneTimeConstant; // Membrane time constant (ms)
    
    // Ion channels
    float sodiumConductance;   // Sodium channel conductance (nS)
    float potassiumConductance; // Potassium channel conductance (nS)
    float leakConductance;     // Leak conductance (nS)
    float calciumConductance;  // Calcium channel conductance (nS)
    float adaptationConductance; // Spike frequency adaptation (nS)
    
    // Reversal potentials
    float v_exc;      // Excitatory reversal potential (mV)
    float v_inh;      // Inhibitory reversal potential (mV)
    float v_leak;     // Leak reversal potential (mV)
    float v_na;       // Sodium reversal potential (mV)
    float v_k;        // Potassium reversal potential (mV)
    float v_ca;       // Calcium reversal potential (mV)
    
    // Temperature and noise
    float temperature;           // Temperature (K)
    float noiseLevel;             // Noise standard deviation
    unsigned int randomSeed;      // Random seed
    mutable std::mt19937 rng;     // Random number generator
    float thermalFactor;          // Temperature scaling factor
    
    // Synaptic conductances
    std::vector<SynapticConductance> excitatoryConductances;
    std::vector<SynapticConductance> inhibitoryConductances;
    
    // Integration buffers
    std::vector<float> voltageHistory;
    std::vector<float> currentHistory;
    
    Impl() : 
        membraneCapacitance(100.0f),
        membraneResistance(10000.0f),
        membraneTimeConstant(20.0f),
        sodiumConductance(120.0f),
        potassiumConductance(36.0f),
        leakConductance(0.3f),
        calciumConductance(0.5f),
        adaptationConductance(2.0f),
        v_exc(50.0f),
        v_inh(-80.0f),
        v_leak(-70.0f),
        v_na(55.0f),
        v_k(-90.0f),
        v_ca(120.0f),
        temperature(310.15f), // 37°C
        noiseLevel(0.01f),
        randomSeed(12345u),
        rng(12345u),
        thermalFactor(1.0f) {
        
        // Initialize random number generator
        std::seed_seq seed{rng()};
        rng.seed(seed);
    }
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const NeuronState& state = neuron->getState();
    
    // Get thermal scaling factor
    float thermalFactor = std::pow(3.0f, (pImpl->temperature - 310.15f) / 10.0f);
    pImpl->thermalFactor = thermalFactor;
    
    // Generate stochastic noise for biological realism
    std::normal_distribution<float> noiseDist(0.0f, pImpl->noiseLevel * thermalFactor);
    float noise = noiseDist(pImpl->rng);
    
    // Calculate total synaptic current including dendritic filtering
    float totalConductance = state.leakConductance + state.synapseConductance;
    float totalCurrent = 0.0f;
    
    // Excitation contribution (with dendritic integration)
    if (neuron->getType() == NeuronType::Excitatory || 
        neuron->getType() == NeuronType::Internal) {
        float excConductance = 0.0f;
        for (const auto& g : pImpl->excitatoryConductances) {
            excConductance += g.voltage;
        }
        float excCurrent = excConductance * (pImpl->v_exc - neuron->getMembranePotential() +
            noise * pImpl->membraneResistance);
        totalCurrent += excCurrent;
    }
    
    // Inhibition contribution
    if (neuron->getType() == NeuronType::Inhibitory) {
        float inhConductance = 0.0f;
        for (const auto& g : pImpl->inhibitoryConductances) {
            inhConductance += g.voltage;
        }
        float inhCurrent = inhConductance * (pImpl->v_inh - neuron->getMembranePotential() -
            noise * pImpl->membraneResistance);
        totalCurrent += inhCurrent;
    }
    
    // Leak current (with temperature dependence)
    float leakCurrent = totalConductance * (pImpl->v_leak - neuron->getMembranePotential());
    totalCurrent += leakCurrent;
    
    // Ionic currents (if neuron type supports it)
    if (neuron->getType() == NeuronType::Sensory || neuron->getType() == NeuronType::Internal) {
        // Simplified ion channel dynamics
        float sodiumCurrent = pImpl->sodiumConductance * 
            std::pow(neuron->getMembranePotential(), 3) * 
            (1.0f - neuron->getMembranePotential() / 50.0f) * thermalFactor;
        
        float potassiumCurrent = pImpl->potassiumConductance * 
            std::pow(neuron->getMembranePotential(), 4) * thermalFactor;
        
        float calciumCurrent = pImpl->calciumConductance * 
            (1.0f - neuron->getMembranePotential() / 100.0f) * thermalFactor;
        
        totalCurrent += (sodiumCurrent - potassiumCurrent + calciumCurrent);
    }
    
    // Apply temperature scaling to membrane dynamics
    float effectiveTimeConstant = pImpl->membraneTimeConstant / thermalFactor;
    float effectiveCapacitance = pImpl->membraneCapacitance * thermalFactor;
    
    // Membrane potential update with exponential Euler integration
    MembranePotential V = neuron->getMembranePotential();
    float dV_dt = (totalCurrent / effectiveCapacitance) - 
                  ((V - pImpl->v_leak) / effectiveTimeConstant) +
                  noise * 0.1f;
    
    neuron->addToMembranePotential(dV_dt * static_cast<float>(dt) * 1000.0f);
    
    // Temperature-dependent threshold
    float thermalThreshold = state.threshold * (1.0f + 0.02f * (pImpl->temperature - 310.15f) / 10.0f);
    
    // Refractory period handling
    if (neuron->isRefractory()) {
        neuron->setMembranePotential(state.resetPotential);
        neuron->decrementRefractory();
        return;
    }
    
    // Spike detection with noise modulation
    if (V + noise * pImpl->membraneResistance >= thermalThreshold) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0); // Would pass actual time in full implementation
        
        // Reset membrane potential
        neuron->setMembranePotential(state.resetPotential);
        
        // Enter refractory period
        neuron->setRefractoryPeriod(static_cast<uint32_t>(pImpl->membraneTimeConstant * 2.0f * thermalFactor));
        neuron->setFiringState(FiringState::Refractory);
        
        // Update adaptation for next spike
        NeuronState& neuronState = neuron->getState();
        neuronState.adaptationVariable += pImpl->adaptationConductance * thermalFactor;
    }
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Simple synapse update
    synapse->step(0.0);
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Simple synaptic input
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(synapse->getWeight() * 1000.0f);
    } else {
        neuron->receiveInhibitoryInput(std::abs(synapse->getWeight()) * 1000.0f);
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    float V = neuron->getMembranePotential();
    
    // Temperature-dependent threshold with noise
    float thermalFactor = std::pow(3.0f, (pImpl->temperature - 310.15f) / 10.0f);
    float threshold = neuron->getThreshold() * (1.0f + 0.02f * (pImpl->temperature - 310.15f));
    
    // Add stochastic component for biological realism
    std::normal_distribution<float> noiseDist(0.0f, pImpl->noiseLevel * thermalFactor);
    float noise = noiseDist(pImpl->rng);
    
    return V + noise * pImpl->membraneResistance >= threshold;
}

void IntegrateAndFireDynamics::reset() {
    pImpl->excitatoryConductances.clear();
    pImpl->inhibitoryConductances.clear();
    pImpl->voltageHistory.clear();
    pImpl->currentHistory.clear();
}

void IntegrateAndFireDynamics::setTemperature(float temperature) {
    pImpl->temperature = temperature;
    pImpl->thermalFactor = Thermal::q10TemperatureFactor(temperature);
}

float IntegrateAndFireDynamics::getTemperature() const {
    return pImpl->temperature;
}

void IntegrateAndFireDynamics::setRandomSeed(unsigned int seed) {
    pImpl->randomSeed = seed;
    pImpl->rng.seed(seed);
}

float IntegrateAndFireDynamics::getNoiseLevel() const {
    return pImpl->noiseLevel;
}

void IntegrateAndFireDynamics::setNoiseLevel(float noise) {
    pImpl->noiseLevel = noise;
}

void IntegrateAndFireDynamics::setSodiumConductance(float g_na) {
    pImpl->sodiumConductance = g_na;
}

void IntegrateAndFireDynamics::setPotassiumConductance(float g_k) {
    pImpl->potassiumConductance = g_k;
}

void IntegrateAndFireDynamics::setLeakConductance(float g_l) {
    pImpl->leakConductance = g_l;
}

void IntegrateAndFireDynamics::setCalciumConductance(float g_ca) {
    pImpl->calciumConductance = g_ca;
}

void IntegrateAndFireDynamics::setMembraneCapacitance(float capacitance) {
    pImpl->membraneCapacitance = capacitance;
}

void IntegrateAndFireDynamics::setRefractoryPeriod(float period) {
    pImpl->membraneTimeConstant = period;
}

void IntegrateAndFireDynamics::setAdaptationConductance(float g_adapt) {
    pImpl->adaptationConductance = g_adapt;
}

void IntegrateAndFireDynamics::setDendriticResistivity(float resistivity) {
    // Store but not used in this simplified implementation
}

void IntegrateAndFireDynamics::setDendriticCapacitance(float capacitance) {
    // Store but not used in this simplified implementation
}

void IntegrateAndFireDynamics::setReversalPotential(float v_exc, float v_inh, float v_leak) {
    pImpl->v_exc = v_exc;
    pImpl->v_inh = v_inh;
    pImpl->v_leak = v_leak;
}

void IntegrateAndFireDynamics::setSynapticTimeConstants(float tau_rise, float tau_decay) {
    // Store but not used in this simplified implementation
}

} // namespace nlm

// Hodgkin-Huxley implementation
namespace nlm {

struct HodgkinHuxleyDynamics::Impl {
    // HH ion channel parameters
    float g_na_max;      // Maximum sodium conductance (mS/cm²)
    float g_k_max;       // Maximum potassium conductance (mS/cm²)
    float g_l;           // Leak conductance (mS/cm²)
    
    // Reversal potentials
    float v_na;          // Sodium reversal potential (mV)
    float v_k;           // Potassium reversal potential (mV)
    float v_l;           // Leak reversal potential (mV)
    float v_ca;          // Calcium reversal potential (mV)
    
    // Channel kinetics parameters
    float phi;           // Temperature scaling factor
    float tau_m;         // Sodium activation time constant scaling
    float tau_h;         // Sodium inactivation time constant scaling
    float tau_n;         // Potassium activation time constant scaling
    
    // Adaptation parameters
    float alpha;         // Adaptation strength
    float tau_adapt;      // Adaptation time constant
    
    // Temperature
    float temperature;   // Temperature (K)
    float noiseLevel;     // Noise level
    
    // State variables for HH model
    float V;             // Membrane potential
    float m;             // Sodium activation
    float h;             // Sodium inactivation
    float n;             // Potassium activation
    float d;             // Calcium activation
    float f;             // Calcium inactivation
    
    // Random generator
    mutable std::mt19937 rng;
    
    Impl() :
        g_na_max(120.0f),
        g_k_max(36.0f),
        g_l(0.3f),
        v_na(55.0f),
        v_k(-90.0f),
        v_l(-70.0f),
        v_ca(120.0f),
        phi(1.0f),
        tau_m(1.0f),
        tau_h(1.0f),
        tau_n(1.0f),
        alpha(0.0f),
        tau_adapt(100.0f),
        temperature(310.15f),
        noiseLevel(0.01f),
        V(-65.0f),
        m(0.05f),
        h(0.6f),
        n(0.32f),
        d(0.01f),
        f(0.1f) {
        
        std::random_device rd;
        rng.seed(rd());
    }
};

HodgkinHuxleyDynamics::HodgkinHuxleyDynamics() : pImpl(new Impl) {}

HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() = default;

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    float& V = pImpl->V;
    float& m = pImpl->m;
    float& h = pImpl->h;
    float& n = pImpl->n;
    float& d = pImpl->d;
    float& f = pImpl->f;
    
    // Temperature scaling
    float temperatureFactor = std::pow(3.0f, (pImpl->temperature - 310.15f) / 10.0f);
    pImpl->phi = temperatureFactor;
    
    // Generate noise for stochastic HH model
    std::normal_distribution<float> noiseDist(0.0f, pImpl->noiseLevel * temperatureFactor);
    float noise = noiseDist(pImpl->rng);
    
    // Compute HH currents with temperature dependence
    float I_Na = pImpl->g_na_max * m * m * m * h * (V - pImpl->v_na);
    float I_K = pImpl->g_k_max * n * n * n * n * (V - pImpl->v_k);
    float I_L = pImpl->g_l * (V - pImpl->v_l);
    
    // Add calcium current
    float I_Ca = 0.0f;
    if (neuron->getType() == NeuronType::Sensory || neuron->getType() == NeuronType::Internal) {
        I_Ca = 0.5f * d * (1.0f - f) * (V - pImpl->v_ca);
    }
    
    // Adaptation current
    float I_adapt = pImpl->alpha * neuron->getState().adaptationVariable * (V - pImpl->v_l);
    
    // Total ionic current
    float I_ion = I_Na + I_K + I_L + I_Ca + I_adapt;
    
    // External current from synaptic inputs
    float I_ext = neuron->getTotalCurrent();
    
    // Membrane equation: C dV/dt = -I_ion + I_ext + noise
    float C = 1.0f; // Membrane capacitance (µF/cm²)
    float dV_dt = (-I_ion + I_ext + noise * 0.1f) / C;
    
    // Update membrane potential
    V += dV_dt * static_cast<float>(dt) * 1000.0f;
    
    // Update gating variables with temperature scaling
    float alpha_m = pImpl->phi * 0.1f * (V + 40.0f) / (1.0f - std::exp(-(V + 40.0f) / 10.0f));
    float beta_m = pImpl->phi * 4.0f * std::exp(-V / 18.0f);
    m += (alpha_m * (1.0f - m) - beta_m * m) * static_cast<float>(dt) * 1000.0f;
    m = std::clamp(m, 0.0f, 1.0f);
    
    float alpha_h = pImpl->phi * 0.07f * std::exp(-V / 20.0f);
    float beta_h = pImpl->phi * 1.0f / (1.0f + std::exp(-(V + 30.0f) / 10.0f));
    h += (alpha_h * (1.0f - h) - beta_h * h) * static_cast<float>(dt) * 1000.0f;
    h = std::clamp(h, 0.0f, 1.0f);
    
    float alpha_n = pImpl->phi * 0.01f * (V + 55.0f) / (1.0f - std::exp(-(V + 55.0f) / 10.0f));
    float beta_n = pImpl->phi * 0.125f * std::exp(-V / 80.0f);
    n += (alpha_n * (1.0f - n) - beta_n * n) * static_cast<float>(dt) * 1000.0f;
    n = std::clamp(n, 0.0f, 1.0f);
    
    // Calcium channel dynamics
    float alpha_d = 0.095f * (V + 50.0f) / (1.0f - std::exp(-(V + 50.0f) / 10.0f));
    float beta_d = 0.07f * std::exp(-V / 20.0f);
    d += (alpha_d * (1.0f - d) - beta_d * d) * static_cast<float>(dt) * 1000.0f;
    d = std::clamp(d, 0.0f, 1.0f);
    
    float alpha_f = 0.012f * (V + 60.0f) / (1.0f - std::exp(-(V + 60.0f) / 10.0f));
    float beta_f = 0.06f * std::exp(-V / 30.0f);
    f += (alpha_f * (1.0f - f) - beta_f * f) * static_cast<float>(dt) * 1000.0f;
    f = std::clamp(f, 0.0f, 1.0f);
    
    // Update neuron state
    neuron->setMembranePotential(V);
    
    // Spike detection
    if (V >= 50.0f) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);
        
        // Reset membrane potential
        V = pImpl->v_l;
        
        // Update adaptation variable
        NeuronState& neuronState = neuron->getState();
        neuronState.adaptationVariable += pImpl->alpha * temperatureFactor;
        
        // Enter refractory period
        neuron->setRefractoryPeriod(static_cast<uint32_t>(pImpl->tau_adapt * temperatureFactor));
        neuron->setFiringState(FiringState::Refractory);
    }
    
    // Clean up total current
    neuron->clearTotalCurrent();
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Simple synapse update
    synapse->step(0.0);
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Simple synaptic input
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(synapse->getWeight() * 1000.0f);
    } else {
        neuron->receiveInhibitoryInput(std::abs(synapse->getWeight()) * 1000.0f);
    }
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= 50.0f;
}

void HodgkinHuxleyDynamics::reset() {
    pImpl->V = -65.0f;
    pImpl->m = 0.05f;
    pImpl->h = 0.6f;
    pImpl->n = 0.32f;
    pImpl->d = 0.01f;
    pImpl->f = 0.1f;
}

void HodgkinHuxleyDynamics::setTemperature(float temperature) {
    pImpl->temperature = temperature;
}

float HodgkinHuxleyDynamics::getTemperature() const {
    return pImpl->temperature;
}

void HodgkinHuxleyDynamics::setRandomSeed(unsigned int seed) {
    pImpl->rng.seed(seed);
}

float HodgkinHuxleyDynamics::getNoiseLevel() const {
    return pImpl->noiseLevel;
}

void HodgkinHuxleyDynamics::setNoiseLevel(float noise) {
    pImpl->noiseLevel = noise;
}

void HodgkinHuxleyDynamics::setSodiumMaxConductance(float g_na_max) {
    pImpl->g_na_max = g_na_max;
}

void HodgkinHuxleyDynamics::setPotassiumMaxConductance(float g_k_max) {
    pImpl->g_k_max = g_k_max;
}

void HodgkinHuxleyDynamics::setLeakConductance(float g_l) {
    pImpl->g_l = g_l;
}

void HodgkinHuxleyDynamics::setSodiumReversalPotential(float v_na) {
    pImpl->v_na = v_na;
}

void HodgkinHuxleyDynamics::setPotassiumReversalPotential(float v_k) {
    pImpl->v_k = v_k;
}

void HodgkinHuxleyDynamics::setLeakReversalPotential(float v_l) {
    pImpl->v_l = v_l;
}

void HodgkinHuxleyDynamics::setCalciumReversalPotential(float v_ca) {
    pImpl->v_ca = v_ca;
}

void HodgkinHuxleyDynamics::setSodiumActivationTau(float tau_m) {
    pImpl->tau_m = tau_m;
}

void HodgkinHuxleyDynamics::setSodiumInactivationTau(float tau_h) {
    pImpl->tau_h = tau_h;
}

void HodgkinHuxleyDynamics::setPotassiumActivationTau(float tau_n) {
    pImpl->tau_n = tau_n;
}

void HodgkinHuxleyDynamics::setAdaptationStrength(float alpha) {
    pImpl->alpha = alpha;
}

void HodgkinHuxleyDynamics::setAdaptationTimeConstant(float tau_adapt) {
    pImpl->tau_adapt = tau_adapt;
}

} // namespace nlm

// FitzHugh-Nagumo implementation
namespace nlm {

struct FitzHughNagumoDynamics::Impl {
    // FHN parameters
    float epsilon;           // Time scale separation
    float threshold;         // Excitation threshold
    float recoveryTimeConstant; // Recovery time constant
    float couplingStrength;   // Coupling strength
    float baselineCurrent;    // Baseline current input
    
    // Temperature
    float temperature;       // Temperature (K)
    float noiseLevel;         // Noise level
    
    // State variables
    float V;                 // Membrane potential
    float w;                 // Recovery variable
    
    // Random generator
    mutable std::mt19937 rng;
    
    Impl() :
        epsilon(0.08f),
        threshold(0.5f),
        recoveryTimeConstant(12.0f),
        couplingStrength(0.8f),
        baselineCurrent(0.0f),
        temperature(310.15f),
        noiseLevel(0.01f),
        V(-1.2f),
        w(0.0f) {
        
        std::random_device rd;
        rng.seed(rd());
    }
};

FitzHughNagumoDynamics::FitzHughNagumoDynamics() : pImpl(new Impl) {}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    float& V = pImpl->V;
    float& w = pImpl->w;
    
    // Temperature scaling
    float temperatureFactor = std::pow(3.0f, (pImpl->temperature - 310.15f) / 10.0f);
    
    // Generate noise
    std::normal_distribution<float> noiseDist(0.0f, pImpl->noiseLevel * temperatureFactor);
    float noise = noiseDist(pImpl->rng);
    
    // FHN equations with noise
    float dV_dt = pImpl->epsilon * (V - V*V*V/3.0f + w + 
                            neuron->getTotalCurrent() * 0.001f + noise * 0.1f);
    
    float dw_dt = pImpl->epsilon * (0.08f * (V + 0.7f - 0.8f * w));
    
    // Update state variables
    V += dV_dt * static_cast<float>(dt) * 1000.0f;
    w += dw_dt * static_cast<float>(dt) * 1000.0f;
    
    // Apply spike threshold
    if (V >= pImpl->threshold) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);
        
        // Reset with recovery
        V = -1.2f;
        w += 0.5f; // Spike-induced recovery
        
        // Enter refractory period
        neuron->setRefractoryPeriod(static_cast<uint32_t>(pImpl->recoveryTimeConstant));
        neuron->setFiringState(FiringState::Refractory);
    }
    
    // Update neuron membrane potential
    neuron->setMembranePotential(V);
    
    // Clean up total current
    neuron->clearTotalCurrent();
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Simple synapse update
    synapse->step(0.0);
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Simple synaptic input
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(synapse->getWeight() * 1000.0f);
    } else {
        neuron->receiveInhibitoryInput(std::abs(synapse->getWeight()) * 1000.0f);
    }
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= pImpl->threshold;
}

void FitzHughNagumoDynamics::reset() {
    pImpl->V = -1.2f;
    pImpl->w = 0.0f;
}

void FitzHughNagumoDynamics::setTemperature(float temperature) {
    pImpl->temperature = temperature;
}

float FitzHughNagumoDynamics::getTemperature() const {
    return pImpl->temperature;
}

void FitzHughNagumoDynamics::setRandomSeed(unsigned int seed) {
    pImpl->rng.seed(seed);
}

float FitzHughNagumoDynamics::getNoiseLevel() const {
    return pImpl->noiseLevel;
}

void FitzHughNagumoDynamics::setNoiseLevel(float noise) {
    pImpl->noiseLevel = noise;
}

void FitzHughNagumoDynamics::setExcitationThreshold(float threshold) {
    pImpl->threshold = threshold;
}

void FitzHughNagumoDynamics::setRecoveryTimeConstant(float tau) {
    pImpl->recoveryTimeConstant = tau;
}

void FitzHughNagumoDynamics::setCouplingStrength(float epsilon) {
    pImpl->epsilon = epsilon;
}

void FitzHughNagumoDynamics::setBaselineCurrent(float current) {
    pImpl->baselineCurrent = current;
}

} // namespace nlm