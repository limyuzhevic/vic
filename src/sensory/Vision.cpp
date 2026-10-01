#include "Vision.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <numeric>

namespace nlm {

struct VisionProcessor::Impl {
    // Feature extraction layers
    std::vector<float> rawFeatures;
    std::vector<float> edgeFeatures;
    std::vector<float> textureFeatures;
    std::vector<float> orientationFeatures;
    std::vector<float> motionFeatures;
    
    // Retinal processing
    std::vector<float> retinaOutput;
    std::vector<float> opticNerveSignals;
    
    // Processing parameters
    size_t retinaResolutionX;
    size_t retinaResolutionY;
    size_t featureLayers;
    float edgeSensitivity;
    float textureSensitivity;
    float noiseLevel;
    
    // Neural processing
    std::vector<float> neuralFilters[4];  // Gabor, Laplacian, Sobel, motion
    std::vector<float> receptiveField[4]; // Center-surround, etc.
    
    // Optic nerve simulation
    float transmissionDelay;  // ms
    float signalAmplitude;   // Arbitrary units
    float signalNoise;       // Gaussian noise
    
    Impl() : 
        retinaResolutionX(84),  // Human equivalent foveal resolution
        retinaResolutionY(84),
        featureLayers(8),
        edgeSensitivity(0.8f),
        textureSensitivity(0.6f),
        noiseLevel(0.01f),
        transmissionDelay(5.0f),  // ms delay from retina to LGN
        signalAmplitude(1.0f),
        signalNoise(0.05f) {}
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    const auto& data = input.getData();
    size_t width = input.getWidth();
    size_t height = input.getHeight();
    size_t channels = input.getChannels();
    
    // Initialize features
    pImpl->rawFeatures.clear();
    pImpl->edgeFeatures.clear();
    pImpl->textureFeatures.clear();
    pImpl->orientationFeatures.clear();
    pImpl->motionFeatures.clear();
    pImpl->retinaOutput.clear();
    pImpl->opticNerveSignals.clear();
    
    // Reshape input to 2D if needed
    std::vector<std::vector<float>> image2D;
    if (channels == 1 || data.empty()) {
        // Grayscale image
        image2D.resize(height, std::vector<float>(width));
        for (size_t i = 0; i < height * width; ++i) {
            image2D[i / width][i % width] = data[i];
        }
    } else {
        // RGB image - convert to grayscale using luminance formula
        image2D.resize(height, std::vector<float>(width));
        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                size_t idx = (y * width + x) * channels;
                if (channels >= 3) {
                    // Standard luminance: 0.299*R + 0.587*G + 0.114*B
                    image2D[y][x] = 0.299f * data[idx] + 
                                    0.587f * data[idx + 1] + 
                                    0.114f * data[idx + 2];
                } else {
                    image2D[y][x] = data[idx];
                }
            }
        }
    }
    
    // Step 1: Retinal preprocessing (retina simulation)
    processRetinal(input, image2D);
    
    // Step 2: Edge detection (Sobel operator)
    detectEdges(image2D, pImpl->edgeFeatures);
    
    // Step 3: Texture and orientation analysis (Gabor filters)
    extractFeatures(image2D, pImpl->orientationFeatures, 
                   pImpl->textureFeatures);
    
    // Step 4: Motion feature extraction
    detectMotion(image2D, pImpl->motionFeatures);
    
    // Step 5: Neural integration and normalization
    integrateFeatures(pImpl->retinaOutput);
    
    // Step 6: Optic nerve transmission simulation
    transmitOpticNerve(pImpl->opticNerveSignals);
    
    // Combine all features into final feature vector
    combineFeatures();
}

void VisionProcessor::processRetinal(const Vision& input, 
                                   const std::vector<std::vector<float>>& image) {
    size_t width = input.getWidth();
    size_t height = input.getHeight();
    
    pImpl->retinaOutput.resize(height * width, 0.0f);
    
    // Retinal center-surround receptive fields
    for (size_t y = 1; y < height - 1; ++y) {
        for (size_t x = 1; x < width - 1; ++x) {
            float center = image[y][x];
            
            // Center-surround filter ( Difference of Gaussians )
            float surrounding = 0.0f;
            float weightSum = 0.0f;
            
            for (int dy = -2; dy <= 2; ++dy) {
                for (int dx = -2; dx <= 2; ++dx) {
                    if (dy == 0 && dx == 0) continue;
                    
                    int ny = static_cast<int>(y) + dy;
                    int nx = static_cast<int>(x) + dx;
                    
                    if (ny >= 0 && ny < static_cast<int>(height) && 
                        nx >= 0 && nx < static_cast<int>(width)) {
                        float weight = std::exp(-(dx*dx + dy*dy) / 4.0f);
                        surrounding += image[ny][nx] * weight;
                        weightSum += weight;
                    }
                }
            }
            
            if (weightSum > 0.0f) {
                surrounding /= weightSum;
            }
            
            // Center-surround output
            pImpl->retinaOutput[y * width + x] = center - surrounding;
            
            // Add retinal noise (photon noise, neural noise)
            float noise = ((rand() % 2000 - 1000) / 1000000.0f) * pImpl->noiseLevel;
            pImpl->retinaOutput[y * width + x] += noise;
            
            // Apply retina gain control (automatic contrast normalization)
            pImpl->retinaOutput[y * width + x] *= (1.0f + std::abs(pImpl->retinaOutput[y * width + x]));
        }
    }
}

void VisionProcessor::detectEdges(const std::vector<std::vector<float>>& image, 
                                 std::vector<float>& edges) {
    size_t width = image[0].size();
    size_t height = image.size();
    
    edges.resize(height * width, 0.0f);
    
    // Sobel edge detection operators
    const std::vector<std::vector<float>> sobelX = {
        {-1, 0, 1},
        {-2, 0, 2},
        {-1, 0, 1}
    };  
    
    const std::vector<std::vector<float>> sobelY = {
        {-1, -2, -1},
        { 0,  0,  0},
        { 1,  2,  1}
    };
    
    for (size_t y = 1; y < height - 1; ++y) {
        for (size_t x = 1; x < width - 1; ++x) {
            float gx = 0.0f, gy = 0.0f;
            
            // Compute gradient using Sobel operators
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    int ny = static_cast<int>(y) + dy;
                    int nx = static_cast<int>(x) + dx;
                    
                    gx += image[ny][nx] * sobelX[dy + 1][dx + 1];
                    gy += image[ny][nx] * sobelY[dy + 1][dx + 1];
                }
            }
            
            // Gradient magnitude
            float magnitude = std::sqrt(gx * gx + gy * gy);
            
            // Non-maximum suppression
            if (magnitude > 0.1f) {
                // Normalize to [0, 1]
                magnitude = std::min(magnitude / 4.0f, 1.0f);
                
                // Add edge response with sensitivity
                edges[y * width + x] = magnitude * pImpl->edgeSensitivity;
                
                // Add to orientation features
                float orientation = std::atan2(gy, gx);
                pImpl->orientationFeatures[y * width + x] += std::cos(2 * orientation);
            }
        }
    }
}

void VisionProcessor::extractFeatures(const std::vector<std::vector<float>>& image,
                                     std::vector<float>& orientation, 
                                     std::vector<float>& texture) {
    size_t width = image[0].size();
    size_t height = image.size();
    
    orientation.resize(height * width, 0.0f);
    texture.resize(height * width, 0.0f);
    
    // Gabor filter bank for multi-orientation feature extraction
    const float sigma = 2.0f;  // Filter width
    const float gamma = 1.0f;  // Spatial aspect ratio
    
    // Multiple orientations
    const std::vector<float> orientations = {0.0f, 0.5f, 1.0f, 1.5f, 2.0f, 2.5f, 3.0f, 3.5f};  // radians
    
    for (float theta : orientations) {
        for (size_t y = 1; y < height - 1; ++y) {
            for (size_t x = 1; x < width - 1; ++x) {
                float gx = 0.0f, gy = 0.0f;
                
                for (int dy = -2; dy <= 2; ++dy) {
                    for (int dx = -2; dx <= 2; ++dx) {
                        int ny = static_cast<int>(y) + dy;
                        int nx = static_cast<int>(x) + dx;
                        
                        // Gabor filter response
                        float x_theta = nx * std::cos(theta) + ny * std::sin(theta);
                        float y_theta = -nx * std::sin(theta) + ny * std::cos(theta);
                        
                        float gaussian = std::exp(-(x_theta*x_theta + y_theta*y_theta) / (2 * sigma * sigma));
                        float sinusoidal = std::cos(2 * M_PI * x_theta / (gamma * sigma));
                        
                        float response = gaussian * sinusoidal * image[ny][nx];
                        
                        gx += response * std::cos(theta);
                        gy += response * std::sin(theta);
                    }
                }
                
                float magnitude = std::sqrt(gx * gx + gy * gy);
                magnitude = std::min(magnitude / 10.0f, 1.0f);
                
                // Texture feature accumulation
                texture[y * width + x] += magnitude;
                
                // Orientation feature
                float orient = std::atan2(gy, gx);
                orientation[y * width + x] += std::cos(orient);
            }
        }
    }
    
    // Normalize texture features
    float maxTex = *std::max_element(texture.begin(), texture.end());
    if (maxTex > 0.0f) {
        for (size_t i = 0; i < texture.size(); ++i) {
            texture[i] /= maxTex;
        }
    }
}

void VisionProcessor::detectMotion(const std::vector<std::vector<float>>& image, 
                                   std::vector<float>& motion) {
    size_t width = image[0].size();
    size_t height = image.size();
    
    motion.resize(height * width, 0.0f);
    
    // Simple motion detection using temporal derivatives
    // In a real implementation, this would compare with previous frame
    // For now, we'll use image gradient as proxy for motion cues
    
    // Motion energy model
    for (size_t y = 1; y < height - 1; ++y) {
        for (size_t x = 1; x < width - 1; ++x) {
            float gx = image[y][x + 1] - image[y][x - 1];
            float gy = image[y + 1][x] - image[y - 1][x];
            
            // Motion energy (gradient magnitude squared)
            float motionEnergy = gx * gx + gy * gy;
            motion[y * width + x] = std::min(motionEnergy * 100.0f, 1.0f) * pImpl->textureSensitivity;
        }
    }
}

void VisionProcessor::integrateFeatures(std::vector<float>& output) {
    size_t width = pImpl->retinaOutput.size() > 0 ? 
                   static_cast<size_t>(std::sqrt(pImpl->retinaOutput.size())) : 0;
    size_t height = width;
    
    output.resize(height * width, 0.0f);
    
    // Multi-scale feature integration
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            size_t idx = y * width + x;
            
            // Weighted combination of different feature streams
            float integrated = 0.0f;
            
            // Retina output (low-level features)
            if (idx < pImpl->retinaOutput.size()) {
                integrated += pImpl->retinaOutput[idx] * 0.3f;
            }
            
            // Edge features (mid-level features)
            if (idx < pImpl->edgeFeatures.size()) {
                integrated += pImpl->edgeFeatures[idx] * 0.25f;
            }
            
            // Orientation features (orientation selectivity)
            if (idx < pImpl->orientationFeatures.size()) {
                integrated += pImpl->orientationFeatures[idx] * 0.2f;
            }
            
            // Texture features (surface properties)
            if (idx < pImpl->textureFeatures.size()) {
                integrated += pImpl->textureFeatures[idx] * 0.15f;
            }
            
            // Motion features (temporal dynamics)
            if (idx < pImpl->motionFeatures.size()) {
                integrated += pImpl->motionFeatures[idx] * 0.1f;
            }
            
            // Normalize and apply nonlinearities
            output[idx] = std::tanh(integrated);
            output[idx] = output[idx] * output[idx];  // Rectify
        }
    }
}

void VisionProcessor::transmitOpticNerve(std::vector<float>& signals) {
    size_t size = pImpl->retinaOutput.size();
    signals.resize(size, 0.0f);
    
    // Optic nerve transmission simulation
    // 1. Latency and conduction velocity
    // 2. Signal amplification
    // 3. Noise addition (neural noise)
    
    for (size_t i = 0; i < size; ++i) {
        // Transmission delay modeled as a simple conduction velocity
        // With some random variation
        float delayFactor = 1.0f / (1.0f + pImpl->transmissionDelay / 10.0f);
        
        // Signal amplification (galvanic receptors)
        float amplifiedSignal = pImpl->retinaOutput[i] * pImpl->signalAmplitude;
        
        // Neural noise (Gaussian)
        float noise = ((rand() % 2000 - 1000) / 1000000.0f) * pImpl->signalNoise;
        
        // Final signal with noise and transmission effects
        signals[i] = amplifiedSignal * delayFactor + noise;
        
        // Clip to valid range
        signals[i] = std::clamp(signals[i], -1.0f, 1.0f);
    }
}

void VisionProcessor::combineFeatures() {
    // Combine all feature streams into a unified feature vector
    size_t maxSize = 0;
    maxSize = std::max(maxSize, pImpl->retinaOutput.size());
    maxSize = std::max(maxSize, pImpl->edgeFeatures.size());
    maxSize = std::max(maxSize, pImpl->orientationFeatures.size());
    maxSize = std::max(maxSize, pImpl->textureFeatures.size());
    maxSize = std::max(maxSize, pImpl->motionFeatures.size());
    maxSize = std::max(maxSize, pImpl->opticNerveSignals.size());
    
    pImpl->features.clear();
    pImpl->features.reserve(maxSize);
    
    // Create compact feature representation
    for (size_t i = 0; i < maxSize; ++i) {
        float feature = 0.0f;
        float weightSum = 0.0f;
        
        // Aggregate features from different streams
        if (i < pImpl->retinaOutput.size()) {
            feature += pImpl->retinaOutput[i];
            weightSum += 1.0f;
        }
        
        if (i < pImpl->edgeFeatures.size()) {
            feature += pImpl->edgeFeatures[i] * 0.5f;
            weightSum += 0.5f;
        }
        
        if (i < pImpl->orientationFeatures.size()) {
            feature += pImpl->orientationFeatures[i] * 0.3f;
            weightSum += 0.3f;
        }
        
        if (i < pImpl->textureFeatures.size()) {
            feature += pImpl->textureFeatures[i] * 0.4f;
            weightSum += 0.4f;
        }
        
        if (i < pImpl->motionFeatures.size()) {
            feature += pImpl->motionFeatures[i] * 0.2f;
            weightSum += 0.2f;
        }
        
        if (i < pImpl->opticNerveSignals.size()) {
            feature += pImpl->opticNerveSignals[i] * 0.6f;
            weightSum += 0.6f;
        }
        
        if (weightSum > 0.0f) {
            feature /= weightSum;
        }
        
        pImpl->features.push_back(feature);
    }
    
    pImpl->featureDimensions = pImpl->features.size() > 0 ? 1 : 0;
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
