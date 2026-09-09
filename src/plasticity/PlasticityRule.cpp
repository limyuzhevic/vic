#include "PlasticityRule.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

bool PlasticityRule::isEnabled() const {
    return enabled_;
}

void PlasticityRule::setEnabled(bool enabled) {
    enabled_ = enabled;
}

struct HebbianRule::Impl {
    // BCM (Bienenstock-Cooper-Munro) rule parameters
    float learningRate;           // Learning rate (η)
    float mu;                     // Steady-state calcium level
    float theta_m;                // Sliding modification threshold
    float theta_plus;             // Maximum potentiation threshold
    float theta_minus;            // Maximum depression threshold
    float calcium;                // Current calcium concentration
    float calciumDecay;           // Calcium decay rate
    float weightNormTarget;       // Target weight for normalization
    float weightNormRate;          // Weight normalization rate
    float neuromodulatorGating;    // Neuromodulator (e.g., dopamine) gating factor
    float metaplasticRate;         // Metaplastic learning rate adaptation
    float spikeHistorySize;        // Size of spike history buffer
    
    // Activity history for covariance computation
    std::vector<Timestamp> preSpikeHistory;
    std::vector<Timestamp> postSpikeHistory;
    
    // Statistical moments for covariance
    float preSpikeMean;           // Mean of pre-synaptic activity
    float postSpikeMean;          // Mean of post-synaptic activity
    float preSpikeVar;            // Variance of pre-synaptic activity
    float postSpikeVar;           // Variance of post-synaptic activity
    float prePostCov;             // Covariance between pre and post spikes
    
    // Calcium dynamics
    float calciumHistory[10];     // Recent calcium levels
    int calciumHistoryIdx;        // Index for circular buffer
    
    // Oja's rule normalization
    float ojaWeightSq;            // Square of current weight
    
    // Hebbian-LTD parameters
    float ltdThreshold;            // Threshold for long-term depression
    float ltdRate;                 // LTD learning rate
    
    // Spike pattern detection
    struct BurstInfo {
        Timestamp startTime;
        Timestamp endTime;
        size_t spikeCount;
    };
    BurstInfo currentBurst;
    
    Impl() : learningRate(0.01f), mu(0.5f), theta_m(0.8f), theta_plus(1.2f), theta_minus(0.2f),
             calcium(0.0f), calciumDecay(0.1f), weightNormTarget(1.0f), weightNormRate(0.01f),
             neuromodulatorGating(1.0f), metaplasticRate(0.001f), spikeHistorySize(1000.0f),
             ltdThreshold(0.3f), ltdRate(0.005f) {
        calciumHistoryIdx = 0;
        for (int i = 0; i < 10; ++i) calciumHistory[i] = 0.0f;
        ojaWeightSq = 1.0f;
    }
};

HebbianRule::HebbianRule() : pImpl(new Impl) {}

HebbianRule::~HebbianRule() = default;

void HebbianRule::update(Synapse* synapse,
                          const std::vector<Timestamp>& preSpikes,
                          const std::vector<Timestamp>& postSpikes,
                          TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty() || dt <= 0.0) {
        return;
    }
    
    // Update spike history
    pImpl->preSpikeHistory.insert(pImpl->preSpikeHistory.end(), 
                                 preSpikes.begin(), preSpikes.end());
    pImpl->postSpikeHistory.insert(pImpl->postSpikeHistory.end(), 
                                  postSpikes.begin(), postSpikes.end());
    
    // Maintain history size
    if (pImpl->preSpikeHistory.size() > pImpl->spikeHistorySize) {
        pImpl->preSpikeHistory.erase(pImpl->preSpikeHistory.begin());
    }
    if (pImpl->postSpikeHistory.size() > pImpl->spikeHistorySize) {
        pImpl->postSpikeHistory.erase(pImpl->postSpikeHistory.begin());
    }
    
    // Compute statistical moments for covariance rule
    computeCovariance(pImpl->preSpikeHistory, pImpl->preSpikeMean, 
                     pImpl->preSpikeVar, pImpl->spikeHistorySize);
    computeCovariance(pImpl->postSpikeHistory, pImpl->postSpikeMean, 
                     pImpl->postSpikeVar, pImpl->spikeHistorySize);
    computeCrossCovariance(pImpl->preSpikeHistory, pImpl->postSpikeHistory,
                          pImpl->prePostCov, pImpl->spikeHistorySize);
    
    // Update calcium dynamics
    updateCalciumDynamics(synapse);
    
    // Compute weight change using BCM rule with sliding threshold
    float delta = computeBCMWeightChange(pImpl->prePostCov, pImpl->calcium, 
                                        pImpl->theta_m, pImpl->learningRate);
    
    // Apply neuromodulator gating
    delta *= pImpl->neuromodulatorGating;
    
    // Apply metaplastic adaptation
    adaptLearningRate(delta);
    
    // Apply Oja's rule normalization if applicable
    delta = applyOjaNormalization(delta, synapse->getWeight());
    
    // Apply synaptic homeostasis (weight normalization)
    float currentWeight = synapse->getWeight();
    float normalizedWeight = normalizeWeight(currentWeight, pImpl->weightNormTarget,
                                            pImpl->weightNormRate);
    float normDelta = normalizedWeight - currentWeight;
    delta += normDelta;
    
    // Apply weight change
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
    
    // Detect and handle burst patterns
    detectAndHandleBursts(preSpikes, postSpikes);
    
    // Update sliding threshold (theta_m)
    updateSlidingThreshold(pImpl->postSpikeMean, pImpl->mu);
}

void HebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, -1.0f, 1.0f); // Keep weights bounded
    synapse->setWeight(newWeight);
    
    // Update Oja's rule weight square
    pImpl->ojaWeightSq = newWeight * newWeight;
}

const char* HebbianRule::getName() const {
    return "HebbianRule";
}

void HebbianRule::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
}

float HebbianRule::getLearningRate() const {
    return pImpl->learningRate;
}

void HebbianRule::Impl::updateSlidingThreshold(float postActivity, float mu) {
    theta_m = mu * (1.0f - std::exp(-postActivity * 10.0f));
}

void HebbianRule::Impl::updateCalciumDynamics(Synapse* synapse) {
    // Update calcium concentration based on post-synaptic spikes
    float calciumInput = static_cast<float>(postSpikeHistory.size()) * 0.1f;
    
    // Decay existing calcium
    calcium *= (1.0f - calciumDecay);
    
    // Add new calcium
    calcium += calciumInput;
    calcium = std::clamp(calcium, 0.0f, 2.0f);
    
    // Store in circular buffer
    calciumHistory[calciumHistoryIdx] = calcium;
    calciumHistoryIdx = (calciumHistoryIdx + 1) % 10;
}

void HebbianRule::computeCovariance(const std::vector<Timestamp>& spikes, 
                                   float& mean, float& variance, float n) {
    if (spikes.empty()) {
        mean = 0.0f;
        variance = 0.0f;
        return;
    }
    
    // Simple covariance approximation for spike trains
    // Use firing rate as proxy for spike activity
    mean = static_cast<float>(spikes.size()) / n;
    variance = mean * (1.0f - mean); // Binomial variance approximation
}

void HebbianRule::computeCrossCovariance(const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes,
                                        float& cov, float n) {
    if (preSpikes.empty() || postSpikes.empty()) {
        cov = 0.0f;
        return;
    }
    
    // Simplified cross-covariance computation
    // Count coincident spikes within a time window
    size_t coincidences = 0;
    float timeWindow = 10.0f; // 10ms window
    
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            if (std::abs(pre - post) < timeWindow) {
                ++coincidences;
            }
        }
    }
    
    cov = static_cast<float>(coincidences) / n;
}

float HebbianRule::computeBCMWeightChange(float covariance, float calcium, 
                                         float theta_m, float learningRate) {
    // BCM rule: Δw = φ(ξ - θ_m) * (calcium - θ_m)
    // where φ(x) = x if x > 0, 0 otherwise
    float delta = 0.0f;
    
    // Hebbian-LTD component
    if (calcium > theta_m) {
        // Potentiation
        float hebbianTerm = calcium - theta_m;
        delta = learningRate * hebbianTerm;
    } else {
        // Depression (Hebbian-LTD)
        if (calcium < pImpl->ltdThreshold) {
            float ltdTerm = -(pImpl->ltdThreshold - calcium);
            delta = pImpl->ltdRate * ltdTerm;
        }
    }
    
    // Add covariance-based modulation
    delta += learningRate * 0.5f * covariance;
    
    return delta;
}

void HebbianRule::adaptLearningRate(float delta) {
    // Metaplasticity: adapt learning rate based on recent weight changes
    pImpl->learningRate += pImpl->metaplasticRate * delta;
    pImpl->learningRate = std::clamp(pImpl->learningRate, 0.001f, 0.1f);
}

float HebbianRule::applyOjaNormalization(float delta, float weight) {
    // Oja's rule: Δw = η * (pre * post - w * pre^2 * post)
    // Simplified for our case
    if (pImpl->ojaWeightSq > 0.0f) {
        delta -= pImpl->learningRate * weight * delta / pImpl->ojaWeightSq;
    }
    return delta;
}

float HebbianRule::normalizeWeight(float weight, float target, float rate) {
    // Exponential moving toward target weight
    return target + (weight - target) * std::exp(-rate);
}

void HebbianRule::detectAndHandleBursts(const std::vector<Timestamp>& preSpikes,
                                       const std::vector<Timestamp>& postSpikes) {
    // Simple burst detection
    for (size_t i = 1; i < preSpikes.size(); ++i) {
        float dt = preSpikes[i] - preSpikes[i-1];
        if (dt < 5.0f) { // 5ms inter-spike interval for burst detection
            pImpl->currentBurst.spikeCount++;
            pImpl->currentBurst.endTime = preSpikes[i];
        }
    }
    
    // Apply burst-specific learning if detected
    if (pImpl->currentBurst.spikeCount > 2) {
        // Burst-enhanced Hebbian learning
        pImpl->learningRate *= 1.5f; // Boost learning during bursts
    }
    
    pImpl->currentBurst = Impl::BurstInfo(); // Reset
}

} // namespace nlm
