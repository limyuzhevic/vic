#include "Neuron.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <stdexcept>

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
    
    // Configuration-based parameters
    float membraneCapacitance;      // nF
    float timeConstant;              // ms
    float leakConductance;           // mS
    float adaptationVariable;        // For spike-frequency adaptation
    float adaptationTimeConstant;    // ms
    
    // Validation and safety limits
    static constexpr float MIN_THRESHOLD = -100.0f;
    static constexpr float MAX_THRESHOLD = 50.0f;
    static constexpr float MIN_RESTING_POTENTIAL = -80.0f;
    static constexpr float MAX_RESTING_POTENTIAL = -40.0f;
    static constexpr float MIN_RESET_POTENTIAL = -60.0f;
    static constexpr float MAX_RESET_POTENTIAL = -30.0f;
    static constexpr float MIN_LEAK_CONDUCTANCE = 0.0f;
    static constexpr float MAX_LEAK_CONDUCTANCE = 10.0f;
    static constexpr float MIN_MEMBRANE_CAPACITANCE = 0.1f;
    static constexpr float MAX_MEMBRANE_CAPACITANCE = 10.0f;
    static constexpr float MIN_TIME_CONSTANT = 1.0f;
    static constexpr float MAX_TIME_CONSTANT = 100.0f;
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f),
             membraneCapacitance(1.0f), timeConstant(20.0f),
             leakConductance(0.3f), adaptationVariable(0.0f), adaptationTimeConstant(100.0f) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    
    // Initialize with safe default values
    pImpl->state.membranePotential = -70.0f;
    pImpl->state.restingPotential = -70.0f;
    pImpl->state.threshold = -55.0f;
    pImpl->state.resetPotential = -65.0f;
    pImpl->state.leakConductance = 0.3f;
    pImpl->state.refractoryPeriod = 2;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    pImpl->state.firingRate = 0.0f;
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
    if (potential < -100.0f || potential > 50.0f) {
        throw std::out_of_range("Membrane potential out of valid range: " + std::to_string(potential));
    }
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    MembranePotential newPotential = pImpl->state.membranePotential + delta;
    if (newPotential < -100.0f || newPotential > 50.0f) {
        throw std::out_of_range("Membrane potential would exceed valid range after addition");
    }
    pImpl->state.membranePotential = newPotential;
}

void Neuron::setThreshold(MembranePotential threshold) {
    if (threshold < Impl::MIN_THRESHOLD || threshold > Impl::MAX_THRESHOLD) {
        throw std::out_of_range("Threshold out of valid range: " + std::to_string(threshold));
    }
    if (threshold > pImpl->state.restingPotential) {
        throw std::invalid_argument("Threshold must be below resting potential");
    }
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
    if (rate < 0.0f || rate > 1000.0f) {
        throw std::out_of_range("Firing rate out of valid range: " + std::to_string(rate));
    }
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    if (conductance < Impl::MIN_LEAK_CONDUCTANCE || conductance > Impl::MAX_LEAK_CONDUCTANCE) {
        throw std::out_of_range("Leak conductance out of valid range: " + std::to_string(conductance));
    }
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (steps < 0 || steps > 1000) {
        throw std::out_of_range("Refractory period out of valid range: " + std::to_string(steps));
    }
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    if (potential < Impl::MIN_RESTING_POTENTIAL || potential > Impl::MAX_RESTING_POTENTIAL) {
        throw std::out_of_range("Resting potential out of valid range: " + std::to_string(potential));
    }
    if (potential < pImpl->state.threshold) {
        throw std::invalid_argument("Resting potential must be above threshold");
    }
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    if (potential < Impl::MIN_RESET_POTENTIAL || potential > Impl::MAX_RESET_POTENTIAL) {
        throw std::out_of_range("Reset potential out of valid range: " + std::to_string(potential));
    }
    if (potential > pImpl->state.restingPotential) {
        throw std::invalid_argument("Reset potential must be below resting potential");
    }
    pImpl->state.resetPotential = potential;
}

bool Neuron::checkThreshold() const {
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    if (amplitude < -100.0f || amplitude > 100.0f) {
        throw std::out_of_range("Excitatory input amplitude out of valid range: " + std::to_string(amplitude));
    }
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    if (amplitude < -100.0f || amplitude > 100.0f) {
        throw std::out_of_range("Inhibitory input amplitude out of valid range: " + std::to_string(amplitude));
    }
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    if (amplitude < -10.0f || amplitude > 10.0f) {
        throw std::out_of_range("Modulatory input amplitude out of valid range: " + std::to_string(amplitude));
    }
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    if (current < -100.0f || current > 100.0f) {
        throw std::out_of_range("Injected current out of valid range: " + std::to_string(current));
    }
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    if (timestamp < 0.0f) {
        throw std::invalid_argument("Spike timestamp must be non-negative");
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

void Neuron::configureFromConfig(const Config& config) {
    // Configure neuron parameters from configuration file
    try {
        // Set membrane capacitance
        if (auto capOpt = config.get<float>("neuron_membrane_capacitance")) {
            pImpl->membraneCapacitance = std::clamp(*capOpt, Impl::MIN_MEMBRANE_CAPACITANCE, 
                                                   Impl::MAX_MEMBRANE_CAPACITANCE);
        }
        
        // Set time constant
        if (auto tauOpt = config.get<float>("neuron_time_constant")) {
            pImpl->timeConstant = std::clamp(*tauOpt, Impl::MIN_TIME_CONSTANT, 
                                            Impl::MAX_TIME_CONSTANT);
        }
        
        // Set leak conductance
        if (auto leakOpt = config.get<float>("neuron_leak_conductance")) {
            pImpl->leakConductance = std::clamp(*leakOpt, Impl::MIN_LEAK_CONDUCTANCE, 
                                              Impl::MAX_LEAK_CONDUCTANCE);
        }
        
        // Set adaptation time constant
        if (auto adaptTauOpt = config.get<float>("neuron_adaptation_time_constant")) {
            pImpl->adaptationTimeConstant = std::max(1.0f, *adaptTauOpt);  // Ensure positive
        }
        
        // Log configuration
        NLM_LOG_DEBUG("Neuron " + std::to_string(pImpl->id.index()) + " configured from config file");
        
    } catch (const std::exception& e) {
        NLM_LOG_WARNING("Failed to configure neuron from config: " + std::string(e.what()));
    }
}

void Neuron::initializeRandom(RandomGenerator& rng, const Config* config) {
    try {
        // Real random initialization with biological constraints
        // Membrane potential starts near resting potential with small variation
        if (config) {
            // Get configuration-based ranges
            float membraneRange = config->getOr<float>("neuron_initial_membrane_range", 3.0f);
            float thresholdRange = config->getOr<float>("neuron_initial_threshold_range", 2.0f);
            float restingRange = config->getOr<float>("neuron_initial_resting_range", 2.0f);
            
            pImpl->state.membranePotential = pImpl->state.restingPotential + 
                rng.uniformReal(-membraneRange, membraneRange);
            
            pImpl->state.threshold = -55.0f + rng.uniformReal(-thresholdRange, thresholdRange);
            
            pImpl->state.restingPotential = -70.0f + rng.uniformReal(-restingRange, restingRange);
            
            // Ensure physical constraints
            pImpl->state.threshold = std::clamp(pImpl->state.threshold, -100.0f, -40.0f);
            pImpl->state.restingPotential = std::clamp(pImpl->state.restingPotential, -80.0f, -50.0f);
            
            // Reset potential is usually close to resting
            pImpl->state.resetPotential = pImpl->state.restingPotential + 
                rng.uniformReal(0.0f, 5.0f);
            
            // Ensure reset is reasonable
            pImpl->state.resetPotential = std::clamp(pImpl->state.resetPotential, 
                                                   pImpl->state.restingPotential - 20.0f, 
                                                   pImpl->state.restingPotential + 10.0f);
        } else {
            // Fallback to original random initialization
            pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
            
            // Threshold is typically -55mV with small variation
            pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
            
            // Resting potential typically -70mV
            pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
            
            // Reset potential is usually close to resting
            pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
        }
        
        // Refractory period: 2-10ms typical, could be made configurable
        pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
        
        // Initial state
        pImpl->state.firingState = FiringState::Resting;
        pImpl->state.refractoryRemaining = 0;
        pImpl->state.adaptationVariable = 0.0f;
        pImpl->state.lastSpikeTime = -1.0f;
        pImpl->state.firingRate = 0.0f;
        
        // Clear any residual state
        pImpl->synapticInput = 0.0f;
        pImpl->spikeHistory.clear();
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize neuron " + std::to_string(pImpl->id.index()) + 
                     ": " + std::string(e.what()));
        throw; // Re-throw as this is a critical initialization failure
    }
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    if (dt <= 0.0 || dt > 1.0) {
        throw std::invalid_argument("Invalid timestep duration: " + std::to_string(dt));
    }
    
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
    
    float tau = pImpl->timeConstant;  // ms
    float C = pImpl->membraneCapacitance;  // nF
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
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
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    try {
        stepLIF(currentTime, dt);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("LIF step failed for neuron " + std::to_string(pImpl->id.index()) + 
                     ": " + std::string(e.what()));
        throw; // Re-throw as this indicates a critical error
    }
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    pImpl->adaptationVariable = 0.0f;
    pImpl->state.adaptationVariable = 0.0f;
}

} // namespace nlm
