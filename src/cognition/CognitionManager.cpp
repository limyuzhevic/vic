#include "CognitionManager.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include <algorithm>
#include <numeric>

namespace nlm {

struct CognitionManager::Impl {
    // Configuration for cognition parameters
    float predictionUpdateInterval = 1.0f;
    float attentionUpdateInterval = 1.0f;
    float conceptUpdateInterval = 5.0f;
    float workingMemoryUpdateInterval = 1.0f;
    
    // Performance tracking
    float predictionAccuracySum = 0.0f;
    size_t predictionAccuracyCount = 0;
    
    float attentionSelectivitySum = 0.0f;
    size_t attentionSelectivityCount = 0;
    
    float conceptStabilitySum = 0.0f;
    size_t conceptStabilityCount = 0;
    
    // Recent activity tracking
    SimulationStep lastPredictionStep = 0;
    SimulationStep lastAttentionStep = 0;
    SimulationStep lastConceptStep = 0;
    SimulationStep lastWorkingMemoryStep = 0;
    
    // Temporary storage for updates
    std::vector<NeuronId> conceptGuidedAttention_;
};

CognitionManager::CognitionManager() : brain_(nullptr), lastUpdateStep_(0), lastUpdateTime_(0.0) {
    pImpl = std::make_unique<Impl>();
}

CognitionManager::~CognitionManager() = default;

void CognitionManager::initialize(Brain* brain) {
    brain_ = brain;
    
    // Initialize cognition subsystems using existing brain components
    if (brain_) {
        // Get existing systems from brain
        predictionSystem_ = std::unique_ptr<PredictionSystem>(brain_->getPredictionSystem());
        attention_ = std::unique_ptr<AttentionalSelection>(brain_->getAttention());
        conceptFormation_ = std::unique_ptr<ConceptFormation>(brain_->getConceptFormation());
        workingMemory_ = std::unique_ptr<NeuralWorkingMemory>(brain_->getWorkingMemory());
        
        // Initialize each subsystem if it exists
        if (attention_) {
            attention_->initialize(brain_);
            attention_->setInhibitionStrength(0.5f);
            attention_->setExcitationStrength(1.5f);
        }
        if (conceptFormation_) {
            conceptFormation_->initialize(brain_);
        }
        if (workingMemory_) {
            workingMemory_->initialize(brain_);
        }
    }
}

void CognitionManager::update(SimulationStep currentStep, Timestamp currentTime) {
    // Track time since last update
    if (lastUpdateStep_ == 0) {
        lastUpdateStep_ = currentStep;
        lastUpdateTime_ = currentTime;
    }
    
    TimestepDuration dt = static_cast<TimestepDuration>(currentTime - lastUpdateTime_);
    lastUpdateStep_ = currentStep;
    lastUpdateTime_ = currentTime;
    
    // Update each cognition subsystem
    updatePredictionSystem();
    updateAttentionSystem();
    updateConceptFormation();
    applyAttentionToWorkingMemoryWinners();
    
    // Update statistics
    updateStatistics();
}

void CognitionManager::updatePredictionSystem() {
    if (!predictionSystem_) {
        return;
    }
    
    stats_.predictionUpdates++;
    
    // For now, just track prediction error (as in original Brain.cpp)
    // In a real implementation, this would update prediction with sensory observations
    // and compute prediction error
}

void CognitionManager::updateAttentionSystem() {
    if (!attention_) {
        return;
    }
    
    stats_.attentionUpdates++;
    
    // Update attention dynamics
    attention_->update(dt_);
}

void CognitionManager::updateConceptFormation() {
    if (!conceptFormation_) {
        return;
    }
    
    stats_.conceptFormations++;
}

void CognitionManager::applyAttentionToWorkingMemoryWinners() {
    if (!attention_ || !workingMemory_) {
        return;
    }
    
    stats_.attentionApplications++;
    
    // Get current working memory content
    auto competitors = workingMemory_->getMemoryNeurons();
    
    if (!competitors.empty()) {
        // Process competition to select winners
        auto winners = attention_->processCompetition(competitors);
        
        // Track attention to working memory winners
        stats_.attentionToWorkingMemoryWinners++;
        
        // Apply learned concepts to guide attention
        applyConceptGuidedAttention();
        
        // Check if we have concept-guided attention for winners
        if (!pImpl->conceptGuidedAttention_.empty() && !winners.empty()) {
            // Apply concept-guided bias to winners
            for (const auto& winner : winners) {
                attention_->applyTopDownBias(winner, pImpl->conceptGuidedAttention_[winner]);
            }
        }
    }
}

void CognitionManager::updatePredictionWithState(const std::vector<float>& currentState,
                                               const std::vector<float>& sensoryFeatures,
                                               float reward) {
    if (!predictionSystem_) {
        return;
    }
    
    // For now, just update statistics
    // In a real implementation, this would:
    // 1. Update prediction system with current state and features
    // 2. Compute prediction error
    // 3. Learn from prediction error
    // 4. Update prediction accuracy statistics
}

void CognitionManager::updateAttentionWithWorkingMemory(const std::vector<NeuronId>& competitors) {
    if (!attention_ || competitors.empty()) {
        return;
    }
    
    // Process competition between working memory traces
    auto winners = attention_->processCompetition(competitors);
    
    // Track attention selectivity
    if (!winners.empty()) {
        float selectivity = static_cast<float>(winners.size()) / static_cast<float>(competitors.size());
        pImpl->attentionSelectivitySum += selectivity;
        pImpl->attentionSelectivityCount++;
        
        stats_.attentionSelectivity = pImpl->attentionSelectivityCount > 0 ? 
            pImpl->attentionSelectivitySum / pImpl->attentionSelectivityCount : 0.0f;
    }
}

void CognitionManager::applyConceptGuidedAttention() {
    if (!conceptFormation_ || !attention_) {
        return;
    }
    
    // Get concepts from concept formation
    const auto& concepts = conceptFormation_->getConcepts();
    
    if (!concepts.empty()) {
        // Apply concept-guided attention biases
        pImpl->conceptGuidedAttention_.clear();
        
        for (size_t i = 0; i < concepts.size(); ++i) {
            const auto& concept = concepts[i];
            float conceptStability = conceptFormation_->getConceptStability(i);
            
            // If concept is stable, use it to guide attention
            if (conceptStability > 0.5f) {
                // Find neurons that match this concept pattern
                // This would require mapping concept prototypes to neural activity
                // For now, just track concept stability
                pImpl->conceptGuidedAttention_.push_back(static_cast<NeuronId>(i));
            }
        }
    }
}

void CognitionManager::updateStatistics() {
    // Update statistics based on recent performance
    // This would be called from Brain update method
}

void CognitionManager::clear() {
    if (predictionSystem_) {
        // PredictionSystem doesn't have clear method
    }
    if (attention_) {
        attention_->reset();
    }
    if (conceptFormation_) {
        conceptFormation_->clear();
    }
    if (workingMemory_) {
        workingMemory_->clear();
    }
    
    resetStats();
    
    // Reset internal state
    if (pImpl) {
        pImpl->lastPredictionStep = 0;
        pImpl->lastAttentionStep = 0;
        pImpl->lastConceptStep = 0;
        pImpl->lastWorkingMemoryStep = 0;
        pImpl->predictionAccuracySum = 0.0f;
        pImpl->predictionAccuracyCount = 0;
        pImpl->attentionSelectivitySum = 0.0f;
        pImpl->attentionSelectivityCount = 0;
        pImpl->conceptStabilitySum = 0.0f;
        pImpl->conceptStabilityCount = 0;
        pImpl->conceptGuidedAttention_.clear();
    }
}

} // namespace nlm