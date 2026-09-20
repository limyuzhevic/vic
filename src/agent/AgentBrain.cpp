#include "AgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

// Implementation of configuration validation methods
bool AgentBrain::validateConfiguration() const {
    bool isValid = true;
    
    // Check for null brain pointer
    if (!brain_) {
        NLM_LOG_ERROR("AgentBrain has null brain pointer - system cannot function");
        isValid = false;
    }
    
    // Validate motor neuron group sizes
    size_t totalMotorNeurons = motorForward_.size() + motorBackward_.size() +
                              motorTurnLeft_.size() + motorTurnRight_.size() +
                              motorLookLeft_.size() + motorLookRight_.size() +
                              motorInteract_.size() + motorWait_.size();
    
    if (totalMotorNeurons == 0) {
        NLM_LOG_WARNING("AgentBrain has no motor neurons configured");
        isValid = false;
    }
    
    // Check for duplicate neurons (basic check)
    // In a more complete implementation, we'd track neuron IDs
    
    // Validate sensory neuron group sizes
    size_t totalSensoryNeurons = sensoryVision_.size() + sensoryTouch_.size() +
                                sensoryInternal_.size() + sensoryProprioception_.size();
    
    if (totalSensoryNeurons == 0) {
        NLM_LOG_WARNING("AgentBrain has no sensory neurons configured");
        isValid = false;
    }
    
    // Check neuromodulation parameter ranges
    if (dopamineLevel_ < -1.0f || dopamineLevel_ > 1.0f) {
        NLM_LOG_WARNING("Dopamine level out of expected range: " + std::to_string(dopamineLevel_));
    }
    
    if (noveltyLevel_ < 0.0f || noveltyLevel_ > 1.0f) {
        NLM_LOG_WARNING("Novelty level out of expected range: " + std::to_string(noveltyLevel_));
        isValid = false;
    }
    
    if (curiosityLevel_ < 0.0f || curiosityLevel_ > 1.0f) {
        NLM_LOG_WARNING("Curiosity level out of expected range: " + std::to_string(curiosityLevel_));
        isValid = false;
    }
    
    return isValid;
}

std::string AgentBrain::getConfigurationSummary() const {
    std::stringstream ss;
    ss << "AgentBrain Configuration Summary:" << std::endl;
    ss << "  Motor neurons: " << (motorForward_.size() + motorBackward_.size() +
                                 motorTurnLeft_.size() + motorTurnRight_.size() +
                                 motorLookLeft_.size() + motorLookRight_.size() +
                                 motorInteract_.size() + motorWait_.size()) << std::endl;
    ss << "  Sensory neurons: " << (sensoryVision_.size() + sensoryTouch_.size() +
                                 sensoryInternal_.size() + sensoryProprioception_.size()) << std::endl;
    ss << "  Neuromodulation enabled: Reward=" << (rewardModulationEnabled_ ? "Yes" : "No")
       << ", Structural=" << (structuralPlasticityEnabled_ ? "Yes" : "No")
       << ", Development=" << (developmentEnabled_ ? "Yes" : "No")
       << ", Curiosity=" << (curiosityEnabled_ ? "Yes" : "No") << std::endl;
    ss << "  Developmental stage: " << static_cast<int>(getDevelopmentalStage()) << std::endl;
    ss << "  Dopamine level: " << dopamineLevel_ << std::endl;
    ss << "  Novelty level: " << noveltyLevel_ << std::endl;
    ss << "  Curiosity level: " << curiosityLevel_ << std::endl;
    ss << "  Prediction error: " << predictionError_ << std::endl;
    ss << "  Expected reward: " << expectedReward_ << std::endl;
    
    return ss.str();
}

// Implementation of motor neuron access methods
const std::vector<Neuron*>& AgentBrain::getMotorNeurons(MotorCommand cmd) const {
    switch (cmd) {
        case MotorCommand::MoveForward: return motorForward_;
        case MotorCommand::MoveBackward: return motorBackward_;
        case MotorCommand::TurnLeft: return motorTurnLeft_;
        case MotorCommand::TurnRight: return motorTurnRight_;
        case MotorCommand::LookLeft: return motorLookLeft_;
        case MotorCommand::LookRight: return motorLookRight_;
        case MotorCommand::Interact: return motorInteract_;
        case MotorCommand::Wait: return motorWait_;
        default: static std::vector<Neuron*> empty; return empty;
    }
}

// Configuration setter with validation
bool AgentBrain::setSensoryNoveltyDecay(float decay) {
    if (decay >= 0.0f && decay <= 1.0f) {
        sensoryNoveltyDecay_ = decay;
        return true;
    }
    NLM_LOG_ERROR("Invalid sensory novelty decay value: " + std::to_string(decay));
    return false;
}

bool AgentBrain::setCuriosityThreshold(float threshold) {
    if (threshold >= 0.0f && threshold <= 1.0f) {
        curiosityThreshold_ = threshold;
        return true;
    }
    NLM_LOG_ERROR("Invalid curiosity threshold value: " + std::to_string(threshold));
    return false;
}

// Additional helper methods
void AgentBrain::logMotorActivity() const {
    NLM_LOG_INFO("Motor neuron activity summary:");
    NLM_LOG_INFO("  Forward: " + std::to_string(calcMotorActivity(motorForward_)));
    NLM_LOG_INFO("  Backward: " + std::to_string(calcMotorActivity(motorBackward_)));
    NLM_LOG_INFO("  Turn Left: " + std::to_string(calcMotorActivity(motorTurnLeft_)));
    NLM_LOG_INFO("  Turn Right: " + std::to_string(calcMotorActivity(motorTurnRight_)));
    NLM_LOG_INFO("  Look Left: " + std::to_string(calcMotorActivity(motorLookLeft_)));
    NLM_LOG_INFO("  Look Right: " + std::to_string(calcMotorActivity(motorLookRight_)));
    NLM_LOG_INFO("  Interact: " + std::to_string(calcMotorActivity(motorInteract_)));
    NLM_LOG_INFO("  Wait: " + std::to_string(calcMotorActivity(motorWait_)));
}

float AgentBrain::calcMotorActivity(const std::vector<Neuron*>& neurons) const {
    if (neurons.empty()) return 0.0f;
    float sum = 0.0f;
    for (const Neuron* n : neurons) {
        if (n) {
            sum += std::abs(n->getState().membranePotential - n->getState().restingPotential);
        }
    }
    return sum / neurons.size();
}

// Reset with additional cleanup
void AgentBrain::reset() {
    AgentBrain::reset();  // Call parent reset if it exists
    curiosityThreshold_ = 0.3f;  // Default curiosity threshold
    NLM_LOG_INFO("AgentBrain completely reset for new episode");
}

// Get most active neurons for debugging
std::vector<MotorCommand> AgentBrain::getMostActiveCommands(size_t count) const {
    std::vector<std::pair<MotorCommand, float>> activityLevels;
    
    struct { MotorCommand cmd; float activity; } commands[] = {
        {MotorCommand::MoveForward, calcMotorActivity(motorForward_)},
        {MotorCommand::MoveBackward, calcMotorActivity(motorBackward_)},
        {MotorCommand::TurnLeft, calcMotorActivity(motorTurnLeft_)},
        {MotorCommand::TurnRight, calcMotorActivity(motorTurnRight_)},
        {MotorCommand::LookLeft, calcMotorActivity(motorLookLeft_)},
        {MotorCommand::LookRight, calcMotorActivity(motorLookRight_)},
        {MotorCommand::Interact, calcMotorActivity(motorInteract_)},
        {MotorCommand::Wait, calcMotorActivity(motorWait_)}
    };
    
    for (const auto& c : commands) {
        activityLevels.push_back(std::make_pair(c.cmd, c.activity));
    }
    
    // Sort by activity level (descending)
    std::sort(activityLevels.begin(), activityLevels.end(),
              [](const std::pair<MotorCommand, float>& a,
                 const std::pair<MotorCommand, float>& b) {
                  return a.second > b.second;
              });
    
    std::vector<MotorCommand> mostActive;
    for (size_t i = 0; i < std::min(count, activityLevels.size()); ++i) {
        mostActive.push_back(activityLevels[i].first);
    }
    
    return mostActive;
}

} // namespace nlm
