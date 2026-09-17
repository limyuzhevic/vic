#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <chrono>
#include <execution>
#include <atomic>
#include <unordered_map>
#include <mutex>
#include <thread>

namespace nlm {

// Internal implementation structure for STDP
struct STDP::Impl {
    STDPConfig config;           // Configuration parameters
    STDPVariant variant;         // Current STDP variant
    float predictionError;       // For learning rate adaptation
    float lastUpdateTime;        // For profiling
    size_t spikePairCount;       // Number of spike pairs processed
    float totalUpdateTime;       // Accumulated update time for profiling
    
    // Thread-local storage for parallel processing
    static thread_local std::vector<std::pair<Timestamp, Timestamp>> localSpikePairs;
    static thread_local float localTotalDelta;
    
    // Mutex for thread safety
    static std::mutex globalMutex;
    
    Impl(const STDPConfig& cfg) : config(cfg), variant(STDPVariant::Standard),
                                 predictionError(0.0f), lastUpdateTime(0.0f),
                                 spikePairCount(0), totalUpdateTime(0.0f) {}
};

// Initialize thread-local storage
template<typename T> thread_local T STDP::Impl::localSpikePairs;
template<typename T> thread_local T STDP::Impl::localTotalDelta;
std::mutex STDP::Impl::globalMutex;

// Mathematical formulations documentation
namespace MathematicalFormulas {
    // Standard STDP formulation
    constexpr const char* STANDARD_STDP = R"(
    Standard STDP Rule:
    -----------------
    For pre-post spike pair with timing difference Δt = t_post - t_pre:
    
    If Δt > 0 (pre before post): POTENTIATION
        Δw = A+ * exp(-Δt / τ+)
    
    If Δt < 0 (post before pre): DEPRESSION  
        Δw = -A- * exp(Δt / τ-)
    
    Where:
        A+ = ltpWeight (potentiation amplitude)
        A- = ltdWeight (depression amplitude)
        τ+ = τ- = timeConstant (STDP time window)
    
    Biological basis:
    - NMDA receptor-mediated calcium dynamics
    - Pre-before-post activates NMDA receptors
    - Post-before-pre causes backpropagating action potentials
    )";
    
    // Reward-modulated STDP formulation
    constexpr const char* REWARD_MODULATED_STDP = R"(
    Reward-Modulated STDP (R-STDP):
    -------------------------------
    Δw = [A+ * exp(-Δt / τ+) - A- * exp(Δt / τ-)] * R * ET
    
    Where:
        R = (reward - baseline) / rewardModGain + 1.0
        ET = eligibility trace (decays exponentially)
    
    Reward R influences learning speed and direction.
    )";
    
    // Triplet STDP formulation
    constexpr const char* TRIPLET_STDP = R"(
    Triplet STDP:
    ------------
    Involves higher-order spike interactions: pre-pre-post and pre-post-post patterns
    
    For pre-post pairs with additional preceding/following spikes:
    Δw = A+ * f(Δt) * g(history) - A- * h(Δt) * g(history)
    
    Where g(history) captures recent spike interactions beyond pairwise timing.
    )";
}

// STDP implementation
STDP::STDP(const STDPConfig& config) : pImpl(std::make_unique<Impl>(config)) {
}

STDP::~STDP() = default;

void STDP::validateParameters() const {
    const auto& cfg = pImpl->config;
    
    // Validate core parameters
    if (cfg.ltpWeight < 0.0f || cfg.ltpWeight > 1.0f) {
        throw std::invalid_argument("LTP weight must be in range [0.0, 1.0]");
    }
    if (cfg.ltdWeight < 0.0f || cfg.ltdWeight > 1.0f) {
        throw std::invalid_argument("LTD weight must be in range [0.0, 1.0]");
    }
    
    if (cfg.tauPlus <= 0.0f || cfg.tauPlus > 1000.0f) {
        throw std::invalid_argument("TauPlus must be in range (0.0, 1000.0]");
    }
    if (cfg.tauMinus <= 0.0f || cfg.tauMinus > 1000.0f) {
        throw std::invalid_argument("TauMinus must be in range (0.0, 1000.0]");
    }
    
    // Validate time window configuration
    if (cfg.minPairingWindow <= 0.0f || cfg.minPairingWindow >= cfg.maxPairingWindow) {
        throw std::invalid_argument("Invalid minPairingWindow configuration");
    }
    if (cfg.maxPairingWindow <= cfg.minPairingWindow || cfg.maxPairingWindow > 1000.0f) {
        throw std::invalid_argument("Invalid maxPairingWindow configuration");
    }
    
    // Validate variant-specific parameters
    if (cfg.enableLearningRateAdaptation && cfg.predictionErrorGain < 0.0f) {
        throw std::invalid_argument("Prediction error gain must be non-negative");
    }
    
    if (cfg.enableEligibilityTraces && cfg.eligibilityDecayRate <= 0.0f) {
        throw std::invalid_argument("Eligibility decay rate must be positive");
    }
    
    if (cfg.useBatchProcessing && cfg.maxBatchSize == 0) {
        throw std::invalid_argument("Max batch size must be positive when batch processing is enabled");
    }
}

void STDP::validateSpikeHistory(const std::vector<Timestamp>& spikes) const {
    if (spikes.empty()) {
        return; // Empty history is valid (no spikes)
    }
    
    // Check for valid timestamps
    for (Timestamp t : spikes) {
        if (std::isnan(t) || std::isinf(t) || t < 0.0) {
            throw std::invalid_argument("Invalid timestamp in spike history");
        }
    }
    
    // Check for reasonable spike frequency (prevent runaway spike counts)
    if (spikes.size() > 10000) {
        throw std::invalid_argument("Spike history too large (potential data corruption)");
    }
}

void STDP::computeSTDPSpikePairs(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Validate inputs
    validateParameters();
    validateSpikeHistory(preSpikes);
    validateSpikeHistory(postSpikes);
    
    if (!synapse) {
        return;
    }
    
    float totalDelta = 0.0f;
    size_t spikePairCount = 0;
    
    switch (pImpl->variant) {
        case STDPVariant::Standard:
            computeStandardSTDP(synapse, preSpikes, postSpikes);
            break;
        case STDPVariant::RewardModulated:
            computeRewardModulatedSTDP(synapse, preSpikes, postSpikes);
            break;
        case STDPVariant::Triplet:
            computeTripletSTDP(synapse, preSpikes, postSpikes);
            break;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    
    // Update profiling data
    pImpl->totalUpdateTime += static_cast<float>(duration.count()) / 1e9f;
    pImpl->spikePairCount += spikePairCount;
    pImpl->lastUpdateTime = static_cast<float>(duration.count()) / 1e9f;
}

void STDP::computeStandardSTDP(Synapse* synapse,
                              const std::vector<Timestamp>& preSpikes,
                              const std::vector<Timestamp>& postSpikes) {
    const auto& cfg = pImpl->config;
    float totalDelta = 0.0f;
    size_t spikePairCount = 0;
    
    // Use parallel processing if enabled
    if (cfg.useParallelProcessing && (preSpikes.size() * postSpikes.size() > 100)) {
        // Parallel version using std::execution::par
        totalDelta = computeStandardSTDPPairwiseParallel(preSpikes, postSpikes, spikePairCount);
    } else {
        // Sequential version with optimized spike time lookup
        totalDelta = computeStandardSTDPPairwiseOptimized(preSpikes, postSpikes, spikePairCount);
    }
    
    // Apply the total weight change
    if (std::abs(totalDelta) > 1e-8f) {
        // Scale by synaptic efficacy if enabled
        if (cfg.enableEfficacyModulation) {
            totalDelta *= synapse->getEfficacy();
        }
        
        // Apply weight change with bounds checking
        applyWeightChange(synapse, totalDelta);
        
        // Update eligibility trace if enabled
        if (cfg.enableEligibilityTraces) {
            updateEligibilityTrace(synapse, totalDelta);
        }
    }
    
    // Update instance's spike pair count
    pImpl->spikePairCount += spikePairCount;
}

float STDP::computeStandardSTDPPairwiseOptimized(const std::vector<Timestamp>& preSpikes,
                                                 const std::vector<Timestamp>& postSpikes,
                                                 size_t& spikePairCount) {
    float totalDelta = 0.0f;
    spikePairCount = 0;
    
    const auto& cfg = pImpl->config;
    float tau = cfg.tauPlus;  // Using same tau for both + and -
    
    // Sort spike times for efficient pairing (if not already sorted)
    std::vector<Timestamp> sortedPreSpikes = preSpikes;
    std::vector<Timestamp> sortedPostSpikes = postSpikes;
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    // Optimized algorithm using sorted arrays and early termination
    for (size_t i = 0; i < sortedPreSpikes.size(); ++i) {
        Timestamp preTime = sortedPreSpikes[i];
        
        // Find the first post spike that could potentially contribute
        // Use binary search for efficiency
        auto postIt = std::lower_bound(sortedPostSpikes.begin(), sortedPostSpikes.end(), preTime);
        
        // Process post spikes in both directions from the current position
        // Forward direction (post after pre)
        for (auto it = postIt; it != sortedPostSpikes.end(); ++it) {
            Timestamp postTime = *it;
            float dt = static_cast<float>(postTime - preTime);
            
            if (dt > cfg.timeWindowSize) {
                break; // Further post spikes will have even larger dt
            }
            
            if (dt > 0) {
                // Pre before post: potentiation
                float delta = cfg.ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
                spikePairCount++;
            }
        }
        
        // Backward direction (post before pre) - search backwards
        postIt = std::prev(std::upper_bound(sortedPostSpikes.begin(), sortedPostSpikes.end(), preTime));
        
        for (auto it = postIt; it != sortedPostSpikes.begin(); --it) {
            Timestamp postTime = *it;
            float dt = static_cast<float>(postTime - preTime);
            
            if (-dt > cfg.timeWindowSize) {
                break; // Further pre spikes will have even larger negative dt
            }
            
            if (dt < 0) {
                // Post before pre: depression
                float delta = -cfg.ltdWeight * std::exp(dt / tau); // dt is negative
                totalDelta += delta;
                spikePairCount++;
            }
        }
    }
    
    return totalDelta;
}

float STDP::computeStandardSTDPPairwiseParallel(const std::vector<Timestamp>& preSpikes,
                                                const std::vector<Timestamp>& postSpikes,
                                                size_t& spikePairCount) {
    float totalDelta = 0.0f;
    spikePairCount = 0;
    
    const auto& cfg = pImpl->config;
    float tau = cfg.tauPlus;
    
    // Reset thread-local storage
    Impl::localTotalDelta = 0.0f;
    Impl::localSpikePairs.clear();
    
    // Prepare spike pairs for parallel processing
    std::vector<std::pair<size_t, size_t>> pairIndices;
    pairIndices.reserve(preSpikes.size() * postSpikes.size());
    
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            pairIndices.emplace_back(i, j);
        }
    }
    
    // Process spike pairs in parallel
    std::atomic<size_t> localCount{0};
    
    std::for_each(std::execution::par, pairIndices.begin(), pairIndices.end(),
        [&](const std::pair<size_t, size_t>& indices) {
            Timestamp preTime = preSpikes[indices.first];
            Timestamp postTime = postSpikes[indices.second];
            float dt = static_cast<float>(postTime - preTime);
            
            // Apply time window filtering for efficiency
            if (std::abs(dt) <= cfg.timeWindowSize) {
                if (dt > 0) {
                    float delta = cfg.ltpWeight * std::exp(-dt / tau);
                    Impl::localTotalDelta += delta;
                } else if (dt < 0) {
                    float delta = -cfg.ltdWeight * std::exp(dt / tau);
                    Impl::localTotalDelta += delta;
                }
                localCount++;
            }
        }
    );
    
    totalDelta = Impl::localTotalDelta;
    spikePairCount = localCount.load();
    
    return totalDelta;
}

void STDP::computeRewardModulatedSTDP(Synapse* synapse,
                                      const std::vector<Timestamp>& preSpikes,
                                      const std::vector<Timestamp>& postSpikes) {
    const auto& cfg = pImpl->config;
    
    // Compute baseline STDP contribution
    float baselineDelta = 0.0f;
    if (pImpl->variant != STDPVariant::RewardModulated) {
        // Temporarily set to standard for calculation
        STDPVariant originalVariant = pImpl->variant;
        pImpl->variant = STDPVariant::Standard;
        computeStandardSTDP(synapse, preSpikes, postSpikes);
        baselineDelta = synapse->getEligibilityTrace();  // This is approximate
        pImpl->variant = originalVariant;
    }
    
    // Calculate reward modulation factor
    float rewardFactor = (pImpl->predictionError - cfg.rewardBaseline) / cfg.rewardModGain + 1.0f;
    
    // Apply reward modulation to baseline STDP
    float modulatedDelta = baselineDelta * rewardFactor;
    
    // Apply weight change
    applyWeightChange(synapse, modulatedDelta);
    
    // Update eligibility trace with modulation
    updateEligibilityTrace(synapse, modulatedDelta);
}

void STDP::computeTripletSTDP(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes) {
    const auto& cfg = pImpl->config;
    float totalDelta = 0.0f;
    size_t spikePairCount = 0;
    
    // Triplet STDP implementation (simplified)
    // Consider patterns: pre-pre-post, pre-post-post
    
    if (preSpikes.size() >= 2 && postSpikes.size() >= 1) {
        // Pre-pre-post triplet interactions
        for (size_t i = 1; i < preSpikes.size(); ++i) {
            Timestamp preTime1 = preSpikes[i-1];
            Timestamp preTime2 = preSpikes[i];
            Timestamp postTime = postSpikes[0];
            
            float dt1 = static_cast<float>(postTime - preTime1);  // First pre-post interval
            float dt2 = static_cast<float>(preTime2 - preTime1); // Pre-pre interval
            
            if (dt1 > 0 && dt2 > 0 && dt1 < cfg.tripletGluingTime && dt2 < cfg.tripletGluingTime) {
                // Glued pre-pre-post triplet contributes to potentiation
                float tripletFactor = std::exp(-dt2 / cfg.tauPlus);
                float delta = cfg.ltpWeight * std::exp(-dt1 / cfg.tauPlus) * tripletFactor;
                totalDelta += delta;
                spikePairCount++;
            }
        }
    }
    
    if (postSpikes.size() >= 2 && preSpikes.size() >= 1) {
        // Pre-post-post triplet interactions
        for (size_t j = 1; j < postSpikes.size(); ++j) {
            Timestamp preTime = preSpikes[0];
            Timestamp postTime1 = postSpikes[j-1];
            Timestamp postTime2 = postSpikes[j];
            
            float dt1 = static_cast<float>(postTime1 - preTime); // Pre-post interval
            float dt2 = static_cast<float>(postTime2 - postTime1); // Post-post interval
            
            if (dt1 > 0 && dt2 > 0 && dt1 < cfg.tripletGluingTime && dt2 < cfg.tripletGluingTime) {
                // Glued pre-post-post triplet contributes to depression
                float tripletFactor = std::exp(-dt2 / cfg.tauMinus);
                float delta = -cfg.ltdWeight * std::exp(-dt1 / cfg.tauMinus) * tripletFactor;
                totalDelta += delta;
                spikePairCount++;
            }
        }
    }
    
    // Add standard STDP contribution for remaining pairs
    float standardDelta = 0.0f;
    size_t standardPairs = 0;
    
    STDPVariant originalVariant = pImpl->variant;
    pImpl->variant = STDPVariant::Standard;
    
    // Compute standard STDP but only count standard pairs
    computeStandardSTDP(synapse, preSpikes, postSpikes);
    
    pImpl->variant = originalVariant;
    
    // Combine triplet and standard contributions
    float finalDelta = totalDelta + standardDelta;
    
    if (std::abs(finalDelta) > 1e-8f) {
        if (cfg.enableEfficacyModulation) {
            finalDelta *= synapse->getEfficacy();
        }
        
        applyWeightChange(synapse, finalDelta);
        
        if (cfg.enableEligibilityTraces) {
            updateEligibilityTrace(synapse, finalDelta);
        }
    }
    
    pImpl->spikePairCount += spikePairCount;
}

void STDP::updateEligibilityTrace(Synapse* synapse, float delta) {
    if (!synapse) return;
    
    const auto& cfg = pImpl->config;
    float currentTrace = synapse->getEligibilityTrace();
    
    // Update eligibility trace with the weight change
    synapse->setEligibilityTrace(currentTrace + delta);
}

void STDP::decayEligibilityTrace(Synapse* synapse, TimestepDuration dt) {
    if (!synapse || !pImpl->config.enableEligibilityTraces) return;
    
    float decayRate = pImpl->config.eligibilityDecayRate;
    float timeStep = static_cast<float>(dt);
    
    // Exponential decay of eligibility trace
    float currentTrace = synapse->getEligibilityTrace();
    float decayedTrace = currentTrace * std::exp(-decayRate * timeStep);
    
    // Set to zero if below threshold
    if (std::abs(decayedTrace) < 0.001f) {
        decayedTrace = 0.0f;
    }
    
    synapse->setEligibilityTrace(decayedTrace);
}

void STDP::computeBatchSTDPSpikePairs(Synapse* synapse,
                                      const std::vector<Timestamp>& preSpikes,
                                      const std::vector<Timestamp>& postSpikes,
                                      TimestepDuration dt) {
    // Batch processing implementation
    // Split spike history into chunks and process in parallel
    
    const auto& cfg = pImpl->config;
    if (!cfg.useBatchProcessing) {
        computeSTDPSpikePairs(synapse, preSpikes, postSpikes, dt);
        return;
    }
    
    size_t batchSize = std::min(cfg.maxBatchSize, 
                               std::max(preSpikes.size(), postSpikes.size()));
    
    if (batchSize < 2) {
        computeSTDPSpikePairs(synapse, preSpikes, postSpikes, dt);
        return;
    }
    
    // Process in batches
    std::vector<std::future<void>> futures;
    
    // For simplicity, we'll implement a simple batching approach
    // In a real implementation, this could use multiple threads or vectorization
    size_t numBatches = (preSpikes.size() + batchSize - 1) / batchSize;
    
    for (size_t batch = 0; batch < numBatches; ++batch) {
        size_t start = batch * batchSize;
        size_t end = std::min(start + batchSize, preSpikes.size());
        
        if (start >= preSpikes.size()) break;
        
        // Create mini spike lists for this batch
        std::vector<Timestamp> batchPreSpikes(
            preSpikes.begin() + start, preSpikes.begin() + end);
        
        // Process this batch (could be parallelized further)
        computeStandardSTDPPairwiseOptimized(batchPreSpikes, postSpikes, pImpl->spikePairCount);
    }
}

void STDP::update(Synapse* synapse,
                 const std::vector<Timestamp>& preSpikes,
                 const std::vector<Timestamp>& postSpikes,
                 TimestepDuration dt) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Decay eligibility trace if enabled
    if (pImpl->config.enableEligibilityTraces) {
        decayEligibilityTrace(synapse, dt);
    }
    
    // Use batch processing if enabled and appropriate
    if (pImpl->config.useBatchProcessing && 
        (preSpikes.size() + postSpikes.size() > pImpl->config.maxBatchSize)) {
        computeBatchSTDPSpikePairs(synapse, preSpikes, postSpikes, dt);
    } else {
        computeSTDPSpikePairs(synapse, preSpikes, postSpikes, dt);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    
    // Update profiling data
    pImpl->totalUpdateTime += static_cast<float>(duration.count()) / 1e9f;
    pImpl->lastUpdateTime = static_cast<float>(duration.count()) / 1e9f;
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    const auto& cfg = pImpl->config;
    
    // Validate delta to prevent runaway weight changes
    if (std::abs(delta) > cfg.timeWindowSize * 10.0f) {
        throw std::invalid_argument("Weight change delta exceeds safe threshold");
    }
    
    // Apply weight change with advanced bounds checking
    float newWeight = synapse->getWeight() + delta;
    
    // Enforce weight bounds with hysteresis
    newWeight = std::clamp(newWeight, cfg.wMin, cfg.wMax);
    
    // Additional safety check: prevent rapid weight oscillations
    if (std::abs(newWeight - synapse->getWeight()) > 0.1f) {
        // If weight change is too large, apply it gradually
        newWeight = synapse->getWeight() + delta * 0.1f;  // Scale down large changes
        newWeight = std::clamp(newWeight, cfg.wMin, cfg.wMax);
    }
    
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    if (pImpl->config.enableLearningRateAdaptation) {
        // Calculate adaptive learning rate
        float baseRate = std::clamp(weight, 0.0f, 1.0f);
        float adaptiveRate = calculateLearningRate(baseRate);
        pImpl->config.ltpWeight = adaptiveRate;
    } else {
        pImpl->config.ltpWeight = std::clamp(weight, 0.0f, 1.0f);
    }
}

float STDP::getLTPWeight() const {
    return pImpl->config.ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    if (pImpl->config.enableLearningRateAdaptation) {
        float baseRate = std::clamp(weight, 0.0f, 1.0f);
        float adaptiveRate = calculateLearningRate(baseRate);
        pImpl->config.ltdWeight = adaptiveRate;
    } else {
        pImpl->config.ltdWeight = std::clamp(weight, 0.0f, 1.0f);
    }
}

float STDP::getLTDWeight() const {
    return pImpl->config.ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->config.tauPlus = std::clamp(tau, 1.0f, 1000.0f);
    pImpl->config.tauMinus = std::clamp(tau, 1.0f, 1000.0f);
}

float STDP::getTimeConstant() const {
    return pImpl->config.tauPlus;
}

void STDP::setTimeConstants(float tauPlus, float tauMinus) {
    pImpl->config.tauPlus = std::clamp(tauPlus, 1.0f, 1000.0f);
    pImpl->config.tauMinus = std::clamp(tauMinus, 1.0f, 1000.0f);
}

void STDP::setVariant(STDPVariant variant) {
    pImpl->variant = variant;
}

STDPVariant STDP::getVariant() const {
    return pImpl->variant;
}

void STDP::configure(const STDPConfig& config) {
    pImpl->config = config;
    validateParameters();
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    configure(STDPConfig{ltpWeight, ltdWeight, tau, tau});
}

STDPConfig STDP::getConfig() const {
    return pImpl->config;
}

void STDP::enableLearningRateAdaptation(bool enable, float gain) {
    pImpl->config.enableLearningRateAdaptation = enable;
    pImpl->config.predictionErrorGain = gain;
}

void STDP::enableEfficacyModulation(bool enable) {
    pImpl->config.enableEfficacyModulation = enable;
}

void STDP::enableEligibilityTraces(bool enable, float decayRate) {
    pImpl->config.enableEligibilityTraces = enable;
    pImpl->config.eligibilityDecayRate = decayRate;
}

void STDP::enableBatchProcessing(bool enable, size_t maxBatchSize) {
    pImpl->config.useBatchProcessing = enable;
    pImpl->config.maxBatchSize = maxBatchSize;
}

void STDP::enableParallelProcessing(bool enable) {
    pImpl->config.useParallelProcessing = enable;
}

void STDP::enableProfiling(bool enable) {
    pImpl->config.enableProfiling = enable;
}

float STDP::getLastUpdateTime() const {
    return pImpl->lastUpdateTime;
}

size_t STDP::getSpikePairCount() const {
    return pImpl->spikePairCount;
}

float STDP::getAverageUpdateTime() const {
    if (pImpl->spikePairCount == 0) return 0.0f;
    return pImpl->totalUpdateTime / static_cast<float>(pImpl->spikePairCount);
}

void STDP::setPredictionError(float error) {
    pImpl->predictionError = error;
}

void STDP::resetPredictionError() {
    pImpl->predictionError = 0.0f;
}

float STDP::calculateLearningRate(float baseRate) const {
    const auto& cfg = pImpl->config;
    
    if (!cfg.enableLearningRateAdaptation) {
        return baseRate;
    }
    
    // Adapt learning rate based on prediction error
    float errorFactor = 1.0f + cfg.predictionErrorGain * std::abs(pImpl->predictionError);
    
    // Clamp to reasonable bounds
    float adaptiveRate = baseRate * errorFactor;
    adaptiveRate = std::clamp(adaptiveRate, cfg.wMin, cfg.wMax);
    
    return adaptiveRate;
}

// RewardModulatedSTDP implementation
struct RewardModulatedSTDP::Impl {
    STDPConfig config;
    float rewardBaseline;
    float rewardModGain;
    
    Impl(const STDPConfig& cfg) : config(cfg), rewardBaseline(0.0f), rewardModGain(1.0f) {}
};

RewardModulatedSTDP::RewardModulatedSTDP(const STDPConfig& config) 
    : pImpl(std::make_unique<Impl>(config)) {
}

RewardModulatedSTDP::~RewardModulatedSTDP() = default;

void RewardModulatedSTDP::update(Synapse* synapse,
                                const std::vector<Timestamp>& preSpikes,
                                const std::vector<Timestamp>& postSpikes,
                                TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate reward-modulated STDP
    // First compute standard STDP contribution
    STDP standardSTDP(pImpl->config);
    standardSTDP.enableLearningRateAdaptation(false);
    
    // Calculate reward factor
    float rewardFactor = (pImpl->predictionError - pImpl->rewardBaseline) / pImpl->rewardModGain + 1.0f;
    
    // Apply standard STDP and then modulate by reward
    standardSTDP.update(synapse, preSpikes, postSpikes, dt);
    
    // Get the resulting weight change
    float delta = synapse->getEligibilityTrace();  // Approximate
    
    // Apply reward modulation
    float modulatedDelta = delta * rewardFactor;
    
    // Apply modulated weight change
    applyWeightChange(synapse, modulatedDelta);
}

void RewardModulatedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply weight change with reward modulation safety checks
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->config.wMin, pImpl->config.wMax);
    
    // Additional safety for reward-modulated learning
    if (std::abs(delta) > 0.5f) {
        newWeight = synapse->getWeight() + delta * 0.5f;  // Scale down large reward effects
        newWeight = std::clamp(newWeight, pImpl->config.wMin, pImpl->config.wMax);
    }
    
    synapse->setWeight(newWeight);
}

void RewardModulatedSTDP::setRewardBaseline(float reward) {
    pImpl->rewardBaseline = reward;
}

void RewardModulatedSTDP::setRewardModGain(float gain) {
    pImpl->rewardModGain = std::max(gain, 0.1f);  // Prevent division by zero
}

float RewardModulatedSTDP::getRewardBaseline() const {
    return pImpl->rewardBaseline;
}

float RewardModulatedSTDP::getRewardModGain() const {
    return pImpl->rewardModGain;
}

// TripletSTDP implementation
struct TripletSTDP::Impl {
    STDPConfig config;
    float gluingTime;
    
    Impl(const STDPConfig& cfg) : config(cfg), gluingTime(30.0f) {}
};

TripletSTDP::TripletSTDP(const STDPConfig& config) 
    : pImpl(std::make_unique<Impl>(config)) {
    pImpl->gluingTime = config.tripletGluingTime;
}

TripletSTDP::~TripletSTDP() = default;

void TripletSTDP::update(Synapse* synapse,
                        const std::vector<Timestamp>& preSpikes,
                        const std::vector<Timestamp>& postSpikes,
                        TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Implement triplet STDP with higher-order spike interactions
    computeTripletSTDP(synapse, preSpikes, postSpikes);
}

void TripletSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->config.wMin, pImpl->config.wMax);
    synapse->setWeight(newWeight);
}

void TripletSTDP::setGluingTime(float time) {
    pImpl->gluingTime = std::clamp(time, 1.0f, 1000.0f);
}

float TripletSTDP::getGluingTime() const {
    return pImpl->gluingTime;
}

} // namespace nlm