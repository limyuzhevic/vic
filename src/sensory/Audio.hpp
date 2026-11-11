#pragma once

#include "SensoryInput.hpp"
#include <vector>
#include <string>
#include <memory>

namespace nlm {

// Audio processing for sensory input
// REAL IMPLEMENTATION - Phase 2 real audio processing using NLM's neural machinery
class AudioProcessor {
public:
    AudioProcessor();
    ~AudioProcessor();
    
    // Process audio input through neural network
    void process(const Audio& input);
    
    // Get extracted audio features
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
    // Get feature descriptors for recognition
    const std::vector<std::string>& getFeatureDescriptors() const;
    
    // Check if processor is ready
    bool isReady() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
