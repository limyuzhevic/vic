// STDP.cpp improvements implementation

#include "STDP.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace nlm {

// STDP implementation namespace for internal helper functions
namespace STDPImpl {
    // Constants for STDP behavior
    namespace Constants {
        constexpr float DEFAULT_LTP_WEIGHT = 0.01f;
        constexpr float DEFAULT_LTD_WEIGHT = 0.012f;
        constexpr float DEFAULT_TIME_CONSTANT = 20.0f;  // ms
        constexpr float DEFAULT_MIN_WEIGHT = -1.0f;
        constexpr float DEFAULT_MAX_WEIGHT = 1.0f;
        constexpr float DEFAULT_SPIKE_WINDOW = 50.0f;   // ms
        constexpr float EFFICIENCY_MIN = 0.0f;
        constexpr float EFFICIENCY_MAX = 2.0f;
        constexpr float ELIGIBILITY_TRACE_DECAY = 0.95f;
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
    
    // Efficient spike pairing with sliding window optimization
    float computeEfficientSpikePairing(
        const std::vector<Timestamp>& preSpikes,
        const std::vector<Timestamp>& postSpikes,
        float ltp_weight,
        float ltd_weight,
        float time_constant,
        float spike_window = Constants::DEFAULT_SPIKE_WINDOW) {
        
        if (preSpikes.empty() || postSpikes.empty()) {
            return 0.0f;
        }
        
        // For very small spike sets, use brute force approach (O(N²))
        if (preSpikes.size() <= 5 || postSpikes.size() <= 5) {
            float total_delta = 0.0f;
            for (Timestamp preTime : preSpikes) {
                for (Timestamp postTime : postSpikes) {
                    float dt = static_cast<float>(postTime - preTime);
                    // Apply temporal window filter for correlation
                    if (std::abs(dt) <= spike_window) {
                        total_delta += calculateWeightChangeForSpikePair(
                            dt, ltp_weight, ltd_weight, time_constant);
                    }
                }
            }
            return total_delta;
        }
        
        // For larger spike sets, use optimized approach
        // Sort spikes for window-based processing
        std::vector<Timestamp> sortedPreSpikes = preSpikes;
        std::vector<Timestamp> sortedPostSpikes = postSpikes;
        std::sort(sortedPreSpikes.begin(), sortedPreSpikes.end());
        std::sort(sortedPostSpikes.begin(), sortedPostSpikes.end());
        
        float total_delta = 0.0f;
        size_t preIdx = 0;
        size_t postIdx = 0;
        
        // Two-pointer sliding window approach for O(N log N + K) complexity
        while (preIdx < sortedPreSpikes.size() && postIdx < sortedPostSpikes.size()) {
            float dt = static_cast<float>(sortedPostSpikes[postIdx] - sortedPreSpikes[preIdx]);
            
            if (std::abs(dt) <= spike_window) {
                total_delta += calculateWeightChangeForSpikePair(
                    dt, ltp_weight, ltd_weight, time_constant);
                
                // Move both pointers to avoid double counting
                preIdx++;
                postIdx++;
            } else if (dt < -spike_window) {
                // Post spike is too old, advance post pointer
                postIdx++;
            } else {
                // Pre spike is too old, advance pre pointer
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
    
    // Validate spike vector for STDP processing
    bool validateSpikesForSTDP(const std::vector<Timestamp>& preSpikes,
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
    
    // Compute eligibility trace update for reward-modulated learning
    float computeEligibilityTraceUpdate(float previous_trace, float delta_weight) {
        return previous_trace + delta_weight * Constants::ELIGIBILITY_TRACE_DECAY;
    }
}}

struct STDP::Impl {
    float ltpWeight;      // A+ for potentiation
    float ltdWeight;      // A- for depression
    float timeConstant;   // Tau for exponential window (ms)
    float minWeight;      // Minimum synaptic weight
    float maxWeight;      // Maximum synaptic weight
    float spikeWindow;    // Time window for spike correlation (ms)
    
    Impl() : ltpWeight(STDPImpl::Constants::DEFAULT_LTP_WEIGHT),
             ltdWeight(STDPImpl::Constants::DEFAULT_LTD_WEIGHT),
             timeConstant(STDPImpl::Constants::DEFAULT_TIME_CONSTANT),
             minWeight(STDPImpl::Constants::DEFAULT_MIN_WEIGHT),
             maxWeight(STDPImpl::Constants::DEFAULT_MAX_WEIGHT),
             spikeWindow(STDPImpl::Constants::DEFAULT_SPIKE_WINDOW) {}
};

STDP::STDP() : pImpl(new Impl) {
    if (!pImpl) {
        throw std::bad_alloc();  // Handle memory allocation failure
    }
}

STDP::~STDP() = default;

// Unit-testable function: Compute weight change from spike timing
float STDP::computeWeightChange(
    const std::vector<Timestamp>& preSpikes,
    const std::vector<Timestamp>& postSpikes) const {
    
    if (!STDPImpl::validateSpikesForSTDP(preSpikes, postSpikes)) {
        return 0.0f;
    }
    
    return STDPImpl::computeEfficientSpikePairing(
        preSpikes, postSpikes, 
        pImpl->ltpWeight, pImpl->ltdWeight, 
        pImpl->timeConstant, pImpl->spikeWindow);
}

// Unit-testable function: Clamp weight to valid range
float STDP::clampWeight(float weight) const {
    return std::clamp(weight, pImpl->minWeight, pImpl->maxWeight);
}

// Unit-testable function: Check if spike pairing is within time window
bool STDP::isWithinTimeWindow(float dt) const {
    return std::abs(dt) <= pImpl->spikeWindow;
}

void STDP::update(Synapse* synapse,
                   const std::vector<Timestamp>& preSpikes,
                   const std::vector<Timestamp>& postSpikes,
                   TimestepDuration dt) {
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
     */
    
    if (!synapse || !STDPImpl::validateSpikesForSTDP(preSpikes, postSpikes) || dt <= 0) {
        return;
    }
    
    // Compute weight change using efficient spike pairing algorithm
    float totalDelta = computeWeightChange(preSpikes, postSpikes);
    
    // Apply weight change with bounds checking
    if (std::abs(totalDelta) > 1e-6f) {
        // Scale by synaptic efficacy if available
        float efficacy = synapse->getEfficacy();
        efficacy = std::clamp(efficacy, STDPImpl::Constants::EFFICIENCY_MIN, 
                              STDPImpl::Constants::EFFICIENCY_MAX);
        totalDelta *= efficacy;
        
        // Apply weight change using clamped result
        float newWeight = STDPImpl::clampWeightChange(
            synapse->getWeight(), totalDelta, pImpl->minWeight, pImpl->maxWeight);
        
        // Only apply if weight actually changed
        if (std::abs(newWeight - synapse->getWeight()) > 1e-8f) {
            synapse->setWeight(newWeight);
            
            // Update eligibility trace for reward-modulated learning
            float currentTrace = synapse->getEligibilityTrace();
            float newTrace = STDPImpl::computeEligibilityTraceUpdate(currentTrace, totalDelta);
            synapse->setEligibilityTrace(newTrace);
        }
    }
}

void STDP::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Clamp weight to bounds
    float newWeight = STDPImpl::clampWeightChange(
        synapse->getWeight(), delta, pImpl->minWeight, pImpl->maxWeight);
    
    // Apply weight change if it actually changed
    if (std::abs(newWeight - synapse->getWeight()) > 1e-8f) {
        synapse->setWeight(newWeight);
    }
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

void STDP::setSpikeWindow(float window) {
    pImpl->spikeWindow = std::clamp(window, 1.0f, 500.0f);
}

float STDP::getSpikeWindow() const {
    return pImpl->spikeWindow;
}

void STDP::configure(float ltpWeight, float ltdWeight, float tau) {
    setLTPWeight(ltpWeight);
    setLTDWeight(ltdWeight);
    setTimeConstant(tau);
}

// Homeostatic adaptation for learning rate
void STDP::applyHomeostaticAdaptation(float targetWeight, float learningRate) {
    if (!pImpl) return;
    
    float currentWeight = pImpl->ltpWeight + pImpl->ltdWeight;
    float deviation = targetWeight - currentWeight;
    
    // Apply homeostatic learning rate adjustment
    float adaptiveRate = learningRate * (1.0f + std::abs(deviation));
    
    // Adjust weights towards target
    float weightAdjustment = adaptiveRate * deviation * 0.01f;
    
    setLTPWeight(pImpl->ltpWeight + weightAdjustment);
    setLTDWeight(pImpl->ltdWeight + weightAdjustment);
}

// Compute synaptic weight change from spike traces
float STDP::computeTraceBasedWeightChange(
    const std::vector<float>& preTrace,
    const std::vector<float>& postTrace,
    float dt) const {
    
    if (preTrace.empty() || postTrace.empty() || dt <= 0.0f) {
        return 0.0f;
    }
    
    // Compute cross-correlation between pre and post traces
    float totalChange = 0.0f;
    size_t minSize = std::min(preTrace.size(), postTrace.size());
    
    for (size_t i = 0; i < minSize; ++i) {
        totalChange += preTrace[i] * postTrace[i] * dt;
    }
    
    return totalChange;
}

} // namespace nlm
