#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float learningThreshold;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             learningThreshold(LEARNING_THRESHOLD) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

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
     * For spike-based systems with spike trains:
     * Δw = η * (rate_coactivity - rate_pre * rate_post)
     * 
     * Where:
     *   rate_coactivity = average correlation rate over recent history
     *   rate_pre = pre-synaptic firing rate
     *   rate_post = post-synaptic firing rate
     *   η = learning rate
     * 
     * Biological inspiration:
     *   - Reflects BCM (Bienenstock-Cooper-Munro) theory
     *   - Achieves spike timing dependent potentiation when both active
     *   - Includes LTD to prevent runaway potentiation
     *   - Activity-dependent threshold for learning
     *   
     * Limitations:
     *   - Simplified covariance calculation
     *   - Fixed time window for rate calculation
     *   - Doesn't account for synaptic eligibility traces
     */
    
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate firing rates (spikes per second)
    float preRate = static_cast<float>(preSpikes.size()) / (Impl::COHERENCE_WINDOW_SECONDS);
    float postRate = static_cast<float>(postSpikes.size()) / (Impl::COHERENCE_WINDOW_SECONDS);
    
    // Calculate correlation rate (co-activity)
    // Count spikes within a narrow window as correlated
    size_t correlatedPairs = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = static_cast<float>(postTime - preTime);
            if (std::abs(dt) < Impl::COHERENCE_WINDOW_MS) {
                correlatedPairs++;
            }
        }
    }
    
    float coactivityRate = static_cast<float>(correlatedPairs) / 
                          (Impl::COHERENCE_WINDOW_SECONDS * Impl::COHERENCE_WINDOW_SECONDS);
    
    // Compute covariance-based weight change
    // Δw = η * (coactivity - pre_rate * post_rate)
    float delta = pImpl->learningRate * (coactivityRate - preRate * postRate);
    
    // Apply BCM-like threshold: only learn when post is active and pre is more active
    float threshold = pImpl->learningThreshold * postRate;
    if (postRate > Impl::MIN_POST_RATE_FOR_LEARNING) {
        // Apply thresholded Hebbian rule
        if (preRate > threshold) {
            // Positive covariance -> LTP
            delta = std::max(0.0f, delta);
        } else if (preRate < threshold * 0.5f) {
            // Negative covariance -> LTD  
            delta = std::min(0.0f, delta);
        } else {
            // Near threshold: minimal learning
            delta = delta * 0.1f;
        }
        
        // Apply weight change with bounds
        if (std::abs(delta) > 1e-6f) {
            applyWeightChange(synapse, delta);
        }
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
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

    void Hebbian::setLearningThreshold(float threshold) {
        pImpl->learningThreshold = std::clamp(threshold, 0.0f, 1.0f);
    }

    float Hebbian::getLearningThreshold() const {
        return pImpl->learningThreshold;
    }

} // namespace nlm
