#include "SensoryProcessor.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

SensoryProcessor::SensoryProcessor(float noveltyDecay)
    : noveltyLevel_(0.0f)
    , curiosityLevel_(0.0f)
    , curiosityEnabled_(true)
    , curiosityGain_(2.0f)
    , noveltyDecay_(noveltyDecay)
{
}

void SensoryProcessor::processInput(const SensoryPercept& percept,
                                    const std::vector<Neuron*>& visionNeurons,
                                    const std::vector<Neuron*>& touchNeurons,
                                    const std::vector<Neuron*>& internalNeurons,
                                    const std::vector<Neuron*>& proprioceptionNeurons,
                                    std::vector<float>& previousVision,
                                    float predictionError) {
    // Vision input processing
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < visionNeurons.size() && i < vision.size(); ++i) {
        if (visionNeurons[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            visionNeurons[i]->injectCurrent(current);
        }
    }
    
    // Touch input processing
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < touchNeurons.size() && i < touch.size(); ++i) {
        if (touchNeurons[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            touchNeurons[i]->injectCurrent(current);
        }
    }
    
    // Internal signals processing
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < internalNeurons.size() && i < intern.size(); ++i) {
        if (internalNeurons[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            internalNeurons[i]->injectCurrent(current);
        }
    }
    
    // Proprioception processing
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < proprioceptionNeurons.size() && i < proprio.size(); ++i) {
        if (proprioceptionNeurons[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            proprioceptionNeurons[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < previousVision.size(); ++i) {
            float diff = std::abs(vision[i] - previousVision[i]);
            totalDiff += diff;
        }
        
        // Normalize
        noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        noveltyLevel_ *= noveltyDecay_;
        
        // Store for next time
        previousVision = vision;
    }
    
    // Update curiosity based on novelty
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * curiosityGain_ + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

} // namespace nlm
