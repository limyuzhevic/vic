#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Advanced audio processing using NLM's neural machinery
// Implements real spectral analysis, rhythm detection, and neural integration
// with the brain's spiking neural network for auditory pattern recognition

class AudioProcessor {
public:
    AudioProcessor();
    ~AudioProcessor();
    
    // Configure audio processing parameters
    void configure(float frequencyResolution = 0.01f,
                   float temporalResolution = 0.001f,
                   float spectralCentroidWeight = 0.5f,
                   float zeroCrossingRateWeight = 0.3f,
                   float temporalSmoothing = 0.7f);
    
    // Process audio input through neural machinery
    // Optional brain parameter allows direct neural integration
    void process(const Audio& input, class Brain* brain = nullptr);
    
    // Get feature representation
    const std::vector<float>& getFeatures() const;
    
    // Get feature vector with fixed size output for API compatibility
    void getFeatureVector(float* output, size_t maxSize) const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
    // Get current processing parameters
    float getFrequencyResolution() const;
    float getTemporalResolution() const;
    float getSpectralCentroidWeight() const;
    float getZeroCrossingRateWeight() const;
    
    // Reset processor state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm