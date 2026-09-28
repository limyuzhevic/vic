#include "Vision.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct VisionProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    float preprocessingGain;
    float noiseReductionFactor;
    bool useNeuralFeatures;
    
    Impl() : featureDimensions(0), preprocessingGain(1.0f), noiseReductionFactor(0.1f), useNeuralFeatures(false) {
        features.reserve(1024); // Pre-allocate reasonable memory
    }
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    // Phase 6: Real vision processing using NLM's neural machinery
    // Extract and process features from visual input
    
    const auto& inputData = input.getData();
    pImpl->featureDimensions = input.getDimensions();
    
    if (inputData.empty()) {
        NLM_LOG_WARNING("Vision input data is empty");
        pImpl->features.clear();
        pImpl->featureDimensions = 0;
        return;
    }
    
    // Apply basic preprocessing for neural feature extraction
    pImpl->features = inputData;
    
    // Apply gain control (simulate retinal processing)
    if (pImpl->preprocessingGain != 1.0f) {
        for (auto& feature : pImpl->features) {
            feature = std::tanh(feature * pImpl->preprocessingGain);
        }
    }
    
    // Apply noise reduction
    if (pImpl->noiseReductionFactor > 0.0f) {
        std::vector<float> smoothedFeatures = pImpl->features;
        for (size_t i = 1; i < pImpl->features.size() - 1; ++i) {
            smoothedFeatures[i] = pImpl->features[i] * (1.0f - pImpl->noiseReductionFactor) +
                                smoothedFeatures[i-1] * (pImpl->noiseReductionFactor * 0.5f) +
                                smoothedFeatures[i+1] * (pImpl->noiseReductionFactor * 0.5f);
        }
        pImpl->features = std::move(smoothedFeatures);
    }
    
    // If neural features enabled, apply additional processing
    if (pImpl->useNeuralFeatures) {
        // Apply edge detection-like processing
        std::vector<float> edgeFeatures = pImpl->features;
        for (size_t i = 1; i < pImpl->features.size() - 1; ++i) {
            edgeFeatures[i] = std::abs(pImpl->features[i] - pImpl->features[i-1]);
        }
        pImpl->features = std::move(edgeFeatures);
        pImpl->featureDimensions = pImpl->features.size();
    }
    
    NLM_LOG_DEBUG("Vision processing complete: " + std::to_string(pImpl->features.size()) + 
                  " features, dimensions: " + std::to_string(pImpl->featureDimensions));
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

void VisionProcessor::setPreprocessingGain(float gain) {
    if (gain <= 0.0f) {
        NLM_LOG_WARNING("Invalid preprocessing gain: " + std::to_string(gain));
        return;
    }
    pImpl->preprocessingGain = gain;
}

void VisionProcessor::setNoiseReductionFactor(float factor) {
    if (factor < 0.0f || factor > 1.0f) {
        NLM_LOG_WARNING("Invalid noise reduction factor: " + std::to_string(factor));
        return;
    }
    pImpl->noiseReductionFactor = factor;
}

void VisionProcessor::enableNeuralFeatures(bool enable) {
    pImpl->useNeuralFeatures = enable;
}

bool VisionProcessor::getNeuralFeaturesEnabled() const {
    return pImpl->useNeuralFeatures;
}

float VisionProcessor::getPreprocessingGain() const {
    return pImpl->preprocessingGain;
}

float VisionProcessor::getNoiseReductionFactor() const {
    return pImpl->noiseReductionFactor;
}

} // namespace nlm
