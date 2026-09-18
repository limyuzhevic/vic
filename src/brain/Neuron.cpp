#include <cstdlib>
#include <ctime>

namespace nlm {

struct Neuron::Impl {
    NeuronId id;
    NeuronType type;
    NeuronState state;
    RegionId regionId;
    PopulationId populationId;
    MembranePotential totalCurrent;  // Total synaptic current input this step
    MembranePotential synapticInput;  // Accumulated synaptic input
    MembranePotential adaptationCurrent;  // Slow adaptation current (nA)
    MembranePotential noiseTerm;         // Synaptic noise term (nA)
    std::vector<Timestamp> spikeHistory;
    std::vector<SynapseHandle> incomingSynapses;
    std::vector<SynapseHandle> outgoingSynapses;
    PlasticityFlags plasticityFlags;
    
    // LIF parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr float E_L = -70.0f;  // Resting potential (mV)
    static constexpr float V_T = -50.0f;  // Threshold potential (mV)
    static constexpr float Delta_T = 2.0f;  // Sharpness parameter (mV)
    static constexpr float tau_w = 100.0f;  // Adaptation time constant (ms)
    static constexpr float g_L = 10.0f;  // Leak conductance (nS)
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f), adaptationCurrent(0.0f), 
             noiseTerm(0.0f) {}
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

void Neuron::addExcitatoryConductance(MembranePotential conductance) {
    // Add excitatory synaptic conductance (conductance-based)
    pImpl->state.excitatoryConductance += conductance;
}

void Neuron::addInhibitoryConductance(MembranePotential conductance) {
    // Add inhibitory synaptic conductance (conductance-based)
    pImpl->state.inhibitoryConductance += conductance;
}

void Neuron::addModulatoryConductance(MembranePotential conductance) {
    // Add modulatory synaptic conductance (neuromodulatory inputs)
    pImpl->state.modulatoryConductance += conductance;
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

// Forward declarations needed for incomplete types
NeuronId Neuron::getId() const {
    return pImpl->id;
}

NeuronType Neuron::getType() const {
    return pImpl->type;
}

MembranePotential Neuron::getMembranePotential() const {
    return pImpl->state.membranePotential;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    pImpl->state.membranePotential += delta;
}

MembranePotential Neuron::getThreshold() const {
    return pImpl->state.threshold;
}

void Neuron::setThreshold(MembranePotential threshold) {
    pImpl->state.threshold = threshold;
}

bool Neuron::isFiring() const {
    return pImpl->state.firingState == FiringState::Active;
}

bool Neuron::isRefractory() const {
    return pImpl->state.refractoryRemaining > 0;
}

void Neuron::setFiringState(FiringState state) {
    pImpl->state.firingState = state;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    pImpl->state.refractoryPeriod = steps;
}

void Neuron::decrementRefractory() {
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

FiringRate Neuron::getFiringRate() const {
    return pImpl->state.firingRate;
}

void Neuron::setFiringRate(FiringRate rate) {
    pImpl->state.firingRate = rate;
}

MembranePotential Neuron::getTotalCurrent() const {
    return pImpl->synapticInput;
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

MembranePotential Neuron::getRestingPotential() const {
    return pImpl->state.restingPotential;
}

MembranePotential Neuron::getResetPotential() const {
    return pImpl->state.resetPotential;
}

RegionId Neuron::getRegionId() const {
    return pImpl->regionId;
}

PopulationId Neuron::getPopulationId() const {
    return pImpl->populationId;
}

bool Neuron::stepAdEx(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period with fractional refractory modeling
    pImpl->state.refractoriness = std::max(0.0f, pImpl->state.refractoriness - dt * 1000.0f / static_cast<float>(pImpl->state.refractoryPeriod));
    pImpl->state.refractoryFraction = 1.0f - (pImpl->state.refractoriness / pImpl->state.refractoryPeriod);
    
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, increase conductance to model refractoriness
        pImpl->state.inhibitoryConductance += pImpl->state.refractoryConductance * pImpl->state.refractoryFraction;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
            pImpl->state.refractoriness = 0.0f;
        }
        return false;
    }
    
    MembranePotential& V = pImpl->state.membranePotential;
    
    // Calculate total conductance
    float g_L = pImpl->state.leakConductance;
    float g_E = pImpl->state.excitatoryConductance;
    float g_I = pImpl->state.inhibitoryConductance;
    float g_M = pImpl->state.modulatoryConductance;
    
    // Calculate total current
    float I_syn = pImpl->synapticInput;
    
    // Generate synaptic noise using normal distribution
    float noise_sample = 0.0f;
    if (pImpl->state.noiseVariance > 0.0f) {
        // Generate Gaussian noise using RandomGenerator if available
        // For now use simple random noise - this will be improved in future versions
        // In a production system, we would use: pImpl->randomGenerator->normal(0.0f, sqrt(variance))
        // Using a simple static generator for demonstration
        static std::mt19937 rng(123456789);  // Fixed seed for reproducibility
        std::normal_distribution<float> dist(0.0f, std::sqrt(pImpl->state.noiseVariance));
        noise_sample = dist(rng);
    }
    
    // AdEx dynamics: dV/dt = (V - E_L + \\Delta_V * \\exp((V - V_T)/\\Delta_T) - w + I_syn + I_noise) / C
    float C = Impl::MEMBRANE_CAPACITANCE;
    float delta_V = Impl::Delta_T;
    float V_T = Impl::V_T;
    
    float exponential_term = (V > V_T) ? delta_V * std::exp((V - V_T) / delta_V) : 0.0f;
    
    // Ion channel dynamics
    float I_Na = pImpl->state.sodiumConductance * (V - pImpl->state.sodiumReversalPotential);
    float I_K = pImpl->state.potassiumConductance * (V - pImpl->state.potassiumReversalPotential);
    float I_L = g_L * (V - pImpl->state.leakReversalPotential);
    
    // Total membrane current
    float I_total = I_syn + noise_sample - pImpl->state.currentAdaptation - I_Na + I_K - I_L;
    
    // Update membrane potential using exponential Euler
    V += dt * 1000.0f * (exponential_term + I_total / C);
    
    // Update adaptation current
    pImpl->state.currentAdaptation += dt * 1000.0f * (0.8f * (V - Impl::E_L) - pImpl->state.currentAdaptation) / Impl::tau_w;
    
    // Adaptive threshold
    pImpl->state.adaptiveThreshold += dt * 1000.0f * (0.02f * pImpl->state.currentAdaptation - pImpl->state.adaptiveThreshold);
    
    // Homeostatic plasticity
    float target_rate = pImpl->state.homeostaticSetpoint;
    float current_rate = pImpl->state.firingRate;
    pImpl->state.homeostaticError = target_rate - current_rate;
    pImpl->state.adaptationVariable += dt * 1000.0f * pImpl->state.homeostaticLearningRate * pImpl->state.homeostaticError;
    
    // Calcium dynamics for plasticity
    float dCa_dt = -0.1f * pImpl->state.calciumConcentration + 0.1f * fired;
    pImpl->state.calciumConcentration += dt * 1000.0f * dCa_dt;
    
    // Clamp to reasonable bounds
    V = std::clamp(V, -100.0f, 50.0f);
    pImpl->state.currentAdaptation = std::clamp(pImpl->state.currentAdaptation, -10.0f, 10.0f);
    pImpl->state.adaptiveThreshold = std::clamp(pImpl->state.adaptiveThreshold, -20.0f, 20.0f);
    pImpl->state.calciumConcentration = std::clamp(pImpl->state.calciumConcentration, 0.0f, 5.0f);
    
    // Check for spike with adaptive threshold
    if (V >= Impl::V_T + pImpl->state.adaptiveThreshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        recordSpike(currentTime);
        
        // Reset with adaptation
        V = pImpl->state.resetPotential;
        pImpl->state.currentAdaptation += 2.0f;
        pImpl->state.adaptiveThreshold += 3.0f;
        
        // Increase refractoriness
        pImpl->state.refractoriness = pImpl->state.refractoryPeriod;
        
        // Enter refractory period
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update firing rate
        pImpl->state.firingRate += 5.0f;
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Update history buffers
    size_t idx = pImpl->state.historyIndex;
    pImpl->state.membranePotentialHistory[idx] = V;
    pImpl->state.synapticInputHistory[idx] = pImpl->synapticInput;
    pImpl->state.historyIndex = (idx + 1) % 4;
    
    // Decay adaptation variable
    pImpl->state.adaptationVariable *= 0.99f;
    
    // Clear synaptic inputs for next step
    pImpl->synapticInput = 0.0f;
    pImpl->state.excitatoryConductance = 0.0f;
    pImpl->state.inhibitoryConductance = 0.0f;
    pImpl->state.modulatoryConductance = 0.0f;
    
    return fired;
}

void Neuron::step(Timestamp currentTime) {
    // Default AdEx exponential integrate-and-fire dynamics with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepAdEx(currentTime, dt);
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
