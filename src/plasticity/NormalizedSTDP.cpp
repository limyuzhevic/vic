// Normalized STDP implementation

#include "NormalizedSTDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <functional>

namespace nlm {

// Normalized STDP implementation namespace for internal helper functions
namespace NormalizedSTDPImpl {
    // Constants for Normalized STDP behavior
    namespace Constants {
        constexpr float DEFAULT_LTP_WEIGHT = 0.01f;
        constexpr float DEFAULT_LTD_WEIGHT = 0.012f;
        constexpr float DEFAULT_TIME_CONSTANT = 20.0f;  // ms
        constexpr float DEFAULT_MIN_WEIGHT = -1.0f;
        constexpr float DEFAULT_MAX_WEIGHT = 1.0f;
        constexpr float DEFAULT_SPIKE_WINDOW = 50.0f;   // ms
        constexpr float DEFAULT_NORMALIZATION_FACTOR = 0.1f;  // Learning rate normalization
        constexpr float EFFICIENCY_MIN = 0.0f;
        constexpr float EFFICIENCY_MAX = 2.0f;
        constexpr float ELIGIBILITY_TRACE_DECAY = 0.95f;
        constexpr float WEIGHT_DIVERGENCE_THRESHOLD = 0.5f;
        constexpr float STABILITY_THRESHOLD = 0.01f;
    }
    
    // Helper function to calculate spike time difference and apply appropriate learning
    float calculateWeightChangeForSpikePair(
        float delta_t, 
        float ltp_weight, 
        float ltd_weight, 
        float time_constant) {
        
        if (delta_t > 0.0f) {
            // Pre before post: POTENTIATION (LTP)
            return ltp_weight * std::exp(-delta_t / time_constant);
        } else if (delta_t < 0.0f) {
            // Post before pre: DEPRESSION (LTD)
            return -ltd_weight * std::exp(delta_t / time_constant);
        }
        return 0.0f;  // Simultaneous spikes: no change
    }
    
    // Compute normalized weight update with regularization
    float computeNormalizedWeightUpdate(
        float delta_t,
        float ltp_weight,
        float ltd_weight,
        float time_constant,
        float current_weight,
        float normalization_factor,
        float min_weight,
        float max_weight) {
        
        // Calculate raw STDP weight change
        float raw_delta = calculateWeightChangeForSpikePair(
            delta_t, ltp_weight, ltd_weight, time_constant);
        
        // Apply normalization to prevent weight divergence
        float normalized_delta = raw_delta * normalization_factor;
        
        // Apply regularization based on current weight
        float weight_magnitude = std::abs(current_weight);
        if (weight_magnitude > Constants::WEIGHT_DIVERGENCE_THRESHOLD) {
            // Scale down update for weights close to bounds
            normalized_delta *= (1.0f - (weight_magnitude - Constants::WEIGHT_DIVERGENCE_THRESHOLD));
        }
        
        // Ensure stability
        if (std::abs(normalized_delta) > Constants::STABILITY_THRESHOLD) {
            normalized_delta = std::copysign(Constants::STABILITY_THRESHOLD, normalized_delta);
        }
        
        return normalized_delta;
    }
    
    // Efficient spike pairing with normalization for Normalized STDP
    float computeEfficientSpikePairingWithNormalization(
        const std::vector<Timestamp>& preSpikes,
        const std::vector<Timestamp>& postSpikes,
        float ltp_weight,
        float ltd_weight,
        float time_constant,
        float normalization_factor,
        float spike_window = Constants::DEFAULT_SPIKE_WINDOW,
        float min_weight = Constants::DEFAULT_MIN_WEIGHT,
        float max_weight = Constants::DEFAULT_MAX_WEIGHT) {
        
        if (preSpikes.empty() || postSpikes.empty()) {
            return 0.0f;
        }
        
        // For very small spike sets, use brute force approach
        if (preSpikes.size() <= 5 || postSpikes.size() <= 5) {
            float total_delta = 0.0f;
            for (Timestamp preTime : preSpikes) {
                for (Timestamp postTime : postSpikes) {
                    float dt = static_cast<float>(postTime - preTime);
                    if (std::abs(dt) <= spike_window) {
                        total_delta += computeNormalizedWeightUpdate(
                            dt, ltp_weight, ltd_weight, time_constant, 0.0f,
                            normalization_factor, min_weight, max_weight);
                    }
                }
            }
            return total_delta;
        }
        
        // For larger spike sets, use optimized approach
        std::vector<Timestamp> sortedPreSpikes = preSpikes;
        std::vector<Timestamp> sortedPostSpikes = postSpikes;
        std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
        std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
        
        float total_delta = 0.0f;
        size_t preIdx = 0;
        size_t postIdx = 0;
        
        while (preIdx < sortedPreSpikes.size() && postIdx < sortedPostSpikes.size()) {
            float dt = static_cast<float>(sortedPostSpikes[postIdx] - sortedPreSpikes[preIdx]);
            
            if (std::abs(dt) <= spike_window) {
                total_delta += computeNormalizedWeightUpdate(
                    dt, ltp_weight, ltd_weight, time_constant, 0.0f,
                    normalization_factor, min_weight, max_weight);
                
                preIdx++;
                postIdx++;
            } else if (dt < -spike_window) {
                postIdx++;
            } else {
                preIdx++;
            }
        }
        
        return total_delta;
    }
    
    // Validate and clamp synaptic weight changes
    float clampWeightChange(float current_weight, float delta, 
                           float min_weight, float max_weight) {
        float new_weight = current_weight + delta;
        return std::clamp(new_weight, min_weight, max_weight);
    }
    
    // Validate spike vector for Normalized STDP processing
    bool validateSpikesForNormalizedSTDP(const std::vector<Timestamp>& preSpikes,
                                         const std::vector<Timestamp>& postSpikes) {
        if (preSpikes.empty() || postSpikes.empty()) {
            return false;
        }
        
        // Validate all timestamps are non-negative
        for (Timestamp t : preSpikes) {
            if (t < 0) return false;
        }
        for (Timestamp t : postSpikes) {
            if (t < 0) return false;
        }
        
        return true;
    }
    
    // Compute eligibility trace update with normalization
    float computeEligibilityTraceUpdateWithNormalization(float previous_trace, 
                                                        float delta_weight) {
        return previous_trace + delta_weight * Constants::ELIGIBILITY_TRACE_DECAY;
    }
    
    // Compute weight normalization factor based on recent learning history
    float computeLearningRateNormalization(
        const std::vector<float>& recent_deltas,
        float target_stability = 0.01f,
        float max_factor = 2.0f) {
        
        if (recent_deltas.empty()) {
            return Constants::DEFAULT_NORMALIZATION_FACTOR;
        }
        
        // Calculate average magnitude of recent weight changes
        float sum_abs_delta = std::accumulate(recent_deltas.begin(), recent_deltas.end(), 0.0f,
                                              [](float sum, float val) { return sum + std::abs(val); });
        float avg_abs_delta = sum_abs_delta / recent_deltas.size();
        
        // Adjust normalization factor based on learning history
        float normalization_factor = Constants::DEFAULT_NORMALIZATION_FACTOR;
        
        if (avg_abs_delta > target_stability) {
            // Reduce learning rate if too volatile
            normalization_factor = std::max(target_stability / avg_abs_delta, 
                                           Constants::DEFAULT_NORMALIZATION_FACTOR * 0.5f);
        } else if (avg_abs_delta < target_stability * 0.5f) {
            // Increase learning rate if too stable
            normalization_factor = std::min(avg_abs_delta / target_stability, max_factor);
        }
        
        return normalization_factor;
    }
}}

struct NormalizedSTDP::Impl {
    float ltpWeight;           // A+ for potentiation
    float ltdWeight;           // A- for depression
    float timeConstant;        // Tau for exponential window (ms)
    float minWeight;           // Minimum synaptic weight
    float maxWeight;           // Maximum synaptic weight
    float spikeWindow;         // Time window for spike correlation (ms)
    float normalizationFactor; // Learning rate normalization factor
    
    Impl() : ltpWeight(NormalizedSTDPImpl::Constants::DEFAULT_LTP_WEIGHT),
             ltdWeight(NormalizedSTDPImpl::Constants::DEFAULT_LTD_WEIGHT),
             timeConstant(NormalizedSTDPImpl::Constants::DEFAULT_TIME_CONSTANT),
             minWeight(NormalizedSTDPImpl::Constants::DEFAULT_MIN_WEIGHT),
             maxWeight(NormalizedSTDPImpl::Constants::DEFAULT_MAX_WEIGHT),
             spikeWindow(NormalizedSTDPImpl::Constants::DEFAULT_SPIKE_WINDOW),
             normalizationFactor(NormalizedSTDPImpl::Constants::DEFAULT_NORMALIZATION_FACTOR) {}
};

NormalizedSTDP::NormalizedSTDP() : pImpl(new Impl) {
    if (!pImpl) {
        throw std::bad_alloc();  // Handle memory allocation failure
    }
}

NormalizedSTDP::~NormalizedSTDP() = default;

// Unit-testable function: Compute normalized weight change from spike timing
float NormalizedSTDP::computeNormalizedWeightChange(
    const std::vector<Timestamp>& preSpikes,
    const std::vector<Timestamp>& postSpikes,
    float normalization_factor,
    float current_weight) const {
    
    if (!NormalizedSTDPImpl::validateSpikesForNormalizedSTDP(preSpikes, postSpikes)) {
        return 0.0f;
    }
    
    return NormalizedSTDPImpl::computeEfficientSpikePairingWithNormalization(
        preSpikes, postSpikes,
        pImpl->ltpWeight, pImpl->ltdWeight,
        pImpl->timeConstant, normalization_factor,
        pImpl->spikeWindow, pImpl->minWeight, pImpl->maxWeight);
}

// Unit-testable function: Compute learning rate normalization based on history
float NormalizedSTDP::computeLearningRateNormalization(
    const std::vector<float>& recent_deltas) const {
    
    return NormalizedSTDPImpl::computeLearningRateNormalization(recent_deltas);
}

// Unit-testable function: Check if weight update would cause divergence
bool NormalizedSTDP::wouldCauseDivergence(float current_weight, float delta) const {
    float new_weight = current_weight + delta;
    float weight_magnitude = std::abs(new_weight);
    
    if (weight_magnitude > 1.0f) {
        return true; // Would exceed bounds
    }
    
    if (std::abs(delta) > NormalizedSTDPImpl::Constants::STABILITY_THRESHOLD) {
        return true; // Unstable update
    }
    
    return false;
}

void NormalizedSTDP::update(Synapse* synapse,
                            const std::vector<Timestamp>& preSpikes,
                            const std::vector<Timestamp>& postSpikes,
                            TimestepDuration dt) {
    /*
     * Normalized STDP implementation
     * 
     * Mathematical formulation with normalization:
     * For each pre-post spike pair with timing difference Δt = t_post - t_pre:
     * 
     * If Δt > 0 (pre before post): POTENTIATION
     *   Δw = A+ * exp(-Δt / τ+) * N(Δw)
     *   
     * If Δt < 0 (post before pre): DEPRESSION
     *   Δw = -A- * exp(Δt / τ-) * N(Δw)
     *   
     * Where:
     *   N(Δw) is normalization factor to prevent weight divergence
     *   N(Δw) = 1.0 if weight magnitude is small
     *   N(Δw) = (1.0 - (weight_magnitude - threshold)) otherwise
     */
    
    if (!synapse || !NormalizedSTDPImpl::validateSpikesForNormalizedSTDP(preSpikes, postSpikes) || dt <= 0) {
        return;
    }
    
    // Compute weight change using normalized spike pairing algorithm
    float totalDelta = computeNormalizedWeightChange(
        preSpikes, postSpikes, pImpl->normalizationFactor, synapse->getWeight());
    
    // Apply weight change with bounds checking and normalization
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        efficacy = std::clamp(efficacy, NormalizedSTDPImpl::Constants::EFFICIENCY_MIN, 
                              NormalizedSTDPImpl::Constants::EFFICIENCY_MAX);
        totalDelta *= efficacy;
        
        // Check for potential divergence
        if (wouldCauseDivergence(synapse->getWeight(), totalDelta)) {
            // Reduce update magnitude to prevent divergence
            float weight_magnitude = std::abs(synapse->getWeight());
            float reduction_factor = 1.0f - (weight_magnitude * 0.5f);
            totalDelta *= std::max(reduction_factor, 0.1f);
        }
        
        // Apply weight change using clamped result
        float newWeight = NormalizedSTDPImpl::clampWeightChange(
            synapse->getWeight(), totalDelta, pImpl->minWeight, pImpl->maxWeight);
        
        // Only apply if weight actually changed
        if (std::abs(newWeight - synapse->getWeight()) > 1e-8f) {
            synapse->setWeight(newWeight);
            
            // Update eligibility trace with normalization
            float currentTrace = synapse->getEligibilityTrace();
            float newTrace = NormalizedSTDPImpl::computeEligibilityTraceUpdateWithNormalization(currentTrace, totalDelta);
            synapse->setEligibilityTrace(newTrace);
        }
    }
}

void NormalizedSTDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = NormalizedSTDPImpl::clampWeightChange(
        synapse->getWeight(), delta, pImpl->minWeight, pImpl->maxWeight);
    
    // Apply weight change if it actually changed
    if (std::abs(newWeight - synapse->getWeight()) > 1e-8f) {
        synapse->setWeight(newWeight);
    }
}

const char* NormalizedSTDP::getName() const {
    return "NormalizedSTDP";
}

void NormalizedSTDP::setLTPWeight(float weight) {
    pImpl->ltpWeight = std::clamp(weight, 0.0f, 1.0f);
}

float NormalizedSTDP::getLTPWeight() const {
    return pImpl->ltpWeight;
}

void NormalizedSTDP::setLTDWeight(float weight) {
    pImpl->ltdWeight = std::clamp(weight, 0.0f, 1.0f);
}

float NormalizedSTDP::getLTDWeight() const {
    return pImpl->ltdWeight;
}

void NormalizedSTDP::setTimeConstant(float tau) {
    pImpl->timeConstant = std::clamp(tau, 1.0f, 100.0f);
}

float NormalizedSTDP::getTimeConstant() const {
    return pImpl->timeConstant;
}

void NormalizedSTDP::setSpikeWindow(float window) {
    pImpl->spikeWindow = std::clamp(window, 1.0f, 500.0f);
}

float NormalizedSTDP::getSpikeWindow() const {
    return pImpl->spikeWindow;
}

void NormalizedSTDP::setNormalizationFactor(float factor) {
    pImpl->normalizationFactor = std::clamp(factor, 0.01f, 2.0f);
}

float NormalizedSTDP::getNormalizationFactor() const {
    return pImpl->normalizationFactor;
}

void NormalizedSTDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

void NormalizedSTDP::configureFull(float ltpWeight, float ltdWeight, float tau, 
                                   float spikeWindow, float normalizationFactor) {
    configure(ltpWeight, ltdWeight, tau);
    setSpikeWindow(spikeWindow);
    setNormalizationFactor(normalizationFactor);
}

// Apply adaptive normalization based on learning history
void NormalizedSTDP::applyAdaptiveNormalization(const std::vector<float>& recent_deltas) {
    if (!pImpl) return;
    
    float new_factor = computeLearningRateNormalization(recent_deltas);
    setNormalizationFactor(new_factor);
}

// Compute trace-based weight change with normalization
float NormalizedSTDP::computeTraceBasedWeightChangeNormalized(
    const std::vector<float>& preTrace,
    const std::vector<float>& postTrace,
    float dt) const {
    
    if (preTrace.empty() || postTrace.empty() || dt <= 0.0f) {
        return 0.0f;
    }
    
    // Compute cross-correlation with normalization
    float totalChange = 0.0f;
    size_t minSize = std::min(preTrace.size(), postTrace.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        // Apply normalization to trace values
        float normalized_pre = std::clamp(preTrace[i], -1.0f, 1.0f);
        float normalized_post = std::clamp(postTrace[i], -1.0f, 1.0f);
        totalChange += normalized_pre * normalized_post * dt;
    }
    
    return totalChange;
}

} // namespace nlm
