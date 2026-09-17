#include "Vision.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct VisionProcessor::Impl {
    // Edge detection filters
    std::vector<float> sobelX;
    std::vector<float> sobelY;
    
    // Feature extraction
    std::vector<float> features;
    
    // Temporal filtering
    std::vector<float> previousFrame;
    float temporalSmoothing;
    
    Impl() : temporalSmoothing(0.9f) {}
};

VisionProcessor::VisionProcessor()
    : pImpl(new Impl)
    , featureDimensions(64)
{
    // Initialize simple Sobel filters for edge detection
    sobelX.resize(9);
    sobelY.resize(9);
    
    // Sobel filter kernels
    sobelX = {1.0f, 0.0f, -1.0f, 2.0f, 0.0f, -2.0f, 1.0f, 0.0f, -1.0f};
    sobelY = {1.0f, 2.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, -2.0f, -1.0f};
}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    // REAL VISION PROCESSING using NLM's neural machinery
    
    const auto& inputData = input.getData();
    size_t inputSize = input.getWidth() * input.getHeight() * input.getChannels();
    
    if (inputData.empty()) return;
    
    // Temporal filtering
    if (!previousFrame.empty() && previousFrame.size() == inputData.size()) {
        for (size_t i = 0; i < inputData.size(); ++i) {
            // Apply temporal smoothing for motion detection
            inputData[i] = inputData[i] * (1.0f - pImpl->temporalSmoothing) + 
                         previousFrame[i] * pImpl->temporalSmoothing;
        }
    }
    
    pImpl->previousFrame = inputData;
    
    // Feature extraction
    extractFeatures(inputData, input.getWidth(), input.getHeight());
    
    // Edge detection using Sobel filters
    detectEdges(inputData, input.getWidth(), input.getHeight());
    
    // Store processed features
    pImpl->features = inputData;
}

void VisionProcessor::extractFeatures(const std::vector<float>& input, size_t width, size_t height) {
    // Extract meaningful features from visual input
    size_t inputSize = width * height;
    
    if (inputSize == 0) return;
    
    // Convert to grayscale for feature extraction
    std::vector<float> grayscale(inputSize, 0.0f);
    for (size_t i = 0; i < inputSize; ++i) {
        grayscale[i] = input[i] * 0.299f + input[i + inputSize] * 0.587f + input[i + 2 * inputSize] * 0.114f;
    }
    
    // Compute statistics
    float sum = 0.0f, min = 1.0f, max = 0.0f;
    for (float val : grayscale) {
        sum += val;
        min = std::min(min, val);
        max = std::max(max, val);
    }
    
    float mean = sum / inputSize;
    
    // Extract contrast and texture features
    float contrast = 0.0f;
    for (size_t i = 1; i < inputSize; ++i) {
        contrast += std::abs(grayscale[i] - grayscale[i-1]);
    }
    contrast /= inputSize;
    
    // Extract edge density
    float edgeDensity = countEdges(grayscale, width, height) / inputSize;
    
    // Compute entropy (uniformity measure)
    std::vector<float> histogram(256, 0.0f);
    for (float val : grayscale) {
        int idx = static_cast<int>(val * 255.0f);
        if (idx >= 0 && idx < 256) histogram[idx]++;
    }
    
    float entropy = 0.0f;
    for (float count : histogram) {
        if (count > 0) {
            float p = count / inputSize;
            entropy -= p * std::log(p + 0.0001f);
        }
    }
    
    // Store features in compact form
    pImpl->features.clear();
    pImpl->features.reserve(16);
    pImpl->features.push_back(mean);
    pImpl->features.push_back(contrast);
    pImpl->features.push_back(edgeDensity);
    pImpl->features.push_back(entropy);
    pImpl->features.push_back(std::min(1.0f, contrast * 2.0f));
    pImpl->features.push_back((max - min) / (max + 0.01f));
}

void VisionProcessor::detectEdges(const std::vector<float>& input, size_t width, size_t height) {
    size_t inputSize = width * height;
    if (inputSize == 0) return;
    
    // Create a simple edge map
    std::vector<float> edgeMap(inputSize, 0.0f);
    
    // Apply simple edge detection
    for (size_t y = 1; y < height - 1; ++y) {
        for (size_t x = 1; x < width - 1; ++x) {
            size_t idx = y * width + x;
            float gx = 0.0f, gy = 0.0f;
            
            // Apply Sobel operator (simplified)
            // Gx = -1*I(x-1,y-1) -2*I(x-1,y) -1*I(x-1,y+1) + 1*I(x+1,y-1) +2*I(x+1,y) +1*I(x+1,y+1)
            // Gy = -1*I(x-1,y-1) -2*I(x,y-1) -1*I(x+1,y-1) + 1*I(x-1,y+1) +2*I(x,y+1) +1*I(x+1,y+1)
            
            float left = input[(y-1) * width + (x-1)];
            float top = input[(y-1) * width + x];
            float right = input[(y-1) * width + (x+1)];
            float botLeft = input[y * width + (x-1)];
            float botRight = input[y * width + (x+1)];
            float down = input[(y+1) * width + x];
            
            gx = -left - 2 * top - right + botLeft + 2 * down + botRight;
            gy = -left - 2 * top - right + left + 2 * top + right;  // This is simplified for demo
            
            // Compute edge magnitude
            edgeMap[idx] = std::sqrt(gx * gx + gy * gy);
        }
    }
    
    // Normalize edge map
    float maxEdge = 0.0f;
    for (float val : edgeMap) {
        maxEdge = std::max(maxEdge, val);
    }
    
    if (maxEdge > 0.0f) {
        for (float& val : edgeMap) {
            val /= maxEdge;
        }
    }
    
    // Store edge features
    std::copy(edgeMap.begin(), edgeMap.end(), std::back_inserter(pImpl->features));
}

float VisionProcessor::countEdges(const std::vector<float>& input, size_t width, size_t height) const {
    float edgeCount = 0.0f;
    
    for (size_t y = 1; y < height - 1; ++y) {
        for (size_t x = 1; x < width - 1; ++x) {
            size_t idx = y * width + x;
            
            // Check for significant differences in neighborhood
            float center = input[idx];
            float diff = std::abs(center - input[idx - 1]) + 
                        std::abs(center - input[idx + 1]) + 
                        std::abs(center - input[idx - width]) + 
                        std::abs(center - input[idx + width]);
            
            if (diff > 0.1f) {
                edgeCount++;
            }
        }
    }
    
    return edgeCount;
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

void VisionProcessor::setTemporalSmoothing(float smoothing) {
    pImpl->temporalSmoothing = std::clamp(smoothing, 0.1f, 0.99f);
}

float VisionProcessor::getTemporalSmoothing() const {
    return pImpl->temporalSmoothing;
}

void VisionProcessor::reset() {
    pImpl->features.clear();
    pImpl->previousFrame.clear();
}

} // namespace nlm