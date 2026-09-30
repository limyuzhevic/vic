#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>

namespace nlm {

// Legacy pImpl structure for backward compatibility during transition
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
    id_ = id;
    type_ = NeuronType::Internal;
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
    return id_;
}

NeuronType Neuron::getType() const {
    return type_;
}

void Neuron::setType(NeuronType type) {
    type_ = type;
}

const NeuronState& Neuron::getState() const {
    return stateManager_.getState();
}

NeuronState& Neuron::getState() {
    return stateManager_.getState();
}

MembranePotential Neuron::getMembranePotential() const {
    return stateManager_.getMembranePotential();
}

void Neuron::setMembranePotential(MembranePotential potential) {
    stateManager_.setMembranePotential(potential);
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    stateManager_.addToMembranePotential(delta);
}

MembranePotential Neuron::getThreshold() const {
    return stateManager_.getThreshold();
}

void Neuron::setThreshold(MembranePotential threshold) {
    stateManager_.setThreshold(threshold);
}

bool Neuron::isFiring() const {
    return stateManager_.isFiring();
}

bool Neuron::isRefractory() const {
    return stateManager_.isRefractory();
}

void Neuron::setFiringState(FiringState state) {
    stateManager_.setFiringState(state);
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    stateManager_.setRefractoryPeriod(steps);
}

void Neuron::decrementRefractory() {
    stateManager_.decrementRefractory();
}

FiringRate Neuron::getFiringRate() const {
    return stateManager_.getFiringRate();
}

void Neuron::setFiringRate(FiringRate rate) {
    stateManager_.setFiringRate(rate);
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    pImpl->state.leakConductance = conductance;
    stateManager_.setLeakConductance(conductance);
}

MembranePotential Neuron::getLeakConductance() const {
    return stateManager_.getLeakConductance();
}

uint32_t Neuron::getRefractoryPeriod() const {
    return stateManager_.getRefractoryPeriod();
}

void Neuron::setRestingPotential(MembranePotential potential) {
    stateManager_.setRestingPotential(potential);
}

MembranePotential Neuron::getRestingPotential() const {
    return stateManager_.getRestingPotential();
}

MembranePotential Neuron::getResetPotential() const {
    return stateManager_.getResetPotential();
}

bool Neuron::checkThreshold() const {
    return stateManager_.checkThreshold();
}

float Neuron::getLastSpikeTime() const {
    return stateManager_.getLastSpikeTime();
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    pImpl->synapticInput += amplitude;
    synapticInput_.receiveExcitatoryInput(amplitude);
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    pImpl->synapticInput -= amplitude;
    synapticInput_.receiveInhibitoryInput(amplitude);
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    pImpl->state.adaptationVariable += amplitude * 0.1f;
    synapticInput_.receiveModulatoryInput(amplitude);
}

void Neuron::injectCurrent(MembranePotential current) {
    pImpl->synapticInput += current;
    synapticInput_.injectCurrent(current);
}

MembranePotential Neuron::getTotalCurrent() const {
    return pImpl->synapticInput;
}

void Neuron::clearTotalCurrent() {
    synapticInput_.clearSynapticInput();
}

void Neuron::recordSpike(Timestamp timestamp) {
    spikeHandler_.recordSpike(timestamp);
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    return spikeHandler_.getSpikeHistory();
}

void Neuron::clearSpikeHistory() {
    spikeHandler_.clearSpikeHistory();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    // Store in legacy pImpl for backward compatibility
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    // Store in legacy pImpl for backward compatibility
    pImpl->outgoingSynapses.push_back(handle);
}

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    return pImpl->incomingSynapses;
}

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    return pImpl->outgoingSynapses;
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
    return plasticityFlags_;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return plasticityFlags_;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    plasticityFlags_.hebbian = hebbian;
    plasticityFlags_.stdp = stdp;
    plasticityFlags_.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    regionId_ = region;
}

RegionId Neuron::getRegionId() const {
    return regionId_;
}

void Neuron::setPopulationId(PopulationId population) {
    populationId_ = population;
}

PopulationId Neuron::getPopulationId() const {
    return populationId_;
}

void Neuron::reset() {
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    stateManager_.reset();
    synapticInput_.clearSynapticInput();
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
    
    // Use component initializer
    initializer_.initializeParameters(rng, stateManager_.getStateInternal());
}

void Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Delegate to dynamics component
    MembranePotential& V = stateManager_.getStateInternal().membranePotential;
    float& adaptationVariable = stateManager_.getStateInternal().adaptationVariable;
    uint32_t& refractoryRemaining = stateManager_.getStateInternal().refractoryRemaining;
    FiringState& firingState = stateManager_.getStateInternal().firingState;
    
    float membraneCapacitance = dynamics_.getMembraneCapacitance();
    float timeConstant = dynamics_.getTimeConstant();
    MembranePotential threshold = getThreshold();
    MembranePotential resetPotential = getResetPotential();
    
    fired = dynamics_.stepLIF(V, currentTime, dt, stateManager_.getStateInternal(),
                             getTotalCurrent(), firingState, adaptationVariable,
                             refractoryRemaining, threshold, resetPotential,
                             getLeakConductance(), membraneCapacitance, timeConstant);
    
    if (fired) {
        // Update legacy state
        pImpl->state.membranePotential = V;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike in spike handler
        spikeHandler_.recordPreSpikeForPlasticity(currentTime);
        spikeHandler_.recordPostSpikeForPlasticity(currentTime);
        
        // Update adaptation via component
        adaptation_.incrementAdaptation(adaptationVariable);
    }
}

void Neuron::step(Timestamp currentTime) {
    // Default LIF step with standard timestep (1ms)
    TimestepDuration dt = 0.001;  // 1ms default
    stepLIF(currentTime, dt);
}

} // namespace nlm