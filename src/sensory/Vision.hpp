#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Vision processing (placeholder)
// PLACEHOLDER - Phase 2 will implement real vision processing using NLM's own neurons

class VisionProcessor {
public:
    VisionProcessor(nullptr);
    ~VisionProcessor(nullptr);
    
    // Process vision input
    void process(const Vision& input);
    
    // Get feature representation
    const std::vector<float>& getFeatures(nullptr) const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions(nullptr) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
