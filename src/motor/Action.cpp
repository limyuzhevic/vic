#include "Action.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

Action::Action() : type_(ActionType::Wait), executionCount_(0), successfulExecutions_(0), averageExecutionTime_(0.0f), totalEnergyConsumed_(0.0f) {}

Action::Action(ActionType type) : type_(type), executionCount_(0), successfulExecutions_(0), averageExecutionTime_(0.0f), totalEnergyConsumed_(0.0f) {}

Action::Action(ActionType type, const std::vector<float>& params) 
    : type_(type), parameters_(params), executionCount_(0), successfulExecutions_(0), averageExecutionTime_(0.0f), totalEnergyConsumed_(0.0f) {}

Action::Action(ActionType type, const MotorDynamics& dynamics) 
    : type_(type), parameters_(std::vector<float>()), dynamics_(dynamics), 
      executionCount_(0), successfulExecutions_(0), averageExecutionTime_(0.0f), totalEnergyConsumed_(0.0f) {}

Action::~Action() = default;

ActionType Action::getType() const {
    return type_;
}

void Action::setType(ActionType type) {
    type_ = type;
}

MotorCommand Action::getMotorCommand() const {
    return dynamics_.motorCommand;
}

void Action::setMotorCommand(MotorCommand cmd) {
    dynamics_.motorCommand = cmd;
}

const std::vector<float>& Action::getParameters() const {
    return parameters_;
}

void Action::setParameters(const std::vector<float>& params) {
    parameters_ = params;
}

const MotorDynamics& Action::getDynamics() const {
    return dynamics_;
}

MotorDynamics& Action::getDynamics() {
    return dynamics_;
}

void Action::setDynamics(const MotorDynamics& dynamics) {
    dynamics_ = dynamics;
}

void Action::updateNeuralDrive(float neuralActivation, float arousalLevel, float neuromodulator) {
    dynamics_.neuralDrive = neuralActivation;
    dynamics_.arousal = arousalLevel;
    dynamics_.neuromodulation = neuromodulator;
    
    // Biological realism: neural drive affects execution probability
    float modulation = 1.0f + neuromodulator * 0.5f + arousalLevel * 0.2f;
    dynamics_.executionTime *= modulation;
    dynamics_.energyCost *= modulation;
    dynamics_.accuracy *= modulation;
}

void Action::updateFatigue(float fatigueRate, double dt) {
    // Fatigue accumulates over time with use
    float fatigueIncrease = fatigueRate * dt;
    dynamics_.fatigueFactor = std::min(1.0f, dynamics_.fatigueFactor + fatigueIncrease);
    
    // Fatigue affects motor performance
    dynamics_.accuracy *= (1.0f - dynamics_.fatigueFactor * 0.3f);
    dynamics_.stability *= (1.0f - dynamics_.fatigueFactor * 0.2f);
    dynamics_.energyCost *= (1.0f + dynamics_.fatigueFactor * 0.5f);
}

void Action::updateAdaptation(float adaptationDelta) {
    dynamics_.adaptationState += adaptationDelta * dynamics_.learningRate;
    dynamics_.adaptationState = std::clamp(dynamics_.adaptationState, -1.0f, 1.0f);
    
    // Adaptation affects motor output
    dynamics_.force *= (1.0f + dynamics_.adaptationState * 0.2f);
    dynamics_.accuracy *= (1.0f + dynamics_.adaptationState * 0.3f);
}

void Action::updateExpectedValue(float reward, float uncertainty) {
    dynamics_.expectedValue = reward;
    dynamics_.risk = uncertainty;
    
    // Risk-adjusted expected value (lower certainty reduces value)
    dynamics_.expectedValue *= (1.0f - dynamics_.risk * 0.5f);
}

void Action::updateSuccessRate(bool success) {
    if (executionCount_ > 0) {
        successfulExecutions_ += success ? 1 : 0;
        dynamics_.successRate = static_cast<float>(successfulExecutions_) / executionCount_;
        
        // Update learning rate based on experience
        if (dynamics_.successRate < 0.5f) {
            dynamics_.learningRate *= 1.1f; // Learn faster when failing
        }
    }
}

void Action::applyExploration(float explorationStrength) {
    dynamics_.explorationBonus = explorationStrength;
    
    // Exploration can improve future performance but may reduce current efficiency
    dynamics_.executionTime *= (1.0f + explorationStrength * 0.2f);
    dynamics_.energyCost *= (1.0f + explorationStrength * 0.1f);
}

void Action::setSequenceContext(size_t position, size_t length) {
    dynamics_.sequencePosition = position;
    dynamics_.sequenceLength = length;
    
    // Update sequence context effects
    if (position == 0) {
        dynamics_.isFirstInSequence = true;
        dynamics_.isLastInSequence = (length == 1);
    } else if (position == length - 1) {
        dynamics_.isFirstInSequence = false;
        dynamics_.isLastInSequence = true;
    } else {
        dynamics_.isFirstInSequence = false;
        dynamics_.isLastInSequence = false;
    }
}

void Action::setSequenceFirst(bool isFirst) {
    dynamics_.isFirstInSequence = isFirst;
}

void Action::setSequenceLast(bool isLast) {
    dynamics_.isLastInSequence = isLast;
}

float Action::computeCost() const {
    float baseCost = dynamics_.energyCost;
    
    // Additional costs based on context
    float sequenceCost = 0.0f;
    if (dynamics_.isFirstInSequence) {
        sequenceCost += 0.2f; // Planning cost
    }
    if (dynamics_.isLastInSequence) {
        sequenceCost += 0.1f; // Completion cost
    }
    
    // Fatigue cost
    float fatigueCost = dynamics_.fatigueFactor * 0.3f;
    
    // Opportunity cost (consider doing alternatives)
    float opportunityCost = dynamics_.opportunityCost;
    
    return (baseCost + sequenceCost + fatigueCost + opportunityCost) * (1.0f + dynamics_.risk * 0.2f);
}

float Action::computeBenefit() const {
    float baseBenefit = dynamics_.expectedValue;
    
    // Success rate multiplier
    float successBenefit = dynamics_.successRate * 0.5f;
    
    // Exploration bonus
    float explorationBenefit = dynamics_.explorationBonus;
    
    // Neuromodulatory enhancement
    float neuromodBenefit = dynamics_.neuromodulation > 0.0f ? 0.3f : 0.0f;
    
    return baseBenefit + successBenefit + explorationBenefit + neuromodBenefit;
}

float Action::computeNetValue() const {
    float benefit = computeBenefit();
    float cost = computeCost();
    return benefit - cost;
}

float Action::computeExecutionTime() const {
    float time = dynamics_.executionTime;
    
    // Accuracy affects effective time
    if (dynamics_.accuracy < 0.5f) {
        time *= 2.0f; // Poor accuracy doubles time
    }
    
    // Stability affects reliability
    if (dynamics_.stability < 0.5f) {
        time *= 1.5f; // Poor stability increases time
    }
    
    // Adaptation affects performance
    if (std::abs(dynamics_.adaptationState) > 0.5f) {
        time *= (1.0f + std::abs(dynamics_.adaptationState) * 0.3f);
    }
    
    return time;
}

bool Action::isExecutable() const {
    return (dynamics_.neuralDrive > 0.1f) && (dynamics_.accuracy > 0.2f) && (dynamics_.stability > 0.2f);
}

float Action::getExecutionProbability() const {
    if (!isExecutable()) {
        return 0.0f;
    }
    
    // Neural drive is primary determinant of execution probability
    float prob = dynamics_.neuralDrive * dynamics_.accuracy * dynamics_.stability;
    
    // Modulate with neuromodulation
    if (dynamics_.neuromodulation > 0.0f) {
        prob *= (1.0f + dynamics_.neuromodulation * 0.5f);
    } else if (dynamics_.neuromodulation < 0.0f) {
        prob *= (1.0f + dynamics_.neuromodulation * 0.3f);
    }
    
    return std::clamp(prob, 0.0f, 1.0f);
}

float Action::getMotorOutputLevel() const {
    // Combine neural drive with motor capabilities
    float motorLevel = dynamics_.neuralDrive;
    
    // Motor command intensity
    float commandIntensity = 0.0f;
    switch (dynamics_.motorCommand) {
        case MotorCommand::MoveForward:
        case MotorCommand::MoveBackward:
            commandIntensity = 0.8f;
            break;
        case MotorCommand::TurnLeft:
        case MotorCommand::TurnRight:
            commandIntensity = 0.7f;
            break;
        case MotorCommand::LookLeft:
        case MotorCommand::LookRight:
            commandIntensity = 0.6f;
            break;
        case MotorCommand::Interact:
            commandIntensity = 0.9f;
            break;
        case MotorCommand::Wait:
            commandIntensity = 0.0f;
            break;
    }
    
    // Combine with motor command
    motorLevel = motorLevel * 0.7f + commandIntensity * 0.3f;
    
    // Adjust for fatigue and adaptation
    motorLevel *= (1.0f - dynamics_.fatigueFactor * 0.2f);
    motorLevel *= (1.0f + dynamics_.adaptationState * 0.1f);
    
    return std::clamp(motorLevel, 0.0f, 1.0f);
}

void Action::learnFromOutcome(float reward, float predictionError) {
    // Update success rate
    executionCount_++;
    if (reward > 0.0f) {
        successfulExecutions_++;
        dynamics_.successRate = static_cast<float>(successfulExecutions_) / executionCount_;
    }
    
    // Update expected value with prediction error
    dynamics_.expectedValue += dynamics_.learningRate * predictionError;
    
    // Update adaptation based on prediction error
    dynamics_.adaptationState += dynamics_.learningRate * predictionError;
    dynamics_.adaptationState = std::clamp(dynamics_.adaptationState, -1.0f, 1.0f);
    
    // Update energy cost based on outcome
    if (reward > 0.0f) {
        totalEnergyConsumed_ += dynamics_.energyCost;
        averageExecutionTime_ = (averageExecutionTime_ * (executionCount_ - 1) + computeExecutionTime()) / executionCount_;
    }
    
    // Decay learning rate over time
    dynamics_.learningRate *= 0.99f;
}

void Action::updateMotorPattern(const std::vector<float>& pattern) {
    parameters_ = pattern;
    
    // Update motor dynamics based on pattern
    if (!pattern.empty()) {
        // Extract key dynamics from pattern
        size_t i = 0;
        if (i < pattern.size()) dynamics_.force = pattern[i]; i++;
        if (i < pattern.size()) dynamics_.torque = pattern[i]; i++;
        if (i < pattern.size()) dynamics_.velocity = pattern[i]; i++;
        if (i < pattern.size()) dynamics_.accuracy = pattern[i]; i++;
        if (i < pattern.size()) dynamics_.stability = pattern[i]; i++;
        if (i < pattern.size()) dynamics_.energyCost = pattern[i]; i++;
    }
}

std::string Action::getName() const {
    return std::string(actionTypeToString(type_));
}

std::unique_ptr<Action> Action::clone() const {
    auto cloned = std::make_unique<Action>(type_, parameters_);
    cloned->dynamics_ = dynamics_;
    cloned->executionCount_ = executionCount_;
    cloned->successfulExecutions_ = successfulExecutions_;
    cloned->averageExecutionTime_ = averageExecutionTime_;
    cloned->totalEnergyConsumed_ = totalEnergyConsumed_;
    return cloned;
}

const char* Action::actionTypeToString(ActionType type) {
    switch (type) {
        case ActionType::MoveForward: return "MoveForward";
        case ActionType::MoveBackward: return "MoveBackward";
        case ActionType::MoveLeft: return "MoveLeft";
        case ActionType::MoveRight: return "MoveRight";
        case ActionType::TurnLeft: return "TurnLeft";
        case ActionType::TurnRight: return "TurnRight";
        case ActionType::Look: return "Look";
        case ActionType::LookUp: return "LookUp";
        case ActionType::LookDown: return "LookDown";
        case ActionType::Interact: return "Interact";
        case ActionType::Eat: return "Eat";
        case ActionType::Drink: return "Drink";
        case ActionType::Rest: return "Rest";
        case ActionType::Wait: return "Wait";
        case ActionType::Custom: return "Custom";
        default: return "Unknown";
    }
}

const char* Action::motorCommandToString(MotorCommand cmd) {
    switch (cmd) {
        case MotorCommand::MoveForward: return "MoveForward";
        case MotorCommand::MoveBackward: return "MoveBackward";
        case MotorCommand::TurnLeft: return "TurnLeft";
        case MotorCommand::TurnRight: return "TurnRight";
        case MotorCommand::LookLeft: return "LookLeft";
        case MotorCommand::LookRight: return "LookRight";
        case MotorCommand::Interact: return "Interact";
        case MotorCommand::Wait: return "Wait";
        default: return "Unknown";
    }
}

} // namespace nlm
