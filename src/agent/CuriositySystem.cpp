#include "CuriositySystem.hpp"
#include "../brain/Neuron.hpp"
#include <algorithm>

namespace nlm {

void CuriositySystem::initialize() {
    curiosityLevel_ = 0.0f;
}

void CuriositySystem::updateCuriosity(
    float noveltyLevel,
    float predictionError,
    bool curiosityEnabled,
    float& curiosityLevel) {
    
    if (!validateCuriosityParams(noveltyLevel, predictionError)) {
        return;
    }
    
    if (curiosityEnabled) {
        curiosityLevel_ = noveltyLevel * AgentBrainConstants::CURIOSITY_NOVELTY_WEIGHT + 
                         std::abs(predictionError) * AgentBrainConstants::CURIOSITY_PREDICTION_ERROR_WEIGHT;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
        curiosityLevel = curiosityLevel_;
    }
}

float CuriositySystem::calculateExplorationChance(float curiosityLevel) {
    return curiosityLevel * 0.3f;  // Up to 30% random
}

MotorCommand CuriositySystem::generateRandomMotorCommand(class RandomGenerator* rng) {
    if (!rng) {
        return MotorCommand::Wait;
    }
    
    int choice = rng->uniformInt(0, 7);
    switch (choice) {
        case 0: return MotorCommand::MoveForward;
        case 1: return MotorCommand::MoveBackward;
        case 2: return MotorCommand::TurnLeft;
        case 3: return MotorCommand::TurnRight;
        case 4: return MotorCommand::LookLeft;
        case 5: return MotorCommand::LookRight;
        case 6: return MotorCommand::Interact;
        default: return MotorCommand::Wait;
    }
}

bool CuriositySystem::validateCuriosityParams(float noveltyLevel, float predictionError) {
    // Check for NaN or infinite values
    if (std::isnan(noveltyLevel) || std::isnan(predictionError) ||
        std::isinf(noveltyLevel) || std::isinf(predictionError)) {
        return false;
    }
    return true;
}

} // namespace nlm