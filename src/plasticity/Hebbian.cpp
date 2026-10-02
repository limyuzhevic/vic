#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <unordered_map>
#include <cmath>

namespace nlm {

// Forward declaration for helper functions
float calculateAdvancedCovariance(const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 float learningRate, float threshold);
float calculateActivityDependentLearning(const std::vector<Timestamp>& preSpikes,
                                        const std::vector<Timestamp>& postSpikes,
                                        float baselineLearningRate);

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;
    
    // Advanced Hebbian features
    float normalizedLearningRate;
    float activityHistory;
    float weightHistory;
    float recentActivity;
    float metaplasticity;
    float homeostaticFactor;
    float spikeTimingWindow;
    
    // Covariance calculation parameters
    float correlationWindow;
    float baselineActivity;
    float activityVariability;
    
    // Synaptic tagging and capture
    float tagStrength;
    float captureThreshold;
    float decayRate;
    
    // Homeostatic mechanisms
    float targetFiringRate;
    float synapticCompensation;
    float synapticWeakening;
    
    Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f),
             covarianceThreshold(0.0f), normalizedLearningRate(0.01f),
             activityHistory(0.0f), weightHistory(0.0f), recentActivity(0.0f),
             metaplasticity(0.0f), homeostaticFactor(1.0f),
             spikeTimingWindow(50.0f), correlationWindow(100.0f),
             baselineActivity(0.0f), activityVariability(0.0f),
             tagStrength(0.0f), captureThreshold(0.5f), decayRate(0.95f),
             targetFiringRate(0.1f), synapticCompensation(0.0f),
             synapticWeakening(0.0f) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {}

Hebbian::~Hebbian::Hebbian() = default;

void Hebbian::update(Synapse* synapse,
                     const std::vector<Timestamp>& preSpikes,
                     const std::vector<Timestamp>& postSpikes,
                     TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Calculate advanced covariance-based learning
    float covarianceDelta = calculateAdvancedCovariance(preSpikes, postSpikes,
                                                         pImpl->learningRate, pImpl->covarianceThreshold);
    
    // Calculate activity-dependent learning rate
    float activityDelta = calculateActivityDependentLearning(preSpikes, postSpikes, pImpl->learningRate);
    
    // Calculate spike timing dependent Hebbian contribution
    float timingDelta = calculateSpikeTimingHebbian(preSpikes, postSpikes, pImpl->spikeTimingWindow);
    
    // Calculate homeostatic compensation
    float homeostaticDelta = calculateHomeostaticCompensation(synapse, preSpikes, postSpikes);
    
    // Combine all Hebbian mechanisms
    float totalDelta = covarianceDelta + activityDelta + timingDelta + homeostaticDelta;
    
    // Apply metaplasticity and normalization
    totalDelta = applyMetaplasticity(totalDelta, synapse, preSpikes, postSpikes);
    totalDelta = normalizeHebbianUpdate(totalDelta, synapse);
    
    // Apply synaptic tagging and capture
    totalDelta = applySynapticTagging(synapse, totalDelta);
    
    // Update activity and weight history
    pImpl->activityHistory = static_cast<float>(preSpikes.size() + postSpikes.size()) / 2.0f;
    pImpl->weightHistory = synapse->getWeight();
    
    // Apply final weight change with bounds
    applyHebbianWeightChangeWithHistory(synapse, totalDelta);
}

float Hebbian::calculateAdvancedCovariance(const std::vector<Timestamp>& preSpikes,
                                          const std::vector<Timestamp>& postSpikes,
                                          float learningRate, float threshold) {
    // Advanced covariance calculation based on spike timing and correlations
    float covariance = 0.0f;
    float baseline = 0.0f;
    float variance = 0.0f;
    
    // Calculate spike statistics
    float avgPreSpikes = static_cast<float>(preSpikes.size());
    float avgPostSpikes = static_cast<float>(postSpikes.size());
    
    // Calculate covariance based on temporal correlation
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            float dt = static_cast<float>(postSpikes[j] - preSpikes[i]);
            
            if (std::abs(dt) < pImpl->correlationWindow) {
                // Weight correlation based on temporal proximity
                float temporalWeight = std::exp(-std::abs(dt) / pImpl->correlationWindow);
                covariance += temporalWeight;
                baseline += temporalWeight * learningRate;
            }
        }
    }
    
    // Normalize covariance
    covariance = covariance / static_cast<float>(preSpikes.size() * postSpikes.size());
    baseline = baseline / static_cast<float>(preSpikes.size() * postSpikes.size());
    
    // Apply threshold to prevent weak correlations from causing learning
    float delta = (covariance - baseline) * learningRate;
    if (std::abs(delta) < threshold) {
        delta = 0.0f;
    }
    
    return delta;
}

float Hebbian::calculateActivityDependentLearning(const std::vector<Timestamp>& preSpikes,
                                                  const std::vector<Timestamp>& postSpikes,
                                                  float baselineLearningRate) {
    // Activity-dependent learning rate based on recent network activity
    float recentActivity = static_cast<float>(preSpikes.size() + postSpikes.size()) / 2.0f;
    
    // Adapt learning rate based on activity level
    float activityFactor = 1.0f;
    if (recentActivity > pImpl->activityHistory * 2.0f) {
        // High activity: reduce learning rate to prevent runaway potentiation
        activityFactor = 0.7f;
    } else if (recentActivity < pImpl->activityHistory * 0.5f) {
        // Low activity: increase learning rate to promote exploration
        activityFactor = 1.3f;
    }
    
    // Apply metaplasticity
    activityFactor *= (1.0f + pImpl->metaplasticity);
    
    return baselineLearningRate * activityFactor;
}

float Hebbian::calculateSpikeTimingHebbian(const std::vector<Timestamp>& preSpikes,
                                          const std::vector<Timestamp>& postSpikes,
                                          float timingWindow) {
    // Spike timing dependent Hebbian learning (integrates with STDP-like timing)
    float timingDelta = 0.0f;
    
    for (size_t i = 0; i < preSpikes.size(); ++i) {
        for (size_t j = 0; j < postSpikes.size(); ++j) {
            float dt = static_cast<float>(postSpikes[j] - preSpikes[i]);
            
            if (std::abs(dt) < timingWindow) {
                // Hebbian strengthening for correlated spikes
                float timingWeight = std::exp(-std::abs(dt) / timingWindow);
                
                if (dt > 0) {
                    // Pre before post: typical Hebbian potentiation
                    timingDelta += timingWeight * pImpl->learningRate * 0.5f;
                } else {
                    // Post before pre: weaker anti-Hebbian effect
                    timingDelta += -timingWeight * pImpl->learningRate * 0.2f;
                }
            }
        }
    }
    
    return timingDelta;
}

float Hebbian::calculateHomeostaticCompensation(Synapse* synapse,
                                                const std::vector<Timestamp>& preSpikes,
                                                const std::vector<Timestamp>& postSpikes) {
    // Synaptic homeostasis to maintain network stability
    if (!synapse) return 0.0f;
    
    float currentWeight = synapse->getWeight();
    
    // Calculate firing rate based on spike activity
    float firingRate = static_cast<float>(preSpikes.size() + postSpikes.size()) / 2.0f;
    
    // Homeostatic compensation to maintain target firing rate
    float homeostaticDelta = 0.0f;
    if (firingRate > pImpl->targetFiringRate * 1.5f) {
        // Too much firing: weaken synapses
        homeostaticDelta = -(currentWeight - pImpl->minWeight) * pImpl->synapticWeakening;
    } else if (firingRate < pImpl->targetFiringRate * 0.5f) {
        // Too little firing: strengthen synapses
        homeostaticDelta = (pImpl->maxWeight - currentWeight) * pImpl->synapticCompensation;
    }
    
    return homeostaticDelta;
}

float Hebbian::applyMetaplasticity(float delta, Synapse* synapse,
                                   const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes) {
    // Apply metaplasticity: adapt learning based on recent experience
    float metaplasticFactor = 1.0f + pImpl->metaplasticity;
    
    // Activity-dependent metaplasticity
    float recentActivity = static_cast<float>(preSpikes.size() + postSpikes.size()) / 2.0f;
    if (recentActivity > pImpl->activityHistory * 2.0f) {
        // High activity: increase metaplasticity to stabilize learning
        pImpl->metaplasticity = std::min(1.0f, pImpl->metaplasticity + 0.01f);
    } else if (recentActivity < pImpl->activityHistory * 0.5f) {
        // Low activity: decrease metaplasticity to allow more exploration
        pImpl->metaplasticity = std::max(0.0f, pImpl->metaplasticity - 0.01f);
    }
    
    return delta * metaplasticFactor;
}

float Hebbian::normalizeHebbianUpdate(float delta, Synapse* synapse) {
    // Normalize Hebbian weight changes to prevent runaway potentiation
    if (!synapse) return delta;
    
    float currentWeight = synapse->getWeight();
    
    // Normalize based on current weight
    float normFactor = pImpl->normalizedLearningRate;
    
    // Stronger normalization near weight bounds
    if (std::abs(currentWeight - pImpl->maxWeight) < 0.1f) {
        normFactor *= 0.5f;
    } else if (std::abs(currentWeight - pImpl->minWeight) < 0.1f) {
        normFactor *= 0.5f;
    }
    
    // Apply normalization
    return delta * normFactor;
}

float Hebbian::applySynapticTagging(Synapse* synapse, float delta) {
    // Synaptic tagging and capture mechanism
    if (!synapse) return delta;
    
    // Update tag strength
    pImpl->tagStrength = std::min(1.0f, pImpl->tagStrength + std::abs(delta) * 0.1f);
    
    // Decay tags over time
    pImpl->tagStrength *= pImpl->decayRate;
    
    // Apply tagging to weight change
    float taggedDelta = delta * pImpl->tagStrength;
    
    // Check if tagging exceeds threshold for capture
    if (pImpl->tagStrength > pImpl->captureThreshold) {
        // Enhanced weight change due to tagging
        taggedDelta *= 1.5f;
    }
    
    return taggedDelta;
}

void Hebbian::applyHebbianWeightChangeWithHistory(Synapse* synapse, float delta) {
    if (!synapse) return;
    
    // Apply Hebbian weight change with eligibility traces and history
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Clamp weight to bounds
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    
    // Update synapse weight
    synapse->setWeight(newWeight);
    
    // Update eligibility trace for reward-modulated learning
    float eligibilityTrace = synapse->getEligibilityTrace();
    eligibilityTrace = eligibilityTrace * pImpl->decayRate + delta * 0.5f;
    synapse->setEligibilityTrace(eligibilityTrace);
    
    // Update baseline activity based on recent learning
    if (std::abs(delta) > 0.001f) {
        pImpl->baselineActivity = pImpl->activityHistory * 0.9f + pImpl->activityHistory * 0.1f;
    }
}

void Hebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    // Original simple implementation for compatibility
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

const char* Hebbian::getName() const {
    return "Advanced Hebbian";
}

void Hebbian::setLearningRate(float rate) {
    pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
    pImpl->normalizedLearningRate = std::clamp(rate * 0.8f, 0.0f, 0.8f);
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

void Hebbian::setCovarianceThreshold(float threshold) {
    pImpl->covarianceThreshold = threshold;
}

float Hebbian::getCovarianceThreshold() const {
    return pImpl->covarianceThreshold;
}

void Hebbian::setTargetFiringRate(float rate) {
    pImpl->targetFiringRate = std::clamp(rate, 0.01f, 1.0f);
}

float Hebbian::getTargetFiringRate() const {
    return pImpl->targetFiringRate;
}

void Hebbian::setSynapticCompensation(float compensation) {
    pImpl->synapticCompensation = std::clamp(compensation, 0.0f, 1.0f);
}

float Hebbian::getSynapticCompensation() const {
    return pImpl->synapticCompensation;
}

void Hebbian::setSynapticWeakening(float weakening) {
    pImpl->synapticWeakening = std::clamp(weakening, 0.0f, 1.0f);
}

float Hebbian::getSynapticWeakening() const {
    return pImpl->synapticWeakening;
}

void Hebbian::configure(float learningRate, float covarianceThreshold,
                        float targetFiringRate, float synapticCompensation,
                        float synapticWeakening) {
    setLearningRate(learningRate);
    setCovarianceThreshold(covarianceThreshold);
    setTargetFiringRate(targetFiringRate);
    setSynapticCompensation(synapticCompensation);
    setSynapticWeakening(synapticWeakening);
}

} // namespace nlm