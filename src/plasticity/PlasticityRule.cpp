#include "PlasticityRule.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    // Learning parameters
    float learningRate;           // η - learning rate (0.001 - 0.1 typical)
    float maxWeight;              // Maximum synaptic weight
    float minWeight;              // Minimum synaptic weight
    float covarianceThreshold;    // Threshold for considering activity as correlated
    bool useNormalization;        // Whether to apply synaptic normalization
    
    // For covariance learning
    float runningMeanPre;         // Running average of pre-synaptic activity
    float runningMeanPost;        // Running average of post-synaptic activity
    float runningMeanProduct;     // Running average of product activity
    float timeConstant;           // Time constant for running averages
    
    // Synaptic history
    std::vector<float> preSpikeHistory;  // Recent pre-synaptic spike rates
    std::vector<float> postSpikeHistory; // Recent post-synaptic spike rates
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.1f), useNormalization(true),
             runningMeanPre(0.0f), runningMeanPost(0.0f), runningMeanProduct(0.0f),
             timeConstant(100.0f) {}  // 100ms time constant for running averages
};

// Constants for Hebbian rule behavior
namespace {
    constexpr float DEFAULT_LEARNING_RATE = 0.01f;
    constexpr float DEFAULT_MAX_WEIGHT = 1.0f;
    constexpr float DEFAULT_MIN_WEIGHT = -1.0f;
    constexpr float DEFAULT_COVARIANCE_THRESHOLD = 0.1f;
    constexpr float DEFAULT_TIME_CONSTANT = 100.0f;  // 100ms
    constexpr float WEIGHT_CLAMP_MAX = 10.0f;
    constexpr float WEIGHT_CLAMP_MIN = -10.0f;
    constexpr float NORMALIZATION_CLAMP_MIN = 0.0f;
    constexpr float NORMALIZATION_CLAMP_MAX = 1.0f;
    constexpr float CORRELATION_WINDOW = 50.0f;  // 50ms
    constexpr float MIN_DELTA_FOR_UPDATE = 1e-6f;
    constexpr float DECAY_FACTOR = 0.9f;
    constexpr size_t SPIKE_HISTORY_LIMIT = 100;
}

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    if (!synapse || !pImpl->useNormalization) {
        return;
    }
    
    // Compute instantaneous spike rates from spike history
    float preRate = computeSpikeRate(pImpl->preSpikeHistory);
    float postRate = computeSpikeRate(pImpl->postSpikeHistory);
    
    // Update running averages with exponential decay
    float decayFactor = std::exp(-dt / pImpl->timeConstant);
    pImpl->runningMeanPre = decayFactor * pImpl->runningMeanPre + (1.0f - decayFactor) * preRate;
    pImpl->runningMeanPost = decayFactor * pImpl->runningMeanPost + (1.0f - decayFactor) * postRate;
    
    // For covariance learning, we need correlation between pre and post activity
    // This simplified implementation uses spike timing patterns
    float covariance = computeCovariance(preSpikes, postSpikes, dt);
    
    // Apply learning rule: Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
    // This is the standard covariance rule for Hebbian learning
    float delta = pImpl->learningRate * (covariance - pImpl->runningMeanPre * pImpl->runningMeanPost);
    
    // Apply weight change with biological constraints
    if (std::abs(delta) > MIN_DELTA_FOR_UPDATE) {
        applyWeightChange(synapse, delta);
    }
    
    // Update spike history for next computation
    updateSpikeHistory(pImpl->preSpikeHistory, preSpikes, dt);
    updateSpikeHistory(pImpl->postSpikeHistory, postSpikes, dt);
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Apply weight change
    float newWeight = currentWeight + delta;
    
    // Apply biological constraints
    if (pImpl->useNormalization) {
        // Apply weight bounds
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        
        // Apply synaptic normalization (Oja's rule variant)
        // Normalization maintains stability and promotes competition
        float totalWeight = computeTotalWeight(synapse);
        if (totalWeight > 0.0f) {
            float normFactor = 1.0f / (1.0f + pImpl->learningRate * totalWeight);
            newWeight *= normFactor;
        }
    } else {
        // Simple bounds without normalization
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    }
    
    // Set the new weight
    synapse->setWeight(newWeight);
}

const char* HebbianRule::getName() const {
    return "Hebbian";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 0.1f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::setMaxWeight(float maxWeight) {
    pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
}

float HebbianRule::getMaxWeight() const {
    return pImpl->maxWeight;
}

void HebbianRule::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float HebbianRule::getMinWeight() const {
    return pImpl->minWeight;
}

void HebbianRule::setCovarianceThreshold(float threshold) {
    pImpl->covarianceThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float HebbianRule::getCovarianceThreshold() const {
    return pImpl->covarianceThreshold;
}

void HebbianRule::setUseNormalization(bool useNorm) {
    pImpl->useNormalization = useNorm;
}

bool HebbianRule::getUseNormalization() const {
    return pImpl->useNormalization;
}

// Helper functions for Hebbian learning
float HebbianRule::computeSpikeRate(const std::vector<float>& spikeHistory) const {
    if (spikeHistory.empty()) return 0.0f;
    
    // Simple moving average of spike rates
    float sum = std::accumulate(spikeHistory.begin(), spikeHistory.end(), 0.0f);
    return sum / spikeHistory.size();
}

void HebbianRule::updateSpikeHistory(std::vector<float>& history, 
                                    const std::vector<Timestamp>& spikes,
                                    TimestepDuration dt) {
    float instantaneousRate = static_cast<float>(spikes.size()) / static_cast<float>(dt);
    
    // Use exponential moving average for history
    if (history.empty()) {
        history.push_back(instantaneousRate);
    } else {
        // Decay old rates and add new one
        for (auto& rate : history) {
            rate *= DECAY_FACTOR;
        }
        history.push_back(instantaneousRate);
        
        // Keep history size manageable
        if (history.size() > SPIKE_HISTORY_LIMIT) {
            history.erase(history.begin());
        }
    }
}

float HebbianRule::computeCovariance(const std::vector<Timestamp>& preSpikes,
                                     const std::vector<Timestamp>& postSpikes,
                                     TimestepDuration dt) const {
    if (preSpikes.empty() || postSpikes.empty()) {
        return 0.0f;
    }
    
    // Compute covariance between pre- and post-synaptic spike trains
    // This is a simplified version - real implementations might use
    // cross-correlation functions or spike timing dependent plasticity
    
    // Calculate spike rates
    float preRate = static_cast<float>(preSpikes.size()) / static_cast<float>(dt);
    float postRate = static_cast<float>(postSpikes.size()) / static_cast<float>(dt);
    
    // Calculate correlation based on spike timing
    float correlation = 0.0f;
    
    // For each pre-synaptic spike, check for post-synaptic spikes within window
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt = std::abs(static_cast<float>(postTime) - static_cast<float>(preTime));
            if (dt < CORRELATION_WINDOW) {
                // Spike within correlation window contributes to correlation
                correlation += std::exp(-dt / CORRELATION_WINDOW);
            }
        }
    }
    
    return correlation / preSpikes.size();
}

float HebbianRule::computeTotalWeight(Synapse* synapse) const {
    // This would need to be implemented to collect all outgoing weights
    // For now, return a reasonable estimate based on average synaptic count
    constexpr float AVERAGE_SYNAPTIC_COUNT = 10.0f;
    return std::abs(synapse->getWeight()) * AVERAGE_SYNAPTIC_COUNT;
}

} // namespace nlm
