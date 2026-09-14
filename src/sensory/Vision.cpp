#include "Vision.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

struct VisionProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    
    Impl() : featureDimensions(0) {}
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    // Get raw vision data
    const std::vector<float>& rawData = input.getData();
    size_t width = input.getWidth();
    size_t height = input.getHeight();
    size_t channels = input.getChannels();
    
    if (rawData.empty()) {
        pImpl->features.clear();
        pImpl->featureDimensions = 0;
        return;
    }
    
    // Simple feature extraction pipeline
    pImpl->features.clear();
    
    // 1. Extract basic statistics (mean, variance, edges)
    if (channels == 1) {
        // Grayscale processing
        float sum = std::accumulate(rawData.begin(), rawData.end(), 0.0f);
        float mean = sum / rawData.size();
        
        float varianceSum = 0.0f;
        for (float pixel : rawData) {
            varianceSum += (pixel - mean) * (pixel - mean);
        }
        float variance = varianceSum / rawData.size();
        
        pImpl->features.push_back(mean);
        pImpl->features.push_back(variance);
        
        // Edge detection (simple gradient)
        float edgeSum = 0.0f;
        for (size_t i = 1; i < rawData.size(); ++i) {
            edgeSum += std::abs(rawData[i] - rawData[i-1]);
        }
        float edgeStrength = edgeSum / (rawData.size() - 1);
        pImpl->features.push_back(edgeStrength);
        
        // Centralization (check if center is brighter/darker)
        if (width > 0 && height > 0) {
            size_t center = (width * height) / 2;
            float centerPixel = rawData[center];
            pImpl->features.push_back(centerPixel - mean);
        }
    } else if (channels >= 3) {
        // RGB processing - compute luminance
        std::vector<float> luminance;
        luminance.reserve(rawData.size() / channels);
        
        for (size_t i = 0; i + 2 < rawData.size(); i += channels) {
            // Standard luminance formula: 0.299*R + 0.587*G + 0.114*B
            float r = rawData[i];
            float g = rawData[i + 1];
            float b = rawData[i + 2];
            float lum = 0.299f * r + 0.587f * g + 0.114f * b;
            luminance.push_back(lum);
        }
        
        // Apply same statistical features to luminance
        if (!luminance.empty()) {
            float sum = std::accumulate(luminance.begin(), luminance.end(), 0.0f);
            float mean = sum / luminance.size();
            
            float varianceSum = 0.0f;
            for (float pixel : luminance) {
                varianceSum += (pixel - mean) * (pixel - mean);
            }
            float variance = varianceSum / luminance.size();
            
            pImpl->features.push_back(mean);
            pImpl->features.push_back(variance);
            
            // Edge detection on luminance
            float edgeSum = 0.0f;
            for (size_t i = 1; i < luminance.size(); ++i) {
                edgeSum += std::abs(luminance[i] - luminance[i-1]);
            }
            float edgeStrength = edgeSum / (luminance.size() - 1);
            pImpl->features.push_back(edgeStrength);
        }
    }
    
    // Add color information features for multi-channel
    if (channels > 1) {
        // Simple color distribution features
        std::vector<size_t> channelCounts(channels, 0);
        for (size_t i = 0; i < rawData.size(); i += channels) {
            for (size_t c = 0; c < channels && i + c < rawData.size(); ++c) {
                channelCounts[c]++;
            }
        }
        
        for (size_t c = 0; c < std::min<size_t>(channels, 3); ++c) {
            float count = static_cast<float>(channelCounts[c]);
            float percentage = (count / (rawData.size() / channels)) * 100.0f;
            pImpl->features.push_back(percentage);
        }
    }
    
    pImpl->featureDimensions = pImpl->features.size();
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
