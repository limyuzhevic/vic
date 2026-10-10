#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    float membraneCapacitance;  // nF (configurable)
    float timeConstant;  // ms (configurable)
    size_t maxSpikeHistory;  // maximum spike history size
    float adaptationTimeConstant;  // ms
    uint32_t minPopulationSize;  // minimum size for healthy population
    
    Impl()
        : id(), type(NeuronType::Internal), regionId(), populationId()
        , membraneCapacitance(1.0f), timeConstant(20.0f), maxSpikeHistory(100)
        , adaptationTimeConstant(50.0f), minPopulationSize(5) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->state.restingPotential = -70.0f;
    pImpl->state.threshold = -55.0f;
    pImpl->state.resetPotential = -70.0f;
    pImpl->state.leakConductance = 10.0f;
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
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
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

// Get membrane capacitance (nF)
float Neuron::getMembraneCapacitance() const {
    return pImpl->membraneCapacitance;
}

// Get time constant (ms)
float Neuron::getTimeConstant() const {
    return pImpl->timeConstant;
}

// Get input resistance (MOhm)
float Neuron::getInputResistance() const {
    return pImpl->state.inputResistance;
}

// Get threshold adaptation (mV per spike)
float Neuron::getThresholdAdaptation() const {
    return pImpl->state.thresholdAdaptation;
}

// Get adaptation time constant (ms)
float Neuron::getAdaptationTimeConstant() const {
    return pImpl->adaptationTimeConstant;
}

// Get noise level (mV)
float Neuron::getNoiseLevel() const {
    return pImpl->state.neuralNoiseSigma;
}

// Get refractory adaptation (ms per spike)
float Neuron::getRefractoryAdaptation() const {
    return pImpl->state.refractoryAdaptation;
}

// Get energy efficiency (0.0-1.0)
float Neuron::getEnergyEfficiency() const {
    return pImpl->state.energyEfficiency;
}

// Set membrane capacitance (nF)
void Neuron::setMembraneCapacitance(float capacitance) {
    pImpl->membraneCapacitance = capacitance;
}

// Set time constant (ms)
void Neuron::setTimeConstant(float tau) {
    pImpl->timeConstant = tau;
}

// Set input resistance (MOhm)
void Neuron::setInputResistance(float resistance) {
    pImpl->state.inputResistance = resistance;
}

// Set threshold adaptation (mV per spike)
void Neuron::setThresholdAdaptation(float alpha) {
    pImpl->state.thresholdAdaptation = alpha;
}

// Set adaptation time constant (ms)
void Neuron::setAdaptationTimeConstant(float tau_adapt) {
    pImpl->adaptationTimeConstant = tau_adapt;
}

// Set noise level (mV)
void Neuron::setNoiseLevel(float sigma) {
    pImpl->state.neuralNoiseSigma = sigma;
}

// Set refractory adaptation (ms per spike)
void Neuron::setRefractoryAdaptation(float alpha_refrac) {
    pImpl->state.refractoryAdaptation = alpha_refrac;
}

// Set energy efficiency (0.0-1.0)
void Neuron::setEnergyEfficiency(float efficiency) {
    pImpl->state.energyEfficiency = efficiency;
}

// Check if neuron is firing
bool Neuron::isFiring() const {
    return pImpl->state.firingState == FiringState::Active;
}

// Check if neuron is refractory
bool Neuron::isRefractory() const {
    return pImpl->state.refractoryRemaining > 0;
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
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    float basePotential = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        basePotential -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability (configurable range)
    float minPotential = -90.0f + pImpl->state.thresholdAdaptation;  // Dynamic minimum
    float maxPotential = 60.0f - pImpl->state.thresholdAdaptation;  // Dynamic maximum
    float V_new = std::clamp(basePotential, minPotential, maxPotential);
    
    // Add neural noise for biological realism
    if (pImpl->state.neuralNoiseSigma > 0.0f) {
        float noise = pImpl->state.neuralNoiseSigma * (rand() / (RAND_MAX + 1.0f) * 2.0f - 1.0f);
        V_new += noise;
    }
    
    // Apply synaptic input filtering
    if (pImpl->state.synapticInputFilter > 0.0f) {
        float filterAlpha = std::exp(-dt * 1000.0f / pImpl->state.synapticInputFilter);
        V_new = pImpl->synapticInput * (1.0f - filterAlpha) + V_new * filterAlpha;
    }
    
    V = V_new;
    
    // Check for spike with adaptive threshold
    float currentThreshold = pImpl->state.threshold;
    if (pImpl->state.consecutiveSpikes > 0) {
        currentThreshold += pImpl->state.thresholdAdaptation * pImpl->state.consecutiveSpikes;
    }
    
    if (V >= currentThreshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        pImpl->state.timeSinceLastSpike = 0.0f;
        pImpl->state.consecutiveSpikes++;
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential with refractory adaptation
        float refractoryImpact = pImpl->state.refractoryAdaptation * pImpl->state.consecutiveSpikes;
        pImpl->state.resetPotential = std::clamp(pImpl->state.resetPotential + refractoryImpact, -80.0f, -60.0f);
        V = pImpl->state.resetPotential;
        
        // Enter refractory period with adaptation
        pImpl->state.refractoryRemaining = static_cast<uint32_t>(pImpl->state.refractoryPeriod * (1.0f + refractoryImpact));
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        pImpl->state.adaptationVariable += 1.0f + pImpl->state.consecutiveSpikes * 0.1f;
        
        // Update energy consumption
        pImpl->state.energyConsumption += 1.0f + pImpl->state.consecutiveSpikes * 0.5f;
    } else {
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.consecutiveSpikes = 0;
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    // Update time since last spike
    pImpl->state.timeSinceLastSpike += static_cast<float>(dt) * 1000.0f;
    
    return fired;
}
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
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
}

} // namespace nlm
