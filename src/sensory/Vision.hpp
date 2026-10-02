#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Advanced vision processing using NLM's neural machinery
// Implements real feature extraction, edge detection, motion processing
// and neural integration with the brain's spiking neural network

class VisionProcessor {
public:
    VisionProcessor();
    ~VisionProcessor();
    
    // Configure vision processing parameters
    void configure(float contrastThreshold = 0.5f, 
                   float edgeSensitivity = 1.0f,
                   float motionDetection = 0.3f,
                   float temporalSmoothing = 0.7f);
    
    // Process vision input through neural machinery
    // Optional brain parameter allows direct neural integration
    void process(const Vision& input, class Brain* brain = nullptr);
    
    // Get feature representation
    const std::vector<float>& getFeatures() const;
    
    // Get feature vector with fixed size output for API compatibility
    void getFeatureVector(float* output, size_t maxSize) const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
    // Get current processing parameters
    float getContrastThreshold() const;
    float getEdgeSensitivity() const;
    float getMotionDetection() const;
    
    // Reset processor state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm