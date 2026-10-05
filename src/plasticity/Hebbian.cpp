#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;
    bool ojasRule;           // Enable Oja's rule normalization
    size_t totalUpdates;     // Total Hebbian updates
    float totalDelta;        // Sum of all weight changes
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), ojasRule(true),
             totalUpdates(0), totalDelta(0.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian() = default;

Hebbian::Hebbian(Hebbian&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

Hebbian& Hebbian::operator=(Hebbian&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Count spike coactivity with temporal window
    size_t correlationCount = 0;
    
    // Optimized: use firing rates instead of pairwise spike counting
    // More biologically plausible: correlated firing rates
    size_t preCount = preSpikes.size();
    size_t postCount = postSpikes.size();
    
    // Calculate correlation based on co-firing within biologically relevant window
    // Use exponential kernel for temporal correlation
    float correlation = 0.0f;
    float tau_corr = 50.0f;  // Correlation time constant (50ms)
    
    // For simplicity, use firing rates and compute covariance
    // More sophisticated implementations would use spike trains
    float preRate = static_cast<float>(preCount) / (dt_ms + 0.001f);
    float postRate = static_cast<float>(postCount) / (dt_ms + 0.001f);
    
    // Mean activity levels (simplified - could be learned)
    float meanPre = preRate * 0.5f;  // Simplified assumption
    float meanPost = postRate * 0.5f;
    
    // Covariance-based Hebbian rule
    float covariance = (preRate - meanPre) * (postRate - meanPost);
    
    // Apply Oja's rule if enabled
    float delta = pImpl->learningRate * covariance;
    
    if (pImpl->ojasRule) {
        // Oja's rule: Δw = η * (x * y - y * w^2)
        // Where x is pre-synaptic activity, y is post-synaptic output
        // This normalizes weights and prevents runaway potentiation
        
        float preActivity = std::min(preRate, 1.0f);
        float postActivity = std::min(postRate, 1.0f);
        float currentWeight = synapse->getWeight();
        
        float ojaTerm = preActivity * postActivity - postActivity * currentWeight * currentWeight;
        delta = pImpl->learningRate * ojaTerm;
        
        // Also add covariance term for bidirectional learning
        delta += pImpl->learningRate * covariance * 0.5f;
    }
    
    // Apply with bounds and normalization
    if (std::abs(delta) > 1e-8f) {
        // Apply weight change
        applyWeightChange(synapse, delta);
        
        // Update statistics
        ++pImpl->totalUpdates;
        pImpl->totalDelta += delta;
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Weight normalization if Oja's rule is enabled
    if (pImpl->ojasRule) {
        // Additional normalization to prevent runaway
        float normFactor = 1.0f / std::sqrt(1.0f + newWeight * newWeight);
        newWeight *= normFactor;
    }
    
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

void Hebbian::setMinWeight(float minWeight) {
    pImpl->minWeight = minWeight;
}

float Hebbian::getMinWeight() const {
    return pImpl->minWeight;
}

void Hebbian::enableOjasRule(bool enable) {
    pImpl->ojasRule = enable;
}

bool Hebbian::isOjasRuleEnabled() const {
    return pImpl->ojasRule;
}

void Hebbian::setCovarianceThreshold(float threshold) {
    pImpl->covarianceThreshold = threshold;
}

float Hebbian::getCovarianceThreshold() const {
    return pImpl->covarianceThreshold;
}

void Hebbian::setLearningRule(const std::string& ruleName) {
    // TODO: Implement different learning rules based on ruleName
    // Could be "Oja", "Hebb", "Covariance", "BCM", etc.
}

std::string Hebbian::getLearningRuleName() const {
    return pImpl->ojasRule ? "Oja's Rule" : "Standard Hebbian";
}

size_t Hebbian::getTotalHebbianUpdates() const {
    return pImpl->totalUpdates;
}

float Hebbian::getAverageHebbianChange() const {
    if (pImpl->totalUpdates == 0) return 0.0f;
    return pImpl->totalDelta / static_cast<float>(pImpl->totalUpdates);
}

} // namespace nlm
