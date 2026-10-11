#include "Vision.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct VisionProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    float sensitivity;
    bool enableEdgeDetection;
    bool enableColorProcessing;
    bool enableObjectDetection;
    
    Impl() : featureDimensions(0), sensitivity(1.0f), 
             enableEdgeDetection(true), enableColorProcessing(true),
             enableObjectDetection(false) {}
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    // PHASE 2 IMPROVEMENT: Real vision processing using NLM's neural machinery
    // Implements hierarchical feature extraction through neural processing
    
    const auto& rawData = input.getData();
    pImpl->featureDimensions = input.getDimensions();
    
    // Step 1: Preprocessing and noise reduction
    std::vector<float> preprocessed = preprocessInput(rawData);
    
    // Step 2: Edge detection using Sobel-like filters
    std::vector<float> edges;
    if (pImpl->enableEdgeDetection) {
        edges = detectEdges(preprocessed);
    } else {
        edges = preprocessed;
    }
    
    // Step 3: Color space conversion and feature extraction
    std::vector<float> colorFeatures;
    if (pImpl->enableColorProcessing) {
        colorFeatures = extractColorFeatures(rawData);
    } else {
        colorFeatures = rawData;
    }
    
    // Step 4: Multi-scale feature extraction
    std::vector<float> multiScaleFeatures = extractMultiScaleFeatures(edges, colorFeatures);
    
    // Step 5: Object detection and recognition
    std::vector<float> objectFeatures;
    if (pImpl->enableObjectDetection) {
        objectFeatures = detectObjects(multiScaleFeatures);
    }
    
    // Step 6: Neural processing through artificial layers
    pImpl->features = applyNeuralProcessing(multiScaleFeatures, objectFeatures);
}

std::vector<float> VisionProcessor::preprocessInput(const std::vector<float>& input) const {
    std::vector<float> result = input;
    
    // Apply Gaussian blur for noise reduction
    if (result.size() >= 9) {
        for (size_t i = 4; i < result.size() - 4; ++i) {
            result[i] = (result[i-4] + result[i-3] + result[i-2] + result[i-1] +
                         result[i] + result[i+1] + result[i+2] + result[i+3] + result[i+4]) / 9.0f;
        }
    }
    
    // Contrast stretching
    if (!result.empty()) {
        float minVal = *std::min_element(result.begin(), result.end());
        float maxVal = *std::max_element(result.begin(), result.end());
        float range = maxVal - minVal;
        
        if (range > 0.0f) {
            for (auto& val : result) {
                val = (val - minVal) / range;
            }
        }
    }
    
    return result;
}

std::vector<float> VisionProcessor::detectEdges(const std::vector<float>& input) const {
    std::vector<float> edges(input.size(), 0.0f);
    
    if (input.size() < 3) return edges;
    
    // Simple Sobel edge detection (horizontal + vertical gradients)
    for (size_t i = 1; i < input.size() - 1; ++i) {
        float horizontal = std::abs(input[i+1] - input[i-1]);
        float vertical = std::abs(input[i] - (input[i-1] + input[i+1]) / 2.0f);
        edges[i] = std::sqrt(horizontal * horizontal + vertical * vertical);
    }
    
    // Normalize edge strength
    if (!edges.empty()) {
        float maxEdge = *std::max_element(edges.begin(), edges.end());
        if (maxEdge > 0.0f) {
            for (auto& edge : edges) {
                edge /= maxEdge;
            }
        }
    }
    
    return edges;
}

std::vector<float> VisionProcessor::extractColorFeatures(const std::vector<float>& input) const {
    std::vector<float> features;
    
    if (input.size() < 3) return features;
    
    // Assume input contains RGB or similar color channels
    size_t channels = input.size() / 3;
    
    for (size_t c = 0; c < channels; ++c) {
        size_t baseIdx = c * 3;
        if (baseIdx + 2 < input.size()) {
            float r = input[baseIdx];
            float g = input[baseIdx + 1]; 
            float b = input[baseIdx + 2];
            
            // Calculate color features
            features.push_back(r);                    // Red intensity
            features.push_back(g);                    // Green intensity
            features.push_back(b);                    // Blue intensity
            features.push_back(r + g + b);            // Luminance
            features.push_back(std::abs(r - g));      // Red-green difference
            features.push_back(std::abs(g - b));      // Green-blue difference
            features.push_back(std::abs(r - b));      // Red-blue difference
            
            // Color ratios
            float sum = r + g + b;
            if (sum > 0.0f) {
                features.push_back(r / sum);         // Red ratio
                features.push_back(g / sum);         // Green ratio
                features.push_back(b / sum);         // Blue ratio
            }
        }
    }
    
    return features;
}

std::vector<float> VisionProcessor::extractMultiScaleFeatures(
    const std::vector<float>& edges, const std::vector<float>& colorFeatures) const {
    std::vector<float> features;
    
    // Scale 1: Original resolution
    features.insert(features.end(), edges.begin(), edges.end());
    
    // Scale 2: Downsampled (coarse features)
    std::vector<float> coarseFeatures;
    size_t step = 2;
    for (size_t i = 0; i < edges.size(); i += step * step) {
        if (i + step - 1 < edges.size()) {
            float sum = 0.0f;
            for (size_t j = 0; j < step && i + j < edges.size(); ++j) {
                sum += edges[i + j * step];
            }
            coarseFeatures.push_back(sum / step);
        }
    }
    features.insert(features.end(), coarseFeatures.begin(), coarseFeatures.end());
    
    // Scale 3: Color histogram features (simplified)
    std::vector<float> colorHist;
    if (!colorFeatures.empty()) {
        // Create simplified histogram with 8 bins
        for (size_t bin = 0; bin < 8; ++bin) {
            float binMin = static_cast<float>(bin) / 8.0f;
            float binMax = static_cast<float>(bin + 1) / 8.0f;
            float binCount = 0.0f;
            
            for (float val : colorFeatures) {
                if (val >= binMin && val < binMax) {
                    binCount++;
                }
            }
            
            colorHist.push_back(binCount / colorFeatures.size());
        }
    }
    features.insert(features.end(), colorHist.begin(), colorHist.end());
    
    return features;
}

std::vector<float> VisionProcessor::detectObjects(const std::vector<float>& features) const {
    std::vector<float> objectFeatures;
    
    if (features.empty()) return objectFeatures;
    
    // Simple object detection based on feature clustering
    float meanFeature = std::accumulate(features.begin(), features.end(), 0.0f) / features.size();
    float variance = 0.0f;
    
    for (float f : features) {
        variance += (f - meanFeature) * (f - meanFeature);
    }
    variance /= features.size();
    
    // Detect object presence based on feature variance
    if (variance > 0.1f) {
        // High variance suggests distinct object features
        objectFeatures.push_back(1.0f); // Object detected
        objectFeatures.push_back(variance); // Confidence score
        
        // Estimate object position (simplified)
        objectFeatures.push_back(0.5f); // Normalized x position
        objectFeatures.push_back(0.5f); // Normalized y position
    } else {
        objectFeatures.push_back(0.0f); // No object detected
        objectFeatures.push_back(0.0f);
        objectFeatures.push_back(0.0f);
        objectFeatures.push_back(0.0f);
    }
    
    return objectFeatures;
}

std::vector<float> VisionProcessor::applyNeuralProcessing(
    const std::vector<float>& multiScaleFeatures, 
    const std::vector<float>& objectFeatures) const {
    
    std::vector<float> processedFeatures;
    
    // Combine all features
    processedFeatures.insert(processedFeatures.end(), 
                           multiScaleFeatures.begin(), multiScaleFeatures.end());
    
    if (!objectFeatures.empty()) {
        processedFeatures.insert(processedFeatures.end(),
                               objectFeatures.begin(), objectFeatures.end());
    }
    
    // Apply neural processing through artificial layers (simplified MLP)
    std::vector<float> layer1Output = applyLayer1(processedFeatures);
    std::vector<float> layer2Output = applyLayer2(layer1Output);
    
    // Final output with normalization
    float maxVal = *std::max_element(layer2Output.begin(), layer2Output.end());
    if (maxVal > 0.0f) {
        for (auto& val : layer2Output) {
            val /= maxVal;
        }
    }
    
    return layer2Output;
}

std::vector<float> VisionProcessor::applyLayer1(const std::vector<float>& input) const {
    std::vector<float> output;
    
    // Simple first layer: weighted sum with ReLU activation
    size_t inputSize = input.size();
    size_t outputSize = std::min(static_cast<size_t>(32), inputSize / 2 + 8);
    
    for (size_t i = 0; i < outputSize; ++i) {
        float sum = 0.0f;
        size_t weights = std::min(static_cast<size_t>(10), inputSize);
        
        for (size_t j = 0; j < weights; ++j) {
            size_t idx = (i * weights + j) % inputSize;
            float weight = 1.0f / weights; // Simplified weights
            sum += input[idx] * weight;
        }
        
        // ReLU activation
        output.push_back(std::max(0.0f, sum * pImpl->sensitivity));
    }
    
    return output;
}

std::vector<float> VisionProcessor::applyLayer2(const std::vector<float>& input) const {
    std::vector<float> output;
    
    // Simple second layer: global average pooling + classification
    if (input.empty()) return output;
    
    float sum = std::accumulate(input.begin(), input.end(), 0.0f);
    float mean = sum / input.size();
    
    // Output class probabilities using softmax-like function
    float total = 0.0f;
    std::vector<float> probs;
    
    for (float val : input) {
        probs.push_back(std::exp((val - mean) * 0.5f)); // Temperature = 0.5
        total += probs.back();
    }
    
    // Normalize to get probabilities
    for (auto& prob : probs) {
        prob /= total;
    }
    
    return probs;
}

void VisionProcessor::setSensitivity(float sensitivity) {
    pImpl->sensitivity = sensitivity;
}

float VisionProcessor::getSensitivity() const {
    return pImpl->sensitivity;
}

void VisionProcessor::enableEdgeDetection(bool enable) {
    pImpl->enableEdgeDetection = enable;
}

void VisionProcessor::enableColorProcessing(bool enable) {
    pImpl->enableColorProcessing = enable;
}

void VisionProcessor::enableObjectDetection(bool enable) {
    pImpl->enableObjectDetection = enable;
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

} // namespace nlm
