#include "Curiosity.hpp"
#include "../core/Logger/Logger.hpp"
#include "../cognition/NeuralAttentionalSelection.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"

namespace nlm {

struct Curiosity::Impl {
    class Brain* brain;
    float level;
    float noveltyWeight;
    float predictionErrorWeight;
    float decayRate;
    
    // Enhanced curiosity effects
    float workingMemoryEncodingBoost;
    float learningRateEnhancement;
    float attentionGuidance;
    float planningDepth;
    
    Impl() 
        : brain(nullptr)
        , level(0.0f)
        , noveltyWeight(0.5f)
        , predictionErrorWeight(0.5f)
        , decayRate(0.05f)
        , workingMemoryEncodingBoost(0.0f)
        , learningRateEnhancement(0.0f)
        , attentionGuidance(0.0f)
        , planningDepth(0.0f) {}
};

Curiosity::Curiosity() : pImpl(new Impl) {}

Curiosity::~Curiosity() = default;

void Curiosity::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Curiosity system initialized");
}

float Curiosity::getLevel() const {
    return pImpl->level;
}

void Curiosity::update(float novelty, float predictionError, TimestepDuration dt) {
    // Curiosity increases with novelty and prediction error
    float target = pImpl->noveltyWeight * novelty + 
                   pImpl->predictionErrorWeight * predictionError;
    
    // Smooth update
    pImpl->level += (target - pImpl->level) * 0.1f;
    
    // Decay
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

float Curiosity::getExplorationDrive() const {
    return pImpl->level;
}

void Curiosity::setNoveltyWeight(float weight) {
    pImpl->noveltyWeight = weight;
}

void Curiosity::setPredictionErrorWeight(float weight) {
    pImpl->predictionErrorWeight = weight;
}

void Curiosity::reset() {
    pImpl->level = 0.0f;
}

// Enhanced curiosity effects for all systems
void Curiosity::applyWorkingMemoryEnhancement(NeuralWorkingMemory* workingMemory, float deltaTime) {
    if (!workingMemory) return;
    
    // Enhance working memory encoding based on curiosity level
    float encodingBoost = pImpl->level * pImpl->workingMemoryEncodingBoost;
    workingMemory->enhanceEncoding(encodingBoost);
    
    // Boost memory consolidation
    workingMemory->enhanceConsolidation(pImpl->level * 0.5f);
}

void Curiosity::applyLearningRateEnhancement(PredictionSystem* predictionSystem, float deltaTime) {
    if (!predictionSystem) return;
    
    // Enhance learning rate based on curiosity level
    float learningRateBoost = pImpl->level * pImpl->learningRateEnhancement;
    predictionSystem->enhanceLearningRate(learningRateBoost);
}

void Curiosity::applyAttentionGuidance(NeuralAttentionalSelection* attention, float deltaTime) {
    if (!attention) return;
    
    // Guide attention to novel areas based on curiosity level
    float noveltyGuidance = pImpl->level * pImpl->attentionGuidance;
    attention->guideToNovelAreas(noveltyGuidance);
}

void Curiosity::applyPlanningDepthEnhancement(NeuralPlanner* planner, float deltaTime) {
    if (!planner) return;
    
    // Affect planning depth based on curiosity level
    float planningBoost = pImpl->level * pImpl->planningDepth;
    planner->increasePlanningDepth(planningBoost);
}

} // namespace nlm
