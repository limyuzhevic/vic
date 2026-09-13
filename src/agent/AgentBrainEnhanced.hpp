// Enhanced AgentBrain with Command Interface Integration
// This file integrates the CommandExecutor system into AgentBrain

#include "AgentBrain.hpp"
#include "CommandExecutor.hpp"
#include <iostream>
#include <string>
#include <sstream>

namespace nlm {

// Enhanced AgentBrain with Command Interface
class AgentBrainEnhanced : public AgentBrain {
public:
    AgentBrainEnhanced(std::shared_ptr<Brain> brain)
        : AgentBrain(brain), commandExecutor_(std::make_shared<CommandExecutor>(shared_from_this())) {
    }
    
    // Command processing interface
    CommandResult processCommand(const std::string& input) {
        // Parse the command
        ParsedCommand parsed = commandExecutor_->parseCommand(input);
        
        // Execute the command
        CommandResult result = commandExecutor_->executeCommand(parsed);
        
        // Format and return the result
        std::string formattedOutput = commandExecutor_->formatOutput(result);
        result.output = formattedOutput;
        
        return result;
    }
    
    // Convenient command processing with string output
    std::string executeCommand(const std::string& input) {
        CommandResult result = processCommand(input);
        return result.output;
    }
    
    // List all available commands
    std::string listAvailableCommands() {
        return commandExecutor_->listAvailableCommands();
    }
    
    // Get help for specific command
    std::string getCommandHelp(const std::string& command) {
        return commandExecutor_->getCommandHelp(command);
    }
    
    // Get current system status
    std::string getSystemStatus() {
        CommandResult result = processCommand("status");
        return result.output;
    }
    
    // Get quick statistics
    std::string getQuickStats() {
        CommandResult result = processCommand("stats");
        return result.output;
    }
    
    // Get brain state summary
    std::string getBrainState() {
        CommandResult result = processCommand("get_state");
        return result.output;
    }
    
    // Enable a feature
    bool enableFeature(const std::string& feature) {
        CommandResult result = processCommand("enable_feature " + feature);
        return result.success;
    }
    
    // Disable a feature
    bool disableFeature(const std::string& feature) {
        CommandResult result = processCommand("disable_feature " + feature);
        return result.success;
    }
    
    // Set dopamine level
    bool setDopamineLevel(double level) {
        std::stringstream ss;
        ss << "set_dopamine " << level;
        CommandResult result = processCommand(ss.str());
        return result.success;
    }
    
    // Get dopamine level
    double getDopamineLevel() {
        CommandResult result = processCommand("get_dopamine");
        // Parse the output to extract dopamine level
        // For now, return 0.0 as sample implementation
        return 0.0;
    }
    
    // Set developmental stage
    bool setDevelopmentalStage(const std::string& stage) {
        CommandResult result = processCommand("set_developmental_stage " + stage);
        return result.success;
    }
    
    // Initialize brain with command processing
    void initialize() override {
        AgentBrain::initialize();
        
        // Additional initialization for command interface
        commandInitialized_ = true;
        
        // Log initialization
        std::cout << "AgentBrain Enhanced initialized with command interface" << std::endl;
    }
    
    // Reset with command processing
    void reset() override {
        AgentBrain::reset();
        
        // Reset command interface state
        commandInitialized_ = false;
        
        // Log reset
        std::cout << "AgentBrain Enhanced reset" << std::endl;
    }
    
    // Get command interface reference
    std::shared_ptr<CommandExecutor> getCommandExecutor() const {
        return commandExecutor_;
    }
    
private:
    std::shared_ptr<CommandExecutor> commandExecutor_;
    bool commandInitialized_ = false;
};

// Factory function for creating enhanced AgentBrain
inline std::shared_ptr<AgentBrainEnhanced> createEnhancedAgentBrain(std::shared_ptr<Brain> brain) {
    return std::make_shared<AgentBrainEnhanced>(brain);
}

// Convenience functions for common command patterns
namespace CommandPatterns {
    
    // Run complete simulation with automatic command processing
    void runSimulation(std::shared_ptr<AgentBrainEnhanced> agentBrain, 
                      std::shared_ptr<SimpleWorld> world,
                      int steps) {
        std::cout << "=== Running Simulation ===" << std::endl;
        
        for (int i = 0; i < steps; ++i) {
            // Update world
            world->update(0.1);
            
            // Process world state
            std::cout << "Step " << i << ": ";
            
            // Get status at milestone steps
            if (i % 100 == 0) {
                std::cout << agentBrain->getQuickStats();
            } else {
                std::cout << ".";
            }
            
            // Small delay for readability
            if (i % 10 == 0) {
                std::cout << std::endl;
            }
        }
        
        std::cout << std::endl << "=== Simulation Complete ===" << std::endl;
    }
    
    // Configure and initialize brain with settings
    void configureAndInitialize(std::shared_ptr<AgentBrainEnhanced> agentBrain,
                                std::shared_ptr<SimpleWorld> world,
                                int worldWidth, int worldHeight,
                                int visionWidth, int visionHeight) {
        std::cout << "=== Configuring System ===" << std::endl;
        
        // Configure world
        world->configure(worldWidth, worldHeight, visionWidth, visionHeight);
        
        // Initialize agent brain
        agentBrain->initialize();
        
        // Initialize world
        world->reset();
        
        // Enable key features
        agentBrain->enableFeature("reward_modulation");
        agentBrain->enableFeature("structural_plasticity");
        agentBrain->enableFeature("development");
        agentBrain->enableFeature("curiosity");
        
        // Set dopamine level
        agentBrain->setDopamineLevel(1.0);
        
        // Set developmental stage
        agentBrain->setDevelopmentalStage("CriticalPeriod");
        
        std::cout << "=== System Configured ===" << std::endl;
    }
    
    // Get comprehensive system report
    std::string getSystemReport(std::shared_ptr<AgentBrainEnhanced> agentBrain) {
        std::string report = "=== System Report ===\n\n";
        
        // Get brain state
        report += agentBrain->getBrainState() + "\n";
        
        // Get status
        report += agentBrain->getSystemStatus() + "\n";
        
        // Get stats
        report += agentBrain->getQuickStats() + "\n";
        
        // Get configuration
        report += agentBrain->executeCommand("list_config") + "\n";
        
        report += "=== End System Report ===";
        return report;
    }
}

} // namespace nlm