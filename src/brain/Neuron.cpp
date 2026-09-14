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

Neuron::~Neuron() = default;

Neuron::Neuron(Neuron&& other) noexcept : PImplBase(std::move(other.pImpl)) {}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    if (this != &other) {
        PImplBase::operator=(std::move(other));
    }
    return *this;
}

void Neuron::setType(NeuronType type) {
    auto& impl = getImpl();
    impl.type = type;
}

void Neuron::setMembranePotential(MembranePotential potential) {
    auto& impl = getImpl();
    impl.state.membranePotential = potential;
}

void Neuron::addToMembranePotential(MembranePotential delta) {
    auto& impl = getImpl();
    impl.state.membranePotential += delta;
}

void Neuron::setThreshold(MembranePotential threshold) {
    auto& impl = getImpl();
    impl.state.threshold = threshold;
}

void Neuron::setFiringState(FiringState state) {
    auto& impl = getImpl();
    impl.state.firingState = state;
}

void Neuron::decrementRefractory() {
    auto& impl = getImpl();
    if (impl.state.refractoryRemaining > 0) {
        --impl.state.refractoryRemaining;
        if (impl.state.refractoryRemaining == 0) {
            impl.state.firingState = FiringState::Resting;
        }
    }
}

void Neuron::setFiringRate(FiringRate rate) {
    auto& impl = getImpl();
    impl.state.firingRate = rate;
}

void Neuron::setLeakConductance(MembranePotential conductance) {
    auto& impl = getImpl();
    impl.state.leakConductance = conductance;
}

MembranePotential Neuron::getLeakConductance() const {
    return getImpl().state.leakConductance;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    auto& impl = getImpl();
    impl.state.refractoryPeriod = steps;
}

uint32_t Neuron::getRefractoryPeriod() const {
    return getImpl().state.refractoryPeriod;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    auto& impl = getImpl();
    impl.state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    return getImpl().state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    auto& impl = getImpl();
    impl.state.resetPotential = potential;
}

bool Neuron::checkThreshold() const {
    return getImpl().state.membranePotential >= getImpl().state.threshold;
}

float Neuron::getLastSpikeTime() const {
    return getImpl().state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    auto& impl = getImpl();
    impl.synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    auto& impl = getImpl();
    impl.synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    auto& impl = getImpl();
    impl.state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    auto& impl = getImpl();
    impl.synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    auto& impl = getImpl();
    impl.synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    auto& impl = getImpl();
    impl.spikeHistory.push_back(timestamp);
    if (impl.spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        impl.spikeHistory.erase(impl.spikeHistory.begin());
    }
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    return getImpl().spikeHistory;
}

void Neuron::clearSpikeHistory() {
    getImpl().spikeHistory.clear();
}

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    getImpl().incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    getImpl().outgoingSynapses.push_back(handle);
}

NeuronState& Neuron::getState() {
    return getImpl().state;
}

const NeuronState& Neuron::getState() const {
    return getImpl().state;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    return getImpl().plasticityFlags;
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
    return getImpl().plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    auto& impl = getImpl();
    impl.plasticityFlags.hebbian = hebbian;
    impl.plasticityFlags.stdp = stdp;
    impl.plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    getImpl().regionId = region;
}

RegionId Neuron::getRegionId() const {
    return getImpl().regionId;
}

void Neuron::setPopulationId(PopulationId population) {
    getImpl().populationId = population;
}

PopulationId Neuron::getPopulationId() const {
    return getImpl().populationId;
}

NeuronId Neuron::getId() const {
    return getImpl().id;
}

NeuronType Neuron::getType() const {
    return getImpl().type;
}

FiringState Neuron::getFiringState() const {
    return getImpl().state.firingState;
}

bool Neuron::isFiring() const {
    return getImpl().state.firingState == FiringState::Active;
}

bool Neuron::isRefractory() const {
    return getImpl().state.refractoryRemaining > 0;
}

FiringRate Neuron::getFiringRate() const {
    return getImpl().state.firingRate;
}

MembranePotential Neuron::getMembranePotential() const {
    return getImpl().state.membranePotential;
}

MembranePotential Neuron::getTotalCurrent() const {
    return getImpl().totalCurrent;
}

void Neuron::step(Timestamp currentTime) {
    TimestepDuration dt = 0.001;
    stepLIF(currentTime, dt);
}

void Neuron::reset() {
    auto& impl = getImpl();
    impl.state = NeuronState();
    impl.synapticInput = 0.0f;
    impl.spikeHistory.clear();
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    auto& impl = getImpl();
    impl.state.membranePotential = impl.state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    impl.state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    impl.state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    impl.state.resetPotential = impl.state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    impl.state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    impl.state.firingState = FiringState::Resting;
    impl.state.refractoryRemaining = 0;
    impl.state.adaptationVariable = 0.0f;
    impl.state.lastSpikeTime = -1.0f;
    impl.synapticInput = 0.0f;
    impl.spikeHistory.clear();
}

bool Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    auto& impl = getImpl();
    
    if (impl.state.refractoryRemaining > 0) {
        --impl.state.refractoryRemaining;
        impl.synapticInput = 0.0f;
        if (impl.state.refractoryRemaining == 0) {
            impl.state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    MembranePotential& V = impl.state.membranePotential;
    MembranePotential V_rest = impl.state.restingPotential;
    MembranePotential V_reset = impl.state.resetPotential;
    MembranePotential threshold = impl.state.threshold;
    float tau = Impl::TIME_CONSTANT;
    float C = Impl::MEMBRANE_CAPACITANCE;
    
    float synapticContribution = impl.synapticInput / C;
    float leakContribution = (V_rest - V) / tau;
    
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    if (impl.state.adaptationVariable > 0.0f) {
        V -= impl.state.adaptationVariable * 0.01f;
        impl.state.adaptationVariable *= 0.95f;
    }
    
    V = std::clamp(V, -100.0f, 50.0f);
    
    if (V >= threshold) {
        fired = true;
        impl.state.firingState = FiringState::Active;
        impl.state.lastSpikeTime = static_cast<float>(currentTime);
        recordSpike(currentTime);
        V = V_reset;
        impl.state.refractoryRemaining = impl.state.refractoryPeriod;
        impl.state.firingState = FiringState::Refractory;
        impl.state.adaptationVariable += 1.0f;
    } else {
        impl.state.firingState = FiringState::Active;
    }
    
    impl.synapticInput = 0.0f;
    
    return fired;
}

} // namespace nlm
