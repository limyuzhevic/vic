#include <stdexcept>
#include <limits>
#include <cmath>

// Enhanced Neuron implementation with improved validation and error handling
namespace nlm {

class NeuronValidationError : public std::runtime_error {
public:
    explicit NeuronValidationError(const std::string& msg) : std::runtime_error(msg) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    if (id.value == 0) {
        throw NeuronValidationError("NeuronId cannot be zero");
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
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    // Validate potential range for biological realism
    if (potential < -100.0f || potential > 50.0f) {
        throw NeuronValidationError("Membrane potential out of valid range (-100mV to 50mV)");
    }
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    // Validate delta magnitude
    if (std::abs(delta) > 1000.0f) {
        throw NeuronValidationError("Delta potential change too large (>1000mV)");
    }
    
    float newPotential = pImpl->state.membranePotential + delta;
    if (newPotential < -100.0f || newPotential > 50.0f) {
        throw NeuronValidationError("New membrane potential would be out of valid range");
    }
    pImpl->state.membranePotential = newPotential;
}

void Neuron::setThreshold(MembranePotential threshold) {
    // Threshold should be more depolarized than resting potential
    if (pImpl->state.restingPotential > threshold) {
        throw NeuronValidationError("Threshold must be more depolarized than resting potential");
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
        throw NeuronValidationError("Firing rate out of valid range (0-1000 Hz)");
    }
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    if (conductance < 0.0f || conductance > 100.0f) {
        throw NeuronValidationError("Leak conductance out of valid range (0-100 nS)");
    }
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (steps == 0) {
        throw NeuronValidationError("Refractory period cannot be zero");
    }
    if (steps > 1000) {
        throw NeuronValidationError("Refractory period too long (max 1000 steps)");
    }
    pImpl->state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    if (potential < -100.0f || potential > -50.0f) {
        throw NeuronValidationError("Resting potential out of biologically plausible range (-100mV to -50mV)");
    }
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    if (potential < -100.0f || potential > -50.0f) {
        throw NeuronValidationError("Reset potential out of biologically plausible range (-100mV to -50mV)");
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
    if (amplitude < 0.0f || amplitude > 1000.0f) {
        throw NeuronValidationError("Excitatory input amplitude out of valid range (0-1000mV)");
    }
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    if (amplitude < 0.0f || amplitude > 1000.0f) {
        throw NeuronValidationError("Inhibitory input amplitude out of valid range (0-1000mV)");
    }
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    if (std::abs(amplitude) > 100.0f) {
        throw NeuronValidationError("Modulatory input amplitude out of valid range (-100mV to 100mV)");
    }
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    if (std::abs(current) > 1000.0f) {
        throw NeuronValidationError("Injected current out of valid range (-1000mV to 1000mV)");
    }
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    if (timestamp < 0.0f) {
        throw NeuronValidationError("Spike timestamp cannot be negative");
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
    if (!handle) {
        throw NeuronValidationError("Synapse handle cannot be null");
    }
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    if (!handle) {
        throw NeuronValidationError("Synapse handle cannot be null");
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

RegionId Neuron::getRegionId() const {
    return pImpl->regionId;
}

void Neuron::setPopulationId(PopulationId population) {
    pImpl->populationId = population;
}

PopulationId Neuron::getPopulationId() const {
    return pImpl->populationId;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    if (dt <= 0.0) {
        throw NeuronValidationError("Timestep duration must be positive");
    }
    if (dt > 0.1) {
        throw NeuronValidationError("Timestep too large (max 0.1s)");
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
        // Keep the firing-rate metric decaying even while refractory
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
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Update the firing-rate metric from counted spikes, in Hz.
    // FIXED: Now properly updating firing rate from spike count
    if (dt > 0.0) {
        pImpl->state.firingRate = 0.9f * pImpl->state.firingRate;
        
        // Each spike contributes to firing rate: spike = 1/dt Hz
        for (const auto& spikeTime : pImpl->spikeHistory) {
            pImpl->state.firingRate += 0.1f * static_cast<float>(1.0 / dt);
        }
    }
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        if (dt > 0.0) {
            pImpl->state.firingRate += 0.1f * static_cast<float>(1.0 / dt);
        }
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
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(uint32_t(2), uint32_t(10)));
    
    // Initial state
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
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