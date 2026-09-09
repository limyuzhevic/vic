#pragma once

#include "SensoryInput.hpp"
#include "../brain/Neuron.hpp"
#include <memory>
#include <vector>

namespace nlm {

class VisionProcessor;

// Vision processing system
// Uses NLM's neural machinery for edge detection and feature extraction
class Vision : public SensoryInput {
public:
    Vision();
    explicit Vision(size_t width, size_t height, size_t channels = 3);
    ~Vision() override;
    
    const char* getType() const override;
    const std::vector<float>& getData() const override;
    size_t getDimensions() const override;
    std::unique_ptr<SensoryInput> clone() const override;
    
    // Set image data with automatic processing
    void setData(const std::vector<float>& data);
    void setData(float* data, size_t size);
    
    // Process vision through neural networks for feature extraction
    void processWithNeurons(const std::vector<Neuron*>& featureNeurons);
    
    // Get processed features
    const std::vector<float>& getProcessedFeatures() const;
    
    // Image properties
    size_t getWidth() const;
    size_t getHeight() const;
    size_t getChannels() const;
    
    // Apply neural transformations
    void applyEdgeDetection(float threshold = 0.5f);
    void applyGaussianBlur(float sigma = 1.0f);
    void applySobelOperator();
    
    // Get feature vector for brain input
    std::vector<float> getFeatureVector() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Vision processing system
// Integrates with NLM's neural machinery
class VisionProcessor {
public:
    VisionProcessor();
    ~VisionProcessor();
    
    // Process vision input through neural networks
    void process(const Vision& input, const std::vector<Neuron*>& neurons);
    
    // Get feature representation from neural activity
    const std::vector<float>& getFeatures() const;
    
    // Get feature dimensionality
    size_t getFeatureDimensions() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
