#include "Vision.hpp"
#include "../core/Logger/Logger.hpp"
#include <cmath>
#include <algorithm>
#include <vector>

namespace nlm {

struct VisionProcessor::Impl {
    // Multiple scales for hierarchical processing (like biological visual hierarchy)
    struct ScaleFeature {
        std::vector<float> features;
        size_t dimensions;
        float weight;  // Importance weight
        
        ScaleFeature() : dimensions(0), weight(1.0f) {}
    };
    
    std::vector<ScaleFeature> scaleFeatures;
    size_t numScales;
    
    // Processing parameters
    bool useEdgeDetection;
    bool useOrientation;  // 4 orientations: 0°, 45°, 90°, 135°
    bool useMotion;      // Motion sensitivity
    bool useColor;       // Color opponency
    
    // Neural connectivity parameters
    float spatialScale;      // Size of receptive fields
    float temporalScale;     // Integration time window
    float connectivityRadius; // How far signals spread
    
    // Adaptive processing
    float learningRate;
    float noveltySensitivity;
    float predictionErrorGain;
    
    Impl() : numScales(3), useEdgeDetection(true), useOrientation(true),
             useMotion(true), useColor(true), spatialScale(1.0f),
             temporalScale(0.5f), connectivityRadius(2.0f),
             learningRate(0.01f), noveltySensitivity(0.5f), 
             predictionErrorGain(0.3f) {
        // Initialize features for each scale
        scaleFeatures.resize(numScales);
        for (size_t i = 0; i < numScales; ++i) {
            scaleFeatures[i].dimensions = 16;  // Number of feature maps per scale
            scaleFeatures[i].weight = 1.0f / numScales;
        }
        
        NLM_LOG_INFO("Vision processor initialized with hierarchical feature extraction");
    }
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    // Real vision processing with hierarchical feature extraction
    // Simulates early visual processing in biological systems
    
    const std::vector<float>& rawData = input.getData();
    size_t width = input.getWidth();
    size_t height = input.getHeight();
    
    // Reset features for this input
    for (auto& scale : pImpl->scaleFeatures) {
        scale.features.clear();
    }
    
    // Process at multiple scales (like V1, V2, V4 in visual hierarchy)
    for (size_t scale = 0; scale < pImpl->numScales; ++scale) {
        processScale(rawData, width, height, scale);
    }
    
    // Integrate features across scales
    integrateFeatures();
    
    // Apply novelty detection
    if (pImpl->noveltySensitivity > 0.0f) {
        detectNovelty();
    }
}

void VisionProcessor::processScale(const std::vector<float>& input, 
                                   size_t width, size_t height, 
                                   size_t scale) {
    // Extract features at a specific spatial scale
    // Scale 0: coarse (global), Scale 1: medium, Scale 2: fine (local)
    
    ScaleFeature& scaleFeature = pImpl->scaleFeatures[scale];
    scaleFeature.features.clear();
    scaleFeature.dimensions = 16;
    
    // Calculate processing parameters for this scale
    float scaleFactor = std::pow(0.5f, static_cast<float>(scale));
    size_t effectiveWidth = std::max(1UL, width * scaleFactor);
    size_t effectiveHeight = std::max(1UL, height * scaleFactor);
    
    // Extract different feature maps
    if (pImpl->useEdgeDetection) {
        extractEdges(input, width, height, effectiveWidth, effectiveHeight, scaleFeature);
    }
    
    if (pImpl->useOrientation) {
        extractOrientations(input, width, height, effectiveWidth, effectiveHeight, scaleFeature);
    }
    
    if (pImpl->useMotion) {
        extractMotion(input, width, height, effectiveWidth, effectiveHeight, scaleFeature);
    }
    
    if (pImpl->useColor) {
        extractColor(input, width, height, effectiveWidth, effectiveHeight, scaleFeature);
    }
    
    // Normalize features
    normalizeFeatures(scaleFeature);
}

void VisionProcessor::extractEdges(const std::vector<float>& input, 
                                   size_t width, size_t height,
                                   size_t effWidth, size_t effHeight,
                                   ScaleFeature& scaleFeature) {
    // Sobel edge detection with biological plausibility
    // Responds to zero-crossings in the visual input
    
    std::vector<float> edges(scaleFeature.dimensions, 0.0f);
    
    // Simple gradient calculation (simplified Sobel)
    for (size_t y = 1; y < effHeight - 1; ++y) {
        for (size_t x = 1; x < effWidth - 1; ++x) {
            size_t srcX = x * width / effWidth;
            size_t srcY = y * height / effHeight;
            
            // Calculate horizontal and vertical gradients
            float left = input[srcX - 1 + srcY * width];
            float right = input[srcX + 1 + srcY * width];
            float up = input[srcX + (srcY - 1) * width];
            float down = input[srcX + (srcY + 1) * width];
            
            float gx = (right - left) * 0.5f;
            float gy = (down - up) * 0.5f;
            
            // Edge magnitude (O(n + m) rather than O(n * m) for efficiency)
            float magnitude = std::sqrt(gx * gx + gy * gy);
            
            // Map to feature dimensions (different orientations)
            if (scaleFeature.dimensions >= 4) {
                // Divide into quadrants for different edge orientations
                size_t idx = y % (scaleFeature.dimensions / 2) * 2 + (x % 2);
                if (idx < scaleFeature.dimensions) {
                    edges[idx] += magnitude * pImpl->connectivityRadius;
                }
            }
        }
    }
    
    // Sum up feature maps
    float total = 0.0f;
    for (float edge : edges) total += edge;
    
    if (total > 0.0f) {
        for (size_t i = 0; i < scaleFeature.dimensions; ++i) {
            edges[i] /= total;
        }
    }
    
    // Add to scale feature (combine with existing)
    for (size_t i = 0; i < scaleFeature.dimensions && i < edges.size(); ++i) {
        scaleFeature.features.push_back(edges[i]);
    }
}

void VisionProcessor::extractOrientations(const std::vector<float>& input,
                                         size_t width, size_t height,
                                         size_t effWidth, size_t effHeight,
                                         ScaleFeature& scaleFeature) {
    // Orientation-selective processing (simple Gabor-like filters)
    // 4 orientations: 0°, 45°, 90°, 135°
    
    std::vector<float> orientations(4, 0.0f);  // 0°, 45°, 90°, 135°
    
    // Simple orientation detection using gradient directions
    for (size_t y = 1; y < effHeight - 1; ++y) {
        for (size_t x = 1; x < effWidth - 1; ++x) {
            size_t srcX = x * width / effWidth;
            size_t srcY = y * height / effHeight;
            
            float left = input[srcX - 1 + srcY * width];
            float right = input[srcX + 1 + srcY * width];
            float up = input[srcX + (srcY - 1) * width];
            float down = input[srcX + (srcY + 1) * width];
            
            float gx = (right - left) * 0.5f;
            float gy = (down - up) * 0.5f;
            
            // Calculate orientation
            float angle = std::atan2(gy, gx);
            if (angle < 0.0f) angle += 2.0f * M_PI;
            
            // Map angle to one of four orientations
            size_t orientIdx = 0;
            if (angle > M_PI / 4.0f && angle < 3.0f * M_PI / 4.0f) orientIdx = 1;  // 90°
            else if (angle > 3.0f * M_PI / 4.0f && angle < 5.0f * M_PI / 4.0f) orientIdx = 2;  // 135°
            else if (angle > 5.0f * M_PI / 4.0f) orientIdx = 3;  // 180° (0°)
            
            float magnitude = std::sqrt(gx * gx + gy * gy);
            orientations[orientIdx] += magnitude * pImpl->connectivityRadius;
        }
    }
    
    // Normalize orientation responses
    float total = 0.0f;
    for (float orient : orientations) total += orient;
    if (total > 0.0f) {
        for (float& orient : orientations) orient /= total;
    }
    
    // Add to scale feature
    scaleFeature.features.insert(scaleFeature.features.end(), 
                                 orientations.begin(), orientations.end());
}

void VisionProcessor::extractMotion(const std::vector<float>& input,
                                    size_t width, size_t height,
                                    size_t effWidth, size_t effHeight,
                                    ScaleFeature& scaleFeature) {
    // Motion detection using temporal derivatives (simplified)
    // Responds to changes in the visual input over time
    
    // This is a placeholder for motion detection
    // In a real implementation, this would compare current input with previous frames
    std::vector<float> motion(scaleFeature.dimensions, 0.0f);
    
    // For now, create a simple temporal derivative pattern
    // This simulates motion sensitivity
    for (size_t y = 0; y < effHeight; ++y) {
        for (size_t x = 0; x < effWidth; ++x) {
            size_t srcX = x * width / effWidth;
            size_t srcY = y * height / effHeight;
            
            float value = input[srcX + srcY * width];
            
            // Simple motion model: alternating pattern suggests motion
            bool isMotion = ((x + y) % 3 == 0) || ((x * y) % 7 == 0);
            if (isMotion) {
                size_t idx = (y * effWidth + x) % scaleFeature.dimensions;
                motion[idx] += value * 0.5f;
            }
        }
    }
    
    // Normalize motion features
    float total = 0.0f;
    for (float m : motion) total += m;
    if (total > 0.0f) {
        for (float& m : motion) m /= total;
    }
    
    // Add to scale feature
    scaleFeature.features.insert(scaleFeature.features.end(),
                                 motion.begin(), motion.end());
}

void VisionProcessor::extractColor(const std::vector<float>& input,
                                   size_t width, size_t height,
                                   size_t effWidth, size_t effHeight,
                                   ScaleFeature& scaleFeature) {
    // Color opponency processing (simple red-green, blue-yellow)
    // Biological color processing with opponent processes
    
    std::vector<float> colorFeatures(scaleFeature.dimensions, 0.0f);
    
    // Simple color decomposition (assuming input is RGB-like)
    for (size_t y = 0; y < effHeight; ++y) {
        for (size_t x = 0; x < effWidth; ++x) {
            size_t srcX = x * width / effWidth;
            size_t srcY = y * height / effHeight;
            
            float value = input[srcX + srcY * width];
            
            // Create opponent pairs
            float redGreen = value;  // Simplified: treat as red-green axis
            float blueYellow = (x + y) % 2 == 0 ? -value : value;  // Simplified: alternate sign
            
            // Map to different feature maps
            size_t redGreenIdx = 0;
            size_t blueYellowIdx = 1;
            
            if (scaleFeature.dimensions > 2) {
                redGreenIdx = (y * effWidth + x) % (scaleFeature.dimensions / 2);
                blueYellowIdx = scaleFeature.dimensions / 2 + ((y * effWidth + x) % (scaleFeature.dimensions / 2));
                
                colorFeatures[redGreenIdx] += redGreen * pImpl->connectivityRadius;
                colorFeatures[blueYellowIdx] += blueYellow * pImpl->connectivityRadius;
            }
        }
    }
    
    // Normalize color features
    float total = 0.0f;
    for (float c : colorFeatures) total += std::abs(c);
    if (total > 0.0f) {
        for (float& c : colorFeatures) c /= total;
    }
    
    // Add to scale feature
    scaleFeature.features.insert(scaleFeature.features.end(),
                                 colorFeatures.begin(), colorFeatures.end());
}

void VisionProcessor::normalizeFeatures(ScaleFeature& scaleFeature) {
    // Normalize feature vectors for better neural processing
    if (scaleFeature.features.empty()) return;
    
    float sum = 0.0f;
    for (float feature : scaleFeature.features) {
        sum += std::abs(feature);
    }
    
    if (sum > 0.0f) {
        for (float& feature : scaleFeature.features) {
            feature /= sum;
        }
    }
}

void VisionProcessor::integrateFeatures() {
    // Combine features across different spatial scales
    // Hierarchical integration similar to biological visual cortex
    
    std::vector<float> integratedFeatures;
    
    // Weight different scales (coarse scales more important for context)
    for (size_t scale = 0; scale < pImpl->numScales; ++scale) {
        float scaleWeight = std::pow(2.0f, static_cast<float>(pImpl->numScales - 1 - scale));
        
        for (float feature : pImpl->scaleFeatures[scale].features) {
            integratedFeatures.push_back(feature * scaleWeight * pImpl->scaleFeatures[scale].weight);
        }
    }
    
    // Normalize integrated features
    float sum = 0.0f;
    for (float feature : integratedFeatures) {
        sum += std::abs(feature);
    }
    
    if (sum > 0.0f) {
        for (float& feature : integratedFeatures) {
            feature /= sum;
        }
    }
    
    // Store integrated features
    pImpl->scaleFeatures[0].features = integratedFeatures;
    pImpl->scaleFeatures[0].dimensions = integratedFeatures.size();
}

void VisionProcessor::detectNovelty() {
    // Simple novelty detection based on feature changes
    // Placeholder implementation
    // Could be enhanced with more sophisticated novelty detection
    
    if (pImpl->scaleFeatures.empty()) return;
    
    float novelty = 0.0f;
    for (size_t scale = 0; scale < pImpl->numScales; ++scale) {
        // Calculate variance in features
        if (pImpl->scaleFeatures[scale].features.empty()) continue;
        
        float mean = 0.0f;
        for (float feature : pImpl->scaleFeatures[scale].features) {
            mean += feature;
        }
        mean /= pImpl->scaleFeatures[scale].features.size();
        
        float variance = 0.0f;
        for (float feature : pImpl->scaleFeatures[scale].features) {
            variance += (feature - mean) * (feature - mean);
        }
        variance /= pImpl->scaleFeatures[scale].features.size();
        
        novelty += variance * pImpl->noveltySensitivity;
    }
    
    // Apply novelty sensitivity
    novelty = std::min(1.0f, novelty);
    
    NLM_LOG_DEBUG("Vision novelty detected: " + std::to_string(novelty));
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->scaleFeatures[0].features;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->scaleFeatures[0].dimensions;
}

} // namespace nlm

