#include "Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>
#include <thread>

namespace nlm {

// Generate deterministic seed for hash-based random initialization
static uint64_t deterministicHash(const std::string& seed, uint64_t seedValue = 5381ULL) {
    uint64_t hash = seedValue;
    for (char c : seed) {
        hash = hash * 131 + static_cast<uint64_t>(c);
    }
    return hash;
}

struct Synapse::Impl {
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId destinationNeuron;
    SynapticWeight weight;
    Delay delay;  // Synaptic delay in simulation steps
    
    // Synapse type
    SynapseType type;
    
    // Spike history for STDP
    std::vector<Timestamp> preSpikeHistory;
    std::vector<Timestamp> postSpikeHistory;
    
    // Plasticity state
    PlasticityFlags plasticityFlags;
    
    // Short-term plasticity state
    float shortTermDepression;  // Utilization factor (0-1)
    float shortTermFacilitation;  // Facilitation factor
    float lastPreSpikeTime;  // For short-term dynamics
    float lastPostSpikeTime;
    
    // Eligibility trace for reward-modulated learning
    float eligibilityTrace;
    
    // Synaptic efficacy (use-dependent modulation)
    float efficacy;
    
    // Weight bounds
    static constexpr float MIN_WEIGHT = -1.0f;
    static constexpr float MAX_WEIGHT = 1.0f;
    
    // Short-term plasticity parameters
    static constexpr float STP_FACILITATION_TAU = 100.0f;  // ms
    static constexpr float STP_DEPRESSION_TAU = 200.0f;  // ms
    static constexpr float STP_U_MAX = 1.0f;  // Max utilization
    
    static constexpr size_t MAX_SPIKE_HISTORY = 100;
    
    // Performance monitoring
    std::shared_ptr<PerformanceProfiler> performanceProfiler;
    mutable SynapsePerformanceMetrics perfMetrics;
};

// Centralized weight clamping for consistency across all plasticity rules
SynapticWeight Synapse::clampWeight(SynapticWeight weight) {
    return std::clamp(weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

Synapse::Synapse(SynapseId id, NeuronId source, NeuronId destination)
    : pImpl(new Impl) {
    pImpl->id = id;
    pImpl->sourceNeuron = source;
    pImpl->destinationNeuron = destination;
    pImpl->weight = 0.0f;
    pImpl->delay = 1;  // Default: 1 step delay
    pImpl->type = SynapseType::Excitatory;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    
    // Initialize performance metrics
    pImpl->perfMetrics.id = id;
    pImpl->perfMetrics.source = source;
    pImpl->perfMetrics.destination = destination;
    pImpl->perfMetrics.type = SynapseType::Excitatory;
    pImpl->perfMetrics.isExcitatory = true;
    pImpl->perfMetrics.isInhibitory = false;
}

Synapse::~Synapse() = default;

Synapse::Synapse(Synapse&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Synapse& Synapse::operator=(Synapse&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

// === CORE OPERATIONS ===

SynapseId Synapse::getId() const {
    return pImpl->id;
}

NeuronId Synapse::getSourceNeuron() const {
    return pImpl->sourceNeuron;
}

NeuronId Synapse::getDestinationNeuron() const {
    return pImpl->destinationNeuron;
}

SynapticWeight Synapse::getWeight() const {
    return pImpl->weight;
}

void Synapse::setWeight(SynapticWeight weight) {
    pImpl->weight = clampWeight(weight);
    
    // Record performance metrics
    recordWeightChange(pImpl->weight - weight);
    recordPerformanceMetrics();
}

void Synapse::addToWeight(SynapticWeight delta) {
    pImpl->weight += delta;
    // Use centralized clamping
    pImpl->weight = clampWeight(pImpl->weight);
    
    // Record performance metrics
    recordWeightChange(delta);
    recordPerformanceMetrics();
}

// === DELAY OPERATIONS ===

Delay Synapse::getDelay() const {
    return pImpl->delay;
}

void Synapse::setDelay(Delay delay) {
    pImpl->delay = delay;
}

// === TYPE OPERATIONS ===

SynapseType Synapse::getType() const {
    return pImpl->type;
}

void Synapse::setType(SynapseType type) {
    pImpl->type = type;
    pImpl->perfMetrics.type = type;
    pImpl->perfMetrics.isExcitatory = (type == SynapseType::Excitatory);
    pImpl->perfMetrics.isInhibitory = (type == SynapseType::Inhibitory);
}

// === SPIKE RECORDING ===

void Synapse::recordPreSpike(Timestamp timestamp) {
    pImpl->preSpikeHistory.push_back(timestamp);
    
    // Record spike metrics
    recordSpikeMetrics(true, timestamp);
    recordPerformanceMetrics();
}

void Synapse::recordPostSpike(Timestamp timestamp) {
    pImpl->postSpikeHistory.push_back(timestamp);
    
    // Record spike metrics
    recordSpikeMetrics(false, timestamp);
    recordPerformanceMetrics();
}

const std::vector<Timestamp>& Synapse::getPreSpikeHistory() const {
    return pImpl->preSpikeHistory;
}

const std::vector<Timestamp>& Synapse::getPostSpikeHistory() const {
    return pImpl->postSpikeHistory;
}

void Synapse::clearHistory() {
    pImpl->preSpikeHistory.clear();
    pImpl->postSpikeHistory.clear();
    
    // Update spike metrics
    pImpl->perfMetrics.preSpikeCount = 0;
    pImpl->perfMetrics.postSpikeCount = 0;
    pImpl->perfMetrics.spikeSynchronization = 0.0f;
    pImpl->perfMetrics.crossCorrelation = 0.0f;
    pImpl->perfMetrics.burstFrequency = 0.0f;
    
    recordPerformanceMetrics();
}

// === PLASTICITY OPERATIONS ===

const PlasticityFlags& Synapse::getPlasticityFlags() const {
    return pImpl->plasticityFlags;
}

PlasticityFlags& Synapse::getPlasticityFlags() {
    return pImpl->plasticityFlags;
}

void Synapse::enablePlasticity(bool hebbian, bool stdp, bool rewardModulated) {
    pImpl->plasticityFlags.hebbian = hebbian;
    pImpl->plasticityFlags.stdp = stdp;
    pImpl->plasticityFlags.rewardModulated = rewardModulated;
    
    pImpl->perfMetrics.hebbianEnabled = hebbian;
    pImpl->perfMetrics.stdpEnabled = stdp;
    pImpl->perfMetrics.rewardModulated = rewardModulated;
    
    recordPerformanceMetrics();
}

// === ELIGIBILITY TRACE ===

float Synapse::getEligibilityTrace() const {
    return pImpl->eligibilityTrace;
}

void Synapse::setEligibilityTrace(float trace) {
    pImpl->eligibilityTrace = trace;
    
    // Record plasticity metrics
    recordPlasticityMetrics(trace - pImpl->eligibilityTrace, pImpl->efficacy);
    recordPerformanceMetrics();
}

void Synapse::decayEligibilityTrace(float decayRate) {
    pImpl->eligibilityTrace *= decayRate;
}

// === SYNAPTIC EFFICACY ===

float Synapse::getEfficacy() const {
    return pImpl->efficacy;
}

void Synapse::setEfficacy(float efficacy) {
    pImpl->efficacy = efficacy;
    
    // Record efficacy metrics
    recordEfficacyMetrics(efficacy);
    recordPerformanceMetrics();
}

// === STEP UPDATE ===

void Synapse::step(Timestamp currentTime) {
    pImpl->stepCount++;
    
    // Update performance metrics
    pImpl->perfMetrics.updateCount++;
    pImpl->perfMetrics.lastUpdateTime = std::chrono::steady_clock::now();
    
    // Record performance metrics
    recordPerformanceMetrics();
}

// === RESET ===

void Synapse::reset() {
    pImpl->weight = 0.0f;
    pImpl->delay = 1;
    pImpl->type = SynapseType::Excitatory;
    pImpl->plasticityFlags = PlasticityFlags();
    pImpl->shortTermDepression = 1.0f;
    pImpl->shortTermFacilitation = 0.0f;
    pImpl->lastPreSpikeTime = -1.0f;
    pImpl->lastPostSpikeTime = -1.0f;
    pImpl->eligibilityTrace = 0.0f;
    pImpl->efficacy = 1.0f;
    
    clearHistory();
    
    // Reset performance metrics
    pImpl->perfMetrics.reset();
}

// === INITIALIZATION ===

void Synapse::initializeRandom(RandomGenerator& rng) {
    pImpl->weight = rng.uniformFloat(-1.0f, 1.0f);
    pImpl->delay = rng.randint(1, 5);
    
    // Randomly choose type
    int typeChoice = rng.randint(0, 3);
    if (typeChoice == 0) pImpl->type = SynapseType::Excitatory;
    else if (typeChoice == 1) pImpl->type = SynapseType::Inhibitory;
    else pImpl->type = SynapseType::Modulatory;
    
    pImpl->perfMetrics.type = pImpl->type;
    pImpl->perfMetrics.isExcitatory = (pImpl->type == SynapseType::Excitatory);
    pImpl->perfMetrics.isInhibitory = (pImpl->type == SynapseType::Inhibitory);
    
    pImpl->efficacy = rng.uniformFloat(0.5f, 1.5f);
    
    // Record performance metrics
    recordPerformanceMetrics();
}

// === PERFORMANCE MONITORING ===

void Synapse::setPerformanceProfiler(std::shared_ptr<PerformanceProfiler> profiler) {
    pImpl->performanceProfiler = profiler;
}

SynapsePerformanceMetrics Synapse::getPerformanceMetrics() const {
    return pImpl->perfMetrics;
}

void Synapse::recordPerformanceMetrics() {
    // Update spike metrics
    if (pImpl->preSpikeHistory.size() > pImpl->perfMetrics.preSpikeCount) {
        pImpl->perfMetrics.preSpikeCount = pImpl->preSpikeHistory.size();
    }
    
    if (pImpl->postSpikeHistory.size() > pImpl->perfMetrics.postSpikeCount) {
        pImpl->perfMetrics.postSpikeCount = pImpl->postSpikeHistory.size();
    }
    
    // Calculate spike synchronization
    if (!pImpl->preSpikeHistory.empty() && !pImpl->postSpikeHistory.empty()) {
        float crossCorr = calculateCrossCorrelation();
        pImpl->perfMetrics.crossCorrelation = crossCorr;
        
        // Calculate burst frequency
        pImpl->perfMetrics.burstFrequency = calculateBurstFrequency();
    }
    
    // Record to performance profiler if available
    if (pImpl->performanceProfiler) {
        auto summary = getPerformanceSummary();
        pImpl->performanceProfiler->recordSynapseProfile(
            std::to_string(pImpl->id), summary
        );
    }
}

void Synapse::recordWeightChange(SynapticWeight delta) {
    pImpl->perfMetrics.weightChange = delta;
    pImpl->perfMetrics.weightChangeRate = std::abs(delta);
    
    if (pImpl->perfMetrics.weight > pImpl->perfMetrics.maxWeight) {
        pImpl->perfMetrics.maxWeight = pImpl->perfMetrics.weight;
    }
    
    if (pImpl->perfMetrics.weight < pImpl->perfMetrics.minWeight) {
        pImpl->perfMetrics.minWeight = pImpl->perfMetrics.weight;
    }
    
    if (pImpl->perfMetrics.weight > 0.0f) {
        pImpl->perfMetrics.weightVariance = (pImpl->perfMetrics.weightVariance * (pImpl->perfMetrics.updateCount - 1) + (pImpl->perfMetrics.weight - pImpl->perfMetrics.averageWeight) * (pImpl->perfMetrics.weight - pImpl->perfMetrics.averageWeight)) / pImpl->perfMetrics.updateCount;
    }
}

void Synapse::recordSpikeMetrics(bool preSpike, Timestamp timestamp) {
    if (preSpike) {
        pImpl->perfMetrics.preSpikeCount++;
        
        // Calculate time since last spike
        if (pImpl->lastPreSpikeTime > 0.0f) {
            float isi = timestamp - pImpl->lastPreSpikeTime;
            pImpl->perfMetrics.timeSinceLastSpike = isi;
            
            if (pImpl->perfMetrics.averageInterSpikeInterval == 0.0f) {
                pImpl->perfMetrics.averageInterSpikeInterval = isi;
            } else {
                pImpl->perfMetrics.averageInterSpikeInterval = (pImpl->perfMetrics.averageInterSpikeInterval + isi) / 2.0f;
            }
            
            if (isi > 0.0f) {
                pImpl->perfMetrics.isiCoefficientOfVariation = std::sqrt(pImpl->perfMetrics.weightVariance) / isi;
            }
        }
        
        pImpl->lastPreSpikeTime = timestamp;
    } else {
        pImpl->perfMetrics.postSpikeCount++;
        
        // Calculate spike synchronization
        if (pImpl->perfMetrics.preSpikeCount > 0 && pImpl->perfMetrics.postSpikeCount > 0) {
            pImpl->perfMetrics.spikeSynchronization = calculateSpikeSynchronization();
        }
        
        pImpl->lastPostSpikeTime = timestamp;
    }
}

void Synapse::recordPlasticityMetrics(float trace, float strength) {
    pImpl->perfMetrics.eligibilityTrace = trace;
    pImpl->perfMetrics.plasticityStrength = strength;
}

void Synapse::recordEfficacyMetrics(float eff) {
    pImpl->perfMetrics.efficacy = eff;
    pImpl->perfMetrics.reliability = (pImpl->perfMetrics.reliability + eff) / 2.0f;
}

void Synapse::recordStepMetrics() {
    pImpl->perfMetrics.stepCount++;
    pImpl->perfMetrics.lastUpdateTime = std::chrono::steady_clock::now();
}

std::unordered_map<std::string, double> Synapse::getPerformanceSummary() const {
    return pImpl->perfMetrics.getPerformanceSummary();
}

// === HELPER METHODS ===

float Synapse::calculateCrossCorrelation() const {
    if (pImpl->preSpikeHistory.empty() || pImpl->postSpikeHistory.empty()) {
        return 0.0f;
    }
    
    // Simple cross-correlation calculation
    float sumPre = 0.0f, sumPost = 0.0f;
    for (const auto& time : pImpl->preSpikeHistory) {
        sumPre += static_cast<float>(time);
    }
    for (const auto& time : pImpl->postSpikeHistory) {
        sumPost += static_cast<float>(time);
    }
    
    float meanPre = sumPre / pImpl->preSpikeHistory.size();
    float meanPost = sumPost / pImpl->postSpikeHistory.size();
    
    float covariance = 0.0f;
    for (const auto& time : pImpl->preSpikeHistory) {
        covariance += (static_cast<float>(time) - meanPre) * (static_cast<float>(time) - meanPre);
    }
    for (const auto& time : pImpl->postSpikeHistory) {
        covariance += (static_cast<float>(time) - meanPost) * (static_cast<float>(time) - meanPost);
    }
    
    covariance /= (pImpl->preSpikeHistory.size() + pImpl->postSpikeHistory.size());
    float stdDevPre = std::sqrt(covariance);
    float stdDevPost = std::sqrt(covariance);
    
    if (stdDevPre == 0.0f || stdDevPost == 0.0f) return 0.0f;
    
    return covariance / (stdDevPre * stdDevPost);
}

float Synapse::calculateSpikeSynchronization() const {
    if (pImpl->preSpikeCount == 0 || pImpl->postSpikeCount == 0) {
        return 0.0f;
    }
    
    // Calculate synchronization based on spike timing
    float totalSync = 0.0f;
    size_t syncCount = 0;
    
    for (size_t i = 0; i < std::min(pImpl->preSpikeHistory.size(), pImpl->postSpikeHistory.size()); ++i) {
        float timeDiff = std::abs(static_cast<float>(pImpl->preSpikeHistory[i] - pImpl->postSpikeHistory[i]));
        totalSync += std::exp(-timeDiff / 10.0f);  // Exponential decay
        syncCount++;
    }
    
    if (syncCount == 0) return 0.0f;
    
    return totalSync / syncCount;
}

float Synapse::calculateBurstFrequency() const {
    if (pImpl->postSpikeCount < 3) {
        return 0.0f;
    }
    
    // Calculate burst frequency based on spike density
    float totalTime = 0.0f;
    if (!pImpl->postSpikeHistory.empty()) {
        totalTime = static_cast<float>(pImpl->postSpikeHistory.back() - pImpl->postSpikeHistory.front());
    }
    
    if (totalTime == 0.0f) return 0.0f;
    
    return (pImpl->postSpikeCount - 2) / totalTime;  // Subtract first 2 spikes for burst definition
}

// === OPTIMIZED RECORDING FUNCTIONS ===

void Synapse::recordPreSpikeToHistory(Synapse& synapse, bool isPreSpike, Timestamp timestamp) {
    if (isPreSpike) {
        synapse.pImpl->preSpikeHistory.push_back(timestamp);
        synapse.pImpl->perfMetrics.preSpikeCount++;
    } else {
        synapse.pImpl->postSpikeHistory.push_back(timestamp);
        synapse.pImpl->perfMetrics.postSpikeCount++;
    }
}

void Synapse::recordPostSpikeToHistory(Synapse& synapse, bool isPostSpike, Timestamp timestamp) {
    if (isPostSpike) {
        synapse.pImpl->postSpikeHistory.push_back(timestamp);
        synapse.pImpl->perfMetrics.postSpikeCount++;
    }
}

// === BATCH OPERATIONS ===

void Synapse::batchSetWeights(const std::vector<Synapse*>& synapses,
                              const std::vector<SynapticWeight>& weights) {
    if (synapses.size() != weights.size()) return;
    
    for (size_t i = 0; i < synapses.size(); ++i) {
        if (synapses[i]) {
            synapses[i]->setWeight(weights[i]);
        }
    }
}

void Synapse::batchAddToWeights(const std::vector<Synapse*>& synapses,
                                const std::vector<SynapticWeight>& deltas) {
    if (synapses.size() != deltas.size()) return;
    
    std::vector<SynapticWeight> newWeights;
    newWeights.reserve(synapses.size());
    
    for (size_t i = 0; i < synapses.size(); ++i) {
        if (synapses[i]) {
            newWeights.push_back(clampWeight(synapses[i]->getWeight() + deltas[i]));
        }
    }
    
    batchSetWeights(synapses, newWeights);
}

void Synapse::batchSetWeights(const std::vector<SynapseId>& synapseIds,
                              const std::vector<SynapticWeight>& weights,
                              const NeuralRegion& region) {
    if (synapseIds.size() != weights.size()) return;
    
    const auto& regionSynapses = region.getSynapses();
    std::unordered_map<SynapseId, Synapse*> synapseMap;
    for (const auto& rs : regionSynapses) {
        synapseMap[rs->getId()] = rs.get();
    }
    
    for (size_t i = 0; i < synapseIds.size(); ++i) {
        auto it = synapseMap.find(synapseIds[i]);
        if (it != synapseMap.end()) {
            it->second->setWeight(weights[i]);
        }
    }
}

void Synapse::batchRecordPreSpikes(const std::vector<Synapse*>& synapses,
                                   const std::vector<Timestamp>& timestamps) {
    if (synapses.size() != timestamps.size()) return;
    
    for (size_t i = 0; i < synapses.size(); ++i) {
        if (synapses[i]) {
            recordPreSpikeToHistory(*synapses[i], true, timestamps[i]);
        }
    }
}

void Synapse::batchRecordPostSpikes(const std::vector<Synapse*>& synapses,
                                     const std::vector<Timestamp>& timestamps) {
    if (synapses.size() != timestamps.size()) return;
    
    for (size_t i = 0; i < synapses.size(); ++i) {
        if (synapses[i]) {
            recordPostSpikeToHistory(*synapses[i], false, timestamps[i]);
        }
    }
}

void Synapse::batchRecordSpikes(const std::vector<Synapse*>& synapses,
                                const std::vector<std::vector<Timestamp>>& preTimestamps,
                                const std::vector<std::vector<Timestamp>>& postTimestamps) {
    if (synapses.size() != preTimestamps.size() || 
        synapses.size() != postTimestamps.size()) return;
    
    for (size_t i = 0; i < synapses.size(); ++i) {
        if (synapses[i]) {
            for (const auto& timestamp : preTimestamps[i]) {
                recordPreSpikeToHistory(*synapses[i], true, timestamp);
            }
            
            for (const auto& timestamp : postTimestamps[i]) {
                recordPostSpikeToHistory(*synapses[i], false, timestamp);
            }
        }
    }
}

void Synapse::batchComputeSynapticCurrents(const std::vector<Synapse*>& synapses,
                                          const std::vector<Neuron*>& preNeurons,
                                          const std::vector<float>& preFiringRates,
                                          std::vector<MembranePotential>& currents) {
    if (synapses.size() != preNeurons.size() || 
        synapses.size() != preFiringRates.size() ||
        currents.size() != synapses.size()) return;
    
    for (size_t i = 0; i < synapses.size(); ++i) {
        if (synapses[i] && preNeurons[i]) {
            currents[i] = synapses[i]->getWeight() * preFiringRates[i];
        }
    }
}

void Synapse::batchEnablePlasticity(std::vector<Synapse*>& synapses,
                                     bool hebbian, bool stdp, bool rewardModulated) {
    for (auto* syn : synapses) {
        if (syn) {
            syn->enablePlasticity(hebbian, stdp, rewardModulated);
        }
    }
}

void Synapse::batchSetEligibilityTraces(std::vector<Synapse*>& synapses,
                                          const std::vector<float>& traces) {
    if (synapses.size() != traces.size()) return;
    
    for (size_t i = 0; i < synapses.size(); ++i) {
        if (synapses[i]) {
            synapses[i]->setEligibilityTrace(traces[i]);
        }
    }
}

} // namespace nlm