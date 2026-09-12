// Demo: NLM Action System and ConfigUtils
// Demonstrates the new advanced features added to NLM

#include "Action.hpp"
#include "ActionUtils.hpp"
#include "core/Config/ConfigUtils.hpp"
#include <iostream>
#include <memory>

void demoActionSystem() {
    std::cout << "=== NLM Action System Demo ===" << std::endl << std::endl;
    
    // Create different types of actions
    auto forwardAction = Action::create(ActionType::MoveForward);
    auto backwardAction = Action::create(ActionType::MoveBackward);
    auto leftTurnAction = Action::create(ActionType::TurnLeft);
    auto interactAction = Action::create(ActionType::Interact);
    
    // Create action with parameters (e.g., looking with parameters)
    auto lookAction = Action::create(ActionType::Look, std::vector<float>{0.5f, 0.3f});
    
    // Get action names
    std::cout << "Action names:" << std::endl;
    std::cout << "  MoveForward: " << forwardAction->getName() << std::endl;
    std::cout << "  MoveBackward: " << backwardAction->getName() << std::endl;
    std::cout << "  TurnLeft: " << leftTurnAction->getName() << std::endl;
    std::cout << "  Interact: " << interactAction->getName() << std::endl;
    std::cout << "  Look: " << lookAction->getName() << std::endl;
    std::cout << std::endl;
    
    // Clone actions
    auto forwardClone = forwardAction->clone();
    std::cout << "Cloned MoveForward action type: " << forwardClone->getName() << std::endl;
    std::cout << std::endl;
    
    // Check action types
    std::cout << "Movement actions:" << std::endl;
    for (auto type : Action::getAllActionTypes()) {
        if (Action::isMovementAction(type)) {
            std::cout << "  - " << Action::toString(type) << std::endl;
        }
    }
    std::cout << std::endl;
    
    std::cout << "Rotation actions:" << std::endl;
    for (auto type : Action::getAllActionTypes()) {
        if (Action::isRotationAction(type)) {
            std::cout << "  - " << Action::toString(type) << std::endl;
        }
    }
    std::cout << std::endl;
    
    std::cout << "Interaction actions:" << std::endl;
    for (auto type : Action::getAllActionTypes()) {
        if (Action::isInteractionAction(type)) {
            std::cout << "  - " << Action::toString(type) << std::endl;
        }
    }
    std::cout << std::endl;
    
    // Create constant actions (const version)
    auto constAction = Action::createConst(ActionType::Wait);
    std::cout << "Created constant action (type: " << constAction->getType() << ")" << std::endl;
    std::cout << std::endl;
}

void demoConfigUtils() {
    std::cout << "=== NLM ConfigUtils Demo ===" << std::endl << std::endl;
    
    // Create a default configuration
    auto config = ConfigUtils::createDefaultConfig();
    std::cout << "Default configuration created:" << std::endl;
    std::cout << config->summary() << std::endl;
    
    // Apply different profiles
    std::cout << "Applying 'fast' profile:" << std::endl;
    ConfigUtils::applyProfile(*config, "fast");
    std::cout << "  neuron_count: " << config->getOr<int64_t>("neuron_count", 0) << std::endl;
    std::cout << "  region_count: " << config->getOr<int64_t>("region_count", 0) << std::endl;
    std::cout << "  synaptogenesis_rate: " << config->getOr<double>("synaptogenesis_rate", 0.0) << std::endl;
    std::cout << std::endl;
    
    // Create test configuration
    auto testConfig = ConfigUtils::createTestConfig();
    std::cout << "Test configuration (deterministic):" << std::endl;
    std::cout << "  random_seed: " << testConfig->getOr<int64_t>("random_seed", 0) << std::endl;
    std::cout << "  stdp_ltp_weight: " << testConfig->getOr<double>("stdp_ltp_weight", 0.0) << std::endl;
    std::cout << std::endl;
    
    // Demonstrate merging configurations
    std::cout << "Merging configurations..." << std::endl;
    ConfigUtils::applyProfile(*config, "accurate");
    ConfigUtils::merge(*testConfig, *config);
    std::cout << "After merging, test_config has " << testConfig->getOr<int64_t>("neuron_count", 0) << " neurons" << std::endl;
    std::cout << std::endl;
    
    // Demonstrate available profiles
    std::cout << "Available profiles:" << std::endl;
    for (const auto& profile : ConfigUtils::getAvailableProfiles()) {
        std::cout << "  - " << profile << std::endl;
    }
    std::cout << std::endl;
}

void demoAdvancedUsage() {
    std::cout << "=== Advanced Usage Demo ===" << std::endl << std::endl;
    
    // Create action history (as MotorSystem would)
    std::vector<std::unique_ptr<const Action>> actionHistory;
    
    // Add different actions to history
    actionHistory.push_back(Action::createConst(ActionType::MoveForward));
    actionHistory.push_back(Action::createConst(ActionType::TurnLeft, std::vector<float>{0.5f}));
    actionHistory.push_back(Action::createConst(ActionType::Interact));
    
    std::cout << "Action history (" << actionHistory.size() << " actions):" << std::endl;
    for (size_t i = 0; i < actionHistory.size(); ++i) {
        std::cout << "  " << (i + 1) << ". " << actionHistory[i]->getName();
        if (!actionHistory[i]->getParameters().empty()) {
            std::cout << " (params: [";
            for (size_t j = 0; j < actionHistory[i]->getParameters().size(); ++j) {
                if (j > 0) std::cout << ", ";
                std::cout << actionHistory[i]->getParameters()[j];
            }
            std::cout << "]";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    
    // Convert to string values (as ConfigUtils does)
    std::cout << "Configuration utility: getAllStringValues() demo:" << std::endl;
    auto stringValues = ConfigUtils::getAllStringValues(*ConfigUtils::createDefaultConfig());
    for (const auto& pair : stringValues) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
    std::cout << std::endl;
}

int main() {
    std::cout << "NLM Advanced Features Demo" << std::endl;
    std::cout << "=============================" << std::endl << std::endl;
    
    demoActionSystem();
    demoConfigUtils();
    demoAdvancedUsage();
    
    std::cout << "=== All Demos Completed Successfully ===" << std::endl;
    std::cout << std::endl;
    std::cout << "The NLM now provides enhanced:" << std::endl;
    std::cout << "  - Advanced Action system with creation, cloning, and utilities" << std::endl;
    std::cout << "  - ConfigUtils for configuration management and validation" << std::endl;
    std::cout << "  - Better memory management with unique_ptr throughout" << std::endl;
    std::cout << "  - Fixed Logger API inconsistencies" << std::endl;
    std::cout << "  - Improved code organization and documentation" << std::endl;
    
    return 0;
}
