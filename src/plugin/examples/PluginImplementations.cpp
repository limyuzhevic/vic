/*
 * Plugin Implementation Files
 * 
 * This file contains the actual implementation of the example plugins.
 * These plugins demonstrate how to implement different types of plugins
 * for the NLM plugin system.
 */

#include "PluginImplementations.hpp"
#include "../../brain/Neuron.hpp"
#include "../../plasticity/PlasticityRule.hpp"
#include "../../neuromodulation/Neuromodulator.hpp"
#include "../../memory/WorkingMemory.hpp"
#include <cmath>
#include <stdexcept>

namespace nlm {
namespace plugin {

// LIF Neuron Plugin Implementation
class LIFNeuronPlugin::Impl {
public:
    float threshold;
    float restingPotential;
    float membraneTimeConstant;
    float refractoryPeriod;
    float leakConductance;
    
    Impl() : threshold(-55.0f), restingPotential(-70.0f),
             membraneTimeConstant(20.0f), refractoryPeriod(5.0f),
             leakConductance(10.0f) {
        NLM_LOG_INFO("LIFNeuronPlugin::Impl: Initialized with threshold=" + std::to_string(threshold) +
                     ", restingPotential=" + std::to_string(restingPotential) +
                     ", timeConstant=" + std::to_string(membraneTimeConstant) +
                     ", refractoryPeriod=" + std::to_string(refractoryPeriod) +
                     ", leakConductance=" + std::to_string(leakConductance));
    }
};

LIFNeuronPlugin::LIFNeuronPlugin() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("LIFNeuronPlugin: Initializing LIF neuron type plugin");
    metadata_.name = "LIF Neuron Plugin";
    metadata_.version = "1.0.0";
    metadata_.description = "Leaky Integrate-and-Fire neuron type plugin";
    metadata_.author = "NLM Team";
    metadata_.license = "MIT";
    metadata_.category = "neuron";
    metadata_.requiresRestart = false;
}

LIFNeuronPlugin::~LIFNeuronPlugin() {
    NLM_LOG_INFO("LIFNeuronPlugin: Destroying LIF neuron type plugin");
}

const std::string& LIFNeuronPlugin::getNeuronTypeName() const {
    static const std::string name = "LIF";
    return name;
}

float LIFNeuronPlugin::getDefaultThreshold() const {
    return pImpl->threshold;
}

float LIFNeuronPlugin::getDefaultRestingPotential() const {
    return pImpl->restingPotential;
}

float LIFNeuronPlugin::getDefaultMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

float LIFNeuronPlugin::getDefaultRefractoryPeriod() const {
    return pImpl->refractoryPeriod;
}

std::unique_ptr<class Neuron> LIFNeuronPlugin::createNeuron(NeuronId id) {
    auto neuron = std::make_unique<class Neuron>(id);
    neuron->setType(NeuronType::LIF);
    neuron->setThreshold(getDefaultThreshold());
    neuron->setRestingPotential(getDefaultRestingPotential());
    
    // Configure neuron parameters
    neuron->setLeakConductance(pImpl->leakConductance);
    neuron->setRefractoryPeriod(static_cast<uint32_t>(pImpl->refractoryPeriod));
    
    NLM_LOG_INFO("LIFNeuronPlugin: Created LIF neuron with ID " + std::to_string(id));
    return neuron;
}

bool LIFNeuronPlugin::updateNeuronState(class Neuron& neuron, TimestepDuration dt) {
    // LIF dynamics: dV/dt = (-V + I_leak + I_syn + I_injected) / tau
    NeuronState& state = neuron.getState();
    
    float tau = 1.0f / pImpl->membraneTimeConstant; // time constant
    
    // Calculate membrane potential change
    float dV = (-state.getMembranePotential() +
                neuron.getTotalCurrent() - 
                state.getMembranePotential() + 2.0f) * tau;
    
    state.setMembranePotential(state.getMembranePotential() + dV * dt);
    
    // Update firing rate
    if (state.isFiring()) {
        state.setFiringRate(1000.0f / dt); // spikes per second
    } else {
        state.setFiringRate(0.0f);
    }
    
    return true;
}

bool LIFNeuronPlugin::checkForSpike(class Neuron& neuron) const {
    const NeuronState& state = neuron.getState();
    return state.isFiring();
}

void LIFNeuronPlugin::resetNeuron(class Neuron& neuron) {
    NeuronState& state = neuron.getState();
    state.setMembranePotential(getDefaultRestingPotential());
    state.setFiringState(FiringState::Resting);
    state.setFiringRate(0.0f);
    state.clearSpikeHistory();
    neuron.clearTotalCurrent();
}

// AdaptiveExponential Neuron Plugin Implementation
class AdaptiveExponentialNeuronPlugin::Impl {
public:
    float threshold;
    float restingPotential;
    float membraneTimeConstant;
    float refractoryPeriod;
    float leakConductance;
    float adaptationVariable;
    
    Impl() : threshold(-50.0f), restingPotential(-65.0f),
             membraneTimeConstant(10.0f), refractoryPeriod(2.0f),
             leakConductance(8.0f), adaptationVariable(0.0f) {
        NLM_LOG_INFO("AdaptiveExponentialNeuronPlugin::Impl: Initialized with threshold=" + std::to_string(threshold) +
                     ", restingPotential=" + std::to_string(restingPotential) +
                     ", timeConstant=" + std::to_string(membraneTimeConstant) +
                     ", refractoryPeriod=" + std::to_string(refractoryPeriod) +
                     ", leakConductance=" + std::to_string(leakConductance));
    }
};

AdaptiveExponentialNeuronPlugin::AdaptiveExponentialNeuronPlugin() 
    : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("AdaptiveExponentialNeuronPlugin: Initializing adaptive-exponential neuron type plugin");
    metadata_.name = "Adaptive Exponential Neuron Plugin";
    metadata_.version = "1.0.0";
    metadata_.description = "Adaptive exponential integrate-and-fire neuron type plugin";
    metadata_.author = "NLM Team";
    metadata_.license = "MIT";
    metadata_.category = "neuron";
    metadata_.requiresRestart = false;
}

AdaptiveExponentialNeuronPlugin::~AdaptiveExponentialNeuronPlugin() {
    NLM_LOG_INFO("AdaptiveExponentialNeuronPlugin: Destroying adaptive-exponential neuron type plugin");
}

const std::string& AdaptiveExponentialNeuronPlugin::getNeuronTypeName() const {
    static const std::string name = "AdaptiveExponential";
    return name;
}

float AdaptiveExponentialNeuronPlugin::getDefaultThreshold() const {
    return pImpl->threshold;
}

float AdaptiveExponentialNeuronPlugin::getDefaultRestingPotential() const {
    return pImpl->restingPotential;
}

float AdaptiveExponentialNeuronPlugin::getDefaultMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

float AdaptiveExponentialNeuronPlugin::getDefaultRefractoryPeriod() const {
    return pImpl->refractoryPeriod;
}

std::unique_ptr<class Neuron> AdaptiveExponentialNeuronPlugin::createNeuron(NeuronId id) {
    auto neuron = std::make_unique<class Neuron>(id);
    neuron->setType(NeuronType::AdaptiveExponential);
    neuron->setThreshold(getDefaultThreshold());
    neuron->setRestingPotential(getDefaultRestingPotential());
    
    // Configure adaptive exponential neuron parameters
    neuron->setLeakConductance(pImpl->leakConductance);
    neuron->setRefractoryPeriod(static_cast<uint32_t>(pImpl->refractoryPeriod));
    neuron->getState().setAdaptationVariable(pImpl->adaptationVariable);
    
    NLM_LOG_INFO("AdaptiveExponentialNeuronPlugin: Created adaptive-exponential neuron with ID " + std::to_string(id));
    return neuron;
}

bool AdaptiveExponentialNeuronPlugin::updateNeuronState(class Neuron& neuron, TimestepDuration dt) {
    NeuronState& state = neuron.getState();
    
    float tau = 1.0f / pImpl->membraneTimeConstant; // time constant
    float V = state.getMembranePotential();
    float a = 4.0f; // adaptation variable scaling
    float b = 80.0f; // adaptation variable scaling
    float Vt = -50.0f; // threshold for spike initiation
    
    // Adaptive exponential integrate-and-fire dynamics
    float dV = (-V + neuron.getTotalCurrent() - a * (state.getAdaptationVariable() - b) + 
                exp((V - Vt) / 0.1f)) * tau;
    
    state.setMembranePotential(V + dV * dt);
    
    // Update adaptation variable
    state.setAdaptationVariable(state.getAdaptationVariable() + (0.1f * dt));
    
    // Update firing rate
    if (state.isFiring()) {
        state.setFiringRate(1000.0f / dt);
        state.setAdaptationVariable(state.getAdaptationVariable() + 100.0f);
    } else {
        state.setFiringRate(0.0f);
    }
    
    return true;
}

bool AdaptiveExponentialNeuronPlugin::checkForSpike(class Neuron& neuron) const {
    const NeuronState& state = neuron.getState();
    return state.isFiring();
}

void AdaptiveExponentialNeuronPlugin::resetNeuron(class Neuron& neuron) {
    NeuronState& state = neuron.getState();
    state.setMembranePotential(getDefaultRestingPotential());
    state.setFiringState(FiringState::Resting);
    state.setFiringRate(0.0f);
    state.setAdaptationVariable(0.0f);
    state.clearSpikeHistory();
    neuron.clearTotalCurrent();
}

// STDP Plugin Implementation
class STDPPlugin::Impl {
public:
    float ltpWeight;
    float ltdWeight;
    float timeConstant;
    float minWeight;
    float maxWeight;
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             minWeight(-1.0f), maxWeight(1.0f) {
        NLM_LOG_INFO("STDPPlugin::Impl: Initialized with LTP weight=" + std::to_string(ltpWeight) +
                     ", LTD weight=" + std::to_string(ltdWeight) +
                     ", time constant=" + std::to_string(timeConstant) +
                     ", weight bounds=[" + std::to_string(minWeight) + "," + std::to_string(maxWeight) + "]");
    }
};

STDPPlugin::STDPPlugin() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("STDPPlugin: Initializing STDP plasticity rule plugin");
    metadata_.name = "STDP Plasticity Rule Plugin";
    metadata_.version = "1.0.0";
    metadata_.description = "Spike-Timing Dependent Plasticity rule based on precise spike timing differences";
    metadata_.author = "NLM Team";
    metadata_.license = "MIT";
    metadata_.category = "plasticity";
    metadata_.requiresRestart = false;
}

STDPPlugin::~STDPPlugin() {
    NLM_LOG_INFO("STDPPlugin: Destroying STDP plasticity rule plugin");
}

const std::string& STDPPlugin::getRuleName() const {
    static const std::string name = "STDP";
    return name;
}

const std::string& STDPPlugin::getRuleDescription() const {
    static const std::string description = "Spike-Timing Dependent Plasticity rule based on precise spike timing differences";
    return description;
}

PlasticityRuleType STDPPlugin::getSupportedRuleType() const {
    return PlasticityRuleType::STDP;
}

bool STDPPlugin::updatePlasticity(class Synapse& synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    // Simplified STDP implementation
    if (preSpikes.empty() || postSpikes.empty()) {
        return true;
    }
    
    float totalDelta = 0.0f;
    
    // For each pre-post spike pair, compute STDP weight change
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            
            if (std::abs(dt) < 1e-6f) {
                continue; // Negligible effect for simultaneous spikes
            }
            
            if (dt > 0) {
                // LTP: pre before post
                float delta = pImpl->ltpWeight * std::exp(-dt / pImpl->timeConstant);
                totalDelta += delta;
            } else {
                // LTD: post before pre
                float delta = -pImpl->ltdWeight * std::exp(dt / pImpl->timeConstant);
                totalDelta += delta;
            }
        }
    }
    
    // Apply the total weight change
    return applyWeightChange(synapse, totalDelta);
}

bool STDPPlugin::applyWeightChange(class Synapse& synapse, SynapticWeight delta) {
    float currentWeight = synapse.getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply bounds checking
    if (newWeight < pImpl->minWeight) {
        newWeight = pImpl->minWeight;
    } else if (newWeight > pImpl->maxWeight) {
        newWeight = pImpl->maxWeight;
    }
    
    synapse.setWeight(newWeight);
    return true;
}

float STDPPlugin::getEligibilityTrace(class Synapse& synapse) const {
    return synapse.getEligibilityTrace();
}

void STDPPlugin::updateEligibilityTrace(class Synapse& synapse, float delta, TimestepDuration dt) {
    float currentTrace = synapse.getEligibilityTrace();
    float newTrace = currentTrace + delta;
    
    // Clamp eligibility trace
    newTrace = std::clamp(newTrace, -1.0f, 1.0f);
    synapse.setEligibilityTrace(newTrace);
    
    // Decay eligibility trace
    float decayFactor = std::exp(-static_cast<float>(dt) / 1000.0f); // 1 second decay
    synapse.setEligibilityTrace(newTrace * decayFactor);
}

void STDPPlugin::resetEligibilityTrace(class Synapse& synapse) {
    synapse.setEligibilityTrace(0.0f);
}

// Hebbian Plugin Implementation
class HebbianPlugin::Impl {
public:
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f) {
        NLM_LOG_INFO("HebbianPlugin::Impl: Initialized with learning rate=" + std::to_string(learningRate) +
                     ", weight bounds=[" + std::to_string(minWeight) + "," + std::to_string(maxWeight) + "]");
    }
};

HebbianPlugin::HebbianPlugin() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("HebbianPlugin: Initializing Hebbian plasticity rule plugin");
    metadata_.name = "Hebbian Plasticity Rule Plugin";
    metadata_.version = "1.0.0";
    metadata_.description = "Correlation-based plasticity rule: 'neurons that fire together, wire together'";
    metadata_.author = "NLM Team";
    metadata_.license = "MIT";
    metadata_.category = "plasticity";
    metadata_.requiresRestart = false;
}

HebbianPlugin::~HebbianPlugin() {
    NLM_LOG_INFO("HebbianPlugin: Destroying Hebbian plasticity rule plugin");
}

const std::string& HebbianPlugin::getRuleName() const {
    static const std::string name = "Hebbian";
    return name;
}

const std::string& HebbianPlugin::getRuleDescription() const {
    static const std::string description = "Correlation-based plasticity rule: 'neurons that fire together, wire together'";
    return description;
}

PlasticityRuleType HebbianPlugin::getSupportedRuleType() const {
    return PlasticityRuleType::Hebbian;
}

bool HebbianPlugin::updatePlasticity(class Synapse& synapse,
                                     const std::vector<Timestamp>& preSpikes,
                                     const std::vector<Timestamp>& postSpikes,
                                     TimestepDuration dt) {
    if (preSpikes.empty() || postSpikes.empty()) {
        return true;
    }
    
    // Compute correlation count
    size_t correlationCount = 0;
    float temporalWindow = 100.0f; // 100ms temporal window
    
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < temporalWindow) {
                correlationCount++;
            }
        }
    }
    
    // Compute weight change
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply weight change
    return applyWeightChange(synapse, delta);
}

bool HebbianPlugin::applyWeightChange(class Synapse& synapse, SynapticWeight delta) {
    float currentWeight = synapse.getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply bounds checking
    if (newWeight < pImpl->minWeight) {
        newWeight = pImpl->minWeight;
    } else if (newWeight > pImpl->maxWeight) {
        newWeight = pImpl->maxWeight;
    }
    
    synapse.setWeight(newWeight);
    return true;
}

float HebbianPlugin::getEligibilityTrace(class Synapse& synapse) const {
    return synapse.getEligibilityTrace();
}

void HebbianPlugin::updateEligibilityTrace(class Synapse& synapse, float delta, TimestepDuration dt) {
    float currentTrace = synapse.getEligibilityTrace();
    float newTrace = currentTrace + delta;
    
    // Clamp eligibility trace
    newTrace = std::clamp(newTrace, -1.0f, 1.0f);
    synapse.setEligibilityTrace(newTrace);
}

void HebbianPlugin::resetEligibilityTrace(class Synapse& synapse) {
    synapse.setEligibilityTrace(0.0f);
}

// Dopamine Plugin Implementation
class DopaminePlugin::Impl {
public:
    float baselineLevel;
    float rewardSensitivity;
    float predictionErrorSensitivity;
    
    Impl() : baselineLevel(0.0f), rewardSensitivity(1.0f), predictionErrorSensitivity(1.0f) {
        NLM_LOG_INFO("DopaminePlugin::Impl: Initialized with baseline level=" + std::to_string(baselineLevel) +
                     ", reward sensitivity=" + std::to_string(rewardSensitivity) +
                     ", prediction error sensitivity=" + std::to_string(predictionErrorSensitivity));
    }
};

DopaminePlugin::DopaminePlugin() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("DopaminePlugin: Initializing dopamine neuromodulator plugin");
    metadata_.name = "Dopamine Neuromodulator Plugin";
    metadata_.version = "1.0.0";
    metadata_.description = "Reward and reinforcement learning signal neuromodulator";
    metadata_.author = "NLM Team";
    metadata_.license = "MIT";
    metadata_.category = "neuromodulation";
    metadata_.requiresRestart = false;
}

DopaminePlugin::~DopaminePlugin() {
    NLM_LOG_INFO("DopaminePlugin: Destroying dopamine neuromodulator plugin");
}

const std::string& DopaminePlugin::getModulatorName() const {
    static const std::string name = "Dopamine";
    return name;
}

NeuromodulatorType DopaminePlugin::getModulatorType() const {
    return NeuromodulatorType::Dopamine;
}

float DopaminePlugin::getDefaultConcentration() const {
    return pImpl->baselineLevel;
}

float DopaminePlugin::getEffectOnPlasticity() const {
    return 1.2f; // 20% enhancement
}

float DopaminePlugin::getEffectOnExcitation() const {
    return 1.1f; // 10% enhancement
}

float DopaminePlugin::getEffectOnInhibition() const {
    return 0.9f; // 10% suppression
}

bool DopaminePlugin::updateModulatorLevel(TimestepDuration dt) {
    // Simple dopamine dynamics: increase with reward, decay otherwise
    float currentLevel = getLevel();
    
    // Decay towards baseline
    float newLevel = currentLevel * 0.95f + 0.05f * pImpl->baselineLevel;
    
    setLevel(newLevel);
    return true;
}

void DopaminePlugin::applyModulatoryEffects(class Brain& brain) {
    // Apply dopamine effects to plasticity rules
    float plasticityFactor = getEffectOnPlasticity();
    
    NLM_LOG_DEBUG("DopaminePlugin: Applying plasticity factor " + std::to_string(plasticityFactor) + " to brain");
    
    // This would normally modulate plasticity rule strength
    // For simplicity, we just log the effect
}

void DopaminePlugin::resetModulator() {
    setLevel(0.0f);
}

void DopaminePlugin::signalReward(float reward) {
    float currentLevel = getLevel();
    
    // Increase dopamine level based on reward
    float rewardBoost = std::min(reward * pImpl->rewardSensitivity, 10.0f); // Cap at 10
    setLevel(currentLevel + rewardBoost);
    
    NLM_LOG_INFO("DopaminePlugin: Reward signal of " + std::to_string(reward) + " produced dopamine level " + std::to_string(getLevel()));
}

void DopaminePlugin::signalRewardPredictionError(float error) {
    float currentLevel = getLevel();
    
    // Negative prediction error reduces dopamine (punishment)
    float errorEffect = -std::min(error * pImpl->predictionErrorSensitivity, 5.0f);
    setLevel(currentLevel + errorEffect);
    
    NLM_LOG_INFO("DopaminePlugin: Prediction error of " + std::to_string(error) + 
                 " produced dopamine level " + std::to_string(getLevel()));
}

// Serotonin Plugin Implementation
class SerotoninPlugin::Impl {
public:
    float baselineLevel;
    float moodInfluence;
    float impulsivitySuppression;
    
    Impl() : baselineLevel(0.0f), moodInfluence(1.0f), impulsivitySuppression(0.8f) {
        NLM_LOG_INFO("SerotoninPlugin::Impl: Initialized with baseline level=" + std::to_string(baselineLevel) +
                     ", mood influence=" + std::to_string(moodInfluence) +
                     ", impulsivity suppression=" + std::to_string(impulsivitySuppression));
    }
};

SerotoninPlugin::SerotoninPlugin() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("SerotoninPlugin: Initializing serotonin neuromodulator plugin");
    metadata_.name = "Serotonin Neuromodulator Plugin";
    metadata_.version = "1.0.0";
    metadata_.description = "Mood and impulsivity regulation neuromodulator";
    metadata_.author = "NLM Team";
    metadata_.license = "MIT";
    metadata_.category = "neuromodulation";
    metadata_.requiresRestart = false;
}

SerotoninPlugin::~SerotoninPlugin() {
    NLM_LOG_INFO("SerotoninPlugin: Destroying serotonin neuromodulator plugin");
}

const std::string& SerotoninPlugin::getModulatorName() const {
    static const std::string name = "Serotonin";
    return name;
}

NeuromodulatorType SerotoninPlugin::getModulatorType() const {
    return NeuromodulatorType::Serotonin;
}

float SerotoninPlugin::getDefaultConcentration() const {
    return pImpl->baselineLevel;
}

float SerotoninPlugin::getEffectOnPlasticity() const {
    return 0.8f; // 20% suppression
}

float SerotoninPlugin::getEffectOnExcitation() const {
    return 1.05f; // 5% enhancement
}

float SerotoninPlugin::getEffectOnInhibition() const {
    return 1.1f; // 10% enhancement
}

bool SerotoninPlugin::updateModulatorLevel(TimestepDuration dt) {
    // Simple serotonin dynamics: slow response, mood-related
    float currentLevel = getLevel();
    
    // Decay with slow time constant
    float newLevel = currentLevel * 0.98f + 0.02f * pImpl->baselineLevel;
    
    setLevel(newLevel);
    return true;
}

void SerotoninPlugin::applyModulatoryEffects(class Brain& brain) {
    // Apply serotonin effects to behavior
    float plasticityFactor = getEffectOnPlasticity();
    
    NLM_LOG_DEBUG("SerotoninPlugin: Applying plasticity factor " + std::to_string(plasticityFactor) + " to brain");
    
    // Serotonin typically reduces impulsivity and enhances avoidance learning
}

void SerotoninPlugin::resetModulator() {
    setLevel(0.0f);
}

void SerotoninPlugin::signalMoodChange(float moodLevel) {
    float currentLevel = getLevel();
    
    // Update serotonin based on mood
    setLevel(currentLevel + moodLevel * pImpl->moodInfluence * 0.5f);
    
    NLM_LOG_INFO("SerotoninPlugin: Mood change of " + std::to_string(moodLevel) + 
                 " produced serotonin level " + std::to_string(getLevel()));
}

// WorkingMemory Plugin Implementation
class WorkingMemoryPlugin::Impl {
public:
    size_t maxCapacity;
    float activationDecay;
    float updateFrequency;
    std::map<MemoryId, float> memoryActivity;
    std::map<MemoryId, MemoryEntry> memoryEntries;
    std::vector<MemoryId> activeMemoryIds;
    
    Impl() : maxCapacity(100), activationDecay(0.95f), updateFrequency(10.0f) {
        NLM_LOG_INFO("WorkingMemoryPlugin::Impl: Initialized with max capacity=" + std::to_string(maxCapacity) +
                     ", activation decay=" + std::to_string(activationDecay) +
                     ", update frequency=" + std::to_string(updateFrequency));
    }
};

WorkingMemoryPlugin::WorkingMemoryPlugin() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("WorkingMemoryPlugin: Initializing working memory system plugin");
    metadata_.name = "Working Memory Plugin";
    metadata_.version = "1.0.0";
    metadata_.description = "Short-term memory system for active information";
    metadata_.author = "NLM Team";
    metadata_.license = "MIT";
    metadata_.category = "memory";
    metadata_.requiresRestart = false;
}

WorkingMemoryPlugin::~WorkingMemoryPlugin() {
    NLM_LOG_INFO("WorkingMemoryPlugin: Destroying working memory system plugin");
}

const std::string& WorkingMemoryPlugin::getMemorySystemName() const {
    static const std::string name = "WorkingMemory";
    return name;
}

MemorySystemType WorkingMemoryPlugin::getMemorySystemType() const {
    return MemorySystemType::WorkingMemory;
}

size_t WorkingMemoryPlugin::getMaxCapacity() const {
    return pImpl->maxCapacity;
}

bool WorkingMemoryPlugin::storeMemory(const MemoryEntry& entry) {
    // Store working memory entry
    pImpl->memoryEntries[entry.id] = entry;
    pImpl->activeMemoryIds.push_back(entry.id);
    
    // Keep memory active for a limited time
    pImpl->memoryActivity[entry.id] = 1.0f;
    
    NLM_LOG_DEBUG("WorkingMemoryPlugin: Stored working memory entry " + std::to_string(entry.id));
    return true;
}

bool WorkingMemoryPlugin::retrieveMemory(MemoryId id, MemoryEntry& entry) const {
    auto it = pImpl->memoryEntries.find(id);
    if (it == pImpl->memoryEntries.end()) {
        return false;
    }
    
    entry = it->second;
    return true;
}

bool WorkingMemoryPlugin::updateMemory(const MemoryEntry& entry) {
    auto it = pImpl->memoryEntries.find(entry.id);
    if (it == pImpl->memoryEntries.end()) {
        return false;
    }
    
    it->second = entry;
    pImpl->memoryActivity[entry.id] = 1.0f; // Reactivate memory
    
    NLM_LOG_DEBUG("WorkingMemoryPlugin: Updated working memory entry " + std::to_string(entry.id));
    return true;
}

bool WorkingMemoryPlugin::deleteMemory(MemoryId id) {
    auto it = pImpl->memoryEntries.find(id);
    if (it == pImpl->memoryEntries.end()) {
        return false;
    }
    
    pImpl->memoryEntries.erase(it);
    pImpl->memoryActivity.erase(id);
    
    // Remove from active list
    pImpl->activeMemoryIds.erase(
        std::remove(pImpl->activeMemoryIds.begin(), 
                   pImpl->activeMemoryIds.end(), id),
        pImpl->activeMemoryIds.end()
    );
    
    NLM_LOG_DEBUG("WorkingMemoryPlugin: Deleted working memory entry " + std::to_string(id));
    return true;
}

bool WorkingMemoryPlugin::clearMemory() {
    pImpl->memoryEntries.clear();
    pImpl->memoryActivity.clear();
    pImpl->activeMemoryIds.clear();
    
    NLM_LOG_INFO("WorkingMemoryPlugin: Cleared all working memory entries");
    return true;
}

size_t WorkingMemoryPlugin::getMemoryUsage() const {
    return pImpl->memoryEntries.size();
}

float WorkingMemoryPlugin::getMemoryUtilization() const {
    float capacity = static_cast<float>(getMaxCapacity());
    float usage = static_cast<float>(getMemoryUsage());
    return (capacity > 0.0f) ? (usage / capacity) : 0.0f;
}

std::vector<MemoryId> WorkingMemoryPlugin::getAllMemoryIds() const {
    std::vector<MemoryId> ids;
    for (const auto& pair : pImpl->memoryEntries) {
        ids.push_back(pair.first);
    }
    return ids;
}

bool WorkingMemoryPlugin::isMemoryActive(MemoryId id) const {
    auto it = pImpl->memoryActivity.find(id);
    return (it != pImpl->memoryActivity.end()) && (it->second > 0.0f);
}

void WorkingMemoryPlugin::updateMemoryActivity(MemoryId id, float activity) {
    pImpl->memoryActivity[id] = activity;
}

float WorkingMemoryPlugin::getMemoryPriority(MemoryId id) const {
    auto it = pImpl->memoryActivity.find(id);
    return (it != pImpl->memoryActivity.end()) ? it->second : 0.0f;
}

}}} // namespace nlm::plugin
