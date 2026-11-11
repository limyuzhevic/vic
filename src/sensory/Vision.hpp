#pragma once

#include "SensoryInput.hpp"
#include <vector>
#include <string>
#include <memory>

namespace nlm {

// Vision processing for sensory input
// REAL IMPLEMENTATION - Phase 2 real vision processing using NLM's neural machinery
class VisionProcessor {
public:
    VisionProcessor();
    ~VisionProcessor();
    
    // Process vision input through neural network
    void process(const Vision& input);
    
    // Get extracted visual features
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
    // Get feature descriptors for recognition
    const std::vector<std::string>& getFeatureDescriptors() const;
    
    // Check if processor is ready
    bool isReady() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
