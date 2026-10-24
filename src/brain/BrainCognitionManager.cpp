#include "BrainCognitionManager.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

struct BrainCognitionManager::Impl {
    // State for cognition systems
    std::vector<NeuronId> workingMemoryWinners;
    
    Impl() {}
};

BrainCognitionManager::BrainCognitionManager() : pImpl(std::make_unique<Impl>()) {}

BrainCognitionManager::~BrainCognitionManager() = default;

void BrainCognitionManager::initialize(Brain* brain) {
    // Initialize cognition systems through brain
    if (brain->getPlanner()) {
        brain->getPlanner()->initialize(brain);
    }
    if (brain->getConceptFormation()) {
        brain->getConceptFormation()->initialize(brain);
    }
    if (brain->getAttention()) {
        brain->getAttention()->initialize(brain);
    }
}

void BrainCognitionManager::updatePlanner(Brain* brain, TimestepDuration timestep) {
    if (brain->getPlanner()) {
        brain->getPlanner()->setPlanningDepth(5);
    }
}

void BrainCognitionManager::updateConceptFormation(Brain* brain) {
    // Concept formation would process current neural activity patterns
    // This requires sensory state encoding
    if (brain->getConceptFormation()) {
        // For now, just log that concept formation is active
        NLM_LOG_INFO("Concept formation updating with neural patterns");
    }
}

void BrainCognitionManager::updateAttention(Brain* brain, TimestepDuration timestep) {
    if (brain->getAttention()) {
        brain->getAttention()->update(timestep);
        brain->getAttention()->setInhibitionStrength(0.5f);
        brain->getAttention()->setExcitationStrength(1.5f);
        
        // Apply attention to working memory winners
        if (brain->getWorkingMemory() && !brain->getWorkingMemory()->getMemoryNeurons().empty()) {
            std::vector<NeuronId> competitors = brain->getWorkingMemory()->getMemoryNeurons();
            brain->getAttention()->processCompetition(competitors);
        }
    }
}

void BrainCognitionManager::processWorkingMemoryCompetition(Brain* brain) {
    if (brain->getWorkingMemory() && !brain->getWorkingMemory()->getMemoryNeurons().empty()) {
        std::vector<NeuronId> competitors = brain->getWorkingMemory()->getMemoryNeurons();
        if (brain->getAttention()) {
            brain->getAttention()->processCompetition(competitors);
        }
    }
}

} // namespace nlm
