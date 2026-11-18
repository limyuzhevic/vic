// AgentBrain motor processing module
// Handles motor decoding and command selection

#include "AgentBrainMotor.hpp"
#include "MotorCommand.hpp"
#include "../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

// Motor command selection logic
MotorCommand AgentBrainMotor::selectMotorCommand(
    const std::vector<Neuron*>& motorNeurons,
    const std::vector<Neuron*>& motorForward,
    const std::vector<Neuron*>& motorBackward,
    const std::vector<Neuron*>& motorTurnLeft,
    const std::vector<Neuron*>& motorTurnRight,
    const std::vector<Neuron*>& motorInteract,
    const std::vector<Neuron*>& motorWait,
    bool curiosityEnabled,
    const MotorCommand& defaultCmd,
    float curiosityLevel,
    bool hasLastDecoded,
    const MotorCommand& lastDecoded,
    int stepsSinceTurn,
    int repeatCount,
    int consecutiveWinnerCount,
    float lastWinnerMargin,
    int lastWinnerGroup) {
    
    if (curiosityEnabled && curiosityLevel > 0.5f) {
        // Add some exploration based on curiosity
        return selectWithCuriosity(defaultCmd, curiosityLevel);
    }
    
    // Check if the same command has been repeating
    if (repeatCount > 5 && hasLastDecoded && lastDecoded == defaultCmd) {
        // Introduce some variety after too much repetition
        return selectWithCuriosity(defaultCmd, 0.3f);
    }
    
    // Apply curiosity-modulated selection
    return selectWithCuriosity(defaultCmd, curiosityLevel * 0.5f);
}

// Winner-take-all lateral inhibition
void AgentBrainMotor::applyWTAInhibition(
    const std::vector<Neuron*>& allMotorNeurons,
    int lastWinnerGroup,
    float lastWinnerMargin,
    float noveltyLevel,
    int consecutiveWinnerCount,
    int& wtaPulseCount,
    std::vector<Neuron*>& motorForward,
    std::vector<Neuron*>& motorBackward,
    std::vector<Neuron*>& motorTurnLeft,
    std::vector<Neuron*>& motorTurnRight,
    std::vector<Neuron*>& motorInteract,
    std::vector<Neuron*>& motorWait) {
    
    // Phase 5 winner-take-all lateral inhibition logic
    if (lastWinnerGroup_ >= 0 && lastWinnerGroup_ != 4 &&
        consecutiveWinnerCount_ < 3 &&
        lastWinnerMargin_ > 5.0f && noveltyLevel_ < 0.3f) {
        
        const std::vector<Neuron*>* groups[5] = {
            &motorForward, &motorBackward, &motorTurnLeft,
            &motorTurnRight, &motorInteract};
        
        for (int g = 0; g < 5; ++g) {
            if (g == lastWinnerGroup_) continue;
            for (Neuron* n : *groups[g]) {
                if (n) n->injectCurrent(-4.0f);
            }
        }
        
        wtaPulseCount_++;
    }
}

// Curiosity-modulated motor command selection
MotorCommand AgentBrainMotor::selectWithCuriosity(
    MotorCommand defaultCmd,
    float curiosityLevel) {
    
    if (curiosityLevel < 0.3f) {
        return defaultCmd;
    }
    
    // Simple curiosity-based exploration
    // In a real implementation, this would consider neural activity patterns
    // and select a different command based on potential information gain
    
    // For now, implement a simple random exploration
    // This would be replaced with more sophisticated neural mechanisms
    return defaultCmd; // Placeholder - TODO: Implement actual curiosity-based selection
}

// Decode motor command from neural activity
MotorCommand AgentBrainMotor::decodeFromMotorNeurons(
    const std::vector<Neuron*>& motorForward,
    const std::vector<Neuron*>& motorBackward,
    const std::vector<Neuron*>& motorTurnLeft,
    const std::vector<Neuron*>& motorTurnRight,
    const std::vector<Neuron*>& motorInteract,
    const std::vector<Neuron*>& motorWait,
    std::vector<float>& activities,
    std::vector<size_t>& refractoryCounts,
    std::vector<size_t>& sizes,
    std::vector<float>& firingRates) {
    
    // Calculate firing rates for each motor group
    float maxFiringRate = 0.0f;
    int winnerGroup = -1;
    
    // Group 0: MoveForward
    float sumForward = 0.0f, sumRefForward = 0.0f, sumSizeForward = 0;
    for (const Neuron* n : motorForward) {
        if (n) {
            sumForward += n->getFiringRate();
            sumRefForward += n->getRefractoryCount();
            sumSizeForward++;
        }
    }
    float firingForward = sumSizeForward > 0 ? sumForward / sumSizeForward : 0.0f;
    float refForward = sumRefForward > 0 ? sumRefForward / sumSizeForward : 0.0f;
    
    activities[0] = firingForward;
    refractoryCounts[0] = refForward;
    sizes[0] = motorForward.size();
    firingRates[0] = firingForward;
    
    if (firingForward > maxFiringRate) {
        maxFiringRate = firingForward;
        winnerGroup = 0;
    }
    
    // Group 1: MoveBackward
    float sumBackward = 0.0f, sumRefBackward = 0.0f, sumSizeBackward = 0;
    for (const Neuron* n : motorBackward) {
        if (n) {
            sumBackward += n->getFiringRate();
            sumRefBackward += n->getRefractoryCount();
            sumSizeBackward++;
        }
    }
    float firingBackward = sumSizeBackward > 0 ? sumBackward / sumSizeBackward : 0.0f;
    float refBackward = sumRefBackward > 0 ? sumRefBackward / sumSizeBackward : 0.0f;
    
    activities[1] = firingBackward;
    refractoryCounts[1] = refBackward;
    sizes[1] = motorBackward.size();
    firingRates[1] = firingBackward;
    
    if (firingBackward > maxFiringRate) {
        maxFiringRate = firingBackward;
        winnerGroup = 1;
    }
    
    // Group 2: TurnLeft
    float sumTurnLeft = 0.0f, sumRefTurnLeft = 0.0f, sumSizeTurnLeft = 0;
    for (const Neuron* n : motorTurnLeft) {
        if (n) {
            sumTurnLeft += n->getFiringRate();
            sumRefTurnLeft += n->getRefractoryCount();
            sumSizeTurnLeft++;
        }
    }
    float firingTurnLeft = sumSizeTurnLeft > 0 ? sumTurnLeft / sumSizeTurnLeft : 0.0f;
    float refTurnLeft = sumRefTurnLeft > 0 ? sumRefTurnLeft / sumSizeTurnLeft : 0.0f;
    
    activities[2] = firingTurnLeft;
    refractoryCounts[2] = refTurnLeft;
    sizes[2] = motorTurnLeft.size();
    firingRates[2] = firingTurnLeft;
    
    if (firingTurnLeft > maxFiringRate) {
        maxFiringRate = firingTurnLeft;
        winnerGroup = 2;
    }
    
    // Group 3: TurnRight
    float sumTurnRight = 0.0f, sumRefTurnRight = 0.0f, sumSizeTurnRight = 0;
    for (const Neuron* n : motorTurnRight) {
        if (n) {
            sumTurnRight += n->getFiringRate();
            sumRefTurnRight += n->getRefractoryCount();
            sumSizeTurnRight++;
        }
    }
    float firingTurnRight = sumSizeTurnRight > 0 ? sumTurnRight / sumSizeTurnRight : 0.0f;
    float refTurnRight = sumRefTurnRight > 0 ? sumRefTurnRight / sumSizeTurnRight : 0.0f;
    
    activities[3] = firingTurnRight;
    refractoryCounts[3] = refTurnRight;
    sizes[3] = motorTurnRight.size();
    firingRates[3] = firingTurnRight;
    
    if (firingTurnRight > maxFiringRate) {
        maxFiringRate = firingTurnRight;
        winnerGroup = 3;
    }
    
    // Group 4: Interact
    float sumInteract = 0.0f, sumRefInteract = 0.0f, sumSizeInteract = 0;
    for (const Neuron* n : motorInteract) {
        if (n) {
            sumInteract += n->getFiringRate();
            sumRefInteract += n->getRefractoryCount();
            sumSizeInteract++;
        }
    }
    float firingInteract = sumSizeInteract > 0 ? sumInteract / sumSizeInteract : 0.0f;
    float refInteract = sumRefInteract > 0 ? sumRefInteract / sumSizeInteract : 0.0f;
    
    activities[4] = firingInteract;
    refractoryCounts[4] = refInteract;
    sizes[4] = motorInteract.size();
    firingRates[4] = firingInteract;
    
    if (firingInteract > maxFiringRate) {
        maxFiringRate = firingInteract;
        winnerGroup = 4;
    }
    
    // Group 5: Wait
    float sumWait = 0.0f, sumRefWait = 0.0f, sumSizeWait = 0;
    for (const Neuron* n : motorWait) {
        if (n) {
            sumWait += n->getFiringRate();
            sumRefWait += n->getRefractoryCount();
            sumSizeWait++;
        }
    }
    float firingWait = sumSizeWait > 0 ? sumWait / sumSizeWait : 0.0f;
    float refWait = sumRefWait > 0 ? sumRefWait / sumSizeWait : 0.0f;
    
    activities[5] = firingWait;
    refractoryCounts[5] = refWait;
    sizes[5] = motorWait.size();
    firingRates[5] = firingWait;
    
    // Update winner tracking
    if (winnerGroup >= 0) {
        lastWinnerGroup = winnerGroup;
        lastWinnerMargin = maxFiringRate - firingRates[(winnerGroup + 1) % 6];
    } else {
        lastWinnerGroup = -1;
        lastWinnerMargin = 0.0f;
    }
    
    // Convert winner group to motor command
    switch (winnerGroup) {
        case 0: return MotorCommand::MoveForward;
        case 1: return MotorCommand::MoveBackward;
        case 2: return MotorCommand::TurnLeft;
        case 3: return MotorCommand::TurnRight;
        case 4: return MotorCommand::Interact;
        default: return MotorCommand::Wait;
    }
}

} // namespace nlm
