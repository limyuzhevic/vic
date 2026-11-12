// Neuron constructor implementation
Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    pImpl->synapticInput = 0.0f;
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
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in setType");
        return;
    }
    pImpl->type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in setMembranePotential");
        return;
    }
    pImpl->state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in addToMembranePotential");
        return;
    }
    pImpl->state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in setThreshold");
        return;
    }
    pImpl->state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in setFiringState");
        return;
    }
    pImpl->state.firingState = state;
}

void Neuron::decrementRefractory() {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in decrementRefractory");
        return;
    }
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in setFiringRate");
        return;
    }
    pImpl->state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in setLeakConductance");
        return;
    }
    pImpl->state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in getLeakConductance");
        return 0.0f;
    }
    return pImpl->state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in setRefractoryPeriod");
        return;
    }
    pImpl_rsaticPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in getRefractoryPeriod");
        return 0;
    }
    return pImpl->state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in setRestingPotential");
        return;
    }
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in getRestingPotential");
        return 0.0f;
    }
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in setResetPotential");
        return;
    }
    pImpl->state.resetPotential = potential;
}

MembranePotential Neuron::getResetPotential() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in getResetPotential");
        return 0.0f;
    }
    return pImpl->state.resetPotential;
}

bool Neuron::checkThreshold() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in checkThreshold");
        return false;
    }
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

float Neuron::getLastSpikeTime() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in getLastSpikeTime");
        return -1.0f;
    }
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in receiveExcitatoryInput");
        return;
    }
    // Real synaptic input: excitatory currents add to total current
    // amplitude represents synaptic conductance * reversal potential contribution
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in receiveInhibitoryInput");
        return;
    }
    // Real inhibitory input: subtract from total current
    // Inhibitory synaptic currents hyperpolarize the neuron
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in receiveModulatoryInput");
        return;
    }
    // Modulatory input affects plasticity but not directly integrated
    // Used for neuromodulation (e.g., dopamine, acetylcholine)
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in injectCurrent");
        return;
    }
    // Direct current injection (e.g., from sensory input or external source)
    // Add to synaptic input for LIF integration
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in clearTotalCurrent");
        return;
    }
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in recordSpike");
        return;
    }
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

void Neuron::clearSpikeHistory() {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in clearSpikeHistory");
        return;
    }
    pImpl->spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in addIncomingSynapse");
        return;
    }
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in addOutgoingSynapse");
        return;
    }
    pImpl->outgoingSynapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in getState");
        static NeuronState invalidState;
        return invalidState;
    }
    return pImpl->state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in getPlasticityFlags");
        static PlasticityFlags invalidFlags;
        return invalidFlags;
    }
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in enablePlasticity");
        return;
    }
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in setRegionId");
        return;
    }
    pImpl->regionId = region;
}

RegionId Neuron::getRegionId() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in getRegionId");
        return INVALID_REGION_ID;
    }
    return pImpl->regionId;
}

void Neuron::setPopulationId(PopulationId population) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in setPopulationId");
        return;
    }
    pImpl->populationId = population;
}

PopulationId Neuron::getPopulationId() const {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in getPopulationId");
        return INVALID_POPULATION_ID;
    }
    return pImpl->populationId;
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in stepLIF");
        return false;
    }
    
    // Handle refractory period
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        // Keep the firing-rate metric decaying even while refractory, so it
        // reflects recent activity instead of freezing at its spike value.
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
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in reset");
        return;
    }
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    if (!pImpl) {
        NLM_LOG_ERROR("Null neuron implementation in initializeRandom");
        return;
    }
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