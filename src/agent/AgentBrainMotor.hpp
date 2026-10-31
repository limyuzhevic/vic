#pragma once

#include "AgentBrainCore.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>

namespace nlm {

class AgentBrainMotor {
public:
    AgentBrainMotor(AgentBrainCore* parent) : parent_(parent) {}
    
    // Motor neuron distribution
    void distributeMotorNeurons(const Brain* brain);
    
    // Decode motor command from neurons
    MotorCommand decodeMotorCommand();
    
    // Get motor group activities
    std::vector<AgentBrainCore::MotorGroupActivity> getMotorGroupActivities() const;
    
    // Get last winner group
    int getLastWinnerGroup() const { return parent_->lastWinnerGroup_; }
    
    // Set last winner group
    void setLastWinnerGroup(int group) { parent_->lastWinnerGroup_ = group; }
    
    // Get last winner margin
    float getLastWinnerMargin() const { return parent_->lastWinnerMargin_; }
    
    // Set last winner margin
    void setLastWinnerMargin(float margin) { parent_->lastWinnerMargin_ = margin; }
    
    // Get consecutive winner count
    int getConsecutiveWinnerCount() const { return parent_->consecutiveWinnerCount_; }
    
    // Increment consecutive winner count
    void incrementConsecutiveWinnerCount() { parent_->consecutiveWinnerCount_++; }
    
    // Reset consecutive winner count
    void resetConsecutiveWinnerCount() { parent_->consecutiveWinnerCount_ = 1; }
    
    // Get last decoded command
    MotorCommand getLastDecoded() const { return parent_->lastDecoded_; }
    
    // Set last decoded command
    void setLastDecoded(MotorCommand cmd) { parent_->lastDecoded_ = cmd; }
    
    // Check if has last decoded
    bool hasLastDecoded() const { return parent_->hasLastDecoded_; }
    
    // Set has last decoded
    void setHasLastDecoded(bool has) { parent_->hasLastDecoded_ = has; }
    
    // Get steps since turn
    int getStepsSinceTurn() const { return parent_->stepsSinceTurn_; }
    
    // Increment steps since turn
    void incrementStepsSinceTurn() { parent_->stepsSinceTurn_++; }
    
    // Reset steps since turn
    void resetStepsSinceTurn() { parent_->stepsSinceTurn_ = 0; }
    
    // Get repeat count
    int getRepeatCount() const { return parent_->repeatCount_; }
    
    // Set repeat count
    void setRepeatCount(int count) { parent_->repeatCount_ = count; }
    
    // Get max touch
    float getMaxTouch() const { return parent_->maxTouch_; }
    
    // Set max touch
    void setMaxTouch(float touch) { parent_->maxTouch_ = touch; }
    
    // Get forward touch
    float getForwardTouch() const { return parent_->forwardTouch_; }
    
    // Set forward touch
    void setForwardTouch(float touch) { parent_->forwardTouch_ = touch; }
    
    // Get forward object vision
    float getForwardObjectVision() const { return parent_->forwardObjectVision_; }
    
    // Set forward object vision
    void setForwardObjectVision(float vision) { parent_->forwardObjectVision_ = vision; }
    
    // Get novelty level
    float getNoveltyLevel() const { return parent_->noveltyLevel_; }
    
    // Compute motor command with all biases applied
    MotorCommand computeMotorCommandWithBiases(const std::vector<AgentBrainCore::MotorGroupActivity>& activities);
    
    // Apply curiosity-based exploration
    MotorCommand applyCuriosity(MotorCommand defaultCmd);
    
    // Apply diversity counters
    void applyDiversityCounters(MotorCommand decoded);
    
    // Check if resource cue
    bool checkResourceCue() const;
    
    // Check if hazard cue
    bool checkHazardCue() const;
    
    // Apply vision-gated interact reflex
    void applyVisionGatedInteractReflex(float& interactRate);
    
    // Apply turn diversity / repetition fatigue
    void applyTurnDiversityAndRepetitionFatigue(MotorCommand& current);
    
    // Apply momentum bias
    void applyMomentumBias(MotorCommand lastDecoded, float& forwardRate, float& backwardRate);
    
    // Check if should wait
    bool shouldWait(float bestActivity) const;
    
    // Update winner-take-all state
    void updateWinnerTakeAllState(const std::vector<AgentBrainCore::MotorGroupActivity>& activities);
    
private:
    AgentBrainCore* parent_;
};

inline void AgentBrainMotor::distributeMotorNeurons(const Brain* brain) {
    if (!brain) return;
    
    parent_->motorForward_.clear();
    parent_->motorBackward_.clear();
    parent_->motorTurnLeft_.clear();
    parent_->motorTurnRight_.clear();
    parent_->motorInteract_.clear();
    parent_->motorWait_.clear();
    
    for (const auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            NeuronType type = pop->getNeuronType();
            if (type == NeuronType::Motor) {
                for (Neuron* n : pop->getNeurons()) {
                    size_t idx = parent_->motorForward_.size() + parent_->motorBackward_.size() + 
                                parent_->motorTurnLeft_.size() + parent_->motorTurnRight_.size() +
                                parent_->motorInteract_.size() + parent_->motorWait_.size();
                    
                    switch (idx % 6) {
                        case 0: parent_->motorForward_.push_back(n); break;
                        case 1: parent_->motorBackward_.push_back(n); break;
                        case 2: parent_->motorTurnLeft_.push_back(n); break;
                        case 3: parent_->motorTurnRight_.push_back(n); break;
                        case 4: parent_->motorInteract_.push_back(n); break;
                        case 5: parent_->motorWait_.push_back(n); break;
                    }
                }
            }
        }
    }
}

inline std::vector<AgentBrainCore::MotorGroupActivity> AgentBrainMotor::getMotorGroupActivities() const {
    auto calc = [](const std::vector<Neuron*>& neurons, const char* name) {
        AgentBrainCore::MotorGroupActivity out{name, 0.0f, 0, neurons.size()};
        if (neurons.empty()) return out;
        float sum = 0.0f;
        float rateSum = 0.0f;
        for (const Neuron* n : neurons) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
            if (n->getState().firingState == FiringState::Refractory) ++out.refractoryCount;
            rateSum += n->getFiringRate();
        }
        out.activity = sum / static_cast<float>(neurons.size());
        out.firingRate = rateSum / static_cast<float>(neurons.size());
        return out;
    };
    return {
        calc(parent_->motorForward_, "forward"),
        calc(parent_->motorBackward_, "backward"),
        calc(parent_->motorTurnLeft_, "left"),
        calc(parent_->motorTurnRight_, "right"),
        calc(parent_->motorInteract_, "interact"),
        calc(parent_->motorWait_, "wait"),
    };
}

inline MotorCommand AgentBrainMotor::computeMotorCommandWithBiases(const std::vector<AgentBrainCore::MotorGroupActivity>& activities) {
    float interactRate = activities[4].firingRate;
    float turnLeftRate = activities[2].firingRate;
    float turnRightRate = activities[3].firingRate;
    float forwardRate = activities[0].firingRate;
    float backwardRate = activities[1].firingRate;
    
    const bool resourceCue = parent_->forwardTouch_ > 0.05f &&
                             parent_->forwardObjectVision_ > 0.5f &&
                             parent_->forwardObjectVision_ < 0.99f;
    const bool hazardCue = parent_->forwardTouch_ > 0.05f &&
                           parent_->forwardObjectVision_ < 0.35f;
    
    if (parent_->maxTouch_ <= 0.05f && parent_->forwardObjectVision_ > 0.5f &&
        parent_->forwardObjectVision_ < 0.95f) {
        forwardRate += 6.0f;
    }
    
    if (parent_->maxTouch_ <= 0.05f) {
        interactRate = std::max(0.0f, interactRate - 12.0f);
    }
    
    if (resourceCue && interactRate > 0.1f && parent_->noveltyLevel_ > 0.02f &&
        !(parent_->hasLastDecoded_ && parent_->lastDecoded_ == MotorCommand::Interact)) {
        interactRate += 16.0f;
        parent_->reflexBonusCount_++;
    } else if (hazardCue) {
        interactRate = std::max(0.0f, interactRate - 12.0f);
        turnLeftRate += 8.0f;
        turnRightRate += 8.0f;
        parent_->reflexSuppressCount_++;
    }
    
    if (brain_) {
        interactRate += brain_->getLearnedPreference() * 5.0f;
    }
    
    if (parent_->stepsSinceTurn_ > 25 &&
        (forwardRate > backwardRate || backwardRate > forwardRate)) {
        turnLeftRate += 6.0f;
        turnRightRate += 6.0f;
    }
    
    if (parent_->repeatCount_ >= 3) {
        if (parent_->hasLastDecoded_ && parent_->lastDecoded_ == MotorCommand::MoveForward) {
            forwardRate = std::max(0.0f, forwardRate - 8.0f);
        } else if (parent_->hasLastDecoded_ && parent_->lastDecoded_ == MotorCommand::MoveBackward) {
            backwardRate = std::max(0.0f, backwardRate - 8.0f);
        }
    }
    
    if (parent_->hasLastDecoded_ && parent_->lastDecoded_ == MotorCommand::MoveForward) {
        backwardRate = std::max(0.0f, backwardRate - 8.0f);
    } else if (parent_->hasLastDecoded_ && parent_->lastDecoded_ == MotorCommand::MoveBackward) {
        forwardRate = std::max(0.0f, forwardRate - 8.0f);
    }
    
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, forwardRate},
        {MotorCommand::MoveBackward, backwardRate},
        {MotorCommand::TurnLeft, turnLeftRate},
        {MotorCommand::TurnRight, turnRightRate},
        {MotorCommand::Interact, interactRate},
    };
    
    MotorCommand best = MotorCommand::Wait;
    float bestActivity = 0.0f;
    for (const auto& c : commands) {
        if (c.activity > bestActivity) {
            bestActivity = c.activity;
            best = c.cmd;
        }
    }
    
    if (bestActivity <= 0.1f) {
        return MotorCommand::Wait;
    }
    
    return best;
}

inline MotorCommand AgentBrainMotor::applyCuriosity(MotorCommand defaultCmd) {
    if (!parent_->curiosityEnabled_) return defaultCmd;
    
    float exploreChance = parent_->curiosityLevel_ * 0.3f;
    
    float r = parent_->brain_->getRandomGenerator()->uniformReal(0.0f, 1.0f);
    if (r < exploreChance) {
        int choice = brain_->getRandomGenerator()->uniformInt(uint32_t(0), uint32_t(7));
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

inline void AgentBrainMotor::applyDiversityCounters(MotorCommand decoded) {
    if (decoded == MotorCommand::TurnLeft || decoded == MotorCommand::TurnRight) {
        parent_->stepsSinceTurn_ = 0;
    } else {
        parent_->stepsSinceTurn_++;
    }
    
    if (parent_->hasLastDecoded_ && decoded == parent_->lastDecoded_ &&
        (decoded == MotorCommand::MoveForward || decoded == MotorCommand::MoveBackward)) {
        parent_->repeatCount_++;
    } else {
        parent_->repeatCount_ = 1;
    }
}

inline bool AgentBrainMotor::checkResourceCue() const {
    return parent_->forwardTouch_ > 0.05f &&
           parent_->forwardObjectVision_ > 0.5f &&
           parent_->forwardObjectVision_ < 0.99f;
}

inline bool AgentBrainMotor::checkHazardCue() const {
    return parent_->forwardTouch_ > 0.05f &&
           parent_->forwardObjectVision_ < 0.35f;
}

inline void AgentBrainMotor::applyVisionGatedInteractReflex(float& interactRate) {
    if (parent_->maxTouch_ <= 0.05f) {
        interactRate = std::max(0.0f, interactRate - 12.0f);
    }
    
    if (checkResourceCue() && interactRate > 0.1f && parent_->noveltyLevel_ > 0.02f &&
        !(parent_->hasLastDecoded_ && parent_->lastDecoded_ == MotorCommand::Interact)) {
        interactRate += 16.0f;
        parent_->reflexBonusCount_++;
    } else if (checkHazardCue()) {
        interactRate = std::max(0.0f, interactRate - 12.0f);
        // Note: turnLeftRate and turnRightRate are handled in the calling function
    }
}

inline void AgentBrainMotor::applyTurnDiversityAndRepetitionFatigue(MotorCommand& current) {
    if (parent_->stepsSinceTurn_ > 25 &&
        (current == MotorCommand::MoveForward || current == MotorCommand::MoveBackward)) {
        // Note: turnLeft and turnRight rates are handled in the calling function
    }
    
    if (parent_->repeatCount_ >= 3 && parent_->hasLastDecoded_) {
        if (parent_->lastDecoded_ == MotorCommand::MoveForward) {
            // Note: forwardRate is modified in the calling function
        } else if (parent_->lastDecoded_ == MotorCommand::MoveBackward) {
            // Note: backwardRate is modified in the calling function
        }
    }
}

inline void AgentBrainMotor::applyMomentumBias(MotorCommand lastDecoded, float& forwardRate, float& backwardRate) {
    if (lastDecoded == MotorCommand::MoveForward) {
        backwardRate = std::max(0.0f, backwardRate - 8.0f);
    } else if (lastDecoded == MotorCommand::MoveBackward) {
        forwardRate = std::max(0.0f, forwardRate - 8.0f);
    }
}

inline bool AgentBrainMotor::shouldWait(float bestActivity) const {
    return bestActivity <= 0.1f;
}

inline void AgentBrainMotor::updateWinnerTakeAllState(const std::vector<AgentBrainCore::MotorGroupActivity>& activities) {
    float best = 0.0f, second = 0.0f;
    int bestIdx = -1;
    for (int i = 0; i < 5 && i < static_cast<int>(activities.size()); ++i) {
        float r = activities[i].firingRate;
        if (r > best) { second = best; best = r; bestIdx = i; }
        else if (r > second) { second = r; }
    }
    
    if (bestIdx == parent_->lastWinnerGroup_) {
        parent_->consecutiveWinnerCount_++;
    } else {
        parent_->consecutiveWinnerCount_ = 1;
    }
    
    parent_->lastWinnerGroup_ = bestIdx;
    parent_->lastWinnerMargin_ = best - second;
}

} // namespace nlm