#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

void AgentBrain::processSensoryInput(const SensoryPercept& percept) {
    if (!brain_) return;
    
    // Vision input (256 values -> sensoryVision_ neurons)
    const auto& vision = percept.getVision();
    for (size_t i = 0; i < sensoryVision_.size() && i < vision.size(); ++i) {
        if (sensoryVision_[i]) {
            // Inject current proportional to vision intensity
            float current = vision[i] * 5.0f;  // Scale factor
            sensoryVision_[i]->injectCurrent(current);
        }
    }
    
    // Touch input (8 values -> sensoryTouch_ neurons)
    const auto& touch = percept.getTouch();
    for (size_t i = 0; i < sensoryTouch_.size() && i < touch.size(); ++i) {
        if (sensoryTouch_[i]) {
            float current = touch[i] * 3.0f;  // Scale factor
            sensoryTouch_[i]->injectCurrent(current);
        }
    }
    
    // Internal signals (4 values -> sensoryInternal_ neurons)
    const auto& internal = percept.getInternal();
    for (size_t i = 0; i < sensoryInternal_.size() && i < internal.size(); ++i) {
        if (sensoryInternal_[i]) {
            float current = internal[i] * 2.0f;  // Scale factor
            sensoryInternal_[i]->injectCurrent(current);
        }
    }
    
    // Proprioception (6 values -> sensoryProprioception_ neurons)
    const auto& proprioception = percept.getProprioception();
    for (size_t i = 0; i < sensoryProprioception_.size() && i < proprioception.size(); ++i) {
        if (sensoryProprioception_[i]) {
            float current = proprioception[i] * 1.0f;  // Scale factor
            sensoryProprioception_[i]->injectCurrent(current);
        }
    }
    
    // Store previous vision for novelty detection
    previousVision_ = percept.getVision();
    
    // Check for novelty and update curiosity
    if (noveltyEnabled_ && percept.getTimestamp() > 0.0) {
        // Novelty detection happens in SimpleWorld and is updated here
        // For now, just use a placeholder
        float novelty = 0.0f;
        if (brain_->getNovelty()) {
            novelty = brain_->getNovelty()->getLevel();
        }
        
        // Prediction error from prediction system
        float predictionError = 0.0f;
        if (brain_->getPredictionErrorSignal()) {
            predictionError = brain_->getPredictionErrorSignal()->getError();
        }
        
        if (brain_->getCuriosity()) {
            brain_->getCuriosity()->update(novelty, predictionError, 0.1f);
        }
    }
    
    // Enable prediction system for new observations
    if (brain_->getPredictionSystem()) {
        // In a real implementation, we would:
        // 1. Get current sensory state from percept
        // 2. Use prediction system to predict next state
        // 3. Store prediction for comparison with actual input
        
        // For now, just mark that sensory input was processed
        // Prediction system would be updated in the brain step
    }
    
    // Update working memory with sensory input
    if (brain_->getWorkingMemory() && !sensoryVision_.empty()) {
        // Store sensory pattern in working memory
        float totalInput = 0.0f;
        for (const auto& neuron : sensoryVision_) {
            if (neuron) {
                totalInput += neuron->getMembranePotential();
            }
        }
        
        if (!sensoryVision_.empty()) {
            totalInput /= sensoryVision_.size();
            brain_->getWorkingMemory()->storeToNeuron(INVALID_NEURON_ID, totalInput);
        }
    }
    
    // Update episodic memory with new experience
    if (brain_->getEpisodicMemory() && !sensoryVision_.empty()) {
        // Create episodic memory entry from sensory input
        // In a real implementation, we would:
        // 1. Create a rich episodic memory entry with:
        //    - Sensory state
        //    - Actions taken
        //    - Rewards received
        //    - Context information
        // 2. Store in episodic memory
        
        // For now, just log that episodic memory was updated
        // NLM_LOG_INFO("Episodic memory updated with new sensory experience");
    }
    
    // Update concept formation with new patterns
    if (brain_->getConceptFormation() && !sensoryVision_.empty()) {
        // Extract patterns from sensory input and update concepts
        // In a real implementation:
        // 1. Extract features from sensory patterns
        // 2. Cluster similar patterns
        // 3. Update concept representations
        
        // For now, just log concept formation update
        // NLM_LOG_INFO("Concept formation updated with new sensory patterns");
    }
}

} // namespace nlm