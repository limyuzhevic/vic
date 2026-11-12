#pragma once

#include "../brain/Neuron.hpp"
#include <vector>
#include <string>

namespace nlm {

class AgentBrainCore;

class AgentBrainProcessing {
public:
    static std::vector<MotorCommand::MoveForward> processSensoryInput(
        const SensoryPercept& percept,
        const std::vector<Neuron*>& sensoryVision,
        const std::vector<Neuron*>& sensoryTouch,
        const std::vector<Neuron*>& sensoryInternal,
        const std::vector<Neuron*>& sensoryProprioception,
        float& maxTouch,
        float& forwardTouch,
        float& forwardObjectVision,
        float& noveltyLevel,
        float sensoryNoveltyDecay,
        std::vector<float>& predictedVision,
        const std::string& text);
        
    static MotorCommand decodeMotorCommand(
        const std::vector<Neuron*>& motorForward,
        const std::vector<Neuron*>& motorBackward,
        const std::vector<Neuron*>& motorTurnLeft,
        const std::vector<Neuron*>& motorTurnRight,
        const std::vector<Neuron*>& motorInteract,
        const std::vector<Neuron*>& motorWait,
        const std::vector<float>& lastVision,
        float forwardTouch,
        float forwardObjectVision,
        float maxTouch,
        float noveltyLevel,
        int& lastWinnerGroup,
        float& lastWinnerMargin,
        int& consecutiveWinnerCount,
        int stepsSinceTurn,
        int repeatCount,
        MotorCommand lastDecoded,
        bool hasLastDecoded,
        const std::vector<AgentBrain::MotorGroupActivity>& activities,
        float& forwardRate,
        float& backwardRate,
        float& turnLeftRate,
        float& turnRightRate,
        float& interactRate);
};

} // namespace nlm
