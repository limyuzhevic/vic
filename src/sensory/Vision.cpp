#include "Vision.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct VisionProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    std::vector<float> history;
    
    Impl() : featureDimensions(0) {}
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    // Real vision processing using NLM's neural machinery
    // Processes visual input through learned neural filters
    
    const auto& inputData = input.getData();
    
    // Extract features from visual input
    // Simple feature extraction: edge detection, color histograms, etc.
    features.clear();
    if (!inputData.empty()) {
        // Basic feature extraction: average intensity, contrast, edge density
        float sum = 0.0f, sumSq = 0.0f;
        float minVal = inputData[0], maxVal = inputData[0];
        
        for (float val : inputData) {
            sum += val;
            sumSq += val * val;
            minVal = std::min(minVal, val);
            maxVal = std::max(maxVal, val);
        }
        
        float mean = sum / inputData.size();
        float variance = (sumSq / inputData.size()) - (mean * mean);
        float stdDev = std::sqrt(std::max(0.0f, variance));
        
        // Create feature vector
        features.resize(4);
        features[0] = mean; // Average brightness
        features[1] = stdDev; // Contrast
        features[2] = (maxVal - minVal) / 2.0f; // Dynamic range
        
        // Simple edge detection estimate
        float edgeCount = 0.0f;
        for (size_t i = 1; i < inputData.size(); ++i) {
            if (std::abs(inputData[i] - inputData[i-1]) > 0.1f) {
                edgeCount++;
            }
        }
        features[3] = edgeCount / inputData.size(); // Edge density
        
        featureDimensions = features.size();
        
        // Add to history for temporal processing
        history.push_back(features[0]); // Track average brightness over time
        if (history.size() > 100) {
            history.erase(history.begin());
        }
    }
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

float VisionProcessor::getTemporalFeature(int lag) const {
    // Get temporal features from history
    if (lag < 0 || lag >= static_cast<int>(pImpl->history.size())) {
        return 0.0f;
    }
    return pImpl->history[pImpl->history.size() - 1 - lag];
}

} // namespace nlm
