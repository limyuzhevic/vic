// Brain cognition implementation - handles cognition system integration
#include "BrainCognition.hpp"
#include "Brain.hpp"
#include "CognitionSystem.hpp"
#include <stdexcept>

namespace nlm {

BrainCognition::BrainCognition(std::shared_ptr<Config> config) : Brain(config) {
    cognitionSystem = std::make_unique<CognitionSystem>(*this);
}

BrainCognition::~BrainCognition() = default;

bool BrainCognition::initializeCognitionSystems() {
    if (!cognitionSystem) {
        return false;
    }
    
    return cognitionSystem->initialize();
}

void BrainCognition::updateCognitionSystems(TimestepDuration dt) {
    if (cognitionSystem) {
        cognitionSystem->update(dt);
    }
}

bool BrainCognition::initialize() {
    if (!Brain::initialize()) {
        return false;
    }
    
    if (!initializeCognitionSystems()) {
        return false;
    }
    
    return true;
}

void BrainCognition::step(SimulationStep currentStep) {
    Brain::step(currentStep);
}

void BrainCognition::step(SimulationStep currentStep, Timestamp currentTime) {
    Brain::step(currentStep, currentTime);
    
    if (cognitionSystem) {
        cognitionSystem->update(static_cast<TimestepDuration>(currentTime));
    }
}

CognitionSystem* BrainCognition::getCognitionSystem() {
    return cognitionSystem.get();
}

} // namespace nlm
