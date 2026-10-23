#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Optimized constants for better performance
struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;
    MembranePotential synapticInput;
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // Performance-optimized LIF parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;     // nF
    static constexpr float TIME_CONSTANT = 20.0f;          // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    static constexpr float MS_PER_S = 1000.0f;             // Optimization constant
    static constexpr float ADAPTATION_COEFF = 0.01f;       // Adaptation coefficient
    static constexpr float ADAPTATION_DECAY = 0.95f;       // Adaptation decay
    static constexpr float MIN_VOLTAGE = -100.0f;          // Minimum voltage bound
    static constexpr float MAX_VOLTAGE = 50.0f;            // Maximum voltage bound
    static constexpr float MAX_ADAPTATION = 10.0f;        // Maximum adaptation
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {}
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
    
    // Handle refractory period with early exit
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        pImpl->synapticInput = 0.0f;  // Clear during refractory period
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Optimized LIF dynamics computation with local variables
    MembranePotential& V = pImpl->state.membranePotential;
    const MembranePotential V_rest = pImpl->state.restingPotential;
    const MembranePotential V_reset = pImpl->state.resetPotential;
    const MembranePotential threshold = pImpl->state.threshold;
    const float tau = Impl::TIME_CONSTANT;
    const float C = Impl::MEMBRANE_CAPACITANCE;
    
    // Pre-compute time step in milliseconds for optimization
    const float dt_ms = static_cast<float>(dt) * Impl::MS_PER_S;
    
    // Compute contributions with combined operations for better ILP
    const float synapticContribution = pImpl->synapticInput / C;
    const float leakContribution = (V_rest - V) / tau;
    
    // Single update with optimized arithmetic
    V += dt_ms * (leakContribution + synapticContribution);
    
    // Optimized spike-frequency adaptation
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * Impl::ADAPTATION_COEFF;
        pImpl->state.adaptationVariable *= Impl::ADAPTATION_DECAY;
    }
    
    // Optimized bounds checking
    if (V < Impl::MIN_VOLTAGE) {
        V = Impl::MIN_VOLTAGE;
    } else if (V > Impl::MAX_VOLTAGE) {
        V = Impl::MAX_VOLTAGE;
    }
    
    // Spike detection with early threshold check
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
        
        // Update adaptation variable
        pImpl->state.adaptationVariable += 1.0f;
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Use optimized default timestep
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Optimized random initialization with biological constraints
    pImpl->state.membranePotential = pImpl->state.restingPotential + 
                                     rng.uniformReal(-3.0f, 3.0f);
    
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    pImpl->state.resetPotential = pImpl->state.restingPotential + 
                                 rng.uniformReal(0.0f, 5.0f);
    
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

// Validation and utility methods implementation
NeuronId Neuron::getId() const {
    return pImpl->id;
}

NeuronType Neuron::getType() const {
    return pImpl->type;
}

const NeuronState& Neuron::getState() const {
    return pImpl->state;
}

MembranePotential Neuron::getMembranePotential() const {
    return pImpl->state.membranePotential;
}

MembranePotential Neuron::getThreshold() const {
    return pImpl->state.threshold;
}

bool Neuron::isFiring() const {
    return pImpl->state.firingState == FiringState::Active;
}

bool Neuron::isRefractory() const {
    return pImpl->state.refractoryRemaining > 0;
}

FiringRate Neuron::getFiringRate() const {
    return pImpl->state.firingRate;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

MembranePotential Neuron::getResetPotential() const {
    return pImpl->state.resetPotential;
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    return pImpl->incomingSynapses;
}

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    return pImpl->outgoingSynapses;
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

RegionId Neuron::getRegionId() const {
    return pImpl->regionId;
}

PopulationId Neuron::getPopulationId() const {
    return pImpl->populationId;
}

void Neuron::validate() const {
    // Comprehensive validation with early exit for performance
    const auto& state = pImpl->state;
    
    // Validate membrane potential
    if (!validateMembranePotential(state.membranePotential)) {
        throw NeuronStateException("Invalid membrane potential: " + 
                                  std::to_string(state.membranePotential));
    }
    
    // Validate threshold
    if (!validateThreshold(state.threshold)) {
        throw NeuronStateException("Invalid threshold: " + std::to_string(state.threshold));
    }
    
    // Validate resting potential
    if (!validateMembranePotential(state.restingPotential)) {
        throw NeuronStateException("Invalid resting potential: " + 
                                  std::to_string(state.restingPotential));
    }
    
    // Validate refractory period
    if (!validateRefractoryPeriod(state.refractoryPeriod)) {
        throw NeuronStateException("Invalid refractory period: " + 
                                  std::to_string(state.refractoryPeriod));
    }
    
    // Validate adaptation variable
    if (std::abs(state.adaptationVariable) > Impl::MAX_ADAPTATION) {
        throw NeuronStateException("Adaptation variable out of range: " + 
                                  std::to_string(state.adaptationVariable));
    }
}

std::string Neuron::getStateSummary() const {
    std::stringstream ss;
    ss << "Neuron ID: " << pImpl->id << "\n";
    ss << "Type: " << static_cast<int>(pImpl->type) << "\n";
    ss << "Membrane Potential: " << pImpl->state.membranePotential << " mV\n";
    ss << "Threshold: " << pImpl->state.threshold << " mV\n";
    ss << "Firing State: " << static_cast<int>(pImpl->state.firingState) << "\n";
    ss << "Refractory: " << pImpl->state.refractoryRemaining << "/" 
       << pImpl->state.refractoryPeriod << "\n";
    ss << "Spike Count: " << pImpl->spikeHistory.size() << "\n";
    ss << "Last Spike: " << pImpl->state.lastSpikeTime << "s\n";
    
    return ss.str();
}

// Validation helper methods
bool Neuron::validateMembranePotential(MembranePotential potential) {
    return potential >= Impl::MIN_VOLTAGE && potential <= Impl::MAX_VOLTAGE;
}

bool Neuron::validateThreshold(MembranePotential threshold) {
    return threshold >= -60.0f && threshold <= -40.0f;
}

bool Neuron::validateRefractoryPeriod(uint32_t steps) {
    return steps <= 1000;
}

bool Neuron::validateInputAmplitude(MembranePotential amplitude) {
    return std::isfinite(amplitude);
}

bool Neuron::validateTimestamp(Timestamp timestamp) {
    return timestamp >= 0.0;
}

std::string Neuron::getValidationErrorMessage(const std::string& field, 
                                               const std::string& value,
                                               const std::string& constraint) {
    return "Validation failed for field '" + field + "': value " + 
           value + " does not satisfy constraint " + constraint;
}

} // namespace nlm
