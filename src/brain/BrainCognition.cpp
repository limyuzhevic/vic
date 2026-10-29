#include "BrainHeaders.hpp"

namespace nlm {

void BrainCognition::initializeCognitionSystems(Brain* brain) {
    if (!brain) return;
    
    auto* planner = brain->getPlanner();
    if (planner) {
        planner->initialize(brain);
        planner->setPlanningDepth(5);
    }
    
    auto* conceptFormation = brain->getConceptFormation();
    if (conceptFormation) {
        conceptFormation->initialize(brain);
    }
    
    auto* attention = brain->getAttention();
    if (attention) {
        attention->initialize(brain);
        attention->setInhibitionStrength(0.5f);
        attention->setExcitationStrength(1.5f);
    }
}

void BrainCognition::updatePlanningSystem(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    auto* planner = brain->getPlanner();
    if (planner) {
        planner->update(timestep);
    }
}

void BrainCognition::updateConceptFormation(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    auto* conceptFormation = brain->getConceptFormation();
    if (conceptFormation) {
        conceptFormation->update(timestep);
    }
}

void BrainCognition::updateAttentionSystem(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    auto* attention = brain->getAttention();
    if (attention) {
        attention->update(timestep);
    }
}

void BrainCognition::applyCognitiveBiases(Brain* brain) {
    if (!brain) return;
    auto* attention = brain->getAttention();
    if (attention) {
        attention->applyBiases();
    }
}

void BrainCognition::generateMotorCommands(Brain* brain) {
    if (!brain) return;
    // Motor commands are generated based on brain activity
    // This is typically handled by the motor neuron populations
}

} // namespace nlm