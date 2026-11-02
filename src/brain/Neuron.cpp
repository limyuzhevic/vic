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
    // Activity regulation
    MembranePotential activityThreshold;
    MembranePotential adaptationConductance;
    FiringRate homeostaticTarget;
    float learningRate;
    FiringRate maxFiringRate;
    float energy;
    bool structuralPlasticityEnabled;
    DevelopmentalStage developmentalStage;
    uint32_t activityCounter;
    float activityHistory;
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
    
    // Update the firing-rate metric from counted spikes, in Hz.
    // Previously setFiringRate() was never called anywhere, so
    // getAverageFiringRate() summed zeros even with thousands of spikes.
    // EMA per brain step: a spike contributes 1/dt Hz, otherwise decay.
    // dt is in seconds (0.001 in the demos), so a neuron firing every
    // step would approach 1000 Hz, and refractory periods cap it well
    // below that in practice.
    if (dt > 0.0) {
        pImpl->state.firingRate = 0.9f * pImpl->state.firingRate;
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

}

namespace nlm {

// Activity regulation implementations

void Neuron::setAdaptationConductance(MembranePotential conductance) {
    pImpl->adaptationConductance = conductance;
}

MembranePotential Neuron::getAdaptationConductance() const {
    return pImpl->adaptationConductance;
}

void Neuron::setActivityThreshold(MembranePotential threshold) {
    pImpl->activityThreshold = threshold;
}

MembranePotential Neuron::getActivityThreshold() const {
    return pImpl->activityThreshold;
}

void Neuron::resetActivityCounter() {
    pImpl->activityCounter = 0;
}

uint32_t Neuron::getActivityCounter() const {
    return pImpl->activityCounter;
}

bool Neuron::shouldInhibit() const {
    // Inhibit if firing rate exceeds threshold or adaptation variable is high
    return (getFiringRate() > pImpl->homeostaticTarget * 2.0f) || 
           (pImpl->state.adaptationVariable > 10.0f);
}

void Neuron::setHomeostaticTarget(FiringRate targetHz) {
    pImpl->homeostaticTarget = targetHz;
}

FiringRate Neuron::getHomeostaticTarget() const {
    return pImpl->homeostaticTarget;
}

FiringRate Neuron::getHomeostaticError() const {
    return getFiringRate() - pImpl->homeostaticTarget;
}

void Neuron::adjustHomeostasis(TimestepDuration dt) {
    // Implement homeostatic plasticity: slow, ongoing adjustment toward target firing rate
    float error = getHomeostaticError() / pImpl->homeostaticTarget;  // Normalize to [-1, 1]
    float adjustment = error * 0.001f * dt;  // Small homeostatic adjustment per timestep
    
    // Adjust leak conductance to maintain target firing rate
    float currentConductance = getLeakConductance();
    currentConductance = std::max(0.1f, std::min(currentConductance + adjustment, 100.0f));
    setLeakConductance(currentConductance);
}

void Neuron::receiveNeuromodulationInput(float amount, NeuromodulatorType type) {
    // Simplified neuromodulation: scale firing rate based on input
    if (type == NeuromodulatorType::Dopamine) {
        // Positive dopamine increases excitability, negative decreases
        float excitabilityModifier = std::clamp(amount, -1.0f, 1.0f);
        setThreshold(getThreshold() + excitabilityModifier * 5.0f);  // Shift threshold
    } else if (type == NeuromodulatorType::Adenosine) {
        // Adenosine is inhibitory, reduces excitability
        float inhibitionModifier = -std::clamp(amount, 0.0f, 1.0f);
        setThreshold(getThreshold() + inhibitionModifier * 5.0f);
    }
}

float Neuron::getAverageMembranePotential() const {
    return pImpl->state.membranePotential;  // Simplified: return current potential
}

FiringRate Neuron::getAverageFiringRate() const {
    return pImpl->state.firingRate;
}

float Neuron::getActivityHistogram() const {
    // Simplified: return a simple activity counter
    return static_cast<float>(pImpl->activityCounter);
}

void Neuron::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.001f, 1.0f);
}

float Neuron::getLearningRate() const {
    return pImpl->learningRate;
}

void Neuron::modulateLearningRate(float factor) {
    pImpl->learningRate *= (1.0f + factor);
    pImpl->learningRate = std::clamp(pImpl->learningRate, 0.001f, 1.0f);
}

void Neuron::enableStructuralPlasticity(bool enable) {
    pImpl->structuralPlasticityEnabled = enable;
}

bool Neuron::isStructuralPlasticityEnabled() const {
    return pImpl->structuralPlasticityEnabled;
}

void Neuron::growSynapse(NeuronId target, float initialWeight) {
    if (pImpl->structuralPlasticityEnabled) {
        // Create new synapse to target
        // This would need to interact with the brain's synapse management system
        // For now, just record that we want to grow
        pImpl->activityCounter++;  // Increment activity counter as evidence of growth
    }
}

void Neuron::pruneSynapse(SynapseHandle handle) {
    if (pImpl->structuralPlasticityEnabled) {
        // Remove synapse from list
        // This would need to interact with the brain's synapse management system
        pImpl->activityCounter++;  // Increment activity counter as evidence of pruning
    }
}

void Neuron::setDevelopmentalStage(DevelopmentalStage stage) {
    pImpl->developmentalStage = stage;
    
    // Adjust parameters based on developmental stage
    switch (stage) {
        case DevelopmentalStage::Initial:
            // High plasticity, low firing thresholds
            setThreshold(-45.0f);
            setHomeostaticTarget(5.0f);
            pImpl->learningRate = 0.05f;
            break;
        case DevelopmentalStage::CriticalPeriod:
            // Even higher plasticity, sensitive to environment
            setThreshold(-40.0f);
            setHomeostaticTarget(10.0f);
            pImpl->learningRate = 0.1f;
            break;
        case DevelopmentalStage::Maturation:
            // Moderate plasticity, stabilizing patterns
            setThreshold(-55.0f);
            setHomeostaticTarget(15.0f);
            pImpl->learningRate = 0.02f;
            break;
        case DevelopmentalStage::Adult:
            // Low plasticity, stable function
            setThreshold(-60.0f);
            setHomeostaticTarget(20.0f);
            pImpl->learningRate = 0.005f;
            break;
        case DevelopmentalStage::Aging:
            // Decreased plasticity, reduced activity
            setThreshold(-65.0f);
            setHomeostaticTarget(10.0f);
            pImpl->learningRate = 0.001f;
            break;
        default:
            break;
    }
}

DevelopmentalStage Neuron::getDevelopmentalStage() const {
    return pImpl->developmentalStage;
}

float Neuron::getPlasticityRate() const {
    // Plasticity rate depends on developmental stage
    switch (pImpl->developmentalStage) {
        case DevelopmentalStage::Initial:
        case DevelopmentalStage::CriticalPeriod:
            return 0.1f;
        case DevelopmentalStage::Maturation:
            return 0.05f;
        case DevelopmentalStage::Adult:
            return 0.01f;
        case DevelopmentalStage::Aging:
            return 0.005f;
        default:
            return 0.01f;
    }
}

void Neuron::setMaxFiringRate(FiringRate maxRate) {
    pImpl->maxFiringRate = std::max(1.0f, maxRate);
}

FiringRate Neuron::getMaxFiringRate() const {
    return pImpl->maxFiringRate;
}

void Neuron::setEnergyThreshold(float energy) {
    pImpl->energy = std::max(0.0f, energy);
}

float Neuron::getEnergy() const {
    return pImpl->energy;
}

bool Neuron::hasEnergy(float required) const {
    return pImpl->energy >= required;
}

// Update activity counter when neuron is active
void Neuron::incrementActivity() {
    pImpl->activityCounter++;
}

void Neuron::decayActivity(float decayRate) {
    pImpl->activityHistory = pImpl->activityHistory * (1.0f - decayRate) + 
                              static_cast<float>(pImpl->activityCounter) * decayRate;
    pImpl->activityCounter = 0;  // Reset after recording
}

void Neuron::addActivity(float activityValue) {
    pImpl->activityHistory = pImpl->activityHistory * 0.9f + activityValue * 0.1f;
}
