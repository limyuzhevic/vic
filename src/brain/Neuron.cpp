#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

// Neuron implementation with enhanced LIF dynamics and plasticity

struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;  // Total synaptic current input this step
    MembranePotential synapticInput;  // Accumulated synaptic input
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // Enhanced LIF parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // Advanced biophysical parameters
    float inputResistance;        // Mohms
    float membraneTimeConstant;   // ms
    float thresholdDynamics;     // Threshold adaptation factor
    float adaptationStrength;     // Spike-frequency adaptation strength
    float noiseLevel;            // Membrane noise level
    float thermalNoise;          // Thermal noise (mV/sqrt(ms))
    
    // Plasticity state
    float eligibilityTrace;      // For reward-modulated plasticity
    float dopamineModulation;    // Neuromodulatory influence
    
    // Homeostatic mechanisms
    float homeostaticSetpoint;   // Target firing rate
    float homeostaticStrength;  // Homeostatic plasticity strength
    float synapticScaling;       // Activity-dependent scaling
    
    // Random number generator for this neuron
    std::mt19937_64 rng;
    uint32_t randomSeed;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f),
             inputResistance(100.0f), membraneTimeConstant(TIME_CONSTANT),
             thresholdDynamics(0.0f), adaptationStrength(1.0f),
             noiseLevel(0.5f), thermalNoise(2.0f),
             eligibilityTrace(0.0f), dopamineModulation(0.0f),
             homeostaticSetpoint(-65.0f), homeostaticStrength(0.001f),
             synapticScaling(1.0f), randomSeed(0) {
        std::random_device rd;
        rng.seed(randomSeed);
    }
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
}

Neuron::~Neuron() = default;

Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void Neuron::setType(NeuronType type) {
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
}

void Neuron::decrementRefractory() {
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->state.resetPotential = potential;
}

bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
    
    // Update eligibility trace for reward-modulated plasticity
    if (pImpl->plasticityFlags.reward_modulated) {
        pImpl->eligibilityTrace += amplitude * 0.01f;
    }
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
    
    // Update eligibility trace for reward-modulated plasticity
    if (pImpl->plasticityFlags.reward_modulated) {
        pImpl->eligibilityTrace -= amplitude * 0.01f;
    }
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
    pImpl->dopamineModulation += amplitude * 0.05f;
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
    
    // Update eligibility trace for reward-modulated plasticity
    if (pImpl->plasticityFlags.reward_modulated) {
        pImpl->eligibilityTrace += current * 0.02f;
    }
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->outgoingSynapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    pImpl->regionId = region;
}

void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Enhanced LIF dynamics with thermal noise
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    
    // Add thermal noise to membrane potential (biologically realistic)
    float noise = pImpl->thermalNoise * std::sqrt(static_cast<float>(dt)) * 
                  (pImpl->rng.uniformReal(-1.0f, 1.0f));
    V += noise;
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C + noise
    // Discrete approximation with adaptive time constant
    
    float tau = pImpl->membraneTimeConstant;  // Adaptive time constant
    float C = pImpl->inputResistance;  // Input resistance in Mohms
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution with adaptation
    float leakContribution = (V_rest - V) / tau;
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        float adaptationCurrent = pImpl->state.adaptationVariable * pImpl->adaptationStrength;
        leakContribution -= adaptationCurrent;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply homeostatic plasticity (activity-dependent scaling)
    float currentFiringRate = getFiringRate();
    float homeostaticError = pImpl->homeostaticSetpoint - currentFiringRate;
    if (std::abs(homeostaticError) > 0.1f) {
        float scaling = 1.0f + pImpl->homeostaticStrength * homeostaticError * dt;
        V *= scaling;
        pImpl->synapticScaling *= scaling;
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Dynamic threshold adjustment based on neuromodulation
    float thresholdAdjustment = pImpl->dopamineModulation * 5.0f;
    float effectiveThreshold = threshold + thresholdAdjustment;
    
    // Check for spike with dynamic threshold
    if (V >= effectiveThreshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period with adaptive duration
        uint32_t refractoryDuration = pImpl->state.refractoryPeriod;
        // Modify refractory period based on neuromodulation
        if (pImpl->dopamineModulation > 0.0f) {
            refractoryDuration = static_cast<uint32_t>(refractoryDuration * 0.8f);  // Shorter with positive dopamine
        } else if (pImpl->dopamineModulation < 0.0f) {
            refractoryDuration = static_cast<uint32_t>(refractoryDuration * 1.2f);  // Longer with negative dopamine
        }
        
        pImpl->state.refractoryRemaining = refractoryDuration;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        pImpl->state.adaptationVariable += 1.0f * pImpl->adaptationStrength;
        
        // Update eligibility trace for reward-modulated plasticity
        if (pImpl->plasticityFlags.reward_modulated) {
            pImpl->eligibilityTrace *= 0.5f;  // Decay eligibility trace after spike
        }
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Enhanced LIF step with adaptive timestep based on neuron state
    TimestepDuration dt = 0.001;  // 1ms default
    
    // Adjust timestep based on neuron dynamics
    if (pImpl->state.adaptationVariable > 5.0f) {
        dt = 0.0005f;  // Slower for high adaptation
    } else if (pImpl->state.adaptationVariable < 0.5f) {
        dt = 0.002f;  // Faster for low adaptation
    }
    
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    
    // Reset enhanced parameters
    pImpl->thresholdDynamics = 0.0f;
    pImpl->dopamineModulation = 0.0f;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->synapticScaling = 1.0f;
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Real random initialization with biological constraints
    // Membrane potential starts near resting potential
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold is typically -55mV with small variation
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70mV
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential is usually close to resting
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Refractory period: 2-10ms typical
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    
    // Initialize enhanced biophysical parameters
    pImpl->inputResistance = rng.uniformReal(50.0f, 150.0f);
    pImpl->membraneTimeConstant = rng.uniformReal(10.0f, 30.0f);
    pImpl->adaptationStrength = rng.uniformReal(0.5f, 2.0f);
    pImpl->noiseLevel = rng.uniformReal(0.2f, 1.0f);
    pImpl->thermalNoise = rng.uniformReal(1.0f, 4.0f);
    
    // Set homeostatic parameters
    pImpl->homeostaticSetpoint = -65.0f + rng.uniformReal(-5.0f, 5.0f);
    pImpl->homeostaticStrength = rng.uniformReal(0.0005f, 0.002f);
    pImpl->synapticScaling = 1.0f;
}

} // namespace nlm