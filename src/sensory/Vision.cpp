#include "Vision.hpp"
#include <cmath>
#include <algorithm>
#include <random>

namespace nlm {

struct VisionProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    
    // Biological encoding parameters
    float responseGain;
    float saturationLevel;
    float adaptationTimeConstant;
    float baselineNoise;
    float nonlinearExponent;
    
    Impl() : featureDimensions(0), 
             responseGain(1.0f), 
             saturationLevel(1.0f),
             adaptationTimeConstant(0.1f),
             baselineNoise(0.01f),
             nonlinearExponent(1.5f) {}
};

VisionProcessor::VisionProcessor() : pImpl(std::make_unique<Impl>()) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    // Implement sophisticated neural encoding based on biological principles
    
    const std::vector<float>& rawData = input.getData();
    size_t inputSize = input.getDimensions();
    
    // Step 1: Apply biologically plausible transduction function
    // Modeled after retinal ganglion cell responses: sigmoidal, with adaptation
    std::vector<float> transduced(inputSize);
    float adaptation = 1.0f;  // Dynamic range adaptation
    
    for (size_t i = 0; i < inputSize; ++i) {
        // Normalize input to [0, 1]
        float normalized = std::max(0.0f, std::min(1.0f, rawData[i]));
        
        // Apply nonlinear transduction (like photoreceptor response)
        // Sigmoid-like function with adjustable exponent for different receptor types
        float transducedValue = pImpl->responseGain * 
                               std::pow(normalized, pImpl->nonlinearExponent) /
                               (1.0f + std::pow(normalized, pImpl->nonlinearExponent));
        
        // Add baseline neural noise (like spontaneous firing)
        float noise = (static_cast<float>(rand()) / RAND_MAX - 0.5f) * pImpl->baselineNoise;
        transducedValue += noise;
        
        // Apply contrast adaptation (dynamic range compression)
        adaptation *= (1.0f - pImpl->adaptationTimeConstant);
        adaptation += pImpl->adaptationTimeConstant * transducedValue;
        
        // Apply adaptation gain control
        transduced[i] = transducedValue * adaptation;
        
        // Apply saturation at maximum firing rate
        transduced[i] = std::min(transduced[i], pImpl->saturationLevel);
    }
    
    // Step 2: Extract population features
    pImpl->features.clear();
    
    // Feature 1: Global contrast (average activation across the visual field)
    float globalContrast = 0.0f;
    for (float v : transduced) globalContrast += std::abs(v);
    globalContrast /= inputSize;
    pImpl->features.push_back(globalContrast);
    
    // Feature 2: Center-surround energy (simplified spatial filtering)
    // This simulates simple cell responses in V1
    float centerSurroundEnergy = 0.0f;
    // Simplified center-surround: center pixels weighted positively, surround negatively
    for (size_t i = 0; i < std::min<size_t>(9, inputSize); ++i) {
        // Center pixel (assuming i=0,4,8 are center in 3x3)
        if (i == 0 || i == 4 || i == 8) {
            centerSurroundEnergy += transduced[i];
        } else {
            centerSurroundEnergy -= transduced[i] * 0.5f; // Surround suppression
        }
    }
    pImpl->features.push_back(centerSurroundEnergy);
    
    // Feature 3: Temporal derivative (change detection)
    // This simulates motion detection in biological vision
    float temporalDerivative = 0.0f;
    if (pImpl->featureDimensions > 0) {
        // Store last frame features for comparison
        static std::vector<float> lastFrame;
        if (lastFrame.empty()) {
            lastFrame = transduced;
        } else {
            for (size_t i = 0; i < std::min(lastFrame.size(), transduced.size()); ++i) {
                temporalDerivative += std::abs(transduced[i] - lastFrame[i]);
            }
            temporalDerivative /= inputSize;
            lastFrame = transduced;
        }
    }
    pImpl->features.push_back(temporalDerivative);
    
    // Feature 4: Spatial frequency content (edge detection)
    // This simulates complex cell responses
    float edgeEnergy = 0.0f;
    for (size_t i = 0; i + 2 < inputSize; i += 3) {
        // Check for edges using simple difference detection
        float edgeStrength = std::abs(transduced[i] - transduced[i+2]);
        edgeEnergy += edgeStrength * edgeStrength;
    }
    pImpl->features.push_back(std::sqrt(edgeEnergy));
    
    // Feature 5: Local variance (texture complexity)
    float localVariance = 0.0f;
    float localMean = 0.0f;
    for (size_t i = 0; i < std::min<size_t>(16, inputSize); ++i) {
        localMean += transduced[i];
    }
    localMean /= std::min<size_t>(16, inputSize);
    
    for (size_t i = 0; i < std::min<size_t>(16, inputSize); ++i) {
        localVariance += (transduced[i] - localMean) * (transduced[i] - localMean);
    }
    localVariance /= std::min<size_t>(16, inputSize);
    pImpl->features.push_back(localVariance);
    
    pImpl->featureDimensions = pImpl->features.size();
    
    // NLM_LOG_DEBUG("Vision processor: extracted " + std::to_string(pImpl->featureDimensions) +
    //               " neural features from " + std::to_string(inputSize) + " input pixels");
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
