// Main integration file for AgentBrain Command Interface
// This file demonstrates how to use the enhanced AgentBrain with command interface

#include "AgentBrain.hpp"
#include "AgentBrainEnhanced.hpp"
#include "AgentBrainCommandInterface.h"
#include <iostream>
#include <string>

// Example 1: Basic Command Interface Usage
void exampleBasicUsage() {
    std::cout << "=== Example 1: Basic Command Interface Usage ===\n";

    // Create enhanced AgentBrain with command interface
    auto brain = nlm::createEnhancedAgentBrain(nlm::createBrain(nlm::createDefaultConfig()));

    // Initialize the brain
    brain->initialize();

    // Create command interface
    auto commandInterface = std::make_shared<nlm::AgentBrainCommandInterface>(brain->getAgentBrain());

    // Use command interface
    std::cout << commandInterface->listAvailableCommands();

    // Execute commands
    CommandResult result = commandInterface->processCommand("initialize");
    std::cout << commandInterface->formatOutput(result);

    result = commandInterface->processCommand("status");
    std::cout << commandInterface->formatOutput(result);

    brain->reset();
}

// Example 2: Command Pattern Usage
void exampleCommandPatterns() {
    std::cout << "=== Example 2: Command Pattern Usage ===\n";

    // Create brain and world
    auto brain = nlm::createEnhancedAgentBrain(nlm::createBrain(nlm::createDefaultConfig()));
    auto world = nlm::createSimpleWorld();

    // Initialize system
    brain->initialize();
    world->reset();

    // Configure world
    world->configure(20, 20, 8, 8);

    // Initialize agent
    auto agent = nlm::createAgentBrain(brain);
    agent->initialize(*world);

    // Enable features
    brain->executeCommand("enable_feature reward_modulation");
    brain->executeCommand("enable_feature curiosity");
    brain->executeCommand("enable_feature development");

    // Configure vision
    brain->executeCommand("set_vision 8 8 60.0");

    // Set dopamine level
    brain->setDopamineLevel(1.0);

    // Set developmental stage
    brain->executeCommand("set_developmental_stage CriticalPeriod");

    // Get status
    std::cout << brain->getSystemStatus();

    // Get quick stats
    std::cout << brain->getQuickStats();

    brain->reset();
    agent->reset();
    world->reset();
}

// Example 3: Interactive Command Loop
void exampleInteractiveLoop() {
    std::cout << "=== Example 3: Interactive Command Loop ===\n";
    std::cout << "Type 'help' for available commands or 'exit' to quit.\n\n";

    // Create enhanced brain
    auto brain = nlm::createEnhancedAgentBrain(nlm::createBrain(nlm::createDefaultConfig()));
    auto commandInterface = std::make_shared<nlm::AgentBrainCommandInterface>(brain->getAgentBrain());

    brain->initialize();

    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        if (input == "exit" || input == "quit") {
            break;
        }

        if (input == "help") {
            std::cout << commandInterface->listAvailableCommands();
            continue;
        }

        CommandResult result = commandInterface->processCommand(input);
        std::cout << commandInterface->formatOutput(result);

        if (input == "status") {
            std::cout << commandInterface->getSystemStatus();
        }
    }

    brain->reset();
}

// Example 4: Batch Command Processing
void exampleBatchProcessing() {
    std::cout << "=== Example 4: Batch Command Processing ===\n";

    auto brain = nlm::createEnhancedAgentBrain(nlm::createBrain(nlm::createDefaultConfig()));
    brain->initialize();

    // Process multiple commands
    std::string batchCommands = "initialize\n";
    batchCommands += "enable_feature reward_modulation\n";
    batchCommands += "enable_feature curiosity\n";
    batchCommands += "set_vision 8 8 60.0\n";
    batchCommands += "configure_world 20 20 default\n";
    batchCommands += "enable_curiosity true\n";
    batchCommands += "set_dopamine 1.5\n";
    batchCommands += "set_developmental_stage CriticalPeriod\n";
    batchCommands += "status\n";
    batchCommands += "stats\n";
    batchCommands += "get_state\n";

    std::cout << brain->executeMultipleCommands(batchCommands);

    brain->reset();
}

// Example 5: Command Help System
void exampleHelpSystem() {
    std::cout << "=== Example 5: Command Help System ===\n";

    auto brain = nlm::createEnhancedAgentBrain(nlm::createBrain(nlm::createDefaultConfig()));
    brain->initialize();

    // Get help for specific commands
    std::cout << brain->getCommandHelp("initialize");
    std::cout << "\n---\n\n";
    std::cout << brain->getCommandHelp("set_vision");
    std::cout << "\n---\n\n";
    std::cout << brain->getCommandHelp("get_config");
    std::cout << "\n---\n\n";
    std::cout << brain->getCommandHelp("enable_feature");

    brain->reset();
}

// Example 6: Backward Compatibility
void exampleBackwardCompatibility() {
    std::cout << "=== Example 6: Backward Compatibility ===\n";

    // Original usage still works
    auto brain = nlm::createAgentBrain(nlm::createBrain(nlm::createDefaultConfig()));
    brain->initialize();

    // Use original methods
    brain->step(100);
    brain->enableCuriosity(true);
    brain->enableRewardModulation(true);

    // Enhanced usage with command interface
    auto enhancedBrain = nlm::createEnhancedAgentBrain(brain);
    enhancedBrain->executeCommand("status");

    enhancedBrain->reset();
}

// Example 7: Error Handling
void exampleErrorHandling() {
    std::cout << "=== Example 7: Error Handling ===\n";

    auto brain = nlm::createEnhancedAgentBrain(nlm::createBrain(nlm::createDefaultConfig()));
    brain->initialize();

    // Process commands that might fail
    std::vector<std::string> commands = {
        "initialize",
        "invalid_command",
        "enable_feature",
        "status",
        "set_vision invalid",
        "enable_feature reward_modulation"
    };

    for (const auto& cmd : commands) {
        std::cout << "Executing: " << cmd << "\n";
        CommandResult result = brain->processCommand(cmd);
        std::cout << brain->formatOutput(result);
        std::cout << "\n---\n\n";
    }

    brain->reset();
}

// Example 8: Complete Simulation with Commands
void exampleCompleteSimulation() {
    std::cout << "=== Example 8: Complete Simulation with Commands ===\n";

    // Create all components
    auto brain = nlm::createEnhancedAgentBrain(nlm::createBrain(nlm::createDefaultConfig()));
    auto world = nlm::createSimpleWorld();
    auto agent = nlm::createAgentBrain(brain);

    // Configure system using commands
    world->configure(20, 20, 8, 8);
    world->reset();
    world->setAgentStart(10.0, 10.0);

    agent->initialize(*world);

    // Enable learning features
    brain->executeCommand("enable_feature reward_modulation");
    brain->executeCommand("enable_feature curiosity");
    brain->executeCommand("enable_feature development");

    // Set neuromodulation
    brain->setDopamineLevel(1.0);

    // Set developmental stage
    brain->executeCommand("set_developmental_stage CriticalPeriod");

    // Run simulation
    std::cout << "=== Starting Simulation ===\n";
    for (int step = 0; step < 100; ++step) {
        // Update world
        world->update(0.1);

        // Get sensory input
        SensoryPercept percept = world->getSensoryPercept();

        // Process sensory input
        agent->processSensoryInput(percept);

        // Run brain step
        brain->step(step);

        // Decode motor command
        MotorCommand action = agent->decodeMotorCommand();

        // Apply motor command
        world->applyMotorCommand(action, world->getSimulationTime());

        // Apply reward modulation
        double reward = 0.0;
        agent->applyRewardModulation(reward, 0.0);

        // Update development
        agent->updateDevelopment(0.1);

        // Print progress every 20 steps
        if (step % 20 == 0) {
            std::cout << "Step " << step << ": ";
            std::cout << brain->getQuickStats();
        }
    }

    std::cout << "=== Simulation Complete ===\n";

    // Get final system status
    std::cout << brain->getSystemStatus();

    // Save final state
    brain->executeCommand("save simulation_state.bin");

    // Cleanup
    brain->reset();
    agent->reset();
    world->reset();
}

int main() {
    std::cout << "=== AgentBrain Command Interface Examples ===\n\n";

    // Run examples
    exampleBasicUsage();
    std::cout << "\n" << std::string(80, '-') << "\n\n";

    exampleCommandPatterns();
    std::cout << "\n" << std::string(80, '-') << "\n\n";

    exampleHelpSystem();
    std::cout << "\n" << std::string(80, '-') << "\n\n";

    exampleErrorHandling();
    std::cout << "\n" << std::string(80, '-') << "\n\n";

    exampleCompleteSimulation();

    std::cout << "\n=== All Examples Completed ===\n";

    return 0;
}
