#pragma once

#include "AgentBrain.hpp"
#include "../sensory/SensoryPercept.hpp"
#include <vector>
#include <unordered_map>

namespace nlm {

class SensoryProcessor {
public:
    SensoryProcessor() = default;
    ~SensoryProcessor() = default;
    
    // Process sensory percept and extract relevant features
    void process(const SensoryPercept& percept, std::vector<float>& processedSignals);
    
    // Get sensory feature weights
    const std::vector<float>& getVisionWeights() const { return visionWeights_; }
    const std::vector<float>& getTouchWeights() const { return touchWeights_; }
    const std::vector<float>& getInternalWeights() const { return internalWeights_; }
    const std::vector<float>& getProprioceptionWeights() const { return proprioceptionWeights_; }
    
    // Configuration
    void setVisionWeights(const std::vector<float>& weights) { visionWeights_ = weights; }
    void setTouchWeights(const std::vector<float>& weights) { touchWeights_ = weights; }
    void setInternalWeights(const std::vector<float>& weights) { internalWeights_ = weights; }
    void setProprioceptionWeights(const std::vector<float>& weights) { proprioceptionWeights_ = weights; }
    
    // Feature extraction methods
    std::vector<float> extractVisualFeatures(const std::vector<float>& vision) const;
    std::vector<float> extractTactileFeatures(const std::vector<float>& touch) const;
    std::vector<float> extractInternalFeatures(const std::vector<float>& internal) const;
    std::vector<float> extractProprioceptiveFeatures(const std::vector<float>& proprioception) const;
    
    // Feature normalization
    void normalizeFeatures(std::vector<float>& features, float targetMean = 0.5f, float targetStd = 0.2f) const;
    float computeFeatureVariance(const std::vector<float>& features) const;
    
    // Feature enhancement
    void enhanceFeatures(std::vector<float>& features, float enhancementFactor = 1.2f) const;
    void applyFeatureSmoothing(std::vector<float>& features, size_t windowSize = 3) const;
    
    // Statistics
    float getTotalFeatureCount() const;
    float getActiveFeatureCount(const std::vector<float>& features) const;
    
private:
    // Feature extraction parameters
    std::vector<float> visionWeights_;
    std::vector<float> touchWeights_;
    std::vector<float> internalWeights_;
    std::vector<float> proprioceptionWeights_;
    
    // Processing parameters
    float visionSensitivity_ = 1.0f;
    float touchSensitivity_ = 1.0f;
    float internalSensitivity_ = 1.0f;
    float proprioceptionSensitivity_ = 1.0f;
    
    // Feature extraction cache
    mutable std::unordered_map<size_t, std::vector<float>> featureCache_;
};

} // namespace nlm
