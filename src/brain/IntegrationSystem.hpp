#include "IntegrationSystem.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct IntegrationSystem::Impl {
    Brain* brain_;
    std::shared_ptr<Config> config_;
    
    Impl() : brain_(nullptr) {}
};

IntegrationSystem::IntegrationSystem() : pImpl(std::make_unique<Impl>()) {}
IntegrationSystem::~IntegrationSystem() = default;

void IntegrationSystem::initialize(Brain* brain, std::shared_ptr<Config> config) {
    pImpl->brain_ = brain;
    pImpl->config_ = config;
}

void IntegrationSystem::updateAttention(AttentionalSelection* attention,
                                       NeuralWorkingMemory* workingMemory,
                                       TimestepDuration dt) {
    if (attention) {
        attention->update(dt);
        
        // Apply attention to working memory winners
        if (workingMemory && !workingMemory->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
            attention->processCompetition(competitors);
        }
    }
}

void IntegrationSystem::updateConceptFormation(ConceptFormation* conceptFormation,
                                              Timestamp currentTime) {
    // Concept formation is updated by Brain directly via its step method
    // Integration system provides coordination
}

void IntegrationSystem::updatePredictionSystem(PredictionSystem* predictionSystem) {
    // Prediction system is updated by Brain directly
    // Integration system provides coordination
}

void IntegrationSystem::updateCognitionSystems(NeuralPlanner* planner,
                                              ConceptFormation* conceptFormation,
                                              AttentionalSelection* attention,
                                              Timestamp currentTime) {
    if (planner) {
        // Would update planner with current context
    }
    
    if (conceptFormation) {
        // Would update concept formation with current patterns
    }
    
    if (attention) {
        // Would update attention with current state
    }
}

void IntegrationSystem::updateAllSystems(Brain* brain, Timestamp currentTime, TimestepDuration dt) {
    // Coordinate attention system
    if (brain && brain->getAttention()) {
        updateAttention(brain->getAttention(), brain->getWorkingMemory(), dt);
    }
    
    // Coordinate concept formation
    if (brain && brain->getConceptFormation()) {
        // Concept formation is updated during its dedicated step
    }
    
    // Coordinate prediction system
    if (brain && brain->getPredictionSystem()) {
        // Prediction system is updated during its dedicated step
    }
    
    // Coordinate cognition systems
    if (brain) {
        updateCognitionSystems(brain->getPlanner(), brain->getConceptFormation(), brain->getAttention(), currentTime);
    }
}

void IntegrationSystem::reset() {
    // Reset integration system state
}

} // namespace nlm
