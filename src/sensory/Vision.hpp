#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Vision processing (placeholder)
// PLACEHOLDER - Phase 2 will implement real vision processing using NLM's own neurons

class VisionProcessor {
public:
    VisionProcessor();
    ~VisionProcessor();
    
    // Process vision input
    void process(const Vision& input);
    
    // Get feature representation
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
    // New enhanced vision processing methods
    void setEdgeSensitivity(float sensitivity);
    void setColorSensitivity(float sensitivity);
    void setMotionSensitivity(float sensitivity);
    void setFeatureLearningRate(float rate);
    float getFeatureLearningRate() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
