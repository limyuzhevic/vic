#include "MotorDecoder.hpp"

namespace nlm {

MotorDecoder::MotorDecoder() : activityThreshold_(0.5f) { }

MotorDecoder::~MotorDecoder() = default;

void MotorDecoder::setMotorGroups(
    const std::vector<Neuron*>& motorForward,
    const std::vector<Neuron*>& motorBackward,
    const std::vector<Neuron*>& motorTurnLeft,
    const std::vector<Neuron*>& motorTurnRight,
    const std::vector<Neuron*>& motorInteract,
    const std::vector<Neuron*>& motorWait
) {
    motorForward_ = motorForward;
    motorBackward_ = motorBackward;
    motorTurnLeft_ = motorTurnLeft;
    motorTurnRight_ = motorTurnRight;
    motorInteract_ = motorInteract;
    motorWait_ = motorWait;
}

MotorCommand MotorDecoder::decodeMotorCommand() {
    // Calculate average activity in each motor group
    float forwardAct = calculateActivity(motorForward_);
    float backwardAct = calculateActivity(motorBackward_);
    float leftAct = calculateActivity(motorTurnLeft_);
    float rightAct = calculateActivity(motorTurnRight_);
    float interactAct = calculateActivity(motorInteract_);
    float waitAct = calculateActivity(motorWait_);
    
    // Store for debugging
    lastForwardActivity_ = forwardAct;
    lastBackwardActivity_ = backwardAct;
    lastLeftActivity_ = leftAct;
    lastRightActivity_ = rightAct;
    lastInteractActivity_ = interactAct;
    lastWaitActivity_ = waitAct;
    
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
    if (bestActivity < activityThreshold_) {
        NLM_LOG_DEBUG("MotorDecoder: Activity " + std::to_string(bestActivity) + 
                     " below threshold " + std::to_string(activityThreshold_) + ", selecting Wait");
        return MotorCommand::Wait;
    }
    
    NLM_LOG_DEBUG("MotorDecoder: Best command " + std::to_string(static_cast<int>(best)) + 
                 " with activity " + std::to_string(bestActivity));
    return best;
}

float MotorDecoder::calculateActivity(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) return 0.0f;
    float sum = 0.0f;
    for (Neuron* n : neurons) {
        // Use membrane potential deviation from rest as activity measure
        sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
    }
    return sum / neurons.size();
}

void MotorDecoder::reset() {
    // Clear neuron group references (not strictly necessary as vectors will handle this)
    motorForward_.clear();
    motorBackward_.clear();
    motorTurnLeft_.clear();
    motorTurnRight_.clear();
    motorInteract_.clear();
    motorWait_.clear();
    
    // Reset debug values
    lastForwardActivity_ = 0.0f;
    lastBackwardActivity_ = 0.0f;
    lastLeftActivity_ = 0.0f;
    lastRightActivity_ = 0.0f;
    lastInteractActivity_ = 0.0f;
    lastWaitActivity_ = 0.0f;
}

} // namespace nlm
