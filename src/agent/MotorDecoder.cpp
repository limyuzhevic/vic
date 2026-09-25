#include "MotorDecoder.hpp"
#include "../brain/Neuron.hpp"
#include "../core/Constants.hpp"

namespace nlm {

MotorDecoder::MotorDecoder() {}

MotorDecoder::~MotorDecoder() {}

float MotorDecoder::calculateActivity(const std::vector<Neuron*>& neurons) {
    if (neurons.empty()) return 0.0f;
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

MotorCommand MotorDecoder::selectCommandFromActivity(
    float forwardAct, float backwardAct, float leftAct, 
    float rightAct, float interactAct, float waitAct) {
    // Find maximum activity
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardAct},
        {MotorCommand::MoveBackward, backwardAct},
        {MotorCommand::TurnLeft, leftAct},
        {MotorCommand::TurnRight, rightAct},
        {MotorCommand::Interact, interactAct},
        {MotorCommand::Wait, waitAct}
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = waitAct;  // Default to wait if nothing stronger
    
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    // Only act if there's meaningful activity
    if (bestActivity < MotorConstants::MOTOR_ACTIVITY_THRESHOLD) {
        return MotorCommand::Wait;
    }
    
    return best;
}

bool MotorDecoder::isMeaningfulActivity(float activity, float threshold) {
    return activity >= threshold;
}

} // namespace nlm