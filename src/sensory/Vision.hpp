#pragma once

#include "SensoryInput.hpp"

namespace nlm {

// Real vision processing using NLM's neural machinery
// Implements biologically plausible visual processing stages:
// - Retina: center-surround receptive fields, population coding
// - LGN: contrast enhancement, lateral inhibition
// - V1: orientation selectivity, edge detection
// - Working memory: feature binding via neural populations
// - Attention: saccadic eye movements, neuromodulation

class VisionProcessor {
public:
    VisionProcessor();
    ~VisionProcessor();
    
    // Process vision input through biological stages
    void process(const Vision& input);
    
    // Get feature representation for working memory
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
    // Configure visual processing parameters
    void setFieldOfView(float fov);           // degrees
    void setResolution(float resolution);     // cycles per degree
    
    // Attention/saccade control
    void updateSaccade(float reward);         // reward-modulated attention
    float getLastSaccadeTime() const;         // time since last saccade
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
