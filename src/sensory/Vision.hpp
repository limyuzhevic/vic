#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Vision processing (placeholder)
// TODO PHASE 6+: Implement real vision processing using NLM's neural substrate for advanced users
// (Connects to brain for perception, enables cognitive mapping of environment)

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
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
