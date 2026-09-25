#include "SensoryProcessor.hpp"
#include "../core/Constants.hpp"

namespace nlm {

SensoryProcessor::SensoryProcessor() : noveltyLevel_(0.0f), sensoryNoveltyDecay_(0.99f) {}

SensoryProcessor::~SensoryProcessor() {}

void SensoryProcessor::processVisionInput(
    const std::vector<Neuron*>& sensoryVision,
    const std::vector<float>& vision,
    float scaleFactor) {
    
    for (size_t i = 0; i < sensoryVision.size() && i < vision.size(); ++i) {
        if (sensoryVision[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * scaleFactor;
            sensoryVision[i]->injectCurrent(current);
        }
    }
}

void SensoryProcessor::processTouchInput(
    const std::vector<Neuron*>& sensoryTouch,
    const std::vector<float>& touch,
    float scaleFactor) {
    
    for (size_t i = 0; i < sensoryTouch.size() && i < touch.size(); ++i) {
        if (sensoryTouch[i]) {
            float current = touch[i] * scaleFactor;
            sensoryTouch[i]->injectCurrent(current);
        }
    }
}

void SensoryProcessor::processInternalInput(
    const std::vector<Neuron*>& sensoryInternal,
    const std::vector<float>& internal,
    float scaleFactor) {
    
    for (size_t i = 0; i < sensoryInternal.size() && i < internal.size(); ++i) {
        if (sensoryInternal[i]) {
            // Center and scale internal signals
            float current = (internal[i] * 2.0f - 1.0f) * scaleFactor;
            sensoryInternal[i]->injectCurrent(current);
        }
    }
}

void SensoryProcessor::processProprioceptionInput(
    const std::vector<Neuron*>& sensoryProprioception,
    const std::vector<float>& proprio,
    float scaleFactor) {
    
    for (size_t i = 0; i < sensoryProprioception.size() && i < proprio.size(); ++i) {
        if (sensoryProprioception[i]) {
            // Center and scale proprioception signals
            float current = (proprio[i] * 2.0f - 1.0f) * scaleFactor;
            sensoryProprioception[i]->injectCurrent(current);
        }
    }
}

float SensoryProcessor::computeNovelty(const std::vector<float>& vision,
                                    const std::vector<float>& previousVision) {
    if (vision.empty()) return 0.0f;
    
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size() && i < previousVision.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision[i]);
        totalDiff += diff;
    }
    
    // Normalize
    float novelty = totalDiff / std::max<size_t>(vision.size(), 1);
    
    // Apply decay and store
    noveltyLevel_ = novelty * sensoryNoveltyDecay_;
    
    return noveltyLevel_;
}

float SensoryProcessor::updateCuriosity(float novelty, float predictionError,
                                       float decayFactor, float errorWeight) {
    // Update curiosity based on novelty and prediction error
    float curiosity = novelty * decayFactor + std::abs(predictionError) * errorWeight;
    noveltyLevel_ = std::clamp(curiosity, 0.0f, 1.0f);
    return noveltyLevel_;
}

} // namespace nlm