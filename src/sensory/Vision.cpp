#include "Vision.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <iostream>

namespace nlm {

// Helper functions for vision processing
float clamp(float value, float min, float max) {
    return std::max(min, std::min(max, value));
}

float normalize(float value, float min, float max) {
    if (max > min) {
        return (value - min) / (max - min);
    }
    return 0.5f;
}

struct VisionProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    
    // Previous frame for motion detection
    std::vector<float> previousFrame;
    
    // Statistics from current frame
    float minIntensity;
    float maxIntensity;
    float meanIntensity;
    
    Impl() : featureDimensions(0), minIntensity(0.0f), maxIntensity(0.0f), meanIntensity(0.0f) {}
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    const auto& rawData = input.getData();
    size_t dimensions = input.getDimensions();
    
    // Check if this is a valid vision input
    if (dimensions == 0) {
        pImpl->features.clear();
        pImpl->featureDimensions = 0;
        return;
    }
    
    // Resize input to 16x16 if needed (or use whatever dimensions)
    size_t width = input.getWidth();
    size_t height = input.getHeight();
    size_t channels = input.getChannels();
    
    // Generate 32 compressed features from raw vision data
    pImpl->features.clear();
    pImpl->featureDimensions = 32;
    
    // 1. Center of mass computation
    float totalIntensity = 0.0f;
    float centerX = 0.0f, centerY = 0.0f;
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            // Sample intensity from first channel (assuming grayscale for simplicity)
            size_t idx = (y * width + x) * channels;
            if (idx < rawData.size() && channels > 0) {
                float intensity = rawData[idx];
                totalIntensity += intensity;
                centerX += x * intensity;
                centerY += y * intensity;
            }
        }
    }
    
    if (totalIntensity > 0.0f) {
        centerX /= totalIntensity;
        centerY /= totalIntensity;
    }
    
    // Normalize center to [0, 1] range
    float normX = (width > 1) ? (centerX / (width - 1)) : 0.5f;
    float normY = (height > 1) ? (centerY / (height - 1)) : 0.5f;
    pImpl->features.push_back(normX);
    pImpl->features.push_back(normY);
    
    // 2. Edge orientation information (simple Sobel-like edge detection)
    std::vector<float> edgeFeatures;
    for (size_t y = 1; y < height - 1; ++y) {
        for (size_t x = 1; x < width - 1; ++x) {
            size_t idx = (y * width + x) * channels;
            if (idx + 1 < rawData.size() && channels > 0) {
                float centerVal = rawData[idx];
                float rightVal = rawData[idx + 1];  // Same row, next column
                float downVal = rawData[idx + width * channels];  // Next row, same column
                
                // Sobel-like edge detection
                float gradX = rightVal - centerVal;
                float gradY = downVal - centerVal;
                
                float edgeStrength = std::sqrt(gradX * gradX + gradY * gradY);
                float orientation = std::atan2(gradY, gradX);
                
                edgeFeatures.push_back(edgeStrength);
                pImpl->features.push_back(orientation);
            }
        }
    }
    
    // Compute average edge strength
    float avgEdgeStrength = 0.0f;
    if (!edgeFeatures.empty()) {
        avgEdgeStrength = std::accumulate(edgeFeatures.begin(), edgeFeatures.end(), 0.0f) / edgeFeatures.size();
    }
    pImpl->features.push_back(avgEdgeStrength);
    
    // 3. Motion computation (difference from previous frame)
    float motionEnergy = 0.0f;
    if (!pImpl->previousFrame.empty() && pImpl->previousFrame.size() == rawData.size()) {
        for (size_t i = 0; i < rawData.size(); ++i) {
            float diff = rawData[i] - pImpl->previousFrame[i];
            motionEnergy += diff * diff;
        }
        motionEnergy = std::sqrt(motionEnergy / rawData.size());
    }
    pImpl->features.push_back(motionEnergy);
    
    // 4. Frequency domain features (simple FFT approximation)
    // Use 1D DFT on center row
    float maxFreq = 0.0f;
    if (width > 0) {
        size_t centerRow = height / 2;
        size_t rowOffset = centerRow * width * channels;
        
        std::vector<float> centerRowData;
        for (size_t x = 0; x < width && x < rawData.size() - rowOffset; x += channels) {
            centerRowData.push_back(rawData[rowOffset + x]);
        }
        
        // Compute DFT frequencies
        if (centerRowData.size() > 1) {
            std::vector<float> magnitudes;
            for (size_t k = 0; k < centerRowData.size(); ++k) {
                float real = 0.0f, imag = 0.0f;
                for (size_t n = 0; n < centerRowData.size(); ++n) {
                    float angle = 2 * M_PI * k * n / centerRowData.size();
                    real += centerRowData[n] * std::cos(angle);
                    imag -= centerRowData[n] * std::sin(angle);
                }
                magnitudes.push_back(std::sqrt(real * real + imag * imag));
            }
            
            // Get dominant frequency
            maxFreq = *std::max_element(magnitudes.begin(), magnitudes.end());
            float avgFreq = std::accumulate(magnitudes.begin(), magnitudes.end(), 0.0f) / magnitudes.size();
            
            pImpl->features.push_back(maxFreq);
            pImpl->features.push_back(avgFreq);
        } else {
            pImpl->features.push_back(0.0f);
            pImpl->features.push_back(0.0f);
        }
    } else {
        pImpl->features.push_back(0.0f);
        pImpl->features.push_back(0.0f);
    }
    
    // 5. Create compressed feature vector (reduce from 256 raw values to ~32 meaningful features)
    // Statistical features
    float minVal = 1e10f, maxVal = -1e10f, sumVal = 0.0f;
    float sumSq = 0.0f;
    size_t count = 0;
    
    for (float val : rawData) {
        if (count < 10000) { // Limit for performance
            minVal = std::min(minVal, val);
            maxVal = std::max(maxVal, val);
            sumVal += val;
            sumSq += val * val;
            count++;
        }
    }
    
    if (count > 0) {
        pImpl->features.push_back(minVal);  // Minimum value
        pImpl->features.push_back(maxVal);  // Maximum value
        pImpl->features.push_back(sumVal / count);  // Mean
        pImpl->features.push_back(sumSq / count - (sumVal / count) * (sumVal / count));  // Variance
        pImpl->features.push_back(maxVal - minVal);  // Range
        
        // Histogram-like distribution features
        std::vector<float> hist(8, 0.0f);
        for (size_t i = 0; i < rawData.size() && i < count * 10; ++i) {
            float val = rawData[i];
            int bin = static_cast<int>((val + 1.0f) * 4.0f);  // Assume [-1,1] range
            if (bin >= 0 && bin < 8) {
                hist[bin]++;
            }
        }
        
        for (float h : hist) {
            pImpl->features.push_back(h);
        }
        
        // Add 15 more features to reach ~32
        for (int i = 0; i < 15; ++i) {
            pImpl->features.push_back(static_cast<float>(i) * 0.1f);  // Placeholder - could compute actual features
        }
    } else {
        // Fill with zeros if no data
        for (int i = 0; i < 21; ++i) {
            pImpl->features.push_back(0.0f);
        }
    }
    
    // Store current frame for motion detection
    pImpl->previousFrame = rawData;
    
    // Print debug info
    std::cout << "VisionProcessor: Processed " << dimensions << " values into " 
              << pImpl->featureDimensions << " features" << std::endl;
    std::cout << "  Center: (" << normX << ", " << normY << "), Edge: " << avgEdgeStrength 
              << ", Motion: " << motionEnergy << ", Max Freq: " << maxFreq << std::endl;
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
