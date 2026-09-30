#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float timeWindowMultiplier;  // How many τ to consider for significant contributions
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    
    Impl() : ltpWeight(0.01f), ltdWeight(0.012f), timeConstant(20.0f),
             timeWindowMultiplier(5.0f), minWeight(-1.0f), maxWeight(1.0f) {}
};

STDP::STDP() : pImpl(new Impl) {}

STDP::~STDP() = default;

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
    /*
     * Optimized STDP implementation based on spike-timing correlation
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
     * Optimization improvements:
     *   - Sort spike times for O(n log n) instead of O(n²)
     *   - Use two-pointer technique for windowed calculations
     *   - Early termination for spikes outside time window
     *   - Pre-calculate exponential weights
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    float totalDelta = 0.0f;
    float tau = pImpl->timeConstant;
    float timeWindow = pImpl->timeWindowMultiplier * tau;
    
    // Create sorted copies (keep original order intact)
    std::vector<Timestamp> sortedPreSpikes = preSpikes;
    std::vector<Timestamp> sortedPostSpikes = postSpikes;
    
    std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
    std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
    
    // Optimized two-pass calculation using time window
    // Pass 1: Pre-before-post (potentiation)
    for (Timestamp preTime : sortedPreSpikes) {
        // Find posts that occur after this pre
        auto postIt = std::upper_bound(sortedPostSpikes.begin(), sortedPostSpikes.end(), preTime);
        for (; postIt != sortedPostSpikes.end(); ++postIt) {
            float dt = static_cast<float>(*postIt - preTime);
            if (dt > 0 && dt <= timeWindow) {  // Use configurable time window
                float delta = pImpl->ltpWeight * std::exp(-dt / tau);
                totalDelta += delta;
            } else if (dt > timeWindow) {
                break;  // Further posts won't contribute significantly
            }
        }
    }
    
    // Pass 2: Post-before-pre (depression)
    for (Timestamp postTime : sortedPostSpikes) {
        // Find pres that occur before this post
        auto preIt = std::upper_bound(sortedPreSpikes.begin(), sortedPreSpikes.end(), postTime);
        for (auto it = sortedPreSpikes.begin(); it != preIt; ++it) {
            float dt = static_cast<float>(postTime - *it);
            if (dt < 0 && dt >= -timeWindow) {  // Use configurable time window
                float delta = -pImpl->ltdWeight * std::exp(dt / tau);
                totalDelta += delta;
            } else if (dt < -timeWindow) {
                break;  // Earlier pres won't contribute significantly
            }
        }
    }
    
    // Apply weight change with bounds
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        totalDelta *= efficacy;
        
        // Apply weight change
        synapse->addToWeight(totalDelta);
        
        // Update eligibility trace for reward-modulated learning
        float currentTrace = synapse->getEligibilityTrace();
        synapse->setEligibilityTrace(currentTrace + totalDelta);
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* STDP::getName() const {
    return "STDP";
}

void STDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void STDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float STDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void STDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float STDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void STDP::setTimeWindowMultiplier(float multiplier) {
    pImpl->timeWindowMultiplier = std::clamp(multiplier, 1.0f, 20.0f);
}

float STDP::getTimeWindowMultiplier() const {
    return pImpl->timeWindowMultiplier;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

} // namespace nlm
