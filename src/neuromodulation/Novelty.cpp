#include "Novelty.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../development/DevelopmentSystem.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Novelty::Impl {
    class Brain* brain;
    float level;
    float decayRate;
    std::vector<float> history;
    std::vector<float> lastPattern;
    float noveltyThreshold;
    
    // Enhanced novelty effects
    float memoryConsolidationStrength;
    float explorationGuidance;
    float conceptFormationBoost;
    float developmentTrigger;
    
    Impl() : brain(nullptr), level(0.0f), decayRate(0.1f), noveltyThreshold(0.3f),
             memoryConsolidationStrength(0.0f), explorationGuidance(0.0f),
             conceptFormationBoost(0.0f), developmentTrigger(0.0f) {}
};

Novelty::Novelty() : pImpl(new Impl) {}

Novelty::~Novelty() = default;

void Novelty::initialize(Brain* brain) {
    pImpl->brain = brain;
    NLM_LOG_INFO("Novelty detection initialized");
}

float Novelty::getLevel() const {
    return pImpl->level;
}

void Novelty::setLevel(float level) {
    pImpl->level = level;
}

void Novelty::detectNovelty(const Observation& observation, 
                            const Observation& previousObservation) {
    // Extract features from observations and compare
    // Simple implementation: just set to a placeholder
    pImpl->level = 1.0f;
    pImpl->history.push_back(pImpl->level);
}

void Novelty::detectNovelty(const std::vector<float>& currentPattern,
                            const std::vector<float>& previousPattern) {
    if (currentPattern.empty() || previousPattern.empty()) {
        pImpl->level = 0.0f;
        return;
    }
    
    // Compute difference between patterns
    float totalDiff = 0.0f;
    size_t compareLen = std::min(currentPattern.size(), previousPattern.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        float diff = std::abs(currentPattern[i] - previousPattern[i]);
        totalDiff += diff;
    }
    
    // Normalize by pattern size
    float avgDiff = totalDiff / compareLen;
    
    // Update novelty level based on difference
    pImpl->level = std::min(1.0f, avgDiff / pImpl->noveltyThreshold);
    pImpl->history.push_back(pImpl->level);
    
    // Store current pattern for next comparison
    pImpl->lastPattern = currentPattern;
}

void Novelty::update(TimestepDuration dt) {
    // Decay novelty
    pImpl->level = std::max(0.0f, pImpl->level - pImpl->decayRate * static_cast<float>(dt));
}

const std::vector<float>& Novelty::getHistory() const {
    return pImpl->history;
}

void Novelty::clearHistory() {
    pImpl->history.clear();
}

// Enhanced novelty effects for all systems
void Novelty::applyMemoryConsolidation(NeuralWorkingMemory* workingMemory, float deltaTime) {
    if (!workingMemory) return;
    
    // Enhance working memory consolidation based on novelty
    float consolidationStrength = pImpl->level * pImpl->memoryConsolidationStrength;
    workingMemory->enhanceConsolidation(consolidationStrength);
    
    // Boost encoding of novel information
    workingMemory->boostNovelEncoding(pImpl->level * 0.5f);
}

void Novelty::applyExplorationGuidance(NeuralPlanner* planner, float deltaTime) {
    if (!planner) return;
    
    // Guide exploration based on novelty level
    float explorationGuidance = pImpl->level * pImpl->explorationGuidance;
    planner->guideExploration(explorationGuidance);
    
    // Increase exploration entropy based on novelty
    planner->increaseExplorationEntropy(pImpl->level * 0.3f);
}

void Novelty::applyConceptFormationEnhancement(ConceptFormation* conceptFormation, float deltaTime) {
    if (!conceptFormation) return;
    
    // Boost concept formation with novel input
    float conceptBoost = pImpl->level * pImpl->conceptFormationBoost;
    conceptFormation->enhanceNoveltyProcessing(conceptBoost);
    
    // Facilitate pattern separation for novel inputs
    conceptFormation->facilitatePatternSeparation(pImpl->level * 0.4f);
}

void Novelty::applyDevelopmentTriggering(DevelopmentSystem* developmentSystem, float deltaTime) {
    if (!developmentSystem) return;
    
    // Trigger developmental processes based on novelty
    if (pImpl->level > 0.7f) {
        developmentSystem->triggerNoveltyDrivenDevelopment(pImpl->level * pImpl->developmentTrigger);
    }
    
    // Accumulate developmental pressure from sustained novelty
    if (pImpl->level > 0.5f) {
        developmentSystem->accumulateDevelopmentalPressure(pImpl->level * 0.1f);
    }
}

} // namespace nlm
