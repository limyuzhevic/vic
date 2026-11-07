#include "CognitionManager.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

CognitionManager::CognitionManager(Brain* brain)
    : brain_(brain)
    , planner(std::make_unique<NeuralPlanner>())
    , conceptFormation(std::make_unique<ConceptFormation>())
    , attention(std::make_unique<AttentionalSelection>()) {
}

CognitionManager::~CognitionManager() = default;

bool CognitionManager::initialize() {
    NLM_LOG_INFO("Initializing Cognition Manager...");
    
    if (!brain_) return false;
    
    // Initialize cognition systems
    planner->initialize(brain_);
    planner->setPlanningDepth(5);
    
    conceptFormation->initialize(brain_);
    
    attention->initialize(brain_);
    attention->setInhibitionStrength(0.5f);
    attention->setExcitationStrength(1.5f);
    
    NLM_LOG_INFO("Cognition Manager initialized successfully");
    return true;
}

void CognitionManager::reset() {
    // Cognition systems maintain state across episodes
}

void CognitionManager::step(double timestep) {
    updatePlanner(timestep);
    updateConceptFormation(timestep);
    updateAttention(timestep);
}

void CognitionManager::updatePlanner(double timestep) {
    // Planner is updated with action quality from rewards in AgentBrain
}

void CognitionManager::updateConceptFormation(double timestep) {
    // Concept formation would process current neural activity patterns to form concepts
    // This requires sensory state encoding, which is handled by AgentBrain
}

void CognitionManager::updateAttention(double timestep) {
    if (attention) {
        attention->update(timestep);
        
        // Apply attention to working memory winners
        if (brain_->pImpl->workingMemory && !brain_->pImpl->workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = brain_->pImpl->workingMemory->getMemoryNeurons();
            attention->processCompetition(competitors);
        }
    }
}

void CognitionManager::updateSelfModel(const std::vector<float>& lastVision, const std::vector<float>& currentVision) {
    // Self-model updates are handled by AgentBrain
}

} // namespace nlm
