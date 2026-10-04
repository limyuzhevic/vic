#include "Vision.hpp"

namespace nlm {

struct VisionProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    
    Impl() : featureDimensions(0) {}
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    // IMPROVED: Enhanced vision processing with better feature extraction
    pImpl->features.clear();
    
    const auto& inputData = input.getData();
    if (inputData.empty()) {
        NLM_LOG_WARNING("VisionProcessor: Empty input data");
        return;
    }
    
    // Basic feature extraction for Phase 2
    size_t width = input.getWidth();
    size_t height = input.getHeight();
    
    // Extract simple features: average, edges, and intensity distribution
    float sum = std::accumulate(inputData.begin(), inputData.end(), 0.0f);
    float mean = sum / inputData.size();
    
    // Calculate edge-like features (simplified gradient)
    float edgeFeatures = 0.0f;
    for (size_t i = 1; i < inputData.size(); ++i) {
        edgeFeatures += std::abs(inputData[i] - inputData[i-1]);
    }
    
    // Feature intensity distribution (binned)
    std::array<size_t, 8> intensityBins = {0};
    for (float val : inputData) {
        float normalized = (val + 1.0f) / 2.0f;  // Assume [-1, 1] range
        size_t bin = static_cast<size_t>(normalized * 8.0f);
        bin = std::min(bin, static_cast<size_t>(7));
        intensityBins[bin]++;
    }
    
    // Build feature vector
    pImpl->features.reserve(10 + intensityBins.size());
    
    // Basic statistics
    pImpl->features.push_back(mean);  // Average intensity
    pImpl->features.push_back(edgeFeatures / inputData.size());  // Edge density
    pImpl->features.push_back(static_cast<float>(inputData.size()) / (width * height));  // Fill ratio
    
    // Add intensity distribution features
    for (size_t bin = 0; bin < intensityBins.size(); ++bin) {
        pImpl->features.push_back(static_cast<float>(intensityBins[bin]) / inputData.size());
    }
    
    pImpl->featureDimensions = pImpl->features.size();
    
    NLM_LOG_INFO("VisionProcessor: Extracted " + std::to_string(pImpl->features.size()) + " features");
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
