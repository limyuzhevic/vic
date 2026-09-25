#include "BrainCognitionManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"

namespace nlm {

void BrainCognitionManager::initialize(Brain* brain) {
    if (!brain) return;
    
    // Initialize cognition systems
    if (auto* planner = brain->getPlanner()) {
        planner->initialize(brain);
        planner->setPlanningDepth(5);
    }
    
    if (auto* conceptFormation = brain->getConceptFormation()) {
        conceptFormation->initialize(brain);
    }
    
    if (auto* attention = brain->getAttention()) {
        attention->initialize(brain);
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
}

void BrainCognitionManager::update(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    // Update attention system
    if (auto* attention = brain->getAttention()) {
        attention->update(timestep);
    }
    
    // Process attention to working memory winners
    if (auto* workingMemory = brain->getWorkingMemory() && 
        !workingMemory->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
        if (auto* attention = brain->getAttention()) {
            attention->processCompetition(competitors);
        }
    }
}

void BrainCognitionManager::processAttentionCompetition(Brain* brain) {
    if (!brain) return;
    
    if (auto* workingMemory = brain->getWorkingMemory() && 
        !workingMemory->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = workingMemory->getMemoryNeurons();
        if (auto* attention = brain->getAttention()) {
            attention->processCompetition(competitors);
        }
    }
}

void BrainCognitionManager::collectCognitionStats(Brain* brain) const {
    // Cognition statistics will be collected in logStatus()
}

} // namespace nlm
