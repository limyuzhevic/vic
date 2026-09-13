#include "AgentCommands.hpp"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>

namespace nlm {

// Agent Commands Implementation

void AgentCommandManager::registerCommand(std::unique_ptr<AgentCommand> command) {
    commands_.push_back(std::move(command));
}

CommandResult AgentCommandManager::executeCommand(const std::string& commandLine, 
                                                  std::shared_ptr<Brain> brain,
                                                  std::shared_ptr<SimpleWorld> world) {
    std::vector<std::string> tokens = tokenize(commandLine);
    if (tokens.empty()) {
        return CommandResult(false, "Empty command");
    }
    
    std::string commandName = tokens[0];
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    
    // Set references if provided
    if (brain && brain != brain_) {
        setBrain(brain);
    }
    if (world && world != world_) {
        setWorld(world);
    }
    
    // Find and execute command
    for (auto& cmd : commands_) {
        if (cmd->getName() == commandName) {
            // Check if world is needed
            if (cmd->requiresWorld() && !world_) {
                return CommandResult(false, "Command requires world context");
            }
            
            return cmd->execute(args);
        }
    }
    
    return CommandResult(false, "Unknown command: " + commandName + "\n" +
                       "Available commands: " + getAvailableCommands());
}

std::string AgentCommandManager::getHelp() const {
    std::string help = "=== NLM AGENT COMMANDS ===\n\n";
    help += "These commands provide advanced control over the NLM brain and agent.\n\n";
    
    for (const auto& cmd : commands_) {
        help += "Command: " + cmd->getName() + "\n";
        help += "  Description: " + cmd->getDescription() + "\n";
        
        if (!cmd->getUsage().empty()) {
            help += "  Usage:\n";
            for (const auto& usage : cmd->getUsage()) {
                help += "    " + usage + "\n";
            }
        }
        help += "\n";
    }
    
    help += "=== SYSTEM COMMANDS ===\n";
    help += "  help\n";
    help += "    Show this help message\n";
    help += "  quit\n";
    help += "    Exit the agent\n";
    help += "  reset\n";
    help += "    Reset brain and world to initial state\n";
    help += "  status\n";
    help += "    Show complete system status\n";
    
    return help;
}

std::vector<std::string> AgentCommandManager::tokenize(const std::string& commandLine) {
    std::vector<std::string> tokens;
    std::string token;
    bool inQuotes = false;
    char quoteChar = '\0';
    
    for (char c : commandLine) {
        if (!inQuotes && (c == ' ' || c == '\t')) {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        } else if (c == '"' || c == '\'') {
            if (!inQuotes) {
                inQuotes = true;
                quoteChar = c;
            } else if (quoteChar == c) {
                inQuotes = false;
                quoteChar = '\0';
            } else {
                token += c;
            }
        } else {
            token += c;
        }
    }
    
    if (!token.empty()) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string AgentCommandManager::getAvailableCommands() const {
    std::string commands;
    for (size_t i = 0; i < commands_.size(); ++i) {
        commands += commands_[i]->getName();
        if (i < commands_.size() - 1) {
            commands += ", ";
        }
    }
    return commands;
}

// System Commands Implementation

class HelpCommand : public AgentCommand {
public:
    CommandResult execute(const std::vector<std::string>& args) override {
        if (agentManager_) {
            return CommandResult(true, agentManager_->getHelp());
        }
        return CommandResult(true, "Help system not initialized");
    }
    
    std::string getName() const override { return "help"; }
    std::string getDescription() const override { 
        return "Show this help message with all available commands"; 
    }
    
    std::vector<std::string> getUsage() const override {
        return {"help", "Show command help and usage information"};
    }
    
    bool requiresBrain() const override { return false; }
    bool requiresWorld() const override { return false; }
    
    void setAgentManager(AgentCommandManager* manager) { agentManager_ = manager; }
    
private:
    AgentCommandManager* agentManager_ = nullptr;
};

class QuitCommand : public AgentCommand {
public:
    CommandResult execute(const std::vector<std::string>& args) override {
        return CommandResult(true, "Exiting agent...");
    }
    
    std::string getName() const override { return "quit"; }
    std::string getDescription() const override { 
        return "Exit the agent and save checkpoint if requested"; 
    }
    
    std::vector<std::string> getUsage() const override {
        return {"quit", "Exit the agent"};
    }
    
    bool requiresBrain() const override { return false; }
    bool requiresWorld() const override { return false; }
};

class ResetCommand : public AgentCommand {
public:
    CommandResult execute(const std::vector<std::string>& args) override {
        if (!brain_) {
            return CommandResult(false, "Cannot reset: Brain not initialized");
        }
        
        if (!world_) {
            return CommandResult(false, "Cannot reset: World not initialized");
        }
        
        // Reset brain
        brain_->reset();
        
        // Reset world
        world_->reset();
        
        return CommandResult(true, "System reset complete. Brain and world returned to initial state.");
    }
    
    std::string getName() const override { return "reset"; }
    std::string getDescription() const override { 
        return "Reset brain and world to initial state"; 
    }
    
    std::vector<std::string> getUsage() const override {
        return {"reset", "Reset brain and world to initial state"};
    }
    
    bool requiresBrain() const override { return true; }
    bool requiresWorld() const override { return true; }
    
    void setBrain(std::shared_ptr<Brain> brain) { brain_ = brain; }
    void setWorld(std::shared_ptr<SimpleWorld> world) { world_ = world; }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
};

class StatusCommand : public AgentCommand {
public:
    CommandResult execute(const std::vector<std::string>& args) override {
        std::string output = "=== COMPLETE SYSTEM STATUS ===\n\n";
        
        // Brain status
        output += "=== BRAIN STATUS ===\n";
        if (brain_) {
            output += "Status: Active\n";
            output += "Neurons: " + std::to_string(brain_->getTotalNeuronCount()) + "\n";
            output += "Synapses: " + std::to_string(brain_->getTotalSynapseCount()) + "\n";
            output += "Firing Rate: " + 
                std::to_string(brain_->getFiringNeuronCount() / 
                    std::max<size_t>(brain_->getTotalNeuronCount(), 1) * 100.0f) + "%\n";
            output += "Development Stage: " + std::to_string(static_cast<int>(brain_->getDevelopmentalStage())) + "\n";
        } else {
            output += "Status: Not Initialized\n";
        }
        
        // Memory systems status
        output += "\n=== MEMORY SYSTEMS STATUS ===\n";
        if (brain_) {
            if (brain_->getWorkingMemory()) {
                output += "Working Memory: Active (capacity: " + 
                    std::to_string(brain_->getWorkingMemory()->getCapacity()) + ")\n";
            }
            if (brain_->getEpisodicMemory()) {
                output += "Episodic Memory: Active (episodes: " + 
                    std::to_string(brain_->getEpisodicMemory()->getEpisodeCount()) + ")\n";
            }
            if (brain_->getAssociativeMemory()) {
                output += "Associative Memory: Active\n";
            }
            if (brain_->getConceptFormation()) {
                output += "Concept Formation: Active (concepts: " + 
                    std::to_string(brain_->getConceptFormation()->getConceptCount()) + ")\n";
            }
            if (brain_->getPredictionSystem()) {
                output += "Prediction System: Active (error: " + 
                    std::to_string(brain_->getPredictionSystem()->getPredictionError()) + ")\n";
            }
        }
        
        // Neuromodulation status
        output += "\n=== NEUROMODULATION STATUS ===\n";
        if (brain_) {
            if (brain_->getDopamine()) {
                output += "Dopamine: Level " + std::to_string(brain_->getDopamine()->getLevel()) + "\n";
            }
            if (brain_->getCuriosity()) {
                output += "Curiosity: Level " + std::to_string(brain_->getCuriosity()->getLevel()) + "\n";
            }
            if (brain_->getNovelty()) {
                output += "Novelty: Level " + std::to_string(brain_->getNovelty()->getLevel()) + "\n";
            }
        }
        
        // World status
        output += "\n=== WORLD STATUS ===\n";
        if (world_) {
            output += "Status: Active\n";
            output += "Dimensions: " + std::to_string(world_->getWidth()) + " x " + 
                std::to_string(world_->getHeight()) + "\n";
            output += "Vision: " + std::to_string(world_->getVisionWidth()) + 
                " x " + std::to_string(world_->getVisionHeight()) + "\n";
            
            // Get agent body
            auto body = world_->getAgentBody();
            output += "Agent Position: (" + std::to_string(body.x) + ", " + 
                std::to_string(body.y) + ")\n";
            output += "Energy: " + std::to_string(body.energy) + "\n";
            output += "Health: " + std::to_string(body.health) + "\n";
        } else {
            output += "Status: Not Initialized\n";
        }
        
        output += "\n=== AGENT STATUS ===\n";
        output += "Status: " + (agent_ ? "Active" : "Not Initialized") + "\n";
        
        if (agent_) {
            output += "Reward Modulation: " + std::string(agent_->isRewardModulationEnabled() ? "Enabled" : "Disabled") + "\n";
            output += "Structural Plasticity: " + std::string(agent_->isStructuralPlasticityEnabled() ? "Enabled" : "Disabled") + "\n";
            output += "Development: " + std::string(agent_->isDevelopmentEnabled() ? "Enabled" : "Disabled") + "\n";
            output += "Curiosity: " + std::string(agent_->isCuriosityEnabled() ? "Enabled" : "Disabled") + "\n";
        }
        
        output += "\n=== SYSTEM HEALTH ===\n";
        output += "All systems operational: " + std::string(areAllSystemsHealthy() ? "YES" : "NO") + "\n";
        
        return CommandResult(true, output);
    }
    
    std::string getName() const override { return "status"; }
    std::string getDescription() const override { 
        return "Show complete system status and health report"; 
    }
    
    std::vector<std::string> getUsage() const override {
        return {"status", "Show complete system status and health"};
    }
    
    bool requiresBrain() const override { return false; }
    bool requiresWorld() const override { return false; }
    
    void setBrain(std::shared_ptr<Brain> brain) { brain_ = brain; }
    void setWorld(std::shared_ptr<SimpleWorld> world) { world_ = world; }
    void setAgent(std::shared_ptr<AgentBrain> agent) { agent_ = agent; }
    
    bool areAllSystemsHealthy() const {
        if (!brain_) return false;
        if (!world_) return false;
        
        // Check critical systems
        if (brain_->getWorkingMemory() == nullptr) return false;
        if (brain_->getEpisodicMemory() == nullptr) return false;
        if (brain_->getDopamine() == nullptr) return false;
        if (brain_->getCuriosity() == nullptr) return false;
        
        return true;
    }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
};

// Helper function to create and register all commands
void initializeAgentCommands(AgentCommandManager& commandManager, 
                             std::shared_ptr<Brain> brain,
                             std::shared_ptr<SimpleWorld> world,
                             std::shared_ptr<AgentBrain> agent) {
    
    // Set references for commands that need them
    commandManager.setBrain(brain);
    commandManager.setWorld(world);
    
    // Register system commands (these don't require brain/world)
    auto helpCmd = std::make_unique<HelpCommand>();
    helpCmd->setAgentManager(&commandManager);
    commandManager.registerCommand(std::move(helpCmd));
    
    commandManager.registerCommand(std::make_unique<QuitCommand>());
    
    auto resetCmd = std::make_unique<ResetCommand>();
    resetCmd->setBrain(brain);
    resetCmd->setWorld(world);
    commandManager.registerCommand(std::move(resetCmd));
    
    auto statusCmd = std::make_unique<StatusCommand>();
    statusCmd->setBrain(brain);
    statusCmd->setWorld(world);
    statusCmd->setAgent(agent);
    commandManager.registerCommand(std::move(statusCmd));
    
    // Note: The actual brain commands (BrainStatusCommand, CheckpointCommand, etc.)
    // need to be registered separately when the system is fully initialized
    // This prevents registration before all references are available
}

} // namespace nlm
