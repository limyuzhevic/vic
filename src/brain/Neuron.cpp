#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

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
    
    // Performance optimization flags
    bool inputFilteringEnabled;
    float maxFiringRateHz;
    bool refractoryOptimizationEnabled;
    bool adaptationEnabled;
    float adaptationTimeConstant;
    float adaptationStrength;
    
    // Spike rate tracking for performance optimization
    float recentSpikeCount;
    float spikeRateEstimate;
    size_t consecutiveHighRateSpikes;
    
    // Input filtering state
    float filteredInput;
    float inputBuffer;
    size_t inputBufferSize;
    
    // LIF parameters
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;  // nF
    static constexpr float TIME_CONSTANT = 20.0f;  // ms
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f),
             inputFilteringEnabled(true),
             maxFiringRateHz(1000.0f),
             refractoryOptimizationEnabled(true),
             adaptationEnabled(true),
             adaptationTimeConstant(100.0f),
             adaptationStrength(0.01f),
             recentSpikeCount(0.0f),
             spikeRateEstimate(0.0f),
             consecutiveHighRateSpikes(0),
             filteredInput(0.0f),
             inputBuffer(0.0f),
             inputBufferSize(4) {}
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
    if (!pImpl->inputFilteringEnabled) {
        pImpl->synapticInput += amplitude;
        return;
    }
    
    // Input filtering to prevent runaway excitation
    // Simple moving average filter to smooth high-frequency inputs
    pImpl->inputBuffer += amplitude;
    pImpl->inputBufferSize++;
    
    // Calculate running average
    float avgInput = pImpl->inputBuffer / static_cast<float>(pImpl->inputBufferSize);
    
    // Apply filtering based on input level
    if (std::abs(avgInput) > globalPerformanceSettings.inputFilteringThreshold) {
        // Reduce extreme inputs
        pImpl->filteredInput = avgInput * 0.5f;
    } else {
        pImpl->filteredInput = avgInput;
    }
    
    // Add filtered input
    pImpl->synapticInput += pImpl->filteredInput;
    
    // Reset buffer periodically to adapt to changing input patterns
    if (pImpl->inputBufferSize >= 100) {
        pImpl->inputBuffer = 0.0f;
        pImpl->inputBufferSize = 0;
    }
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    // Inhibitory input filtering (similar to excitatory but negative)
    if (!pImpl->inputFilteringEnabled) {
        pImpl->synapticInput -= amplitude;
        return;
    }
    
    float avgInput = pImpl->inputBuffer / static_cast<float>(pImpl->inputBufferSize);
    if (std::abs(avgInput) > globalPerformanceSettings.inputFilteringThreshold) {
        pImpl->filteredInput = avgInput * 0.5f;
    } else {
        pImpl->filteredInput = avgInput;
    }
    
    pImpl->synapticInput -= pImpl->filteredInput;
    
    if (pImpl->inputBufferSize >= 100) {
        pImpl->inputBuffer = 0.0f;
        pImpl->inputBufferSize = 0;
    }
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    // Modulatory input affects plasticity but not directly integrated
    // Enhanced with optimization settings
    float modulationFactor = 1.0f;
    
    if (pImpl->adaptationEnabled) {
        modulationFactor = globalPerformanceSettings.adaptationStrength;
    }
    
    pImpl->state.adaptationVariable += amplitude * modulationFactor;
    
    // Limit adaptation variable to prevent runaway modulation
    if (pImpl->state.adaptationVariable > 100.0f) {
        pImpl->state.adaptationVariable = 100.0f;
    }
}

void Neuron::injectCurrent(MembranePotential current) {
    // Direct current injection with clamping to prevent instability
    if (pImpl->inputFilteringEnabled) {
        // Apply clamping based on firing rate estimate
        float rateFactor = pImpl->spikeRateEstimate / globalPerformanceSettings.maxFiringRateHz;
        if (rateFactor > 1.0f) {
            current *= (2.0f - rateFactor);  // Reduce current at high firing rates
        }
        current = std::clamp(current, -100.0f, 100.0f);  // Clamp to prevent instability
    }
    
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
    
    // Track spike rate for performance optimization
    pImpl->recentSpikeCount++;
    pImpl->consecutiveHighRateSpikes++;
}

void Neuron::clearSpikeHistory() {
    pImpl->spikeHistory.clear();
    pImpl->recentSpikeCount = 0.0f;
    pImpl->consecutiveHighRateSpikes = 0;
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
    
    // Handle refractory period with optimization
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        pImpl->synapticInput = 0.0f;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
        // Performance optimization: skip detailed rate calculation during refractory
        if (!pImpl->refractoryOptimizationEnabled) {
            pImpl->state.firingRate *= 0.9f;
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
    
    // Performance optimization: adaptive spike-frequency adaptation
    if (pImpl->adaptationEnabled && pImpl->state.adaptationVariable > 0.0f) {
        // Use time constant from global settings
        float decayFactor = std::pow(0.5f, dt * 1000.0f / globalPerformanceSettings.adaptationTimeConstant);
        float adaptationEffect = pImpl->state.adaptationVariable * globalPerformanceSettings.adaptationStrength;
        V -= adaptationEffect;
        pImpl->state.adaptationVariable *= decayFactor;  // Decay adaptation
    }
    
    // Clamp membrane potential to prevent instability with safety margins
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Performance optimization: adaptive firing rate calculation
    if (dt > 0.0) {
        // Update firing rate estimate using weighted average
        pImpl->spikeRateEstimate = 0.9f * pImpl->spikeRateEstimate + 0.1f * (pImpl->recentSpikeCount / dt);
        pImpl->recentSpikeCount = 0.0f;
        
        // Spike rate limiting based on global settings
        if (pImpl->spikeRateEstimate > globalPerformanceSettings.maxFiringRateHz) {
            // Reduce firing rate contribution when approaching limit
            float rateLimitFactor = globalPerformanceSettings.maxFiringRateHz / pImpl->spikeRateEstimate;
            pImpl->state.firingRate = pImpl->state.firingRate * rateLimitFactor;
        }
        
        // Performance optimization: rate decay
        pImpl->state.firingRate *= 0.95f;
    }
    
    // Check for spike with performance optimization
    if (V >= threshold) {
        fired = true;
        
        // Spike rate limiting based on global settings
        if (pImpl->spikeRateEstimate >= globalPerformanceSettings.maxFiringRateHz * 0.9f) {
            // High firing rate detected, suppress this spike
            // Reset membrane potential but don't enter refractory period
            V = V_reset;
            pImpl->state.firingState = FiringState::Resting;
            
            // Record spike for history
            recordSpike(currentTime);
            
            // Update adaptation
            pImpl->state.adaptationVariable += 0.5f;  // Smaller adaptation for suppressed spikes
            
            // Reset spike tracking
            pImpl->consecutiveHighRateSpikes++;
            
            if (pImpl->consecutiveHighRateSpikes > 10) {
                // Reset tracking after sustained high rate
                pImpl->consecutiveHighRateSpikes = 0;
            }
            
            return false;  // Suppress spike due to rate limiting
        }
        
        // Normal spike processing
        if (dt > 0.0) {
            pImpl->state.firingRate += 0.1f * static_cast<float>(1.0 / dt);
        }
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period with performance optimization
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
    // Reset performance tracking
    pImpl->recentSpikeCount = 0.0f;
    pImpl->spikeRateEstimate = 0.0f;
    pImpl->consecutiveHighRateSpikes = 0;
}

void Neuron::initializeRandom(RandomGenerator& rng) {
    // Real random initialization with biological constraints and performance considerations
    // Membrane potential starts near resting potential
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Threshold is typically -55mV with small variation
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Resting potential typically -70mV
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Reset potential is usually close to resting
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Refractory period: 2-10ms typical, but optimize for performance
    int refractoryRange = 10 - 2 + 1;  // 2 to 10 inclusive
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

void Neuron::enableInputFiltering(bool enabled) {
    pImpl->inputFilteringEnabled = enabled;
}

void Neuron::setMaxFiringRate(float maxRate) {
    pImpl->maxFiringRateHz = maxRate;
}

void Neuron::setRefractoryOptimization(bool enabled) {
    pImpl->refractoryOptimizationEnabled = enabled;
}

void Neuron::setAdaptationEnabled(bool enabled) {
    pImpl->adaptationEnabled = enabled;
}

size_t Neuron::getRecentSpikeCount(size_t windowSteps) const {
    // Estimate spike count based on recent spike rate
    float ratePerStep = pImpl->spikeRateEstimate / 1000.0f;  // Hz to spikes per step
    return static_cast<size_t>(ratePerStep * static_cast<float>(windowSteps));
}

float Neuron::getFiringRateEstimate(size_t windowSteps) const {
    return pImpl->spikeRateEstimate;
}

void Neuron::configureRefractoryPeriod(float maxFiringRateHz) {
    // Optimize refractory period based on maximum firing rate
    // Firing rate limit = 1 / refractoryPeriodInSeconds
    float maxRefractorySeconds = 1.0f / maxFiringRateHz;
    uint32_t optimalRefractorySteps = static_cast<uint32_t>(maxRefractorySeconds / 0.001f);  // 1ms timesteps
    
    // Use adaptive refractory period
    pImpl->state.refractoryPeriod = optimalRefractorySteps;
}

void Neuron::configureAdaptationParameters(float timeConstant, float strength) {
    pImpl->adaptationTimeConstant = timeConstant;
    pImpl->adaptationStrength = strength;
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