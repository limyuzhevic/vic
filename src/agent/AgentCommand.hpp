#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include "AgentBrain.hpp"

namespace nlm {

// Command types for agent control
class AgentCommand {
public:
    virtual ~AgentCommand() = default;
    virtual bool execute(AgentBrain& agent) = 0;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual bool isAdvanced() const { return false; }
};

// Basic agent control commands
class AgentResetCommand : public AgentCommand {
public:
    bool execute(AgentBrain& agent) override {
        agent.reset();
        return true;
    }
    std::string getName() const override { return "reset"; }
    std::string getDescription() const override { return "Reset agent state to initial values"; }
    std::string getUsage() const override { return "reset"; }
};

class AgentInitializeCommand : public AgentCommand {
private:
    std::shared_ptr<Brain> brain_;
public:
    AgentInitializeCommand(std::shared_ptr<Brain> brain) : brain_(brain) {}
    bool execute(AgentBrain& agent) override {
        if (brain_) {
            // Note: This is a simplified initialization
            // In practice, the world would be passed from context
            agent.reset();
            return true;
        }
        return false;
    }
    std::string getName() const override { return "initialize"; }
    std::string getDescription() const override { return "Initialize agent with current brain state"; }
    std::string getUsage() const override { return "initialize"; }
};

class AgentStepCommand : public AgentCommand {
private:
    double timestep_;
public:
    AgentStepCommand(double timestep = 0.001) : timestep_(timestep) {}
    bool execute(AgentBrain& agent) override {
        agent.updateDevelopment(timestep_);
        return true;
    }
    std::string getName() const override { return "step"; }
    std::string getDescription() const override { return "Advance agent development by timestep"; }
    std::string getUsage() const override { return "step [timestep]";
    }
};

class AgentEnableModulationCommand : public AgentCommand {
private:
    std::string modulationType_;
    bool enable_;
public:
    AgentEnableModulationCommand(const std::string& type, bool enable) 
        : modulationType_(type), enable_(enable) {}
    bool execute(AgentBrain& agent) override {
        if (modulationType_ == "reward") {
            agent.enableRewardModulation(enable_);
        } else if (modulationType_ == "curiosity") {
            agent.enableCuriosity(enable_);
        } else if (modulationType_ == "structural_plasticity") {
            agent.enableStructuralPlasticity(enable_);
        } else if (modulationType_ == "development") {
            agent.enableDevelopment(enable_);
        }
        return true;
    }
    std::string getName() const override { return "enable_modulation"; }
    std::string getDescription() const override { return "Enable or disable neuromodulation systems"; }
    std::string getUsage() const override { return "enable_modulation [reward|curiosity|structural_plasticity|development] [true|false]"; }
};

class AgentSetCuriosityLevelCommand : public AgentCommand {
private:
    float level_;
public:
    AgentSetCuriosityLevelCommand(float level) : level_(level) {}
    bool execute(AgentBrain& agent) override {
        // Note: This is a simplified setter
        // In practice, curiosity is updated through processSensoryInput
        return true;
    }
    std::string getName() const override { return "set_curiosity_level"; }
    std::string getDescription() const override { return "Set curiosity level (0.0-1.0)"; }
    std::string getUsage() const override { return "set_curiosity_level [level 0.0-1.0]"; }
};

class AgentSetNoveltyLevelCommand : public AgentCommand {
private:
    float level_;
public:
    AgentSetNoveltyLevelCommand(float level) : level_(level) {}
    bool execute(AgentBrain& agent) override {
        // Note: This is a simplified setter
        // In practice, novelty is updated through processSensoryInput
        return true;
    }
    std::string getName() const override { return "set_novelty_level"; }
    std::string getDescription() const override { return "Set novelty level (0.0-1.0)"; }
    std::string getUsage() const override { return "set_novelty_level [level 0.0-1.0]"; }
};

class AgentSetPredictionErrorCommand : public AgentCommand {
private:
    float error_;
public:
    AgentSetPredictionErrorCommand(float error) : error_(error) {}
    bool execute(AgentBrain& agent) override {
        // Note: This is a simplified setter
        // In practice, prediction error is updated through applyRewardModulation
        return true;
    }
    std::string getName() const override { return "set_prediction_error"; }
    std::string getDescription() const override { return "Set prediction error (-1.0 to 1.0)"; }
    std::string getUsage() const override { return "set_prediction_error [error -1.0 to 1.0]"; }
};

class AgentGetStateCommand : public AgentCommand {
public:
    bool execute(AgentBrain& agent) override {
        std::cout << "=== Agent State ===" << std::endl;
        std::cout << "Neuromodulation Level: " << agent.getNeuromodulationLevel() << std::endl;
        std::cout << "Curiosity Level: " << agent.getCuriosityLevel() << std::endl;
        std::cout << "Novelty Level: " << agent.getNoveltyLevel() << std::endl;
        std::cout << "Prediction Error: " << agent.getPredictionError() << std::endl;
        std::cout << "Developmental Stage: " << static_cast<int>(agent.getDevelopmentalStage()) << std::endl;
        std::cout << "============================================" << std::endl;
        return true;
    }
    std::string getName() const override { return "get_state"; }
    std::string getDescription() const override { return "Get current agent state information"; }
    std::string getUsage() const override { return "get_state"; }
};

class AgentHelpCommand : public AgentCommand {
private:
    std::unordered_map<std::string, std::shared_ptr<AgentCommand>> commands_;
public:
    AgentHelpCommand(const std::unordered_map<std::string, std::shared_ptr<AgentCommand>>& commands) 
        : commands_(commands) {}
    bool execute(AgentBrain& agent) override {
        std::cout << "=== Available Agent Commands ===" << std::endl;
        std::cout << "Basic Commands:" << std::endl;
        for (const auto& pair : commands_) {
            const auto& cmd = pair.second;
            if (!cmd->isAdvanced()) {
                std::cout << "  " << cmd->getName() << " - " << cmd->getDescription() << std::endl;
                std::cout << "    Usage: " << cmd->getUsage() << std::endl;
            }
        }
        std::cout << "Advanced Commands:" << std::endl;
        for (const auto& pair : commands_) {
            const auto& cmd = pair.second;
            if (cmd->isAdvanced()) {
                std::cout << "  " << cmd->getName() << " - " << cmd->getDescription() << std::endl;
                std::cout << "    Usage: " << cmd->getUsage() << std::endl;
            }
        }
        std::cout << "===============================" << std::endl;
        return true;
    }
    std::string getName() const override { return "help"; }
    std::string getDescription() const override { return "Show help for all available commands"; }
    std::string getUsage() const override { return "help"; }
};

class AgentQuitCommand : public AgentCommand {
public:
    bool execute(AgentBrain& agent) override {
        std::cout << "Shutting down agent..." << std::endl;
        return true;
    }
    std::string getName() const override { return "quit"; }
    std::string getDescription() const override { return "Exit the agent system"; }
    std::string getUsage() const override { return "quit"; }
};

class AgentLearnCommand : public AgentCommand {
public:
    bool isAdvanced() const override { return true; }
    bool execute(AgentBrain& agent) override {
        std::cout << "Applying learning increment to agent..." << std::endl;
        // Note: This is a simplified learning command
        // In practice, this would involve more complex learning operations
        return true;
    }
    std::string getName() const override { return "learn"; }
    std::string getDescription() const override { return "Apply learning increment to agent based on current state"; }
    std::string getUsage() const override { return "learn"; }
};

class AgentExecuteExperimentCommand : public AgentCommand {
private:
    std::string experimentType_;
public:
    AgentExecuteExperimentCommand(const std::string& type) : experimentType_(type) {}
    bool isAdvanced() const override { return true; }
    bool execute(AgentBrain& agent) override {
        std::cout << "Executing experiment: " << experimentType_ << std::endl;
        // Note: This is a simplified experiment command
        // In practice, experiments would be implemented differently
        return true;
    }
    std::string getName() const override { return "execute_experiment"; }
    std::string getDescription() const override { return "Execute a predefined experiment"; }
    std::string getUsage() const override { return "execute_experiment <experiment_type>"; }
};

// Agent Command Handler - manages agent commands and execution
class AgentCommandHandler {
private:
    std::unordered_map<std::string, std::shared_ptr<AgentCommand>> commands_;
public:
    AgentCommandHandler() {
        // Register basic commands
        commands_["reset"] = std::make_shared<AgentResetCommand>();
        commands_["initialize"] = std::make_shared<AgentInitializeCommand>(std::shared_ptr<Brain>());
        commands_["step"] = std::make_shared<AgentStepCommand>();
        commands_["help"] = std::make_shared<AgentHelpCommand>(commands_);
        commands_["quit"] = std::make_shared<AgentQuitCommand>();
        commands_["get_state"] = std::make_shared<AgentGetStateCommand>();
        
        // Register advanced commands
        commands_["learn"] = std::make_shared<AgentLearnCommand>();
    }
    
    bool executeCommand(AgentBrain& agent, const std::string& commandLine) {
        std::vector<std::string> tokens = parseCommandLine(commandLine);
        if (tokens.empty()) {
            return false;
        }
        
        std::string cmdName = tokens[0];
        auto it = commands_.find(cmdName);
        if (it == commands_.end()) {
            std::cout << "Unknown command: " << cmdName << ". Type 'help' for available commands." << std::endl;
            return false;
        }
        
        auto& cmd = it->second;
        
        // Simplified argument parsing for demonstration
        // In a full implementation, this would be more sophisticated
        try {
            if (cmdName == "step" && tokens.size() > 1) {
                double timestep = std::stod(tokens[1]);
                commands_["step"] = std::make_shared<AgentStepCommand>(timestep);
                return commands_["step"]->execute(agent);
            } else if (cmdName == "enable_modulation" && tokens.size() > 2) {
                std::string type = tokens[1];
                bool enable = (tokens[2] == "true" || tokens[2] == "1");
                commands_["enable_modulation"] = std::make_shared<AgentEnableModulationCommand>(type, enable);
                return commands_["enable_modulation"]->execute(agent);
            } else if (cmdName == "set_curiosity_level" && tokens.size() > 1) {
                float level = std::stof(tokens[1]);
                commands_["set_curiosity_level"] = std::make_shared<AgentSetCuriosityLevelCommand>(level);
                return commands_["set_curiosity_level"]->execute(agent);
            } else if (cmdName == "set_novelty_level" && tokens.size() > 1) {
                float level = std::stof(tokens[1]);
                commands_["set_novelty_level"] = std::make_shared<AgentSetNoveltyLevelCommand>(level);
                return commands_["set_novelty_level"]->execute(agent);
            } else if (cmdName == "set_prediction_error" && tokens.size() > 1) {
                float error = std::stof(tokens[1]);
                commands_["set_prediction_error"] = std::make_shared<AgentSetPredictionErrorCommand>(error);
                return commands_["set_prediction_error"]->execute(agent);
            } else if (cmdName == "initialize_experiment" && tokens.size() > 1) {
                commands_["execute_experiment"] = std::make_shared<AgentExecuteExperimentCommand>(tokens[1]);
                return commands_["execute_experiment"]->execute(agent);
            }
            
            // Execute command with default parameters if available
            return cmd->execute(agent);
        } catch (const std::exception& e) {
            std::cout << "Error executing command: " << e.what() << std::endl;
            return false;
        }
    }
    
    std::vector<std::string> parseCommandLine(const std::string& commandLine) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream iss(commandLine);
        while (iss >> token) {
            tokens.push_back(token);
        }
        return tokens;
    }
    
    void registerCommand(const std::string& name, std::shared_ptr<AgentCommand> command) {
        commands_[name] = command;
    }
};