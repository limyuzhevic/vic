#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <iomanip>

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
    
    // LIF parameters (now configurable)
    float membraneCapacitance;      // nF
    float timeConstant;              // ms
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f),
             membraneCapacitance(1.0f), timeConstant(20.0f) {}
};

Neuron::Neuron(NeuronId id) : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->type = NeuronType::Internal;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
}

Neuron::~Neuron() {
    delete pImpl;
}

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
    if (pImpl->spikeHistory.size() > 100) {
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
    
    // Handle refractory period with exponential decay
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, gradually recover from adaptation
        if (pImpl->state.refractoryRemaining == pImpl->state.refractoryPeriod / 2) {
            // Halfway through refractory period, start recovering from adaptation
            pImpl->state.adaptationVariable *= 0.5f;
        }
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Enhanced LIF dynamics with more biological realism
    // dV/dt = (V_rest - V)/tau + I/C + I_adaptation
    // Where I_adaptation represents spike-frequency adaptation currents
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = pImpl->state.leakConductance;  // ms, now configurable
    float C = pImpl->state.synapseConductance; // nF, now configurable
    
    // Synaptic input contributes to membrane potential change
    // Include synaptic conductance and driving force
    float synapticContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Adaptation current contribution (spike-frequency adaptation)
    float adaptationContribution = -pImpl->state.adaptationVariable * 0.005f;  // Negative for hyperpolarization
    
    // Update membrane potential using exponential Euler integration
    // Convert dt from seconds to milliseconds for biological units
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution + adaptationContribution);
    
    // Enhanced spike-frequency adaptation with more realistic dynamics
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.02f;  // Stronger adaptation effect
        pImpl->state.adaptationVariable *= 0.90f;  // Slower decay, more biologically realistic
    }
    
    // Add stochasticity for biological noise
    float biologicalNoise = 0.1f;  // 10% noise
    float randomComponent = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * biologicalNoise;
    V += randomComponent;
    
    // Enhanced membrane potential clamping with biological bounds
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike with hysteresis to prevent multiple spikes
    if (V >= threshold) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Enhanced reset dynamics with adaptation
        V = V_reset;
        
        // Enter refractory period with biological recovery
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Stronger adaptation for spike-frequency adaptation
        pImpl->state.adaptationVariable += 2.0f;  // Increased adaptation strength
        
        // Log spike for debugging
        NLM_LOG_DEBUG("Neuron " + std::to_string(pImpl->id) + " fired at time " + std::to_string(currentTime) + 
                     " (V=" + std::to_string(V) + ", threshold=" + std::to_string(threshold) + ")");
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
    // Enhanced random initialization with biological constraints and validation
    
    // Validate RNG is properly initialized
    if (!rng.engine()) {
        NLM_LOG_ERROR("Invalid random generator for neuron " + std::to_string(pImpl->id));
        return;
    }
    
    // Membrane potential starts near resting potential with biological constraints
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-5.0f, 5.0f);
    
    // Validate membrane potential is within biological bounds
    pImpl->state.membranePotential = std::clamp(pImpl->state.membranePotential, -90.0f, -50.0f);
    
    // Threshold is typically -55mV with small variation (biological range: -60 to -40mV)
    pImpl->state.threshold = -55.0f + rng.uniformReal(-5.0f, 5.0f);
    pImpl->state.threshold = std::clamp(pImpl->state.threshold, -60.0f, -40.0f);
    
    // Resting potential typically -70mV with small variation (biological range: -65 to -75mV)
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-5.0f, 5.0f);
    pImpl->state.restingPotential = std::clamp(pImpl->state.restingPotential, -75.0f, -65.0f);
    
    // Reset potential is usually close to resting potential
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 10.0f);
    pImpl->state.resetPotential = std::clamp(pImpl->state.resetPotential, pImpl->state.restingPotential - 5.0f, pImpl->state.restingPotential + 10.0f);
    
    // Refractory period: 2-20ms typical for different neuron types
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 20));
    
    // Initial state with validation
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.adaptationVariable = 0.0f;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Log initialization for debugging
    NLM_LOG_DEBUG("Neuron " + std::to_string(pImpl->id) + " initialized:");
    NLM_LOG_DEBUG("  V=" + std::to_string(pImpl->state.membranePotential) 
                 + " V_rest=" + std::to_string(pImpl->state.restingPotential)
                 + " threshold=" + std::to_string(pImpl->state.threshold)
                 + " V_reset=" + std::to_string(pImpl->state.resetPotential)
                 + " refractory=" + std::to_string(pImpl->state.refractoryPeriod) + "ms");
    
    // Clear any residual state
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
}

} // namespace nlm
