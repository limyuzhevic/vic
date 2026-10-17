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
    // Real vision processing: extract meaningful features from image data
    const std::vector<float>& rawData = input.getData();
    pImpl->featureDimensions = input.getDimensions();
    
    if (rawData.empty()) {
        pImpl->features.clear();
        return;
    }
    
    // Extract multiple feature types for richer representation
    std::vector<float> features;
    
    // 1. Mean/stddev features for overall statistics
    float sum = 0.0f, sumSq = 0.0f;
    for (float value : rawData) {
        sum += value;
        sumSq += value * value;
    }
    float mean = sum / static_cast<float>(rawData.size());
    float stddev = std::sqrt((sumSq / rawData.size()) - (mean * mean));
    features.push_back(mean);
    features.push_back(stddev);
    
    // 2. Contrast features (difference between adjacent pixels)
    float maxContrast = 0.0f;
    for (size_t i = 1; i < rawData.size(); ++i) {
        float contrast = std::abs(rawData[i] - rawData[i-1]);
        maxContrast = std::max(maxContrast, contrast);
    }
    features.push_back(maxContrast);
    
    // 3. Edge-like features (high-frequency components)
    float edgeStrength = 0.0f;
    for (size_t i = 2; i < rawData.size(); ++i) {
        float localChange = std::abs(rawData[i] - rawData[i-2]);
        edgeStrength += localChange;
    }
    features.push_back(edgeStrength / static_cast<float>(rawData.size() - 2));
    
    // 4. Texture-like features (variance in local neighborhoods)
    float textureVariation = 0.0f;
    if (rawData.size() >= 4) {
        for (size_t i = 0; i < rawData.size() - 3; i += 4) {
            float localVar = 0.0f;
            for (size_t j = 0; j < 4 && i + j < rawData.size(); ++j) {
                localVar += (rawData[i + j] - mean) * (rawData[i + j] - mean);
            }
            textureVariation += localVar / 4.0f;
        }
        textureVariation /= static_cast<float>(rawData.size() / 4);
    }
    features.push_back(textureVariation);
    
    // 5. Global contrast ratio
    float normalizedMin = 1.0f, normalizedMax = 0.0f;
    for (float value : rawData) {
        normalizedMin = std::min(normalizedMin, value);
        normalizedMax = std::max(normalizedMax, value);
    }
    float contrastRatio = (normalizedMax - normalizedMin) / (normalizedMax + normalizedMin + 1e-6f);
    features.push_back(contrastRatio);
    
    pImpl->features = features;
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
