#include "CognitiveOrchestrator.hpp"
#include "../core/Logger/Logger.hpp"
#include "../memory/NeuralWorkingMemory.hpp"

namespace nlm {

struct CognitiveOrchestrator::Impl {
    std::shared_ptr<Config> config;
    Brain* brain;
    std::unique_ptr<NeuralPlanner> planner;
    std::unique_ptr<ConceptFormation> conceptFormation;
    std::unique_ptr<AttentionalSelection> attention;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg), brain(nullptr) {
        planner = std::make_unique<NeuralPlanner>();
        conceptFormation = std::make_unique<ConceptFormation>();
        attention = std::make_unique<AttentionalSelection>();
    }
    
    ~Impl() = default;
};

CognitiveOrchestrator::CognitiveOrchestrator(std::shared_ptr<Config> config) : 
    pImpl(new Impl(config)), 
    planningDepth(5),
    attentionExcitation(1.5f),
    attentionInhibition(0.5f) {}

CognitiveOrchestrator::~CognitiveOrchestrator() {
    delete pImpl;
}

bool CognitiveOrchestrator::initialize(Brain* brain) {
    NLM_LOG_INFO("Initializing CognitiveOrchestrator...");
    
    pImpl->brain = brain;
    
    pImpl->planner->initialize(brain);
    pImpl->planner->setPlanningDepth(planningDepth);
    
    pImpl->conceptFormation->initialize(brain);
    
    pImpl->attention->initialize(brain);
    pImpl->attention->setInhibitionStrength(attentionInhibition);
    pImpl->attention->setExcitationStrength(attentionExcitation);
    
    NLM_LOG_INFO("CognitiveOrchestrator initialized successfully");
    return true;
}

void CognitiveOrchestrator::update(Timestamp time, TimestepDuration timestep) {
    // Update attention system
    if (pImpl->attention) {
        pImpl->attention->update(timestep);
        
        // Apply attention to working memory winners
        if (pImpl->brain->getWorkingMemory() && !pImpl->brain->getWorkingMemory()->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = pImpl->brain->getWorkingMemory()->getMemoryNeurons();
            pImpl->attention->processCompetition(competitors);
        }
    }
}

size_t CognitiveOrchestrator::getActiveConcepts() const {
    // Return count of active concepts
    return 0;  // Placeholder - would depend on implementation
}

size_t CognitiveOrchestrator::getActiveGoals() const {
    // Return count of active goals
    return 0;  // Placeholder - would depend on implementation
}

void CognitiveOrchestrator::reset() {
    NLM_LOG_INFO("Resetting CognitiveOrchestrator...");
    if (pImpl->attention) pImpl->attention->reset();
    NLM_LOG_INFO("CognitiveOrchestrator reset complete");
}

} // namespace nlm