#pragma once

#include "SensoryInput.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Vision processing (placeholder)
// PLACEHOLDER - Phase 2 will implement real vision processing using NLM's own neurons

class VisionProcessor {
public:
    VisionProcessor();
    ~VisionProcessor();
    
    // Process vision input
    void process(const SensoryInput& input);
    
    // Get feature representation
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
    // Get feature at specific location
    float getFeature(size_t x, size_t y, size_t channel) const;
    
    // Reset processor for new sequence
    void reset();
    
    // Enable/disable vision processing
    void enable(bool enabled) { pImpl->enabled = enabled; }
    bool isEnabled() const { return pImpl->enabled; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
