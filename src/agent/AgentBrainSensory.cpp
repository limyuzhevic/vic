// AgentBrain sensory processing module
// Handles sensory input processing and injection

#include "AgentBrainSensory.hpp"
#include "SensoryPercept.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

void AgentBrainSensory::processVisionInput(
    const std::vector<float>& vision,
    const std::vector<Neuron*>& visionNeurons,
    float& visionMean,
    std::vector<float>& lastVision) {
    
    for (size_t i = 0; i < visionNeurons.size() && i < vision.size(); ++i) {
        if (visionNeurons[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            visionNeurons[i]->injectCurrent(current);
        }
    }
}

void AgentBrainSensory::processTouchInput(
    const std::vector<float>& touch,
    float& maxTouch,
    float& forwardTouch,
    std::vector<float>& lastTouch) {
    
    maxTouch = 0.0f;
    for (float t : touch) maxTouch = std::max(maxTouch, t);
    
    forwardTouch = 0.0f;
    if (touch.size() >= 8) {
        forwardTouch = std::max({touch[0], touch[1], touch[7]});
    } else {
        forwardTouch = maxTouch;
    }
}

void AgentBrainSensory::processTextInput(
    const std::string& text,
    const std::function<std::vector<size_t>(const std::string&)>& encodeText,
    const std::vector<Neuron*>& visionNeurons) {
    
    if (!text.empty()) {
        for (size_t idx : encodeText(text)) {
            size_t neuronIdx = 16 + idx;
            if (neuronIdx < visionNeurons.size() && visionNeurons[neuronIdx]) {
                visionNeurons[neuronIdx]->injectCurrent(8.0f);
            }
        }
    }
}

void AgentBrainSensory::updateNoveltyAndPrediction(
    const std::vector<float>& vision,
    const std::vector<float>& touch,
    float& noveltyLevel,
    float& forwardTouch,
    float& forwardObjectVision,
    std::vector<float>& predictedVision,
    float sensoryNoveltyDecay) {
    
    float visionMean = 0.0f;
    for (float v : vision) visionMean += v;
    if (!vision.empty()) visionMean /= static_cast<float>(vision.size());
    
    // Calculate novelty as difference from predicted vision
    if (!predictedVision.empty() && predictedVision.size() == vision.size()) {
        float sumDiff = 0.0f;
        for (size_t i = 0; i < vision.size(); ++i) {
            sumDiff += std::abs(vision[i] - predictedVision[i]);
        }
        noveltyLevel = sumDiff / static_cast<float>(vision.size());
    } else {
        noveltyLevel = 1.0f; // Maximum novelty if no prediction
    }
    
    // Decay prediction
    for (float& v : predictedVision) {
        v *= sensoryNoveltyDecay;
    }
}

void AgentBrainSensory::updatePrediction(const std::vector<float>& vision,
                                         std::vector<float>& predictedVision,
                                         float decayRate) {
    // Gradually approach current vision (moving average)
    if (predictedVision.empty()) {
        predictedVision = vision;
    } else {
        for (size_t i = 0; i < vision.size() && i < predictedVision.size(); ++i) {
            predictedVision[i] = predictedVision[i] * (1.0f - decayRate) + vision[i] * decayRate;
        }
    }
}

} // namespace nlm
