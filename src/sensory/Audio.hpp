#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Audio processing (placeholder)
// PLACEHOLDER - Phase 2 will implement real audio processing using NLM's own neurons

class Brain;

class AudioProcessor {
public:
    AudioProcessor();
    ~AudioProcessor();
    
    // Process audio input
    void process(const Audio& input);
    
    // Process audio input with brain's neural substrate
    void process(const Audio& input, Brain* brain);
    
    // Get feature representation
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
    // Get audio-specific features
    float getSpectralCentroid() const;
    float getRMSEnergy() const;
    float getZeroCrossingRate() const;
    float getSpectralRollOff() const;
    
private:
    // Helper functions for audio feature extraction
    void extractAudioFeatures(const std::vector<float>& data);
    void computeBasicStats(const std::vector<float>& data);
    void computeSpectralFeatures(const std::vector<float>& data, 
                                size_t windowSize, size_t hopSize, 
                                size_t numFrames);
    void computeTemporalFeatures(const std::vector<float>& data,
                                size_t windowSize, size_t hopSize,
                                size_t numFrames);
    void aggregateFeatures();
    float computeSpectralCentroid(const std::vector<float>& spectrum);
    float computeSpectralRollOff(const std::vector<float>& spectrum);
    float computeSpectralFlux(const std::vector<float>& spectrum, const std::vector<float>& prevSpectrum);
    float computeSpectralEntropy(const std::vector<float>& spectrum);
    float estimatePitch(const std::vector<float>& frame);
    float estimateFormant(const std::vector<float>& frame);
    float computeAttackTime(const std::vector<float>& frame);
    float computeDecayTime(const std::vector<float>& frame);
    void computeFFTMagnitude(const std::vector<float>& input, std::vector<float>& output);
    
    void encodeFeaturesToNeural(Brain* brain);
    void storeFeaturesInWorkingMemory(Brain* brain);
    void updateSensorySynapticWeights(Brain* brain);
    
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
