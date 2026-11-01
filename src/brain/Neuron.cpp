#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <chrono>

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
    
    // Enhanced LIF parameters with better defaults
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;      // ms
    static constexpr float MEMBRANE_DECAY_RATE = 0.05f; // per ms for better stability
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    static constexpr float ADAPTATION_DECAY_RATE = 0.95f;
    static constexpr float ADAPTATION_GAIN = 0.01f;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {
        // Initialize with more robust default values
        initializeDefaults();
    }
    
    void initializeDefaults() {
        state.membranePotential = -70.0f;
        state.restingPotential = -70.0f;
        state.threshold = -55.0f;
        state.resetPotential = -70.0f;
        state.leakConductance = 10.0f;
        state.synapseConductance = 0.0f;
        state.firingRate = 0.0f;
        state.firingState = FiringState::Resting;
        state.refractoryRemaining = 0;
        state.refractoryPeriod = 5;
        state.adaptationVariable = 0.0f;
        state.lastSpikeTime = -1.0f;
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

Neuron::Neuron(Neuron&& other) noexcept : pImpl(std::exchange(other.pImpl, nullptr)) {}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = std::exchange(other.pImpl, nullptr);
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
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
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
    
    // Handle refractory period with optimized logic
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        pImpl->synapticInput = 0.0f;  // Block input during refractory
        
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        
        // Maintain firing rate decay for activity tracking
        pImpl->state.firingRate *= 0.9f;
        return false;
    }
    
    // Enhanced LIF dynamics with better numerical stability
    MembranePotential& V = pImpl->state.membranePotential;
    const MembranePotential V_rest = pImpl->state.restingPotential;
    const MembranePotential V_reset = pImpl->state.resetPotential;
    const MembranePotential threshold = pImpl->state.threshold;
    
    const float tau = Impl::TIME_CONSTANT;
    const float C = Impl::MEMBRANE_CAPACITANCE;
    
    // Convert dt from seconds to milliseconds for consistency with tau (ms)
    const float dt_ms = static_cast<float>(dt) * 1000.0f;
    
    // Calculate contributions with optimized arithmetic
    const float synapticContribution = pImpl->synapticInput / C;
    const float leakContribution = (V_rest - V) / tau;
    
    // Apply exponential Euler integration with improved stability
    V += dt_ms * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation with better clamping
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * Impl::ADAPTATION_GAIN;
        pImpl->state.adaptationVariable *= Impl::ADAPTATION_DECAY_RATE;
    }
    
    // Enhanced clamping to prevent instability with safety margins
    const float minVoltage = -100.0f;
    const float maxVoltage = 50.0f;
    V = std::clamp(V, minVoltage, maxVoltage);
    
    // Update firing rate with more accurate exponential moving average
    if (dt > 0.0) {
        const float dt_inv = 1.0f / static_cast<float>(dt);
        pImpl->state.firingRate = 0.9f * pImpl->state.firingRate;
        
        // Check for spike
        if (V >= threshold) {
            fired = true;
            pImpl->state.firingRate += 0.1f * dt_inv;
            pImpl->state.firingState = FiringState::Active;
            pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
            
            // Record spike with bounded history
            recordSpike(currentTime);
            
            // Reset membrane potential
            V = V_reset;
            
            // Enter refractory period
            pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
            pImpl->state.firingState = FiringState::Refractory;
            
            // Update adaptation variable for next spikes
            pImpl->state.adaptationVariable += 1.0f;
        } else {
            pImpl->state.firingState = FiringState::Active;
        }
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Use standard timestep but allow for variable timestep in future
    TimestepDuration dt = 0.001;  // 1ms standard timestep
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    // Use move assignment to efficiently reset state
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    // Note: We keep type, regionId, and populationId as they are persistent
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Enhanced random initialization with biological constraints
    
    // Membrane potential starts near resting potential with physiological bounds
    pImpl->state.membranePotential = pImpl->state.restingPotential + 
                                     rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold typically -55mV with smaller, more realistic variation
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70mV with biological variation
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential close to resting (typical for LIF neurons)
    pImpl->state.resetPotential = pImpl->state.restingPotential + 
                                   rng.uniformReal(0.0f, 5.0f);
    
    // Refractory period: 2-10ms typical for biological neurons
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(
        rng.uniformInt(uint32_t(2), uint32_t(10)));
    
    // Initial state management
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

// Additional optimized methods for better performance

void Neuron::batchInjectCurrent(const std::vector<std::pair<NeuronId, MembranePotential>>& injections) {
    // Optimized batch injection for multiple currents
    for (const auto& [neuronId, current] : injections) {
        if (neuronId == pImpl->id) {
            pImpl->synapticInput += current;
        }
    }
}

float Neuron::getAverageFiringRate(size_t timeWindow) const {
    if (pImpl->spikeHistory.size() < 2) {
        return pImpl->state.firingRate;
    }
    
    // Calculate average firing rate from spike history
    float totalTime = pImpl->spikeHistory.back() - pImpl->spikeHistory.front();
    if (totalTime <= 0.0f) {
        return pImpl->state.firingRate;
    }
    
    float spikeCount = static_cast<float>(pImpl->spikeHistory.size());
    return spikeCount / totalTime; // spikes per second
}

bool Neuron::hasRecentSpike(float timeThreshold) const {
    if (pImpl->spikeHistory.empty()) {
        return false;
    }
    
    return (pImpl->state.lastSpikeTime >= 0.0f && 
            (static_cast<float>(std::chrono::system_clock::now().time_since_epoch().count()) - pImpl->state.lastSpikeTime) < timeThreshold);
}

void Neuron::scaleParameters(float factor) {
    // Scale neuron parameters (for developmental changes)
    pImpl->state.restingPotential *= factor;
    pImpl->state.threshold *= factor;
    pImpl->state.resetPotential *= factor;
    pImpl->state.leakConductance *= factor;
    pImpl->state.adaptationVariable *= factor;
}

NeuronId Neuron::getId() const { return pImpl->id; }

NeuronType Neuron::getType() const { return pImpl->type; }

const NeuronState& Neuron::getState() const { return pImpl->state; }

MembranePotential Neuron::getMembranePotential() const { return pImpl->state.membranePotential; }

MembranePotential Neuron::getThreshold() const { return pImpl->state.threshold; }

bool Neuron::isFiring() const { return pImpl->state.firingState == FiringState::Active; }

bool Neuron::isRefractory() const { return pImpl->state.refractoryRemaining > 0; }

FiringRate Neuron::getFiringRate() const { return pImpl->state.firingRate; }

MembranePotential Neuron::getTotalCurrent() const { return pImpl->totalCurrent; }

const std::vector<Timestamp>& Neuron::getSpikeHistory() const { return pImpl->spikeHistory; }

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const { return pImpl->incomingSynapses; }

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const { return pImpl->outgoingSynapses; }

const PlasticityFlags& Neuron::getPlasticityFlags() const { return pImpl->plasticityFlags; }

RegionId Neuron::getRegionId() const { return pImpl->regionId; }

PopulationId Neuron::getPopulationId() const { return pImpl->populationId; }

} // namespace nlm

