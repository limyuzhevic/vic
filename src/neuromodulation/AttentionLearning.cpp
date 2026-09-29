#include "AttentionLearning.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <queue>
#include <unordered_map>

namespace nlm {

// Predictive Coding Implementation
class PredictiveCoding {
public:
    struct PredictiveParameters {
        float predictionErrorWeight;   // Weight of prediction errors
        float predictionGain;          // Gain in prediction updates
        float predictionHorizon;       // Time horizon for predictions
        float predictionPrecision;     // Precision of predictions
    };
    
    PredictiveCoding() {
        params.predictionErrorWeight = 0.3f;
        params.predictionGain = 0.1f;
        params.predictionHorizon = 10.0f;
        params.predictionPrecision = 0.5f;
    }
    
    std::vector<float> predict(const std::vector<float>& currentState,
                               const std::vector<float>& previousPrediction) {
        // Predict next state based on current state
        std::vector<float> prediction = currentState;
        
        // Simple predictive coding: small changes based on state dynamics
        for (size_t i = 0; i < prediction.size(); ++i) {
            // Apply prediction dynamics
            float change = (currentState[i] - previousPrediction[i]) * 
                          params.predictionGain;
            prediction[i] += change;
        
            // Constrain to reasonable bounds
            prediction[i] = std::clamp(prediction[i], 0.0f, 1.0f);
        }
        
        return prediction;
    }
    
    float computePredictionError(const std::vector<float>& actual,
                                const std::vector<float>& predicted) const {
        // Compute prediction error between actual and predicted states
        if (actual.size() != predicted.size()) return 0.0f;
        
        float error = 0.0f;
        for (size_t i = 0; i < actual.size(); ++i) {
            float diff = actual[i] - predicted[i];
            error += diff * diff;
        }
        
        return std::sqrt(error);
    }
    
    void setParameters(const PredictiveParameters& params) {
        this->params = params;
    }
    
    void reset() {
        // Nothing to reset for predictive coding
    }
    
private:
    PredictiveParameters params;
};

// Selective Attention Implementation
class SelectiveAttention {
public:
    struct AttentionParameters {
        float topDownWeight;           // Weight of top-down attention
        float bottomUpWeight;          // Weight of bottom-up attention
        float featureSensitivity;      // Sensitivity to features
        float saliencyThreshold;       // Threshold for feature saliency
    };
    
    SelectiveAttention() {
        params.topDownWeight = 0.4f;
        params.bottomUpWeight = 0.3f;
        params.featureSensitivity = 0.5f;
        params.saliencyThreshold = 0.3f;
    }
    
    std::vector<float> allocateAttention(const std::vector<float>& features,
                                        const std::vector<float>& goals,
                                        const std::vector<float>& context) {
        // Allocate attention based on features, goals, and context
        std::vector<float> attentionWeights(features.size(), 0.0f);
        
        // Compute feature saliency
        std::vector<float> saliency = computeFeatureSaliency(features, goals, context);
        
        // Apply top-down attention
        for (size_t i = 0; i < features.size(); ++i) {
            float topDown = computeTopDownAttention(features[i], goals);
            attentionWeights[i] += topDown * params.topDownWeight;
        }
        
        // Apply bottom-up attention
        for (size_t i = 0; i < features.size(); ++i) {
            float bottomUp = saliency[i] * params.bottomUpWeight;
            attentionWeights[i] += bottomUp;
        }
        
        // Normalize attention weights
        float totalWeight = std::accumulate(attentionWeights.begin(), attentionWeights.end(), 0.0f);
        if (totalWeight > 0.0f) {
            for (float& weight : attentionWeights) {
                weight /= totalWeight;
            }
        }
        
        return attentionWeights;
    }
    
    std::vector<float> computeFeatureSaliency(const std::vector<float>& features,
                                              const std::vector<float>& goals,
                                              const std::vector<float>& context) const {
        // Compute saliency of features based on goals and context
        std::vector<float> saliency(features.size(), 0.0f);
        
        for (size_t i = 0; i < features.size(); ++i) {
            // Saliency from similarity to goals
            float goalSimilarity = 0.0f;
            if (!goals.empty()) {
                float diff = std::abs(features[i] - goals[i]);
                goalSimilarity = 1.0f - diff;
            }
            
            // Saliency from context match
            float contextMatch = 0.0f;
            if (!context.empty()) {
                float diff = std::abs(features[i] - context[i]);
                contextMatch = 1.0f - diff;
            }
            
            // Combine with sensitivity
            saliency[i] = (goalSimilarity + contextMatch) * params.featureSensitivity;
            saliency[i] = std::clamp(saliency[i], 0.0f, 1.0f);
        }
        
        return saliency;
    }
    
    float computeTopDownAttention(float featureValue,
                                 const std::vector<float>& goals) const {
        // Compute top-down attention based on goals
        if (goals.empty()) return 0.0f;
        
        // Find the goal index that matches this feature
        float goalValue = goals[0]; // Simplified: use first goal
        float similarity = 1.0f - std::abs(featureValue - goalValue);
        
        return similarity > params.saliencyThreshold ? similarity : 0.0f;
    }
    
    void setParameters(const AttentionParameters& params) {
        this->params = params;
    }
    
    void reset() {
        // Nothing to reset for selective attention
    }
    
private:
    AttentionParameters params;
};

// Feature-Based Attention Implementation
class FeatureAttention {
public:
    struct FeatureParameters {
        float spatialWeight;           // Weight of spatial attention
        float temporalWeight;          // Weight of temporal attention
        float semanticWeight;          // Weight of semantic attention
        float relevanceThreshold;      // Threshold for feature relevance
    };
    
    FeatureAttention() {
        params.spatialWeight = 0.4f;
        params.temporalWeight = 0.3f;
        params.semanticWeight = 0.3f;
        params.relevanceThreshold = 0.2f;
    }
    
    std::vector<float> allocateFeatureAttention(const std::vector<std::vector<float>>& spatialFeatures,
                                               const std::vector<std::vector<float>>& temporalFeatures,
                                               const std::vector<float>& semanticFeatures) {
        // Allocate attention to features based on spatial, temporal, and semantic information
        std::vector<float> featureAttention;
        
        // Spatial attention
        if (!spatialFeatures.empty()) {
            std::vector<float> spatialAttention = computeSpatialAttention(spatialFeatures);
            featureAttention.insert(featureAttention.end(), spatialAttention.begin(), spatialAttention.end());
        }
        
        // Temporal attention
        if (!temporalFeatures.empty()) {
            std::vector<float> temporalAttention = computeTemporalAttention(temporalFeatures);
            featureAttention.insert(featureAttention.end(), temporalAttention.begin(), temporalAttention.end());
        }
        
        // Semantic attention
        if (!semanticFeatures.empty()) {
            std::vector<float> semanticAttention = computeSemanticAttention(semanticFeatures);
            featureAttention.insert(featureAttention.end(), semanticAttention.begin(), semanticAttention.end());
        }
        
        return featureAttention;
    }
    
    std::vector<float> computeSpatialAttention(const std::vector<std::vector<float>>& spatialFeatures) {
        // Compute spatial attention from spatial features
        std::vector<float> spatialAttention(spatialFeatures.size(), 0.0f);
        
        for (size_t i = 0; i < spatialFeatures.size(); ++i) {
            const auto& feature = spatialFeatures[i];
            
            // Compute spatial salience based on feature values
            float salience = 0.0f;
            for (float value : feature) {
                salience += value;
            }
            salience /= feature.size();
            
            // Normalize to [0, 1]
            spatialAttention[i] = std::clamp(salience, 0.0f, 1.0f);
            
            // Apply weight
            spatialAttention[i] *= params.spatialWeight;
        }
        
        // Normalize total attention
        float total = std::accumulate(spatialAttention.begin(), spatialAttention.end(), 0.0f);
        if (total > 0.0f) {
            for (float& attention : spatialAttention) {
                attention /= total;
            }
        }
        
        return spatialAttention;
    }
    
    std::vector<float> computeTemporalAttention(const std::vector<std::vector<float>>& temporalFeatures) {
        // Compute temporal attention from temporal features
        std::vector<float> temporalAttention(temporalFeatures.size(), 0.0f);
        
        for (size_t i = 0; i < temporalFeatures.size(); ++i) {
            const auto& feature = temporalFeatures[i];
            
            // Compute temporal change or pattern
            float change = 0.0f;
            if (feature.size() > 1) {
                for (size_t j = 1; j < feature.size(); ++j) {
                    change += std::abs(feature[j] - feature[j-1]);
                }
                change /= feature.size() - 1;
            }
            
            // High change = high attention
            temporalAttention[i] = std::clamp(change, 0.0f, 1.0f);
            temporalAttention[i] *= params.temporalWeight;
        }
        
        // Normalize total attention
        float total = std::accumulate(temporalAttention.begin(), temporalAttention.end(), 0.0f);
        if (total > 0.0f) {
            for (float& attention : temporalAttention) {
                attention /= total;
            }
        }
        
        return temporalAttention;
    }
    
    std::vector<float> computeSemanticAttention(const std::vector<float>& semanticFeatures) {
        // Compute semantic attention from semantic features
        std::vector<float> semanticAttention(semanticFeatures.size(), 0.0f);
        
        for (size_t i = 0; i < semanticFeatures.size(); ++i) {
            // Semantic relevance based on feature value
            semanticAttention[i] = std::clamp(semanticFeatures[i], 0.0f, 1.0f);
            semanticAttention[i] *= params.semanticWeight;
        }
        
        // Normalize total attention
        float total = std::accumulate(semanticAttention.begin(), semanticAttention.end(), 0.0f);
        if (total > 0.0f) {
            for (float& attention : semanticAttention) {
                attention /= total;
            }
        }
        
        return semanticAttention;
    }
    
    bool isFeatureRelevant(float attention, float threshold) const {
        return attention > threshold;
    }
    
    void setParameters(const FeatureParameters& params) {
        this->params = params;
    }
    
    void reset() {
        // Nothing to reset for feature attention
    }
    
private:
    FeatureParameters params;
};

// Attention Learning Implementation
struct AttentionLearning::Impl {
    PredictiveCoding predictiveCoding;
    SelectiveAttention selectiveAttention;
    FeatureAttention featureAttention;
    
    std::vector<std::vector<float>> attentionHistory;
    std::vector<float> attentionStrengths;
    
    Impl() : predictiveCoding(), selectiveAttention(), featureAttention() {
        attentionHistory.reserve(100);
        attentionStrengths.reserve(100);
    }
};

AttentionLearning::AttentionLearning()
    : pImpl(new Impl)
    , topDownControlEnabled_(true)
    , bottomUpControlEnabled_(true)
    , featureBasedControlEnabled_(true)
    , predictionBasedControlEnabled_(true)
    , attentionThreshold_(0.5f)
{
}

AttentionLearning::~AttentionLearning() = default;

void AttentionLearning::initialize(Brain* brain) {
    NLM_LOG_INFO("AttentionLearning initialized");
}

void AttentionLearning::update(const std::vector<float>& currentState,
                              const std::vector<float>& goals,
                              const std::vector<float>& context,
                              std::vector<float>& attentionWeights) {
    // Update attention allocation based on current state, goals, and context
    std::vector<float> spatialFeatures = extractSpatialFeatures(currentState);
    std::vector<float> temporalFeatures = extractTemporalFeatures(currentState);
    std::vector<float> semanticFeatures = extractSemanticFeatures(currentState);
    
    // Update attention mechanisms
    std::vector<float> featureAttention = pImpl->featureAttention.allocateFeatureAttention(
        spatialFeatures, temporalFeatures, semanticFeatures);
    
    // Apply top-down control
    std::vector<float> topDownAttention = pImpl->selectiveAttention.allocateAttention(
        currentState, goals, context);
    
    // Combine attention mechanisms
    if (topDownControlEnabled_ && topDownAttention.size() == featureAttention.size()) {
        for (size_t i = 0; i < featureAttention.size(); ++i) {
            featureAttention[i] = featureAttention[i] * (1.0f - 0.3f) + 
                               topDownAttention[i] * 0.3f;
        }
    }
    
    // Apply bottom-up control
    if (bottomUpControlEnabled_) {
        std::vector<float> bottomUpAttention = pImpl->selectiveAttention.computeFeatureSaliency(
            currentState, goals, context);
        
        for (size_t i = 0; i < featureAttention.size() && i < bottomUpAttention.size(); ++i) {
            featureAttention[i] = featureAttention[i] * (1.0f - 0.2f) + 
                               bottomUpAttention[i] * 0.2f;
        }
    }
    
    // Update attention weights
    attentionWeights = featureAttention;
    
    // Record attention history
    attentionHistory.push_back(attentionWeights);
    attentionStrengths.push_back(computeAttentionStrength(attentionWeights));
    
    // Keep history bounded
    while (attentionHistory.size() > 100) {
        attentionHistory.erase(attentionHistory.begin());
    }
    while (attentionStrengths.size() > 100) {
        attentionStrengths.erase(attentionStrengths.begin());
    }
}

void AttentionLearning::updatePrediction(const std::vector<float>& currentState,
                                        const std::vector<float>& predictedState,
                                        std::vector<float>& predictionError) {
    // Update prediction based on prediction error
    predictionError = computePredictionError(currentState, predictedState);
}

float AttentionLearning::computePredictionError(const std::vector<float>& actual,
                                               const std::vector<float>& predicted) const {
    return pImpl->predictiveCoding.computePredictionError(actual, predicted);
}

void AttentionLearning::updateFeatureAllocation(const std::vector<float>& features,
                                                const std::vector<float>& goals) {
    // Update feature allocation based on goals
    // This is a simplified version
    std::vector<float> spatialFeatures = features;
    std::vector<float> temporalFeatures(features.size(), 0.5f); // Placeholder
    std::vector<float> semanticFeatures = features;
    
    std::vector<float> featureAttention = pImpl->featureAttention.allocateFeatureAttention(
        spatialFeatures, temporalFeatures, semanticFeatures);
}

float AttentionLearning::computeAttentionStrength(const std::vector<float>& attentionWeights) const {
    // Compute overall attention strength
    if (attentionWeights.empty()) return 0.0f;
    
    float strength = 0.0f;
    for (float weight : attentionWeights) {
        strength += weight;
    }
    
    return strength / attentionWeights.size();
}

void AttentionLearning::setTopDownControlEnabled(bool enabled) {
    topDownControlEnabled_ = enabled;
}

void AttentionLearning::setBottomUpControlEnabled(bool enabled) {
    bottomUpControlEnabled_ = enabled;
}

void AttentionLearning::setFeatureBasedControlEnabled(bool enabled) {
    featureBasedControlEnabled_ = enabled;
}

void AttentionLearning::setPredictionBasedControlEnabled(bool enabled) {
    predictionBasedControlEnabled_ = enabled;
}

void AttentionLearning::setAttentionThreshold(float threshold) {
    attentionThreshold_ = threshold;
}

void AttentionLearning::reset() {
    pImpl->predictiveCoding.reset();
    pImpl->selectiveAttention.reset();
    pImpl->featureAttention.reset();
    
    pImpl->attentionHistory.clear();
    pImpl->attentionStrengths.clear();
}

std::vector<float> AttentionLearning::extractSpatialFeatures(const std::vector<float>& state) {
    // Extract spatial features from state
    // Simplified implementation: just return the state
    return state;
}

std::vector<float> AttentionLearning::extractTemporalFeatures(const std::vector<float>& state) {
    // Extract temporal features from state
    // Simplified implementation: return derived features
    std::vector<float> temporalFeatures(state.size(), 0.5f);
    return temporalFeatures;
}

std::vector<float> AttentionLearning::extractSemanticFeatures(const std::vector<float>& state) {
    // Extract semantic features from state
    // Simplified implementation: just return the state
    return state;
}

} // namespace nlm
