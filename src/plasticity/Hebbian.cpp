#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>
#include <chrono>
#include <thread>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    
    // State tracking
    float currentWeight{0.0f};
    float averageWeight{0.0f};
    float weightVariance{0.0f};
    
    // Spike timing statistics
    float averageFiringRate{0.0f};
    float firingRateVariability{0.0f};
    float spikeSynchronization{0.0f};
    float crossCorrelation{0.0f};
    
    // Performance metrics
    size_t hebbianEvents{0};
    size_t totalSpikePairs{0};
    float averageHebbianChange{0.0f};
    float stdDevHebbianChange{0.0f};
    
    // Temporal tracking
    std::chrono::steady_clock::time_point firstEventTime;
    std::chrono::steady_clock::time_point lastUpdateTime;
    float totalActiveTime{0.0f};
    
    // Performance monitoring
    std::shared_ptr<PerformanceProfiler> performanceProfiler;
    mutable HebbianPerformanceMetrics perfMetrics;
    
    // Custom metrics
    std::unordered_map<std::string, double> customMetrics;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f),
             currentWeight(0.0f), averageWeight(0.0f),
             weightVariance(0.0f),
             firstEventTime(std::chrono::steady_clock::now()),
             lastUpdateTime(firstEventTime) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {
    pImpl->currentWeight = 0.0f;
    pImpl->averageWeight = 0.0f;
    pImpl->weightVariance = 0.0f;
    
    // Initialize performance metrics
    pImpl->perfMetrics.learningRate = pImpl->learningRate;
    pImpl->perfMetrics.maxWeight = pImpl->maxWeight;
    pImpl->perfMetrics.minWeight = pImpl->minWeight;
}

Hebbian::~Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Real Hebbian learning implementation
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Simplified version for spike-based systems:
     * Δw = η * (coactivity - baseline)
     * 
     * Where:
     *   coactivity = number of correlated pre/post spikes
     *   baseline = learningRate * mean activity
     * 
     * This implements "neurons that fire together, wire together"
     * but with a threshold to prevent runaway potentiation.
     * 
     * Biological inspiration:
     *   - Reflects AMPA receptor trafficking
     *   - Hebbian plasticity at Schaffer collateral synapses in hippocampus
     *   - Correlation-based learning in visual cortex
     *   
     * Limitations:
     *   - Doesn't account for STDP timing details
     *   - Single learning rate (no separate potentiation/depression rates)
     *   - Assumes stationary statistics
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count correlated spike pairs (simplified covariance)
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            // Count spikes within a broad time window as correlated
            if (std::abs(dt) < 100.0f) {  // 100ms correlation window
                ++correlationCount;
            }
        }
    }
    
    // Calculate firing rate statistics
    float preFiringRate = static_cast<float>(preSpikes.size()) / dt;
    float postFiringRate = static_cast<float>(postSpikes.size()) / dt;
    
    // Calculate average firing rate
    if (pImpl->perfMetrics.updateCount == 0) {
        pImpl->perfMetrics.averageFiringRate = (preFiringRate + postFiringRate) / 2.0f;
    } else {
        pImpl->perfMetrics.averageFiringRate = (pImpl->perfMetrics.averageFiringRate + (preFiringRate + postFiringRate) / 2.0f) / 2.0f;
    }
    
    // Calculate firing rate variability
    pImpl->perfMetrics.firingRateVariability = std::sqrt(
        ((preFiringRate - pImpl->perfMetrics.averageFiringRate) * (preFiringRate - pImpl->perfMetrics.averageFiringRate) +
         (postFiringRate - pImpl->perfMetrics.averageFiringRate) * (postFiringRate - pImpl->perfMetrics.averageFiringRate)) / 2.0f
    );
    
    // Compute weight change based on correlation
    // More sophisticated: use actual spike counts and firing rates
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    // Apply with bounds
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
        
        // Record Hebbian event
        recordHebbianEvent(delta);
        
        // Record spike metrics
        recordSpikeMetrics(preFiringRate, postFiringRate);
        
        // Record performance metrics
        recordPerformanceMetrics();
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    // Use centralized clamping function
    newWeight = Synapse::clampWeight(newWeight);
    synapse->setWeight(newWeight);
    
    pImpl->currentWeight = newWeight;
    pImpl->perfMetrics.currentWeight = newWeight;
    
    // Update performance metrics
    if (pImpl->perfMetrics.updateCount > 0) {
        pImpl->weightVariance = (pImpl->weightVariance * (pImpl->perfMetrics.updateCount - 1) + 
                                (newWeight - pImpl->averageWeight) * (newWeight - pImpl->averageWeight)) / 
                                pImpl->perfMetrics.updateCount;
    }
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
    pImpl->perfMetrics.learningRate = pImpl->learningRate;
}

float Hebbian::getLearningRate() const {
    return pImpl->learningRate;
}

void Hebbian::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float Hebbian::getMaxWeight() const {
    return pImpl->maxWeight;
}

void Hebbian::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float Hebbian::getMinWeight() const {
    return pImpl->minWeight;
}

void Hebbian::configure(float learningRate, float maxWeight, float minWeight) {
    setLearningRate(learningRate);
    setMaxWeight(maxWeight);
    setMinWeight(minWeight);
}

// Performance monitoring methods
void Hebbian::setPerformanceProfiler(std::shared_ptr<PerformanceProfiler> profiler) {
    pImpl->performanceProfiler = profiler;
}

HebbianPerformanceMetrics Hebbian::getPerformanceMetrics() const {
    return pImpl->perfMetrics;
}

void Hebbian::recordPerformanceMetrics() {
    // Update performance metrics
    pImpl->perfMetrics.learningRate = pImpl->learningRate;
    pImpl->perfMetrics.maxWeight = pImpl->maxWeight;
    pImpl->perfMetrics.minWeight = pImpl->minWeight;
    pImpl->perfMetrics.currentWeight = pImpl->currentWeight;
    pImpl->perfMetrics.averageWeight = pImpl->averageWeight;
    pImpl->perfMetrics.weightChange = pImpl->perfMetrics.weightChange;
    pImpl->perfMetrics.weightChangeRate = calculateWeightChangeRate();
    pImpl->perfMetrics.weightVariance = pImpl->weightVariance;
    pImpl->perfMetrics.averageFiringRate = pImpl->perfMetrics.averageFiringRate;
    pImpl->perfMetrics.firingRateVariability = pImpl->perfMetrics.firingRateVariability;
    pImpl->perfMetrics.spikeSynchronization = pImpl->perfMetrics.spikeSynchronization;
    pImpl->perfMetrics.crossCorrelation = pImpl->perfMetrics.crossCorrelation;
    pImpl->perfMetrics.plasticityEfficiency = getPlasticityEfficiency();
    pImpl->perfMetrics.learningEffectiveness = getLearningEffectiveness();
    pImpl->perfMetrics.weightStability = getWeightStability();
    
    // Record to performance profiler if available
    if (pImpl->performanceProfiler) {
        auto summary = getPerformanceSummary();
        pImpl->performanceProfiler->recordPlasticityProfile(getName(), summary);
    }
}

std::unordered_map<std::string, double> Hebbian::getPerformanceSummary() const {
    return pImpl->perfMetrics.getPerformanceSummary();
}

// Helper methods
void Hebbian::recordHebbianEvent(float delta) {
    pImpl->perfMetrics.hebbianEvents++;
    pImpl->perfMetrics.totalSpikePairs++;
    pImpl->perfMetrics.weightChange += delta;
    updateMetrics();
}

void Hebbian::recordSpikeMetrics(float preFiringRate, float postFiringRate) {
    // Update cross-correlation
    pImpl->perfMetrics.crossCorrelation = (pImpl->perfMetrics.crossCorrelation + 
                                          (preFiringRate - pImpl->perfMetrics.averageFiringRate) * 
                                          (postFiringRate - pImpl->perfMetrics.averageFiringRate)) / 
                                         static_cast<float>(pImpl->perfMetrics.updateCount + 1);
    
    // Update spike synchronization
    pImpl->perfMetrics.spikeSynchronization = calculateSpikeSynchronization(preFiringRate, postFiringRate);
}

void Hebbian::updateMetrics() {
    // Update average weight
    if (pImpl->perfMetrics.totalSpikePairs > 0) {
        pImpl->averageWeight = pImpl->currentWeight;
    }
    
    // Update last update time
    pImpl->lastUpdateTime = std::chrono::steady_clock::now();
    pImpl->totalActiveTime = std::chrono::duration<double>(pImpl->lastUpdateTime - pImpl->firstEventTime).count();
    
    pImpl->perfMetrics.updateCount++;
}

float Hebbian::calculateWeightChangeRate() const {
    if (pImpl->firstEventTime.time_since_epoch().count() > 0) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->firstEventTime).count();
        if (elapsed > 0) {
            return pImpl->perfMetrics.weightChange / elapsed;
        }
    }
    return 0.0f;
}

float Hebbian::getLearningEffectiveness() const {
    return (pImpl->perfMetrics.averageHebbianChange > 0.0f) ? std::min(100.0f, pImpl->perfMetrics.averageHebbianChange * 1000.0f) : 0.0f;
}

float Hebbian::getWeightStability() const {
    return std::max(0.0f, 1.0f - (pImpl->weightVariance / (pImpl->maxWeight - pImpl->minWeight)));
}

float Hebbian::calculateSpikeSynchronization(float preFiringRate, float postFiringRate) const {
    // Calculate synchronization based on firing rates
    float sumSync = 0.0f;
    
    // Normalize firing rates
    float normPre = preFiringRate / pImpl->perfMetrics.averageFiringRate;
    float normPost = postFiringRate / pImpl->perfMetrics.averageFiringRate;
    
    // Calculate synchronization as correlation of normalized firing rates
    if (normPre > 0.0f && normPost > 0.0f) {
        sumSync = std::exp(-std::abs(normPre - normPost));
    }
    
    return sumSync;
}

} // namespace nlm