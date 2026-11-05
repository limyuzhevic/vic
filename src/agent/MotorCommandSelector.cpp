// MotorCommandSelector.cpp - Motor command selection implementation

#include "MotorCommandSelector.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

MotorCommand MotorCommandSelector::selectCommand(const std::vector<AgentBrain::MotorGroupActivity>& activities,
                                               const AgentBrain& agent,
                                               float noveltyLevel,
                                               float forwardTouch,
                                               float forwardObjectVision,
                                               float maxTouch,
                                               float learnedPreference) {
    // Phase 5 winner-take-all state for the next presentation
    int bestIdx = -1;
    float bestMargin = 0.0f;
    calculateWinner(activities, bestIdx, bestMargin);
    
    // Phase 6 planner IN the loop (was advisory only)
    MotorCommand preBiasBest = MotorCommand::Wait;
    if (bestIdx >= 0 && bestIdx < static_cast<int>(activities.size())) {
        preBiasBest = getMotorCommandFromGroup(bestIdx);
    }
    
    // Apply various biases to the selected command
    MotorCommand selected = preBiasBest;
    
    // Apply vision gating (bright-approach phototaxis)
    if (maxTouch <= 0.05f && forwardObjectVision > 0.3f && 
        forwardObjectVision < 0.95f) {
        if (preBiasBest == MotorCommand::MoveForward) {
            // Forward already selected, nothing to do
        } else {
            // Re-evaluate with vision bonus
            selected = evaluateWithVisionGating(preBiasBest, forwardObjectVision, activities);
        }
    }
    
    // Apply interact gating
    if (maxTouch <= 0.05f) {
        if (selected == MotorCommand::Interact) {
            selected = MotorCommand::Wait; // Suppress interact when nothing is within range
        }
    }
    
    // Apply resource/hazard cues
    const bool resourceCue = forwardTouch > 0.05f &&
                           forwardObjectVision > 0.5f &&
                           forwardObjectVision < 0.99f;
    const bool hazardCue = forwardTouch > 0.05f &&
                          forwardObjectVision < 0.35f;
    
    if (resourceCue && noveltyLevel > 0.02f) {
        // Resource interaction bonus
        if (selected == MotorCommand::Interact) {
            // Already interacting with resource
        } else if (selected == MotorCommand::Wait) {
            // Could potentially interact
        }
    } else if (hazardCue) {
        // Hazard avoidance - suppress interact, boost turns
        if (selected == MotorCommand::Interact) {
            selected = MotorCommand::Wait;
        }
        // Turn away from hazard
        if (forwardObjectVision < 0.35f) {
            // Prefer turning over waiting
            selected = selectTurnDirection(activities);
        }
    }
    
    // Apply learned preference bias
    if (learnedPreference > 0.1f || learnedPreference < -0.1f) {
        // Small bias toward or away from Interact based on learned preference
        if (selected == MotorCommand::Wait && std::abs(learnedPreference) > 0.05f) {
            selected = MotorCommand::Interact;
        }
    }
    
    // Final fallback - if no command selected, use wait
    if (selected == MotorCommand::Wait && bestIdx >= 0) {
        selected = preBiasBest;
    }
    
    return selected;
}

void MotorCommandSelector::applyVisionGating(float& forwardRate, float& backwardRate,
                                             float& turnLeftRate, float& turnRightRate,
                                             float& interactRate,
                                             float maxTouch, float forwardObjectVision) {
    // Phase 5 bright-approach (phototaxis), widened in Phase 6:
    // a bright object in the forward vision direction, before
    // touch range, biases Forward by +10 Hz (was +6). The old
    // >0.5 threshold only saw a resource within ~3 units
    // ((8-3)/8*0.8 = 0.5); at 5 units a resource reads 0.3, so
    // the agent walked past distant resources without ever
    // steering toward them — the main remaining conversion gap
    // once every action actually executes. The >0.3 floor still
    // excludes hazards (their encoding tops out at 0.3, at
    // touch range) while admitting resources out to ~5 units.
    // A far wall also reads bright (distance/8), so this can
    // still walk the agent toward a wall; as it approaches,
    // the wall's vision value falls below 0.35, the hazard cue
    // takes over, and turn diversity breaks the approach.
    if (maxTouch <= 0.05f && forwardObjectVision > 0.3f &&
        forwardObjectVision < 0.95f) {
        forwardRate += config_.forwardBonus;
    }
}

void MotorCommandSelector::applyMovementPenalties(float& forwardRate, float& backwardRate,
                                                   const AgentBrain& agent) {
    // Phase 5 momentum bias: reversing direction on the very next
    // step is penalised by 8 Hz. Phase 4's dominant failure was
    // forward/backward alternation (80-98 oscillations per 200-step
    // episode in the Phase 5 baseline measurement), which keeps the
    // agent in 2-6 cells all episode. A reversal penalty is movement
    // inertia, not a pathfinder: the brain can still reverse when its
    // motor pattern is decisive, but a near-tie no longer flips it
    // back and forth every step.
    
    if (agent.hasLastDecoded() && agent.getLastDecoded() == MotorCommand::MoveForward) {
        backwardRate = std::max(0.0f, backwardRate - config_.momentumBias);
    } else if (agent.hasLastDecoded() && agent.getLastDecoded() == MotorCommand::MoveBackward) {
        forwardRate = std::max(0.0f, forwardRate - config_.momentumBias);
    }
}

MotorCommand MotorCommandSelector::applyCuriosity(const MotorCommand& defaultCmd, float curiosityLevel,
                                                  std::mt19937& rng) {
    // Exploration: occasionally choose random action, with probability
    // proportional to curiosity. Phase 4: the old 0.5 hard gate meant
    // exploration never fired once novelty fell below 0.25 (which was
    // every step after step 0, given the novelty bug). Now a typical
    // moving curiosity of ~0.14 gives ~4% random actions, a novel scene
    // at curiosity ~0.85 gives ~25%, and a fully familiar, stationary
    // scene (curiosity ~0) gives none — exploration scales with novelty
    // instead of being on/off behind an unreachable threshold.
    
    float exploreChance = curiosityLevel * 0.3f;  // Up to 30% random
    
    float r = std::uniform_real_distribution<float>(0.0f, 1.0f)(rng);
    if (r < exploreChance) {
        // Random motor command
        int choice = std::uniform_int_distribution<int>(0, 7)(rng);
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
    
    return defaultCmd;
}

void MotorCommandSelector::calculateWinner(const std::vector<AgentBrain::MotorGroupActivity>& activities,
                                         int& bestIdx, float& bestMargin) {
    float best = 0.0f, second = 0.0f;
    bestIdx = -1;
    
    for (int i = 0; i < 5 && i < static_cast<int>(activities.size()); ++i) {
        float r = activities[i].firingRate;
        if (r > best) {
            second = best;
            best = r;
            bestIdx = i;
        } else if (r > second) {
            second = r;
        }
    }
    
    bestMargin = best - second;
}

MotorCommand MotorCommandSelector::getMotorCommandFromGroup(int groupIdx) const {
    switch (groupIdx) {
        case 0: return MotorCommand::MoveForward;
        case 1: return MotorCommand::MoveBackward;
        case 2: return MotorCommand::TurnLeft;
        case 3: return MotorCommand::TurnRight;
        case 4: return MotorCommand::Interact;
        default: return MotorCommand::Wait;
    }
}

bool MotorCommandSelector::isMovementCommand(MotorCommand cmd) const {
    return cmd == MotorCommand::MoveForward || 
           cmd == MotorCommand::MoveBackward ||
           cmd == MotorCommand::TurnLeft ||
           cmd == MotorCommand::TurnRight;
}

float MotorCommandSelector::calculateDistance(float forwardVision, float forwardTouch) const {
    // Combine vision and touch distance information
    float visionDist = (1.0f - forwardVision) * 8.0f;  // Max 8 units
    float touchDist = forwardTouch > 0.05f ? 1.5f : 8.0f;  // Touch range ~1.5 units
    return std::min(visionDist, touchDist);
}

MotorCommand MotorCommandSelector::evaluateWithVisionGating(MotorCommand currentCmd, 
                                                          float forwardObjectVision,
                                                          const std::vector<AgentBrain::MotorGroupActivity>& activities) {
    // If we have a current command, evaluate whether vision gating should override it
    if (currentCmd == MotorCommand::Wait) {
        // No movement selected, use wait
        return currentCmd;
    }
    
    // For movement commands, check if vision suggests a different action
    int currentIdx = -1;
    for (int i = 0; i < 5; ++i) {
        if (getMotorCommandFromGroup(i) == currentCmd) {
            currentIdx = i;
            break;
        }
    }
    
    // If the current command is forward, check if we should turn instead
    if (currentCmd == MotorCommand::MoveForward && forwardObjectVision < 0.35f) {
        // Hazard ahead - turn instead
        return selectTurnDirection(activities);
    }
    
    return currentCmd;
}

MotorCommand MotorCommandSelector::selectTurnDirection(const std::vector<AgentBrain::MotorGroupActivity>& activities) {
    // Prefer turning left or right based on which is stronger
    int bestTurnIdx = -1;
    float bestTurnRate = 0.0f;
    
    // Check turn left and turn right groups
    for (int i = 2; i <= 3; ++i) {
        if (i < static_cast<int>(activities.size()) && 
            activities[i].firingRate > bestTurnRate) {
            bestTurnRate = activities[i].firingRate;
            bestTurnIdx = i;
        }
    }
    
    if (bestTurnIdx >= 0) {
        return getMotorCommandFromGroup(bestTurnIdx);
    }
    
    // Default to turn left if both turns are equal or zero
    return MotorCommand::TurnLeft;
}

} // namespace nlm
