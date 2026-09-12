#include "Vision.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>
#include <unordered_set>
#include "../core/Logger/Logger.hpp"
#include "../core/Random/Random.hpp"

namespace nlm {

struct VisionProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    
    // Neural network parameters
    size_t receptiveFieldSize;
    float learningRate;
    float decayRate;
    
    // Feature maps
    std::vector<std::vector<float>> featureMaps;
    
    // Receptive field kernels (simple Gabor-like filters)
    std::vector<std::vector<float>> kernels;
    
    // Neuronal activity patterns
    std::vector<float> neuronActivations;
    
    Impl() : featureDimensions(0), receptiveFieldSize(3), learningRate(0.01f), decayRate(0.95f) {
        // Initialize receptive field kernels
        initializeKernels();
        
        // Initialize feature maps
        featureMaps.resize(6);  // 6 feature maps (orientation, magnitude, etc.)
        for (auto& fm : featureMaps) {
            fm.resize(0);
        }
    }
    
    void initializeKernels() {
        // Create simple Gabor-like receptive field kernels
        // Orientation 0 (vertical)
        kernels.push_back(std::vector<float>{0, 1, 0, 0, -1, 0});
        
        // Orientation 45 degrees
        kernels.push_back(std::vector<float>{0, 0, 1, 0, 0, -1});
        
        // Orientation 90 degrees (horizontal)
        kernels.push_back(std::vector<float>{1, 0, 0, 0, -1, 0});
        
        // Orientation 135 degrees
        kernels.push_back(std::vector<float>{0, 0, -1, 0, 0, 1});
        
        // Edge detection (diagonal)
        kernels.push_back(std::vector<float>{1, 0, 0, 0, 0, -1});
        
        // Center-surround (blob detector)
        kernels.push_back(std::vector<float>{-1, -1, -1, -1, 4, -1, -1, -1, -1});
    }
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    // Real vision processing using NLM's neural machinery
    // Implements simple convolutional neural network inspired by biological vision
    
    const auto& inputData = input.getData();
    size_t width = input.getWidth();
    size_t height = input.getHeight();
    
    if (inputData.empty()) {
        NLM_LOG_WARNING("Empty vision input provided");
        return;
    }
    
    // Convert to grayscale if needed (assuming RGB input)
    std::vector<float> grayscale(width * height, 0.0f);
    if (input.getChannels() == 3 && inputData.size() >= width * height * 3) {
        for (size_t i = 0; i < width * height; ++i) {
            // Simple averaging for RGB to grayscale conversion
            grayscale[i] = (inputData[i * 3] + inputData[i * 3 + 1] + inputData[i * 3 + 2]) / 3.0f;
        }
    } else {
        grayscale = inputData;
    }
    
    // Apply neural processing
    applyNeuralProcessing(grayscale);
    
    // Extract features using neural network
    features = extractNeuralFeaturesInternal(grayscale, width, height);
    pImpl->featureDimensions = features.size();
}

void VisionProcessor::applyNeuralProcessing(const std::vector<float>& input) {
    size_t width = pImpl->featureMaps.empty() ? 0 : pImpl->featureMaps[0].size();
    if (width == 0) {
        return;
    }
    
    size_t inputSize = input.size();
    
    // Apply each receptive field kernel
    for (size_t k = 0; k < pImpl->kernels.size(); ++k) {
        const auto& kernel = pImpl->kernels[k];
        
        // Create feature map for this kernel
        std::vector<float> featureMap;
        featureMap.reserve(inputSize);
        
        // Simple convolution (matching receptive field size)
        size_t kernelHalf = kernel.size() / 2;
        
        for (size_t y = 0; y < width; ++y) {
            for (size_t x = 0; x < width; ++x) {
                float response = 0.0f;
                
                // Apply kernel centered at (x, y)
                for (size_t ky = 0; ky < kernel.size(); ++ky) {
                    for (size_t kx = 0; kx < kernel.size(); ++kx) {
                        if (ky < kernel.size() && kx < kernel.size()) {
                            size_t srcY = y + ky - kernelHalf;
                            size_t srcX = x + kx - kernelHalf;
                            
                            if (srcY < width && srcX < width && srcY * width + srcX < inputSize) {
                                float pixelValue = input[srcY * width + srcX];
                                response += pixelValue * kernel[ky * kernel.size() + kx];
                            }
                        }
                    }
                }
                
                // Apply nonlinearity (sigmoid-like activation)
                response = 1.0f / (1.0f + std::exp(-response * 5.0f));
                
                featureMap.push_back(response);
            }
        }
        
        pImpl->featureMaps[k] = std::move(featureMap);
    }
    
    // Combine feature maps into single feature vector
    std::vector<float> combinedFeatures;
    for (const auto& fm : pImpl->featureMaps) {
        combinedFeatures.insert(combinedFeatures.end(), fm.begin(), fm.end());
    }
    
    // Apply decay to old features
    for (float& feat : pImpl->neuronActivations) {
        feat *= pImpl->decayRate;
    }
    
    // Add new activations
    for (float& feat : combinedFeatures) {
        if (pImpl->neuronActivations.size() < combinedFeatures.size()) {
            pImpl->neuronActivations.push_back(feat);
        } else {
            pImpl->neuronActivations[feat] = (pImpl->neuronActivations[feat] * 0.9f) + (feat * 0.1f);
        }
    }
}

std::vector<float> VisionProcessor::extractNeuralFeatures(const Vision& input) {
    std::vector<float> grayscale(input.getWidth() * input.getHeight(), 0.0f);
    
    if (input.getChannels() == 3 && !input.getData().empty()) {
        const auto& inputData = input.getData();
        for (size_t i = 0; i < input.getWidth() * input.getHeight(); ++i) {
            grayscale[i] = (inputData[i * 3] + inputData[i * 3 + 1] + inputData[i * 3 + 2]) / 3.0f;
        }
    } else {
        grayscale = input.getData();
    }
    
    return extractNeuralFeaturesInternal(grayscale, input.getWidth(), input.getHeight());
}

std::vector<float> VisionProcessor::extractNeuralFeaturesInternal(const std::vector<float>& input, 
                                                                size_t width, size_t height) {
    std::vector<float> features;
    features.reserve(pImpl->kernels.size() * width * height);
    
    // Apply each kernel to extract features
    for (const auto& kernel : pImpl->kernels) {
        size_t kernelHalf = kernel.size() / 2;
        
        // Extract feature vector for this kernel
        std::vector<float> kernelFeatures;
        
        for (size_t y = 0; y < width; y += 2) {  // Downsample for efficiency
            for (size_t x = 0; x < width; x += 2) {
                float response = 0.0f;
                
                // Apply kernel
                for (size_t ky = 0; ky < kernel.size(); ++ky) {
                    for (size_t kx = 0; kx < kernel.size(); ++kx) {
                        if (ky < kernel.size() && kx < kernel.size()) {
                            size_t srcY = y + ky - kernelHalf;
                            size_t srcX = x + kx - kernelHalf;
                            
                            if (srcY < width && srcX < width && srcY * width + srcX < input.size()) {
                                float pixelValue = input[srcY * width + srcX];
                                response += pixelValue * kernel[ky * kernel.size() + kx];
                            }
                        }
                    }
                }
                
                // Apply nonlinear activation
                response = tanh(response);  // tanh for bounded response
                kernelFeatures.push_back(response);
            }
        }
        
        // Normalize this feature map
        float maxResponse = *std::max_element(kernelFeatures.begin(), kernelFeatures.end());
        float minResponse = *std::min_element(kernelFeatures.begin(), kernelFeatures.end());
        float range = maxResponse - minResponse;
        
        if (range > 0.0f) {
            for (float& resp : kernelFeatures) {
                resp = (resp - minResponse) / range;
            }
        }
        
        features.insert(features.end(), kernelFeatures.begin(), kernelFeatures.end());
    }
    
    // Add statistics-based features
    std::vector<float> statsFeatures;
    statsFeatures.push_back(computeMean(input));
    statsFeatures.push_back(computeVariance(input));
    statsFeatures.push_back(computeContrast(input));
    statsFeatures.push_back(computeEdgeDensity(input));
    
    features.insert(features.end(), statsFeatures.begin(), statsFeatures.end());
    
    return features;
}

void VisionProcessor::learnFeatureRepresentation(const std::vector<float>& input) {
    // Online learning of feature representations
    // Implements Hebbian learning for feature extraction
    
    // Update kernel weights based on input
    for (size_t k = 0; k < pImpl->kernels.size(); ++k) {
        auto& kernel = pImpl->kernels[k];
        
        // Simple Hebbian update: increase correlation with input
        for (size_t i = 0; i < kernel.size(); ++i) {
            // In a real implementation, this would involve spike timing
            // For now, use simple correlation
            float gradient = input[i % input.size()] * kernel[i] * pImpl->learningRate;
            kernel[i] += gradient;
            
            // Normalize kernel
            float norm = std::sqrt(std::inner_product(kernel.begin(), kernel.end(), kernel.begin(), 0.0f));
            if (norm > 0.0f) {
                for (float& val : kernel) {
                    val /= norm;
                }
            }
        }
    }
    
    pImpl->learningRate *= 0.999f;  // Decay learning rate
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

float VisionProcessor::computeMean(const std::vector<float>& data) const {
    if (data.empty()) return 0.0f;
    float sum = std::accumulate(data.begin(), data.end(), 0.0f);
    return sum / data.size();
}

float VisionProcessor::computeVariance(const std::vector<float>& data) const {
    if (data.size() < 2) return 0.0f;
    float mean = computeMean(data);
    float sumSq = 0.0f;
    for (float val : data) {
        sumSq += (val - mean) * (val - mean);
    }
    return sumSq / (data.size() - 1);
}

float VisionProcessor::computeContrast(const std::vector<float>& data) const {
    float mean = computeMean(data);
    float variance = computeVariance(data);
    return std::sqrt(variance) / (std::abs(mean) + 1e-6f);
}

float VisionProcessor::computeEdgeDensity(const std::vector<float>& data) const {
    size_t width = static_cast<size_t>(std::sqrt(data.size()));
    if (width == 0) return 0.0f;
    
    int edgeCount = 0;
    for (size_t y = 1; y < width - 1; ++y) {
        for (size_t x = 1; x < width - 1; ++x) {
            float center = data[y * width + x];
            float right = data[y * width + x + 1];
            float bottom = data[(y + 1) * width + x];
            
            if (std::abs(center - right) > 0.1f || std::abs(center - bottom) > 0.1f) {
                edgeCount++;
            }
        }
    }
    
    return static_cast<float>(edgeCount) / (width * width);
}

} // namespace nlm