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
    MembranePotential totalCurrent;  // Total synaptic current input this step
    MembranePotential synapticInput;  // Accumulated synaptic input
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // LIF parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    // Validate neuron ID
    if (id.index() == 0) { // Invalid ID
        Logger::getGlobal().error("Neuron::Neuron: Invalid neuron ID provided");
        // Create with default invalid ID (implementation defined)
    }
    
    try {
        pImpl->id = id;
        pImpl->type = NeuronType::Internal;
        pImpl->regionId = INVALID_REGION_ID;
        pImpl->populationId = INVALID_POPULATION_ID;
        pImpl->totalCurrent = 0.0f;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::Neuron: Exception during initialization: " + std::string(e.what()));
        delete pImpl;
        pImpl = nullptr;
        throw; // Re-throw if initialization fails
    }
}

Neuron::~Neuron() = default;

Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    try {
        other.pImpl = nullptr;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::Neuron(move): Exception during move constructor: " + std::string(e.what()));
        // Restore state if move fails
        pImpl = other.pImpl;
        other.pImpl = nullptr;
        throw;
    }
}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    try {
        if (this != &other) {
            delete pImpl;
            pImpl = other.pImpl;
            other.pImpl = nullptr;
        }
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::operator=(move): Exception during move assignment: " + std::string(e.what()));
        // Restore original state
        std::swap(pImpl, other.pImpl);
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

void Neuron::setType(NeuronType type) {
    try {
        // Validate neuron type
        if (static_cast<int>(type) < 0 || static_cast<int>(type) > 4) {
            Logger::getGlobal().error("Neuron::setType: Invalid neuron type: " + std::to_string(static_cast<int>(type)));
            return;
        }
        
        pImpl->type = type;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::setType: Exception setting neuron type: " + std::string(e.what()));
    }
}

void Neuron::setMembranePotential(MembranePotential potential) {
    try {
        // Validate membrane potential range
        if (potential < -100.0f || potential > 50.0f) {
            Logger::getGlobal().warning("Neuron::setMembranePotential: Membrane potential out of biological range: " + std::to_string(potential));
            // Clamp to reasonable range instead of rejecting
            const_cast<MembranePotential&>(pImpl->state.membranePotential) = std::clamp(potential, -100.0f, 50.0f);
            return;
        }
        
        pImpl->state.membranePotential = potential;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::setMembranePotential: Exception setting membrane potential: " + std::string(e.what()));
    }
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    try {
        // Validate delta
        if (std::isnan(delta) || std::isinf(delta)) {
            Logger::getGlobal().error("Neuron::addToMembranePotential: Invalid delta value: " + std::to_string(delta));
            return;
        }
        
        const_cast<MembranePotential&>(pImpl->state.membranePotential) += delta;
        
        // Check bounds after addition
        const_cast<MembranePotential&>(pImpl->state.membranePotential) = std::clamp(pImpl->state.membranePotential, -100.0f, 50.0f);
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::addToMembranePotential: Exception adding to membrane potential: " + std::string(e.what()));
    }
}

void Neuron::setThreshold(MembranePotential threshold) {
    try {
        // Validate threshold range
        if (threshold < -80.0f || threshold > 0.0f) {
            Logger::getGlobal().warning("Neuron::setThreshold: Threshold out of typical biological range: " + std::to_string(threshold));
            const_cast<MembranePotential&>(pImpl->state.threshold) = std::clamp(threshold, -80.0f, 0.0f);
            return;
        }
        
        pImpl->state.threshold = threshold;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::setThreshold: Exception setting threshold: " + std::string(e.what()));
    }
}

void Neuron::setFiringState(FiringState state) {
    try {
        // Validate firing state
        if (static_cast<int>(state) < 0 || static_cast<int>(state) > 3) {
            Logger::getGlobal().error("Neuron::setFiringState: Invalid firing state: " + std::to_string(static_cast<int>(state)));
            return;
        }
        
        pImpl->state.firingState = state;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::setFiringState: Exception setting firing state: " + std::string(e.what()));
    }
}

void Neuron::decrementRefractory() {
    try {
        if (pImpl->state.refractoryRemaining > 0) {
            --pImpl->state.refractoryRemaining;
            if (pImpl->state.refractoryRemaining == 0) {
                pImpl->state.firingState = FiringState::Resting;
            }
        }
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::decrementRefractory: Exception decrementing refractory period: " + std::string(e.what()));
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    try {
        // Validate firing rate
        if (rate < 0.0f || rate > 1000.0f) {
            Logger::getGlobal().warning("Neuron::setFiringRate: Firing rate out of range: " + std::to_string(rate));
            const_cast<FiringRate&>(pImpl->state.firingRate) = std::clamp(rate, 0.0f, 1000.0f);
            return;
        }
        
        pImpl->state.firingRate = rate;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::setFiringRate: Exception setting firing rate: " + std::string(e.what()));
    }
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    try {
        // Validate conductance
        if (conductance < 0.0f || conductance > 100.0f) {
            Logger::getGlobal().warning("Neuron::setLeakConductance: Leak conductance out of range: " + std::to_string(conductance));
            const_cast<MembranePotential&>(pImpl->state.leakConductance) = std::clamp(conductance, 0.0f, 100.0f);
            return;
        }
        
        pImpl->state.leakConductance = conductance;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::setLeakConductance: Exception setting leak conductance: " + std::string(e.what()));
    }
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    try {
        // Validate refractory period
        if (steps > 1000) {
            Logger::getGlobal().warning("Neuron::setRefractoryPeriod: Refractory period too long: " + std::to_string(steps));
            const_cast<uint32_t&>(pImpl->state.refractoryPeriod) = 1000;
            return;
        }
        
        pImpl->state.refractoryPeriod = steps;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::setRefractoryPeriod: Exception setting refractory period: " + std::string(e.what()));
    }
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    try {
        // Validate resting potential
        if (potential < -90.0f || potential > -50.0f) {
            Logger::getGlobal().warning("Neuron::setRestingPotential: Resting potential out of range: " + std::to_string(potential));
            const_cast<MembranePotential&>(pImpl->state.restingPotential) = std::clamp(potential, -90.0f, -50.0f);
            return;
        }
        
        pImpl->state.restingPotential = potential;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::setRestingPotential: Exception setting resting potential: " + std::string(e.what()));
    }
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    try {
        // Validate reset potential
        if (potential < -80.0f || potential > 0.0f) {
            Logger::getGlobal().warning("Neuron::setResetPotential: Reset potential out of range: " + std::to_string(potential));
            const_cast<MembranePotential&>(pImpl->state.resetPotential) = std::clamp(potential, -80.0f, 0.0f);
            return;
        }
        
        pImpl->state.resetPotential = potential;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::setResetPotential: Exception setting reset potential: " + std::string(e.what()));
    }
}

bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    try {
        // Validate amplitude
        if (std::isnan(amplitude) || std::isinf(amplitude)) {
            Logger::getGlobal().error("Neuron::receiveExcitatoryInput: Invalid amplitude: " + std::to_string(amplitude));
            return;
        }
        
        if (amplitude < 0.0f) {
            Logger::getGlobal().warning("Neuron::receiveExcitatoryInput: Negative amplitude for excitatory input: " + std::to_string(amplitude));
        }
        
        pImpl->synapticInput += amplitude;
        
        // Check bounds
        if (pImpl->synapticInput > 1000.0f) {
            Logger::getGlobal().warning("Neuron::receiveExcitatoryInput: Synaptic input too high: " + std::to_string(pImpl->synapticInput));
            pImpl->synapticInput = 1000.0f;
        }
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::receiveExcitatoryInput: Exception receiving excitatory input: " + std::string(e.what()));
    }
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    try {
        // Validate amplitude
        if (std::isnan(amplitude) || std::isinf(amplitude)) {
            Logger::getGlobal().error("Neuron::receiveInhibitoryInput: Invalid amplitude: " + std::to_string(amplitude));
            return;
        }
        
        if (amplitude < 0.0f) {
            Logger::getGlobal().warning("Neuron::receiveInhibitoryInput: Negative amplitude for inhibitory input: " + std::to_string(amplitude));
        }
        
        pImpl->synapticInput -= amplitude;
        
        // Check bounds
        if (pImpl->synapticInput < -1000.0f) {
            Logger::getGlobal().warning("Neuron::receiveInhibitoryInput: Synaptic input too low: " + std::to_string(pImpl->synapticInput));
            pImpl->synapticInput = -1000.0f;
        }
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::receiveInhibitoryInput: Exception receiving inhibitory input: " + std::string(e.what()));
    }
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    try {
        // Validate amplitude
        if (std::isnan(amplitude) || std::isinf(amplitude)) {
            Logger::getGlobal().error("Neuron::receiveModulatoryInput: Invalid amplitude: " + std::to_string(amplitude));
            return;
        }
        
        pImpl->state.adaptationVariable += amplitude * 0.1f;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::receiveModulatoryInput: Exception receiving modulatory input: " + std::string(e.what()));
    }
}

void Neuron::injectCurrent(MembranePotential current) {
    try {
        // Validate current
        if (std::isnan(current) || std::isinf(current)) {
            Logger::getGlobal().error("Neuron::injectCurrent: Invalid current: " + std::to_string(current));
            return;
        }
        
        pImpl->synapticInput += current;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::injectCurrent: Exception injecting current: " + std::string(e.what()));
    }
}

void Neuron::clearTotalCurrent() {
    try {
        pImpl->synapticInput = 0.0f;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::clearTotalCurrent: Exception clearing current: " + std::string(e.what()));
    }
}

void Neuron::recordSpike(Timestamp timestamp) {
    try {
        // Validate timestamp
        if (timestamp < 0) {
            Logger::getGlobal().warning("Neuron::recordSpike: Negative timestamp: " + std::to_string(timestamp));
        }
        
        pImpl->spikeHistory.push_back(timestamp);
        
        // Check bounds and prune old spikes
        if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
            size_t excess = pImpl->spikeHistory.size() - Impl::MAX_SPIKE_HISTORY;
            pImpl->spikeHistory.erase(pImpl->spikeHistory.begin(), 
                                   pImpl->spikeHistory.begin() + excess);
        }
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::recordSpike: Exception recording spike: " + std::string(e.what()));
    }
}

void Neuron::clearSpikeHistory() {
    try {
        pImpl->spikeHistory.clear();
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::clearSpikeHistory: Exception clearing spike history: " + std::string(e.what()));
    }
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    try {
        pImpl->incomingSynapses.push_back(handle);
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::addIncomingSynapse: Exception adding incoming synapse: " + std::string(e.what()));
    }
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    try {
        pImpl->outgoingSynapses.push_back(handle);
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::addOutgoingSynapse: Exception adding outgoing synapse: " + std::string(e.what()));
    }
}

NeuronState& Neuron::getState() {
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    try {
        pImpl->plasticityFlags.hebbian = hebbian;
        pImpl->plasticityFlags.stdp = stdp;
        pImpl->plasticityFlags.reward_modulated = rewardModulated;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::enablePlasticity: Exception enabling plasticity: " + std::string(e.what()));
    }
}

void Neuron::setRegionId(RegionId region) {
    try {
        pImpl->regionId = region;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::setRegionId: Exception setting region ID: " + std::string(e.what()));
    }
}

void Neuron::setPopulationId(PopulationId population) {
    try {
        pImpl->populationId = population;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::setPopulationId: Exception setting population ID: " + std::string(e.what()));
    }
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    try {
        // Validate parameters
        if (dt <= 0.0) {
            Logger::getGlobal().error("Neuron::stepLIF: Invalid timestep: " + std::to_string(dt));
            return false;
        }
        
        // Handle refractory period with error checking
        if (pImpl->state.refractoryRemaining > 0) {
            --pImpl->state.refractoryRemaining;
            if (pImpl->state.refractoryRemaining == 0) {
                pImpl->state.firingState = FiringState::Resting;
            }
            pImpl->synapticInput = 0.0f;
            return false;
        }
        
        // LIF dynamics with bounds checking
        MembranePotential& V = pImpl->state.membranePotential;
        MembranePotential V_rest = pImpl->state.restingPotential;
        MembranePotential V_reset = pImpl->state.resetPotential;
        MembranePotential threshold = pImpl->state.threshold;
        float tau = Impl::TIME_CONSTANT;
        float C = Impl::MEMBRANE_CAPACITANCE;
        
        // Validate state variables
        if (std::isnan(V) || std::isinf(V)) {
            V = V_rest;
            Logger::getGlobal().warning("Neuron::stepLIF: NaN or infinite membrane potential, reset to resting");
        }
        
        // Synaptic input contributes to membrane potential change
        float synapticContribution = pImpl->synapticInput / C;
        
        // Leak contribution
        float leakContribution = (V_rest - V) / tau;
        
        // Update membrane potential
        V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
        
        // Apply spike-frequency adaptation
        if (pImpl->state.adaptationVariable > 0.0f) {
            V -= pImpl->state.adaptationVariable * 0.01f;
            pImpl->state.adaptationVariable *= 0.95f;
        }
        
        // Clamp membrane potential
        V = std::clamp(V, -100.0f, 50.0f);
        
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
            pImpl->state.adaptationVariable += 1.0f;
        } else {
            pImpl->state.firingState = FiringState::Active;
        }
        
        // Clear synaptic input
        pImpl->synapticInput = 0.0f;
        
    } catch (const std::bad_alloc&) {
        Logger::getGlobal().error("Neuron::stepLIF: Memory allocation failed during LIF computation");
        return false;
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::stepLIF: Exception during LIF computation: " + std::string(e.what()));
        return false;
    }
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    try {
        // Default LIF step with standard timestep
        TimestepDuration dt = 0.001;
        stepLIF(currentTime, dt);
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::step: Exception during neuron step: " + std::string(e.what()));
    }
}

void Neuron::reset() {
    try {
        pImpl->state = NeuronState();
        pImpl->synapticInput = 0.0f;
        pImpl->spikeHistory.clear();
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::reset: Exception during neuron reset: " + std::string(e.what()));
    }
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    try {
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
        
    } catch (const std::bad_alloc&) {
        Logger::getGlobal().error("Neuron::initializeRandom: Memory allocation failed during random initialization");
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::initializeRandom: Exception during random initialization: " + std::string(e.what()));
    }
}

} // namespace nlm

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    try {
        pImpl->state = NeuronState();
        pImpl->synapticInput = 0.0f;
        pImpl->spikeHistory.clear();
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::reset: Exception during neuron reset: " + std::string(e.what()));
    }
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    try {
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
        
    } catch (const std::bad_alloc&) {
        Logger::getGlobal().error("Neuron::initializeRandom: Memory allocation failed during random initialization");
    } catch (const std::exception& e) {
        Logger::getGlobal().error("Neuron::initializeRandom: Exception during random initialization: " + std::string(e.what()));
    }
}

} // namespace nlm
