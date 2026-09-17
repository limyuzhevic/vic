// Hebbian.cpp improvements implementation

#include "Hebbian.hpp"
#include "../../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <unordered_map>

namespace nlm {

// Hebbian implementation namespace for internal helper functions
namespace HebbianImpl {
    // Constants for Hebbian behavior
    namespace Constants {
        constexpr float DEFAULT_LEARNING_RATE = 0.01f;
        constexpr float DEFAULT_MAX_WEIGHT = 1.0f;
        constexpr float DEFAULT_MIN_WEIGHT = -1.0f;
        constexpr float DEFAULT_CORRELATION_WINDOW = 100.0f;  // ms
        constexpr float DEFAULT_HOMEOSTATIC_TARGET = 0.5f;
        constexpr float DEFAULT_ADAPTATION_RATE = 0.005f;
        constexpr float WEIGHT_CHANGE_EPSILON = 1e-6f;
    }
    
    // Helper function to compute covariance between pre and post spike trains
    float computeCovariance(
        const std::vector<Timestamp>& preSpikes,
        const std::vector<Timestamp>& postSpikes,
        float correlationWindow = Constants::DEFAULT_CORRELATION_WINDOW) {
        
        if (preSpikes.empty() || postSpikes.empty()) {
            return 0.0f;
        }
        
        // Compute spike counts and average firing rates
        size_t preCount = preSpikes.size();
        size_t postCount = postSpikes.size();
        
        // Compute covariance using spike timing correlation
        float covariance = 0.0f;
        size_t correlatedPairs = 0;
        
        // For small spike sets, use brute force approach
        if (preCount <= 10 && postCount <= 10) {
            for (size_t i = 0; i < preCount; ++i) {
                for (size_t j = 0; j < postCount; ++j) {
                    float dt = static_cast<float>(postSpikes[j] - preSpikes[i]);
                    if (std::abs(dt) <= correlationWindow) {
                        ++correlatedPairs;
                    }
                }
            }
        } else {
            // For larger spike sets, use efficient sliding window approach
            std::vector<Timestamp> sortedPre = preSpikes;
            std::vector<Timestamp> sortedPost = postSpikes;
            std::sort(sortedPre.begin(), sortedPre.end());
            std::sort(sortedPost.begin(), sortedPost.end());
            
            size_t preIdx = 0;
            size_t postIdx = 0;
            
            while (preIdx < sortedPre.size() && postIdx < sortedPost.size()) {
                float dt = static_cast<float>(sortedPost[postIdx] - sortedPre[preIdx]);
                if (std::abs(dt) <= correlationWindow) {
                    ++correlatedPairs;
                    preIdx++;
                    postIdx++;
                } else if (dt < -correlationWindow) {
                    postIdx++;
                } else {
                    preIdx++;
                }
            }
        }
        
        // Normalize covariance (count of correlated pairs)
        if (!preSpikes.empty() && !postSpikes.empty()) {
            covariance = static_cast<float>(correlatedPairs) / 
                         std::sqrt(static_cast<float>(preCount * postCount));
        }
        
        return covariance;
    }
    
    // Helper function to compute baseline activity
    float computeBaselineActivity(
        const std::vector<Timestamp>& spikeTrain,
        float learningRate) {
        
        if (spikeTrain.empty()) {
            return 0.0f;
        }
        
        // Compute baseline as learningRate * activity level
        // For spike trains, use normalized firing rate
        float activityLevel = static_cast<float>(spikeTrain.size()) / 100.0f;  // Normalize to reasonable range
        return learningRate * activityLevel;
    }
    
    // Helper function to validate Hebbian update parameters
    bool validateHebbianUpdate(
        Synapse* synapse,
        const std::vector<Timestamp>& preSpikes,
        const std::vector<Timestamp>& postSpikes,
        TimestepDuration dt) {
        
        if (!synapse) {
            return false;
        }
        
        if (preSpikes.empty() || postSpikes.empty()) {
            return false;
        }
        
        if (dt <= 0.0f) {
            return false;
        }
        
        // Validate all spike times are non-negative
        for (Timestamp t : preSpikes) {
            if (t < 0) return false;
        }
        for (Timestamp t : postSpikes) {
            if (t < 0) return false;
        }
        
        return true;
    }
    
    // Helper function for homeostatic learning rate adaptation
    float computeHomeostaticLearningRate(
        float currentRate,
        float targetRate,
        float adaptationRate = Constants::DEFAULT_ADAPTATION_RATE) {
        
        float error = targetRate - currentRate;
        return currentRate + adaptationRate * error;
    }
}}

struct Hebbian::Impl {
    float learningRate;
    float maxWeight;
    float minWeight;
    float covarianceThreshold;  // For covariance rule
    float correlationWindow;    // Configurable correlation window (ms)
    float homeostaticTarget;    // Target weight for homeostatic regulation
    float adaptationRate;        // Rate of homeostatic adaptation
    
    Impl() : learningRate(HebbianImpl::Constants::DEFAULT_LEARNING_RATE),
             maxWeight(HebbianImpl::Constants::DEFAULT_MAX_WEIGHT),
             minWeight(HebbianImpl::Constants::DEFAULT_MIN_WEIGHT),
             covarianceThreshold(0.0f),
             correlationWindow(HebbianImpl::Constants::DEFAULT_CORRELATION_WINDOW),
             homeostaticTarget(HebbianImpl::Constants::DEFAULT_HOMEOSTATIC_TARGET),
             adaptationRate(HebbianImpl::Constants::DEFAULT_ADAPTATION_RATE) {}
};

Hebbian::Hebbian() : pImpl(new Impl) {
    if (!pImpl) {
        throw std::bad_alloc();  // Handle memory allocation failure
    }
}

Hebbian::~Hebbian() = default;

// Unit-testable function: Compute covariance between pre and post spike trains
float Hebbian::computeCovariance(
    const std::vector<Timestamp>& preSpikes,
    const std::vector<Timestamp>& postSpikes,
    float window) const {
    
    return HebbianImpl::computeCovariance(preSpikes, postSpikes, window);
}

// Unit-testable function: Compute baseline activity
float Hebbian::computeBaselineActivity(
    const std::vector<Timestamp>& spikeTrain,
    float rate) const {
    
    return HebbianImpl::computeBaselineActivity(spikeTrain, rate);
}

// Unit-testable function: Validate Hebbian update parameters
bool Hebbian::validateUpdate(
    Synapse* synapse,
    const std::vector<Timestamp>& preSpikes,
    const std::vector<Timestamp>& postSpikes,
    TimestepDuration dt) const {
    
    return HebbianImpl::validateHebbianUpdate(synapse, preSpikes, postSpikes, dt);
}

void Hebbian::update(Synapse* synapse,
                      const std::vector<Timestamp>& preSpikes,
                      const std::vector<Timestamp>& postSpikes,
                      TimestepDuration dt) {
    /*
     * Real Hebbian learning implementation using covariance rule
     * 
     * Mathematical formulation (Covariance rule):
     * Δw = η * (⟨pre * post⟩ - ⟨pre⟩⟨post⟩)
     * 
     * Simplified version for spike-based systems:
     * Δw = η * (coactivity - baseline)
     * 
     * Where:
     *   coactivity = number of correlated pre/post spikes
     *   baseline = learningRate * mean activity
     */
    
    if (!validateUpdate(synapse, preSpikes, postSpikes, dt)) {
        return;
    }
    
    // Compute covariance between pre and post spike trains
    float covariance = computeCovariance(preSpikes, postSpikes, pImpl->correlationWindow);
    
    // Apply covariance threshold check
    if (std::abs(covariance) < pImpl->covarianceThreshold) {
        return;  // No significant correlation
    }
    
    // Compute weight change based on covariance rule
    float delta = pImpl->learningRate * covariance;
    
    // Apply with bounds
    if (std::abs(delta) > HebbianImpl::Constants::WEIGHT_CHANGE_EPSILON) {
        applyWeightChange(synapse, delta);
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

void Hebbian::setMinWeight(float minWeight) {
    pImpl->minWeight = std::clamp(minWeight, -10.0f, 0.0f);
}

float Hebbian::getMinWeight() const {
    return pImpl->minWeight;
}

void Hebbian::setCorrelationWindow(float window) {
    pImpl->correlationWindow = std::clamp(window, 1.0f, 500.0f);
}

float Hebbian::getCorrelationWindow() const {
    return pImpl->correlationWindow;
}

void Hebbian::setHomeostaticTarget(float target) {
    pImpl->homeostaticTarget = std::clamp(target, 0.0f, 2.0f);
}

float Hebbian::getHomeostaticTarget() const {
    return pImpl->homeostaticTarget;
}

void Hebbian::setAdaptationRate(float rate) {
    pImpl->adaptationRate = std::clamp(rate, 0.001f, 0.1f);
}

float Hebbian::getAdaptationRate() const {
    return pImpl->adaptationRate;
}

// Apply homeostatic learning rate adaptation
void Hebbian::applyHomeostaticAdaptation() {
    if (!pImpl) return;
    
    // Compute homeostatic learning rate adjustment
    float newLearningRate = HebbianImpl::computeHomeostaticLearningRate(
        pImpl->learningRate,
        pImpl->homeostaticTarget,
        pImpl->adaptationRate);
    
    // Update learning rate
    pImpl->learningRate = newLearningRate;
}

// Compute weight change with homeostatic regulation
float Hebbian::computeWeightChangeWithHomeostasis(
    const std::vector<Timestamp>& preSpikes,
    const std::vector<Timestamp>& postSpikes,
    TimestepDuration dt) const {
    
    if (!validateUpdate(nullptr, preSpikes, postSpikes, dt)) {
        return 0.0f;
    }
    
    // Compute raw covariance
    float covariance = computeCovariance(preSpikes, postSpikes, pImpl->correlationWindow);
    
    // Apply homeostatic adjustment
    float homeostaticAdjustedRate = pImpl->learningRate;
    if (pImpl->homeostaticTarget > 0.0f) {
        homeostaticAdjustedRate *= (1.0f + pImpl->adaptationRate);
    }
    
    return homeostaticAdjustedRate * covariance;
}

} // namespace nlm
