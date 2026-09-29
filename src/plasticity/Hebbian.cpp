#include "Hebbian.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// EnhancedHebbian implementation
struct EnhancedHebbian::Impl {
    HebbianParameters parameters;
    size_t coactivityCount;
    float baselineActivity;
    std::vector<float> weightHistory;
    std::vector<float> activityHistory;
    
    Impl() : coactivityCount(0), baselineActivity(0.0f) {
        parameters = HebbianParameters();
    }
};

EnhancedHebbian::EnhancedHebbian() : pImpl(new Impl()) {}

EnhancedHebbian::~EnhancedHebbian() {
    delete pImpl;
}

float EnhancedHebbian::computeCovarianceHebbian(Synapse* synapse,
                                                 const std::vector<Timestamp>& preSpikes,
                                                 const std::vector<Timestamp>& postSpikes,
                                                 TimestepDuration dt) {
    // Compute spike statistics
    size_t preSpikeCount = preSpikes.size();
    size_t postSpikeCount = postSpikes.size();
    
    // Compute coactivity (correlated spike pairs)
    size_t coactivity = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt_val = static_cast<float>(postTime - preTime);
            if (std::abs(dt_val) <= pImpl->parameters.correlationWindow) {
                ++coactivity;
            }
        }
    }
    
    pImpl->coactivityCount = coactivity;
    
    // Compute correlation coefficient (simplified)
    float correlation = 0.0f;
    if (preSpikeCount > 0 && postSpikeCount > 0) {
        float expectedCoactivity = static_cast<float>(preSpikeCount * postSpikeCount) / 
                                 (pImpl->parameters.correlationWindow * 10.0f);
        if (expectedCoactivity > 0.0f) {
            correlation = static_cast<float>(coactivity) / expectedCoactivity;
        }
    }
    
    // Apply Hebbian rule: Δw = η+ * correlation for positive, η- * correlation for negative
    float weightChange = 0.0f;
    if (correlation > 0.0f) {
        weightChange = pImpl->parameters.learningRatePositive * correlation;
    } else if (correlation < 0.0f) {
        weightChange = pImpl->parameters.learningRateNegative * correlation;
    }
    
    // Apply spike rate normalization
    weightChange *= computeRateNormalization(preSpikeCount, postSpikeCount);
    
    // Apply weight change magnitude limits
    weightChange = std::clamp(weightChange, -pImpl->parameters.maxWeightChange, 
                             pImpl->parameters.maxWeightChange);
    
    return weightChange;
}

float EnhancedHebbian::computeRateNormalization(size_t preCount, size_t postCount) {
    float preRate = static_cast<float>(preCount) / 1000.0f;
    float postRate = static_cast<float>(postCount) / 1000.0f;
    float avgRate = (preRate + postRate) * 0.5f;
    return 1.0f / (1.0f + avgRate * 0.1f);
}

void EnhancedHebbian::applyConstrainedWeightChange(Synapse* synapse, float delta, TimestepDuration dt) {
    if (!synapse) return;
    
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply biological uptake/delivery limits
    newWeight = applyUptakeDeliveryConstraints(newWeight, currentWeight, delta, dt);
    
    // Apply weight bounds
    newWeight = std::clamp(newWeight, pImpl->parameters.minWeight, pImpl->parameters.maxWeight);
    
    // Store in history
    pImpl->weightHistory.push_back(newWeight);
    if (pImpl->weightHistory.size() > 100) {
        pImpl->weightHistory.erase(pImpl->weightHistory.begin());
    }
    
    synapse->setWeight(newWeight);
}

float EnhancedHebbian::applyUptakeDeliveryConstraints(float newWeight, float oldWeight, float delta, TimestepDuration dt) {
    float uptakeLimit = pImpl->parameters.uptakeLimit * dt * 1000.0f;
    if (delta > 0.0f && delta > uptakeLimit) {
        delta = uptakeLimit;
        newWeight = oldWeight + delta;
    }
    
    float deliveryLimit = pImpl->parameters.deliveryLimit * dt * 1000.0f;
    if (delta < 0.0f && std::abs(delta) > deliveryLimit) {
        delta = -deliveryLimit;
        newWeight = oldWeight + delta;
    }
    
    return newWeight;
}

void EnhancedHebbian::updateActivityStatistics(const std::vector<Timestamp>& preSpikes,
                                              const std::vector<Timestamp>& postSpikes,
                                              TimestepDuration dt) {
    float currentActivity = static_cast<float>(preSpikes.size() + postSpikes.size());
    
    pImpl->parameters.meanActivityEstimate = 
        pImpl->parameters.meanActivityEstimate * 0.99f + currentActivity * 0.01f;
    
    pImpl->baselineActivity = 
        pImpl->baselineActivity * 0.95f + currentActivity * 0.05f;
    
    pImpl->activityHistory.push_back(currentActivity);
    if (pImpl->activityHistory.size() > 100) {
        pImpl->activityHistory.erase(pImpl->activityHistory.begin());
    }
}

void EnhancedHebbian::applySynapticScaling(Synapse* synapse, TimestepDuration dt) {
    if (!synapse || pImpl->parameters.synapticScalingRate <= 0.0f) return;
    
    float weightVariance = getWeightVariance();
    float targetVariance = pImpl->parameters.targetWeightRange * 0.1f;
    
    if (weightVariance > 0.0f && targetVariance > 0.0f) {
        float scalingFactor = std::sqrt(targetVariance / weightVariance);
        
        if (std::abs(scalingFactor - 1.0f) > 0.01f) {
            float currentWeight = synapse->getWeight();
            float scaledWeight = currentWeight * scalingFactor;
            synapse->setWeight(scaledWeight);
        }
    }
}

float EnhancedHebbian::getWeightVariance() const {
    if (pImpl->weightHistory.empty()) return 0.0f;
    float mean = pImpl->parameters.meanActivityEstimate;
    float variance = 0.0f;
    for (float weight : pImpl->weightHistory) {
        variance += (weight - mean) * (weight - mean);
    }
    return variance / pImpl->weightHistory.size();
}

void EnhancedHebbian::applySynapticCompetition(float newWeight, Synapse* synapse) {
    if (pImpl->parameters.competitionStrength <= 0.0f || !synapse) {
        return;
    }
    
    float weightDiffFromMean = newWeight - pImpl->parameters.targetWeightRange * 0.5f;
    
    if (std::abs(weightDiffFromMean) > pImpl->parameters.competitionStrength) {
        newWeight = newWeight - 
                   weightDiffFromMean * pImpl->parameters.competitionStrength / 
                   std::abs(weightDiffFromMean);
    }
}

// OriginalHebbian implementation (simplified)
void OriginalHebbian::update(Synapse* synapse,
                             const std::vector<Timestamp>& preSpikes,
                             const std::vector<Timestamp>& postSpikes,
                             TimestepDuration dt) {
    if (!synapse || preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    size_t correlationCount = 0;
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float dt_val = static_cast<float>(postTime - preTime);
            if (std::abs(dt_val) < 100.0f) {
                ++correlationCount;
            }
        }
    }
    
    float delta = pImpl->learningRate * static_cast<float>(correlationCount);
    
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void OriginalHebbian::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) return;
    
    float newWeight = synapse->getWeight() + delta;
    newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
    synapse->setWeight(newWeight);
}

// Factory function implementation
std::unique_ptr<PlasticityRule> createHebbianRule(const std::string& type) {
    if (type == "original" || type == "default") {
        return std::make_unique<OriginalHebbian>();
    } else if (type == "enhanced" || type == "advanced") {
        return std::make_unique<EnhancedHebbian>();
    } else {
        return std::make_unique<OriginalHebbian>();
    }
}

} // namespace nlm
