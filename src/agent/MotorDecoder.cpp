#include "MotorDecoder.hpp"
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

MotorDecoder::MotorDecoder(std::shared_ptr<Brain> brain)
    : brain_(std::move(brain))
    , curiosityLevel_(0.0f)
    , activityThreshold_(0.5f)
    , explorationProbability_(1.0f)
    , curiosityEnabled_(true)
{
    if (!brain_) {
        throw std::invalid_argument("MotorDecoder requires a valid brain pointer");
    }
}

MotorCommand MotorDecoder::decodeMotorCommand() {
    if (!brain_) {
        return MotorCommand::Wait;
    }
    
    MotorCommand decoded = selectWithCuriosity(MotorCommand::Wait);
    
    // Apply curiosity-based exploration
    if (curiosityEnabled_ && curiosityLevel_ > 0.3f) {
        decoded = selectWithCuriosity(decoded);
    }
    
    return validateCommand(decoded);
}

void MotorDecoder::updateCuriosityLevel(float noveltyLevel, float predictionError) {
    curiosityLevel_ = noveltyLevel * 2.0f + std::abs(predictionError) * 0.5f;
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
}

void MotorDecoder::reset() {
    curiosityLevel_ = 0.0f;
}

float MotorDecoder::calculateActivity(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) {
        return 0.0f;
    }
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

MotorCommand MotorDecoder::selectWithCuriosity(MotorCommand defaultCmd) {
    if (curiosityLevel_ > 0.5f) {
        float exploreChance = curiosityLevel_ * 0.3f * explorationProbability_;
        
        float r = brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
        if (r < exploreChance) {
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
    }
    
    return defaultCmd;
}

MotorCommand MotorDecoder::validateCommand(MotorCommand cmd) const {
    return cmd; // All commands are valid in this implementation
}

} // namespace nlm
