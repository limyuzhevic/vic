#pragma once

#include "SensoryInput.hpp"
#include <string>

namespace nlm {

// Vision processing using NLM's neural machinery
class VisionProcessor {
public:
    VisionProcessor();
    ~VisionProcessor();
    
    // Process vision input with neural-based feature extraction
    void process(const Vision& input);
    
    // Get feature representation
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
    // Configuration methods for neural processing
    void setPreprocessingGain(float gain);
    void setNoiseReductionFactor(float factor);
    void enableNeuralFeatures(bool enable);
    
    // Get configuration status
    bool getNeuralFeaturesEnabled() const;
    float getPreprocessingGain() const;
    float getNoiseReductionFactor() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
