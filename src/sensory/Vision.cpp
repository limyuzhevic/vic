#include "Vision.hpp"
./brain/Neuron.hpp"
./brain/NeuralPopulation.hpp"
./brain/NeuralRegion.hpp"
./memory/NeuralWorkingMemory.hpp"
./memory/AdvancedWorkingMemory.hpp"
./memory/Memory.hpp"
./prediction/PredictionSystem.hpp"
./cognition/ConceptFormation.hpp"
./neuromodulation/Neuromodulator.hpp"
./core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>
#include <unordered_map>
#include <functional>

namespace nlm {

struct VisionProcessor::Impl {
    // Neural computation components
    Brain* brain;
    
    // Visual processing neural populations
    std::vector<std::unique_ptr<NeuralRegion>> visualRegions;
    
    // Retinotopic mapping - visual field representation
    struct RetinotopicNeuron {
        NeuronId id;
        float x;  // Visual field x coordinate (-1 to 1)
        float y;  // Visual field y coordinate (-1 to 1)
        float receptiveFieldSize;
        float orientation;  // Preferred orientation in degrees
        float spatialFrequency;  // Preferred spatial frequency
        float temporalFrequency;  // Preferred temporal frequency
        float motionDirection;  // Preferred motion direction in degrees
        float activation;
        
        RetinotopicNeuron() : id(), x(0), y(0), receptiveFieldSize(0.1f),
            orientation(0), spatialFrequency(0.1f), temporalFrequency(0.1f),
            motionDirection(0), activation(0) {}
    };
    
    std::vector<RetinotopicNeuron> retinotopicNeurons;
    
    // Feature extraction populations
    enum class FeatureType {
        Edge, Corner, Line, Blob, Orientation, Motion, Color, Depth
    };
    
    struct FeaturePopulation {
        std::vector<NeuronId> neurons;
        FeatureType featureType;
        float activationThreshold;
        float tuningWidth;
        float responseGain;
        std::vector<float> featureTuning;
    };
    
    std::vector<FeaturePopulation> featurePopulations;
    
    // Predictive coding layers
    struct PredictiveCodingLayer {
        std::vector<NeuronId> predictionNeurons;
        std::vector<NeuronId> errorNeurons;
        std::vector<NeuronId> precisionNeurons;
        
        float predictionStrength;
        float predictionError;
        float predictionPrecision;
        
        PredictiveCodingLayer() : predictionStrength(0.5f), predictionError(0.0f), 
            predictionPrecision(1.0f) {}
    };
    
    std::vector<PredictiveCodingLayer> predictiveCodingLayers;
    
    // Visual working memory
    std::vector<float> visualWorkingMemory;
    std::vector<float> visualAttentionWeights;
    
    // Cross-modal integration
    std::vector<float> crossModalIntegrationWeights;
    
    // Feature binding
    struct ObjectBinding {
        std::vector<float> features;
        std::vector<int> objectIds;
        float bindingStrength;
        float coherence;
    };
    
    std::vector<ObjectBinding> objectBindings;
    
    // Sensory gating and attention
    std::vector<float> sensoryGatingSignals;
    std::vector<float> attentionSignals;
    
    // Motor planning integration
    std::vector<float> motorPlanningSignals;
    
    // Statistics
    std::vector<float> featureExtractors;
    std::vector<float> visualPredictions;
    std::vector<float> predictionErrors;
    float processingEfficiency;
    
    // Random generator for biological variability
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    std::normal_distribution<float> normalDist;
    
    Impl() : brain(nullptr), processingEfficiency(0.0f), 
        rng(std::random_device{}()), uniformDist(0.0f, 1.0f), normalDist(0.0f, 1.0f) {
        // Initialize neural substrate components
        retinotopicNeurons.reserve(100);
        featurePopulations.reserve(8);
        predictiveCodingLayers.reserve(3);
    }
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    if (!pImpl->brain) {
        NLM_LOG_WARNING("VisionProcessor: Brain not initialized");
        return;
    }
    
    const auto& inputData = input.getData();
    if (inputData.empty()) {
        return;
    }
    
    // Reset processing state
    pImpl->features.clear();
    pImpl->featureExtractors.clear();
    pImpl->visualPredictions.clear();
    pImpl->predictionErrors.clear();
    pImpl->visualWorkingMemory.clear();
    pImpl->visualAttentionWeights.clear();
    pImpl->sensoryGatingSignals.clear();
    pImpl->attentionSignals.clear();
    pImpl->motorPlanningSignals.clear();
    pImpl->crossModalIntegrationWeights.clear();
    pImpl->objectBindings.clear();
    
    // Get image dimensions
    size_t width = input.getWidth();
    size_t height = input.getHeight();
    size_t channels = input.getChannels();
    
    // Initialize retinotopic mapping
    initializeRetinotopicMapping(width, height);
    
    // Extract basic visual features using neural population activity
    extractBasicFeatures(inputData, width, height, channels);
    
    // Process visual field using retinotopic mapping
    processVisualField(inputData, width, height, channels);
    
    // Apply predictive coding for context-aware processing
    applyPredictiveCoding(inputData, width, height, channels);
    
    // Integrate with working memory
    integrateWithWorkingMemory();
    
    // Apply attention and sensory gating
    applyAttentionAndGating();
    
    // Perform feature binding and object recognition
    performFeatureBinding();
    
    // Integrate with motor planning
    integrateWithMotorPlanning();
    
    // Cross-modal integration
    crossModalIntegration();
    
    // Update episodic memory
    updateEpisodicMemory(input);
    
    // Store features for output
    pImpl->features = extractSummaryFeatures();
    pImpl->featureDimensions = pImpl->features.size();
}

void VisionProcessor::initializeRetinotopicMapping(size_t width, size_t height) {
    // Create biologically plausible retinotopic mapping
    // Neurons organized by visual field position with Gaussian receptive fields
    
    float fovX = 2.0f;  // Field of view in visual degrees
    float fovY = 2.0f;
    
    pImpl->retinotopicNeurons.clear();
    size_t numRetinotopic = std::min(static_cast<size_t>(128), width * height / 16);
    
    for (size_t i = 0; i < numRetinotopic; ++i) {
        RetinotopicNeuron& neuron = pImpl->retinotopicNeurons.emplace_back();
        
        // Map to visual field coordinates
        float x = (static_cast<float>(i % static_cast<size_t>(width)) / static_cast<float>(width) * 2.0f) - 1.0f;
        float y = (static_cast<float>(i / static_cast<size_t>(width)) / static_cast<float>(height) * 2.0f) - 1.0f;
        
        neuron.x = x;
        neuron.y = y;
        neuron.receptiveFieldSize = 0.1f + pImpl->uniformDist(pImpl->rng) * 0.2f;
        neuron.orientation = pImpl->uniformDist(pImpl->rng) * 360.0f;
        neuron.spatialFrequency = 0.1f + pImpl->uniformDist(pImpl->rng) * 0.4f;
        neuron.temporalFrequency = 0.05f + pImpl->uniformDist(pImpl->rng) * 0.15f;
        neuron.motionDirection = pImpl->uniformDist(pImpl->rng) * 360.0f;
        neuron.activation = 0.0f;
        
        neuron.id = NeuronId(i + 10000);
    }
}

void VisionProcessor::extractBasicFeatures(const std::vector<float>& data, 
                                         size_t width, size_t height, size_t channels) {
    // Extract basic visual features using neural population activity
    // Based on physiological models of early visual processing
    
    pImpl->featureExtractors.clear();
    pImpl->featureExtractors.resize(static_cast<size_t>(FeatureType::Depth) + 1, 0.0f);
    
    if (data.empty() || width < 3 || height < 3) return;
    
    // Edge detection using difference of Gaussians (DoG)
    float edgeStrength = 0.0f;
    float cornerStrength = 0.0f;
    float lineStrength = 0.0f;
    
    // Apply multi-scale edge detection
    for (size_t scale = 0; scale < 3; ++scale) {
        float scaleFactor = 1.0f + static_cast<float>(scale) * 0.5f;
        
        for (size_t y = 1; y < height - 1; ++y) {
            for (size_t x = 1; x < width - 1; ++x) {
                size_t idx = (y * width + x) * channels;
                
                // Compute gradient with Gaussian weighting
                float gx = 0.0f, gy = 0.0f;
                for (size_t c = 0; c < channels; ++c) {
                    float left = (x > 0) ? data[idx - channels] : 0.0f;
                    float right = (x < width - 1) ? data[idx + channels] : 0.0f;
                    float up = (y > 0) ? data[idx - width * channels] : 0.0f;
                    float down = (y < height - 1) ? data[idx + width * channels] : 0.0f;
                    
                    // Apply Gaussian weighting based on scale
                    float weightX = std::exp(-static_cast<float>(std::pow(x - width/2.0f, 2)) / (scaleFactor * scaleFactor * 4.0f)) * (right - left);
                    float weightY = std::exp(-static_cast<float>(std::pow(y - height/2.0f, 2)) / (scaleFactor * scaleFactor * 4.0f)) * (down - up);
                    
                    gx += weightX;
                    gy += weightY;
                }
                
                float gradient = std::sqrt(gx * gx + gy * gy);
                edgeStrength += gradient * scaleFactor;
                
                // Corner detection using Hessian matrix
                float dxx = 0.0f, dyy = 0.0f, dxy = 0.0f;
                for (size_t c = 0; c < channels; ++c) {
                    float leftLeft = (x > 1) ? data[idx - 2 * channels] : 0.0f;
                    float rightRight = (x < width - 2) ? data[idx + 2 * channels] : 0.0f;
                    float upUp = (y > 1) ? data[idx - 2 * width * channels] : 0.0f;
                    float downDown = (y < height - 2) ? data[idx + 2 * width * channels] : 0.0f;
                    
                    dxx += rightRight + leftLeft - 2 * data[idx + c];
                    dyy += downDown + upUp - 2 * data[idx + c];
                    
                    if (x > 0 && y > 0) {
                        dxy += (data[idx - width * channels - channels] - data[idx - width * channels + channels] -
                               data[idx + width * channels - channels] + data[idx + width * channels + channels]) * 0.25f;
                    }
                }
                
                cornerStrength += std::abs(dxx * dyy - dxy * dxy) * scaleFactor;
                
                // Line detection using oriented filters
                float coherence = std::abs(gx) + std::abs(gy);
                lineStrength += coherence * scaleFactor;
            }
        }
    }
    
    // Normalize feature strengths
    float totalPixels = static_cast<float>((width - 2) * (height - 2) * 3);
    if (totalPixels > 0) {
        pImpl->featureExtractors[static_cast<size_t>(FeatureType::Edge)] = edgeStrength / totalPixels;
        pImpl->featureExtractors[static_cast<size_t>(FeatureType::Corner)] = cornerStrength / totalPixels;
        pImpl->featureExtractors[static_cast<size_t>(FeatureType::Line)] = lineStrength / totalPixels;
    }
    
    // Estimate color and depth features
    if (channels >= 3) {
        // Simple color variance estimate
        float colorVariance = 0.0f;
        size_t colorSamples = std::min(static_cast<size_t>(1000), width * height);
        
        for (size_t i = 0; i < colorSamples && i < data.size() / channels; ++i) {
            size_t r = data[i * channels];
            size_t g = data[i * channels + 1];
            size_t b = data[i * channels + 2];
            
            float gray = 0.299f * r + 0.587f * g + 0.114f * b;
            colorVariance += (gray - 128.0f) * (gray - 128.0f);
        }
        
        pImpl->featureExtractors[static_cast<size_t>(FeatureType::Color)] = colorVariance / static_cast<float>(colorSamples);
    }
    
    // Estimate depth from image gradients (simplified)
    if (channels >= 1) {
        float depthStrength = 0.0f;
        size_t depthSamples = std::min(static_cast<size_t>(100), width * height);
        
        for (size_t i = 0; i < depthSamples && i < data.size() / channels; ++i) {
            size_t x = i % width;
            size_t y = i / width;
            
            if (x > 0 && x < width - 1 && y > 0 && y < height - 1) {
                size_t idx = (y * width + x) * channels;
                float center = data[idx];
                float left = data[idx - channels];
                float right = data[idx + channels];
                
                float gradX = std::abs(right - left);
                float gradY = std::abs((y > 0) ? data[(y-1) * width + x] * channels : 0) - 
                                 ((y < height-1) ? data[(y+1) * width + x] * channels : 0));
                
                depthStrength += std::sqrt(gradX * gradX + gradY * gradY);
            }
        }
        
        pImpl->featureExtractors[static_cast<size_t>(FeatureType::Depth)] = depthStrength / static_cast<float>(depthSamples);
    }
}

void VisionProcessor::processVisualField(const std::vector<float>& data, 
                                       size_t width, size_t height, size_t channels) {
    // Process visual field using retinotopic mapping
    // Simulate activity in retinotopic neurons
    
    pImpl->visualPredictions.clear();
    pImpl->predictionErrors.clear();
    
    for (auto& neuron : pImpl->retinotopicNeurons) {
        // Sample visual input within neuron's receptive field
        float fieldSum = 0.0f;
        float fieldEnergy = 0.0f;
        int sampleCount = 0;
        
        // Convert visual field coordinates to image coordinates
        int centerX = static_cast<int>((neuron.x + 1.0f) * width / 2.0f);
        int centerY = static_cast<int>((neuron.y + 1.0f) * height / 2.0f);
        
        int rfRadius = static_cast<int>(neuron.receptiveFieldSize * std::min(static_cast<float>(width), static_cast<float>(height)) / 2.0f);
        rfRadius = std::max(1, std::min(rfRadius, static_cast<int>(std::min(width, height) / 2)));
        
        for (int y = -rfRadius; y <= rfRadius; ++y) {
            for (int x = -rfRadius; x <= rfRadius; ++x) {
                int imgX = centerX + x;
                int imgY = centerY + y;
                
                if (imgX >= 0 && imgX < static_cast<int>(width) &&
                    imgY >= 0 && imgY < static_cast<int>(height)) {
                    
                    size_t dataIdx = (imgY * width + imgX) * channels;
                    float pixelValue = 0.0f;
                    for (size_t c = 0; c < channels; ++c) {
                        pixelValue += data[dataIdx + c];
                    }
                    pixelValue /= static_cast<float>(channels);
                    
                    fieldSum += pixelValue;
                    fieldEnergy += pixelValue * pixelValue;
                    sampleCount++;
                }
            }
        }
        
        if (sampleCount > 0) {
            float meanValue = fieldSum / static_cast<float>(sampleCount);
            float contrast = std::sqrt(fieldEnergy / static_cast<float>(sampleCount) - meanValue * meanValue);
            
            // Compute prediction based on neuron tuning
            float spatialTuning = std::cos(neuron.orientation * M_PI / 180.0f);
            float temporalTuning = std::sin(neuron.temporalFrequency * M_PI);
            float motionTuning = std::cos(neuron.motionDirection * M_PI / 180.0f);
            
            float prediction = meanValue * spatialTuning * temporalTuning * motionTuning;
            
            // Compute prediction error
            float error = std::abs(meanValue - prediction) * contrast;
            
            pImpl->visualPredictions.push_back(prediction);
            pImpl->predictionErrors.push_back(error);
            
            // Update neuron activation
            neuron.activation = std::tanh(meanValue * contrast * 2.0f);
        }
    }
}

void VisionProcessor::applyPredictiveCoding(const std::vector<float>& data, 
                                          size_t width, size_t height, size_t channels) {
    // Apply predictive coding for context-aware visual processing
    // Integrate with brain's prediction system
    
    if (!pImpl->brain || !pImpl->brain->getPredictionSystem()) {
        return;
    }
    
    // Get current neural activity pattern
    std::vector<float> neuralActivity = pImpl->brain->getCurrentNeuralActivityPattern();
    
    if (neuralActivity.empty()) {
        return;
    }
    
    // Generate prediction for next visual state
    std::vector<float> predictedVisualState;
    if (!pImpl->brain->getPredictionSystem()->getPredictionModel().empty()) {
        const auto& weights = pImpl->brain->getPredictionSystem()->getPredictionModel();
        predictedVisualState = generateNeuralPrediction(neuralActivity, weights);
    } else {
        // Create initial prediction from basic features
        predictedVisualState = pImpl->featureExtractors;
        pImpl->brain->getPredictionSystem()->trainPredictionModel(predictedVisualState);
    }
    
    // Update prediction system
    pImpl->brain->getPredictionSystem()->updatePredictions(predictedVisualState, pImpl->featureExtractors);
    
    // Get prediction error and confidence
    float predictionError = pImpl->brain->getPredictionSystem()->getPredictionError();
    float confidence = pImpl->brain->getPredictionSystem()->getConfidence();
    
    // Apply precision weighting
    float precision = 1.0f - predictionError;
    precision = std::clamp(precision, 0.1f, 1.0f);
    
    // Store prediction state
    if (pImpl->brain->getPredictionSystem()) {
        // Create prediction and error neurons
        for (size_t i = 0; i < pImpl->visualPredictions.size(); ++i) {
            // Prediction neuron
            NeuronId predId = NeuronId(60000 + i);
            if (pImpl->brain) {
                pImpl->brain->injectCurrent(predId, pImpl->visualPredictions[i] * 10.0f);
            }
            
            // Error neuron
            NeuronId errId = NeuronId(70000 + i);
            if (pImpl->brain) {
                pImpl->brain->injectCurrent(errId, pImpl->predictionErrors[i] * 10.0f);
            }
        }
        
        // Update attention based on prediction precision
        if (predictionError > 0.5f) {
            pImpl->sensoryGatingSignals.push_back(precision);
            pImpl->attentionSignals.push_back(1.0f - precision);
        } else {
            pImpl->sensoryGatingSignals.push_back(1.0f - precision);
            pImpl->attentionSignals.push_back(precision);
        }
    }
}

void VisionProcessor::integrateWithWorkingMemory() {
    // Integrate visual features with working memory
    
    if (!pImpl->brain || !pImpl->brain->getAdvancedWorkingMemory()) {
        return;
    }
    
    // Store visual features in working memory
    for (size_t i = 0; i < std::min(pImpl->featureExtractors.size(), static_cast<size_t>(50)); ++i) {
        float featureValue = pImpl->featureExtractors[i];
        
        // Store with decay mechanism
        pImpl->brain->getAdvancedWorkingMemory()->store(
            NeuronId(30000 + i),
            featureValue,
            pImpl->brain->getTotalNeuronCount()
        );
        
        // Also update visual working memory vector
        if (i < pImpl->visualWorkingMemory.size()) {
            pImpl->visualWorkingMemory[i] = featureValue * (1.0f - pImpl->brain->getDecayRate());
        } else {
            pImpl->visualWorkingMemory.push_back(featureValue);
        }
    }
}

void VisionProcessor::applyAttentionAndGating() {
    // Apply attention and sensory gating
    
    pImpl->visualAttentionWeights.clear();
    pImpl->sensoryGatingSignals.clear();
    
    // Compute attention based on feature saliency and prediction error
    for (size_t i = 0; i < pImpl->featureExtractors.size(); ++i) {
        float featureValue = pImpl->featureExtractors[i];
        
        // Saliency computation
        float saliency = featureValue;
        if (i == static_cast<size_t>(FeatureType::Edge) || 
            i == static_cast<size_t>(FeatureType::Motion)) {
            saliency *= 1.5f;
        }
        
        // Gating based on prediction error
        float predictionError = (i < pImpl->predictionErrors.size()) ? pImpl->predictionErrors[i] : 0.0f;
        float gating = 1.0f - predictionError * 0.5f;  // Reduce gating for high prediction error
        
        pImpl->visualAttentionWeights.push_back(saliency * gating);
        pImpl->sensoryGatingSignals.push_back(gating);
        pImpl->attentionSignals.push_back(saliency);
    }
}

void VisionProcessor::performFeatureBinding() {
    // Perform feature binding and object recognition
    
    pImpl->objectBindings.clear();
    
    // Simple feature binding based on co-activation
    std::vector<size_t> activeFeatureIndices;
    for (size_t i = 0; i < pImpl->featureExtractors.size(); ++i) {
        if (pImpl->featureExtractors[i] > 0.1f) {
            activeFeatureIndices.push_back(i);
        }
    }
    
    if (!activeFeatureIndices.empty()) {
        ObjectBinding binding;
        binding.features = std::vector<float>(activeFeatureIndices.size());
        binding.objectIds.resize(activeFeatureIndices.size());
        
        for (size_t i = 0; i < activeFeatureIndices.size(); ++i) {
            binding.features[i] = pImpl->featureExtractors[activeFeatureIndices[i]];
            binding.objectIds[i] = static_cast<int>(activeFeatureIndices[i]);
        }
        
        // Compute binding strength based on feature coherence
        if (binding.features.size() > 1) {
            float sum = std::accumulate(binding.features.begin(), binding.features.end(), 0.0f);
            float mean = sum / binding.features.size();
            
            float variance = 0.0f;
            for (float f : binding.features) {
                variance += (f - mean) * (f - mean);
            }
            variance /= binding.features.size();
            
            float coherence = 1.0f - std::sqrt(variance) / mean;
            binding.coherence = std::clamp(coherence, 0.0f, 1.0f);
            binding.bindingStrength = binding.coherence;
        }
        
        pImpl->objectBindings.push_back(binding);
    }
}

void VisionProcessor::integrateWithMotorPlanning() {
    // Integrate with motor planning
    
    pImpl->motorPlanningSignals.clear();
    
    if (!pImpl->brain) return;
    
    // Compute visual-motor integration signals
    float actionRelevance = pImpl->featureExtractors[static_cast<size_t>(FeatureType::Motion)] +
                          pImpl->featureExtractors[static_cast<size_t>(FeatureType::Edge)];
    
    pImpl->motorPlanningSignals.push_back(actionRelevance);
    
    // Update concept formation
    if (pImpl->brain->getConceptFormation()) {
        pImpl->brain->getConceptFormation()->updateConcept(
            "visual_input",
            std::vector<float>(pImpl->featureExtractors.begin(), pImpl->featureExtractors.end())
        );
    }
}

void VisionProcessor::crossModalIntegration() {
    // Cross-modal integration with other sensory modalities
    
    pImpl->crossModalIntegrationWeights.clear();
    
    if (!pImpl->brain) return;
    
    // Check for other sensory processors
    // In a real implementation, we would:
    // 1. Access AudioProcessor's features
    // 2. Access InternalSignalsProcessor's features
    // 3. Combine with visual features
    
    // Simple placeholder implementation
    pImpl->crossModalIntegrationWeights.resize(3, 0.0f);
    pImpl->crossModalIntegrationWeights[0] = 1.0f;  // Visual
    pImpl->crossModalIntegrationWeights[1] = 0.6f;  // Audio
    pImpl->crossModalIntegrationWeights[2] = 0.4f;  // Internal
}

void VisionProcessor::updateEpisodicMemory(const Vision& input) {
    // Update episodic memory with visual experience
    
    if (!pImpl->brain || !pImpl->brain->getAdvancedEpisodicMemory()) return;
    
    EpisodicMemoryItem episode;
    episode.timestamp = pImpl->brain->getTotalNeuronCount();
    episode.sensoryState = pImpl->features;
    
    // Store active neurons
    for (size_t i = 0; i < pImpl->retinotopicNeurons.size(); ++i) {
        if (pImpl->retinotopicNeurons[i].id.value > 0) {
            episode.activeNeurons.push_back(pImpl->retinotopicNeurons[i].id);
            episode.neuronActivations.push_back(pImpl->featureExtractors[i % pImpl->featureExtractors.size()]);
        }
    }
    
    // Store in episodic memory
    pImpl->brain->getAdvancedEpisodicMemory()->storeExperience(
        episode.sensoryState,
        episode.activeNeurons,
        episode.neuronActivations,
        ActionType::Wait,
        0.5f,
        0.1f
    );
}

const std::vector<float>& VisionProcessor::extractSummaryFeatures() {
    // Extract summary features for output
    
    if (pImpl->visualPredictions.empty()) {
        return pImpl->features;
    }
    
    pImpl->features.clear();
    
    // Compute summary statistics
    float meanPrediction = 0.0f, meanError = 0.0f;
    for (float pred : pImpl->visualPredictions) meanPrediction += pred;
    for (float err : pImpl->predictionErrors) meanError += err;
    
    if (!pImpl->visualPredictions.empty()) {
        meanPrediction /= pImpl->visualPredictions.size();
        meanError /= pImpl->predictionErrors.size();
    }
    
    pImpl->features.push_back(meanPrediction);
    pImpl->features.push_back(meanError);
    
    // Add object binding information
    for (const auto& binding : pImpl->objectBindings) {
        pImpl->features.push_back(binding.bindingStrength);
        pImpl->features.push_back(binding.coherence);
    }
    
    // Add attention information
    for (float weight : pImpl->visualAttentionWeights) {
        pImpl->features.push_back(weight);
    }
    
    return pImpl->features;
}

std::vector<float> VisionProcessor::generateNeuralPrediction(const std::vector<float>& neuralActivity,
                                                           const std::vector<float>& weights) {
    std::vector<float> prediction;
    
    if (neuralActivity.empty() || weights.empty()) return prediction;
    
    size_t size = std::min(neuralActivity.size(), weights.size());
    prediction.resize(size);
    
    for (size_t i = 0; i < size; ++i) {
        prediction[i] = neuralActivity[i] * weights[i];
        prediction[i] = std::tanh(prediction[i]);
    }
    
    return prediction;
}

} // namespace nlm
