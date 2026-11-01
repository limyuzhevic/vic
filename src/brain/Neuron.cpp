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
    
    // Rate-based neuron parameters
    float maxFiringRate;  // Maximum firing rate in Hz
    float inputScaleFactor;  // Scale factor for input-to-rate conversion
    float rateFeedbackFactor;  // Strength of rate feedback to membrane potential
    float developmentFactor;  // Factor that affects neuron properties during development
    float plasticityLearningRate;  // Learning rate for plasticity rules
    bool rateDependentPlasticity;  // Enable plasticity that depends on firing rate
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f),
             maxFiringRate(1000.0f),  // 1000 Hz max (realistic)
             inputScaleFactor(100.0f),  // Scale input to rate
             rateFeedbackFactor(0.01f),  // Rate feedback strength
             developmentFactor(1.0f),  // Normal development
             plasticityLearningRate(0.01f),  // Default learning rate
             rateDependentPlasticity(true) {}  // Enable rate-dependent plasticity by default
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

bool Neuron::stepRateBased(Timestamp currentTime, TimestepDuration dt) {
    // Rate-based neural dynamics with integrate-and-fire
    // Combines continuous rate dynamics with discrete spike events
    
    bool fired = false;
    
    // Handle refractory period
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        pImpl->state.firingRate *= 0.9f;  // Decay firing rate during refractory
        return false;
    }
    
    // Rate-based dynamics: dV/dt = (V_rest - V)/tau + I/C + rate_feedback
    float V = pImpl->state.membranePotential;
    float V_rest = pImpl->state.restingPotential;
    float threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Calculate input contribution
    float inputContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Rate feedback term: higher firing rates increase adaptation
    float rateFeedback = pImpl->state.firingRate * 0.01f;
    
    // Update membrane potential with rate feedback
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + inputContribution - rateFeedback);
    
    // Apply spike-frequency adaptation
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;
    }
    
    // Clamp membrane potential
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Update firing rate using exponential moving average
    if (dt > 0.0) {
        // Decay existing rate
        pImpl->state.firingRate *= 0.95f;
        
        // Add contribution from input current (convert current to rate)
        float inputRate = std::max(0.0f, inputContribution * 100.0f);  // Scale factor
        pImpl->state.firingRate += inputRate * 0.1f;
        
        // Cap firing rate at biologically plausible maximum
        pImpl->state.firingRate = std::min(pImpl->state.firingRate, 1000.0f);
    }
    
    // Check for spike based on both potential AND rate
    bool spikeFromPotential = V >= threshold;
    bool spikeFromRate = (pImpl->state.firingRate > 500.0f) && (currentTime - pImpl->state.lastSpikeTime > 1.0f);
    
    if (spikeFromPotential || spikeFromRate) {
        fired = true;
        if (dt > 0.0) {
            pImpl->state.firingRate += 0.1f * static_cast<float>(1.0 / dt);
        }
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = pImpl->state.resetPotential;
        
        // Enter refractory period
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        pImpl->state.adaptationVariable += 1.0f;
    } else {
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.membranePotential = V;
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

void Neuron::stepRateBased(Timestamp currentTime, TimestepDuration dt) {
    // Rate-based neural dynamics with integrate-and-fire
    // Combines continuous rate dynamics with discrete spike events
    
    bool fired = false;
    
    // Handle refractory period
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        pImpl->state.firingRate *= 0.9f;  // Decay firing rate during refractory
        return false;
    }
    
    // Rate-based dynamics: dV/dt = (V_rest - V)/tau + I/C + rate_feedback
    float V = pImpl->state.membranePotential;
    float V_rest = pImpl->state.restingPotential;
    float threshold = pImpl->state.threshold;
    float tau = Impl::TIME_CONSTANT;  // ms
    float C = Impl::MEMBRANE_CAPACITANCE;  // nF
    
    // Calculate input contribution
    float inputContribution = pImpl->synapticInput / C;
    
    // Leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Rate feedback term: higher firing rates increase adaptation
    float rateFeedback = pImpl->state.firingRate * 0.01f;
    
    // Update membrane potential with rate feedback
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + inputContribution - rateFeedback);
    
    // Apply spike-frequency adaptation
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f;
        pImpl->state.adaptationVariable *= 0.95f;
    }
    
    // Clamp membrane potential
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Update firing rate using exponential moving average
    if (dt > 0.0) {
        // Decay existing rate
        pImpl->state.firingRate *= 0.95f;
        
        // Add contribution from input current (convert current to rate)
        float inputRate = std::max(0.0f, inputContribution * 100.0f);  // Scale factor
        pImpl->state.firingRate += inputRate * 0.1f;
        
        // Cap firing rate at biologically plausible maximum
        pImpl->state.firingRate = std::min(pImpl->state.firingRate, 1000.0f);
    }
    
    // Check for spike based on both potential AND rate
    bool spikeFromPotential = V >= threshold;
    bool spikeFromRate = (pImpl->state.firingRate > 500.0f) && (currentTime - pImpl->state.lastSpikeTime > 1.0f);
    
    if (spikeFromPotential || spikeFromRate) {
        fired = true;
        if (dt > 0.0) {
            pImpl->state.firingRate += 0.1f * static_cast<float>(1.0 / dt);
        }
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = pImpl->state.resetPotential;
        
        // Enter refractory period
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        pImpl->state.adaptationVariable += 1.0f;
    } else {
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.membranePotential = V;
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
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

// Implementation of new rate-based neuron methods

// Rate-based firing model parameters
definition void Neuron::setMaxFiringRate(FiringRate rate) {
    pImpl->maxFiringRate = rate;
}

definition FiringRate Neuron::getMaxFiringRate() const {
    return pImpl->maxFiringRate;
}

definition void Neuron::setInputScaleFactor(float factor) {
    pImpl->inputScaleFactor = factor;
}

definition float Neuron::getInputScaleFactor() const {
    return pImpl->inputScaleFactor;
}

definition void Neuron::setRateFeedbackFactor(float factor) {
    pImpl->rateFeedbackFactor = factor;
}

definition float Neuron::getRateFeedbackFactor() const {
    return pImpl->rateFeedbackFactor;
}

definition void Neuron::setDevelopmentFactor(float factor) {
    pImpl->developmentFactor = factor;
    
    // Apply development factor to neuron properties
    float scale = std::max(0.1f, pImpl->developmentFactor);
    pImpl->state.threshold = std::max(-60.0f, pImpl->state.threshold * scale);
    pImpl->state.leakConductance = std::min(20.0f, pImpl->state.leakConductance * scale);
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(std::max(2.0f, pImpl->state.refractoryPeriod * scale));
}

definition float Neuron::getDevelopmentFactor() const {
    return pImpl->developmentFactor;
}

definition void Neuron::setPlasticityLearningRate(float rate) {
    pImpl->plasticityLearningRate = rate;
}

definition float Neuron::getPlasticityLearningRate() const {
    return pImpl->plasticityLearningRate;
}

definition void Neuron::enableRateDependentPlasticity(bool enable) {
    pImpl->rateDependentPlasticity = enable;
}

definition bool Neuron::isRateDependentPlasticityEnabled() const {
    return pImpl->rateDependentPlasticity;
}

definition size_t Neuron::getTotalSpikes() const {
    return pImpl->spikeHistory.size();
}

definition float Neuron::getMeanInterSpikeInterval() const {
    if (pImpl->spikeHistory.size() < 2) {
        return 0.0f;
    }
    
    float totalInterval = 0.0f;
    for (size_t i = 1; i < pImpl->spikeHistory.size(); ++i) {
        totalInterval += pImpl->spikeHistory[i] - pImpl->spikeHistory[i - 1];
    }
    
    return totalInterval / static_cast<float>(pImpl->spikeHistory.size() - 1);
}

definition float Neuron::getFiringCoefficientOfVariation() const {
    if (pImpl->spikeHistory.size() < 2) {
        return 0.0f;
    }
    
    // Calculate inter-spike intervals
    std::vector<float> intervals;
    for (size_t i = 1; i < pImpl->spikeHistory.size(); ++i) {
        intervals.push_back(pImpl->spikeHistory[i] - pImpl->spikeHistory[i - 1]);
    }
    
    // Calculate mean and standard deviation
    float mean = 0.0f;
    for (float interval : intervals) {
        mean += interval;
    }
    mean /= intervals.size();
    
    float variance = 0.0f;
    for (float interval : intervals) {
        variance += (interval - mean) * (interval - mean);
    }
    variance /= intervals.size();
    
    float stdDev = std::sqrt(variance);
    
    // Coefficient of variation = stdDev / mean
    if (mean > 0.0f) {
        return stdDev / mean;
    }
    return 0.0f;
}

// Update stepLIF to use rate-dependent plasticity if enabled
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
    
    // Enhanced leak with development factor
    leakContribution *= pImpl->developmentFactor;
    
    // Input contribution with plasticity learning rate scaling
    synapticContribution *= (1.0f + pImpl->plasticityLearningRate);
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation (slow hyperpolarization after spike)
    if (pImpl->state.adaptationVariable > 0.0f) {
        V -= pImpl->state.adaptationVariable * 0.01f * pImpl->developmentFactor;
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
        pImpl->state.adaptationVariable += 1.0f * pImpl->developmentFactor;
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    return fired;
}

} // namespace nlm
