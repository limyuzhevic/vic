#include "Vision.hpp"
#include <algorithm>
#include <cmath>
#include <queue>
#include <unordered_set>
#include <array>
#include <numeric>

namespace nlm {

struct VisionProcessor::Impl {
    // Multi-scale feature pyramid
    std::vector<std::vector<float>> featurePyramid;
    std::vector<size_t> pyramidLevels;
    
    // Saliency maps
    std::vector<float> intensitySaliency;
    std::vector<float> edgeSaliency;
    std::vector<float> textureSaliency;
    std::vector<float> combinedSaliency;
    
    // Object detection results
    std::vector<std::array<float, 4>> objectBoxes; // x, y, w, h
    std::vector<float> objectScores;
    std::vector<size_t> objectScales;
    
    // Foveal-peripheral attention weights
    std::vector<std::vector<float>> fovealWeights;
    std::vector<std::vector<float>> peripheralWeights;
    
    // Predictive coding buffers
    std::vector<float> predictionError;
    std::vector<float> prediction;
    
    // Feature extraction buffers
    std::vector<float> edges;
    std::vector<float> contours;
    std::vector<float> textures;
    
    Impl() : featureDimensions(0) {
        pyramidLevels.clear();
        featurePyramid.clear();
        intensitySaliency.clear();
        edgeSaliency.clear();
        textureSaliency.clear();
        combinedSaliency.clear();
        objectBoxes.clear();
        objectScores.clear();
        objectScales.clear();
        fovealWeights.clear();
        peripheralWeights.clear();
        predictionError.clear();
        prediction.clear();
        edges.clear();
        contours.clear();
        textures.clear();
    }
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    const std::vector<float>& data = input.getData();
    size_t width = input.getWidth();
    size_t height = input.getHeight();
    size_t channels = input.getChannels();
    size_t dimensions = input.getDimensions();
    
    // Initialize feature buffers
    pImpl->features.clear();
    pImpl->featurePyramid.clear();
    pImpl->pyramidLevels.clear();
    
    // Multi-scale feature extraction
    extractMultiScaleFeatures(data, width, height, channels, dimensions);
    
    // Generate saliency maps with biological plausibility
    generateSaliencyMaps(data, width, height, channels);
    
    // Object detection at multiple scales
    detectObjectsAtMultipleScales(data, width, height, channels);
    
    // Set up foveal-peripheral attention modulation
    setupAttentionModulation(width, height);
    
    // Apply predictive coding integration
    integrateWithPredictiveCoding(data, width, height, channels);
    
    // Store combined features for output
    combineFeaturesIntoOutput();
}

void VisionProcessor::extractMultiScaleFeatures(const std::vector<float>& data,
                                               size_t width, size_t height,
                                               size_t channels, size_t dimensions) {
    // Multi-scale pyramid (Gaussian pyramid)
    pImpl->pyramidLevels = {1, 2, 4, 8, 16, 32};
    
    for (size_t level : pImpl->pyramidLevels) {
        if (width / level > 0 && height / level > 0) {
            std::vector<float> levelFeatures;
            levelFeatures.reserve((width / level) * (height / level) * channels);
            
            // Extract features at this scale
            for (size_t y = 0; y < height; y += level) {
                for (size_t x = 0; x < width; x += level) {
                    // Extract local patch features
                    std::vector<float> patchFeatures = extractLocalFeatures(
                        data, width, height, channels,
                        x, y, level, level);
                    levelFeatures.insert(levelFeatures.end(),
                                       patchFeatures.begin(), patchFeatures.end());
                }
            }
            
            pImpl->featurePyramid.push_back(levelFeatures);
            
            // Accumulate for output
            pImpl->features.insert(pImpl->features.end(),
                                 levelFeatures.begin(), levelFeatures.end());
        }
    }
    
    pImpl->featureDimensions = pImpl->features.size();
}

void VisionProcessor::generateSaliencyMaps(const std::vector<float>& data,
                                          size_t width, size_t height,
                                          size_t channels) {
    size_t totalPixels = width * height;
    
    // Initialize saliency maps
    pImpl->intensitySaliency.assign(totalPixels, 0.0f);
    pImpl->edgeSaliency.assign(totalPixels, 0.0f);
    pImpl->textureSaliency.assign(totalPixels, 0.0f);
    pImpl->combinedSaliency.assign(totalPixels, 0.0f);
    
    if (channels >= 3) {
        // Intensity saliency (center-surround mechanism)
        for (size_t y = 0; y < height; ++y) {
            for (size_t x = 0; x < width; ++x) {
                size_t idx = y * width + x;
                
                // Compute local intensity center-surround difference
                float centerIntensity = data[idx];
                float surroundIntensity = computeSurroundIntensity(
                    data, width, height, x, y, 3, 3);
                
                pImpl->intensitySaliency[idx] = std::abs(centerIntensity - surroundIntensity);
            }
        }
    }
    
    // Edge saliency using Sobel-like operators
    computeEdgeSaliency(data, width, height, channels);
    
    // Texture saliency using local variance
    computeTextureSaliency(data, width, height, channels);
    
    // Combine saliency maps (weighted sum)
    for (size_t i = 0; i < totalPixels; ++i) {
        pImpl->combinedSaliency[i] = 
            0.5f * pImpl->intensitySaliency[i] +
            0.3f * pImpl->edgeSaliency[i] +
            0.2f * pImpl->textureSaliency[i];
        
        // Normalize to [0,1]
        if (pImpl->combinedSaliency[i] > 0.0f) {
            float maxVal = *std::max_element(pImpl->combinedSaliency.begin(),
                                           pImpl->combinedSaliency.end());
            pImpl->combinedSaliency[i] /= maxVal;
        }
    }
}

void VisionProcessor::computeEdgeSaliency(const std::vector<float>& data,
                                         size_t width, size_t height,
                                         size_t channels) {
    size_t totalPixels = width * height;
    
    for (size_t y = 1; y < height - 1; ++y) {
        for (size_t x = 1; x < width - 1; ++x) {
            size_t idx = y * width + x;
            
            if (channels >= 3) {
                // Gradient in x and y directions
                float gradX = 0.0f, gradY = 0.0f;
                
                for (size_t c = 0; c < channels; ++c) {
                    size_t center = ((y * width + x) * channels) + c;
                    size_t left = ((y * width + (x - 1)) * channels) + c;
                    size_t right = ((y * width + (x + 1)) * channels) + c;
                    size_t up = (((y - 1) * width + x) * channels) + c;
                    size_t down = (((y + 1) * width + x) * channels) + c;
                    
                    gradX += std::abs(data[right] - data[left]);
                    gradY += std::abs(data[down] - data[up]);
                }
                
                pImpl->edgeSaliency[idx] = std::sqrt(gradX * gradX + gradY * gradY);
            } else {
                float gradX = std::abs(data[idx] - data[idx - 1]);
                float gradY = std::abs(data[idx] - data[idx - width]);
                pImpl->edgeSaliency[idx] = std::sqrt(gradX * gradX + gradY * gradY);
            }
        }
    }
}

void VisionProcessor::computeTextureSaliency(const std::vector<float>& data,
                                            size_t width, size_t height,
                                            size_t channels) {
    size_t totalPixels = width * height;
    
    for (size_t y = 2; y < height - 2; ++y) {
        for (size_t x = 2; x < width - 2; ++x) {
            size_t idx = y * width + x;
            
            std::vector<float> patch; // Extract 5x5 patch
            for (size_t dy = -2; dy <= 2; ++dy) {
                for (size_t dx = -2; dx <= 2; ++dx) {
                    size_t px = x + dx;
                    size_t py = y + dy;
                    if (px < width && py < height) {
                        if (channels >= 3) {
                            size_t c_idx = (py * width + px) * channels;
                            for (size_t c = 0; c < channels; ++c) {
                                patch.push_back(data[c_idx + c]);
                            }
                        } else {
                            size_t p_idx = py * width + px;
                            patch.push_back(data[p_idx]);
                        }
                    }
                }
            }
            
            // Compute local variance as texture measure
            if (patch.size() > 1) {
                float mean = std::accumulate(patch.begin(), patch.end(), 0.0f) / patch.size();
                float variance = 0.0f;
                for (float val : patch) {
                    variance += (val - mean) * (val - mean);
                }
                variance /= patch.size();
                pImpl->textureSaliency[idx] = variance;
            }
        }
    }
}

float VisionProcessor::computeSurroundIntensity(const std::vector<float>& data,
                                               size_t width, size_t height,
                                               size_t x, size_t y,
                                               size_t kernelW, size_t kernelH) {
    size_t halfW = kernelW / 2;
    size_t halfH = kernelH / 2;
    float sum = 0.0f;
    size_t count = 0;
    
    for (size_t dy = -halfH; dy <= halfH; ++dy) {
        for (size_t dx = -halfW; dx <= halfW; ++dx) {
            size_t px = x + dx;
            size_t py = y + dy;
            
            if (px < width && py < height &&
                (dy != 0 || dx != 0)) { // Exclude center pixel
                if (data.size() >= channels) {
                    size_t idx = (py * width + px) * channels;
                    for (size_t c = 0; c < channels; ++c) {
                        sum += data[idx + c];
                    }
                    count += channels;
                } else {
                    size_t idx = py * width + px;
                    sum += data[idx];
                    count++;
                }
            }
        }
    }
    
    return count > 0 ? sum / count : 0.0f;
}

void VisionProcessor::detectObjectsAtMultipleScales(const std::vector<float>& data,
                                                   size_t width, size_t height,
                                                   size_t channels) {
    // Simple object detection using multi-scale thresholding
    // In a real implementation, this would use trained neural networks
    
    // For now, implement a basic blob detection based on intensity contrast
    pImpl->objectBoxes.clear();
    pImpl->objectScores.clear();
    pImpl->objectScales.clear();
    
    // Test different scales
    std::vector<size_t> testScales = {1, 2, 4, 8};
    
    for (size_t scale : testScales) {
        if (width / scale > 4 && height / scale > 4) {
            // Resize data mentally for this scale (in real implementation)
            std::vector<float> scaleData;
            scaleData.reserve((width / scale) * (height / scale) * channels);
            
            for (size_t y = 0; y < height; y += scale) {
                for (size_t x = 0; x < width; x += scale) {
                    // Sample pixel at this scale
                    size_t srcX = std::min(x, width - 1);
                    size_t srcY = std::min(y, height - 1);
                    size_t srcIdx = (srcY * width + srcX) * channels;
                    
                    float intensity = 0.0f;
                    if (channels >= 3) {
                        for (size_t c = 0; c < channels; ++c) {
                            intensity += data[srcIdx + c];
                        }
                        intensity /= channels;
                    } else {
                        intensity = data[srcIdx];
                    }
                    
                    scaleData.push_back(intensity);
                }
            }
            
            // Detect blobs in scale data
            std::vector<std::array<float, 4>> scaleBoxes;
            std::vector<float> scaleScores;
            detectBlobs(scaleData, width / scale, height / scale, scaleBoxes, scaleScores);
            
            // Filter by scale consistency
            for (size_t i = 0; i < scaleBoxes.size(); ++i) {
                // Scale coordinates back to original size
                std::array<float, 4> box = scaleBoxes[i];
                box[0] *= scale;
                box[1] *= scale;
                box[2] *= scale;
                box[3] *= scale;
                
                // Check scale consistency (similar size at other scales)
                bool consistent = true;
                for (size_t otherScale : testScales) {
                    if (otherScale != scale) {
                        float ratio = static_cast<float>(scale) / otherScale;
                        float expectedW = box[2] * ratio;
                        float expectedH = box[3] * ratio;
                        
                        if (std::abs(box[2] - expectedW) > box[2] * 0.3f ||
                            std::abs(box[3] - expectedH) > box[3] * 0.3f) {
                            consistent = false;
                            break;
                        }
                    }
                }
                
                if (consistent) {
                    pImpl->objectBoxes.push_back(box);
                    pImpl->objectScores.push_back(scaleScores[i] * 0.8f); // Scale score
                    pImpl->objectScales.push_back(scale);
                }
            }
        }
    }
}

void VisionProcessor::detectBlobs(const std::vector<float>& data,
                                  size_t width, size_t height,
                                  std::vector<std::array<float, 4>>& boxes,
                                  std::vector<float>& scores) {
    // Simple connected component analysis for blob detection
    std::vector<bool> visited(height * width, false);
    
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            size_t idx = y * width + x;
            
            if (!visited[idx] && data[idx] > 0.7f) { // Threshold for blob detection
                // Find connected component
                std::queue<size_t> queue;
                queue.push(idx);
                visited[idx] = true;
                
                size_t minX = width, maxX = 0, minY = height, maxY = 0;
                float sumIntensity = 0.0f;
                size_t count = 0;
                
                while (!queue.empty()) {
                    size_t currIdx = queue.front();
                    queue.pop();
                    
                    size_t cy = currIdx / width;
                    size_t cx = currIdx % width;
                    
                    minX = std::min(minX, cx);
                    maxX = std::max(maxX, cx);
                    minY = std::min(minY, cy);
                    maxY = std::max(maxY, cy);
                    
                    sumIntensity += data[currIdx];
                    count++;
                    
                    // Check 4-connected neighbors
                    if (cy > 0 && !visited[currIdx - width] && data[currIdx - width] > 0.7f) {
                        visited[currIdx - width] = true;
                        queue.push(currIdx - width);
                    }
                    if (cy < height - 1 && !visited[currIdx + width] && data[currIdx + width] > 0.7f) {
                        visited[currIdx + width] = true;
                        queue.push(currIdx + width);
                    }
                    if (cx > 0 && !visited[currIdx - 1] && data[currIdx - 1] > 0.7f) {
                        visited[currIdx - 1] = true;
                        queue.push(currIdx - 1);
                    }
                    if (cx < width - 1 && !visited[currIdx + 1] && data[currIdx + 1] > 0.7f) {
                        visited[currIdx + 1] = true;
                        queue.push(currIdx + 1);
                    }
                }
                
                // Calculate blob bounding box
                if (count > 0) {
                    float blobW = (maxX - minX + 1) / static_cast<float>(width);
                    float blobH = (maxY - minY + 1) / static_cast<float>(height);
                    float blobArea = blobW * blobH;
                    float avgIntensity = sumIntensity / count;
                    
                    // Score based on area and intensity contrast
                    float score = blobArea * avgIntensity;
                    
                    // Add box if it's reasonably sized
                    if (blobArea > 0.01f && blobArea < 0.8f) {
                        boxes.push_back({minX / static_cast<float>(width),
                                        minY / static_cast<float>(height),
                                        blobW, blobH});
                        scores.push_back(score);
                    }
                }
            }
        }
    }
}

void VisionProcessor::setupAttentionModulation(size_t width, size_t height) {
    size_t totalPixels = width * height;
    
    // Initialize foveal and peripheral weights
    pImpl->fovealWeights.clear();
    pImpl->fovealWeights.resize(totalPixels, std::vector<float>(totalPixels, 0.0f));
    
    pImpl->peripheralWeights.clear();
    pImpl->peripheralWeights.resize(totalPixels, std::vector<float>(totalPixels, 0.0f));
    
    // Create foveal attention pattern (center-weighted Gaussian)
    float fovealSigma = std::min(width, height) / 8.0f;
    
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            size_t idx = y * width + x;
            
            // Foveal weight: Gaussian centered at image center
            float dx = static_cast<float>(x) - static_cast<float>(width) / 2.0f;
            float dy = static_cast<float>(y) - static_cast<float>(height) / 2.0f;
            float fovealDist = std::sqrt(dx * dx + dy * dy);
            float fovealWeight = std::exp(-(fovealDist * fovealDist) / (2 * fovealSigma * fovealSigma));
            
            // Peripheral weight: anti-Gaussian (higher at edges)
            float edgeDist = std::min({dx, dy, static_cast<float>(width) - x, static_cast<float>(height) - y});
            float peripheralWeight = std::exp(-(edgeDist * edgeDist) / (2 * fovealSigma * fovealSigma));
            
            // Set weights for this pixel
            for (size_t fy = 0; fy < height; ++fy) {
                for (size_t fx = 0; fx < width; ++fx) {
                    size_t fIdx = fy * width + fx;
                    pImpl->fovealWeights[idx][fIdx] = fovealWeight;
                    pImpl->peripheralWeights[idx][fIdx] = peripheralWeight;
                }
            }
        }
    }
}

void VisionProcessor::integrateWithPredictiveCoding(const std::vector<float>& data,
                                                   size_t width, size_t height,
                                                   size_t channels) {
    size_t totalPixels = width * height;
    
    // Simple predictive coding model
    pImpl->predictionError.clear();
    pImpl->prediction.clear();
    pImpl->predictionError.reserve(totalPixels);
    pImpl->prediction.reserve(totalPixels);
    
    // Generate prediction based on previous input (simplified)
    if (pImpl->prediction.empty()) {
        // Initialize prediction with average values
        pImpl->prediction.assign(totalPixels, 0.5f);
        for (size_t i = 0; i < totalPixels; ++i) {
            if (channels >= 3) {
                pImpl->prediction[i] = data[i * channels] / 3.0f; // Average of RGB
            } else {
                pImpl->prediction[i] = data[i];
            }
        }
    }
    
    // Compute prediction error (difference between input and prediction)
    for (size_t i = 0; i < std::min(totalPixels, data.size() / channels); ++i) {
        float inputVal = (channels >= 3) ? data[i * channels] : data[i];
        float predVal = pImpl->prediction[i];
        float error = inputVal - predVal;
        pImpl->predictionError.push_back(std::abs(error));
    }
    
    // Update prediction with error correction (simple delta rule)
    float learningRate = 0.1f;
    for (size_t i = 0; i < std::min(pImpl->prediction.size(), pImpl->predictionError.size()); ++i) {
        pImpl->prediction[i] += learningRate * (data[i] - pImpl->prediction[i]);
    }
}

void VisionProcessor::combineFeaturesIntoOutput() {
    // Combine features from all scales
    pImpl->features.clear();
    
    for (const auto& levelFeatures : pImpl->featurePyramid) {
        pImpl->features.insert(pImpl->features.end(),
                             levelFeatures.begin(), levelFeatures.end());
    }
    
    pImpl->featureDimensions = pImpl->features.size();
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
