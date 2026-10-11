#include "EnhancedSensoryProcessors.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct VisionProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    size_t width;
    size_t height;
    size_t channels;
    
    // Neural processing state
    std::vector<float> neuralActivation;
    std::vector<float> spikeHistory;
    
    // Processing parameters
    float contrastThreshold;
    float edgeSensitivity;
    float temporalDecay;
    
    Impl() : featureDimensions(0), width(0), height(0), channels(0),
             contrastThreshold(0.5f), edgeSensitivity(0.7f), temporalDecay(0.95f) {}
};

VisionProcessor::VisionProcessor() : pImpl(std::make_unique<Impl>()) {}

VisionProcessor::~VisionProcessor() = default;

const char* VisionProcessor::getTypeName() const {
    return "VisionProcessor";
}

bool VisionProcessor::validateInput(const SensoryInput& input) const {
    if (!SensoryProcessor::validateInput(input)) {
        return false;
    }
    
    const auto& data = input.getData();
    size_t expectedSize = pImpl->width * pImpl->height * pImpl->channels;
    
    return data.size() == expectedSize && expectedSize > 0;
}

size_t VisionProcessor::getWidth() const {
    return pImpl->width;
}

size_t VisionProcessor::getHeight() const {
    return pImpl->height;
}

size_t VisionProcessor::getChannels() const {
    return pImpl->channels;
}

void VisionProcessor::process(const SensoryInput& input) {
    // Validate input
    if (!validateInput(input)) {
        throw std::runtime_error("Invalid vision input for processing");
    }
    
    reset();
    
    const auto& data = input.getData();
    pImpl->width = input.getDimensions();  // Simplified - assuming square image
    pImpl->height = pImpl->width;
    pImpl->channels = 3;  // RGB
    pImpl->featureDimensions = data.size();
    
    // Store raw data as initial features
    pImpl->features = data;
    
    // Apply neural processing pipeline
    applyNeuronalResponse(pImpl->features, pImpl->neuralActivation);
    computeSpatialModulation(pImpl->neuralActivation, pImpl->features);
    applyTemporalPooling(pImpl->features, pImpl->neuralActivation);
    
    // Extract meaningful features through neural operations
    detectEdges();
    extractColorFeatures();
    computeTextureFeatures();
    applyNeuralFilter();
    
    // Normalize features
    normalizeFeatures();
}

void VisionProcessor::detectEdges() {
    // Simplified Sobel edge detection using neural approximation
    size_t width = pImpl->width;
    size_t height = pImpl->height;
    
    std::vector<float> grayImage(width * height);
    for (size_t i = 0; i < width * height; ++i) {
        // Convert RGB to grayscale (assuming RGB)
        float r = pImpl->features[i * 3];
        float g = pImpl->features[i * 3 + 1];
        float b = pImpl->features[i * 3 + 2];
        grayImage[i] = 0.299f * r + 0.587f * g + 0.114f * b;
    }
    
    std::vector<float> edgeFeatures;
    edgeFeatures.reserve(width * height);
    
    // Simplified edge detection (convolution with Sobel-like kernel)
    for (size_t y = 1; y < height - 1; ++y) {
        for (size_t x = 1; x < width - 1; ++x) {
            size_t idx = y * width + x;
            
            // Calculate gradient magnitude (simplified)
            float center = grayImage[idx];
            float left = grayImage[idx - 1];
            float right = grayImage[idx + 1];
            float up = grayImage[idx - width];
            float down = grayImage[idx + width];
            
            float gradX = 0.5f * (right - left);
            float gradY = 0.5f * (down - up);
            float magnitude = std::sqrt(gradX * gradX + gradY * gradY);
            
            edgeFeatures.push_back(std::min(1.0f, magnitude * pImpl->edgeSensitivity));
        }
    }
    
    // Add edge features to overall feature vector
    pImpl->features.insert(pImpl->features.end(), edgeFeatures.begin(), edgeFeatures.end());
    pImpl->featureDimensions = pImpl->features.size();
}

void VisionProcessor::extractColorFeatures() {
    // Extract color statistics (mean, variance, dominant color)
    std::vector<float> colorStats;
    colorStats.reserve(12);  // 3 channels * 4 stats each
    
    for (size_t c = 0; c < pImpl->channels; ++c) {
        std::vector<float> channelValues;
        for (size_t i = 0; i < pImpl->width * pImpl->height; ++i) {
            channelValues.push_back(pImpl->features[i * pImpl->channels + c]);
        }
        
        // Calculate statistics
        float sum = std::accumulate(channelValues.begin(), channelValues.end(), 0.0f);
        float mean = sum / channelValues.size();
        
        float sumSq = 0.0f;
        for (float val : channelValues) {
            sumSq += (val - mean) * (val - mean);
        }
        float variance = sumSq / channelValues.size();
        
        // Find dominant color range
        float minVal = *std::min_element(channelValues.begin(), channelValues.end());
        float maxVal = *std::max_element(channelValues.begin(), channelValues.end());
        
        colorStats.push_back(mean);
        colorStats.push_back(std::sqrt(variance));
        colorStats.push_back(minVal);
        colorStats.push_back(maxVal);
    }
    
    // Add color features
    pImpl->features.insert(pImpl->features.end(), colorStats.begin(), colorStats.end());
    pImpl->featureDimensions = pImpl->features.size();
}

void VisionProcessor::computeTextureFeatures() {
    // Simplified texture analysis using local variance
    std::vector<float> textureFeatures;
    
    size_t blockSize = 4;  // 4x4 blocks
    size_t blocksX = pImpl->width / blockSize;
    size_t blocksY = pImpl->height / blockSize;
    
    for (size_t by = 0; by < blocksY; ++by) {
        for (size_t bx = 0; bx < blocksX; ++bx) {
            float sum = 0.0f;
            float sumSq = 0.0f;
            size_t count = 0;
            
            // Calculate local mean and variance
            for (size_t y = by * blockSize; y < std::min((by + 1) * blockSize, pImpl->height); ++y) {
                for (size_t x = bx * blockSize; x < std::min((bx + 1) * blockSize, pImpl->width); ++x) {
                    size_t idx = y * pImpl->width + x;
                    float val = pImpl->features[idx * pImpl->channels];  // Use red channel
                    sum += val;
                    sumSq += val * val;
                    count++;
                }
            }
            
            if (count > 0) {
                float mean = sum / count;
                float variance = (sumSq / count) - (mean * mean);
                textureFeatures.push_back(std::sqrt(std::max(0.0f, variance)));
            }
        }
    }
    
    // Add texture features
    pImpl->features.insert(pImpl->features.end(), textureFeatures.begin(), textureFeatures.end());
    pImpl->featureDimensions = pImpl->features.size();
}

void VisionProcessor::applyNeuralFilter() {
    // Apply neural filtering to enhance features
    std::vector<float> filteredFeatures = pImpl->features;
    
    // Simple neural filter: apply sigmoid activation
    for (float& val : filteredFeatures) {
        val = 1.0f / (1.0f + std::exp(-val * 2.0f + 1.0f));  // Shifted sigmoid
    }
    
    pImpl->features = filteredFeatures;
}

void VisionProcessor::applyNeuronalResponse(const std::vector<float>& input, std::vector<float>& output) {
    // Simulate neuronal response using leaky integrate-and-fire dynamics
    output.clear();
    output.reserve(input.size());
    
    RandomGenerator& rng = RandomGenerator::getInstance();
    
    for (float val : input) {
        // Leaky integration
        float integrated = val * 0.1f;  // Input strength
        
        // Threshold crossing
        if (integrated > 1.0f) {
            // Spike generation
            float spike = 1.0f / (1.0f + std::exp(-10.0f * (integrated - 1.5f)));
            output.push_back(spike);
            
            // Refractory period simulation
            integrated *= 0.1f;
        } else {
            // Sub-threshold response
            float activation = 1.0f / (1.0f + std::exp(-5.0f * (integrated - 0.5f)));
            output.push_back(activation);
        }
    }
}

void VisionProcessor::computeSpatialModulation(const std::vector<float>& input, std::vector<float>& output) {
    // Apply spatial modulation based on receptive field properties
    output.clear();
    output.reserve(input.size());
    
    // Center-surround modulation (simple difference of Gaussians)
    for (size_t i = 0; i < input.size(); ++i) {
        float center = input[i];
        
        // Simple center-surround: center value minus neighbors
        float surround = 0.0f;
        int neighborCount = 0;
        
        // Check 4-neighborhood
        if (i >= 1) surround += input[i - 1]; neighborCount++;
        if (i < input.size() - 1) surround += input[i + 1]; neighborCount++;
        
        surround /= neighborCount;
        
        float modulated = center - surround;
        output.push_back(std::tanh(modulated * 2.0f));  // Bound output
    }
}

void VisionProcessor::applyTemporalPooling(const std::vector<float>& input, std::vector<float>& output) {
    // Apply temporal pooling to stabilize features over time
    output.clear();
    output.reserve(input.size());
    
    // Exponential moving average (simple temporal integration)
    if (!pImpl->spikeHistory.empty()) {
        float alpha = 1.0f - pImpl->temporalDecay;
        for (size_t i = 0; i < input.size(); ++i) {
            float pooled = alpha * input[i] + (1.0f - alpha) * pImpl->spikeHistory[i % pImpl->spikeHistory.size()];
            output.push_back(pooled);
        }
    } else {
        output = input;
    }
    
    // Update spike history
    pImpl->spikeHistory.assign(input.begin(), input.end());
    if (pImpl->spikeHistory.size() > Impl::MAX_SPIKE_HISTORY) {
        pImpl->spikeHistory.erase(pImpl->spikeHistory.begin(), 
                                 pImpl->spikeHistory.begin() + 
                                 (pImpl->spikeHistory.size() - Impl::MAX_SPIKE_HISTORY));
    }
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
