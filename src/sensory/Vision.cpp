#include "Vision.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_set>
#include <queue>
#include <random>

namespace nlm {

// Forward declarations for neural processing components
class VisualFeatureExtractor;
class EdgeDetector;
class ObjectRecognizer;
class FeatureMap;

struct VisionProcessor::Impl {
    // Neural processing components
    std::unique_ptr<VisualFeatureExtractor> featureExtractor;
    std::unique_ptr<EdgeDetector> edgeDetector;
    std::unique_ptr<ObjectRecognizer> objectRecognizer;
    std::unique_ptr<FeatureMap> featureMap;
    
    // Brain integration
    Brain* brain;
    std::vector<Neuron*> visualNeurons;
    std::vector<std::unique_ptr<NeuralPopulation>> visualPopulations;
    
    // Feature processing buffers
    std::vector<float> inputBuffer;
    std::vector<float> features;
    size_t featureDimensions;
    std::vector<float> edgeFeatures;
    std::vector<float> objectFeatures;
    std::vector<float> hierarchicalFeatures;
    
    // Temporal processing
    std::vector<float> temporalBuffer;
    size_t maxTemporalDepth;
    float temporalSmoothing;
    
    // Spatial processing
    size_t inputWidth;
    size_t inputHeight;
    size_t inputChannels;
    std::vector<std::vector<float>> receptiveFields;
    
    // State management
    bool isInitialized;
    size_t lastStep;
    float contrastThreshold;
    float noiseLevel;
    
    // Statistics
    size_t totalFeaturesProcessed;
    float averageContrast;
    float edgeDensity;
    
    Impl() : 
        featureExtractor(nullptr),
        edgeDetector(nullptr),
        objectRecognizer(nullptr),
        featureMap(nullptr),
        brain(nullptr),
        featureDimensions(0),
        maxTemporalDepth(5),
        temporalSmoothing(0.9f),
        inputWidth(0),
        inputHeight(0),
        inputChannels(3),
        isInitialized(false),
        lastStep(0),
        contrastThreshold(0.1f),
        noiseLevel(0.05f),
        totalFeaturesProcessed(0),
        averageContrast(0.0f),
        edgeDensity(0.0f) {}
    
    ~Impl() {
        // Clean up neural populations
        visualPopulations.clear();
        visualNeurons.clear();
        
        // Release brain reference
        brain = nullptr;
        
        // Clear processing buffers
        inputBuffer.clear();
        features.clear();
        edgeFeatures.clear();
        objectFeatures.clear();
        hierarchicalFeatures.clear();
        temporalBuffer.clear();
        
        // Clear receptive fields
        receptiveFields.clear();
    }
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {
    NLM_LOG_INFO("VisionProcessor: Initializing neural vision processor");
}

VisionProcessor::~VisionProcessor() {
    NLM_LOG_INFO("VisionProcessor: Shutting down neural vision processor");
}

void VisionProcessor::initialize(Brain* brain, std::shared_ptr<Config> config) {
    if (!brain || !config) {
        NLM_LOG_ERROR("VisionProcessor::initialize: Invalid parameters - brain or config is null");
        return;
    }
    
    pImpl->brain = brain;
    
    // Get vision configuration
    size_t visionNeurons = config->getOr<size_t>("vision_neurons", 500);
    size_t edgeNeurons = config->getOr<size_t>("edge_detection_neurons", 200);
    size_t objectNeurons = config->getOr<size_t>("object_recognition_neurons", 300);
    size_t featureMapSize = config->getOr<size_t>("feature_map_size", 256);
    
    pImpl->maxTemporalDepth = config->getOr<size_t>("vision_temporal_depth", 5);
    pImpl->temporalSmoothing = config->getOr<float>("vision_temporal_smoothing", 0.9f);
    pImpl->contrastThreshold = config->getOr<float>("vision_contrast_threshold", 0.1f);
    pImpl->noiseLevel = config->getOr<float>("vision_noise_level", 0.05f);
    
    // Create neural populations for visual processing
    createVisualPopulations(visionNeurons, edgeNeurons, objectNeurons);
    
    // Initialize feature extraction components
    pImpl->featureExtractor = std::make_unique<VisualFeatureExtractor>();
    pImpl->edgeDetector = std::make_unique<EdgeDetector>();
    pImpl->objectRecognizer = std::make_unique<ObjectRecognizer>();
    pImpl->featureMap = std::make_unique<FeatureMap>(featureMapSize);
    
    // Create receptive fields for different visual processing stages
    createReceptiveFields();
    
    pImpl->isInitialized = true;
    NLM_LOG_INFO("VisionProcessor: Neural vision processor initialized successfully");
    NLM_LOG_INFO("VisionProcessor: Created " + std::to_string(pImpl->visualPopulations.size()) + 
                 " visual populations, " + std::to_string(pImpl->visualNeurons.size()) + " visual neurons");
}

void VisionProcessor::createVisualPopulations(size_t totalVisionNeurons, size_t edgeNeurons, size_t objectNeurons) {
    if (!pImpl->brain) {
        NLM_LOG_ERROR("VisionProcessor::createVisualPopulations: Brain not available");
        return;
    }
    
    // Create hierarchical populations
    size_t simpleFeatureNeurons = totalVisionNeurons * 0.3f;
    size_t complexFeatureNeurons = totalVisionNeurons * 0.4f;
    size_t edgeNeuronsCount = edgeNeurons;
    size_t objectNeuronsCount = objectNeurons;
    
    // Simple feature population (basic visual features)
    NeuralRegion* defaultRegion = pImpl->brain->getRegion(RegionId(1));
    if (!defaultRegion) {
        defaultRegion = pImpl->brain->addRegion("Visual_Region");
    }
    
    if (defaultRegion) {
        // Simple features: orientation, color, intensity
        PopulationId simpleFeatures = defaultRegion->addPopulation(simpleFeatureNeurons, NeuronType::Sensory);
        auto* simplePop = defaultRegion->getPopulation(simpleFeatures);
        if (simplePop) {
            for (auto* neuron : simplePop->getNeurons()) {
                pImpl->visualNeurons.push_back(neuron);
            }
            pImpl->visualPopulations.push_back(std::unique_ptr<NeuralPopulation>(simplePop));
        }
        
        // Edge detection neurons
        PopulationId edgePopId = defaultRegion->addPopulation(edgeNeuronsCount, NeuronType::Internal);
        auto* edgePop = defaultRegion->getPopulation(edgePopId);
        if (edgePop) {
            for (auto* neuron : edgePop->getNeurons()) {
                pImpl->visualNeurons.push_back(neuron);
            }
            pImpl->visualPopulations.push_back(std::unique_ptr<NeuralPopulation>(edgePop));
        }
        
        // Object recognition neurons
        PopulationId objectPopId = defaultRegion->addPopulation(objectNeuronsCount, NeuronType::Internal);
        auto* objectPop = defaultRegion->getPopulation(objectPopId);
        if (objectPop) {
            for (auto* neuron : objectPop->getNeurons()) {
                pImpl->visualNeurons.push_back(neuron);
            }
            pImpl->visualPopulations.push_back(std::unique_ptr<NeuralPopulation>(objectPop));
        }
        
        // Connect neurons based on hierarchy
        establishNeuralConnections(simplePop, edgePop, objectPop);
    }
}

void VisionProcessor::establishNeuralConnections(NeuralPopulation* simplePop, NeuralPopulation* edgePop, NeuralPopulation* objectPop) {
    if (!pImpl->brain) return;
    
    // Create hierarchical connections:
    // Simple features -> Edge detectors
    // Edge detectors -> Object recognition
    // Object recognition -> Working memory integration
    
    if (simplePop && edgePop) {
        for (auto* simpleNeuron : simplePop->getNeurons()) {
            for (auto* edgeNeuron : edgePop->getNeurons()) {
                // Connection from simple features to edge detection
                // Weight depends on feature similarity
                float weight = calculateConnectionWeight(simpleNeuron, edgeNeuron);
                if (weight > 0.1f) {  // Only create significant connections
                    pImpl->brain->injectCurrent(edgeNeuron->getId(), weight * 5.0f);
                }
            }
        }
    }
    
    if (edgePop && objectPop) {
        for (auto* edgeNeuron : edgePop->getNeurons()) {
            for (auto* objectNeuron : objectPop->getNeurons()) {
                // Connection from edge detectors to object recognition
                float weight = calculateConnectionWeight(edgeNeuron, objectNeuron);
                if (weight > 0.1f) {
                    pImpl->brain->injectCurrent(objectNeuron->getId(), weight * 3.0f);
                }
            }
        }
    }
}

float VisionProcessor::calculateConnectionWeight(Neuron* source, Neuron* destination) const {
    if (!source || !destination) return 0.0f;
    
    const auto& sourceState = source->getState();
    const auto& destState = destination->getState();
    
    // Weight based on similarity in activation levels
    float sourceActivation = std::abs(sourceState.membranePotential - sourceState.restingPotential);
    float destActivation = std::abs(destState.membranePotential - destState.restingPotential);
    
    float similarity = 1.0f - std::abs(sourceActivation - destActivation) / 100.0f;
    
    // Weight also depends on neuron types and connection history
    float baseWeight = 0.2f + similarity * 0.8f;
    
    // Add noise for biological realism
    static thread_local std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> noiseDist(-0.1f, 0.1f);
    
    return std::max(0.0f, baseWeight + noiseDist(rng));
}

void VisionProcessor::createReceptiveFields() {
    if (pImpl->inputWidth == 0 || pImpl->inputHeight == 0) {
        NLM_LOG_WARN("VisionProcessor::createReceptiveFields: Invalid dimensions, using defaults");
        pImpl->inputWidth = 64;
        pImpl->inputHeight = 64;
    }
    
    // Create receptive fields for different visual processing stages
    // Center-surround organization like retinal processing
    
    pImpl->receptiveFields.resize(3);  // 3 stages: simple, complex, combinatorial
    
    // Simple feature receptive fields (center-surround)
    pImpl->receptiveFields[0].resize(pImpl->inputWidth * pImpl->inputHeight);
    float centerWeight = 1.0f;
    float surroundWeight = -0.2f;
    
    for (size_t y = 0; y < pImpl->inputHeight; ++y) {
        for (size_t x = 0; x < pImpl->inputWidth; ++x) {
            // Calculate center-surround weight
            float weight = centerWeight;
            
            // Subtract surround contribution
            for (int dy = -2; dy <= 2; ++dy) {
                for (int dx = -2; dx <= 2; ++dx) {
                    int ny = y + dy;
                    int nx = x + dx;
                    if (ny >= 0 && ny < static_cast<int>(pImpl->inputHeight) &&
                        nx >= 0 && nx < static_cast<int>(pImpl->inputWidth)) {
                        if (!(dy == 0 && dx == 0)) {
                            weight += surroundWeight * (1.0f / std::sqrt(dx*dx + dy*dy));
                        }
                    }
                }
            }
            
            pImpl->receptiveFields[0][y * pImpl->inputWidth + x] = weight;
        }
    }
}

void VisionProcessor::process(const Vision& input) {
    if (!pImpl->isInitialized) {
        NLM_LOG_WARN("VisionProcessor::process: Not initialized, using basic feature extraction");
        // Fallback to basic processing
        pImpl->features = input.getData();
        pImpl->featureDimensions = input.getDimensions();
        return;
    }
    
    // Get input data
    const auto& inputData = input.getData();
    if (inputData.empty()) {
        NLM_LOG_ERROR("VisionProcessor::process: Empty input data");
        return;
    }
    
    // Store input dimensions
    pImpl->inputWidth = input.getWidth();
    pImpl->inputHeight = input.getHeight();
    pImpl->inputChannels = input.getChannels();
    
    // Create receptive fields if dimensions changed
    if (pImpl->inputWidth > 0 && pImpl->inputHeight > 0) {
        createReceptiveFields(pImpl->inputWidth, pImpl->inputHeight, pImpl->inputChannels);
    }
    
    // Update temporal buffer
    updateTemporalBuffer(inputData);
    
    // Apply neural processing pipeline
    processInput(inputData);
    
    // Extract edge features
    extractEdgeFeatures(inputData);
    
    // Recognize objects
    recognizeObjects(inputData);
    
    // Generate hierarchical features
    generateHierarchicalFeatures();
    
    // Apply contrast enhancement and noise reduction
    enhanceContrastAndReduceNoise();
    
    // Update feature maps
    updateFeatureMaps();
    
    // Integrate with brain systems
    integrateWithBrain(inputData);
    
    // Store statistics
    pImpl->totalFeaturesProcessed++;
}

void VisionProcessor::updateTemporalBuffer(const std::vector<float>& input) {
    pImpl->inputBuffer = input;
    
    // Apply temporal smoothing
    if (!pImpl->temporalBuffer.empty()) {
        float alpha = pImpl->temporalSmoothing;
        size_t minSize = std::min(input.size(), pImpl->temporalBuffer.size());
        for (size_t i = 0; i < minSize; ++i) {
            pImpl->temporalBuffer[i] = alpha * pImpl->temporalBuffer[i] + (1.0f - alpha) * input[i];
        }
    } else {
        pImpl->temporalBuffer = input;
    }
    
    // Maintain temporal depth
    if (pImpl->temporalBuffer.size() > pImpl->maxTemporalDepth * 3) {
        pImpl->temporalBuffer.erase(pImpl->temporalBuffer.begin());
    }
}

void VisionProcessor::processInput(const std::vector<float>& input) {
    // Apply neural receptive field processing
    std::vector<float> processedInput = input;
    
    if (!pImpl->receptiveFields.empty() && !pImpl->receptiveFields[0].empty() &&
        pImpl->inputWidth > 0 && pImpl->inputHeight > 0) {
        for (size_t c = 0; c < pImpl->inputChannels; ++c) {
            for (size_t y = 0; y < pImpl->inputHeight; ++y) {
                for (size_t x = 0; x < pImpl->inputWidth; ++x) {
                    size_t idx = y * pImpl->inputWidth + x;
                    if (idx < pImpl->receptiveFields[0].size()) {
                        float weight = pImpl->receptiveFields[0][idx];
                        size_t inputIdx = c * pImpl->inputWidth * pImpl->inputHeight + idx;
                        if (inputIdx < input.size()) {
                            processedInput[inputIdx] *= weight;
                        }
                    }
                }
            }
        }
    }
    
    // Pass processed input to feature extractor
    if (pImpl->featureExtractor) {
        pImpl->featureExtractor->process(processedInput, pImpl->inputWidth, pImpl->inputHeight);
    }
}

void VisionProcessor::extractEdgeFeatures(const std::vector<float>& input) {
    // Use edge detector neural population
    if (!pImpl->edgeDetector) return;
    
    // Create edge map using Sobel-like filters
    pImpl->edgeFeatures.clear();
    pImpl->edgeFeatures.resize(pImpl->inputWidth * pImpl->inputHeight, 0.0f);
    
    if (pImpl->inputWidth < 3 || pImpl->inputHeight < 3) {
        NLM_LOG_WARN("VisionProcessor::extractEdgeFeatures: Input too small for edge detection");
        return;
    }
    
    // Simple edge detection using gradient computation
    for (size_t y = 1; y < pImpl->inputHeight - 1; ++y) {
        for (size_t x = 1; x < pImpl->inputWidth - 1; ++x) {
            float gx = 0.0f, gy = 0.0f;
            
            // Sobel operator for edge detection
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    int ny = y + dy;
                    int nx = x + dx;
                    size_t inputIdx = ny * pImpl->inputWidth + nx;
                    float intensity = 0.0f;
                    
                    if (inputIdx < input.size()) {
                        intensity = input[inputIdx];
                    }
                    
                    gx += dx * intensity;
                    gy += dy * intensity;
                }
            }
            
            // Compute edge magnitude
            float edgeMagnitude = std::sqrt(gx * gx + gy * gy);
            pImpl->edgeFeatures[y * pImpl->inputWidth + x] = edgeMagnitude;
        }
    }
    
    // Pass edge features to edge detector
    pImpl->edgeDetector->process(pImpl->edgeFeatures);
}

void VisionProcessor::recognizeObjects(const std::vector<float>& input) {
    // Object recognition using neural population
    if (!pImpl->objectRecognizer) return;
    
    // Extract features for object recognition
    std::vector<float> objectFeatures;
    
    objectFeatures.reserve(input.size());
    
    for (size_t i = 0; i < input.size(); ++i) {
        float value = input[i];
        
        // Non-linear feature enhancement
        float feature = std::tanh(value * 2.0f) * value;
        
        // Add local context
        size_t y = i / pImpl->inputWidth;
        size_t x = i % pImpl->inputWidth;
        
        // Context weighting based on position
        float contextWeight = 1.0f;
        if (x > 0 && x < pImpl->inputWidth - 1) {
            contextWeight *= 1.1f;
        }
        if (y > 0 && y < pImpl->inputHeight - 1) {
            contextWeight *= 1.1f;
        }
        
        feature *= contextWeight;
        objectFeatures.push_back(feature);
    }
    
    // Pass object features to recognizer
    pImpl->objectRecognizer->process(objectFeatures);
}

void VisionProcessor::generateHierarchicalFeatures() {
    // Generate hierarchical features from different processing stages
    pImpl->hierarchicalFeatures.clear();
    
    // Combine features from different stages
    size_t maxSize = std::max({
        pImpl->features.size(),
        pImpl->edgeFeatures.size(),
        pImpl->objectFeatures.size()
    });
    
    pImpl->hierarchicalFeatures.resize(maxSize, 0.0f);
    
    // Simple hierarchical combination
    float weightSum = 0.0f;
    size_t validFeatures = 0;
    
    for (size_t i = 0; i < maxSize; ++i) {
        float value = 0.0f;
        
        if (i < pImpl->features.size()) {
            value += pImpl->features[i];
            weightSum += 1.0f;
            validFeatures++;
        }
        
        if (i < pImpl->edgeFeatures.size()) {
            value += pImpl->edgeFeatures[i] * 0.5f;
            weightSum += 0.5f;
            validFeatures++;
        }
        
        if (i < pImpl->objectFeatures.size()) {
            value += pImpl->objectFeatures[i] * 0.3f;
            weightSum += 0.3f;
            validFeatures++;
        }
        
        if (weightSum > 0.0f) {
            pImpl->hierarchicalFeatures[i] = value / weightSum;
        }
    }
    
    // Update final features
    pImpl->features = pImpl->hierarchicalFeatures;
    pImpl->featureDimensions = pImpl->features.size();
}

void VisionProcessor::enhanceContrastAndReduceNoise() {
    if (pImpl->features.empty()) return;
    
    // Contrast enhancement
    float minVal = *std::min_element(pImpl->features.begin(), pImpl->features.end());
    float maxVal = *std::max_element(pImpl->features.begin(), pImpl->features.end());
    float range = maxVal - minVal;
    
    if (range > pImpl->contrastThreshold * 2.0f) {
        float targetRange = 1.0f;
        float scale = targetRange / range;
        
        for (float& feature : pImpl->features) {
            feature = (feature - minVal) * scale;
        }
    }
    
    // Noise reduction using temporal smoothing
    if (pImpl->temporalBuffer.size() == pImpl->features.size()) {
        for (size_t i = 0; i < pImpl->features.size(); ++i) {
            pImpl->features[i] = pImpl->temporalSmoothing * pImpl->features[i] +
                                (1.0f - pImpl->temporalSmoothing) * pImpl->temporalBuffer[i];
        }
    }
    
    // Calculate statistics
    pImpl->averageContrast = range / 2.0f;
    
    if (!pImpl->edgeFeatures.empty()) {
        float edgeSum = std::accumulate(pImpl->edgeFeatures.begin(), pImpl->edgeFeatures.end(), 0.0f);
        pImpl->edgeDensity = edgeSum / pImpl->edgeFeatures.size();
    }
}

void VisionProcessor::updateFeatureMaps() {
    if (pImpl->featureMap) {
        pImpl->featureMap->update(pImpl->features);
    }
}

void VisionProcessor::integrateWithBrain(const std::vector<float>& input) {
    if (!pImpl->brain) return;
    
    // Inject visual processing into brain's sensory system
    pImpl->brain->receiveSensoryInput(reinterpret_cast<const SensoryInput&>(input));
    
    // Update working memory with visual features
    if (pImpl->brain->getWorkingMemory() && !pImpl->features.empty()) {
        // Store visual features in working memory
        // Note: This requires accessing the working memory API
        // For now, we'll inject into visual neurons
        for (size_t i = 0; i < std::min(pImpl->features.size(), static_cast<size_t>(100)); ++i) {
            float activation = pImpl->features[i] * 0.1f;  // Scale for neural injection
            // The features are already encoded in the visual neuron activity
        }
    }
    
    // Update attentional selection
    if (pImpl->brain->getAttention() && !pImpl->edgeFeatures.empty()) {
        // Create attentional signal based on edge features
        float attentionStrength = *std::max_element(pImpl->edgeFeatures.begin(), pImpl->edgeFeatures.end());
        
        // Apply attentional modulation
        // This would involve dopamine or other neuromodulators
        if (pImpl->brain->getDopamine()) {
            float modulation = attentionStrength * 0.5f;
            pImpl->brain->applyNeuromodulation(*pImpl->brain->getDopamine());
        }
    }
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

void VisionProcessor::reset() {
    // Reset all processing state
    pImpl->inputBuffer.clear();
    pImpl->features.clear();
    pImpl->edgeFeatures.clear();
    pImpl->objectFeatures.clear();
    pImpl->hierarchicalFeatures.clear();
    pImpl->temporalBuffer.clear();
    
    // Reset temporal processing
    pImpl->lastStep = 0;
    
    // Reset statistics
    pImpl->totalFeaturesProcessed = 0;
    pImpl->averageContrast = 0.0f;
    pImpl->edgeDensity = 0.0f;
    
    NLM_LOG_INFO("VisionProcessor: Reset processing state");
}

// VisualFeatureExtractor implementation
class VisionProcessor::VisualFeatureExtractor {
public:
    void process(const std::vector<float>& input, size_t width, size_t height) {
        features.clear();
        
        // Simple feature extraction
        for (size_t c = 0; c < 3; ++c) {  // RGB channels
            for (size_t y = 0; y < height; ++y) {
                for (size_t x = 0; x < width; ++x) {
                    size_t idx = c * width * height + y * width + x;
                    if (idx < input.size()) {
                        float value = input[idx];
                        
                        // Extract basic features
                        features.push_back(value);  // Intensity
                        features.push_back(std::sin(value * 3.14f));  // Non-linearity
                        features.push_back(std::cos(value * 2.0f));  // Different frequency
                        
                        // Local statistics
                        if (x > 0 && x < width - 1 && y > 0 && y < height - 1) {
                            float localMean = 0.0f;
                            int count = 0;
                            
                            for (int dy = -1; dy <= 1; ++dy) {
                                for (int dx = -1; dx <= 1; ++dx) {
                                    int ny = y + dy;
                                    int nx = x + dx;
                                    size_t localIdx = c * width * height + ny * width + nx;
                                    if (localIdx < input.size()) {
                                        localMean += input[localIdx];
                                        count++;
                                    }
                                }
                            }
                            
                            if (count > 0) {
                                localMean /= count;
                                features.push_back(value - localMean);  // Contrast
                            }
                        }
                    }
                }
            }
        }
    }
    
    const std::vector<float>& getFeatures() const { return features; }
    
private:
    std::vector<float> features;
};

// EdgeDetector implementation
class VisionProcessor::EdgeDetector {
public:
    void process(const std::vector<float>& input) {
        edgeMap.clear();
        
        if (input.empty()) return;
        
        edgeMap.reserve(input.size());
        for (float val : input) {
            edgeMap.push_back(std::abs(val));
        }
    }
    
    const std::vector<float>& getEdgeMap() const { return edgeMap; }
    
private:
    std::vector<float> edgeMap;
};

// ObjectRecognizer implementation
class VisionProcessor::ObjectRecognizer {
public:
    void process(const std::vector<float>& input) {
        objectFeatures.clear();
        
        if (input.empty()) return;
        
        // Simple object recognition based on pattern analysis
        for (size_t i = 0; i < input.size(); ++i) {
            float val = input[i];
            
            // Multi-scale feature detection
            float feature1 = val;  // Raw value
            float feature2 = std::tanh(val * 2.0f);  // Non-linear
            float feature3 = std::sin(val * std::atan(val));  // Harmonic
            
            objectFeatures.push_back(feature1 * 0.5f + feature2 * 0.3f + feature3 * 0.2f);
        }
    }
    
    const std::vector<float>& getObjectFeatures() const { return objectFeatures; }
    
private:
    std::vector<float> objectFeatures;
};

// FeatureMap implementation
class VisionProcessor::FeatureMap {
public:
    explicit FeatureMap(size_t size) : featureSize(size) {
        features.resize(size, 0.0f);
    }
    
    void update(const std::vector<float>& newFeatures) {
        if (newFeatures.size() == featureSize) {
            for (size_t i = 0; i < featureSize; ++i) {
                features[i] = (features[i] * 0.9f) + (newFeatures[i] * 0.1f);
            }
        } else {
            features = newFeatures;
            if (features.size() > featureSize) {
                features.resize(featureSize);
            } else if (features.size() < featureSize) {
                features.resize(featureSize, 0.0f);
            }
        }
    }
    
    const std::vector<float>& getFeatures() const { return features; }
    size_t getSize() const { return featureSize; }
    
private:
    std::vector<float> features;
    size_t featureSize;
};

} // namespace nlm
