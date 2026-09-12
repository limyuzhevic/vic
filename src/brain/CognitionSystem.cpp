// Cognition system implementation - handles planning, concept formation, and attention
#include "CognitionSystem.hpp"
#include "Brain.hpp"
#include <memory>

namespace nlm {

CognitionSystem::CognitionSystem(Brain& brainRef) : brain(brainRef) {
    planner = std::make_unique<NeuralPlanner>();
    conceptFormation = std::make_unique<ConceptFormation>();
    attention = std::make_unique<AttentionalSelection>();
}

CognitionSystem::~CognitionSystem() = default;

bool CognitionSystem::initialize() {
    if (!planner || !conceptFormation || !attention) {
        return false;
    }
    
    return true;
}

void CognitionSystem::update(TimestepDuration dt) {
    if (planner) {
        planner->update(dt);
    }
    if (conceptFormation) {
        conceptFormation->update(dt);
    }
    if (attention) {
        attention->update(dt);
    }
}

std::unique_ptr<Action> CognitionSystem::planAction() {
    if (planner) {
        return planner->planAction();
    }
    return nullptr;
}

void CognitionSystem::formConcept(const class Observation& observation) {
    if (conceptFormation) {
        conceptFormation->processObservation(observation);
    }
}

void CognitionSystem::focusAttention(const class Observation& observation) {
    if (attention) {
        attention->focus(observation);
    }
}

} // namespace nlm
