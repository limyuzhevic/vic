#include "Vision.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct VisionProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    
    // Neural vision parameters
    std::vector<float> receptiveFields;
    std::vector<float> featureMaps;
    
    // Vision processing parameters
    size_t receptiveFieldSize;
    size_t featureMapSize;
    float contrastSensitivity;
    float spatialFrequency;
    
    // Learning components
    std::vector<float> learnedFilters;
    float learningRate;
    float featureAdaptation;
    
    // State tracking
    std::vector<float> previousInput;
    std::vector<float> currentInput;
    float processingTime;
    
    // Attention modulation
    float attentionGain;
    std::vector<float> attentionWeights;
    
    Impl() : 
        featureDimensions(0), receptiveFieldSize(7), featureMapSize(32),
        contrastSensitivity(0.5f), spatialFrequency(0.1f),
        learningRate(0.01f), featureAdaptation(0.9f),
        processingTime(0.0f), attentionGain(1.0f) {
        
        // Initialize receptive fields (center-surround Gabor-like)
        receptiveFields.resize(receptiveFieldSize * receptiveFieldSize, 0.0f);
        featureMaps.resize(featureMapSize, 0.0f);
        learnedFilters.resize(16, 0.0f); // 4x4 filter bank
        attentionWeights.resize(featureMapSize, 1.0f);
        
        // Initialize receptive fields with center-surround patterns
        initializeReceptiveFields();
        
        // Initialize learned filters with random patterns
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
        
        for (size_t i = 0; i < learnedFilters.size(); ++i) {
            learnedFilters[i] = dis(gen);
        }
    }
    
    void initializeReceptiveFields() {
        // Initialize center-surround receptive fields
        float centerStrength = 1.0f;
        float surroundStrength = -0.3f;
        
        size_t halfSize = receptiveFieldSize / 2;
        
        for (size_t y = 0; y < receptiveFieldSize; ++y) {
            for (size_t x = 0; x < receptiveFieldSize; ++x) {
                float distance = std::sqrt(std::pow(static_cast<float>(x) - halfSize, 2) +
                                          std::pow(static_cast<float>(y) - halfSize, 2));
                
                float radius = static_cast<float>(halfSize);
                float normalizedDist = distance / radius;
                
                // Center-surround pattern with smooth transition
                float value = centerStrength * std::exp(-normalizedDist * 2.0f) +
                             surroundStrength * (1.0f - std::exp(-normalizedDist));
                
                receptiveFields[y * receptiveFieldSize + x] = value;
            }
        }
    }
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input, class Brain* brain) {
    // Real vision processing using neural mechanisms
    
    pImpl->previousInput = pImpl->currentInput;
    pImpl->currentInput = input.getData();
    
    // Resize input if necessary
    size_t inputSize = input.getDimensions();
    if (inputSize != pImpl->features.size()) {
        pImpl->features.resize(inputSize, 0.0f);
    }
    
    // Convert input to 2D representation for processing
    std::vector<std::vector<float>> input2D = create2DInput(input);
    
    // Process through multiple stages
    processRetinalStage(input2D, brain);
    processFeatureExtractionStage(brain);
    processIntegrativeStage(brain);
    
    // Store processed features
    pImpl->features = extractFinalFeatures();
    
    // Update attention weights based on feature saliency
    updateAttentionWeights(brain);
    
    // Store processing time
    pImpl->processingTime = calculateProcessingTime();
}

void VisionProcessor::process(const Vision& input) {
    // Legacy method without brain reference
    process(input, nullptr);
}

std::vector<std::vector<float>> VisionProcessor::Impl::create2DInput(const Vision& input) const {
    // Convert 1D vision input to 2D representation
    std::vector<float> data = input.getData();
    size_t dimensions = input.getDimensions();
    
    // Estimate image dimensions (assuming square image)
    size_t width = static_cast<size_t>(std::sqrt(dimensions));
    if (width * width != dimensions) {
        // If not perfect square, use aspect ratio
        width = dimensions / 4; // Assume 4:1 aspect ratio
    }
    
    size_t height = dimensions / width;
    
    std::vector<std::vector<float>> input2D(height, std::vector<float>(width, 0.0f));
    
    for (size_t y = 0; y < height && y * width < dimensions; ++y) {
        for (size_t x = 0; x < width && y * width + x < dimensions; ++x) {
            input2D[y][x] = data[y * width + x];
        }
    }
    
    return input2D;
}

void VisionProcessor::Impl::processRetinalStage(const std::vector<std::vector<float>>& input2D, class Brain* brain) {
    // Simulate retinal processing (photoreceptor response, ganglion cell response)
    
    size_t height = input2D.size();
    size_t width = input2D[0].size();
    
    // Apply receptive field processing (center-surround filtering)
    featureMaps.clear();
    featureMaps.resize(width * height, 0.0f);
    
    size_t halfRF = receptiveFieldSize / 2;
    
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            float centerSurroundResponse = 0.0f;
            
            // Apply center-surround receptive field
            for (size_t rfY = 0; rfY < receptiveFieldSize; ++rfY) {
                for (size_t rfX = 0; rfX < receptiveFieldSize; ++rfX) {
                    size_t imageY = y + rfY - halfRF;
                    size_t imageX = x + rfX - halfRF;
                    
                    if (imageY < height && imageX < width) {
                        float receptiveFieldWeight = receptiveFields[rfY * receptiveFieldSize + rfX];
                        float pixelValue = input2D[imageY][imageX];
                        
                        centerSurroundResponse += receptiveFieldWeight * pixelValue;
                    }
                }
            }
            
            // Apply contrast gain control
            float localContrast = calculateLocalContrast(input2D, y, x, halfRF * 2);
            float gain = 1.0f / (1.0f + localContrast * contrastSensitivity);
            
            featureMaps[y * width + x] = centerSurroundResponse * gain * attentionWeights[y * width + x];
        }
    }
}

void VisionProcessor::Impl::processFeatureExtractionStage(class Brain* brain) {
    // Extract features through learned filter bank
    
    // Apply learned filters (convolution)
    std::vector<float> filteredFeatures(featureMapSize, 0.0f);
    
    // Simple filter bank: edges, orientations, blobs
    size_t featureSize = featureMapSize;
    
    for (size_t f = 0; f < featureSize; ++f) {
        float response = 0.0f;
        
        // Apply different filter types
        switch (f % 4) {
            case 0: // Edge detector (horizontal)
                response = applyFilter(featureMaps, 0, 0, 0, 1);
                break;
            case 1: // Edge detector (vertical)
                response = applyFilter(featureMaps, 1, 0, 0, 1);
                break;
            case 2: // Blob detector (center-surround)
                response = applyFilter(featureMaps, 2, 2, 0, 0);
                break;
            case 3: // Color opponent (simplified)
                response = applyFilter(featureMaps, 0, 0, 1, 1);
                break;
        }
        
        // Normalize response
        response = std::tanh(response * 0.1f);
        filteredFeatures[f] = response;
    }
    
    // Update learned filters based on experience
    adaptFilters(filteredFeatures, featureMaps);
    
    // Store filtered features
    featureMaps = filteredFeatures;
}

void VisionProcessor::Impl::processIntegrativeStage(class Brain* brain) {
    // Integrate features into higher-level representations
    
    // Compute feature statistics
    float meanFeature = calculateMean(featureMaps);
    float stdFeature = calculateStdDev(featureMaps, meanFeature);
    
    // Extract salient features
    std::vector<size_t> salientLocations = findSalientLocations(featureMaps);
    
    // Update attention weights based on saliency
    for (size_t i = 0; i < featureMaps.size(); ++i) {
        bool isSalient = false;
        for (size_t loc : salientLocations) {
            if (i == loc) {
                isSalient = true;
                break;
            }
        }
        
        if (isSalient) {
            attentionWeights[i] = std::min(attentionWeights[i] * 1.1f, 2.0f);
        } else {
            attentionWeights[i] *= 0.95f; // Decay unattended weights
        }
    }
    
    // Normalize attention weights
    float attentionSum = calculateSum(attentionWeights);
    if (attentionSum > 0.0f) {
        for (size_t i = 0; i < attentionWeights.size(); ++i) {
            attentionWeights[i] /= attentionSum;
        }
    }
}

std::vector<float> VisionProcessor::extractFinalFeatures() const {
    // Extract final feature vector for output
    
    std::vector<float> finalFeatures;
    
    // Add basic statistics as features
    float mean = calculateMean(featureMaps);
    float std = calculateStdDev(featureMaps, mean);
    float maxVal = *std::max_element(featureMaps.begin(), featureMaps.end());
    float minVal = *std::min_element(featureMaps.begin(), featureMaps.end());
    
    finalFeatures.push_back(mean);
    finalFeatures.push_back(std);
    finalFeatures.push_back(maxVal);
    finalFeatures.push_back(minVal);
    
    // Add saliency features
    std::vector<size_t> salientLocations = findSalientLocations(featureMaps);
    float saliencyRatio = static_cast<float>(salientLocations.size()) / featureMaps.size();
    finalFeatures.push_back(saliencyRatio);
    
    // Add temporal features
    if (!pImpl->previousInput.empty()) {
        float temporalChange = calculateTemporalChange(pImpl->previousInput, pImpl->currentInput);
        finalFeatures.push_back(temporalChange);
    } else {
        finalFeatures.push_back(0.0f);
    }
    
    // Add attention features
    float attentionMean = calculateMean(attentionWeights);
    finalFeatures.push_back(attentionMean);
    
    // Add processing time feature
    finalFeatures.push_back(pImpl->processingTime);
    
    return finalFeatures;
}

void VisionProcessor::Impl::updateAttentionWeights(class Brain* brain) {
    // Update attention weights based on neural mechanisms
    
    // Integrate with brain's attention system if available
    if (brain) {
        auto attention = brain->getAttention();
        if (attention) {
            // Get attention weights from attention system
            // This creates integration between vision and attention
            for (size_t i = 0; i < std::min(attentionWeights.size(), featureMapSize); ++i) {
                attentionWeights[i] *= 0.7f + 0.3f * attention->getActivation(i % 10); // Simplified
            }
        }
    }
    
    // Apply normalization
    float attentionSum = calculateSum(attentionWeights);
    if (attentionSum > 0.0f) {
        for (size_t i = 0; i < attentionWeights.size(); ++i) {
            attentionWeights[i] /= attentionSum;
        }
    }
}

void VisionProcessor::Impl::adaptFilters(const std::vector<float>& targets, const std::vector<float>& inputs) {
    // Adapt learned filters based on error signal
    
    float learningRate = featureAdaptation;
    
    for (size_t i = 0; i < learnedFilters.size(); ++i) {
        // Simple Hebbian-like learning
        float error = targets[i] - inputs[i];
        learnedFilters[i] += learningRate * error;
        
        // Keep filters bounded
        learnedFilters[i] = std::clamp(learnedFilters[i], -1.0f, 1.0f);
    }
}

float VisionProcessor::Impl::calculateLocalContrast(const std::vector<std::vector<float>>& input2D, 
                                                  size_t centerY, size_t centerX, 
                                                  size_t neighborhoodSize) const {
    // Calculate local contrast around a point
    
    float centerValue = input2D[centerY][centerX];
    
    float minVal = centerValue;
    float maxVal = centerValue;
    
    size_t halfSize = neighborhoodSize / 2;
    
    for (size_t y = std::max(size_t(0), centerY - halfSize); 
         y < std::min(input2D.size(), centerY + halfSize + 1); ++y) {
        for (size_t x = std::max(size_t(0), centerX - halfSize); 
             x < std::min(input2D[y].size(), centerX + halfSize + 1); ++x) {
            float val = input2D[y][x];
            minVal = std::min(minVal, val);
            maxVal = std::max(maxVal, val);
        }
    }
    
    float contrast = (maxVal - minVal) / (maxVal + minVal + 0.0001f);
    return std::clamp(contrast, 0.0f, 1.0f);
}

float VisionProcessor::Impl::applyFilter(const std::vector<float>& source, 
                                       size_t rfSizeY, size_t rfSizeX,
                                       size_t strideY, size_t strideX) const {
    // Apply simple filter to source data
    
    float sum = 0.0f;
    float weightSum = 0.0f;
    
    // Simple filter implementation
    for (size_t y = 0; y < rfSizeY; ++y) {
        for (size_t x = 0; x < rfSizeX; ++x) {
            size_t sourceIdx = y * source.size() / rfSizeY + x * source.size() / rfSizeX;
            if (sourceIdx < source.size()) {
                sum += source[sourceIdx];
                weightSum += 1.0f;
            }
        }
    }
    
    if (weightSum > 0.0f) {
        return sum / weightSum;
    }
    
    return 0.0f;
}

std::vector<size_t> VisionProcessor::Impl::findSalientLocations(const std::vector<float>& features) const {
    // Find locations with high feature responses
    
    std::vector<size_t> salientLocations;
    float threshold = calculateMean(features) + calculateStdDev(features, calculateMean(features)) * 1.0f;
    
    for (size_t i = 0; i < features.size(); ++i) {
        if (features[i] > threshold) {
            salientLocations.push_back(i);
        }
    }
    
    return salientLocations;
}

float VisionProcessor::Impl::calculateMean(const std::vector<float>& values) const {
    if (values.empty()) return 0.0f;
    
    float sum = std::accumulate(values.begin(), values.end(), 0.0f);
    return sum / values.size();
}

float VisionProcessor::Impl::calculateStdDev(const std::vector<float>& values, float mean) const {
    if (values.empty()) return 0.0f;
    
    float sumSq = 0.0f;
    for (float val : values) {
        float diff = val - mean;
        sumSq += diff * diff;
    }
    
    return std::sqrt(sumSq / values.size());
}

float VisionProcessor::Impl::calculateSum(const std::vector<float>& values) const {
    return std::accumulate(values.begin(), values.end(), 0.0f);
}

float VisionProcessor::Impl::calculateTemporalChange(const std::vector<float>& prev, const std::vector<float>& current) const {
    if (prev.size() != current.size() || prev.empty()) return 0.0f;
    
    float sumDiff = 0.0f;
    for (size_t i = 0; i < prev.size(); ++i) {
        float diff = current[i] - prev[i];
        sumDiff += std::abs(diff);
    }
    
    return sumDiff / prev.size();
}

float VisionProcessor::calculateProcessingTime() const {
    return pImpl->processingTime;
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

void VisionProcessor::setFeatureDimensions(size_t dimensions) {
    pImpl->featureDimensions = dimensions;
}

} // namespace nlm
