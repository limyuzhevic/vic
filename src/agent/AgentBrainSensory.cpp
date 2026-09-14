#include "AgentBrainSensory.hpp"
#include "AgentBrain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {
namespace sensory {

void AgentBrainSensory::processSensoryInput(AgentBrain& agentBrain, const SensoryPercept& percept) {
    if (!agentBrain.brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < agentBrain.sensoryVision_.size() && i < vision.size(); ++i) {
        if (agentBrain.sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            agentBrain.sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < agentBrain.sensoryTouch_.size() && i < touch.size(); ++i) {
        if (agentBrain.sensoryTouch_[i]) {
            float current = touch[i] * 8.0f;  // Collision signal
            agentBrain.sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& intern = percept.getInternal();
    for (size_t i = 0; i < agentBrain.sensoryInternal_.size() && i < intern.size(); ++i) {
        if (agentBrain.sensoryInternal_[i]) {
            float current = (intern[i] * 2.0f - 1.0f) * 5.0f;  // Center and scale
            agentBrain.sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprio = percept.getProprioception();
    for (size_t i = 0; i < agentBrain.sensoryProprioception_.size() && i < proprio.size(); ++i) {
        if (agentBrain.sensoryProprioception_[i]) {
            float current = (proprio[i] * 2.0f - 1.0f) * 3.0f;  // Center and scale
            agentBrain.sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Compute novelty (difference from previous vision)
    if (!vision.empty()) {
        float totalDiff = 0.0f;
        for (size_t i = 0; i < vision.size() && i < agentBrain.previousVision_.size(); ++i) {
            float diff = std::abs(vision[i] - agentBrain.previousVision_[i]);
            totalDiff += diff;
        }
        
        // Normalize
        agentBrain.noveltyLevel_ = totalDiff / std::max<size_t>(vision.size(), 1);
        
        // Decay and update
        agentBrain.noveltyLevel_ *= agentBrain.sensoryNoveltyDecay_;
        
        // Store for next time
        agentBrain.previousVision_ = vision;
    }
    
    // Update curiosity based on novelty
    if (agentBrain.curiosityEnabled_) {
        agentBrain.curiosityLevel_ = agentBrain.noveltyLevel_ * 2.0f + std::abs(agentBrain.predictionError_) * 0.5f;
        agentBrain.curiosityLevel_ = std::clamp(agentBrain.curiosityLevel_, 0.0f, 1.0f);
    }
}

} // namespace sensory
} // namespace nlm
