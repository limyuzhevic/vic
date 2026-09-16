#pragma once

#include "SensoryInput.hpp"
#include "Brain.hpp"
#include "core/Config/Config.hpp"
#include <memory>

namespace nlm {

// Real neural vision processing using NLM's LIF neuron dynamics
// Implements hierarchical feature extraction with edge detection, object recognition,
// and integration with brain's memory and attentional systems

class VisionProcessor {
public:
    VisionProcessor();
    ~VisionProcessor();
    
    // Initialize with brain reference and configuration
    void initialize(Brain* brain, std::shared_ptr<Config> config);
    
    // Process vision input through complete neural pipeline
    void process(const Vision& input);
    
    // Get feature representation
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
    // Reset processing state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Nested neural processing components
    class VisualFeatureExtractor;
    class EdgeDetector;
    class ObjectRecognizer;
    class FeatureMap;
};

} // namespace nlm
