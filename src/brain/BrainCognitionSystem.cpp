// Brain Cognition System implementation
#include "BrainCognitionSystem.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

BrainCognitionSystem::BrainCognitionSystem(Brain* brain) : brain(brain), curiosityLevel(0.5f), noveltyLevel(0.3f), predictionError(0.0f), neuromodulationLevel(0.5f) {
    planner = std::make_unique<NeuralPlanner>();
    conceptFormation = std::make_unique<ConceptFormation>();
    attention = std::make_unique<AttentionalSelection>();
    dopamine = std::make_unique<Dopamine>();
    curiosity = std::make_unique<Curiosity>();
    predictionError = std::make_unique<PredictionError>();
    novelty = std::make_unique<Novelty>();
}

void BrainCognitionSystem::initializeCognitionSystems() {
    if (planner) {
        planner->initialize(brain);
        planner->setPlanningDepth(5);
    }
    
    if (conceptFormation) {
        conceptFormation->initialize(brain);
    }
    
    if (attention) {
        attention->initialize(brain);
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
    
    if (novelty) {
        novelty->initialize(brain);
    }
    
    if (curiosity) {
        curiosity->initialize(brain);
    }
    
    NLM_LOG_INFO("Cognition systems initialized");
}

void BrainCognitionSystem::updateCognitionSystems(TimestepDuration timestep) {
    if (novelty) {
        novelty->update(timestep);
        noveltyLevel = novelty->getLevel();
        updateCuriosity(noveltyLevel);
    }
    
    if (curiosity) {
        curiosity->update(timestep);
        curiosityLevel = curiosity->getLevel();
    }
    
    if (dopamine) {
        dopamine->update(timestep);
        neuromodulationLevel = dopamine->getLevel();
        predictionError = neuromodulationLevel * 0.5f;
    }
    
    if (attention) {
        attention->update(timestep);
    }
}

void BrainCognitionSystem::updateCuriosity(float novelty) {
    float curiosityInfluence = novelty * 0.5f;
    curiosityLevel = std::min(1.0f, curiosityLevel + curiosityInfluence * 0.1f);
}

void BrainCognitionSystem::processCompetition(const std::vector<NeuronId>& competitors) {
    if (attention) {
        attention->processCompetition(competitors);
    }
}

void BrainCognitionSystem::formConceptsFromCurrentActivity() {
    if (conceptFormation) {
        // Would process current neural activity patterns to form concepts
        // This requires sensory state encoding
    }
}

} // namespace nlm