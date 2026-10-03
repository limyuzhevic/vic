#include "../prediction/PredictionSystem.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../cognition/NeuralPlanner.hpp"

namespace nlm {

SensoryPercept::SensoryPercept()
    : visionWidth_(16)
    , visionHeight_(16)
    , timestamp_(0.0)
    , predictionError_(0.0f)
    , noveltyLevel_(0.0f)
    , attentionWeight_(0.0f)
    , planningConfidence_(0.0f)
    , workingMemoryActivation_(0.0f)
{
    vision_.resize(16 * 16, 0.0f);
    touch_.resize(8, 0.0f);
    internal_.resize(4, 0.0f);
    proprioception_.resize(6, 0.0f);
    audio_.resize(0, 0.0f);
}

void SensoryPercept::updateFromCognitiveSystems(
    PredictionSystem* predictionSystem,
    ConceptFormation* conceptFormation,
    AttentionalSelection* attention,
    NeuralPlanner* planner) {
    
    // Update from prediction system
    if (predictionSystem) {
        predictionError_ = predictionSystem->getPredictionError();
    }
    
    // Update from concept formation
    if (conceptFormation) {
        size_t conceptCount = conceptFormation->getConceptCount();
        conceptRelevance_.clear();
        
        for (size_t i = 0; i < conceptCount; ++i) {
            float relevance = conceptFormation->getConceptStability(i);
            conceptRelevance_[i] = relevance;
        }
    }
    
    // Update from attentional selection
    if (attention) {
        attentionWeight_ = (attention->getWinners().empty()) ? 0.0f : 1.0f;
    }
    
    // Update from neural planner
    if (planner) {
        planningConfidence_ = planner->getPlanningConfidence();
    }
    
    // Calculate working memory activation from sensory strength
    workingMemoryActivation_ = 0.0f;
    
    // Base activation on vision strength
    for (float v : vision_) {
        workingMemoryActivation_ += std::abs(v);
    }
    
    // Base activation on novelty
    workingMemoryActivation_ += noveltyLevel_ * 0.3f;
    
    // Normalize activation
    workingMemoryActivation_ = std::min(workingMemoryActivation_, 1.0f);
}

std::vector<float> SensoryPercept::getAllSignals() const {
    std::vector<float> all;
    
    // Vision (flattened)
    all.insert(all.end(), vision_.begin(), vision_.end());
    
    // Touch
    all.insert(all.end(), touch_.begin(), touch_.end());
    
    // Internal
    all.insert(all.end(), internal_.begin(), internal_.end());
    
    // Proprioception
    all.insert(all.end(), proprioception_.begin(), proprioception_.end());
    
    // Audio
    all.insert(all.end(), audio_.begin(), audio_.end());
    
    return all;
}

} // namespace nlm
