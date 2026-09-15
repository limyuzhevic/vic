#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Forward declaration
class Metrics;

// Audio processing (placeholder)
// PLACEHOLDER - Phase 2 will implement real audio processing using NLM's own neurons

class AudioProcessor {
public:
    AudioProcessor();
    ~AudioProcessor();
    
    // Process audio input with metrics
    void process(const Audio& input, std::shared_ptr<Metrics> metrics = nullptr);
    
    // Get feature representation
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
    // Get metrics collector
    const std::shared_ptr<Metrics>& getMetrics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
