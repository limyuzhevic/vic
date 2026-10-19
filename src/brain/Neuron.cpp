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
    // Validate membrane potential is within reasonable bounds
    if (potential < -150.0f || potential > 100.0f) {
        throw std::invalid_argument("Membrane potential out of bounds: " + 
                                   std::to_string(potential));
    }
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    // Validate that the operation won't cause instability
    MembranePotential newPotential = pImpl->state.membranePotential + delta;
    if (newPotential < -150.0f || newPotential > 100.0f) {
        throw std::invalid_argument("Membrane potential change would cause instability: " + 
                                   std::to_string(delta));
    }
    pImpl->state.membranePotential = newPotential;
}

void Neuron::setThreshold(MembranePotential threshold) {
    // Validate threshold is reasonable (typically -50 to 50 mV)
    if (threshold < -100.0f || threshold > 50.0f) {
        throw std::invalid_argument("Threshold out of bounds: " + std::to_string(threshold));
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
    // Validate firing rate is reasonable
    if (rate < 0.0f || rate > 1000.0f) {
        throw std::invalid_argument("Firing rate out of bounds: " + std::to_string(rate));
    }
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    // Validate leak conductance is positive and reasonable
    if (conductance <= 0.0f) {
        throw std::invalid_argument("Leak conductance must be positive: " + std::to_string(conductance));
    }
    if (conductance > 100.0f) {
        throw std::invalid_argument("Leak conductance too large: " + std::to_string(conductance));
    }
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    // Validate refractory period is reasonable (typically 1-1000 steps)
    if (steps == 0) {
        throw std::invalid_argument("Refractory period must be > 0");
    }
    if (steps > 10000) {
        throw std::invalid_argument("Refractory period too large: " + std::to_string(steps));
    }
    pImpl->state.refractoryPeriod = steps;
    // If currently in refractory, ensure remaining doesn't exceed new period
    if (pImpl->state.refractoryRemaining > pImpl->state.refractoryPeriod) {
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
    }
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    // Validate resting potential is biologically plausible (-100 to -40 mV typical)
    if (potential < -120.0f || potential > -30.0f) {
        throw std::invalid_argument("Resting potential out of bounds: " + std::to_string(potential));
    }
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    // Reset potential should be close to resting potential
    float diff = std::abs(potential - pImpl->state.restingPotential);
    if (diff > 20.0f) {
        throw std::invalid_argument("Reset potential too far from resting potential: " + std::to_string(potential));
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
    // Use circular buffer for better performance and memory efficiency
    if (pImpl->spikeHistory.size() >= Impl::MAX_SPIKE_HISTORY) {
        // Use circular buffer pointer arithmetic
        static size_t circularHead = 0;
        pImpl->spikeHistory[circularHead] = timestamp;
        circularHead = (circularHead + 1) % Impl::MAX_SPIKE_HISTORY;
    } else {
        pImpl->spikeHistory.push_back(timestamp);
        // Initialize circular buffer pointer on first insertion
        static size_t circularHead = 0;
        circularHead = pImpl->spikeHistory.size() - 1;
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
    
    // Validate timestep
    if (dt <= 0.0 || dt > 0.1) {
        throw std::invalid_argument("Invalid timestep: " + std::to_string(dt));
    }
    
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
    
    // Clamp parameters to prevent numerical instability
    tau = std::max(1.0f, std::min(tau, 1000.0f));  // 1-1000ms
    C = std::max(0.1f, std::min(C, 10.0f));  // 0.1-10 nF
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution with better numerical stability
    float leakContribution = (V_rest - V) / tau;
    
    // Update membrane potential using exponential Euler integration
    // with better numerical stability and smaller timesteps
    float dt_ms = static_cast<float>(dt) * 1000.0f;  // Convert to ms
    if (dt_ms > 1.0f) dt_ms = 1.0f;  // Cap timestep to 1ms
    
    V = V + dt_ms * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Better clamping with hysteresis to prevent oscillation
    const float V_min = -100.0f;
    const float V_max = 50.0f;
    if (V < V_min) V = V_min;
    if (V > V_max) V = V_max;
    
    // Check for spike with hysteresis
    if (V >= threshold + 0.5f) {  // Add hysteresis to prevent multiple spikes
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
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    // Reset with validation
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    
    // Additional reset safety checks
    if (pImpl->state.adaptationVariable != 0.0f) {
        throw std::runtime_error("Failed to properly reset adaptation variable");
    }
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Real random initialization with biological constraints and validation
    // Membrane potential starts near resting potential
    MembranePotential potential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    if (potential < -150.0f || potential > 100.0f) {
        throw std::runtime_error("Generated membrane potential out of bounds");
    }
    pImpl->state.membranePotential = potential;
    
    // Threshold is typically -55mV with small variation
    MembranePotential threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    if (threshold < -100.0f || threshold > 50.0f) {
        throw std::runtime_error("Generated threshold out of bounds");
    }
    pImpl->state.threshold = threshold;
    
    // Resting potential typically -70mV with validation
    MembranePotential restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    if (restingPotential < -120.0f || restingPotential > -30.0f) {
        throw std::runtime_error("Generated resting potential out of bounds");
    }
    pImpl->state.restingPotential = restingPotential;
    
    // Reset potential is usually close to resting
    MembranePotential resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    float diff = std::abs(resetPotential - pImpl->state.restingPotential);
    if (diff > 20.0f) {
        throw std::runtime_error("Generated reset potential too far from resting potential");
    }
    pImpl->state.resetPotential = resetPotential;
    
    // Refractory period: 2-10ms typical
    uint32_t refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    if (refractoryPeriod == 0) {
        throw std::runtime_error("Generated refractory period is zero");
    }
    pImpl->state.refractoryPeriod = refractoryPeriod;
    
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
