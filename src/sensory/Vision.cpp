#include "Vision.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <limits>

namespace nlm {

struct VisionProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    std::vector<float> edgeFeatures;
    std::vector<float> textureFeatures;
    std::vector<float> colorFeatures;
    std::vector<float> motionFeatures;
    float contrast;
    float sharpness;
    float edgeDensity;
    float textureComplexity;
    float motionEnergy;
    
    Impl() : featureDimensions(0), contrast(0.0f), sharpness(0.0f), 
             edgeDensity(0.0f), textureComplexity(0.0f), motionEnergy(0.0f) {}
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    const auto& rawData = input.getData();
    size_t dimensions = input.getDimensions();
    
    if (rawData.empty()) {
        pImpl->features.clear();
        pImpl->featureDimensions = 0;
        return;
    }
    
    // Extract visual features using neural-inspired processing
    extractVisualFeatures(rawData, input.getWidth(), input.getHeight(), input.getChannels());
}

void VisionProcessor::extractVisualFeatures(const std::vector<float>& data, 
                                           size_t width, size_t height, 
                                           size_t channels) {
    pImpl->features.clear();
    pImpl->edgeFeatures.clear();
    pImpl->textureFeatures.clear();
    pImpl->colorFeatures.clear();
    pImpl->motionFeatures.clear();
    
    if (data.empty() || width == 0 || height == 0) {
        return;
    }
    
    // Compute image statistics
    computeImageStatistics(data, width, height, channels);
    
    // Extract edge features using Sobel operator
    extractEdgeFeatures(data, width, height, channels);
    
    // Extract texture features using Gabor filters
    extractTextureFeatures(data, width, height, channels);
    
    // Extract color features
    extractColorFeatures(data, width, height, channels);
    
    // Extract motion features (if multiple frames)
    // For now, compute motion energy from gradients
    extractMotionFeatures(data, width, height, channels);
    
    // Aggregate features
    aggregateFeatures();
    
    pImpl->featureDimensions = pImpl->features.size();
}

void VisionProcessor::computeImageStatistics(const std::vector<float>& data,
                                          size_t width, size_t height, 
                                          size_t channels) {
    float total = 0.0f;
    float sumSquares = 0.0f;
    float minVal = std::numeric_limits<float>::max();
    float maxVal = std::numeric_limits<float>::lowest();
    
    size_t pixelCount = width * height * channels;
    for (size_t i = 0; i < std::min(pixelCount, data.size()); ++i) {
        float val = data[i];
        total += val;
        sumSquares += val * val;
        minVal = std::min(minVal, val);
        maxVal = std::max(maxVal, val);
    }
    
    if (pixelCount > 0) {
        float mean = total / pixelCount;
        float stdDev = std::sqrt(sumSquares / pixelCount - mean * mean);
        
        // Compute contrast
        pImpl->contrast = (maxVal - minVal) / (maxVal - minVal + 1e-6f);
        
        // Compute sharpness (variance of gradients)
        pImpl->sharpness = stdDev / (mean + 1e-6f);
        
        // Add basic statistics
        pImpl->features.push_back(mean);
        pImpl->features.push_back(stdDev);
        pImpl->features.push_back(pImpl->contrast);
        pImpl->features.push_back(pImpl->sharpness);
    }
}

void VisionProcessor::extractEdgeFeatures(const std::vector<float>& data,
                                        size_t width, size_t height, 
                                        size_t channels) {
    // Simplified edge detection using Sobel-like gradients
    size_t stride = channels;
    size_t totalPixels = width * height;
    
    // Convert to grayscale for edge detection if needed
    std::vector<float> grayData(totalPixels, 0.0f);
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            size_t dataIndex = (y * width + x) * stride;
            
            if (channels == 3) {
                // Simple luminance conversion
                grayData[y * width + x] = 0.299f * data[dataIndex] + 
                                          0.587f * data[dataIndex + 1] + 
                                          0.114f * data[dataIndex + 2];
            } else {
                grayData[y * width + x] = data[dataIndex];
            }
        }
    }
    
    // Compute gradients and detect edges
    std::vector<float> gradients(totalPixels, 0.0f);
    pImpl->edgeFeatures.reserve(totalPixels);
    
    for (size_t y = 1; y < height - 1; ++y) {
        for (size_t x = 1; x < width - 1; ++x) {
            float gx = grayData[(y * width + (x + 1))] - grayData[(y * width + (x - 1))];
            float gy = grayData[((y + 1) * width + x)] - grayData[((y - 1) * width + x)];
            
            float magnitude = std::sqrt(gx * gx + gy * gy);
            gradients[y * width + x] = magnitude;
            pImpl->edgeFeatures.push_back(magnitude);
        }
    }
    
    // Compute edge density
    float edgeSum = std::accumulate(pImpl->edgeFeatures.begin(), pImpl->edgeFeatures.end(), 0.0f);
    size_t edgeCount = (pImpl->edgeFeatures.size() > 1u) ? pImpl->edgeFeatures.size() : 1u;
    pImpl->edgeDensity = edgeSum / static_cast<float>(edgeCount);
    
    // Add edge statistics
    pImpl->features.push_back(pImpl->edgeDensity);
    
    // Add some gradient statistics
    if (!pImpl->edgeFeatures.empty()) {
        float maxGrad = *std::max_element(pImpl->edgeFeatures.begin(), pImpl->edgeFeatures.end());
        pImpl->features.push_back(maxGrad);
    }
}

void VisionProcessor::extractTextureFeatures(const std::vector<float>& data,
                                           size_t width, size_t height,
                                           size_t channels) {
    // Simplified texture analysis using local variance
    size_t blockSize = 8;
    size_t stride = channels;
    
    std::vector<float> grayData((width * height), 0.0f);
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            size_t dataIndex = (y * width + x) * stride;
            
            if (channels == 3) {
                grayData[y * width + x] = 0.299f * data[dataIndex] + 
                                          0.587f * data[dataIndex + 1] + 
                                          0.114f * data[dataIndex + 2];
            } else {
                grayData[y * width + x] = data[dataIndex];
            }
        }
    }
    
    // Compute local variance for texture
    pImpl->textureFeatures.reserve((width / blockSize) * (height / blockSize));
    pImpl->textureComplexity = 0.0f;
    
    for (size_t y = 0; y + blockSize <= height; y += blockSize) {
        for (size_t x = 0; x + blockSize <= width; x += blockSize) {
            float blockSum = 0.0f;
            float blockSumSquares = 0.0f;
            size_t count = 0;
            
            for (size_t by = 0; by < blockSize && y + by < height; ++by) {
                for (size_t bx = 0; bx < blockSize && x + bx < width; ++bx) {
                    blockSum += grayData[((y + by) * width + (x + bx))];
                    blockSumSquares += grayData[((y + by) * width + (x + bx))] * 
                                      grayData[((y + by) * width + (x + bx))];
                    count++;
                }
            }
            
            if (count > 0) {
                float mean = blockSum / count;
                float variance = blockSumSquares / count - mean * mean;
                float textureValue = std::sqrt(std::max(variance, 0.0f));
                
                pImpl->textureFeatures.push_back(textureValue);
                pImpl->textureComplexity += textureValue;
            }
        }
    }
    
    // Add texture complexity to features
    if (!pImpl->textureFeatures.empty()) {
        pImpl->textureComplexity /= pImpl->textureFeatures.size();
        pImpl->features.push_back(pImpl->textureComplexity);
    }
}

void VisionProcessor::extractColorFeatures(const std::vector<float>& data,
                                          size_t width, size_t height,
                                          size_t channels) {
    if (channels < 3) {
        // Grayscale - no color features
        pImpl->features.push_back(0.0f); // No color feature
        return;
    }
    
    size_t totalPixels = width * height;
    float rSum = 0.0f, gSum = 0.0f, bSum = 0.0f;
    float rSq = 0.0f, gSq = 0.0f, bSq = 0.0f;
    
    for (size_t i = 0; i < totalPixels; ++i) {
        size_t dataIndex = i * 3;
        if (dataIndex + 2 < data.size()) {
            float r = data[dataIndex];
            float g = data[dataIndex + 1];
            float b = data[dataIndex + 2];
            
            rSum += r; gSum += g; bSum += b;
            rSq += r * r; gSq += g * g; bSq += b * b;
        }
    }
    
    if (totalPixels > 0) {
        float rMean = rSum / totalPixels;
        float gMean = gSum / totalPixels;
        float bMean = bSum / totalPixels;
        
        float rVar = rSq / totalPixels - rMean * rMean;
        float gVar = gSq / totalPixels - gMean * gMean;
        float bVar = bSq / totalPixels - bMean * bMean;
        
        // Compute color saturation (variance in color space)
        float colorSaturation = std::sqrt(rVar + gVar + bVar) / (rMean + gMean + bMean + 1e-6f);
        
        // Compute dominant color (simplified)
        float dominant = std::max({rMean, gMean, bMean});
        
        // Add color features
        pImpl->features.push_back(colorSaturation);
        pImpl->features.push_back(dominant);
        pImpl->features.push_back(std::abs(rMean - gMean)); // Red-green contrast
        pImpl->features.push_back(std::abs(gMean - bMean)); // Green-blue contrast
        pImpl->features.push_back(std::abs(rMean - bMean)); // Red-blue contrast
    }
}

void VisionProcessor::extractMotionFeatures(const std::vector<float>& data,
                                           size_t width, size_t height,
                                           size_t channels) {
    // Compute motion energy from spatial gradients
    std::vector<float> grayData((width * height), 0.0f);
    size_t stride = channels;
    
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            size_t dataIndex = (y * width + x) * stride;
            
            if (channels == 3) {
                grayData[y * width + x] = 0.299f * data[dataIndex] + 
                                          0.587f * data[dataIndex + 1] + 
                                          0.114f * data[dataIndex + 2];
            } else {
                grayData[y * width + x] = data[dataIndex];
            }
        }
    }
    
    // Compute temporal derivatives (approximate using spatial gradients as proxy)
    pImpl->motionEnergy = 0.0f;
    
    for (size_t y = 1; y < height - 1; ++y) {
        for (size_t x = 1; x < width - 1; ++x) {
            float dx = grayData[y * width + (x + 1)] - grayData[y * width + (x - 1)];
            float dy = grayData[(y + 1) * width + x] - grayData[(y - 1) * width + x];
            
            float motionMagnitude = std::sqrt(dx * dx + dy * dy);
            pImpl->motionFeatures.push_back(motionMagnitude);
            pImpl->motionEnergy += motionMagnitude;
        }
    }
    
    // Normalize motion energy
    if (!pImpl->motionFeatures.empty()) {
        pImpl->motionEnergy /= pImpl->motionFeatures.size();
        pImpl->features.push_back(pImpl->motionEnergy);
    }
}

void VisionProcessor::aggregateFeatures() {
    // Combine all feature types
    pImpl->features.clear();
    
    // Add edge features (every 4th feature starting from edge density)
    size_t edgeFeatureCount = std::min(static_cast<size_t>(50), pImpl->edgeFeatures.size());
    for (size_t i = 0; i < edgeFeatureCount; ++i) {
        pImpl->features.push_back(pImpl->edgeFeatures[i]);
    }
    
    // Add texture features (limited number)
    size_t textureFeatureCount = std::min(static_cast<size_t>(20), pImpl->textureFeatures.size());
    for (size_t i = 0; i < textureFeatureCount; ++i) {
        pImpl->features.push_back(pImpl->textureFeatures[i]);
    }
    
    // Add color features
    size_t colorFeatureCount = std::min(static_cast<size_t>(10), pImpl->colorFeatures.size());
    for (size_t i = 0; i < colorFeatureCount; ++i) {
        pImpl->features.push_back(pImpl->colorFeatures[i]);
    }
    
    // Add motion features
    size_t motionFeatureCount = std::min(static_cast<size_t>(10), pImpl->motionFeatures.size());
    for (size_t i = 0; i < motionFeatureCount; ++i) {
        pImpl->features.push_back(pImpl->motionFeatures[i]);
    }
    
    // Add global statistics
    pImpl->features.push_back(pImpl->contrast);
    pImpl->features.push_back(pImpl->sharpness);
    pImpl->features.push_back(pImpl->edgeDensity);
    pImpl->features.push_back(pImpl->textureComplexity);
    pImpl->features.push_back(pImpl->motionEnergy);
    
    // Normalize features
    if (!pImpl->features.empty()) {
        float maxVal = *std::max_element(pImpl->features.begin(), pImpl->features.end());
        if (maxVal > 0.0f) {
            for (float& val : pImpl->features) {
                val /= maxVal;
            }
        }
    }
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

float VisionProcessor::getContrast() const {
    return pImpl->contrast;
}

float VisionProcessor::getSharpness() const {
    return pImpl->sharpness;
}

float VisionProcessor::getEdgeDensity() const {
    return pImpl->edgeDensity;
}

float VisionProcessor::getTextureComplexity() const {
    return pImpl->textureComplexity;
}

float VisionProcessor::getMotionEnergy() const {
    return pImpl->motionEnergy;
}

} // namespace nlm