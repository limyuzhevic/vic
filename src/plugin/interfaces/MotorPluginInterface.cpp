// MotorPluginInterface.cpp
#include "interfaces/MotorPluginInterface.hpp"
#include <stdexcept>

namespace nlm {

// Default implementations for MotorPluginInterface

std::string MotorPluginInterface::getName() const {
    return "Default Motor Plugin";
}

std::string MotorPluginInterface::getVersion() const {
    return "1.0.0";
}

std::string MotorPluginInterface::getDescription() const {
    return "Default motor control interface";
}

std::string MotorPluginInterface::getAuthor() const {
    return "NLM Team";
}

bool MotorPluginInterface::initialize(const Config& config) {
    setConfiguration(config);
    return validateConfiguration();
}

void MotorPluginInterface::shutdown() {
    // Default implementation does nothing
}

bool MotorPluginInterface::isInitialized() const {
    return true;
}

const std::type_info& MotorPluginInterface::getTypeInfo() const {
    return typeid(MotorPluginInterface);
}

void MotorPluginInterface::setConfiguration(const Config& config) {
    // Default implementation - override in concrete implementations
}

Config MotorPluginInterface::getConfiguration() const {
    Config config;
    // Return default configuration
    return config;
}

bool MotorPluginInterface::isEnabled() const {
    return true;
}

void MotorPluginInterface::setEnabled(bool enabled) {
    // Default implementation - can be overridden
}

bool MotorPluginInterface::validateConfiguration() const {
    return true;
}

std::string MotorPluginInterface::getValidationError() const {
    return "";
}

std::string MotorPluginInterface::getId() const {
    return "motor_plugin_default_v1.0";
}

std::chrono::steady_clock::time_point MotorPluginInterface::getLoadTime() const {
    return std::chrono::steady_clock::now();
}

std::unique_ptr<MotorCommand> MotorPluginInterface::createMotorCommand(const Config& config) {
    return createMotorCommand(ActionType::MoveForward, config);
}

std::unique_ptr<MotorCommand> MotorPluginInterface::createMotorCommand(ActionType action, const Config& config) {
    // Default implementation - would create appropriate motor command
    return std::make_unique<MotorCommand>(action);
}

bool MotorPluginInterface::configureMotorCommand(MotorCommand* command, const Config& config) {
    return true;
}

bool MotorPluginInterface::validateMotorCommandConfiguration(const Config& config) const {
    return true;
}

std::vector<std::string> MotorPluginInterface::getSupportedActions() const {
    return {"MoveForward", "MoveBackward", "TurnLeft", "TurnRight", "Interact", "Wait"};
}

float MotorPluginInterface::getDefaultActionSelectionThreshold() const {
    return 0.5f;
}

float MotorPluginInterface::getDefaultNoiseLevel() const {
    return 0.1f;
}

float MotorPluginInterface::getDefaultExplorationRate() const {
    return 0.1f;
}

bool MotorPluginInterface::supportsCustomActionSelection() const {
    return false;
}

std::string MotorPluginInterface::getActionSelectionModelName() const {
    return "Standard Action Selection";
}

bool MotorPluginInterface::supportsTemporalAbstraction() const {
    return false;
}

float MotorPluginInterface::getTemporalAbstractionWindow() const {
    return 1.0f;
}

std::string MotorPluginInterface::getDefaultConfigurationKey() const {
    return "motor_plugin_config";
}

Config MotorPluginInterface::getDefaultConfiguration() const {
    Config config;
    config.set("action_selection_threshold", 0.5f);
    config.set("exploration_rate", 0.1f);
    config.set("noise_level", 0.1f);
    return config;
}

ActionType MotorPluginInterface::selectAction(const std::vector<float>& motorActivity, const Config& config) const {
    // Simple majority voting action selection
    if (motorActivity.empty()) {
        return ActionType::Wait;
    }
    
    float sum = 0.0f;
    for (float activity : motorActivity) {
        sum += activity;
    }
    
    float avgActivity = sum / motorActivity.size();
    
    if (avgActivity > config.getOr<float>("action_selection_threshold", 0.5f)) {
        return ActionType::MoveForward;
    }
    return ActionType::Wait;
}

float MotorPluginInterface::calculateActionSelectionConfidence(const std::vector<float>& motorActivity, const Config& config) const {
    if (motorActivity.empty()) {
        return 0.0f;
    }
    
    float variance = 0.0f;
    float mean = 0.0f;
    for (float activity : motorActivity) {
        mean += activity;
    }
    mean /= motorActivity.size();
    
    for (float activity : motorActivity) {
        variance += (activity - mean) * (activity - mean);
    }
    variance /= motorActivity.size();
    
    return 1.0f - variance; // Confidence based on activity variance
}

float MotorPluginInterface::calculateExplorationRate(float timeStep, const Config& config) const {
    // Simple exploration rate calculation
    return config.getOr<float>("exploration_rate", 0.1f) * (1.0f - timeStep);
}

} // namespace nlm