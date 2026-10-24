#include "AgentCommand.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace nlm {

// Default constructor for AgentCommandHandler
AgentCommandHandler::AgentCommandHandler() {
    // Register basic commands
    commands_["reset"] = std::make_shared<AgentResetCommand>();
    commands_["initialize"] = std::make_shared<AgentInitializeCommand>(std::shared_ptr<Brain>());
    commands_["step"] = std::make_shared<AgentStepCommand>();
    commands_["help"] = std::make_shared<AgentHelpCommand>(commands_);
    commands_["quit"] = std::make_shared<AgentQuitCommand>();
    commands_["get_state"] = std::make_shared<AgentGetStateCommand>();
    
    // Register advanced commands
    commands_["learn"] = std::make_shared<AgentLearnCommand>();
    commands_["execute_experiment"] = std::make_shared<AgentExecuteExperimentCommand>("default");
}

// Implementation of parseCommandLine
std::vector<std::string> AgentCommandHandler::parseCommandLine(const std::string& commandLine) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream iss(commandLine);
    while (iss >> token) {
        tokens.push_back(token);
    }
    return tokens;
}

// Implementation of executeCommand
bool AgentCommandHandler::executeCommand(AgentBrain& agent, const std::string& commandLine) {
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

// Implementation of registerCommand
void AgentCommandHandler::registerCommand(const std::string& name, std::shared_ptr<AgentCommand> command) {
    commands_[name] = command;
}

} // namespace nlm