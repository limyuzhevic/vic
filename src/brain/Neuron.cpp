#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
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

void Neuron::stepLIF(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period with adaptive refractory period
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
    
    // ACTIVITY CONTROL: Prevent runaway excitation
    // Implement multiple levels of activity control
    
    // 1. Hard firing rate limit (preventing ~200 spikes/step)
    const float MAX_FIRING_RATE = 500.0f; // Hz - maximum physiological firing rate
    if (pImpl->state.firingRate > MAX_FIRING_RATE * 0.8f) { // 80% of max
        // Reduce synaptic input to prevent over-excitation
        pImpl->synapticInput *= 0.5f;
        // Increase adaptation to promote stability
        pImpl->state.adaptationVariable += 0.5f;
    }
    
    // 2. Activity-dependent refractory period extension
    float firingRateFactor = pImpl->state.firingRate / 100.0f; // Normalize to 100 Hz
    if (firingRateFactor > 1.0f) {
        // High firing rates get longer refractory periods
        uint32_t additionalRefractory = static_cast<uint32_t>(firingRateFactor * 5.0f); // Up to 5x extra
        pImpl->state.refractoryRemaining += std::min(additionalRefractory, uint32_t(10));
        // Increase firingState.firingState = FiringState::Refractory; would be set by spike
    }
    
    // 3. Strong adaptation for high activity
    if (pImpl->state.adaptationVariable > 10.0f) {
        // Strong adaptation hyperpolarizes the neuron
        pImpl->state.membranePotential -= 0.5f;
        pImpl->state.adaptationVariable *= 0.9f; // Decay adaptation
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
    
    // Apply spike-frequency adaptation (stronger for high activity)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f * (1.0f + firingRateFactor * 0.5f); // Enhanced adaptation
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // ACTIVITY CONTROL: Hard membrane potential clamp
    // Prevent excessive depolarization
    V = std::clamp(V, -100.0f, 50.0f); // Reduced upper bound
    
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
    
    // Check for spike with activity control
    if (V >= threshold) {
        fired = true;
        if (dt > 0.0) {
            // Limit spike-induced firing rate increase
            float rateIncrease = 0.1f * static_cast<float>(1.0 / dt);
            if (pImpl->state.firingRate + rateIncrease > MAX_FIRING_RATE) {
                rateIncrease = MAX_FIRING_RATE - pImpl->state.firingRate;
            }
            pImpl->state.firingRate += rateIncrease;
        }
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period (base + activity-dependent)
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        
        // Stronger adaptation after high-frequency spike
        pImpl->state.adaptationVariable += 2.0f * (1.0f + firingRateFactor * 0.5f);
        
        // Apply activity-dependent synaptic depression to prevent runaway
        if (firingRateFactor > 2.0f) {
            // High-frequency spike induces synaptic depression
            float depression = 0.01f * (firingRateFactor - 2.0f);
            // Apply to synapses (simplified: affects overall excitability)
            pImpl->synapticInput = std::max(0.0f, pImpl->synapticInput - depression);
        }
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
