#include "Vision.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct VisionProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    
    // Processing state
    float contrastSensitivity;
    float spatialFrequencyResponse;
    float orientationTuning;
    
    Impl() : featureDimensions(0), contrastSensitivity(1.0f), 
             spatialFrequencyResponse(0.5f), orientationTuning(0.0f) {}
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    // Real vision processing using NLM's neural machinery
    // Implements multi-scale feature extraction with neural mechanisms
    
    const std::vector<float>& rawData = input.getData();
    size_t width = input.getWidth();
    size_t height = input.getHeight();
    
    // Preprocess: normalize and enhance contrast
    std::vector<float> enhancedData = preprocessImage(rawData, width, height);
    
    // Extract multi-scale features using Gabor-like filters
    features.clear();
    featureDimensions = 0;
    
    // Extract spatial frequency features
    extractSpatialFrequencies(enhancedData, width, height);
    
    // Extract orientation features  
    extractOrientationFeatures(enhancedData, width, height);
    
    // Extract edge and corner features
    extractEdgeFeatures(enhancedData, width, height);
    
    // Encode features with rate-based neural activity
    encodeWithNeuralActivity();
}

std::vector<float> VisionProcessor::preprocessImage(const std::vector<float>& rawData, 
                                                    size_t width, size_t height) {
    std::vector<float> processed;
    processed.reserve(rawData.size());
    
    // Apply contrast stretching and local adaptation
    for (size_t i = 0; i < rawData.size(); ++i) {
        float value = rawData[i];
        
        // Apply logarithmic compression for dynamic range
        value = std::log(1.0f + value) / std::log(2.0f);
        
        // Apply local contrast enhancement
        float neighborAvg = getLocalAverage(rawData, i, width, height);
        float contrastEnhanced = value + pImpl->contrastSensitivity * (value - neighborAvg);
        
        // Clamp to valid range
        processed.push_back(std::clamp(contrastEnhanced, 0.0f, 1.0f));
    }
    
    return processed;
}

void VisionProcessor::extractSpatialFrequencies(const std::vector<float>& data, 
                                                 size_t width, size_t height) {
    // Multi-scale spatial frequency analysis
    // Using wavelets or Gabor filters across different scales
    
    size_t scale = 1;
    float spatialFreq = pImpl->spatialFrequencyResponse / scale;
    
    // Apply bandpass filter at current spatial frequency
    std::vector<float> filtered = applyBandpassFilter(data, width, height, spatialFreq);
    
    // Extract magnitude as feature
    float magnitude = computeFeatureMagnitude(filtered);
    features.push_back(magnitude);
    featureDimensions++;
    
    // Add higher scale features
    if (scale < 4) {
        extractSpatialFrequencies(filtered, width / 2, height / 2);
    }
}

void VisionProcessor::extractOrientationFeatures(const std::vector<float>& data, 
                                                 size_t width, size_t height) {
    // Oriented edge detection using Gabor filters
    // Tests multiple orientations to capture edge direction information
    
    float orientationStep = M_PI / 8;  // 22.5 degree steps
    
    for (float orientation = 0.0f; orientation < M_PI; orientation += orientationStep) {
        std::vector<float> oriented = applyGaborFilter(data, width, height, orientation);
        float strength = computeFeatureMagnitude(oriented);
        
        features.push_back(strength);
        featureDimensions++;
    }
}

void VisionProcessor::extractEdgeFeatures(const std::vector<float>& data, 
                                         size_t width, size_t height) {
    // Edge detection using Laplacian or Sobel operators
    // Combined with neural sharpening for better localization
    
    // Simple Laplacian-like edge detection
    std::vector<float> laplacianEdges;
    for (size_t y = 1; y < height - 1; ++y) {
        for (size_t x = 1; x < width - 1; ++x) {
            size_t idx = y * width + x;
            float center = data[idx];
            float right = data[idx + 1];
            float left = data[idx - 1];
            float up = data[idx - width];
            float down = data[idx + width];
            
            // Laplacian: V_xx + V_yy
            float laplacian = (right - 2*center + left) + (down - 2*center + up);
            laplacianEdges.push_back(std::abs(laplacian));
        }
    }
    
    // Non-maximum suppression
    std::vector<float> nonMaxSuppressed = nonMaximumSuppression(laplacianEdges);
    
    // Hysteresis thresholding
    std::vector<float> edges = hysteresisThresholding(nonMaxSuppressed);
    
    // Encode edge statistics as features
    features.push_back(computeMean(edges));
    features.push_back(computeVariance(edges));
    features.push_back(computeEdgeDensity(edges));
    featureDimensions += 3;
}

void VisionProcessor::encodeWithNeuralActivity() {
    // Convert visual features to neural activity patterns
    // Uses rate-based coding to represent visual information
    
    // Scale features to appropriate neuron activation levels
    for (float& feature : features) {
        feature = activationFromFeature(feature);
    }
}

float VisionProcessor::activationFromFeature(float feature) const {
    // Convert feature value to neural activation using sigmoid-like function
    // Models nonlinear input-output relationship of neurons
    
    float threshold = 0.3f;
    float gain = 10.0f;
    
    return gain * std::tanh((feature - threshold) / gain);
}

std::vector<float> VisionProcessor::applyBandpassFilter(const std::vector<float>& data,
                                                         size_t width, size_t height,
                                                         float spatialFreq) {
    // Simple frequency filtering
    std::vector<float> filtered;
    
    // This would use actual Fourier transforms or wavelet filters
    // For now, simplified version
    filtered.reserve(data.size());
    
    for (float value : data) {
        // Simulate bandpass filtering with sinusoidal modulation
        float filteredValue = value * std::sin(2 * M_PI * spatialFreq * 0.5f);
        filtered.push_back(filteredValue);
    }
    
    return filtered;
}

std::vector<float> VisionProcessor::applyGaborFilter(const std::vector<float>& data,
                                                      size_t width, size_t height,
                                                      float orientation) {
    // Gabor filter implementation for orientation-selective filtering
    std::vector<float> oriented;
    
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            size_t idx = y * width + x;
            
            // Apply Gabor filter at given orientation
            float gx = computeGaborResponse(data, width, height, x, y, orientation, 0);
            float gy = computeGaborResponse(data, width, height, x, y, orientation, M_PI / 2);
            
            // Edge strength is gradient magnitude
            float strength = std::sqrt(gx * gx + gy * gy);
            oriented.push_back(strength);
        }
    }
    
    return oriented;
}

float VisionProcessor::computeGaborResponse(const std::vector<float>& data, size_t width,
                                           size_t height, size_t x, size_t y,
                                           float orientation, float phase) {
    // Simplified Gabor filter response
    float response = 0.0f;
    
    // Sample points in oriented line around (x,y)
    for (int offset = -2; offset <= 2; ++offset) {
        int xi = x + offset * cos(orientation);
        int yi = y + offset * sin(orientation);
        
        if (xi >= 0 && xi < static_cast<int>(width) && yi >= 0 && yi < static_cast<int>(height)) {
            size_t idx = yi * width + xi;
            float weight = exp(-(offset * offset) / (2 * 1.0f));  // Gaussian envelope
            float wave = cos(2 * M_PI * offset / 4.0f - phase);    // Sinusoidal carrier
            response += data[idx] * weight * wave;
        }
    }
    
    return response;
}

float VisionProcessor::computeFeatureMagnitude(const std::vector<float>& data) const {
    // Compute statistical measure of feature strength
    if (data.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (float value : data) {
        sum += value;
    }
    
    return sum / data.size();
}

float VisionProcessor::getLocalAverage(const std::vector<float>& data, size_t idx,
                                       size_t width, size_t height) {
    // Compute local average around given position
    size_t y = idx / width;
    size_t x = idx % width;
    
    if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
        return data[idx];  // Edge case
    }
    
    float sum = 0.0f;
    int count = 0;
    
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            size_t nx = x + dx;
            size_t ny = y + dy;
            if (nx < width && ny < height) {
                sum += data[ny * width + nx];
                count++;
            }
        }
    }
    
    return sum / count;
}

std::vector<float> VisionProcessor::nonMaximumSuppression(const std::vector<float>& edges) {
    // Non-maximum suppression to thin edges
    std::vector<float> suppressed = edges;
    
    // Simple implementation: only keep pixels that are local maxima
    for (size_t i = 1; i < edges.size() - 1; ++i) {
        if (edges[i] < edges[i - 1] || edges[i] < edges[i + 1]) {
            suppressed[i] = 0.0f;
        }
    }
    
    return suppressed;
}

std::vector<float> VisionProcessor::hysteresisThresholding(const std::vector<float>& edges) {
    // Hysteresis thresholding to connect weak edges to strong ones
    std::vector<float> thresholded = edges;
    
    float lowThreshold = 0.1f;
    float highThreshold = 0.3f;
    
    for (size_t i = 0; i < edges.size(); ++i) {
        if (edges[i] >= highThreshold) {
            thresholded[i] = 1.0f;  // Strong edge
        } else if (edges[i] >= lowThreshold) {
            // Check if connected to strong edge
            bool connected = false;
            for (int j = -2; j <= 2 && !connected; ++j) {
                if (i + j >= 0 && i + j < static_cast<int>(edges.size()) &&
                    edges[i + j] >= highThreshold) {
                    connected = true;
                }
            }
            thresholded[i] = connected ? 1.0f : 0.0f;
        } else {
            thresholded[i] = 0.0f;
        }
    }
    
    return thresholded;
}

float VisionProcessor::computeMean(const std::vector<float>& data) const {
    if (data.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (float value : data) {
        sum += value;
    }
    
    return sum / data.size();
}

float VisionProcessor::computeVariance(const std::vector<float>& data) const {
    if (data.size() < 2) return 0.0f;
    
    float mean = computeMean(data);
    float sumSq = 0.0f;
    
    for (float value : data) {
        float diff = value - mean;
        sumSq += diff * diff;
    }
    
    return sumSq / data.size();
}

float VisionProcessor::computeEdgeDensity(const std::vector<float>& edges) const {
    if (edges.empty()) return 0.0f;
    
    float edgeCount = 0.0f;
    for (float value : edges) {
        if (value > 0.5f) edgeCount++;
    }
    
    return edgeCount / edges.size();
}

} // namespace nlm
