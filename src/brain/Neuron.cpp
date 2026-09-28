#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <limits>

namespace nlm {

struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    
    // LIF parameters (configurable)
    float membraneCapacitance;  // nF, typically 0.1-1.0
    float timeConstant;          // ms, typically 10-30
    float leakConductance;       // nS
    
    // Adaptation variables
    float adaptationStrength;      // Spike-frequency adaptation strength
    float adaptationTimeConstant;  // Adaptation decay time constant
    float adaptationVariable;      // Current adaptation level
    
    // Synaptic dynamics
    float synapticConductance;     // Current synaptic conductance
    float reversalPotentialExc;    // Excitatory reversal potential
    float reversalPotentialInh;    // Inhibitory reversal potential
    
    // Recent inputs
    MembranePotential totalCurrent;  // Total synaptic current input this step
    MembranePotential synapticInput;  // Accumulated synaptic input
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // Structural growth
    float dendriteLength;
    float spineDensity;
    bool hasReceptors;
    
    // Constraints and regularization
    float membraneMinVoltage;
    float membraneMaxVoltage;
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             membraneCapacitance(0.5f), timeConstant(20.0f), leakConductance(10.0f),
             adaptationStrength(0.05f), adaptationTimeConstant(50.0f), adaptationVariable(0.0f),
             synapticConductance(0.0f), reversalPotentialExc(0.0f), reversalPotentialInh(-80.0f),
             totalCurrent(0.0f), synapticInput(0.0f),
             dendriteLength(100.0f), spineDensity(1.0f), hasReceptors(false),
             membraneMinVoltage(-100.0f), membraneMaxVoltage(50.0f) {}
};

Neuron::Neuron(NeuronId id, NeuronType type) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = type;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
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

// Identity and metadata
NeuronId Neuron::getId() const {
    return pImpl->id;
}

NeuronType Neuron::getType() const {
    return pImpl->type;
}

void Neuron::setType(NeuronType type) {
    pImpl->type = type;
}

// Region/population membership
void Neuron::setRegionId(RegionId region) {
    pImpl->regionId = region;
}

RegionId Neuron::getRegionId() const {
    return pImpl->regionId;
}

void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

PopulationId Neuron::getPopulationId() const {
    return pImpl->populationId;
}

// ===== LIF DYNAMICS =====
// Configure neuron biophysical parameters
void Neuron::setMembraneCapacitance(float capacitance) {
    if (capacitance <= 0.0f) {
        throw std::invalid_argument("Membrane capacitance must be positive");
    }
    pImpl->membraneCapacitance = capacitance;
}

float Neuron::getMembraneCapacitance() const {
    return pImpl->membraneCapacitance;
}

void Neuron::setTimeConstant(float timeConstant) {
    if (timeConstant <= 0.0f) {
        throw std::invalid_argument("Time constant must be positive");
    }
    pImpl->timeConstant = timeConstant;
}

float Neuron::getTimeConstant() const {
    return pImpl->timeConstant;
}

void Neuron::setLeakConductance(float conductance) {
    pImpl->leakConductance = conductance;
}

float Neuron::getLeakConductance() const {
    return pImpl->leakConductance;
}

void Neuron::setRestingPotential(float potential) {
    pImpl->state.restingPotential = potential;
}

float Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setThreshold(float threshold) {
    pImpl->state.threshold = threshold;
}

float Neuron::getThreshold() const {
    return pImpl->state.threshold;
}

void Neuron::setResetPotential(float potential) {
    pImpl->state.resetPotential = potential;
}

float Neuron::getResetPotential() const {
    return pImpl->state.resetPotential;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (steps == 0) {
        throw std::invalid_argument("Refractory period must be positive");
    }
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setAdaptationStrength(float strength) {
    if (strength < 0.0f) {
        throw std::invalid_argument("Adaptation strength must be non-negative");
    }
    pImpl->adaptationStrength = strength;
}

float Neuron::getAdaptationStrength() const {
    return pImpl->adaptationStrength;
}

void Neuron::setAdaptationTimeConstant(float tau) {
    if (tau <= 0.0f) {
        throw std::invalid_argument("Adaptation time constant must be positive");
    }
    pImpl->adaptationTimeConstant = tau;
}

float Neuron::getAdaptationTimeConstant() const {
    return pImpl->adaptationTimeConstant;
}

void Neuron::setReversalPotentialExc(float potential) {
    pImpl->reversalPotentialExc = potential;
}

void Neuron::setReversalPotentialInh(float potential) {
    pImpl->reversalPotentialInh = potential;
}

// ===== STATE MANAGEMENT =====
const NeuronState& Neuron::getState() const {
    return pImpl->state;
}

NeuronState& Neuron::getState() {
    return pImpl->state;
}

float Neuron::getMembranePotential() const {
    return pImpl->state.membranePotential;
}

void Neuron::setMembranePotential(float potential) {
    if (potential < pImpl->membraneMinVoltage || potential > pImpl->membraneMaxVoltage) {
        throw std::out_of_range("Membrane potential outside valid range");
    }
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(float delta) {
    float newPotential = pImpl->state.membranePotential + delta;
    if (newPotential < pImpl->membraneMinVoltage || newPotential > pImpl->membraneMaxVoltage) {
        throw std::out_of_range("Membrane potential would go outside valid range");
    }
    pImpl->state.membranePotential = newPotential;
}

// ===== Firing and Spiking =====
bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

bool Neuron::isFiring() const {
    return pImpl->state.firingState == FiringState::Active;
}

bool Neuron::isRefractory() const {
    return pImpl->state.refractoryRemaining > 0;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

// Process one LIF integration step with conductance-based synaptic dynamics
bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        pImpl->synapticConductance = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Conductance-based LIF dynamics
    // dV/dt = (gL*(V_rest - V) + g_syn*(E_syn - V)) / C
    // where gL = leak conductance, g_syn = total synaptic conductance
    
    float& V = pImpl->state.membranePotential;
    float V_rest = pImpl->state.restingPotential;
    float V_reset = pImpl->state.resetPotential;
    float threshold = pImpl->state.threshold;
    float C = pImpl->membraneCapacitance;  // nF
    float tau = pImpl->timeConstant;      // ms
    
    // Compute leak current: I_leak = gL * (V - V_rest)  [Note: sign convention]
    float leakCurrent = pImpl->leakConductance * (V - V_rest);
    
    // Compute synaptic current: I_syn = g_syn * (E_syn - V)
    // Where E_syn depends on excitatory/inhibitory balance
    float synapticCurrent = 0.0f;
    if (pImpl->synapticConductance != 0.0f) {
        // Weighted average of reversal potentials based on excitation/inhibition balance
        float excitationFraction = pImpl->synapticInput > 0 ? 1.0f : 0.0f;  // Simplified
        float reversalPotential = excitationFraction * pImpl->reversalPotentialExc + 
                                (1.0f - excitationFraction) * pImpl->reversalPotentialInh;
        synapticCurrent = pImpl->synapticConductance * (reversalPotential - V);
    }
    
    // Total current divided by capacitance gives dV/dt
    float dVdt = (leakCurrent + synapticCurrent) / C;
    
    // Update membrane potential with exponential integration
    V = V + static_cast<float>(dt) * 1000.0f * dVdt;
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->adaptationVariable > 0.0f) {
        V -= pImpl->adaptationStrength * pImpl->adaptationVariable;
        // Decay adaptation
        pImpl->adaptationVariable *= std::exp(-static_cast<float>(dt) * 1000.0f / pImpl->adaptationTimeConstant);
    }
    
    // Clamp membrane potential to prevent instability (biophysical limits)
    V = std::clamp(V, pImpl->membraneMinVoltage, pImpl->membraneMaxVoltage);
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        pImpl->adaptationVariable += pImpl->adaptationStrength;
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic inputs for next step
    pImpl->synapticInput = 0.0f;
    pImpl->synapticConductance = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

// ===== INPUTS AND MODULATION =====
// Synaptic inputs (conductance-based)
void Neuron::receiveExcitatoryInput(float amplitude) {
    if (amplitude < 0.0f) {
        throw std::invalid_argument("Excitatory input must be positive");
    }
    pImpl->synapticConductance += amplitude;
    pImpl->synapticInput += amplitude;  // For backward compatibility and STDP
}

void Neuron::receiveInhibitoryInput(float amplitude) {
    if (amplitude < 0.0f) {
        throw std::invalid_argument("Inhibitory input magnitude must be positive");
    }
    pImpl->synapticConductance += amplitude;
    pImpl->synapticInput -= amplitude;  // For backward compatibility and STDP
}

void Neuron::receiveModulatoryInput(float amplitude) {
    // Neuromodulatory input affects plasticity and adaptation
    pImpl->adaptationVariable += amplitude * 0.1f;
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

// External current injection
void Neuron::injectCurrent(float current) {
    if (std::isnan(current)) {
        throw std::invalid_argument("Current cannot be NaN");
    }
    pImpl->synapticInput += current;
    pImpl->totalCurrent += current;
}

float Neuron::getTotalCurrent() const {
    return pImpl->totalCurrent;
}

void Neuron::clearTotalCurrent() {
    pImpl->totalCurrent = 0.0f;
}

// ===== SPIKE HISTORY =====
void Neuron::recordSpike(Timestamp timestamp) {
    if (timestamp < 0.0f) {
        throw std::invalid_argument("Spike timestamp cannot be negative");
    }
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void Neuron::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
}

size_t Neuron::getRecentSpikeCount(float timeWindow) const {
    if (timeWindow < 0.0f) {
        throw std::invalid_argument("Time window must be non-negative");
    }
    size_t count = 0;
    float currentTime = pImpl->spikeHistory.empty() ? -1.0f : pImpl->spikeHistory.back();
    for (const auto& timestamp : pImpl->spikeHistory) {
        if (currentTime - timestamp <= timeWindow) {
            ++count;
        }
    }
    return count;
}

// ===== SYNAPTIC CONNECTIVITY =====
void Neuron::addIncomingSynapse(SynapseHandle handle) {
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    pImpl->outgoingSynapses.push_back(handle);
}

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    return pImpl->incomingSynapses;
}

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    return pImpl->outgoingSynapses;
}

// ===== PLASTICITY =====
const PlasticityFlags& Neuron::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

// ===== DEVELOPMENT =====
// Initialize neuron with biologically plausible random parameters
void Neuron::initializeRandom(RandomGenerator& rng, bool developmental) {
    // Biologically plausible parameter ranges
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-5.0f, 5.0f);
    
    // Threshold typically -45 to -60 mV
    pImpl->state.threshold = -55.0f + rng.uniformReal(-10.0f, -5.0f);
    
    // Resting potential typically -65 to -75 mV
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-10.0f, -5.0f);
    
    // Reset potential is usually close to resting
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(-5.0f, 5.0f);
    
    // Refractory period: 1-15ms typical
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(1, 15));
    
    // Adaptative strength: 0.01-0.2
    pImpl->adaptationStrength = rng.uniformReal(0.01f, 0.2f);
    
    // Adaptation time constant: 20-100ms
    pImpl->adaptationTimeConstant = rng.uniformReal(20.0f, 100.0f);
    
    // Reversal potentials
    pImpl->reversalPotentialExc = rng.uniformReal(0.0f, 50.0f);   // Excitatory reversal
    pImpl->reversalPotentialInh = rng.uniformReal(-95.0f, -70.0f); // Inhibitory reversal
    
    // Developmental changes
    if (developmental) {
        // During development, neurons are more excitable
        float developmentalScale = 1.0f + rng.uniformReal(0.0f, 0.5f);
        pImpl->state.threshold *= developmentalScale;  // Higher threshold (less excitable)
        pImpl->membraneCapacitance *= developmentalScale;  // Larger capacitance
        pImpl->leakConductance *= developmentalScale;  // Stronger leak
    }
    
    // Structural development
    pImpl->dendriteLength *= (1.0f + rng.uniformReal(0.0f, 0.3f));
    pImpl->spineDensity *= (1.0f + rng.uniformReal(0.0f, 0.5f));
    pImpl->hasReceptors = rng.uniformReal(0.0f, 1.0f) > 0.3f;  // 70% chance of having receptors
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->synapticConductance = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::growDendriteBranch() {
    pImpl->dendriteLength *= 1.5f;
}

void Neuron::addReceptor(float probability) {
    if (rng.uniformReal(0.0f, 1.0f) < probability) {
        pImpl->hasReceptors = true;
    }
}

// Reset to initial state
void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->synapticConductance = 0.0f;
    pImpl->adaptationVariable = 0.0f;
    pImpl->spikeHistory.clear();
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.firingState = FiringState::Resting;
}

// ===== STATISTICS =====
float Neuron::getFiringRate() const {
    // Estimate firing rate from spike history
    if (pImpl->spikeHistory.size() < 2) {
        return 0.0f;
    }
    float totalTime = pImpl->spikeHistory.back() - pImpl->spikeHistory.front();
    if (totalTime <= 0.0f) {
        return 0.0f;
    }
    float spikeCount = static_cast<float>(pImpl->spikeHistory.size());
    return (spikeCount / totalTime) * 1000.0f;  // Convert to Hz
}

void Neuron::setFiringRate(float rate) {
    if (rate < 0.0f) {
        throw std::invalid_argument("Firing rate cannot be negative");
    }
    pImpl->state.firingRate = rate;
}

// ===== DEBUG AND MONITORING =====
void Neuron::logState() const {
    std::cout << "Neuron " << pImpl->id << ": "
              << "V=" << pImpl->state.membranePotential << "mV, "
              << "threshold=" << pImpl->state.threshold << "mV, "
              << "refractory=" << pImpl->state.refractoryRemaining << "/" 
              << pImpl->state.refractoryPeriod << "ms, "
              << "spikes=" << pImpl->spikeHistory.size() << ", "
              << "adaptation=" << pImpl->adaptationVariable << ", "
              << "C=" << pImpl->membraneCapacitance << "nF"
              << std::endl;
}

bool Neuron::validateParameters() const {
    // Check for biologically plausible ranges
    if (pImpl->state.threshold <= pImpl->state.restingPotential) {
        return false;  // Threshold should be higher than resting potential
    }
    if (pImpl->state.resetPotential < pImpl->state.restingPotential - 10.0f) {
        return false;  // Reset should not be too hyperpolarized
    }
    if (pImpl->membraneCapacitance <= 0.0f || pImpl->membraneCapacitance > 10.0f) {
        return false;  // Unreasonable capacitance
    }
    if (pImpl->timeConstant <= 0.0f || pImpl->timeConstant > 100.0f) {
        return false;  // Unreasonable time constant
    }
    if (pImpl->leakConductance < 0.0f || pImpl->leakConductance > 100.0f) {
        return false;  // Unreasonable leak conductance
    }
    return true;
}

} // namespace nlm
