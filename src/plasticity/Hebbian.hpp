#pragma once

#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

// Hebbian learning parameters based on biological models
struct HebbianParameters {
    // Standard Hebbian parameters
    float learningRatePositive;     // Potentiation learning rate (η+)
    float learningRateNegative;     // Depression learning rate (η-)
    float correlationWindow;        // Time window for spike correlation (ms)
    
    // Advanced parameters
    float weightNormalizationFactor; // Weight normalization strength
    float synapticScalingRate;       // Rate of synaptic scaling (0-1)
    float competitionStrength;       // Competition for synaptic resources
    float maxWeightChange;           // Maximum weight change per timestep
    float baselineHebbian;          // Baseline Hebbian contribution
    
    // Biological constraints
    float uptakeLimit;               // Maximum weight increase per unit time
    float deliveryLimit;             // Maximum weight decrease per unit time
    float targetWeightRange;         // Target range for weight normalization
    
    // Statistical parameters
    float meanActivityEstimate;      // Running estimate of mean activity
    float varianceEstimate;         // Running estimate of variance
    float normalizationInterval;    // Time interval for parameter updates
    
    HebbianParameters() 
        : learningRatePositive(0.01f), learningRateNegative(0.008f), correlationWindow(50.0f),
          weightNormalizationFactor(0.1f), synapticScalingRate(0.001f), competitionStrength(0.05f),
          maxWeightChange(0.1f), baselineHebbian(0.001f), uptakeLimit(0.05f), deliveryLimit(0.05f),
          targetWeightRange(0.5f), meanActivityEstimate(0.0f), varianceEstimate(0.0f),
          normalizationInterval(1000.0f) {}
};

// Enhanced Hebbian learning implementation
class EnhancedHebbian : public PlasticityRule {
public:
    EnhancedHebbian() : pImpl(new Impl()) {
        pImpl->parameters = HebbianParameters();
        pImpl->coactivityCount = 0;
        pImpl->baselineActivity = 0.0f;
        pImpl->weightHistory.resize(100, 0.0f);  // Store recent weight history
        pImpl->activityHistory.resize(100, 0.0f); // Store recent activity
    }
    
    ~EnhancedHebbian() override {
        delete pImpl;
    }
    
    // Disable copying
    EnhancedHebbian(const EnhancedHebbian&) = delete;
    EnhancedHebbian& operator=(const EnhancedHebbian&) = delete;
    
    // Enable moving
    EnhancedHebbian(EnhancedHebbian&& other) noexcept : pImpl(other.pImpl) {
        other.pImpl = nullptr;
    }
    
    EnhancedHebbian& operator=(EnhancedHebbian&& other) noexcept {
        if (this != &other) {
            delete pImpl;
            pImpl = other.pImpl;
            other.pImpl = nullptr;
        }
        return *this;
    }
    
    // Enhanced Hebbian update with covariance-based learning
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        pImpl->coactivityCount = 0;
        
        // Compute true covariance-based Hebbian learning
        float weightChange = computeCovarianceHebbian(synapse, preSpikes, postSpikes, dt);
        
        // Apply baseline Hebbian contribution
        weightChange += pImpl->parameters.baselineHebbian;
        
        // Apply weight change with constraints
        applyConstrainedWeightChange(synapse, weightChange, dt);
        
        // Update activity statistics
        updateActivityStatistics(preSpikes, postSpikes, dt);
        
        // Apply synaptic scaling
        applySynapticScaling(synapse, dt);
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        
        float currentWeight = synapse->getWeight();
        float newWeight = currentWeight + delta;
        
        // Apply biological constraints
        newWeight = applyBiologicalConstraints(newWeight, currentWeight, delta);
        
        // Apply competition if needed
        newWeight = applySynapticCompetition(newWeight, synapse);
        
        synapse->setWeight(newWeight);
    }
    
    const char* getName() const override {
        return "EnhancedHebbian";
    }
    
    // Parameter setters and getters
    void setLearningRatePositive(float rate) {
        pImpl->parameters.learningRatePositive = std::clamp(rate, 0.0f, 1.0f);
    }
    
    float getLearningRatePositive() const {
        return pImpl->parameters.learningRatePositive;
    }
    
    void setLearningRateNegative(float rate) {
        pImpl->parameters.learningRateNegative = std::clamp(rate, 0.0f, 1.0f);
    }
    
    float getLearningRateNegative() const {
        return pImpl->parameters.learningRateNegative;
    }
    
    void setLearningRate(float rate) {
        setLearningRatePositive(rate);
        setLearningRateNegative(rate * 0.8f); // Depression typically weaker
    }
    
    float getLearningRate() const {
        return getLearningRatePositive();
    }
    
    void setCorrelationWindow(float window) {
        pImpl->parameters.correlationWindow = std::max(1.0f, window);
    }
    
    float getCorrelationWindow() const {
        return pImpl->parameters.correlationWindow;
    }
    
    void setMaxWeight(float maxWeight) {
        pImpl->parameters.maxWeight = std::max(0.0f, maxWeight);
    }
    
    float getMaxWeight() const {
        return pImpl->parameters.maxWeight;
    }
    
    void setMinWeight(float minWeight) {
        pImpl->parameters.minWeight = std::min(0.0f, minWeight);
    }
    
    float getMinWeight() const {
        return pImpl->parameters.minWeight;
    }
    
    void setWeightNormalizationFactor(float factor) {
        pImpl->parameters.weightNormalizationFactor = std::clamp(factor, 0.0f, 1.0f);
    }
    
    float getWeightNormalizationFactor() const {
        return pImpl->parameters.weightNormalizationFactor;
    }
    
    void setSynapticScalingRate(float rate) {
        pImpl->parameters.synapticScalingRate = std::clamp(rate, 0.0f, 0.1f);
    }
    
    float getSynapticScalingRate() const {
        return pImpl->parameters.synapticScalingRate;
    }
    
    void setCompetitionStrength(float strength) {
        pImpl->parameters.competitionStrength = std::clamp(strength, 0.0f, 1.0f);
    }
    
    float getCompetitionStrength() const {
        return pImpl->parameters.competitionStrength;
    }
    
    void setParameters(const HebbianParameters& params) {
        pImpl->parameters = params;
    }
    
    const HebbianParameters& getParameters() const {
        return pImpl->parameters;
    }
    
    // Statistics and analysis
    float getCoactivityCount() const {
        return pImpl->coactivityCount;
    }
    
    float getBaselineActivity() const {
        return pImpl->baselineActivity;
    }
    
    float getMeanActivityEstimate() const {
        return pImpl->parameters.meanActivityEstimate;
    }
    
    float getWeightVariance() const {
        if (pImpl->weightHistory.empty()) return 0.0f;
        float mean = pImpl->parameters.meanActivityEstimate;  // Simplified
        float variance = 0.0f;
        for (float weight : pImpl->weightHistory) {
            variance += (weight - mean) * (weight - mean);
        }
        return variance / pImpl->weightHistory.size();
    }
    
    // Reset learning state
    void reset() {
        pImpl->coactivityCount = 0;
        pImpl->baselineActivity = 0.0f;
        pImpl->weightHistory.clear();
        pImpl->activityHistory.clear();
    }
    
private:
    struct Impl {
        HebbianParameters parameters;
        size_t coactivityCount;
        float baselineActivity;
        std::vector<float> weightHistory;
        std::vector<float> activityHistory;
        
        Impl() : coactivityCount(0), baselineActivity(0.0f) {}
    };
    
    Impl* pImpl;
    
    // Core Hebbian computation functions
    float computeCovarianceHebbian(Synapse* synapse,
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
            // Simple correlation: coactivity normalized by expected value
            float expectedCoactivity = static_cast<float>(preSpikeCount * postSpikeCount) / 
                                     (pImpl->parameters.correlationWindow * 10.0f);
            if (expectedCoactivity > 0.0f) {
                correlation = static_cast<float>(coactivity) / expectedCoactivity;
            }
        }
        
        // Apply Hebbian rule: Δw = η+ * correlation for positive, η- * correlation for negative
        float weightChange = 0.0f;
        if (correlation > 0.0f) {
            // Potentiation
            weightChange = pImpl->parameters.learningRatePositive * correlation;
        } else if (correlation < 0.0f) {
            // Depression
            weightChange = pImpl->parameters.learningRateNegative * correlation;
        }
        
        // Apply spike rate normalization
        weightChange *= computeRateNormalization(preSpikeCount, postSpikeCount);
        
        // Apply weight change magnitude limits
        weightChange = std::clamp(weightChange, -pImpl->parameters.maxWeightChange, 
                                 pImpl->parameters.maxWeightChange);
        
        return weightChange;
    }
    
    float computeRateNormalization(size_t preCount, size_t postCount) {
        // Normalize Hebbian learning by overall firing rates
        float preRate = static_cast<float>(preCount) / 1000.0f;  // Normalized rate
        float postRate = static_cast<float>(postCount) / 1000.0f;
        
        // Use average firing rate for normalization
        float avgRate = (preRate + postCount) * 0.5f;
        
        // Normalize to prevent runaway potentiation at high firing rates
        float normalization = 1.0f / (1.0f + avgRate * 0.1f);
        
        return normalization;
    }
    
    void applyConstrainedWeightChange(Synapse* synapse, float delta, TimestepDuration dt) {
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
    
    float applyBiologicalConstraints(float newWeight, float oldWeight, float delta, TimestepDuration dt) {
        // Apply uptake limit (max increase per timestep)
        float uptakeLimit = pImpl->parameters.uptakeLimit * dt * 1000.0f;
        if (delta > 0.0f && delta > uptakeLimit) {
            delta = uptakeLimit;
            newWeight = oldWeight + delta;
        }
        
        // Apply delivery limit (max decrease per timestep)
        float deliveryLimit = pImpl->parameters.deliveryLimit * dt * 1000.0f;
        if (delta < 0.0f && std::abs(delta) > deliveryLimit) {
            delta = -deliveryLimit;
            newWeight = oldWeight + delta;
        }
        
        return newWeight;
    }
    
    float applySynapticCompetition(float newWeight, Synapse* synapse) {
        if (pImpl->parameters.competitionStrength <= 0.0f || !synapse) {
            return newWeight;
        }
        
        // Simple competition: keep weights within reasonable range
        // In a real implementation, this would consider all synapses from same source
        float weightDiffFromMean = newWeight - pImpl->parameters.targetWeightRange * 0.5f;
        
        if (std::abs(weightDiffFromMean) > pImpl->parameters.competitionStrength) {
            // Scale back extreme weights
            newWeight = newWeight - 
                       weightDiffFromMean * pImpl->parameters.competitionStrength / 
                       std::abs(weightDiffFromMean);
        }
        
        return newWeight;
    }
    
    void updateActivityStatistics(const std::vector<Timestamp>& preSpikes,
                                  const std::vector<Timestamp>& postSpikes,
                                  TimestepDuration dt) {
        // Update running statistics
        float currentActivity = static_cast<float>(preSpikes.size() + postSpikes.size());
        
        // Update mean activity estimate
        pImpl->parameters.meanActivityEstimate = 
            pImpl->parameters.meanActivityEstimate * 0.99f + currentActivity * 0.01f;
        
        // Update baseline activity (low-pass filtered)
        pImpl->baselineActivity = 
            pImpl->baselineActivity * 0.95f + currentActivity * 0.05f;
        
        // Store activity in history
        pImpl->activityHistory.push_back(currentActivity);
        if (pImpl->activityHistory.size() > 100) {
            pImpl->activityHistory.erase(pImpl->activityHistory.begin());
        }
    }
    
    void applySynapticScaling(Synapse* synapse, TimestepDuration dt) {
        if (!synapse || pImpl->parameters.synapticScalingRate <= 0.0f) return;
        
        // Compute scaling factor based on weight variance
        float weightVariance = getWeightVariance();
        float targetVariance = pImpl->parameters.targetWeightRange * 0.1f;
        
        if (weightVariance > 0.0f && targetVariance > 0.0f) {
            float scalingFactor = std::sqrt(targetVariance / weightVariance);
            
            // Apply scaling if factor deviates from 1.0
            if (std::abs(scalingFactor - 1.0f) > 0.01f) {
                float currentWeight = synapse->getWeight();
                float scaledWeight = currentWeight * scalingFactor;
                
                // Apply scaling
                synapse->setWeight(scaledWeight);
            }
        }
    }
}; 

// Original Hebbian class (maintains backward compatibility)
class OriginalHebbian : public PlasticityRule {
public:
    OriginalHebbian() : pImpl(new Impl()) {}
    
    ~OriginalHebbian() override {
        delete pImpl;
    }
    
    OriginalHebbian(const OriginalHebbian&) = delete;
    OriginalHebbian& operator=(const OriginalHebbian&) = delete;
    
    OriginalHebbian(OriginalHebbian&& other) noexcept : pImpl(other.pImpl) {
        other.pImpl = nullptr;
    }
    
    OriginalHebbian& operator=(OriginalHebbian&& other) noexcept {
        if (this != &other) {
            delete pImpl;
            pImpl = other.pImpl;
            other.pImpl = nullptr;
        }
        return *this;
    }
    
    void update(Synapse* synapse,
                const std::vector<Timestamp>& preSpikes,
                const std::vector<Timestamp>& postSpikes,
                TimestepDuration dt) override {
        // Original simplified Hebbian implementation
        if (!synapse || preSpikes.empty() || postSpikes.empty()) {
            return;
        }
        
        // Count correlated spike pairs (simplified covariance)
        size_t correlationCount = 0;
        for (Timestamp preTime : preSpikes) {
            for (Timestamp postTime : postSpikes) {
                float dt_val = static_cast<float>(postTime - preTime);
                // Count spikes within a broad time window as correlated
                if (std::abs(dt_val) < 100.0f) {  // 100ms correlation window
                    ++correlationCount;
                }
            }
        }
        
        // Compute weight change based on correlation
        float delta = pImpl->learningRate * static_cast<float>(correlationCount);
        
        // Apply with bounds
        if (std::abs(delta) > 1e-6f) {
            applyWeightChange(synapse, delta);
        }
    }
    
    void applyWeightChange(Synapse* synapse, SynapticWeight delta) override {
        if (!synapse) return;
        
        float newWeight = synapse->getWeight() + delta;
        newWeight = std::clamp(newWeight, pImpl->minWeight, pImpl->maxWeight);
        synapse->setWeight(newWeight);
    }
    
    const char* getName() const override {
        return "Hebbian";
    }
    
    void setLearningRate(float rate) {
        pImpl->learningRate = std::clamp(rate, 0.0f, 1.0f);
    }
    
    float getLearningRate() const {
        return pImpl->learningRate;
    }
    
    void setMaxWeight(float maxWeight) {
        pImpl->maxWeight = std::clamp(maxWeight, 0.0f, 10.0f);
    }
    
    float getMaxWeight() const {
        return pImpl->maxWeight;
    }
    
    void setMinWeight(float minWeight) {
        pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
    }
    
    float getMinWeight() const {
        return pImpl->minWeight;
    }
    
    void reset() {
        pImpl->learningRate = 0.01f;
        pImpl->maxWeight = 1.0f;
        pImpl->minWeight = -1.0f;
        pImpl->covarianceThreshold = 0.0f;
    }
    
private:
    struct Impl {
        float learningRate;
        float maxWeight;
        float minWeight;
        float covarianceThreshold;
        
        Impl() : learningRate(0.01f), maxWeight(1.0f), minWeight(-1.0f), covarianceThreshold(0.0f) {}
    };
    
    Impl* pImpl;
};

// Factory function to create appropriate Hebbian variant
std::unique_ptr<PlasticityRule> createHebbianRule(const std::string& type) {
    if (type == "original" || type == "default") {
        return std::make_unique<OriginalHebbian>();
    } else if (type == "enhanced" || type == "advanced") {
        return std::make_unique<EnhancedHebbian>();
    } else {
        // Default to original for backward compatibility
        return std::make_unique<OriginalHebbian>();
    }
}

} // namespace nlm
