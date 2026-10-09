#pragma once

#include "SensoryInput.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations for neural components
class Brain;
class NeuralRegion;
class STDP;
class Hebbian;
class StructuralPlasticity;

// Audio processing (REALISTIC IMPLEMENTATION)
// Phase 4-6: Real audio processing using NLM's neural machinery
// Implements full auditory pathway simulation including transduction, cochlear
// processing, auditory nerve firing, binaural processing, and neural plasticity

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
    
    // Get auditory features for neural integration
    float getSoundLocalizationAzimuth() const;
    float getSoundLocalizationElevation() const;
    float getPitch() const;
    float getTimbreComplexity() const;
    float getSpectralFlux() const;
    float getEnergyEnvelope() const;
    
    // Neural integration methods
    void integrateWithBrain(Brain* brain, RegionId regionId);
    void updateNeuralConnections(STDP* stdp, Hebbian* hebbian, StructuralPlasticity* structural);
    
    // Feature extraction for higher cognitive processing
    std::vector<float> extractPhoneticFeatures() const;
    std::vector<float> extractEmotionalFeatures() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
