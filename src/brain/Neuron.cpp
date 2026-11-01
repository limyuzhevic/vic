#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>
#include <cassert>

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
    
    // Enhanced parameters for better control
    static constexpr float MAX_MEMBRANE_POTENTIAL = 50.0f;
    static constexpr float MIN_MEMBRANE_POTENTIAL = -100.0f;
    static constexpr float MAX_THRESHOLD = 10.0f;
    static constexpr float MIN_THRESHOLD = -40.0f;
    static constexpr float MAX_REFACTORY_PERIOD = 50;
    static constexpr float MIN_REFACTORY_PERIOD = 1;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f) {
        // Initialize LIF parameters with biological constraints
        state.membranePotential = -70.0f;
        state.restingPotential = -70.0f;
        state.threshold = -55.0f;
        state.resetPotential = -70.0f;
        state.leakConductance = 10.0f;
        state.synapseConductance = 0.0f;
        state.firingRate = 0.0f;
        state.firingState = FiringState::Resting;
        state.refractoryRemaining = 0;
        state.refractoryPeriod = 9;  // Biological refractory period
        state.adaptationVariable = 0.0f;
        state.lastSpikeTime = -1.0f;
    }
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    if (id == INVALID_NEURON_ID) {
        throw std::invalid_argument("Neuron cannot have invalid ID");
    }
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
    if (type == NeuronType::Unknown) {
        NLM_LOG_WARNING("Attempting to set neuron type to Unknown, using Internal instead");
        type = NeuronType::Internal;
    }
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    // Clamp to biological limits
    pImpl->state.membranePotential = std::clamp(potential, 
                                               Impl::MIN_MEMBRANE_POTENTIAL,
                                               Impl::MAX_MEMBRANE_POTENTIAL);
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membranePotential += delta;
    // Clamp after addition to prevent overflow
    pImpl->state.membranePotential = std::clamp(pImpl->state.membranePotential,
                                               Impl::MIN_MEMBRANE_POTENTIAL,
                                               Impl::MAX_MEMBRANE_POTENTIAL);
}

void Neuron::setThreshold(MembranePotential threshold) {
    // Clamp threshold to biological limits
    pImpl->state.threshold = std::clamp(threshold,
                                        Impl::MIN_THRESHOLD,
                                        Impl::MAX_THRESHOLD);
    
    // Ensure threshold is always above resting potential
    if (pImpl->state.threshold <= pImpl->state.restingPotential) {
        pImpl->state.threshold = pImpl->state.restingPotential + 15.0f;
    }
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
    pImpl->state.firingRate = std::max(0.0f, rate);  // Ensure non-negative
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->state.leakConductance = std::max(0.0f, conductance);
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    // Clamp to biological limits
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(
        std::clamp(static_cast<float>(steps),
                  static_cast<float>(Impl::MIN_REFACTORY_PERIOD),
                  static_cast<float>(Impl::MAX_REFACTORY_PERIOD)));
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    pImpl->state.restingPotential = std::clamp(potential,
                                              Impl::MIN_MEMBRANE_POTENTIAL,
                                              Impl::MAX_MEMBRANE_POTENTIAL);
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    pImpl->state.resetPotential = std::clamp(potential,
                                            Impl::MIN_MEMBRANE_POTENTIAL,
                                            Impl::MAX_MEMBRANE_POTENTIAL);
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
    if (timestamp < 0.0f) {
        throw std::invalid_argument("Spike timestamp cannot be negative");
    }
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    if (handle == INVALID_SYNAPSE_HANDLE) {
        throw std::invalid_argument("Synapse handle cannot be invalid");
    }
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    if (handle == INVALID_SYNAPSE_HANDLE) {
        throw std::invalid_argument("Synapse handle cannot be invalid");
    }
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
    
    // Handle refractory period with improved logic
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        // Update firing rate even during refractory period for statistics
        pImpl->state.firingRate *= 0.9f;
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
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (enhanced with better dynamics)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, Impl::MIN_MEMBRANE_POTENTIAL, Impl::MAX_MEMBRANE_POTENTIAL);
    
    // Update the firing-rate metric from counted spikes, in Hz.
    // Previously setFiringRate() was never called anywhere, so
    // getAverageFiringRate() summed zeros even with thousands of spikes.
    // Enhanced EMA per brain step: a spike contributes 1/dt Hz, otherwise decay.
    if (dt > 0.0) {
        pImpl->state.firingRate = 0.9f * pImpl->state.firingRate;
    }
    
    // Check for spike with improved detection
    if (V >= threshold) {
        fired = true;
        if (dt > 0.0) {
            pImpl->state.firingRate += 0.1f * static_cast<float>(1.0 / dt);
        }
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike with error checking
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
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Enhanced LIF step with better timestep handling
    TimestepDuration dt = 0.001;  // 1ms default
    
    // Validate timestep
    if (dt <= 0.0 || dt > 0.1) {  // Reasonable bounds
        NLM_LOG_WARNING("Unusual timestep detected: " + std::to_string(dt) + ", using default 0.001");
        dt = 0.001;
    }
    
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    // Reset all state to initial values
    pImpl->state = NeuronState();  // Uses default constructor
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Enhanced random initialization with biological constraints and better sampling
    try {
        // Membrane potential starts near resting potential with small variation
        pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
        
        // Threshold is typically -55mV with small variation, but ensure biological bounds
        float thresholdRange = rng.uniformReal(-2.0f, 2.0f);
        pImpl->state.threshold = -55.0f + thresholdRange;
        
        // Ensure threshold is within biological bounds and above resting potential
        pImpl->state.threshold = std::clamp(pImpl->state.threshold,
                                           -50.0f, -30.0f);
        
        // Resting potential typically -70mV with small variation
        pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
        
        // Reset potential is usually close to resting
        pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
        
        // Refractory period: 2-10ms typical
        pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(uint32_t(2), uint32_t(10)));
        
        // Ensure minimum refractory period for stability
        if (pImpl->state.refractoryPeriod < 2) pImpl->state.refractoryPeriod = 2;
        
        // Initial state
        pImpl->state.firingState = FiringState::Resting;
        pImpl->state.refractoryRemaining = 0;
        pImpl->state.adaptationVariable = 0.0f;
        pImpl->state.lastSpikeTime = -1.0f;
        
        // Clear any residual state
        pImpl->synapticInput = 0.0f;
        pImpl->spikeHistory.clear();
        
        // Log successful initialization
        NLM_LOG_DEBUG("Neuron " + std::to_string(pImpl->id) + " initialized with type " +
                     std::to_string(static_cast<int>(pImpl->type)) +
                     " V_th=" + std::to_string(pImpl->state.threshold) +
                     " V_rest=" + std::to_string(pImpl->state.restingPotential) +
                     " V_reset=" + std::to_string(pImpl->state.resetPotential));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize neuron " + std::to_string(pImpl->id) +
                     ": " + std::string(e.what()));
        throw;
    }
}

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

MembranePotential Neuron::getTotalCurrent() const { 
    return pImpl->totalCurrent; 
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

} // namespace nlm
