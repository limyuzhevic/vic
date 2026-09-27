#include "EnhancedHebbian.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

EnhancedHebbianRule::EnhancedHebbianRule() {
    // Initialize with biologically plausible parameters
    setLearningRate(0.02f);
    setMaxWeight(1.0f);
    setMinWeight(-1.0f);
    covarianceThreshold_ = 0.0f;
}

void EnhancedHebbianRule::update(Synapse* synapse,
                                 const std::vector<Timestamp>& preSpikes,
                                 const std::vector<Timestamp>& postSpikes,
                                 TimestepDuration dt) {
    if (!synapse) {
        NLM_LOG_WARNING("EnhancedHebbianRule::update called with null synapse");
        return;
    }
    
    if (preSpikes.empty() || postSpikes.empty()) {
        return;
    }
    
    // Real Hebbian learning with multiple mechanisms
    float delta = computeHebbianLearning(synapse, preSpikes, postSpikes, dt);
    
    // Apply with normalization and bounds checking
    if (std::abs(delta) > 1e-6f) {
        applyWeightChange(synapse, delta);
    }
}

void EnhancedHebbianRule::applyWeightChange(Synapse* synapse, SynapticWeight delta) {
    if (!synapse) {
        NLM_LOG_ERROR("EnhancedHebbianRule::applyWeightChange called with null synapse");
        return;
    }
    
    // Enhanced weight change with multiple mechanisms
    float currentWeight = synapse->getWeight();
    float newWeight = currentWeight + delta;
    
    // Apply Hebbian normalization (keep weights in reasonable range)
    newWeight = normalizeWeight(newWeight, currentWeight);
    
    // Apply bounds with hysteresis to prevent oscillations
    newWeight = applyHysteresisBounds(newWeight, currentWeight);
    
    // Store old weight for learning rate adaptation
    float oldWeight = currentWeight;
    
    // Apply weight change
    synapse->setWeight(newWeight);
    
    // Log learning if significant
    if (std::abs(newWeight - oldWeight) > 0.01f) {
        NLM_LOG_INFO("Hebbian learning: synapse weight changed from " + 
                    std::to_string(oldWeight) + " to " + std::to_string(newWeight) +
                    " (delta=" + std::to_string(newWeight - oldWeight) + ")");
    }
}

const char* EnhancedHebbianRule::getName() const {
    return "EnhancedHebbian";
}

void EnhancedHebbianRule::setLearningRate(float rate) {
    // Learning rate with biological constraints
    learningRate_ = std::clamp(rate, 0.001f, 0.1f);
    
    // Adjust related parameters based on learning rate
    if (learningRate_ > 0.05f) {
        // High learning rate: reduce max weight to prevent runaway excitation
        maxWeight_ = std::clamp(maxWeight_, 0.5f, 1.0f);
        minWeight_ = std::clamp(minWeight_, -0.5f, -0.1f);
    } else {
        // Low learning rate: allow more weight range for gradual learning
        maxWeight_ = std::clamp(maxWeight_, 0.8f, 1.5f);
        minWeight_ = std::clamp(minWeight_, -1.5f, -0.5f);
    }
}

float EnhancedHebbianRule::getLearningRate() const {
    return learningRate_;
}

void EnhancedHebbianRule::setMaxWeight(float maxWeight) {
    // Max weight with activity-dependent modulation
    maxWeight_ = std::clamp(maxWeight, 0.0f, 2.0f);
    
    // Update min weight proportionally for stability
    if (maxWeight_ > 0.5f) {
        minWeight_ = std::max(minWeight_, -maxWeight_ * 0.8f);
    }
}

float EnhancedHebbianRule::getMaxWeight() const {
    return maxWeight_;
}

void EnhancedHebbianRule::setMinWeight(float minWeight) {
    // Min weight with biological constraints
    minWeight_ = std::clamp(minWeight, -2.0f, 0.0f);
    
    // Update max weight proportionally for stability
    if (std::abs(minWeight_) > 0.5f) {
        maxWeight_ = std::min(maxWeight_, -minWeight_ * 0.8f);
    }
}

float EnhancedHebbianRule::computeHebbianLearning(Synapse* synapse,
                                                  const std::vector<Timestamp>& preSpikes,
                                                  const std::vector<Timestamp>& postSpikes,
                                                  TimestepDuration dt) {
    // Multiple learning mechanisms
    float stdpContribution = computeSTDPContribution(preSpikes, postSpikes, dt);
    float hebbianContribution = computeCovarianceContribution(preSpikes, postSpikes, dt);
    float normalizationContribution = computeNormalizationTerm(synapse, preSpikes, postSpikes, dt);
    
    // Combine contributions with adaptive weights
    float totalDelta = stdpContribution * 0.4f + 
                      hebbianContribution * 0.4f + 
                      normalizationContribution * 0.2f;
    
    // Activity-dependent modulation
    float activityModulation = computeActivityModulation(preSpikes, postSpikes);
    totalDelta *= (1.0f + activityModulation * 0.2f);
    
    return totalDelta;
}

float EnhancedHebbianRule::computeSTDPContribution(const std::vector<Timestamp>& preSpikes,
                                                  const std::vector<Timestamp>& postSpikes,
                                                  TimestepDuration dt) {
    if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
    
    // Optimized STDP calculation with efficient spike pairing
    std::vector<Timestamp> sortedPre = preSpikes;
    std::vector<Timestamp> sortedPost = postSpikes;
    std::sort(sortedPre.begin(), sortedPre.end());
    std::sort(sortedPost.begin(), sortedPost.end());
    
    float totalDelta = 0.0f;
    const float stdpTimeConstant = 20.0f;
    
    // Efficient O(n log n) spike pairing using two-pointer technique
    size_t preIdx = 0, postIdx = 0;
    while (preIdx < sortedPre.size() && postIdx < sortedPost.size()) {
        float deltaT = static_cast<float>(sortedPost[postIdx] - sortedPre[preIdx]);
        
        if (deltaT > 0) {
            // Pre before post: Long-term potentiation
            float ltp = 0.01f * std::exp(-deltaT / stdpTimeConstant);
            totalDelta += ltp;
            ++postIdx;
        } else {
            // Post before pre: Long-term depression
            float ltd = -0.012f * std::exp(deltaT / stdpTimeConstant);
            totalDelta += ltd;
            ++preIdx;
        }
    }
    
    return totalDelta;
}

float EnhancedHebbianRule::computeCovarianceContribution(const std::vector<Timestamp>& preSpikes,
                                                         const std::vector<Timestamp>& postSpikes,
                                                         TimestepDuration dt) {
    // Covariance-based Hebbian learning
    if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
    
    // Compute correlation efficiently
    std::vector<Timestamp> sortedPre = preSpikes;
    std::vector<Timestamp> sortedPost = postSpikes;
    std::sort(sortedPre.begin(), sortedPre.end());
    std::sort(sortedPost.begin(), sortedPost.end());
    
    size_t correlationCount = 0;
    const float window = 50.0f;  // Shorter window for covariance
    
    for (size_t i = 0; i < sortedPre.size(); ++i) {
        auto lower = std::lower_bound(sortedPost.begin(), sortedPost.end(), 
                                     sortedPre[i] - window);
        auto upper = std::upper_bound(sortedPost.begin(), sortedPost.end(), 
                                     sortedPre[i] + window);
        correlationCount += std::distance(lower, upper);
    }
    
    // Normalized covariance contribution
    float normalizedCorrelation = static_cast<float>(correlationCount) / 
                                 std::max(1.0f, static_cast<float>(preSpikes.size() * postSpikes.size()));
    
    return learningRate_ * normalizedCorrelation * 0.1f;  // Scale for stability
}

float EnhancedHebbianRule::computeNormalizationTerm(Synapse* synapse,
                                                  const std::vector<Timestamp>& preSpikes,
                                                  const std::vector<Timestamp>& postSpikes,
                                                  TimestepDuration dt) {
    if (!synapse) return 0.0f;
    
    float currentWeight = synapse->getWeight();
    
    // Target weight based on activity history
    float targetWeight = computeTargetWeight(preSpikes, postSpikes);
    
    // Normalization towards target (stabilization mechanism)
    float weightError = targetWeight - currentWeight;
    float normalizationFactor = 0.005f;  // Slower normalization
    
    return weightError * normalizationFactor;
}

float EnhancedHebbianRule::computeActivityModulation(const std::vector<Timestamp>& preSpikes,
                                                     const std::vector<Timestamp>& postSpikes) {
    // Activity-dependent modulation of learning rate
    size_t totalSpikes = preSpikes.size() + postSpikes.size();
    
    // Modulate learning based on recent activity
    float activityLevel = std::min(static_cast<float>(totalSpikes) * 0.001f, 1.0f);
    
    // Reduce learning rate at high activity levels to prevent runaway excitation
    if (activityLevel > 0.5f) {
        return (0.5f - activityLevel) * 2.0f;  // Negative modulation
    }
    
    // Enhance learning at low activity levels
    return activityLevel * 0.5f;
}

float EnhancedHebbianRule::computeTargetWeight(const std::vector<Timestamp>& preSpikes,
                                              const std::vector<Timestamp>& postSpikes) {
    // Target weight based on spike timing statistics
    if (preSpikes.empty() || postSpikes.empty()) {
        return 0.0f;  // Default for no activity
    }
    
    // Simple target: 0.0 (balanced excitation/inhibition)
    return 0.0f;
}

float EnhancedHebbianRule::normalizeWeight(float newWeight, float currentWeight) {
    // Enhanced normalization with activity-dependent scaling
    float activityLevel = std::min(std::abs(newWeight - currentWeight) * 10.0f, 1.0f);
    
    // Stronger normalization at high activity levels
    float normalizationStrength = 0.1f + activityLevel * 0.2f;
    
    // Pull towards zero more strongly at high activity
    float normalized = newWeight * (1.0f - normalizationStrength) + 0.0f * normalizationStrength;
    
    return normalized;
}

float EnhancedHebbianRule::applyHysteresisBounds(float newWeight, float currentWeight) {
    // Hysteresis bounds to prevent oscillations near threshold
    float weightChange = newWeight - currentWeight;
    
    // Different bounds for strengthening vs weakening
    if (weightChange > 0) {
        // For strengthening: higher upper bound when already strong
        float hysteresisUpper = maxWeight_ * (1.0f + 0.1f * std::min(currentWeight / maxWeight_, 1.0f));
        newWeight = std::min(newWeight, hysteresisUpper);
    } else {
        // For weakening: lower lower bound when already weak
        float hysteresisLower = minWeight_ * (1.0f - 0.1f * std::min(std::abs(currentWeight / minWeight_), 1.0f));
        newWeight = std::max(newWeight, hysteresisLower);
    }
    
    return newWeight;
}

} // namespace nlm