#include "Vision.hpp"
#include "../brain/Brain.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../brain/NeuralPopulation.hpp"
#include "../brain/Neuron.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

struct VisionProcessor::Impl {
    std::vector<float> features;
    size_t featureDimensions;
    std::shared_ptr<Brain> brain;
    RegionId visualRegionId;
    PopulationId edgeDetectorPopId;
    PopulationId orientationPopId;
    PopulationId spatialFreqPopId;
    size_t lastInputWidth;
    size_t lastInputHeight;
    size_t lastInputChannels;
    
    Impl() : featureDimensions(0), visualRegionId(), edgeDetectorPopId(), orientationPopId(), spatialFreqPopId(), lastInputWidth(0), lastInputHeight(0), lastInputChannels(0) {}
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {
    // Initialize brain for neural vision processing
    auto config = std::make_shared<Config>();
    pImpl->brain = std::make_shared<Brain>(config);
    
    // Initialize brain with small neural population for vision
    if (pImpl->brain) {
        pImpl->brain->initialize();
        
        // Create visual processing region (V1-like)
        pImpl->visualRegionId = pImpl->brain->addRegion("Visual_Cortex");
        
        // Create specialized populations for different visual features
        // Edge detectors (simple cells) - 200 neurons
        pImpl->edgeDetectorPopId = pImpl->brain->getRegion(pImpl->visualRegionId)->addPopulation(200, NeuronType::Internal);
        // Orientation detectors (complex cells) - 100 neurons  
        pImpl->orientationPopId = pImpl->brain->getRegion(pImpl->visualRegionId)->addPopulation(100, NeuronType::Internal);
        // Spatial frequency detectors - 100 neurons
        pImpl->spatialFreqPopId = pImpl->brain->getRegion(pImpl->visualRegionId)->addPopulation(100, NeuronType::Internal);
        
        // Enable plasticity for all populations
        auto* edgePop = pImpl->brain->getRegion(pImpl->visualRegionId)->getPopulation(pImpl->edgeDetectorPopId);
        auto* orientPop = pImpl->brain->getRegion(pImpl->visualRegionId)->getPopulation(pImpl->orientationPopId);
        auto* freqPop = pImpl->brain->getRegion(pImpl->visualRegionId)->getPopulation(pImpl->spatialFreqPopId);
        
        if (edgePop) edgePop->initializeRandom(*pImpl->brain->getRandomGenerator());
        if (orientPop) orientPop->initializeRandom(*pImpl->brain->getRandomGenerator());
        if (freqPop) freqPop->initializeRandom(*pImpl->brain->getRandomGenerator());
        
        // Configure development stage to initial (high plasticity)
        pImpl->brain->setDevelopmentalStage(DevelopmentalStage::Initial);
        
        // Configure plasticity rules
        if (auto* stdp = pImpl->brain->getSTDP()) {
            stdp->configure(0.01f, 0.012f, 20.0f);
        }
        if (auto* hebbian = pImpl->brain->getHebbian()) {
            hebbian->setLearningRate(0.001f);
            hebbian->setMaxWeight(2.0f);
        }
    }
}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    const auto& data = input.getData();
    if (data.empty() || !pImpl->brain) {
        pImpl->features.clear();
        pImpl->featureDimensions = 0;
        return;
    }
    
    // Store input dimensions for feature calculation
    pImpl->lastInputWidth = input.getWidth();
    pImpl->lastInputHeight = input.getHeight();
    pImpl->lastInputChannels = input.getChannels();
    
    // Calculate feature dimensions based on neural population sizes
    pImpl->featureDimensions = 300; // 200 + 100 + 100 neurons
    pImpl->features.resize(pImpl->featureDimensions, 0.0f);
    
    // Process visual input through neural populations
    processThroughNeuralPopulations(data, input.getWidth(), input.getHeight(), input.getChannels());
    
    // Apply lateral inhibition for contrast enhancement
    applyLateralInhibition();
    
    // Apply Hebbian learning for pattern detection
    applyHebbianLearning(data);
    
    // Apply STDP for spike-timing dependent plasticity
    applySTDPLearning();
    
    // Integrate development system effects
    integrateDevelopmentEffects();
}

void VisionProcessor::processThroughNeuralPopulations(const std::vector<float>& data, 
                                                      size_t width, size_t height, size_t channels) {
    // Create visual receptive fields (center-surround organization)
    const float PI = 3.14159f;
    
    // Process for edge detectors (V1 simple cells)
    processEdgeDetectors(data, width, height, channels);
    
    // Process for orientation detectors (V1 complex cells)  
    processOrientationDetectors(data, width, height, channels);
    
    // Process for spatial frequency detectors
    processSpatialFreqDetectors(data, width, height, channels);
}

void VisionProcessor::processEdgeDetectors(const std::vector<float>& data, 
                                          size_t width, size_t height, size_t channels) {
    auto* region = pImpl->brain->getRegion(pImpl->visualRegionId);
    if (!region) return;
    
    auto* edgePop = region->getPopulation(pImpl->edgeDetectorPopId);
    if (!edgePop) return;
    
    size_t numNeurons = edgePop->getSize();
    
    // Create center-surround receptive fields for edge detection
    for (size_t neuronIdx = 0; neuronIdx < numNeurons; ++neuronIdx) {
        Neuron* neuron = edgePop->getNeuron(neuronIdx);
        if (!neuron) continue;
        
        // Compute neuron position in population (simulates cortical layout)
        float x = static_cast<float>(neuronIdx % 20) / 20.0f * width;
        float y = static_cast<float>(neuronIdx / 20) / 20.0f * height;
        
        // Center-surround receptive field (difference of Gaussians)
        float centerSurroundResponse = 0.0f;
        float centerSum = 0.0f;
        float surroundSum = 0.0f;
        
        // Sample input around receptive field center
        int radius = 3;
        for (int dy = -radius; dy <= radius; ++dy) {
            for (int dx = -radius; dx <= radius; ++dx) {
                int sampleX = static_cast<int>(x + dx);
                int sampleY = static_cast<int>(y + dy);
                
                if (sampleX >= 0 && sampleX < static_cast<int>(width) && 
                    sampleY >= 0 && sampleY < static_cast<int>(height)) {
                    
                    float distance = std::sqrt(static_cast<float>(dx*dx + dy*dy));
                    float gaussian = std::exp(-(distance * distance) / (2.0f * 2.0f)); // Sigma=2.0
                    
                    // Center-Gaussian (weight = 1.0)
                    if (distance < 1.0f) {
                        centerSum += gaussian;
                    } else {
                        surroundSum += gaussian;
                    }
                    
                    // Sample input with different weights for center vs surround
                    float weight = 1.0f;
                    if (distance < 1.0f) {
                        weight = 1.0f; // Center enhancement
                    } else {
                        weight = -0.3f; // Surround suppression (center-surround)
                    }
                    
                    // Extract color channel information
                    float pixelValue = 0.0f;
                    for (size_t c = 0; c < channels; ++c) {
                        size_t idx = ((sampleY * width + sampleX) * channels + c);
                        pixelValue += data[idx];
                    }
                    pixelValue /= static_cast<float>(channels);
                    
                    centerSurroundResponse += weight * pixelValue * gaussian;
                }
            }
        }
        
        // Apply nonlinear transformation (like simple cell response)
        float response = std::tanhf(centerSurroundResponse * 2.0f);
        
        // Convert to firing rate (Hz)
        float firingRate = 30.0f * (response + 1.0f) / 2.0f; // 0-60 Hz
        
        // Store feature response in edge detector feature vector
        size_t featureIdx = neuronIdx;
        if (featureIdx < pImpl->featureDimensions) {
            pImpl->features[featureIdx] = firingRate / 60.0f; // Normalize to [0,1]
        }
    }
}

void VisionProcessor::processOrientationDetectors(const std::vector<float>& data,
                                                 size_t width, size_t height, size_t channels) {
    auto* region = pImpl->brain->getRegion(pImpl->visualRegionId);
    if (!region) return;
    
    auto* orientPop = region->getPopulation(pImpl->orientationPopId);
    if (!orientPop) return;
    
    size_t numNeurons = orientPop->getSize();
    
    // Orientation tuning (8 orientations)
    for (size_t neuronIdx = 0; neuronIdx < numNeurons; ++neuronIdx) {
        Neuron* neuron = orientPop->getNeuron(neuronIdx);
        if (!neuron) continue;
        
        // Assign orientation preference (0°, 45°, 90°, 135°, etc.)
        float orientation = static_cast<float>(neuronIdx % 8) * (PI / 4.0f);
        
        float orientationResponse = 0.0f;
        int radius = 4;
        
        // Gabor-like filter for orientation selectivity
        for (int y = -radius; y <= radius; ++y) {
            for (int x = -radius; x <= radius; ++x) {
                int sampleX = static_cast<int>(width/2 + x);
                int sampleY = static_cast<int>(height/2 + y);
                
                if (sampleX >= 0 && sampleX < static_cast<int>(width) && 
                    sampleY >= 0 && sampleY < static_cast<int>(height)) {
                    
                    // Gabor filter response
                    float xRel = static_cast<float>(x) / 2.0f;
                    float yRel = static_cast<float>(y) / 2.0f;
                    
                    // Cosine grating component
                    float cosComponent = std::cos(2.0f * xRel * std::cos(orientation) + 
                                                 2.0f * yRel * std::sin(orientation));
                    
                    // Gaussian envelope
                    float gaussian = std::exp(-(xRel*xRel + yRel*yRel) / 2.0f);
                    
                    // Sample input
                    float pixelValue = 0.0f;
                    for (size_t c = 0; c < channels; ++c) {
                        size_t idx = ((sampleY * width + sampleX) * channels + c);
                        pixelValue += data[idx];
                    }
                    pixelValue /= static_cast<float>(channels);
                    
                    orientationResponse += cosComponent * gaussian * pixelValue;
                }
            }
        }
        
        // Nonlinear response with orientation tuning
        float response = std::tanhf(orientationResponse * 1.5f);
        
        // Store in feature vector (after edge detectors)
        size_t featureIdx = 200 + (neuronIdx % 100); // 200-299 range
        if (featureIdx < pImpl->featureDimensions) {
            pImpl->features[featureIdx] = (response + 1.0f) / 2.0f; // Normalize
        }
    }
}

void VisionProcessor::processSpatialFreqDetectors(const std::vector<float>& data,
                                                 size_t width, size_t height, size_t channels) {
    auto* region = pImpl->brain->getRegion(pImpl->visualRegionId);
    if (!region) return;
    
    auto* freqPop = region->getPopulation(pImpl->spatialFreqPopId);
    if (!freqPop) return;
    
    size_t numNeurons = freqPop->getSize();
    
    // Spatial frequency tuning (low to high frequency)
    for (size_t neuronIdx = 0; neuronIdx < numNeurons; ++neuronIdx) {
        Neuron* neuron = freqPop->getNeuron(neuronIdx);
        if (!neuron) continue;
        
        // Spatial frequency preference (cycles per image width)
        float spatialFreq = 1.0f + static_cast<float>(neuronIdx % 9); // 1-10 cycles/width
        
        float spatialFreqResponse = 0.0f;
        int radius = 5;
        
        // Spatial frequency filter
        for (int y = -radius; y <= radius; ++y) {
            for (int x = -radius; x <= radius; ++x) {
                int sampleX = static_cast<int>(width/2 + x);
                int sampleY = static_cast<int>(height/2 + y);
                
                if (sampleX >= 0 && sampleX < static_cast<int>(width) && 
                    sampleY >= 0 && sampleY < static_cast<int>(height)) {
                    
                    // Spatial frequency modulation
                    float xPos = static_cast<float>(sampleX) / static_cast<float>(width);
                    float yPos = static_cast<float>(sampleY) / static_cast<float>(height);
                    
                    // Sine wave at specific spatial frequency
                    float freqComponent = std::sin(2.0f * PI * spatialFreq * xPos);
                    
                    // Gaussian envelope
                    float xDist = static_cast<float>(x) / 2.0f;
                    float yDist = static_cast<float>(y) / 2.0f;
                    float gaussian = std::exp(-(xDist*xDist + yDist*yDist) / 2.0f);
                    
                    // Sample input
                    float pixelValue = 0.0f;
                    for (size_t c = 0; c < channels; ++c) {
                        size_t idx = ((sampleY * width + sampleX) * channels + c);
                        pixelValue += data[idx];
                    }
                    pixelValue /= static_cast<float>(channels);
                    
                    spatialFreqResponse += freqComponent * gaussian * pixelValue;
                }
            }
        }
        
        // Nonlinear response with frequency tuning
        float response = std::tanhf(spatialFreqResponse * 1.0f);
        
        // Store in feature vector (after orientation detectors)
        size_t featureIdx = 300 + neuronIdx; // 300-399 range
        if (featureIdx < pImpl->featureDimensions) {
            pImpl->features[featureIdx] = (response + 1.0f) / 2.0f; // Normalize
        }
    }
}

void VisionProcessor::applyLateralInhibition() {
    // Contrast enhancement through lateral inhibition
    // Simulated by mutual competition between nearby feature channels
    
    const size_t inhibitionRadius = 10;
    
    // Apply lateral inhibition to edge detector features
    for (size_t i = 0; i < 200 && i + inhibitionRadius < pImpl->featureDimensions; ++i) {
        float inhibition = 0.0f;
        
        // Sum activity from neighboring neurons (lateral inhibition)
        for (size_t j = std::max(0ULL, i - inhibitionRadius); 
             j < std::min(pImpl->featureDimensions, i + inhibitionRadius + 1); ++j) {
            if (i != j) {
                float distance = std::abs(static_cast<int>(i) - static_cast<int>(j));
                float inhibitionStrength = 0.5f / (1.0f + distance);
                inhibition += pImpl->features[j] * inhibitionStrength;
            }
        }
        
        // Apply contrast gain control
        pImpl->features[i] = std::tanhf(pImpl->features[i] - inhibition * 0.5f);
    }
    
    // Apply to other feature groups
    for (size_t groupStart = 200; groupStart < pImpl->featureDimensions; groupStart += 100) {
        for (size_t i = groupStart; i < groupStart + 50 && i < pImpl->featureDimensions; ++i) {
            float inhibition = 0.0f;
            
            for (size_t j = std::max(groupStart, i - 5); j < std::min(pImpl->featureDimensions, i + 6); ++j) {
                if (i != j) {
                    inhibition += pImpl->features[j] * 0.1f;
                }
            }
            
            pImpl->features[i] = std::tanhf(pImpl->features[i] - inhibition);
        }
    }
}

void VisionProcessor::applyHebbianLearning(const std::vector<float>& data) {
    if (!pImpl->brain || !pImpl->brain->getHebbian()) return;
    
    auto* region = pImpl->brain->getRegion(pImpl->visualRegionId);
    if (!region) return;
    
    // Get current neural activity (simplified - use features as proxy)
    float currentActivity = 0.0f;
    for (float feature : pImpl->features) {
        currentActivity += feature;
    }
    currentActivity /= static_cast<float>(pImpl->features.size());
    
    // Apply Hebbian learning based on visual input correlation
    // This strengthens connections that respond to similar visual patterns
    
    // Simulated Hebbian learning on visual population connections
    // In a real implementation, this would modify actual synaptic weights
    
    for (size_t i = 0; i < pImpl->featureDimensions; ++i) {
        // Activity-dependent learning rate
        float learningRate = 0.001f * currentActivity * pImpl->features[i];
        
        // Feature potentiation based on visual input strength
        float inputResponse = std::tanhf(currentActivity * 2.0f);
        
        // Apply weight change to feature representation
        pImpl->features[i] += learningRate * inputResponse * (1.0f - pImpl->features[i]);
    }
}

void VisionProcessor::applySTDPLearning() {
    if (!pImpl->brain || !pImpl->brain->getSTDP()) return;
    
    // Apply STDP learning to visual processing neurons
    // In real implementation, this would use actual spike timing data
    
    auto* region = pImpl->brain->getRegion(pImpl->visualRegionId);
    if (!region) return;
    
    // Simplified STDP-like weight updates based on feature correlations
    for (size_t i = 0; i < pImpl->featureDimensions - 1; ++i) {
        for (size_t j = i + 1; j < pImpl->featureDimensions; ++j) {
            // Simulate spike timing correlation
            float correlation = std::abs(pImpl->features[i] - pImpl->features[j]);
            
            // If features are similarly active (pre-post spike correlation)
            if (correlation < 0.5f) {
                // Long-term potentiation
                float ltp = 0.001f * (1.0f - correlation);
                pImpl->features[i] += ltp;
                pImpl->features[j] += ltp;
            } else {
                // Long-term depression  
                float ltd = 0.0005f * (correlation - 0.5f);
                pImpl->features[i] -= ltd;
                pImpl->features[j] -= ltd;
            }
        }
    }
    
    // Clamp feature values
    for (float& feature : pImpl->features) {
        feature = std::max(0.0f, std::min(1.0f, feature));
    }
}

void VisionProcessor::integrateDevelopmentEffects() {
    if (!pImpl->brain) return;
    
    // Get developmental stage and plasticity modifier
    DevelopmentalStage stage = pImpl->brain->getDevelopmentalStage();
    
    // Apply developmental stage effects on visual processing
    switch (stage) {
        case DevelopmentalStage::Initial:
            // High plasticity, exploratory feature detection
            for (float& feature : pImpl->features) {
                feature *= 1.2f;  // Enhanced sensitivity
            }
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // Fine-tuning of visual representations
            for (float& feature : pImpl->features) {
                feature *= 0.9f;  // Stabilization
            }
            break;
            
        case DevelopmentalStage::Maturation:
            // Mature visual processing with optimized feature extraction
            // Minimal changes
            break;
            
        case DevelopmentalStage::Adult:
            // Stable visual representations
            for (float& feature : pImpl->features) {
                feature *= 0.95f;  // Slight decay
            }
            break;
            
        case DevelopmentalStage::Aging:
            // Reduced visual acuity
            for (float& feature : pImpl->features) {
                feature *= 0.8f;  // Reduced sensitivity
            }
            break;
    }
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

} // namespace nlm
