#pragma once

#include "SensoryInput.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Vision processing using NLM's neural machinery
class VisionProcessor {
public:
    VisionProcessor();
    ~VisionProcessor();
    
    // Process vision input through neural network
    void process(const Vision& input);
    
    // Get feature representation extracted by neural network
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
    // Apply neural processing to vision data
    void applyNeuralProcessing(const std::vector<float>& input);
    
    // Extract features using neural dynamics
    std::vector<float> extractNeuralFeatures(const Vision& input);
    
    // Learn feature representations
    void learnFeatureRepresentation(const std::vector<float>& input);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Vision feature extractor using NLM's spiking neural network
class VisionFeatureExtractor {
public:
    VisionFeatureExtractor();
    ~VisionFeatureExtractor();
    
    // Process visual input and extract features
    std::vector<float> process(const Vision& input);
    
    // Apply receptive field processing
    void applyReceptiveFields(const std::vector<float>& input);
    
    // Perform feature binding
    void performFeatureBinding(std::vector<float>& features);
    
    // Normalize features using neural dynamics
    void normalizeFeatures(std::vector<float>& features);
    
    // Get learned feature maps
    const std::vector<std::vector<float>>& getFeatureMaps() const;
    
    // Update feature extractor with new data
    void update(const std::vector<float>& trainingData);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm