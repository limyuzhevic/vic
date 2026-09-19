#pragma once

#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

// Vision processing - Neural feature extraction
// Implements biologically plausible vision processing using Gabor filters and spatial pooling
class VisionProcessor {
public:
    VisionProcessor();
    ~VisionProcessor();
    
    // Process vision input with neural feature extraction
    void process(const Vision& input);
    
    // Get feature representation - neural encoding of visual input
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality - size of neural representation
    size_t getFeatureDimensions() const;
    
    // Clear internal neural state (for plasticity)
    void clearFeatures();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
