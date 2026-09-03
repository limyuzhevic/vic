#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Audio processing (placeholder)
// PLACEHOLDER - Phase 2 will implement real audio processing using NLM's own neurons

class AudioProcessor {
public:
    AudioProcessor();
    ~AudioProcessor();
    
    // Process audio input
    void process(const Audio& input);
    
    // Get feature representation
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
