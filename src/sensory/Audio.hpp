#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Real audio processing using NLM's neural machinery
// Implements biologically plausible auditory processing stages:
// - Cochlea: frequency analysis via basilar membrane place coding
// - Auditory nerve: temporal coding and phase locking
// - Brainstem: sound localization (ILD/ITD), reward modulation
// - Working memory: temporal and spectral feature binding
// - Adaptation: temporal integration and neural adaptation

class AudioProcessor {
public:
    AudioProcessor();
    ~AudioProcessor();
    
    // Process audio input through biological stages
    void process(const Audio& input);
    
    // Get feature representation for working memory
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
    // Configure auditory processing parameters
    void setTemporalIntegration(float integration);  // Adaptation time constant
    
    // Reward modulation for salient sounds
    void updateReward(float reward);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
