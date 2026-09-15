#include "AdvancedNeurons.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

namespace nlm {

// BurstingNeuron implementation
struct BurstingNeuron::Impl {
    NeuronId id;
    BurstingType burstingType;
    float membranePotential;
    float restingPotential;
    float threshold;
    float resetPotential;
    float leakConductance;
    float synapticConductance;
    float firingRate;
    FiringState firingState;
    uint32_t refractoryRemaining;
    uint32_t refractoryPeriod;
    float adaptationVariable;
    float lastSpikeTime;
    
    // Bursting parameters
    bool isBursting;
    float burstState;
    float burstFrequency;
    float burstDuration;
    float interBurstInterval;
    float burstTimer;
    float stateTimer;
    
    // Adaptation parameters
    bool adaptationEnabled;
    float adaptationRate;
    float adaptationCurrent;
    
    // Bursting history
    std::vector<Timestamp> spikeHistory;
    std::vector<float> burstStates;
    
    // Burst detection
    float spikeCount;
    float lastSpikeTimeForBurst;
    
    // Constants
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;
    static constexpr float TIME_CONSTANT = 20.0f;
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl(NeuronId neuronId) : id(neuronId), 
                              burstingType(BurstingType::IntrinsicBurster),
                              membranePotential(-70.0f),
                              restingPotential(-70.0f),
                              threshold(-55.0f),
                              resetPotential(-70.0f),
                              leakConductance(10.0f),
                              synapticConductance(0.0f),
                              firingRate(0.0f),
                              firingState(FiringState::Resting),
                              refractoryRemaining(0),
                              refractoryPeriod(5),
                              adaptationVariable(0.0f),
                              lastSpikeTime(-1.0f),
                              isBursting(false),
                              burstState(0.0f),
                              burstFrequency(10.0f),
                              burstDuration(20.0f),
                              interBurstInterval(100.0f),
                              burstTimer(0.0f),
                              stateTimer(0.0f),
                              adaptationEnabled(false),
                              adaptationRate(0.1f),
                              adaptationCurrent(0.0f),
                              spikeCount(0.0f),
                              lastSpikeTimeForBurst(-1.0f) {}
};

BurstingNeuron::BurstingNeuron(NeuronId id) : pImpl(new Impl(id)) {
    setType(NeuronType::Internal);
}

BurstingNeuron::~BurstingNeuron() = default;

bool BurstingNeuron::step(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period
    if (pImpl->refractoryRemaining > 0) {
        --pImpl->refractoryRemaining;
        pImpl->membranePotential = pImpl->resetPotential;
        if (pImpl->refractoryRemaining == 0) {
            pImpl->firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Update bursting state
    pImpl->stateTimer += static_cast<float>(dt) * 1000.0f;
    
    // Burst generation logic
    if (pImpl->isBursting) {
        pImpl->burstTimer += static_cast<float>(dt) * 1000.0f;
        
        // Generate bursts based on type
        switch (pImpl->burstingType) {
            case BurstingType::IntrinsicBurster:
                // Intrinsic burster: generates regular bursts
                if (pImpl->burstTimer >= pImpl->interBurstInterval) {
                    pImpl->burstTimer = 0.0f;
                    // Start burst
                    pImpl->burstState = 1.0f;
                    // Burst duration determines how long the neuron remains in burst mode
                    pImpl->burstTimer = 0.0f;
                }
                break;
            case BurstingType::RegularSpiking:
                // Regular spiking: maintain burst state based on frequency
                pImpl->burstState = std::sin(2.0f * M_PI * pImpl->burstTimer / pImpl->burstFrequency);
                break;
            case BurstingType::FastSpiking:
                // Fast spiking: higher frequency, shorter burst duration
                pImpl->burstFrequency = 50.0f; // Hz
                pImpl->interBurstInterval = 20.0f; // ms
                pImpl->burstState = std::sin(2.0f * M_PI * pImpl->burstTimer / pImpl->burstFrequency);
                break;
            case BurstingType::IntrinsicOscillator:
                // Oscillator: continuous oscillation
                pImpl->burstState = 0.5f + 0.5f * std::sin(2.0f * M_PI * pImpl->burstTimer / pImpl->burstFrequency);
                break;
            case BurstingType::LowThresholdSpiking:
                // LTS: slower, more sustained bursts
                pImpl->burstFrequency = 5.0f; // Hz
                pImpl->burstState = std::max(0.0f, 1.0f - pImpl->burstTimer / pImpl->burstDuration);
                break;
        }
        
    // Modify threshold during burst
    float originalThreshold = pImpl->threshold;
    pImpl->threshold = originalThreshold * (1.0f + 0.5f * pImpl->burstState);
    } else {
        pImpl->burstState = 0.0f;
        pImpl->threshold = originalThreshold;
    }
    
    // Apply spike-frequency adaptation
    if (pImpl->adaptationEnabled && pImpl->adaptationVariable > 0.0f) {
        pImpl->membranePotential -= pImpl->adaptationVariable * pImpl->adaptationRate;
        pImpl->adaptationVariable *= 0.95f;
    }
    
    // LIF dynamics (simplified)
    float V = pImpl->membranePotential;
    float V_rest = pImpl->restingPotential;
    float V_reset = pImpl->resetPotential;
    float threshold = pImpl->threshold;
    float tau = Impl::TIME_CONSTANT;
    float C = Impl::MEMBRANE_CAPACITANCE;
    
    float synapticContribution = pImpl->synapticConductance / C;
    float leakContribution = (V_rest - V) / tau;
    
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Clamp membrane potential
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike with burst modification
    bool spikeCondition = V >= threshold;
    if (spikeCondition) {
        fired = true;
        pImpl->firingState = FiringState::Active;
        pImpl->lastSpikeTime = static_cast<float>(currentTime);
        pImpl->spikeCount += 1.0f;
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        pImpl->refractoryRemaining = pImpl->refractoryPeriod;
        pImpl->firingState = FiringState::Refractory;
        
        // Update adaptation
        pImpl->adaptationVariable += 1.0f;
        
        // During burst, increase firing rate
        if (pImpl->isBursting && pImpl->burstState > 0.5f) {
            pImpl->firingRate += pImpl->burstFrequency * 0.01f;
        } else {
            pImpl->firingRate += 20.0f; // Hz
        }
    } else {
        pImpl->firingState = FiringState::Active;
    }
    
    pImpl->membranePotential = V;
    
    return fired;
}

void BurstingNeuron::reset() {
    pImpl->membranePotential = pImpl->restingPotential;
    pImpl->firingState = FiringState::Resting;
    pImpl->refractoryRemaining = 0;
    pImpl->adaptationVariable = 0.0f;
    pImpl->lastSpikeTime = -1.0f;
    pImpl->burstTimer = 0.0f;
    pImpl->stateTimer = 0.0f;
    pImpl->burstState = 0.0f;
    pImpl->firingRate = 0.0f;
    pImpl->spikeHistory.clear();
    pImpl->burstStates.clear();
}

void BurstingNeuron::setBurstingType(BurstingType type) {
    pImpl->burstingType = type;
}

void BurstingNeuron::setBurstFrequency(float frequencyHz) {
    pImpl->burstFrequency = frequencyHz;
}

void BurstingNeuron::setBurstDuration(float durationMs) {
    pImpl->burstDuration = durationMs;
}

void BurstingNeuron::setInterBurstInterval(float intervalMs) {
    pImpl->interBurstInterval = intervalMs;
}

bool BurstingNeuron::isBursting() const {
    return pImpl->isBursting;
}

float BurstingNeuron::getBurstState() const {
    return pImpl->burstState;
}

void BurstingNeuron::enableAdaptation(bool enable) {
    pImpl->adaptationEnabled = enable;
}

void BurstingNeuron::setAdaptationRate(float rate) {
    pImpl->adaptationRate = rate;
}

MembranePotential BurstingNeuron::getMembranePotential() const {
    return pImpl->membranePotential;
}

NeuronType BurstingNeuron::getType() const {
    return NeuronType::Internal;
}

void BurstingNeuron::setParameters(const void* params) {
    if (params) {
        // In a real implementation, this would cast to specific parameter type
        // For now, just accept any pointer
    }
}

const void* BurstingNeuron::getParameters() const {
    return nullptr;
}

void BurstingNeuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

const std::vector<Timestamp>& BurstingNeuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

void BurstingNeuron::configureForPattern(bool requiresBursting) {
    pImpl->isBursting = requiresBursting;
}

// AdaptiveNeuron implementation
struct AdaptiveNeuron::Impl {
    NeuronId id;
    float membranePotential;
    float restingPotential;
    float threshold;
    float resetPotential;
    float leakConductance;
    float synapticConductance;
    float firingRate;
    FiringState firingState;
    uint32_t refractoryRemaining;
    uint32_t refractoryPeriod;
    float adaptationVariable;
    float lastSpikeTime;
    
    // Adaptation parameters
    AdaptiveNeuronParams params;
    bool subthresholdAdaptationEnabled;
    float adaptationThreshold;
    float adaptationCurrent;
    float baselineAdaptation;
    
    // Activity tracking
    std::vector<Timestamp> spikeHistory;
    std::vector<float> firingRates;
    float windowSize;
    float adaptationLevel;
    
    // Constants
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;
    static constexpr float TIME_CONSTANT = 20.0f;
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl(NeuronId neuronId) : id(neuronId),
                              membranePotential(-70.0f),
                              restingPotential(-70.0f),
                              threshold(-55.0f),
                              resetPotential(-70.0f),
                              leakConductance(10.0f),
                              synapticConductance(0.0f),
                              firingRate(0.0f),
                              firingState(FiringState::Resting),
                              refractoryRemaining(0),
                              refractoryPeriod(5),
                              adaptationVariable(0.0f),
                              lastSpikeTime(-1.0f),
                              subthresholdAdaptationEnabled(false),
                              adaptationThreshold(0.5f),
                              adaptationCurrent(0.0f),
                              baselineAdaptation(0.01f),
                              windowSize(100.0f),
                              adaptationLevel(0.0f) {
        params.adaptationStrength = 1.0f;
        params.adaptationTimeConstant = 100.0f;
        params.adaptationThreshold = 0.5f;
        params.subthresholdAdaptation = 0.1f;
    }
};

AdaptiveNeuron::AdaptiveNeuron(NeuronId id) : pImpl(new Impl(id)) {
    setType(NeuronType::Internal);
}

AdaptiveNeuron::~AdaptiveNeuron() = default;

bool AdaptiveNeuron::step(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period
    if (pImpl->refractoryRemaining > 0) {
        --pImpl->refractoryRemaining;
        pImpl->membranePotential = pImpl->resetPotential;
        if (pImpl->refractoryRemaining == 0) {
            pImpl->firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Calculate recent firing rate
    float recentFiringRate = getRecentFiringRate(100.0f);
    pImpl->firingRates.push_back(recentFiringRate);
    if (pImpl->firingRates.size() > 10) {
        pImpl->firingRates.erase(pImpl->firingRates.begin());
    }
    
    // Apply subthreshold adaptation if enabled
    if (pImpl->subthresholdAdaptationEnabled && std::abs(pImpl->membranePotential - pImpl->restingPotential) < pImpl->adaptationThreshold) {
        pImpl->adaptationVariable += pImpl->params.subthresholdAdaptation * recentFiringRate;
    }
    
    // Apply spike-frequency adaptation
    if (pImpl->adaptationVariable > 0.0f) {
        pImpl->membranePotential -= pImpl->adaptationVariable * pImpl->params.adaptationStrength;
        pImpl->adaptationVariable *= std::exp(-static_cast<float>(dt) * 1000.0f / pImpl->params.adaptationTimeConstant);
    }
    
    // LIF dynamics
    float V = pImpl->membranePotential;
    float V_rest = pImpl->restingPotential;
    float V_reset = pImpl->resetPotential;
    float threshold = pImpl->threshold;
    float tau = Impl::TIME_CONSTANT;
    float C = Impl::MEMBRANE_CAPACITANCE;
    
    float synapticContribution = pImpl->synapticConductance / C;
    float leakContribution = (V_rest - V) / tau;
    
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Clamp membrane potential
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike
    if (V >= threshold) {
        fired = true;
        pImpl->firingState = FiringState::Active;
        pImpl->lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        pImpl->refractoryRemaining = pImpl->refractoryPeriod;
        pImpl->firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        pImpl->adaptationVariable += pImpl->params.adaptationStrength;
        
        // Update firing rate
        pImpl->firingRate += 20.0f; // Hz
    } else {
        pImpl->firingState = FiringState::Active;
    }
    
    pImpl->membranePotential = V;
    
    // Calculate adaptation level
    pImpl->adaptationLevel = std::min(1.0f, pImpl->adaptationVariable / pImpl->params.adaptationThreshold);
    
    return fired;
}

void AdaptiveNeuron::reset() {
    pImpl->membranePotential = pImpl->restingPotential;
    pImpl->firingState = FiringState::Resting;
    pImpl->refractoryRemaining = 0;
    pImpl->adaptationVariable = 0.0f;
    pImpl->lastSpikeTime = -1.0f;
    pImpl->adaptationLevel = 0.0f;
    pImpl->spikeHistory.clear();
    pImpl->firingRates.clear();
}

void AdaptiveNeuron::setAdaptationParams(const AdaptiveNeuronParams& params) {
    pImpl->params = params;
}

void AdaptiveNeuron::enableSubthresholdAdaptation(bool enable) {
    pImpl->subthresholdAdaptationEnabled = enable;
}

void AdaptiveNeuron::setAdaptationThreshold(float threshold) {
    pImpl->adaptationThreshold = threshold;
}

MembranePotential AdaptiveNeuron::getMembranePotential() const {
    return pImpl->membranePotential;
}

NeuronType AdaptiveNeuron::getType() const {
    return NeuronType::Internal;
}

void AdaptiveNeuron::setParameters(const void* params) {
    if (params) {
        pImpl->params = *static_cast<const AdaptiveNeuronParams*>(params);
    }
}

const void* AdaptiveNeuron::getParameters() const {
    return &pImpl->params;
}

void AdaptiveNeuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

const std::vector<Timestamp>& AdaptiveNeuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

float AdaptiveNeuron::getFiringRate() const {
    return pImpl->firingRate;
}

float AdaptiveNeuron::getAdaptationLevel() const {
    return pImpl->adaptationLevel;
}

float AdaptiveNeuron::getRecentFiringRate(float windowMs) const {
    if (pImpl->spikeHistory.size() < 2) {
        return 0.0f;
    }
    
    float timeWindow = windowMs; // ms
    size_t recentSpikes = 0;
    
    for (size_t i = 0; i < pImpl->spikeHistory.size(); ++i) {
        if (i + 1 < pImpl->spikeHistory.size()) {
            float dt = static_cast<float>(pImpl->spikeHistory[i + 1] - pImpl->spikeHistory[i]);
            if (dt <= timeWindow) {
                recentSpikes++;
            }
        }
    }
    
    if (recentSpikes == 0) return 0.0f;
    
    return (recentSpikes / (timeWindow / 1000.0f)); // Hz
}

// AdaptiveIntegrateAndFire implementation
struct AdaptiveIntegrateAndFire::Impl {
    NeuronId id;
    float membranePotential;
    float restingPotential;
    float threshold;
    float resetPotential;
    float leakConductance;
    float synapticConductance;
    float firingRate;
    FiringState firingState;
    uint32_t refractoryRemaining;
    uint32_t refractoryPeriod;
    float adaptationVariable;
    float lastSpikeTime;
    
    // Adaptive IF parameters
    AdaptiveIFParams params;
    float thresholdAdaptation;   // Adaptive threshold component
    float referencePotential;    // Reference potential for adaptation
    float adaptationConductance; // Adaptive conductance
    
    // History tracking
    std::vector<Timestamp> spikeHistory;
    std::vector<float> adaptationHistory;
    
    // Constants
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;
    static constexpr float TIME_CONSTANT = 20.0f;
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl(NeuronId neuronId) : id(neuronId),
                              membranePotential(-70.0f),
                              restingPotential(-70.0f),
                              threshold(-55.0f),
                              resetPotential(-70.0f),
                              leakConductance(10.0f),
                              synapticConductance(0.0f),
                              firingRate(0.0f),
                              firingState(FiringState::Resting),
                              refractoryRemaining(0),
                              refractoryPeriod(5),
                              adaptationVariable(0.0f),
                              lastSpikeTime(-1.0f),
                              thresholdAdaptation(0.0f),
                              referencePotential(-60.0f),
                              adaptationConductance(0.0f) {
        params.adaptationVariable = 0.0f;
        params.adaptationConductance = 0.0f;
        params.adaptationTau = 100.0f;
    }
};

AdaptiveIntegrateAndFire::AdaptiveIntegrateAndFire(NeuronId id) : pImpl(new Impl(id)) {
    setType(NeuronType::Internal);
}

AdaptiveIntegrateAndFire::~AdaptiveIntegrateAndFire() = default;

bool AdaptiveIntegrateAndFire::step(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period
    if (pImpl->refractoryRemaining > 0) {
        --pImpl->refractoryRemaining;
        pImpl->membranePotential = pImpl->resetPotential;
        if (pImpl->refractoryRemaining == 0) {
            pImpl->firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Update adaptive threshold
    pImpl->thresholdAdaptation *= std::exp(-static_cast<float>(dt) * 1000.0f / pImpl->params.adaptationTau);
    pImpl->adaptationVariable += 1.0f; // Increment with each spike (will be reset on spike)
    
    // Calculate current adaptive threshold
    float adaptiveThreshold = pImpl->threshold + pImpl->thresholdAdaptation + pImpl->adaptationConductance;
    
    // LIF dynamics
    float V = pImpl->membranePotential;
    float V_rest = pImpl->restingPotential;
    float V_reset = pImpl->resetPotential;
    float tau = Impl::TIME_CONSTANT;
    float C = Impl::MEMBRANE_CAPACITANCE;
    
    float synapticContribution = pImpl->synapticConductance / C;
    float leakContribution = (V_rest - V) / tau;
    
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Clamp membrane potential
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike
    if (V >= adaptiveThreshold) {
        fired = true;
        pImpl->firingState = FiringState::Active;
        pImpl->lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        pImpl->refractoryRemaining = pImpl->refractoryPeriod;
        pImpl->firingState = FiringState::Refractory;
        
        // Reset adaptation variable (can be refactored)
        pImpl->adaptationVariable = 0.0f;
        
        // Update firing rate
        pImpl->firingRate += 20.0f; // Hz
    } else {
        pImpl->firingState = FiringState::Active;
    }
    
    pImpl->membranePotential = V;
    
    return fired;
}

void AdaptiveIntegrateAndFire::reset() {
    pImpl->membranePotential = pImpl->restingPotential;
    pImpl->firingState = FiringState::Resting;
    pImpl->refractoryRemaining = 0;
    pImpl->adaptationVariable = 0.0f;
    pImpl->lastSpikeTime = -1.0f;
    pImpl->thresholdAdaptation = 0.0f;
    pImpl->adaptationConductance = 0.0f;
    pImpl->spikeHistory.clear();
    pImpl->adaptationHistory.clear();
}

void AdaptiveIntegrateAndFire::setAdaptiveIFParams(const AdaptiveIFParams& params) {
    pImpl->params = params;
}

void AdaptiveIntegrateAndFire::setThresholdAdaptation(float adaptationStrength) {
    pImpl->thresholdAdaptation = adaptationStrength;
}

void AdaptiveIntegrateAndFire::setReferencePotential(float refPot) {
    pImpl->referencePotential = refPot;
}

MembranePotential AdaptiveIntegrateAndFire::getMembranePotential() const {
    return pImpl->membranePotential;
}

NeuronType AdaptiveIntegrateAndFire::getType() const {
    return NeuronType::Internal;
}

void AdaptiveIntegrateAndFire::setParameters(const void* params) {
    if (params) {
        pImpl->params = *static_cast<const AdaptiveIFParams*>(params);
    }
}

const void* AdaptiveIntegrateAndFire::getParameters() const {
    return &pImpl->params;
}

void AdaptiveIntegrateAndFire::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

const std::vector<Timestamp>& AdaptiveIntegrateAndFire::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

float AdaptiveIntegrateAndFire::getAdaptationVariable() const {
    return pImpl->adaptationVariable;
}

float AdaptiveIntegrateAndFire::getThresholdEvolution() const {
    return pImpl->thresholdAdaptation;
}

// HodgkinHuxleyNeuron implementation
struct HodgkinHuxleyNeuron::Impl {
    NeuronId id;
    float membranePotential;
    float restingPotential;
    float threshold;
    float resetPotential;
    float leakConductance;
    float synapticConductance;
    float firingRate;
    FiringState firingState;
    uint32_t refractoryRemaining;
    uint32_t refractoryPeriod;
    float adaptationVariable;
    float lastSpikeTime;
    
    // HH model parameters
    HHModelParams params;
    
    // HH gating variables
    float sodiumActivation;     // m variable (0-1)
    float sodiumInactivation;   // h variable (0-1)
    float potassiumActivation;   // n variable (0-1)
    
    // HH dynamics
    float sodiumConductance;    // G_Na
    float potassiumConductance; // G_K
    float leakConductance;      // G_L
    
    // Burst detection
    bool inBurst;              // Whether neuron is currently in a burst
    float burstFrequency;       // Detected burst frequency
    float spikeCount;           // Count of recent spikes
    float lastSpikeTime;        // Time of last spike for burst detection
    
    // History tracking
    std::vector<Timestamp> spikeHistory;
    std::vector<float> gatingHistory; // Store m, h, n values over time
    
    // Constants
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;
    static constexpr float TIME_CONSTANT = 20.0f;
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl(NeuronId neuronId) : id(neuronId),
                              membranePotential(-70.0f),
                              restingPotential(-70.0f),
                              threshold(-55.0f),
                              resetPotential(-70.0f),
                              leakConductance(0.3f),
                              synapticConductance(0.0f),
                              firingRate(0.0f),
                              firingState(FiringState::Resting),
                              refractoryRemaining(0),
                              refractoryPeriod(5),
                              adaptationVariable(0.0f),
                              lastSpikeTime(-1.0f),
                              sodiumActivation(0.05f),
                              sodiumInactivation(0.6f),
                              potassiumActivation(0.32f),
                              sodiumConductance(120.0f),  // mS/cm²
                              potassiumConductance(36.0f), // mS/cm²
                              leakConductance(0.3f),   // mS/cm²
                              inBurst(false),
                              burstFrequency(0.0f),
                              spikeCount(0.0f),
                              lastSpikeTime(0.0f) {
        // Set default HH parameters
        params.maxSodiumConductance = 120.0f;
        params.maxPotassiumConductance = 36.0f;
        params.leakConductance = 0.3f;
        params.restingPotential = -70.0f;
        params.sodiumReversal = 50.0f;
        params.potassiumReversal = -77.0f;
        params.alphaNaTau = 1.0f;
        params.betaNaTau = 1.0f;
        params.alphaKTau = 1.0f;
        params.VhalfNa = -40.0f;
        params.VhalfK = -60.0f;
        params.slopeNa = 10.0f;
        params.slopeK = 10.0f;
    }
};

HodgkinHuxleyNeuron::HodgkinHuxleyNeuron(NeuronId id) : pImpl(new Impl(id)) {
    setType(NeuronType::Internal);
}

HodgkinHuxleyNeuron::~HodgkinHuxleyNeuron() = default;

bool HodgkinHuxleyNeuron::step(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period
    if (pImpl->refractoryRemaining > 0) {
        --pImpl->refractoryRemaining;
        pImpl->membranePotential = pImpl->resetPotential;
        if (pImpl->refractoryRemaining == 0) {
            pImpl->firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Update HH gating variables
    updateGatingVariables(static_cast<float>(dt) * 1000.0f);
    
    // Calculate ionic currents
    float sodiumCurrent = pImpl->sodiumConductance * 
                         std::pow(pImpl->sodiumActivation, 3) * 
                         pImpl->sodiumInactivation * 
                         (pImpl->membranePotential - pImpl->params.sodiumReversal);
    
    float potassiumCurrent = pImpl->potassiumConductance * 
                            std::pow(pImpl->potassiumActivation, 4) * 
                            (pImpl->membranePotential - pImpl->params.potassiumReversal);
    
    float leakCurrent = pImpl->leakConductance * 
                       (pImpl->membranePotential - pImpl->params.restingPotential);
    
    // Synaptic current
    float synapticCurrent = pImpl->synapticConductance;
    
    // Total current
    float totalCurrent = sodiumCurrent + potassiumCurrent + leakCurrent + synapticCurrent;
    
    // Update membrane potential (dV/dt = I/C)
    float capacitance = Impl::MEMBRANE_CAPACITANCE;
    pImpl->membranePotential += (totalCurrent / capacitance) * static_cast<float>(dt) * 1000.0f;
    
    // Clamp membrane potential
    pImpl->membranePotential = std::clamp(pImpl->membranePotential, -100.0f, 50.0f);
    
    // Check for spike (threshold crossing)
    if (pImpl->membranePotential >= pImpl->threshold) {
        fired = true;
        pImpl->firingState = FiringState::Active;
        pImpl->lastSpikeTime = static_cast<float>(currentTime);
        pImpl->spikeCount += 1.0f;
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        pImpl->membranePotential = pImpl->resetPotential;
        
        // Enter refractory period
        pImpl->refractoryRemaining = pImpl->refractoryPeriod;
        pImpl->firingState = FiringState::Refractory;
        
        // Reset gating variables (Hodgkin-Huxley reset)
        pImpl->sodiumActivation = 0.05f;
        pImpl->sodiumInactivation = 0.6f;
        pImpl->potassiumActivation = 0.32f;
        
        // Update firing rate
        pImpl->firingRate += 50.0f; // Hz (HH neurons are faster)
        
        // Analyze burst patterns
        analyzeBurstingPattern();
    } else {
        pImpl->firingState = FiringState::Active;
    }
    
    // Store gating variables history
    pImpl->gatingHistory.push_back(pImpl->sodiumActivation);
    pImpl->gatingHistory.push_back(pImpl->sodiumInactivation);
    pImpl->gatingHistory.push_back(pImpl->potassiumActivation);
    if (pImpl->gatingHistory.size() > Impl::MAX_SPIKE_HISTORY * 3) {
        pImpl->gatingHistory.erase(pImpl->gatingHistory.begin(), pImpl->gatingHistory.begin() + 3);
    }
    
    return fired;
}

void HodgkinHuxleyNeuron::reset() {
    pImpl->membranePotential = pImpl->restingPotential;
    pImpl->firingState = FiringState::Resting;
    pImpl->refractoryRemaining = 0;
    pImpl->adaptationVariable = 0.0f;
    pImpl->lastSpikeTime = -1.0f;
    pImpl->inBurst = false;
    pImpl->burstFrequency = 0.0f;
    pImpl->spikeCount = 0.0f;
    pImpl->sodiumActivation = 0.05f;
    pImpl->sodiumInactivation = 0.6f;
    pImpl->potassiumActivation = 0.32f;
    pImpl->spikeHistory.clear();
    pImpl->gatingHistory.clear();
}

void HodgkinHuxleyNeuron::setHHModelParams(const HHModelParams& params) {
    pImpl->params = params;
    // Update conductances
    pImpl->sodiumConductance = params.maxSodiumConductance;
    pImpl->potassiumConductance = params.maxPotassiumConductance;
    pImpl->leakConductance = params.leakConductance;
}

void HodgkinHuxleyNeuron::setInitialGatingVariables(float m, float h, float n) {
    pImpl->sodiumActivation = std::clamp(m, 0.0f, 1.0f);
    pImpl->sodiumInactivation = std::clamp(h, 0.0f, 1.0f);
    pImpl->potassiumActivation = std::clamp(n, 0.0f, 1.0f);
}

bool HodgkinHuxleyNeuron::isBursting() const {
    return pImpl->inBurst;
}

float HodgkinHuxleyNeuron::getBurstState() const {
    return pImpl->inBurst ? 1.0f : 0.0f;
}

MembranePotential HodgkinHuxleyNeuron::getMembranePotential() const {
    return pImpl->membranePotential;
}

NeuronType HodgkinHuxleyNeuron::getType() const {
    return NeuronType::Internal;
}

void HodgkinHuxleyNeuron::setParameters(const void* params) {
    if (params) {
        pImpl->params = *static_cast<const HHModelParams*>(params);
    }
}

const void* HodgkinHuxleyNeuron::getParameters() const {
    return &pImpl->params;
}

void HodgkinHuxleyNeuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

const std::vector<Timestamp>& HodgkinHuxleyNeuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

float HodgkinHuxleyNeuron::getSodiumActivation() const {
    return pImpl->sodiumActivation;
}

float HodgkinHuxleyNeuron::getSodiumInactivation() const {
    return pImpl->sodiumInactivation;
}

float HodgkinHuxleyNeuron::getPotassiumActivation() const {
    return pImpl->potassiumActivation;
}

void HodgkinHuxleyNeuron::analyzeBurstingPattern() {
    if (pImpl->spikeHistory.size() >= 2) {
        float timeSinceLastSpike = static_cast<float>(pImpl->spikeHistory.back() - 
                                                    pImpl->spikeHistory[pImpl->spikeHistory.size() - 2]);
        
        // Simple burst detection: spikes within 50ms are part of a burst
        if (timeSinceLastSpike < 50.0f) {
            pImpl->inBurst = true;
            pImpl->spikeCount += 1.0f;
            
            // Estimate burst frequency
            if (pImpl->spikeCount >= 2.0f) {
                pImpl->burstFrequency = 1000.0f / (pImpl->spikeCount * timeSinceLastSpike);
            }
        } else {
            pImpl->inBurst = false;
            pImpl->spikeCount = 0.0f;
            pImpl->burstFrequency = 0.0f;
        }
    }
}

bool HodgkinHuxleyNeuron::isInBurst() const {
    return pImpl->inBurst;
}

float HodgkinHuxleyNeuron::getBurstFrequency() const {
    return pImpl->burstFrequency;
}

void HodgkinHuxleyNeuron::updateGatingVariables(float dt) {
    float V = pImpl->membranePotential;
    
    // Calculate alpha and beta for sodium activation (m)
    float alpha_m = (2.5f - 0.1f * V) / (std::exp(0.1f * (V - 2.5f)) - 1.0f);
    if (std::isinf(alpha_m) || std::isnan(alpha_m)) alpha_m = 2.5f; // Handle singularity at V=25
    
    float beta_m = 4.0f * std::exp(V / 18.0f);
    
    // Calculate alpha and beta for sodium inactivation (h)
    float alpha_h = 0.07f * std::exp(-V / 20.0f);
    float beta_h = 1.0f / (std::exp(0.1f * (V - 30.0f)) + 1.0f);
    
    // Calculate alpha and beta for potassium activation (n)
    float alpha_n = (0.1f * (V + 40.0f)) / (1.0f - std::exp(-0.1f * (V + 40.0f)));
    if (std::isinf(alpha_n) || std::isnan(alpha_n)) alpha_n = 0.1f; // Handle singularity at V=-40
    
    float beta_n = 0.125f * std::exp(-V / 80.0f);
    
    // Update gating variables
    pImpl->sodiumActivation += dt * (alpha_m * (1.0f - pImpl->sodiumActivation) - beta_m * pImpl->sodiumActivation);
    pImpl->sodiumInactivation += dt * (alpha_h * (1.0f - pImpl->sodiumInactivation) - beta_h * pImpl->sodiumInactivation);
    pImpl->potassiumActivation += dt * (alpha_n * (1.0f - pImpl->potassiumActivation) - beta_n * pImpl->potassiumActivation);
    
    // Clamp to [0,1] range
    pImpl->sodiumActivation = std::clamp(pImpl->sodiumActivation, 0.0f, 1.0f);
    pImpl->sodiumInactivation = std::clamp(pImpl->sodiumInactivation, 0.0f, 1.0f);
    pImpl->potassiumActivation = std::clamp(pImpl->potassiumActivation, 0.0f, 1.0f);
}

// ResonantNeuron implementation
struct ResonantNeuron::Impl {
    NeuronId id;
    float membranePotential;
    float restingPotential;
    float threshold;
    float resetPotential;
    float leakConductance;
    float synapticConductance;
    float firingRate;
    FiringState firingState;
    uint32_t refractoryRemaining;
    uint32_t refractoryPeriod;
    float adaptationVariable;
    float lastSpikeTime;
    
    // Resonant neuron parameters
    ResonantNeuronParams params;
    
    // Resonance oscillator state
    float oscillatorPhase;        // Phase of resonance oscillator
    float oscillatorFrequency;    // Frequency of resonance
    float oscillatorAmplitude;    // Amplitude of oscillation
    float couplingStrength;       // Coupling to membrane potential
    
    // Input filtering
    float inputFilterCutoff;      // Cutoff frequency for input filtering
    float phaseShift;             // Phase shift of filtered input
    float filteredInput;          // Filtered synaptic input
    
    // Resonance metrics
    float resonanceFrequency;     // Measured resonance frequency
    float resonanceQuality;       // Quality factor (0-1)
    float phaseResponse;          // Phase response
    
    // History tracking
    std::vector<Timestamp> spikeHistory;
    std::vector<float> resonanceHistory;
    
    // Constants
    static constexpr float MEMBRANE_CAPACITANCE = 1.0f;
    static constexpr float TIME_CONSTANT = 20.0f;
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    Impl(NeuronId neuronId) : id(neuronId),
                              membranePotential(-70.0f),
                              restingPotential(-70.0f),
                              threshold(-55.0f),
                              resetPotential(-70.0f),
                              leakConductance(10.0f),
                              synapticConductance(0.0f),
                              firingRate(0.0f),
                              firingState(FiringState::Resting),
                              refractoryRemaining(0),
                              refractoryPeriod(5),
                              adaptationVariable(0.0f),
                              lastSpikeTime(-1.0f),
                              oscillatorPhase(0.0f),
                              oscillatorFrequency(10.0f),
                              oscillatorAmplitude(1.0f),
                              couplingStrength(0.5f),
                              inputFilterCutoff(10.0f),
                              phaseShift(0.0f),
                              filteredInput(0.0f),
                              resonanceFrequency(10.0f),
                              resonanceQuality(0.5f),
                              phaseResponse(0.0f) {}
};

ResonantNeuron::ResonantNeuron(NeuronId id) : pImpl(new Impl(id)) {
    setType(NeuronType::Internal);
}

ResonantNeuron::~ResonantNeuron() = default;

bool ResonantNeuron::step(Timestamp currentTime, TimestepDuration dt) {
    bool fired = false;
    
    // Handle refractory period
    if (pImpl->refractoryRemaining > 0) {
        --pImpl->refractoryRemaining;
        pImpl->membranePotential = pImpl->resetPotential;
        if (pImpl->refractoryRemaining == 0) {
            pImpl->firingState = FiringState::Resting;
        }
        return false;
    }
    
    // Update resonance oscillator
    updateResonanceOscillator(static_cast<float>(dt) * 1000.0f);
    
    // Update input filtering
    updateInputFilter(static_cast<float>(dt) * 1000.0f);
    
    // LIF dynamics with resonant input
    float V = pImpl->membranePotential;
    float V_rest = pImpl->restingPotential;
    float V_reset = pImpl->resetPotential;
    float tau = Impl::TIME_CONSTANT;
    float C = Impl::MEMBRANE_CAPACITANCE;
    
    float synapticContribution = pImpl->synapticConductance / C;
    float resonantContribution = pImpl->filteredInput * pImpl->couplingStrength;
    float leakContribution = (V_rest - V) / tau;
    
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution + resonantContribution);
    
    // Clamp membrane potential
    V = std::clamp(V, -100.0f, 50.0f);
    
    // Check for spike
    if (V >= pImpl->threshold) {
        fired = true;
        pImpl->firingState = FiringState::Active;
        pImpl->lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Reset membrane potential
        V = V_reset;
        
        // Enter refractory period
        pImpl->refractoryRemaining = pImpl->refractoryPeriod;
        pImpl->firingState = FiringState::Refractory;
        
        // Update firing rate
        pImpl->firingRate += pImpl->resonanceFrequency * 0.01f; // Scale by resonance frequency
        
        // Update resonance metrics
        updateResonanceMetrics();
    } else {
        pImpl->firingState = FiringState::Active;
    }
    
    pImpl->membranePotential = V;
    
    return fired;
}

void ResonantNeuron::reset() {
    pImpl->membranePotential = pImpl->restingPotential;
    pImpl->firingState = FiringState::Resting;
    pImpl->refractoryRemaining = 0;
    pImpl->adaptationVariable = 0.0f;
    pImpl->lastSpikeTime = -1.0f;
    pImpl->oscillatorPhase = 0.0f;
    pImpl->filteredInput = 0.0f;
    pImpl->resonanceFrequency = pImpl->params.resonanceFrequency;
    pImpl->resonanceQuality = pImpl->params.resonanceQFactor;
    pImpl->spikeHistory.clear();
    pImpl->resonanceHistory.clear();
}

void ResonantNeuron::setResonantNeuronParams(const ResonantNeuronParams& params) {
    pImpl->params = params;
    pImpl->resonanceFrequency = params.resonanceFrequency;
    pImpl->resonanceQuality = params.resonanceQFactor;
    pImpl->inputFilterCutoff = 1000.0f / (2.0f * M_PI * params.resonanceFrequency);
}

void ResonantNeuron::setResonanceFrequency(float frequencyHz) {
    pImpl->params.resonanceFrequency = frequencyHz;
    pImpl->resonanceFrequency = frequencyHz;
    pImpl->inputFilterCutoff = 1000.0f / (2.0f * M_PI * frequencyHz);
}

void ResonantNeuron::setResonanceQFactor(float qFactor) {
    pImpl->params.resonanceQFactor = qFactor;
    pImpl->resonanceQuality = qFactor;
}

bool ResonantNeuron::isBursting() const {
    // Resonant neurons don't burst by default, but can show resonance
    return false;
}

float ResonantNeuron::getBurstState() const {
    return 0.0f;
}

MembranePotential ResonantNeuron::getMembranePotential() const {
    return pImpl->membranePotential;
}

NeuronType ResonantNeuron::getType() const {
    return NeuronType::Internal;
}

void ResonantNeuron::setParameters(const void* params) {
    if (params) {
        pImpl->params = *static_cast<const ResonantNeuronParams*>(params);
    }
}

const void* ResonantNeuron::getParameters() const {
    return &pImpl->params;
}

void ResonantNeuron::recordSpike(Timestamp timestamp) {
    pImpl->spikeHistory.push_back(timestamp);
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

const std::vector<Timestamp>& ResonantNeuron::getSpikeHistory() const {
    return pImpl->spikeHistory;
}

float ResonantNeuron::getResonanceFrequency() const {
    return pImpl->resonanceFrequency;
}

float ResonantNeuron::getResonanceQuality() const {
    return pImpl->resonanceQuality;
}

float ResonantNeuron::getPhaseResponse() const {
    return pImpl->phaseResponse;
}

void ResonantNeuron::setInputFilter(float cutoffFrequencyHz) {
    pImpl->inputFilterCutoff = 1000.0f / (2.0f * M_PI * cutoffFrequencyHz);
}

void ResonantNeuron::setPhaseShift(float phaseShift) {
    pImpl->phaseShift = phaseShift;
}

void ResonantNeuron::updateResonanceOscillator(float dt) {
    // Simple resonance oscillator
    float omega = 2.0f * M_PI * pImpl->params.resonanceFrequency / 1000.0f; // Convert Hz to rad/ms
    
    // Update oscillator phase
    pImpl->oscillatorPhase += omega * dt;
    if (pImpl->oscillatorPhase > 2.0f * M_PI) {
        pImpl->oscillatorPhase -= 2.0f * M_PI;
    }
    
    // Calculate oscillator output
    pImpl->oscillatorAmplitude = pImpl->params.resonanceQFactor * 0.1f;
    
    // Couple oscillator to membrane potential
    pImpl->filteredInput += pImpl->oscillatorAmplitude * std::sin(pImpl->oscillatorPhase);
    pImpl->filteredInput *= std::exp(-dt / pImpl->params.couplingStrength);
}

void ResonantNeuron::updateInputFilter(float dt) {
    // Simple RC low-pass filter for synaptic input
    float alpha = 1.0f / (1.0f + dt * pImpl->inputFilterCutoff);
    pImpl->filteredInput = pImpl->filteredInput * alpha + pImpl->synapticConductance * (1.0f - alpha);
    
    // Apply phase shift
    pImpl->filteredInput *= std::cos(pImpl->phaseShift);
}

void ResonantNeuron::updateResonanceMetrics() {
    if (pImpl->spikeHistory.size() >= 2) {
        // Calculate average inter-spike interval
        float totalInterval = 0.0f;
        size_t count = 0;
        
        for (size_t i = 1; i < pImpl->spikeHistory.size(); ++i) {
            float interval = static_cast<float>(pImpl->spikeHistory[i] - pImpl->spikeHistory[i - 1]);
            totalInterval += interval;
            count++;
        }
        
        if (count > 0) {
            float avgInterval = totalInterval / count;
            pImpl->resonanceFrequency = 1000.0f / avgInterval; // Convert to Hz
        }
        
        // Calculate quality factor based on spike regularity
        float stddev = 0.0f;
        if (count > 1) {
            float mean = totalInterval / count;
            float variance = 0.0f;
            
            for (size_t i = 1; i < pImpl->spikeHistory.size(); ++i) {
                float interval = static_cast<float>(pImpl->spikeHistory[i] - pImpl->spikeHistory[i - 1]);
                variance += (interval - mean) * (interval - mean);
            }
            
            variance /= count;
            stddev = std::sqrt(variance);
            
            // Lower CV (coefficient of variation) means more regular, higher quality
            float cv = (mean > 0.0f) ? (stddev / mean) : 0.0f;
            pImpl->resonanceQuality = std::max(0.0f, 1.0f - cv);
        }
        
        // Calculate phase response (simplified)
        pImpl->phaseResponse = std::cos(2.0f * M_PI * pImpl->oscillatorPhase);
    }
}

// Neuron factory implementation
AdvancedNeuronBase* AdvancedNeuronFactory::createNeuron(NeuronType type, NeuronId id) {
    switch (type) {
        case NeuronType::Internal:
            return new BurstingNeuron(id);
        case NeuronType::Sensory:
            return new AdaptiveNeuron(id);
        case NeuronType::Motor:
            return new AdaptiveIntegrateAndFire(id);
        default:
            return new BurstingNeuron(id);
    }
}

AdvancedNeuronBase* AdvancedNeuronFactory::createBurstingNeuron(NeuronId id, BurstingType burstType) {
    BurstingNeuron* neuron = new BurstingNeuron(id);
    neuron->setBurstingType(burstType);
    return neuron;
}

AdvancedNeuronBase* AdvancedNeuronFactory::createAdaptiveNeuron(NeuronId id) {
    return new AdaptiveNeuron(id);
}

AdvancedNeuronBase* AdvancedNeuronFactory::createAdaptiveIFNeuron(NeuronId id) {
    return new AdaptiveIntegrateAndFire(id);
}

AdvancedNeuronBase* AdvancedNeuronFactory::createHodgkinHuxleyNeuron(NeuronId id) {
    return new HodgkinHuxleyNeuron(id);
}

AdvancedNeuronBase* AdvancedNeuronFactory::createResonantNeuron(NeuronId id) {
    return new ResonantNeuron(id);
}

void AdvancedNeuronFactory::destroyNeuron(AdvancedNeuronBase* neuron) {
    delete neuron;
}

} // namespace nlm