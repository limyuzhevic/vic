#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Vision analysis processor
// PLACEHOLDER - Phase 2 will implement real vision processing using NLM's own neurons

class VisionAnalysis {
public:
    VisionAnalysis();
    ~VisionAnalysis();
    
    // Process vision input
    void process(const Vision& input);
    
    // Get feature representation
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
