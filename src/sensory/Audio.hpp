#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Audio processing
// Phase 6: Simplified audio processing
// Current implementation: Basic feature extraction from audio input
// TODO FUTURE PHASE: Implement real audio processing using NLM's own neural machinery

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
