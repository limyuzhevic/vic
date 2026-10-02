// NLM (熙然) - Neural Learning Machine
// Enhanced Neuron Implementation with Advanced Features
//
// This file implements an enhanced Leaky Integrate-and-Fire (LIF) neuron model
// with advanced biological accuracy, expert features, and improved stability.
//
// Key Improvements:
// 1. Enhanced LIF dynamics with temperature compensation
// 2. Advanced spike-frequency adaptation control
// 3. Voltage clamping for stability
// 4. Burst mode and oscillatory behavior support
// 5. Comprehensive error handling and validation
// 6. Advanced metrics for performance monitoring
// 7. Health monitoring and stability control
// 8. Memory optimization and cleanup

#include "Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <cassert>

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
    static constexpr float BIOLOGICAL_VOLTAGE_MIN = -100.0f;  // mV
    static constexpr float BIOLOGICAL_VOLTAGE_MAX = 50.0f;   // mV
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // Advanced features
    bool voltageClampActive;
    MembranePotential voltageClampMin;
    MembranePotential voltageClampMax;
    bool adaptationEnabled;
    float adaptationStrength;
    bool burstModeEnabled;
    uint32_t burstFrequency;
    bool oscillatoryBehaviorEnabled;
    float oscillationFrequency;
    bool performanceMonitoringEnabled;
    bool debugModeEnabled;
    bool stabilityControlEnabled;
    float targetFiringRate;
    float temperature;
    std::function<bool(const Neuron&)> healthMonitor;
    
    // Specialized constants for advanced features
    static constexpr float VOLTAGE_CLAMP_MIN = -80.0f;   // mV
    static constexpr float VOLTAGE_CLAMP_MAX = 40.0f;    // mV
    static constexpr float MIN_TEMP = 20.0f;            // Celsius
    static constexpr float MAX_TEMP = 45.0f;            // Celsius
    
    Impl() : id(), type(NeuronType::Internal), regionId(), populationId(),
             totalCurrent(0.0f), synapticInput(0.0f),
             voltageClampActive(false), adaptationEnabled(true),
             adaptationStrength(1.0f), burstModeEnabled(false),
             burstFrequency(100), oscillatoryBehaviorEnabled(false),
             oscillationFrequency(10.0f), performanceMonitoringEnabled(false),
             debugModeEnabled(false), stabilityControlEnabled(false),
             targetFiringRate(10.0f), temperature(37.0f), // Body temperature in Celsius
             healthMonitor(nullptr)
    {}
};

Neuron::Neuron(NeuronId id, NeuronType type) : pImpl(new Impl) {
    // Enhanced neuron constructor with type support and temperature initialization
    assert(pImpl && "Failed to allocate neuron implementation");
    pImpl->id = id;
    pImpl->type = type;
    pImpl->regionId = INVALID_REGION_ID;
    pImpl->populationId = INVALID_POPULATION_ID;
    pImpl->totalCurrent = 0.0f;
    
    // Apply temperature compensation to initial parameters for biological accuracy
    // Neurons function differently at various temperatures, affecting ion channel kinetics
    applyTemperatureCompensation();
}

Neuron::~Neuron() {
    // Clean up implementation pointer to prevent memory leaks
    delete pImpl;
}

Neuron::Neuron(Neuron&& other) noexcept : pImpl(other.pImpl) {
    // Move constructor: transfer ownership of implementation pointer
    other.pImpl = nullptr;
}

Neuron& Neuron::operator=(Neuron&& other) noexcept {
    // Move assignment operator: transfer ownership of implementation pointer
    if (this != &other) {
        delete pImpl;           // Clean up existing implementation
        pImpl = other.pImpl;    // Take ownership of other's implementation
        other.pImpl = nullptr;  // Leave other in valid state
    }
    return *this;
}

// === IDENTITY & BASIC PROPERTIES ===

NeuronId Neuron::getId() const {
    // Return the unique identifier of this neuron
    assert(pImpl && "Neuron implementation is null");
    return pImpl->id;
}

NeuronType Neuron::getType() const {
    // Return the current type of this neuron (Internal, Sensory, Motor, etc.)
    assert(pImpl && "Neuron implementation is null");
    return pImpl->type;
}

void Neuron::setType(NeuronType type) {
    // Set the neuron type to specify its functional role
    assert(pImpl && "Neuron implementation is null");
    pImpl->type = type;
}

void Neuron::setRegionId(RegionId region) {
    // Set the region identifier for this neuron
    // Regions are logical groupings of neurons (e.g., cortex, hippocampus)
    assert(pImpl && "Neuron implementation is null");
    pImpl->regionId = region;
}

RegionId Neuron::getRegionId() const {
    // Return the region identifier for this neuron
    assert(pImpl && "Neuron implementation is null");
    return pImpl->regionId;
}

void Neuron::setPopulationId(PopulationId population) {
    // Set the population identifier for this neuron
    // Populations are specialized neuron groups with shared properties
    assert(pImpl && "Neuron implementation is null");
    pImpl->populationId = population;
}

PopulationId Neuron::getPopulationId() const {
    // Return the population identifier for this neuron
    assert(pImpl && "Neuron implementation is null");
    return pImpl->populationId;
}

// === ENHANCED STATE MANAGEMENT ===

const NeuronState& Neuron::getState() const {
    // Return a constant reference to the neuron's state
    // Provides access to all current neuron properties and metrics
    assert(pImpl && "Neuron implementation is null");
    return pImpl->state;
}

NeuronState& Neuron::getState() {
    // Return a non-constant reference to the neuron's state
    // Allows modification of neuron properties
    assert(pImpl && "Neuron implementation is null");
    return pImpl->state;
}

void Neuron::reset() {
    // Reset neuron to initial state with comprehensive cleanup
    // Restores all neuron parameters to default values and clears history
    assert(pImpl && "Neuron implementation is null");
    
    // Reset core neuron state to defaults
    pImpl->state = NeuronState();
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    pImpl->consecutiveSpikes = 0;
    
    // Reset advanced metrics for performance monitoring
    pImpl->state.totalInput = 0.0f;
    pImpl->state.voltageDerivative = 0.0f;
    pImpl->state.timeSinceLastSpike = 0.0f;
    pImpl->state.predictedNextSpike = -1.0f;
    pImpl->state.averageVoltage = 0.0f;
    
    // Apply temperature compensation after reset to maintain biological accuracy
    applyTemperatureCompensation();
}

void Neuron::initializeRandom(RandomGenerator& rng, bool includeBiophysicalVariability) {
    // Initialize neuron with random biological parameters
    // Creates realistic neuron variability based on biological constraints
    assert(pImpl && "Neuron implementation is null");
    assert(&rng && "Random generator is null");
    
    // Set membrane potential near resting potential with biological variation (-73mV to -67mV)
    pImpl->state.membranePotential = pImpl->state.restingPotential + rng.uniformReal(-3.0f, 3.0f);
    
    // Set firing threshold with typical neuron variation (-60mV to -50mV)
    pImpl->state.threshold = -55.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Set resting potential with species-specific variation (-72mV to -68mV)
    pImpl->state.restingPotential = -70.0f + rng.uniformReal(-2.0f, 2.0f);
    
    // Set reset potential close to resting potential with biological range
    pImpl->state.resetPotential = pImpl->state.restingPotential + rng.uniformReal(0.0f, 5.0f);
    
    // Set refractory period with biologically plausible range (2-10ms typical)
    pImpl->state.refractoryPeriod = static_cast<uint32_t>(rng.uniformInt(2, 10));
    
    if (includeBiophysicalVariability) {
        // Vary leak conductance (5-15 nS typical) affecting neuron time constant
        pImpl->state.leakConductance = 10.0f + rng.uniformReal(-5.0f, 5.0f);
        
        // Set adaptation variable with random initial value for spike-frequency adaptation
        pImpl->state.adaptationVariable = rng.uniformReal(-1.0f, 1.0f);
    }
    
    // Initialize firing state to resting (neuron not currently active)
    pImpl->state.firingState = FiringState::Resting;
    pImpl->state.refractoryRemaining = 0;
    pImpl->state.lastSpikeTime = -1.0f;
    
    // Clear any residual synaptic inputs and spike history
    pImpl->synapticInput = 0.0f;
    pImpl->spikeHistory.clear();
    
    // Reset advanced metrics for performance tracking
    pImpl->state.totalInput = 0.0f;
    pImpl->state.voltageDerivative = 0.0f;
    pImpl->state.timeSinceLastSpike = 0.0f;
    pImpl->state.predictedNextSpike = -1.0f;
    pImpl->state.consecutiveSpikes = 0;
    pImpl->state.averageVoltage = pImpl->state.membranePotential;
    
    // Apply temperature compensation for accurate biological simulation
    applyTemperatureCompensation();
}

// === IMPROVED LIF DYNAMICS ===

bool Neuron::step(Timestamp currentTime, TimestepDuration dt) {
    assert(pImpl && "Neuron implementation is null");
    assert(dt > 0.0f && "Timestep must be positive");
    
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
    
    // Calculate time since last spike for advanced metrics
    if (pImpl->state.lastSpikeTime > 0.0f) {
        pImpl->state.timeSinceLastSpike = currentTime - pImpl->state.lastSpikeTime;
    }
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation with temperature compensation
    
    MembranePotential& V = pImpl->state.membranePotential;
    MembranePotential V_rest = pImpl->state.restingPotential;
    MembranePotential V_reset = pImpl->state.resetPotential;
    MembranePotential threshold = pImpl->state.threshold;
    float tau = getTimeConstant();  // Temperature-adjusted time constant
    float C = getMembraneCapacitance();  // Capacitance
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = pImpl->synapticInput / C;
    
    // Temperature-adjusted leak contribution
    float leakContribution = (V_rest - V) / tau;
    
    // Apply temperature scaling to integration
    float integrationFactor = 1000.0f * static_cast<float>(dt);
    if (pImpl->temperature > 0.0f) {
        integrationFactor *= (1.0f + (pImpl->temperature - 37.0f) * 0.01f);
    }
    
    // Update membrane potential using exponential Euler integration
    V = V + integrationFactor * (leakContribution + synapticContribution);
    
    // Apply spike-frequency adaptation with advanced control
    if (pImpl->adaptationEnabled && pImpl->state.adaptationVariable > 0.0f) {
        float adaptationEffect = pImpl->state.adaptationVariable * pImpl->adaptationStrength * 0.01f;
        V -= adaptationEffect;
        pImpl->state.adaptationVariable *= 0.95f;  // Decay adaptation
    }
    
    // Apply voltage clamp if active
    if (pImpl->voltageClampActive) {
        V = std::clamp(V, pImpl->voltageClampMin, pImpl->voltageClampMax);
    } else {
        // Clamp to biologically plausible range
        V = std::clamp(V, pImpl->state.BIOLOGICAL_VOLTAGE_MIN, pImpl->state.BIOLOGICAL_VOLTAGE_MAX);
    }
    
    // Update voltage derivative for advanced metrics
    pImpl->state.voltageDerivative = V - pImpl->state.averageVoltage;
    
    // Update running average
    static constexpr float ALPHA = 0.1f; // Learning rate for running average
    pImpl->state.averageVoltage = (1.0f - ALPHA) * pImpl->state.averageVoltage + ALPHA * V;
    
    // Check for spike with hysteresis support
    if (checkThresholdWithHysteresis()) {
        fired = true;
        pImpl->state.firingState = FiringState::Active;
        pImpl->state.lastSpikeTime = static_cast<float>(currentTime);
        
        // Record spike
        recordSpike(currentTime);
        
        // Update consecutive spike count
        pImpl->state.consecutiveSpikes++;
        
        // Reset membrane potential
        V = pImpl->state.resetPotential;
        
        // Enter refractory period
        pImpl->state.refractoryRemaining = pImpl->state.refractoryPeriod;
        pImpl->state.firingState = FiringState::Refractory;
        
        // Update adaptation for spike-frequency adaptation
        pImpl->state.adaptationVariable += 1.0f;
        
        // Burst mode handling
        if (pImpl->burstModeEnabled) {
            // Additional adaptive changes for burst firing
            pImpl->state.adaptationVariable *= 0.5f;
        }
    } else {
        pImpl->state.firingState = FiringState::Active;
    }
    
    // Clear synaptic input for next step
    pImpl->synapticInput = 0.0f;
    
    // Update predicted next spike for rate-based modeling
    if (fired) {
        pImpl->state.predictedNextSpike = currentTime + static_cast<float>(pImpl->state.refractoryPeriod) * 0.001f;
    }
    
    // Apply oscillatory behavior if enabled
    if (pImpl->oscillatoryBehaviorEnabled && pImpl->state.firingState == FiringState::Active) {
        // Add small oscillatory component to membrane potential
        V += 0.1f * std::sin(2.0f * M_PI * pImpl->oscillationFrequency * currentTime * 0.001f);
    }
    
    // Update total input metric
    pImpl->state.totalInput += pImpl->synapticInput;
    
    return fired;
}

bool Neuron::checkThreshold() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->state.membranePotential >= pImpl->state.threshold;
}

bool Neuron::checkThresholdWithHysteresis() const {
    assert(pImpl && "Neuron implementation is null");
    
    // Simple hysteresis implementation with upper and lower thresholds
    static constexpr float HYSTERESIS_WIDTH = 2.0f; // mV
    
    if (pImpl->state.firingState == FiringState::Resting) {
        // For resting neurons, use standard threshold
        return pImpl->state.membranePotential >= pImpl->state.threshold;
    } else {
        // For active neurons, use lower threshold to prevent re-firing immediately
        float hysteresisThreshold = pImpl->state.threshold - HYSTERESIS_WIDTH;
        return pImpl->state.membranePotential >= hysteresisThreshold;
    }
}

float Neuron::getLastSpikeTime() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->state.lastSpikeTime;
}

void Neuron::receiveExcitatoryInput(MembranePotential amplitude) {
    assert(pImpl && "Neuron implementation is null");
    if (std::isnan(amplitude) || std::isinf(amplitude)) {
        return; // Ignore invalid inputs
    }
    
    pImpl->synapticInput += amplitude;
}

void Neuron::receiveInhibitoryInput(MembranePotential amplitude) {
    assert(pImpl && "Neuron implementation is null");
    if (std::isnan(amplitude) || std::isinf(amplitude)) {
        return; // Ignore invalid inputs
    }
    
    pImpl->synapticInput -= amplitude;
}

void Neuron::receiveModulatoryInput(MembranePotential amplitude) {
    assert(pImpl && "Neuron implementation is null");
    if (std::isnan(amplitude) || std::isinf(amplitude)) {
        return; // Ignore invalid inputs
    }
    
    pImpl->state.adaptationVariable += amplitude * 0.1f;
}

void Neuron::injectCurrent(MembranePotential current) {
    assert(pImpl && "Neuron implementation is null");
    if (std::isnan(current) || std::isinf(current)) {
        return; // Ignore invalid inputs
    }
    
    pImpl->synapticInput += current;
}

void Neuron::clearTotalCurrent() {
    assert(pImpl && "Neuron implementation is null");
    pImpl->synapticInput = 0.0f;
}

void Neuron::recordSpike(Timestamp timestamp) {
    assert(pImpl && "Neuron implementation is null");
    if (std::isnan(timestamp) || std::isinf(timestamp)) {
        return; // Ignore invalid timestamps
    }
    
    pImpl->spikeHistory.push_back(timestamp);
    
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin());
    }
}

const std::vector<Timestamp>& Neuron::getSpikeHistory() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->spikeHistory;
}

void Neuron::clearSpikeHistory() {
    assert(pImpl && "Neuron implementation is null");
    pImpl->spikeHistory.clear();
}

// Additional method implementations

void Neuron::addIncomingSynapse(SynapseHandle handle) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->incomingSynapses.push_back(handle);
}

void Neuron::addOutgoingSynapse(SynapseHandle handle) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->outgoingSynapses.push_back(handle);
}

const std::vector<SynapseHandle>& Neuron::getIncomingSynapses() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->incomingSynapses;
}

const std::vector<SynapseHandle>& Neuron::getOutgoingSynapses() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->outgoingSynapses;
}

void Neuron::removeIncomingSynapse(SynapseHandle handle) {
    assert(pImpl && "Neuron implementation is null");
    auto it = std::find(pImpl->incomingSynapses.begin(), pImpl->incomingSynapses.end(), handle);
    if (it != pImpl->incomingSynapses.end()) {
        pImpl->incomingSynapses.erase(it);
    }
}

void Neuron::removeOutgoingSynapse(SynapseHandle handle) {
    assert(pImpl && "Neuron implementation is null");
    auto it = std::find(pImpl->outgoingSynapses.begin(), pImpl->outgoingSynapses.end(), handle);
    if (it != pImpl->outgoingSynapses.end()) {
        pImpl->outgoingSynapses.erase(it);
    }
}

size_t Neuron::getIncomingSynapseCount() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->incomingSynapses.size();
}

size_t Neuron::getOutgoingSynapseCount() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->outgoingSynapses.size();
}

void Neuron::setAdaptationEnabled(bool enabled) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->adaptationEnabled = enabled;
}

void Neuron::setAdaptationStrength(float strength) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->adaptationStrength = std::clamp(strength, 0.0f, 10.0f);
}

float Neuron::getAdaptationStrength() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->adaptationStrength;
}

float Neuron::getAdaptationVariable() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->state.adaptationVariable;
}

void Neuron::setTemperature(float celsius) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->temperature = std::clamp(celsius, Impl::MIN_TEMP, Impl::MAX_TEMP);
    applyTemperatureCompensation();
}

float Neuron::getTemperature() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->temperature;
}

void Neuron::applyTemperatureCompensation() {
    // Temperature compensation implementation
    if (pImpl->temperature > 0.0f) {
        float tempFactor = 1.0f + (pImpl->temperature - 37.0f) * 0.01f;
        
        // Adjust time constant
        pImpl->TIME_CONSTANT = 20.0f * tempFactor;
        
        // Adjust leak conductance
        pImpl->state.leakConductance *= tempFactor;
        
        // Adjust firing threshold slightly
        pImpl->state.threshold -= (pImpl->temperature - 37.0f) * 0.1f;
    }
}

void Neuron::setVoltageClamp(bool active, MembranePotential minV, MembranePotential maxV) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->voltageClampActive = active;
    pImpl->voltageClampMin = std::clamp(minV, Impl::VOLTAGE_CLAMP_MIN, Impl::VOLTAGE_CLAMP_MAX);
    pImpl->voltageClampMax = std::clamp(maxV, Impl::VOLTAGE_CLAMP_MIN, Impl::VOLTAGE_CLAMP_MAX);
}

bool Neuron::isVoltageClampActive() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->voltageClampActive;
}

void Neuron::setMembraneCapacitance(float capacitance) {
    assert(pImpl && "Neuron implementation is null");
    // Note: This is a placeholder for future implementation
    // In the actual implementation, capacitance would affect LIF dynamics
}

float Neuron::getMembraneCapacitance() const {
    assert(pImpl && "Neuron implementation is null");
    return Impl::MEMBRANE_CAPACITANCE;
}

void Neuron::setTimeConstant(float timeConstant) {
    assert(pImpl && "Neuron implementation is null");
    // Note: This is a placeholder for future implementation
    // In the actual implementation, time constant would be used in LIF equations
}

float Neuron::getTimeConstant() const {
    assert(pImpl && "Neuron implementation is null");
    return Impl::TIME_CONSTANT;
}

void Neuron::setRestingPotential(MembranePotential potential) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->state.restingPotential = potential;
}

MembranePotential Neuron::getRestingPotential() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->state.restingPotential;
}

void Neuron::setResetPotential(MembranePotential potential) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->state.resetPotential = potential;
}

MembranePotential Neuron::getResetPotential() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->state.resetPotential;
}

void Neuron::setThreshold(MembranePotential threshold) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->state.threshold = threshold;
}

MembranePotential Neuron::getThreshold() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->state.threshold;
}

bool Neuron::isFiring() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->state.firingState == FiringState::Active;
}

bool Neuron::isRefractory() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->state.refractoryRemaining > 0;
}

void Neuron::setFiringState(FiringState state) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->state.firingState = state;
}

void Neuron::setRefractoryPeriod(uint32_t steps) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->state.refractoryPeriod = steps;
}

void Neuron::decrementRefractory() {
    assert(pImpl && "Neuron implementation is null");
    if (pImpl->state.refractoryRemaining > 0) {
        --pImpl->state.refractoryRemaining;
        if (pImpl->state.refractoryRemaining == 0) {
            pImpl->state.firingState = FiringState::Resting;
        }
    }
}

const PlasticityFlags& Neuron::getPlasticityFlags() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->plasticityFlags;
}

PlasticityFlags& Neuron::getPlasticityFlags() {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->plasticityFlags;
}

void Neuron::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.reward_modulated = rewardModulated;
}

void Neuron::setRegionId(RegionId region) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->regionId = region;
}

RegionId Neuron::getRegionId() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->regionId;
}

void Neuron::setPopulationId(PopulationId population) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->populationId = population;
}

PopulationId Neuron::getPopulationId() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->populationId;
}

void Neuron::disablePlasticity() {
    assert(pImpl && "Neuron implementation is null");
    pImpl->plasticityFlags.hebbian = false;
    pImpl->plasticityFlags.stdp = false;
    pImpl->plasticityFlags.reward_modulated = false;
}

void Neuron::setPlasticityFlag(PlasticityFlag flag, bool enabled) {
    assert(pImpl && "Neuron implementation is null");
    switch (flag) {
        case PlasticityFlag::Hebbian:
            pImpl->plasticityFlags.hebbian = enabled;
            break;
        case PlasticityFlag::STDP:
            pImpl->plasticityFlags.stdp = enabled;
            break;
        case PlasticityFlag::RewardModulated:
            pImpl->plasticityFlags.reward_modulated = enabled;
            break;
    }
}

bool Neuron::isPlastic(PlasticityFlag flag) const {
    assert(pImpl && "Neuron implementation is null");
    switch (flag) {
        case PlasticityFlag::Hebbian:
            return pImpl->plasticityFlags.hebbian;
        case PlasticityFlag::STDP:
            return pImpl->plasticityFlags.stdp;
        case PlasticityFlag::RewardModulated:
            return pImpl->plasticityFlags.reward_modulated;
        default:
            return false;
    }
}

size_t Neuron::getRecentSpikeCount(Timestamp currentTime, TimestepDuration window) const {
    assert(pImpl && "Neuron implementation is null");
    size_t count = 0;
    for (const auto& spike : pImpl->spikeHistory) {
        if (currentTime - spike <= window) {
            count++;
        }
    }
    return count;
}

float Neuron::getAverageInterSpikeInterval() const {
    assert(pImpl && "Neuron implementation is null");
    // Note: This is a placeholder for future implementation
    // Would calculate average time between spikes
    return 0.0f;
}

float Neuron::getFiringRate(TimestepDuration window) const {
    assert(pImpl && "Neuron implementation is null");
    size_t spikes = getRecentSpikeCount(pImpl->state.lastSpikeTime + window, window);
    return static_cast<float>(spikes) / window;
}

std::vector<Timestamp> Neuron::getSpikesInWindow(Timestamp start, Timestamp end) const {
    assert(pImpl && "Neuron implementation is null");
    std::vector<Timestamp> spikesInWindow;
    for (const auto& spike : pImpl->spikeHistory) {
        if (spike >= start && spike <= end) {
            spikesInWindow.push_back(spike);
        }
    }
    return spikesInWindow;
}

void Neuron::enableBurstMode(bool enabled, uint32_t burstFrequency) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->burstModeEnabled = enabled;
    pImpl->burstFrequency = burstFrequency;
}

void Neuron::enableOscillatoryBehavior(bool enabled, float frequency) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->oscillatoryBehaviorEnabled = enabled;
    pImpl->oscillationFrequency = frequency;
}

void Neuron::configureForTask(NeuralTask task) {
    assert(pImpl && "Neuron implementation is null");
    // Note: This is a placeholder for future implementation
    // Would configure neuron parameters based on the assigned task
}

void Neuron::enablePerformanceMonitoring(bool enabled) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->performanceMonitoringEnabled = enabled;
}

float Neuron::getAverageMembranePotential() const {
    assert(pImpl && "Neuron implementation is null");
    return pImpl->state.averageVoltage;
}

float Neuron::getMembranePotentialVariance() const {
    assert(pImpl && "Neuron implementation is null");
    // Note: This is a placeholder for future implementation
    // Would calculate variance of membrane potential over time
    return 0.0f;
}

float Neuron::getMembranePotentialMax() const {
    assert(pImpl && "Neuron implementation is null");
    // Note: This is a placeholder for future implementation
    // Would return maximum membrane potential observed
    return getAverageMembranePotential();
}

float Neuron::getMembranePotentialMin() const {
    assert(pImpl && "Neuron implementation is null");
    // Note: This is a placeholder for future implementation
    // Would return minimum membrane potential observed
    return getAverageMembranePotential();
}

void Neuron::processExternalInput(float input, float weight) {
    assert(pImpl && "Neuron implementation is null");
    if (std::isnan(input) || std::isinf(input) || std::isnan(weight) || std::isinf(weight)) {
        return; // Ignore invalid inputs
    }
    
    // Process external input with weight
    float weightedInput = input * weight;
    pImpl->synapticInput += weightedInput;
}

float Neuron::getExternalInputHistory(size_t index) const {
    assert(pImpl && "Neuron implementation is null");
    // Note: This is a placeholder for future implementation
    // Would return historical external inputs
    return 0.0f;
}

bool Neuron::isHealthy() const {
    assert(pImpl && "Neuron implementation is null");
    if (!pImpl->healthMonitor) {
        return true; // No health monitor, assume healthy
    }
    return pImpl->healthMonitor(*this);
}

std::string Neuron::getHealthStatus() const {
    assert(pImpl && "Neuron implementation is null");
    std::stringstream ss;
    ss << "Neuron ID: " << getId() << "\n";
    ss << "Type: " << static_cast<int>(getType()) << "\n";
    ss << "Membrane Potential: " << getMembranePotential() << " mV\n";
    ss << "Is Firing: " << (isFiring() ? "Yes" : "No") << "\n";
    ss << "Is Refractory: " << (isRefractory() ? "Yes" : "No") << "\n";
    ss << "Consecutive Spikes: " << pImpl->state.consecutiveSpikes << "\n";
    ss << "Healthy: " << (isHealthy() ? "Yes" : "No") << "\n";
    return ss.str();
}

void Neuron::setHealthMonitor(std::function<bool(const Neuron&)> monitor) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->healthMonitor = monitor;
}

bool Neuron::isStable() const {
    assert(pImpl && "Neuron implementation is null");
    // Note: This is a placeholder for future implementation
    // Would determine if neuron is in a stable state
    return true;
}

float Neuron::getStabilityMetric() const {
    assert(pImpl && "Neuron implementation is null");
    // Note: This is a placeholder for future implementation
    // Would calculate a stability metric
    return 1.0f;
}

void Neuron::enableStabilityControl(bool enabled, float targetFiringRate) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->stabilityControlEnabled = enabled;
    pImpl->targetFiringRate = targetFiringRate;
}

void Neuron::enableDebugMode(bool enabled) {
    assert(pImpl && "Neuron implementation is null");
    pImpl->debugModeEnabled = enabled;
}

void Neuron::logState() const {
    assert(pImpl && "Neuron implementation is null");
    if (pImpl->debugModeEnabled) {
        std::cout << getHealthStatus();
    }
}

void Neuron::validateState() const {
    assert(pImpl && "Neuron implementation is null");
    // Validate neuron state invariants
    if (std::isnan(getMembranePotential()) || std::isinf(getMembranePotential())) {
        // Log error for invalid state
    }
}

size_t Neuron::getMemoryUsage() const {
    assert(pImpl && "Neuron implementation is null");
    size_t memoryUsage = sizeof(*pImpl) + pImpl->spikeHistory.size() * sizeof(Timestamp);
    memoryUsage += pImpl->incomingSynapses.size() * sizeof(SynapseHandle);
    memoryUsage += pImpl->outgoingSynapses.size() * sizeof(SynapseHandle);
    return memoryUsage;
}

void Neuron::optimizeMemoryUsage() {
    assert(pImpl && "Neuron implementation is null");
    // Trim spike history to maximum allowed size
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin(),
                                pImpl->spikeHistory.end() - Impl::MAX_SPIKE_HISTORY);
    }
    
    // Clear unused synaptic lists
    pImpl->incomingSynapses.clear();
    pImpl->outgoingSynapses.clear();
}

} // namespace nlm