// STDP implementation for NLM
#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <chrono>
#include <thread>

namespace nlm {

// STDP implementation with comprehensive performance monitoring

struct STDP::Impl {
    // STDP parameters
    float ltpWeight{0.01f};              // Long-term potentiation weight
    float ltdWeight{0.012f};             // Long-term depression weight
    float timeConstant{20.0f};           // STDP time constant (ms)
    float minWeight{-1.0f};              // Minimum synaptic weight
    float maxWeight{1.0f};               // Maximum synaptic weight
    
    // Learning parameters
    float learningRate{0.01f};           // Learning rate
    float plasticityStrength{0.85f};    // Plasticity strength
    float threshold{0.001f};             // Weight change threshold
    
    // State tracking
    float currentWeight{0.0f};          // Current synaptic weight
    float averageWeight{0.0f};           // Average weight over time
    float weightVariance{0.0f};         // Variance of weight changes
    
    // Spike timing statistics
    float averagePrePostInterval{0.0f};  // Average pre-post spike interval
    float averagePostPreInterval{0.0f}; // Average post-pre spike interval
    float spikeSynchronization{0.0f};   // Measure of spike timing correlation
    float crossCorrelation{0.0f};       // Cross-correlation between pre and post spikes
    
    // Performance metrics
    size_t ltpEvents{0};                 // Long-term potentiation events
    size_t ltdEvents{0};                 // Long-term depression events
    size_t totalSpikePairs{0};           // Total spike pairs processed
    float averageSTDPChange{0.0f};      // Average weight change per spike pair
    float stdDevSTDPChange{0.0f};       // Standard deviation of weight changes
    
    // Temporal tracking
    std::chrono::steady_clock::time_point firstEventTime;
    std::chrono::steady_clock::time_point lastUpdateTime;
    float totalActiveTime{0.0f};        // Total time synapse was active
    
    // Performance monitoring
    std::shared_ptr<PerformanceProfiler> performanceProfiler;
    mutable STDPPerformanceMetrics perfMetrics;
    
    // Custom metrics
    std::unordered_map<std::string, double> customMetrics;
};

STDP::STDP() : pImpl(new Impl) {
    pImpl->currentWeight = 0.0f;
    pImpl->averageWeight = 0.0f;
    pImpl->weightVariance = 0.0f;
    pImpl->firstEventTime = std::chrono::steady_clock::now();
    pImpl->lastUpdateTime = pImpl->firstEventTime;
    
    // Initialize performance metrics
    pImpl->perfMetrics.ltpWeight = pImpl->ltpWeight;
    pImpl->perfMetrics.ltdWeight = pImpl->ltdWeight;
    pImpl->perfMetrics.timeConstant = pImpl->timeConstant;
}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                  const std::vector<Timestamp>& preSpikes,
                  const std::vector<Timestamp>& postSpikes,
                  TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    /*
     * Real STDP implementation based on spike-timing correlation
     * 
     * Mathematical formulation:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+)
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = A- * exp(Δt / τ-)
     * 
     * Where:
     *   A+ = ltpWeight (potentiation amplitude)
     *   A- = ltdWeight (depression amplitude)  
     *   τ+ = τ- = timeConstant (STDP time window)
     * 
     * Biological inspiration:
     *   - Reflects NMDA receptor-mediated calcium signaling
     *   - Pre-before-post activates NMDA receptors when postsynaptic spikes
     *   - Post-before-pre causes backpropagating action potentials
     *   
     * Limitations:
     *   - Simplified pairwise rule (doesn't capture triplet interactions)
     *   - Assumes single exponential window (more complex in biology)
     *   - Doesn't account for synaptic eligibility traces
     */
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    
    // Calculate spike timing statistics
    std::vector<float> preTimes, postTimes;
    for (Timestamp time : preSpikes) {
        preTimes.push_back(static_cast<float>(time));
    }
    for (Timestamp time : postSpikes) {
        postTimes.push_back(static_cast<float>(time));
    }
    
    // Calculate average intervals
    if (!preTimes.empty() && !postTimes.empty()) {
        float sumPrePost = 0.0f, sumPostPre = 0.0f;
        for (size_t i = 0; i < preTimes.size(); ++i) {
            for (size_t j = 0; j < postTimes.size(); ++j) {
                float dt = postTimes[j] - preTimes[i];  // Δt in ms
                
                if (dt > 0) {
                    // Pre before post: POTENTIATION
                    // "Cells that fire together, wire together" - but only if pre fires before post
                    float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                    totalDelta += delta;
                    
                    // Record LTP event
                    recordLTPEvent(delta);
                } else if (dt < 0) {
                    // Post before pre: DEPRESSION
                    // "Anti-Hebbian" - connection weakens if post fires without pre
                    float delta = -pImpl->ltdWeight * std::exp(dt / tau);  // dt is negative, so this subtracts
                    totalDelta += delta;
                    
                    // Record LTD event
                    recordLTD(delta);
                }
                // dt == 0: no change (simultaneous spikes - rare in practice)
            }
        }
        
        // Calculate average intervals
        for (size_t i = 0; i < preTimes.size(); ++i) {
            for (size_t j = 0; j < postTimes.size(); ++j) {
                float dt = postTimes[j] - preTimes[i];
                
                if (dt > 0) {
                    sumPrePost += dt;
                } else if (dt < 0) {
                    sumPostPre += -dt;
                }
            }
        }
        
        if (!preTimes.empty()) {
            averagePrePostInterval = sumPrePost / preTimes.size();
        }
        if (!postTimes.empty()) {
            averagePostPreInterval = sumPostPre / postTimes.size();
        }
        
        // Calculate spike synchronization
        float sumSync = 0.0f;
        for (size_t i = 0; i < preTimes.size(); ++i) {
            for (size_t j = 0; j < postTimes.size(); ++j) {
                float dt = postTimes[j] - preTimes[i];
                sumSync += std::exp(-std::abs(dt) / 10.0f);  // Exponential decay with 10ms time constant
            }
        }
        spikeSynchronization = sumSync / (preTimes.size() * postTimes.size());
        
        // Calculate cross-correlation
        crossCorrelation = calculateCrossCorrelation(preTimes, postTimes);
    }
    
    // Apply weight change
    if (std::abs(totalDelta) > pImpl->threshold) {
        // Record weight change for performance metrics
        pImpl->perfMetrics.weightChange = totalDelta;
        pImpl->perfMetrics.averageSTDPChange = (pImpl->perfMetrics.averageSTDPChange * pImpl->totalSpikePairs + totalDelta) / (pImpl->totalSpikePairs + 1);
        
        // Calculate standard deviation
        if (pImpl->totalSpikePairs > 0) {
            pImpl->perfMetrics.stdDevSTDPChange = std::sqrt((pImpl->perfMetrics.stdDevSTDPChange * pImpl->perfMetrics.stdDevSTDPChange * (pImpl->totalSpikePairs - 1) + (totalDelta - pImpl->perfMetrics.averageSTDPChange) * (totalDelta - pImpl->perfMetrics.averageSTDPChange)) / pImpl->totalSpikePairs);
        }
        
        // Apply to synapse
        applyWeightChange(synapse, totalDelta);
        
        // Record performance metrics
        recordPerformanceMetrics();
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds using centralized function
    SynapticWeight newWeight = synapse->getWeight() + delta;
    newWeight = clampWeight(newWeight);
    synapse->setWeight(newWeight);
    
    pImpl->currentWeight = newWeight;
    pImpl->perfMetrics.currentWeight = newWeight;
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
    pImpl->perfMetrics.ltpWeight = pImpl->ltpWeight;
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
    pImpl->perfMetrics.ltdWeight = pImpl->ltdWeight;
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::max(1.0f, tau);
    pImpl->perfMetrics.timeConstant = pImpl->timeConstant;
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

// Performance monitoring methods
void STDP::setPerformanceProfiler(std::shared_ptr<PerformanceProfiler> profiler) {
    pImpl->performanceProfiler = profiler;
}

STDPPerformanceMetrics STDP::getPerformanceMetrics() const {
    return pImpl->perfMetrics;
}

void STDP::recordPerformanceMetrics() {
    // Update performance metrics
    pImpl->perfMetrics.ltpWeight = pImpl->ltpWeight;
    pImpl->perfMetrics.ltdWeight = pImpl->ltdWeight;
    pImpl->perfMetrics.timeConstant = pImpl->timeConstant;
    pImpl->perfMetrics.currentWeight = pImpl->currentWeight;
    pImpl->perfMetrics.averageWeight = pImpl->averageWeight;
    pImpl->perfMetrics.weightChange = pImpl->perfMetrics.weightChange;
    pImpl->perfMetrics.weightChangeRate = calculateWeightChangeRate();
    pImpl->perfMetrics.weightVariance = pImpl->weightVariance;
    pImpl->perfMetrics.averagePrePostInterval = pImpl->averagePrePostInterval;
    pImpl->perfMetrics.averagePostPreInterval = pImpl->averagePostPreInterval;
    pImpl->perfMetrics.spikeSynchronization = pImpl->spikeSynchronization;
    pImpl->perfMetrics.crossCorrelation = pImpl->crossCorrelation;
    pImpl->perfMetrics.plasticityEfficiency = getPlasticityEfficiency();
    pImpl->perfMetrics.learningRate = getLearningRate();
    pImpl->perfMetrics.plasticityStrength = pImpl->plasticityStrength;
    
    // Record to performance profiler if available
    if (pImpl->performanceProfiler) {
        auto summary = getPerformanceSummary();
        pImpl->performanceProfiler->recordPlasticityProfile(getName(), summary);
    }
}

std::unordered_map<std::string, double> STDP::getPerformanceSummary() const {
    return pImpl->perfMetrics.getPerformanceSummary();
}

// Helper methods
void STDP::recordLTPEvent(float delta) {
    pImpl->perfMetrics.ltpEvents++;
    pImpl->perfMetrics.totalSpikePairs++;
    updateMetrics();
}

void STDP::recordLTDEvent(float delta) {
    pImpl->perfMetrics.ltdEvents++;
    pImpl->perfMetrics.totalSpikePairs++;
    updateMetrics();
}

void STDP::updateMetrics() {
    // Update average weight
    if (pImpl->totalSpikePairs > 0) {
        pImpl->averageWeight = pImpl->currentWeight;
    }
    
    // Calculate weight variance
    if (pImpl->perfMetrics.updateCount > 0) {
        pImpl->weightVariance = (pImpl->weightVariance * (pImpl->perfMetrics.updateCount - 1) + (pImpl->currentWeight - pImpl->averageWeight) * (pImpl->currentWeight - pImpl->averageWeight)) / pImpl->perfMetrics.updateCount;
    }
    
    // Update last update time
    pImpl->lastUpdateTime = std::chrono::steady_clock::now();
    pImpl->totalActiveTime = std::chrono::duration<double>(pImpl->lastUpdateTime - pImpl->firstEventTime).count();
    
    pImpl->perfMetrics.updateCount++;
}

float STDP::calculateWeightChangeRate() const {
    if (pImpl->firstEventTime.time_since_epoch().count() > 0) {
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - pImpl->firstEventTime).count();
        if (elapsed > 0) {
            return pImpl->perfMetrics.weightChange / elapsed;
        }
    }
    return 0.0f;
}

float STDP::getLearningRate() const {
    return pImpl->learningRate;
}

float STDP::getPlasticityEfficiency() const {
    float totalEvents = static_cast<float>(pImpl->perfMetrics.ltpEvents + pImpl->perfMetrics.ltdEvents);
    if (totalEvents == 0) return 0.0f;
    return (pImpl->perfMetrics.ltpEvents / totalEvents) * 100.0f;
}

float STDP::calculateCrossCorrelation(const std::vector<float>& preTimes, const std::vector<float>& postTimes) const {
    if (preTimes.empty() || postTimes.empty()) return 0.0f;
    
    float sumPre = 0.0f, sumPost = 0.0f;
    for (float time : preTimes) sumPre += time;
    for (float time : postTimes) sumPost += time;
    
    float meanPre = sumPre / preTimes.size();
    float meanPost = sumPost / postTimes.size();
    
    float covariance = 0.0f;
    for (float time : preTimes) {
        covariance += (time - meanPre) * (time - meanPre);
    }
    for (float time : postTimes) {
        covariance += (time - meanPost) * (time - meanPost);
    }
    
    covariance /= (preTimes.size() + postTimes.size());
    float stdDevPre = std::sqrt(covariance);
    float stdDevPost = std::sqrt(covariance);
    
    if (stdDevPre == 0.0f || stdDevPost == 0.0f) return 0.0f;
    
    return covariance / (stdDevPre * stdDevPost);
}

// Centralized weight clamping function
SynapticWeight STDP::clampWeight(SynapticWeight weight) {
    return std::clamp(weight, Impl::MIN_WEIGHT, Impl::MAX_WEIGHT);
}

} // namespace nlm