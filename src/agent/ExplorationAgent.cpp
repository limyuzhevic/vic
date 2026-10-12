#include "ExplorationAgent.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

ExplorationAgent::ExplorationAgent(std::shared_ptr<Config> config)
    : config_(config)
    , curiosityLevel_(0.0f)
    , noveltyLevel_(0.0f)
    , predictionError_(0.0f)
    , expectedReward_(0.0f)
    , curiosityEnabled_(true)
{
}

ExplorationAgent::~ExplorationAgent() = default;

void ExplorationAgent::initialize(std::shared_ptr<Brain> brain) {
    brain_ = brain;
}

void ExplorationAgent::update(double timestep, float noveltyLevel) {
    noveltyLevel_ = noveltyLevel;
    
    // Update curiosity based on novelty and prediction error
    if (curiosityEnabled_) {
        curiosityLevel_ = noveltyLevel_ * 2.0f + std::abs(predictionError_) * 0.5f;
        curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    }
}

bool ExplorationAgent::shouldExplore(float curiosityThreshold) const {
    return curiosityLevel_ > curiosityThreshold;
}

MotorCommand ExplorationAgent::generateExplorationAction() {
    return generateRandomAction();
}

void ExplorationAgent::reset() {
    curiosityLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    predictionError_ = 0.0f;
    expectedReward_ = 0.0f;
}

MotorCommand ExplorationAgent::generateRandomAction() {
    if (!brain_) return MotorCommand::Wait;
    
    int choice = brain_->getRandomGenerator()->uniformInt(0, 7);
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

} // namespace nlm
