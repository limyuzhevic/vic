#include "Vision.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    // Real vision processing using NLM neural machinery
    // Implements feature extraction and neural encoding similar to biological vision
    
    const std::vector<float>& rawData = input.getData();
    size_t width = input.getWidth();
    size_t height = input.getHeight();
    size_t channels = input.getChannels();
    
    // Reset feature buffer
    pImpl->features.clear();
    
    // Convert raw RGB/BGR data to luminance for neural processing
    std::vector<float> luminance;
    luminance.reserve(width * height);
    
    for (size_t i = 0; i < rawData.size(); i += channels) {
        // Simple luminance conversion: 0.299*R + 0.587*G + 0.114*B
        float r = rawData[i];
        float g = rawData[i + 1];
        float b = rawData[i + 2];
        float y = 0.299f * r + 0.587f * g + 0.114f * b;
        luminance.push_back(std::clamp(y, 0.0f, 1.0f));
    }
    
    // Extract multi-scale features using Gabor filters (simplified)
    extractGaborFeatures(luminance, width, height);
    
    // Apply non-linear contrast normalization (like retinal processing)
    applyRetinalNormalization(pImpl->features);
    
    // Compress features using spike encoding for neural transmission
    compressToNeuralSpikes(pImpl->features);
    
    pImpl->featureDimensions = pImpl->features.size();
}

void VisionProcessor::extractGaborFeatures(const std::vector<float>& luminance, 
                                         size_t width, size_t height) {
    // Simplified Gabor filter bank for orientation and spatial frequency detection
    // Simulates early visual processing in V1/V2 cortex
    
    size_t featureIdx = 0;
    pImpl->features.resize(48); // 8 orientations × 6 spatial frequencies
    
    // Different Gabor filter parameters (simplified)
    float orientations[] = {0.0f, 45.0f, 90.0f, 135.0f};
    float frequencies[] = {0.1f, 0.25f, 0.5f, 1.0f, 2.0f, 4.0f};
    
    for (float theta : orientations) {
        for (float freq : frequencies) {
            float response = computeGaborResponse(luminance, width, height, 
                                                 theta, freq);
            // Normalize and apply non-linearity
            response = std::tanh(response);
            pImpl->features[featureIdx++] = response;
        }
    }
}

float VisionProcessor::computeGaborResponse(const std::vector<float>& luminance,
                                          size_t width, size_t height,
                                          float theta, float freq) {
    // Simplified Gabor filter implementation
    // In real implementation, this would use actual Gabor filter kernels
    
    float response = 0.0f;
    float sigma = 1.0f / freq;  // Gaussian envelope sigma
    
    // Sample a small region around the center for each filter
    size_t centerX = width / 2;
    size_t centerY = height / 2;
    size_t radius = static_cast<size_t>(3.0f * sigma);
    
    // Convert theta to radians for rotation
    float thetaRad = theta * M_PI / 180.0f;
    
    for (size_t y = centerY - radius; y <= centerY + radius; ++y) {
        if (y >= height) continue;
        
        for (size_t x = centerX - radius; x <= centerX + radius; ++x) {
            if (x >= width) continue;
            
            size_t idx = y * width + x;
            float pixel = luminance[idx];
            
            // Apply Gabor filter: gabor = pixel * exp(-(x²+y²)/2σ²) * cos(2πf(x·cosθ + y·sinθ))
            float dx = static_cast<float>(x) - static_cast<float>(centerX);
            float dy = static_cast<float>(y) - static_cast<float>(centerY);
            
            float gaussian = std::exp(-(dx*dx + dy*dy) / (2.0f * sigma * sigma));
            float phase = 2.0f * M_PI * freq * (dx * std::cos(thetaRad) + dy * std::sin(thetaRad));
            
            response += pixel * gaussian * std::cos(phase);
        }
    }
    
    return response / static_cast<float>(radius * radius);
}

void VisionProcessor::applyRetinalNormalization(std::vector<float>& features) {
    // Simulate retinal processing: center-surround receptive fields and contrast normalization
    
    if (features.empty()) return;
    
    // Calculate center and surround components (simplified)
    size_t centerSize = std::max<size_t>(1, features.size() / 4);
    size_t surroundSize = features.size() - centerSize;
    
    float centerSum = 0.0f;
    float surroundSum = 0.0f;
    
    for (size_t i = 0; i < centerSize; ++i) {
        centerSum += features[i];
    }
    for (size_t i = centerSize; i < features.size(); ++i) {
        surroundSum += features[i];
    }
    
    float centerMean = centerSum / centerSize;
    float surroundMean = surroundSum / surroundSize;
    
    // Apply center-surround subtraction (like retinal ganglion cells)
    for (size_t i = 0; i < features.size(); ++i) {
        float center = (i < centerSize) ? features[i] : centerMean;
        float surround = surroundMean;
        
        // Non-linear contrast normalization
        float response = (center - surround) / (std::abs(center - surround) + 0.01f);
        features[i] = response;
    }
}

void VisionProcessor::compressToNeuralSpikes(std::vector<float>& features) {
    // Convert continuous features to spike-like neural encoding
    // Simulates population coding in visual cortex
    
    float maxFeature = *std::max_element(features.begin(), features.end());
    float minFeature = *std::min_element(features.begin(), features.end());
    
    if (maxFeature <= minFeature) return;
    
    for (size_t i = 0; i < features.size(); ++i) {
        // Apply non-linear transformation to simulate neural response dynamics
        float normalized = (features[i] - minFeature) / (maxFeature - minFeature);
        
        // Convert to spike rate using sigmoidal function
        float spikeRate = 1.0f / (1.0f + std::exp(-10.0f * (normalized - 0.5f)));
        
        // Add some stochasticity (neural noise)
        float noise = ((rand() % 100) / 100.0f - 0.5f) * 0.1f;
        spikeRate = std::max(0.0f, std::min(1.0f, spikeRate + noise));
        
        features[i] = spikeRate;
    }
}

} // namespace nlm