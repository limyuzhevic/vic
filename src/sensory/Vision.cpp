#include "Vision.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct VisionProcessor::Impl {
    // Neural vision representation
    std::vector<float> features;
    size_t featureDimensions;
    
    // Neural vision mechanisms
    float attentionGain;        // Attention modulation of visual processing
    float predictiveCodingGain; // Top-down predictive coding influence
    float noveltySensitivity;   // Sensitivity to novel visual patterns
    float learningRate;         // Vision learning rate
    
    // Historical tracking for prediction
    std::vector<float> recentFeatures;
    std::vector<float> predictionError;
    
    // Population coding for visual objects
    std::vector<float> populationCodes;
    std::vector<NeuronId> activeVisualNeurons;
    
    Impl() : featureDimensions(0), attentionGain(1.0f), 
             predictiveCodingGain(0.5f), noveltySensitivity(0.8f),
             learningRate(0.01f) {}
};

VisionProcessor::VisionProcessor() : pImpl(new Impl) {}

VisionProcessor::~VisionProcessor() = default;

void VisionProcessor::process(const Vision& input) {
    // Extract raw visual data
    pImpl->features = input.getData();
    pImpl->featureDimensions = input.getDimensions();
    
    // Apply neural vision processing
    processWithNeuralMachinery(input);
    
    // Update population codes
    updatePopulationCodes();
}

void VisionProcessor::processWithNeuralMachinery(const Vision& input) {
    // Convert visual input to neural activity patterns
    std::vector<float> neuralResponse;
    neuralResponse.reserve(pImpl->featureDimensions);
    
    for (size_t i = 0; i < pImpl->featureDimensions; ++i) {
        if (i < input.getData().size()) {
            float visualInput = input.getData()[i];
            
            // Apply attention modulation
            float attendedInput = visualInput * pImpl->attentionGain;
            
            // Apply predictive coding (compare with prediction)
            float prediction = getPrediction(i);
            float predictionError = attendedInput - prediction;
            pImpl->predictionError[i] = predictionError;
            
            // Apply predictive coding gain
            float enhancedResponse = attendedInput + pImpl->predictiveCodingGain * predictionError;
            
            // Apply non-linear activation (sigmoid-like)
            float activation = 1.0f / (1.0f + std::exp(-enhancedResponse * 5.0f));
            
            // Apply novelty modulation
            float novelty = std::abs(predictionError) * pImpl->noveltySensitivity;
            float finalActivation = activation * (1.0f + novelty * 0.2f);
            
            neuralResponse.push_back(finalActivation);
        }
    }
    
    // Store enhanced response
    pImpl->features = neuralResponse;
    
    // Store in history for next prediction
    pImpl->recentFeatures.push_back(visualInput);
    if (pImpl->recentFeatures.size() > 100) {
        pImpl->recentFeatures.erase(pImpl->recentFeatures.begin());
    }
}

float VisionProcessor::getPrediction(size_t index) const {
    if (pImpl->recentFeatures.empty()) {
        return 0.5f; // Default neutral prediction
    }
    
    // Use oldest entry for temporal prediction
    float oldest = pImpl->recentFeatures[0];
    
    // Apply simple decay to older inputs
    float decayFactor = 0.9f;
    for (size_t i = 1; i < pImpl->recentFeatures.size(); ++i) {
        oldest = oldest * decayFactor + pImpl->recentFeatures[i] * (1.0f - decayFactor);
    }
    
    return oldest;
}

void VisionProcessor::updatePopulationCodes() {
    // Update population vector coding for object recognition
    // This simulates how visual cortex represents objects
    
    pImpl->populationCodes.clear();
    pImpl->activeVisualNeurons.clear();
    
    const size_t numPopulations = 16; // Number of population codes
    pImpl->populationCodes.resize(numPopulations, 0.0f);
    
    // Simple hierarchical population coding
    for (size_t i = 0; i < std::min(pImpl->featureDimensions, numPopulations); ++i) {
        // Each feature contributes to multiple populations
        for (size_t j = 0; j < 3; ++j) { // Receptive field overlap
            size_t popIdx = (i * 3 + j) % numPopulations;
            pImpl->populationCodes[popIdx] += pImpl->features[i] * (1.0f / 3.0f);
        }
    }
    
    // Normalize population codes
    float maxCode = *std::max_element(pImpl->populationCodes.begin(), 
                                     pImpl->populationCodes.end());
    if (maxCode > 0.0f) {
        for (auto& code : pImpl->populationCodes) {
            code /= maxCode;
        }
    }
}

const std::vector<float>& VisionProcessor::getFeatures() const {
    return pImpl->features;
}

size_t VisionProcessor::getFeatureDimensions() const {
    return pImpl->featureDimensions;
}

float VisionProcessor::getPredictionError(size_t index) const {
    if (index < pImpl->predictionError.size()) {
        return pImpl->predictionError[index];
    }
    return 0.0f;
}

const std::vector<float>& VisionProcessor::getPopulationCodes() const {
    return pImpl->populationCodes;
}

void VisionProcessor::updateAttention(float gain) {
    pImpl->attentionGain = gain;
}

void VisionProcessor::updatePredictiveCoding(float gain) {
    pImpl->predictiveCodingGain = gain;
}

void VisionProcessor::updateNoveltySensitivity(float sensitivity) {
    pImpl->noveltySensitivity = sensitivity;
}

void VisionProcessor::reset() {
    pImpl->features.clear();
    pImpl->predictionError.clear();
    pImpl->recentFeatures.clear();
    pImpl->populationCodes.clear();
    pImpl->activeVisualNeurons.clear();
    
    pImpl->attentionGain = 1.0f;
    pImpl->predictiveCodingGain = 0.5f;
    pImpl->noveltySensitivity = 0.8f;
}

} // namespace nlm
