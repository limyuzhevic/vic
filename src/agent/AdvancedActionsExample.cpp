// Example usage of AdvancedActions system
// This file demonstrates how to use the enhanced action system

#include "AdvancedActions.hpp"
#include "../motor/Action.hpp"
#include <iostream>
#include <thread>

void example_basic_actions() {
    std::cout << "=== Basic Actions Example ===\n";
    
    nlm::AdvancedActions actions;
    
    // Execute a simple action
    auto action = std::make_unique<nlm::Action>(nlm::ActionType::MoveForward);
    auto result = actions.executeAction(std::move(action));
    
    std::cout << "Action executed: " << (result.isSuccess() ? "Success" : "Failed") 
              << " (Duration: " << result.getDuration() << "s)" << std::endl;
}

void example_action_templates() {
    std::cout << "\n=== Action Templates Example ===\n";
    
    nlm::AdvancedActions actions;
    
    // Create an action from template
    std::vector<float> params = {1, 2.5f, 1.0f}; // Type, Distance, Speed
    auto action = actions.createFromTemplate("MoveWithParams", params);
    
    auto result = actions.executeAction(std::move(action));
    std::cout << "Template-based action executed: " << (result.isSuccess() ? "Success" : "Failed") << std::endl;
}

void example_action_sequences() {
    std::cout << "\n=== Action Sequences Example ===\n";
    
    nlm::AdvancedActions actions;
    
    // Create and execute an action sequence
    auto sequence = actions.createSequence("test_sequence");
    sequence->addAction(std::make_unique<nlm::Action>(nlm::ActionType::MoveForward));
    sequence->addAction(std::make_unique<nlm::Action>(nlm::ActionType::TurnRight), true); // Optional
    sequence->addAction(std::make_unique<nlm::Action>(nlm::ActionType::Look));
    
    auto results = sequence->execute();
    std::cout << "Sequence executed with " << results.size() << " actions" << std::endl;
    for (size_t i = 0; i < results.size(); ++i) {
        std::cout << "  Action " << i << ": " << (results[i].isSuccess() ? "Success" : "Failed") << std::endl;
    }
}

void example_callbacks() {
    std::cout << "\n=== Callbacks Example ===\n";
    
    nlm::AdvancedActions actions;
    
    // Register callbacks
    auto loggingCallback = std::make_unique<nlm::LoggingCallback>("main_logger");
    actions.registerCallback("logger", std::move(loggingCallback));
    
    auto statsCallback = std::make_unique<nlm::StatsCallback>();
    actions.registerCallback("stats", std::move(statsCallback));
    
    // Execute some actions
    for (int i = 0; i < 3; ++i) {
        auto action = std::make_unique<nlm::Action>(nlm::ActionType::MoveForward);
        actions.executeAction(std::move(action));
    }
    
    // Get and display statistics
    auto statsCallbackPtr = std::any_cast<nlm::StatsCallback*>(actions.getStatsCallback("stats"));
    if (statsCallbackPtr) {
        auto stats = statsCallbackPtr->getStats();
        std::cout << "Statistics: Total=" << stats.totalActions 
                  << " Successful=" << stats.successfulActions 
                  << " Failed=" << stats.failedActions << std::endl;
    }
}

void example_validation_and_recovery() {
    std::cout << "\n=== Validation and Recovery Example ===\n";
    
    nlm::AdvancedActions actions;
    
    // Configure validation (example - in real usage you'd configure this properly)
    nlm::ActionValidator validator;
    // Add validation rules...
    
    // Configure recovery
    nlm::ActionRecovery recovery;
    recovery.setGlobalRetryPolicy(3, 0.5f);
    
    // Execute action with recovery
    auto action = std::make_unique<nlm::Action>(nlm::ActionType::MoveForward);
    auto result = actions.executeActionWithRecovery(std::move(action), recovery);
    
    std::cout << "Action with recovery: " << (result.isSuccess() ? "Success" : "Failed") 
              << " (Retries: " << (result.getMessage().empty() ? "0" : "multiple") << ")" << std::endl;
}

void example_batch_processing() {
    std::cout << "\n=== Batch Processing Example ===\n";
    
    nlm::AdvancedActions actions;
    
    // Enable batch processing
    actions.enableBatchProcessing(10);
    
    std::cout << "Batch processing enabled: " << actions.isBatchProcessingEnabled() << std::endl;
    
    // Execute multiple actions
    for (int i = 0; i < 5; ++i) {
        auto action = std::make_unique<nlm::Action>(nlm::ActionType::MoveForward);
        actions.executeAction(std::move(action));
    }
    
    auto stats = actions.getStats();
    std::cout << "Executed " << stats.totalActions << " actions in batch mode" << std::endl;
}

void example_configuration() {
    std::cout << "\n=== Configuration Example ===\n";
    
    nlm::AdvancedActions actions;
    
    // Create a configuration
    nlm::Config config;
    config.setInt("AdvancedActions", "BatchSize", 20);
    
    // Load configuration
    actions.loadConfiguration(config);
    
    // Export configuration
    auto exportedConfig = actions.exportConfiguration();
    std::cout << "Configuration loaded and exported successfully" << std::endl;
}

int main() {
    std::cout << "Advanced Actions System Examples\n";
    std::cout << "===============================\n";
    
    example_basic_actions();
    example_action_templates();
    example_action_sequences();
    example_callbacks();
    example_validation_and_recovery();
    example_batch_processing();
    example_configuration();
    
    std::cout << "\n=== All examples completed ===\n";
    return 0;
}
