// CommandExecutionFramework implementation
#include "CommandExecutionFramework.hpp"
#include <iostream>
#include <algorithm>

namespace nlm {

CommandExecutionFramework::CommandExecutionFramework(std::shared_ptr<Logger> logger)
    : logger(logger) {}

bool CommandExecutionFramework::executeCommand(const std::string& command, const std::vector<std::string>& args) {
    // Check if command is registered
    if (commandDescriptions.find(command) == commandDescriptions.end()) {
        handleCommandError(command, "Unknown command: " + command);
        return false;
    }
    
    // Log command execution
    std::string logMessage = "Executing command: " + command;
    if (!args.empty()) {
        logMessage += " with args: ";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) logMessage += ", ";
            logMessage += args[i];
        }
    }
    
    logger->log(logMessage, LogLevel::Info);
    addCommandToHistory(command);
    
    // TODO: Actual command execution logic
    std::cout << "Command execution for " << command << " (placeholder)" << std::endl;
    
    return true;
}

void CommandExecutionFramework::printHelp(const std::string& command) {
    if (command.empty()) {
        std::cout << "=== NLM Advanced CLI Help ===" << std::endl;
        std::cout << "Available command groups:" << std::endl;
        
        // Group 1: Brain Management Commands
        std::cout << "  Brain Management:" << std::endl;
        std::cout << "    --brain-config <file>          Load brain configuration" << std::endl;
        std::cout << "    --brain-save <file>            Save brain state to file" << std::endl;
        std::cout << "    --brain-load <file>            Load brain state from file" << std::endl;
        std::cout << "    --brain-info                   Display detailed brain information" << std::endl;
        std::cout << "    --brain-reset                  Reset brain to initial state" << std::endl;
        std::cout << "    --brain-dump <file>            Dump brain state in binary format" << std::endl;
        std::cout << "    --brain-diff <file1> <file2>   Compare two brain states" << std::endl;
        std::cout << "    --brain-status                 Display current brain status" << std::endl;
        std::cout << "    --brain-validate               Validate brain state consistency" << std::endl;
        
        // Group 2: Simulation Control Commands
        std::cout << "  Simulation Control:" << std::endl;
        std::cout << "    --simulate <steps>             Run simulation for specified steps" << std::endl;
        std::cout << "    --simulate-continue <steps>     Continue from current state" << std::endl;
        std::cout << "    --simulate-pause                Pause simulation" << std::endl;
        std::cout << "    --simulate-resume               Resume simulation" << std::endl;
        std::cout << "    --simulate-step                 Execute single simulation step" << std::endl;
        std::cout << "    --simulate-rewind <steps>       Rewind simulation" << std::endl;
        std::cout << "    --simulate-fastforward <factor> Run simulation at accelerated speed" << std::endl;
        std::cout << "    --simulate-loop <steps>         Run simulation in loop" << std::endl;
        std::cout << "    --simulate-profile              Profile simulation performance" << std::endl;
        std::cout << "    --simulate-benchmark            Run performance benchmark" << std::endl;
        
        std::cout << "\nUse 'help <command>' for specific command details." << std::endl;
    } else {
        std::cout << "Help for command: " << command << std::endl;
        std::cout << "(Help implementation for " << command << ")" << std::endl;
    }
}

void CommandExecutionFramework::printHelpGroup(const std::string& group) {
    std::cout << "Help for command group: " << group << std::endl;
    std::cout << "(Help implementation for group " << group << ")" << std::endl;
}

std::vector<std::string> CommandExecutionFramework::getCommandCompletions(const std::string& partialCommand) {
    std::vector<std::string> completions;
    
    // Basic completion logic - returns all commands starting with partialCommand
    for (const auto& entry : commandDescriptions) {
        if (entry.first.find(partialCommand) == 0) {
            completions.push_back(entry.first);
        }
    }
    
    return completions;
}

void CommandExecutionFramework::addCommandToHistory(const std::string& command) {
    commandHistory.push_back(command);
    
    // Keep history size limited
    if (commandHistory.size() > 1000) {
        commandHistory.erase(commandHistory.begin());
    }
}

std::vector<std::string> CommandExecutionFramework::getCommandHistory() {
    return commandHistory;
}

bool CommandExecutionFramework::handleCommandError(const std::string& command, const std::string& error) {
    std::string errorMessage = "Error: " + error;
    if (!command.empty()) {
        errorMessage += " (while executing command: " + command + ")";
    }
    
    logger->log(errorMessage, LogLevel::Error);
    std::cerr << errorMessage << std::endl;
    return false;
}

void CommandExecutionFramework::registerCommand(const std::string& command, const std::string& description,
                                              const std::string& helpText, const std::string& group) {
    commandDescriptions[command] = description;
    commandHelpTexts[command] = helpText;
    commandGroups[command] = group;
}

void CommandExecutionFramework::registerCommandGroup(const std::string& group, const std::string& description) {
    // Store group information for help display
    std::string groupKey = "group_" + group;
    commandDescriptions[groupKey] = description;
}
