// NLM Command-Line Interface Implementation
// Command-line tool for NLM runtime control

#include "CommandLineInterface.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>
#include <algorithm>
#include <filesystem>

namespace nlm {

CommandLineInterface::CommandLineInterface() : brain_(nullptr), agentBrain_(nullptr) {
}

CommandLineInterface::~CommandLineInterface() {
}

void CommandLineInterface::initialize(std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agentBrain) {
    brain_ = brain;
    agentBrain_ = agentBrain;
    
    // Register all commands
    registerCommand(std::make_unique<HelpCommand>(this));
    registerCommand(std::make_unique<StatusCommand>(this));
    registerCommand(std::make_unique<StepCommand>(this));
    registerCommand(std::make_unique<ResetCommand>(this));
    registerCommand(std::make_unique<SaveCommand>(this));
    registerCommand(std::make_unique<LoadCommand>(this));
    registerCommand(std::make_unique<ConfigCommand>(this));
    registerCommand(std::make_unique<MemoryCommand>(this));
    registerCommand(std::make_unique<StatsCommand>(this));
    registerCommand(std::make_unique<ExperimentCommand>(this));
}

std::string CommandLineInterface::executeCommand(const std::string& commandLine) {
    if (commandLine.empty()) {
        return "Empty command";
    }
    
    std::vector<std::string> tokens = tokenize(commandLine);
    if (tokens.empty()) {
        return "Empty command";
    }
    
    std::string commandName = tokens[0];
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    
    auto it = commands_.find(commandName);
    if (it == commands_.end()) {
        return "Unknown command: " + commandName + ". Type 'help' for available commands.";
    }
    
    try {
        return it->second->execute(args);
    } catch (const std::exception& e) {
        return "Error executing command: " + std::string(e.what());
    } catch (...) {
        return "Unknown error executing command";
    }
}

void CommandLineInterface::registerCommand(std::unique_ptr<Command> cmd) {
    if (cmd) {
        commands_[cmd->getName()] = std::move(cmd);
    }
}

std::string CommandLineInterface::showHelp() const {
    std::stringstream ss;
    ss << "=== NLM COMMAND-LINE INTERFACE ===" << std::endl;
    ss << "Available commands:" << std::endl;
    ss << std::endl;
    
    for (const auto& pair : commands_) {
        ss << "  " << pair.second->getName() << " - " << pair.second->getHelp() << std::endl;
    }
    
    ss << std::endl;
    ss << "Type any command to execute it." << std::endl;
    ss << "Use 'help' for detailed command help." << std::endl;
    
    return ss.str();
}

std::vector<std::string> CommandLineInterface::getCommandNames() const {
    std::vector<std::string> names;
    for (const auto& pair : commands_) {
        names.push_back(pair.first);
    }
    return names;
}

bool CommandLineInterface::hasCommand(const std::string& name) const {
    return commands_.find(name) != commands_.end();
}

// Command implementations
std::string CommandLineInterface::startCommand(const std::vector<std::string>& args) {
    // Placeholder - TODO: Implement start command
    return "Start command not implemented yet";
}

std::string CommandLineInterface::stopCommand(const std::vector<std::string>& args) {
    // Placeholder - TODO: Implement stop command
    return "Stop command not implemented yet";
}

std::string CommandLineInterface::resetCommand(const std::vector<std::string>& args) {
    // Delegate to ResetCommand
    auto it = commands_.find("reset");
    if (it != commands_.end()) {
        return it->second->execute(args);
    }
    return "Reset command not found";
}

std::string CommandLineInterface::stepCommand(const std::vector<std::string>& args) {
    // Delegate to StepCommand
    auto it = commands_.find("step");
    if (it != commands_.end()) {
        return it->second->execute(args);
    }
    return "Step command not found";
}

std::string CommandLineInterface::configViewCommand(const std::vector<std::string>& args) {
    // Delegate to ConfigCommand
    auto it = commands_.find("config");
    if (it != commands_.end()) {
        std::vector<std::string> configArgs;
        configArgs.push_back("view");
        configArgs.insert(configArgs.end(), args.begin(), args.end());
        return it->second->execute(configArgs);
    }
    return "Config command not found";
}

std::string CommandLineInterface::configModifyCommand(const std::vector<std::string>& args) {
    // Delegate to ConfigCommand
    auto it = commands_.find("config");
    if (it != commands_.end()) {
        std::vector<std::string> configArgs;
        configArgs.push_back("modify");
        configArgs.insert(configArgs.end(), args.begin(), args.end());
        return it->second->execute(configArgs);
    }
    return "Config command not found";
}

std::string CommandLineInterface::configSaveCommand(const std::vector<std::string>& args) {
    // Delegate to ConfigCommand
    auto it = commands_.find("config");
    if (it != commands_.end()) {
        std::vector<std::string> configArgs;
        configArgs.push_back("save");
        configArgs.insert(configArgs.end(), args.begin(), args.end());
        return it->second->execute(configArgs);
    }
    return "Config command not found";
}

std::string CommandLineInterface::memoryWorkingCommand(const std::vector<std::string>& args) {
    // Delegate to MemoryCommand
    auto it = commands_.find("memory");
    if (it != commands_.end()) {
        std::vector<std::string> memoryArgs;
        memoryArgs.push_back("working");
        memoryArgs.insert(memoryArgs.end(), args.begin(), args.end());
        return it->second->execute(memoryArgs);
    }
    return "Memory command not found";
}

std::string CommandLineInterface::memoryEpisodicCommand(const std::vector<std::string>& args) {
    // Delegate to MemoryCommand
    auto it = commands_.find("memory");
    if (it != commands_.end()) {
        std::vector<std::string> memoryArgs;
        memoryArgs.push_back("episodic");
        memoryArgs.insert(memoryArgs.end(), args.begin(), args.end());
        return it->second->execute(memoryArgs);
    }
    return "Memory command not found";
}

std::string CommandLineInterface::saveStateCommand(const std::vector<std::string>& args) {
    // Delegate to SaveCommand
    auto it = commands_.find("save");
    if (it != commands_.end()) {
        return it->second->execute(args);
    }
    return "Save command not found";
}

std::string CommandLineInterface::loadStateCommand(const std::vector<std::string>& args) {
    // Delegate to LoadCommand
    auto it = commands_.find("load");
    if (it != commands_.end()) {
        return it->second->execute(args);
    }
    return "Load command not found";
}

std::string CommandLineInterface::statsCommand(const std::vector<std::string>& args) {
    // Delegate to StatsCommand
    auto it = commands_.find("stats");
    if (it != commands_.end()) {
        return it->second->execute(args);
    }
    return "Stats command not found";
}

std::string CommandLineInterface::experimentRunCommand(const std::vector<std::string>& args) {
    // Delegate to ExperimentCommand
    auto it = commands_.find("experiment");
    if (it != commands_.end()) {
        std::vector<std::string> expArgs;
        expArgs.push_back("run");
        expArgs.insert(expArgs.end(), args.begin(), args.end());
        return it->second->execute(expArgs);
    }
    return "Experiment command not found";
}

std::string CommandLineInterface::experimentStatusCommand(const std::vector<std::string>& args) {
    // Delegate to ExperimentCommand
    auto it = commands_.find("experiment");
    if (it != commands_.end()) {
        std::vector<std::string> expArgs;
        expArgs.push_back("status");
        expArgs.insert(expArgs.end(), args.begin(), args.end());
        return it->second->execute(expArgs);
    }
    return "Experiment command not found";
}

std::string CommandLineInterface::batchCommand(const std::vector<std::string>& args) {
    // Placeholder - TODO: Implement batch command
    return "Batch command not implemented yet";
}

std::string CommandLineInterface::scriptCommand(const std::vector<std::string>& args) {
    // Placeholder - TODO: Implement script command
    return "Script command not implemented yet";
}

std::string CommandLineInterface::eventCommand(const std::vector<std::string>& args) {
    // Placeholder - TODO: Implement event command
    return "Event command not implemented yet";
}

std::vector<std::string> CommandLineInterface::tokenize(const std::string& commandLine) const {
    std::vector<std::string> tokens;
    std::stringstream ss(commandLine);
    std::string token;
    
    while (ss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

std::string CommandLineInterface::trim(const std::string& str) const {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) {
        return "";
    }
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

std::string CommandLineInterface::getErrorMessage(const std::string& command, const std::string& error) const {
    return "Error in command '" + command + "': " + error;
}

} // namespace nlm