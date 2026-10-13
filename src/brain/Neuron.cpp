#include <unordered_map>
#include <variant>

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
    
    // Enhanced neuron model parameters
    NeuronModel model;               // Neuron model type
    float membraneTimeConstant;      // Membrane time constant (ms)
    float membraneCapacitance;       // Membrane capacitance (nF)
    float adaptationStrength;        // Spike-frequency adaptation strength
    float noiseLevel;                // Stochastic input level
    float refractoriness;            // Refractory period in ms
    float reversalPotential;         // Reversal potential for synapses (mV)
    float synapticGain;              // Synaptic efficacy factor
    int layerId;                     // Layer identifier for cortical models
    bool isExcitatory;              // Excitation/inhibition flag
    float dendriticLength;           // Dendritic length (arbitrary units)
    int dendriticBranching;          // Number of dendritic branches
    DevelopmentalStage developmentalStage;  // Current developmental stage
    float plasticityModulation;      // Plasticity modulation factor
    float temperature;              // Operating temperature (Celsius)
    
    // Custom parameter storage
    std::unordered_map<std::string, std::variant<int, float, bool, std::string>> customParams;
    
    // LIF parameters (legacy support)
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f),
             model(NeuronModel::LIF), membraneTimeConstant(20.0f), membraneCapacitance(1.0f),
             adaptationStrength(0.1f), noiseLevel(0.0f), refractoriness(5.0f),
             reversalPotential(0.0f), synapticGain(1.0f), layerId(0), isExcitatory(true),
             dendriticLength(10.0f), dendriticBranching(3), developmentalStage(DevelopmentalStage::Adult),
             plasticityModulation(1.0f), temperature(37.0f) {}
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

NeuronId Neuron::getId() const {
    return pImpl->id;
}

// Type methods
NeuronType Neuron::getType() const {
    return pImpl->type;
}

void Neuron::setType(NeuronType type) {
    pImpl->type = type;
}

// NeuronModel methods
Neuron::NeuronModel Neuron::getModel() const {
    return pImpl->model;
}

void Neuron::setModel(NeuronModel model) {
    pImpl->model = model;
}

void Neuron::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float Neuron::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

void Neuron::setMembraneCapacitance(float capacitance) {
    pImpl->membraneCapacitance = capacitance;
}

float Neuron::getMembraneCapacitance() const {
    return pImpl->membraneCapacitance;
}

void Neuron::setAdaptationStrength(float strength) {
    pImpl->adaptationStrength = strength;
}

float Neuron::getAdaptationStrength() const {
    return pImpl->adaptationStrength;
}

void Neuron::setNoiseLevel(float level) {
    pImpl->noiseLevel = level;
}

float Neuron::getNoiseLevel() const {
    return pImpl->noiseLevel;
}

void Neuron::setRefractoriness(float refract) {
    pImpl->refractoriness = refract;
}

float Neuron::getRefractoriness() const {
    return pImpl->refractoriness;
}

void Neuron::setReversalPotential(float potential) {
    pImpl->reversalPotential = potential;
}

float Neuron::getReversalPotential() const {
    return pImpl->reversalPotential;
}

void Neuron::setSynapticGain(float gain) {
    pImpl->synapticGain = gain;
}

float Neuron::getSynapticGain() const {
    return pImpl->synapticGain;
}

void Neuron::setLayerId(int layer) {
    pImpl->layerId = layer;
}

int Neuron::getLayerId() const {
    return pImpl->layerId;
}

void Neuron::setIsExcitatory(bool isExc) {
    pImpl->isExcitatory = isExc;
}

bool Neuron::isExcitatory() const {
    return pImpl->isExcitatory;
}

void Neuron::setDendriticLength(float length) {
    pImpl->dendriticLength = length;
}

float Neuron::getDendriticLength() const {
    return pImpl->dendriticLength;
}

void Neuron::setDendriticBranching(int branches) {
    pImpl->dendriticBranching = branches;
}

int Neuron::getDendriticBranching() const {
    return pImpl->dendriticBranching;
}

void Neuron::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
}

DevelopmentalStage Neuron::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

void Neuron::setPlasticityModulation(float modulation) {
    pImpl->plasticityModulation = modulation;
}

float Neuron::getPlasticityModulation() const {
    return pImpl->plasticityModulation;
}

void Neuron::setTemperature(float temp) {
    pImpl->temperature = temp;
}

float Neuron::getTemperature() const {
    return pImpl->temperature;
}

template<typename T>
void Neuron::setCustomParameter(const std::string& name, T value) {
    pImpl->customParams[name] = value;
}

template<typename T>
std::optional<T> Neuron::getCustomParameter(const std::string& name) const {
    auto it = pImpl->customParams.find(name);
    if (it != pImpl->customParams.end()) {
        // Type-safe extraction would require more complex implementation
        // This is a simplified version
        try {
            return std::get<T>(it->second);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    return std::nullopt;
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
