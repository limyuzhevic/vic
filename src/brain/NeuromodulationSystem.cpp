// Neuromodulation system implementation - handles dopamine, curiosity, novelty, prediction error
#include "NeuromodulationSystem.hpp"
#include "Brain.hpp"
#include <memory>

namespace nlm {

NeuromodulationSystem::NeuromodulationSystem(Brain& brainRef) : brain(brainRef) {
    dopamine = std::make_unique<Dopamine>();
    curiosity = std::make_unique<Curiosity>();
    novelty = std::make_unique<Novelty>();
    predictionError = std::make_unique<PredictionError>();
}

NeuromodulationSystem::~NeuromodulationSystem() = default;

bool NeuromodulationSystem::initialize() {
    if (!dopamine || !curiosity || !novelty || !predictionError) {
        return false;
    }
    
    return true;
}

void NeuromodulationSystem::update(TimestepDuration dt) {
    if (dopamine) dopamine->update(dt);
    if (curiosity) curiosity->update(dt);
    if (novelty) novelty->update(dt);
    if (predictionError) predictionError->update(dt);
}

float NeuromodulationSystem::getDopamineLevel() const {
    return dopamine ? dopamine->getLevel() : 0.0f;
}

float NeuromodulationSystem::getCuriosityLevel() const {
    return curiosity ? curiosity->getLevel() : 0.0f;
}

float NeuromodulationSystem::getNoveltyLevel() const {
    return novelty ? novelty->getLevel() : 0.0f;
}

float NeuromodulationSystem::getPredictionErrorLevel() const {
    return predictionError ? predictionError->getLevel() : 0.0f;
}

void NeuromodulationSystem::applyToBrain() {
    float dopamineLevel = getDopamineLevel();
    float curiosityLevel = getCuriosityLevel();
    float noveltyLevel = getNoveltyLevel();
    
    // Apply dopamine effects on neural excitability
    if (dopamineLevel > 0.0f && brain.getDopamine()) {
        brain.injectCurrentToNeurons(NeuronType::Internal, dopamineLevel * 0.5f);
    }
    
    // Apply curiosity effects on exploration
    if (curiosityLevel > 0.0f) {
        // Encourage exploration
    }
    
    // Apply novelty effects on learning
    if (noveltyLevel > 0.0f) {
        // Enhance learning rate
    }
}

} // namespace nlm
