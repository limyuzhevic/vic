#include "CNNBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <random>
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

void ConvolutionalLayer::applyConvolution(const std::vector<std::vector<float>>& input) {
    activation = std::vector<std::vector<float>>(64, std::vector<float>(64, 0.0f));
    
    // Simple convolution implementation (3x3 kernel to 64x64 output)
    for (size_t y = 0; y < 64; ++y) {
        for (size_t x = 0; x < 64; ++x) {
            float sum = bias;
            for (size_t ky = 0; ky < 3; ++ky) {
                for (size_t kx = 0; kx < 3; ++kx) {
                    size_t inputY = y * 2 + ky;
                    size_t inputX = x * 2 + kx;
                    if (inputY < input.size() && inputX < input[0].size()) {
                        sum += kernel[ky][kx] * input[inputY][inputX];
                    }
                }
            }
            activation[y][x] = sum;
        }
    }
}

void ConvolutionalLayer::applyReLU(float threshold) {
    for (auto& row : activation) {
        for (auto& value : row) {
            value = std::max(value, threshold);
        }
    }
}

void PoolingLayer::applyMaxPooling(const std::vector<std::vector<float>>& input) {
    output = std::vector<std::vector<float>>(32, std::vector<float>(32, 0.0f));
    
    // 2x2 max pooling
    for (size_t y = 0; y < 32; ++y) {
        for (size_t x = 0; x < 32; ++x) {
            float maxVal = input[y * 2][x * 2];
            for (size_t ky = 0; ky < 2; ++ky) {
                for (size_t kx = 0; kx < 2; ++kx) {
                    size_t iy = y * 2 + ky;
                    size_t ix = x * 2 + kx;
                    if (iy < input.size() && ix < input[0].size()) {
                        maxVal = std::max(maxVal, input[iy][ix]);
                    }
                }
            }
            output[y][x] = maxVal;
        }
    }
}

void PoolingLayer::applyAveragePooling(const std::vector<std::vector<float>>& input) {
    output = std::vector<std::vector<float>>(32, std::vector<float>(32, 0.0f));
    
    // 2x2 average pooling
    for (size_t y = 0; y < 32; ++y) {
        for (size_t x = 0; x < 32; ++x) {
            float sum = 0.0f;
            int count = 0;
            for (size_t ky = 0; ky < 2; ++ky) {
                for (size_t kx = 0; kx < 2; ++kx) {
                    size_t iy = y * 2 + ky;
                    size_t ix = x * 2 + kx;
                    if (iy < input.size() && ix < input[0].size()) {
                        sum += input[iy][ix];
                        count++;
                    }
                }
            }
            output[y][x] = count > 0 ? sum / count : 0.0f;
        }
    }
}

CNNBrain::CNNBrain(std::shared_ptr<Config> config, size_t receptiveFieldSize)
    : Brain(config)
    , receptiveFieldSize_(receptiveFieldSize)
    , convolutionalLayersCount_(3)
    , convolutionalLearningRate_(0.001f)
    , learnedFeaturesCount_(0)
    , sparseConnectivityEnabled_(false)
    , receptiveFieldInitialized_(false)
    , featureExtractionStep_(0)
    , globalInhibitionStrength_(0.5f)
    , temporalFilterAlpha_(0.9)
    , enableTemporalFiltering_(true)
    , pyramidLevels_(3)
    , enablePredictiveCoding_(true)
{
    initializeConvolutionalPipeline(config);
    NLM_LOG_INFO("CNNBrain initialized with receptive field size: " + std::to_string(receptiveFieldSize_));
}

CNNBrain::~CNNBrain() = default;

void CNNBrain::initializeConvolutionalPipeline(const std::shared_ptr<Config>& config) {
    createConvolutionalLayers();
    generateReceptiveFields();
    
    // Initialize feature maps
    featureMap_ = std::vector<std::vector<float>>(64, std::vector<float>(64, 0.0f));
    activationMap_ = std::vector<std::vector<float>>(64, std::vector<float>(64, 0.0f));
    
    // Initialize multi-scale features
    multiScaleFeatures_ = std::vector<std::vector<std::vector<float>>>(
        pyramidLevels_, std::vector<std::vector<float>>(
            receptiveFieldSize_ / pyramidLevels_,
            std::vector<float>(receptiveFieldSize_ / pyramidLevels_, 0.0f)
        )
    );
    
    // Initialize Hebbian weights
    hebbianWeights_ = std::vector<std::vector<float>>(
        receptiveFieldSize_, std::vector<float>(receptiveFieldSize_, 0.0f)
    );
    
    // Initialize feature tracking
    featurePotentiation_ = std::vector<float>(64, 0.0f);
    featureDepression_ = std::vector<float>(64, 0.0f);
    featureEnergies_ = std::vector<float>(64, 0.0f);
    
    // Initialize temporal processing
    previousFeatureMap_ = std::vector<std::vector<float>>(64, std::vector<float>(64, 0.0f));
    
    // Initialize predictive coding
    feedbackPredictions_ = std::vector<std::vector<float>>(
        receptiveFieldSize_, std::vector<float>(receptiveFieldSize_, 0.0f)
    );
    predictionErrors_ = std::vector<std::vector<float>>(
        receptiveFieldSize_, std::vector<float>(receptiveFieldSize_, 0.0f)
    );
    
    receptiveFieldInitialized_ = true;
    learnedFeaturesCount_ = 0;
}

void CNNBrain::createConvolutionalLayers() {
    convolutionalLayers_.clear();
    poolingLayers_.clear();
    
    // Create convolutional layers based on configuration
    for (size_t i = 0; i < convolutionalLayersCount_; ++i) {
        convolutionalLayers_.emplace_back();
        poolingLayers_.emplace_back();
    }
    
    NLM_LOG_INFO("Created " + std::to_string(convolutionalLayersCount_) + " convolutional layers");
}

void CNNBrain::generateReceptiveFields() {
    centerSurroundReceptiveField_ = std::vector<std::vector<float>>(
        receptiveFieldSize_, std::vector<float>(receptiveFieldSize_, 0.0f)
    );
    edgeDetectionReceptiveField_ = std::vector<std::vector<float>>(
        receptiveFieldSize_, std::vector<float>(receptiveFieldSize_, 0.0f)
    );
    motionDetectionReceptiveField_ = std::vector<std::vector<float>>(
        receptiveFieldSize_, std::vector<float>(receptiveFieldSize_, 0.0f)
    );
    
    // Initialize center-surround receptive field (for object detection)
    size_t center = receptiveFieldSize_ / 2;
    float centerValue = 2.0f;
    float surroundValue = -0.5f;
    
    for (size_t y = 0; y < receptiveFieldSize_; ++y) {
        for (size_t x = 0; x < receptiveFieldSize_; ++x) {
            float distance = std::sqrt(std::pow(static_cast<float>(y) - center, 2) +
                                     std::pow(static_cast<float>(x) - center, 2));
            if (distance <= center) {
                centerSurroundReceptiveField_[y][x] = centerValue;
            } else {
                centerSurroundReceptiveField_[y][x] = surroundValue;
            }
        }
    }
    
    // Initialize edge detection receptive field (Sobel-like filters)
    for (size_t y = 0; y < receptiveFieldSize_; ++y) {
        for (size_t x = 0; x < receptiveFieldSize_; ++x) {
            if (x > 0 && x < receptiveFieldSize_ - 1) {
                edgeDetectionReceptiveField_[y][x] = static_cast<float>(x - center);
            } else {
                edgeDetectionReceptiveField_[y][x] = 0.0f;
            }
        }
    }
    
    // Initialize motion detection receptive field (directional filters)
    for (size_t y = 0; y < receptiveFieldSize_; ++y) {
        for (size_t x = 0; x < receptiveFieldSize_; ++x) {
            motionDetectionReceptiveField_[y][x] = static_cast<float>(y) - center;
        }
    }
}

void CNNBrain::processVisualInput(const Vision& vision) {
    if (!receptiveFieldInitialized_) {
        return;
    }
    
    // Convert vision data to input image
    std::vector<std::vector<float>> inputImage(
        receptiveFieldSize_, std::vector<float>(receptiveFieldSize_, 0.0f)
    );
    
    // Process vision data into image representation
    size_t width = std::min<size_t>(vision.getWidth(), receptiveFieldSize_);
    size_t height = std::min<size_t>(vision.getHeight(), receptiveFieldSize_);
    
    const auto& visionData = vision.getData();
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            // Map vision data to image intensity (0-1 range)
            size_t index = y * width + x;
            if (index < visionData.size()) {
                inputImage[y][x] = visionData[index];
            }
        }
    }
    
    // Apply preprocessing (normalization, noise reduction)
    for (auto& row : inputImage) {
        for (auto& pixel : row) {
            // Normalize to [0, 1]
            pixel = std::max(0.0f, std::min(1.0f, pixel));
            
            // Apply temporal filtering if enabled
            if (enableTemporalFiltering_) {
                pixel = pixel * (1.0f - temporalFilterAlpha_) +
                        previousFeatureMap_[y][x] * temporalFilterAlpha_;
            }
        }
    }
    
    // Process through convolutional layers
    processThroughAllLayers();
    
    // Update previous state for temporal processing
    previousFeatureMap_ = featureMap_;
    
    // Increment feature extraction step
    featureExtractionStep_++;
    
    // Store current features in pyramid
    if (featureExtractionStep_ % 10 == 0) {
        // Add current features to pyramid at appropriate scale level
        size_t pyramidLevel = featureExtractionStep_ % pyramidLevels_;
        multiScaleFeatures_[pyramidLevel] = featureMap_;
    }
    
    NLM_LOG_INFO("Processed visual input through CNN (step " + std::to_string(featureExtractionStep_) + ")");
}

void CNNBrain::processThroughAllLayers() {
    // Input to first convolutional layer
    convolutionalLayers_[0].applyConvolution(featureMap_);
    convolutionalLayers_[0].applyReLU(0.0f);
    
    // Intermediate layers
    for (size_t i = 1; i < convolutionalLayersCount_ - 1; ++i) {
        convolutionalLayers_[i].applyConvolution(convolutionalLayers_[i-1].activation);
        convolutionalLayers_[i].applyReLU(0.0f);
    }
    
    // Final convolutional layer
    if (convolutionalLayersCount_ > 1) {
        convolutionalLayers_[convolutionalLayersCount_ - 1].applyConvolution(
            convolutionalLayers_[convolutionalLayersCount_ - 2].activation
        );
        convolutionalLayers_[convolutionalLayersCount_ - 1].applyReLU(0.0f);
    }
    
    // Update feature map with final activation
    featureMap_ = convolutionalLayers_.back().activation;
    activationMap_ = featureMap_;
    
    // Apply spatial pooling
    for (size_t i = 0; i < poolingLayers_.size(); ++i) {
        poolingLayers_[i].applyMaxPooling(
            (i == 0) ? featureMap_ : poolingLayers_[i-1].output
        );
    }
    
    // Apply lateral inhibition between feature maps
    applyLateralInhibition(globalInhibitionStrength_);
    
    // Apply sparse connectivity if enabled
    if (sparseConnectivityEnabled_) {
        applySparseProcessing();
    }
}

void CNNBrain::applyLateralInhibition(float strength) {
    // Global inhibition between feature maps
    for (size_t y = 0; y < 64; ++y) {
        for (size_t x = 0; x < 64; ++x) {
            // Find maximum activation in neighborhood
            float maxActivation = featureMap_[y][x];
            for (size_t ny = std::max<size_t>(0, y-1); ny <= std::min<size_t>(63, y+1); ++ny) {
                for (size_t nx = std::max<size_t>(0, x-1); nx <= std::min<size_t>(63, x+1); ++nx) {
                    maxActivation = std::max(maxActivation, featureMap_[ny][nx]);
                }
            }
            
            // Apply lateral inhibition
            featureMap_[y][x] = maxActivation - strength * (maxActivation - featureMap_[y][x]);
            featureMap_[y][x] = std::max(0.0f, featureMap_[y][x]);
        }
    }
}

void CNNBrain::applySparseProcessing() {
    // Simple sparse processing implementation
    // In practice, this would involve thresholding and pruning low-activation neurons
    
    std::vector<std::vector<bool>> activeMask(64, std::vector<bool>(64, false));
    
    // Threshold for activation
    float threshold = 0.01f * getMaxActivation();
    
    // Identify active neurons
    for (size_t y = 0; y < 64; ++y) {
        for (size_t x = 0; x < 64; ++x) {
            if (featureMap_[y][x] > threshold) {
                activeMask[y][x] = true;
            }
        }
    }
    
    // Apply sparse connectivity
    for (size_t y = 0; y < 64; ++y) {
        for (size_t x = 0; x < 64; ++x) {
            if (!activeMask[y][x]) {
                // Suppress inactive neurons
                featureMap_[y][x] = 0.0f;
            }
        }
    }
}

void CNNBrain::learnVisualFeatures(const Vision& visualInput) {
    if (convolutionalLayers_.empty()) {
        return;
    }
    
    // Convert visual input to feature space
    std::vector<std::vector<float>> inputImage(
        receptiveFieldSize_, std::vector<float>(receptiveFieldSize_, 0.0f)
    );
    
    size_t width = std::min<size_t>(visualInput.getWidth(), receptiveFieldSize_);
    size_t height = std::min<size_t>(visualInput.getHeight(), receptiveFieldSize_);
    
    const auto& visualData = visualInput.getData();
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            size_t index = y * width + x;
            if (index < visualData.size()) {
                inputImage[y][x] = visualData[index];
            }
        }
    }
    
    // Update Hebbian weights based on visual experience
    updateConvolutionalWeights(visualInput);
    
    // Store learned features
    learnedFeaturesCount_++;
    
    // Apply synaptic plasticity to convolutional layers
    applyHebbianLearningToConvolutionalLayers(inputImage);
    
    NLM_LOG_INFO("Learned visual features in CNN (total: " + std::to_string(learnedFeaturesCount_) + ")");
}

void CNNBrain::updateConvolutionalWeights(const Vision& visualInput) {
    // Simplified Hebbian learning for convolutional weights
    std::vector<std::vector<float>> inputImage(
        receptiveFieldSize_, std::vector<float>(receptiveFieldSize_, 0.0f)
    );
    
    size_t width = std::min<size_t>(visualInput.getWidth(), receptiveFieldSize_);
    size_t height = std::min<size_t>(visualInput.getHeight(), receptiveFieldSize_);
    
    const auto& visualData = visualInput.getData();
    for (size_t y = 0; y < height; ++y) {
        for (size_t x = 0; x < width; ++x) {
            size_t index = y * width + x;
            if (index < visualData.size()) {
                inputImage[y][x] = visualData[index];
            }
        }
    }
    
    // Update center-surround receptive field
    for (size_t y = 0; y < receptiveFieldSize_; ++y) {
        for (size_t x = 0; x < receptiveFieldSize_; ++x) {
            // Hebbian potentiation for center-surround
            centerSurroundReceptiveField_[y][x] += convolutionalLearningRate_ * inputImage[y][x];
            
            // Weight decay
            centerSurroundReceptiveField_[y][x] *= (1.0f - 0.001f);
        }
    }
    
    // Update edge detection receptive field
    for (size_t y = 0; y < receptiveFieldSize_; ++y) {
        for (size_t x = 0; x < receptiveFieldSize_; ++x) {
            // Simplified edge detection learning
            edgeDetectionReceptiveField_[y][x] += convolutionalLearningRate_ * inputImage[y][x];
            
            // Weight decay
            edgeDetectionReceptiveField_[y][x] *= (1.0f - 0.001f);
        }
    }
}

void CNNBrain::applyHebbianLearningToConvolutionalLayers(const std::vector<std::vector<float>>& inputImage) {
    for (size_t i = 0; i < convolutionalLayers_.size(); ++i) {
        // Apply Hebbian learning to each convolutional layer
        for (size_t y = 0; y < 64; ++y) {
            for (size_t x = 0; x < 64; ++x) {
                // Update weights based on input and activation
                float activation = convolutionalLayers_[i].activation[y][x];
                
                // Hebbian potentiation
                featurePotentiation_[i] += activation * 0.01f;
                
                // Hebbian depression for negative correlations
                if (activation < 0.0f) {
                    featureDepression_[i] += std::abs(activation) * 0.01f;
                }
            }
        }
    }
}

void CNNBrain::extractSpatialFeatures() {
    // Enhanced spatial feature extraction
    featureMap_ = convolutionalLayers_.back().activation;
    
    // Apply multi-scale processing
    for (size_t level = 0; level < pyramidLevels_; ++level) {
        float scale = 1.0f / std::pow(2.0f, static_cast<float>(level));
        size_t targetSize = static_cast<size_t>(64 * scale);
        
        if (targetSize > 0) {
            std::vector<std::vector<float>> scaledFeatures(
                targetSize, std::vector<float>(targetSize, 0.0f)
            );
            
            for (size_t y = 0; y < targetSize; ++y) {
                for (size_t x = 0; x < targetSize; ++x) {
                    size_t sourceY = y * static_cast<size_t>(64 / targetSize);
                    size_t sourceX = x * static_cast<size_t>(64 / targetSize);
                    
                    if (sourceY < featureMap_.size() && sourceX < featureMap_[0].size()) {
                        scaledFeatures[y][x] = featureMap_[sourceY][sourceX];
                    }
                }
            }
            
            multiScaleFeatures_[level] = scaledFeatures;
        }
    }
}

void CNNBrain::applySpatialPooling() {
    // Enhanced spatial pooling
    for (size_t i = 0; i < poolingLayers_.size(); ++i) {
        poolingLayers_[i].applyMaxPooling(
            (i == 0) ? featureMap_ : poolingLayers_[i-1].output
        );
        
        // Apply non-linear transformations
        for (size_t y = 0; y < poolingLayers_[i].output.size(); ++y) {
            for (size_t x = 0; x < poolingLayers_[i].output[y].size(); ++x) {
                poolingLayers_[i].output[y][x] = 
                    std::log(1.0f + poolingLayers_[i].output[y][x]) / std::log(2.0f);
            }
        }
    }
}

void CNNBrain::integrateWithCognition() {
    // Integrate CNN features with higher-level cognitive processing
    // This would involve connecting CNN output to NeuralPlanner, ConceptFormation, etc.
    
    // Extract key features for cognition
    std::vector<float> keyFeatures;
    for (size_t y = 0; y < std::min<size_t>(16, featureMap_.size()); ++y) {
        for (size_t x = 0; x < std::min<size_t>(16, featureMap_[0].size()); ++x) {
            keyFeatures.push_back(featureMap_[y][x]);
        }
    }
    
    // Normalize features
    float sum = std::accumulate(keyFeatures.begin(), keyFeatures.end(), 0.0f);
    if (sum > 0.0f) {
        for (auto& feature : keyFeatures) {
            feature /= sum;
        }
    }
    
    // This would feed into concept formation and planning
    // For now, just log the feature extraction
    NLM_LOG_INFO("Integrated CNN features with cognition (" + std::to_string(keyFeatures.size()) + " features)");
}

float CNNBrain::getMaxActivation() const {
    float maxActivation = 0.0f;
    for (const auto& row : featureMap_) {
        for (const auto& value : row) {
            maxActivation = std::max(maxActivation, value);
        }
    }
    return maxActivation;
}

void CNNBrain::reset() {
    Brain::reset();
    
    // Reset CNN-specific state
    featureMap_ = std::vector<std::vector<float>>(64, std::vector<float>(64, 0.0f));
    activationMap_ = std::vector<std::vector<float>>(64, std::vector<float>(64, 0.0f));
    
    for (auto& layer : convolutionalLayers_) {
        layer.activation = std::vector<std::vector<float>>(64, std::vector<float>(64, 0.0f));
    }
    
    for (auto& pool : poolingLayers_) {
        pool.output = std::vector<std::vector<float>>(32, std::vector<float>(32, 0.0f));
    }
    
    multiScaleFeatures_.clear();
    hebbianWeights_ = std::vector<std::vector<float>>(
        receptiveFieldSize_, std::vector<float>(receptiveFieldSize_, 0.0f)
    );
    
    featurePotentiation_ = std::vector<float>(64, 0.0f);
    featureDepression_ = std::vector<float>(64, 0.0f);
    featureEnergies_ = std::vector<float>(64, 0.0f);
    
    previousFeatureMap_ = std::vector<std::vector<float>>(64, std::vector<float>(64, 0.0f));
    
    feedbackPredictions_ = std::vector<std::vector<float>>(
        receptiveFieldSize_, std::vector<float>(receptiveFieldSize_, 0.0f)
    );
    predictionErrors_ = std::vector<std::vector<float>>(
        receptiveFieldSize_, std::vector<float>(receptiveFieldSize_, 0.0f)
    );
    
    featureExtractionStep_ = 0;
    learnedFeaturesCount_ = 0;
    receptiveFieldInitialized_ = true;
    
    NLM_LOG_INFO("CNNBrain reset complete");
}

std::string CNNBrain::getCNNDescription() const {
    std::stringstream ss;
    ss << "CNNBrain - Convolutional Neural Network inspired visual processor" << std::endl;
    ss << "  Receptive field size: " << receptiveFieldSize_ << std::endl;
    ss << "  Convolutional layers: " << convolutionalLayersCount_ << std::endl;
    ss << "  Feature map size: " << featureMap_.size() << "x" << (featureMap_.empty() ? 0 : featureMap_[0].size()) << std::endl;
    ss << "  Learned features: " << learnedFeaturesCount_ << std::endl;
    ss << "  Sparse connectivity: " << (sparseConnectivityEnabled_ ? "Enabled" : "Disabled") << std::endl;
    ss << "  Predictive coding: " << (enablePredictiveCoding_ ? "Enabled" : "Disabled") << std::endl;
    ss << "  Temporal filtering: " << (enableTemporalFiltering_ ? "Enabled" : "Disabled") << std::endl;
    ss << "  Pyramid levels: " << pyramidLevels_ << std::endl;
    
    return ss.str();
}

std::vector<float> CNNBrain::getNeuralActivityFromFeatures() const {
    std::vector<float> activity;
    
    // Extract neural-like activity from feature maps
    for (size_t y = 0; y < featureMap_.size(); ++y) {
        for (size_t x = 0; x < featureMap_[0].size(); ++x) {
            float normalizedValue = (featureMap_[y][x] + 1.0f) / 2.0f;  // Normalize to [0, 1]
            activity.push_back(normalizedValue);
        }
    }
    
    return activity;
}

void CNNBrain::applySparseProcessing() {
    // Placeholder for sparse processing implementation
    // In a full implementation, this would include thresholding,
    // pruning, and energy-based competition
}

} // namespace nlm
