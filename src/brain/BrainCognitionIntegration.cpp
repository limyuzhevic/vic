// BrainCognitionIntegration.cpp - Neural planner, concept formation, attention integration implementation
// Contains cognition system integration functionality

#include "BrainCognitionIntegration.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Implementation of BrainCognitionIntegration

BrainCognitionIntegration::BrainCognitionIntegration(BrainCore* core) : pImpl(new Impl(core)) {}

BrainCognitionIntegration::~BrainCognitionIntegration() = default;

BrainCognitionIntegration::BrainCognitionIntegration(BrainCognitionIntegration&& other) noexcept : pImpl(other.pImpl) {
    other.pImpl = nullptr;
}

BrainCognitionIntegration& BrainCognitionIntegration::operator=(BrainCognitionIntegration&& other) noexcept {
    if (this != &other) {
        delete pImpl;
        pImpl = other.pImpl;
        other.pImpl = nullptr;
    }
    return *this;
}

bool BrainCognitionIntegration::initialize() {
    NLM_LOG_INFO("Initializing Brain Cognition Integration...");
    
    if (!pImpl->brainCore) {
        NLM_LOG_ERROR("Cannot initialize cognition integration - no brain core available");
        return false;
    }
    
    pImpl->planner = pImpl->brainCore->getPlanner();
    pImpl->conceptFormation = pImpl->brainCore->getConceptFormation();
    pImpl->attention = pImpl->brainCore->getAttention();
    
    NLM_LOG_INFO("Brain Cognition Integration initialized successfully");
    return true;
}

void BrainCognitionIntegration::update(TimestepDuration dt) {
    updatePlanner();
    updateConceptFormation();
    updateAttention();
}

void BrainCognitionIntegration::processCompetition(const std::vector<NeuronId>& competitors) {
    pImpl->competitionHistory.push_back(competitors.size());
    if (pImpl->competitionHistory.size() > 100) {
        pImpl->competitionHistory.erase(pImpl->competitionHistory.begin());
    }
    
    if (pImpl->attention) {
        pImpl->attention->processCompetition(competitors);
        pImpl->attentionSelections++;
    }
}

NeuralPlanner* BrainCognitionIntegration::getPlanner() {
    return pImpl->planner;
}

ConceptFormation* BrainCognitionIntegration::getConceptFormation() {
    return pImpl->conceptFormation;
}

AttentionalSelection* BrainCognitionIntegration::getAttention() {
    return pImpl->attention;
}

void BrainCognitionIntegration::clear() {
    pImpl->activePlan.clear();
    pImpl->competitionHistory.clear();
    pImpl->planningIterations = 0;
    pImpl->conceptFormations = 0;
    pImpl->attentionSelections = 0;
}

size_t BrainCognitionIntegration::getPlanningIterations() const {
    return pImpl->planningIterations;
}

size_t BrainCognitionIntegration::getConceptFormations() const {
    return pImpl->conceptFormations;
}

size_t BrainCognitionIntegration::getAttentionSelections() const {
    return pImpl->attentionSelections;
}

void BrainCognitionIntegration::Impl::Impl(BrainCore* core) : brainCore(core), planner(nullptr), 
    conceptFormation(nullptr), attention(nullptr), planningIterations(0), 
    conceptFormations(0), attentionSelections(0) {}

void BrainCognitionIntegration::updatePlanner() {
    if (pImpl->planner) {
        // Plan actions based on current brain state
        pImpl->planner->update(pImpl->brainCore->getWorkingMemory());
        pImpl->planningIterations++;
    }
}

void BrainCognitionIntegration::updateConceptFormation() {
    if (pImpl->conceptFormation) {
        // Form concepts from neural activity patterns
        // This would process current neural activity to discover patterns
        pImpl->conceptFormations++;
    }
}

void BrainCognitionIntegration::updateAttention() {
    if (pImpl->attention) {
        // Update attentional selection based on current focus areas
        pImpl->attention->update(0.001f); // Using a default timestep
    }
}

void BrainCognitionIntegration::updatePlanningMetrics() {}

void BrainCognitionIntegration::calculateCognitionStats() const {}

} // namespace nlm

