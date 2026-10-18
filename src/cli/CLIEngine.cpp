// CLI Engine Implementation
//
// This file provides the implementation of the CLI execution engine

#include "CLIEngine.hpp"
#include <fstream>
#include <filesystem>
#include <thread>
#include <future>
#include <algorithm>

namespace nlm {

CLIEngine::CLIEngine() : verbose_(false), quiet_(false) {
    startTime_ = std::chrono::steady_clock::now();
}

void CLIEngine::registerCommand(std::unique_ptr<CLICommand> command) {
    if (!command) return;
    
    std::string name = command->getName();
    commands_[name] = std::move(command);
    
    // Categorize the command
    CLICommand::CommandType type = commands_[name]->getType();
    std::string category = commands_[name]->getCategory();
    
    categorizeCommand(name, type, category);
    
    if (verbose_) {
        log("Registered command: " + name);
    }
}

void CLIEngine::registerCommandsFromDirectory(const std::string& directory) {
    // TODO: Implement dynamic command loading from directory
    log("Command loading from directory: " + directory, true);
}

bool CLIEngine::executeCommand(const std::string& commandName, 
                             const std::vector<std::string>& args,
                             Config& config) {
    auto it = commands_.find(commandName);
    if (it == commands_.end()) {
        logError("Unknown command: " + commandName);
        return false;
    }
    
    auto& command = it->second;
    
    // Validate arguments
    if (!command->validateArgs(args)) {
        logError("Invalid arguments for command: " + commandName);
        command->displayHelp();
        return false;
    }
    
    // Execute command
    CLICommand::ExecutionResult result = command->execute(args, config);
    
    switch (result) {
        case CLICommand::ExecutionResult::SUCCESS:
            log("Command completed successfully", true);
            return true;
        case CLICommand::ExecutionResult::FAILURE:
            logError("Command failed: " + commandName);
            return false;
        case CLICommand::ExecutionResult::VALIDATION_ERROR:
            logError("Validation error in command: " + commandName);
            command->displayHelp();
            return false;
        case CLICommand::ExecutionResult::FILE_ERROR:
            logError("File error in command: " + commandName);
            return false;
        case CLICommand::ExecutionResult::NOT_IMPLEMENTED:
            logError("Command not implemented: " + commandName);
            return false;
        default:
            logError("Unknown error in command: " + commandName);
            return false;
    }
}

std::vector<std::string> CLIEngine::getAvailableCommands() const {
    std::vector<std::string> commandNames;
    for (const auto& pair : commands_) {
        commandNames.push_back(pair.first);
    }
    std::sort(commandNames.begin(), commandNames.end());
    return commandNames;
}

std::vector<std::string> CLIEngine::getCommandsByType(CLICommand::CommandType type) const {
    std::vector<std::string> commandNames;
    auto it = commandsByType_.find(type);
    if (it != commandsByType_.end()) {
        commandNames = it->second;
    }
    std::sort(commandNames.begin(), commandNames.end());
    return commandNames;
}

std::vector<std::string> CLIEngine::getCommandsByCategory(const std::string& category) const {
    std::vector<std::string> commandNames;
    auto it = commandsByCategory_.find(category);
    if (it != commandsByCategory_.end()) {
        commandNames = it->second;
    }
    std::sort(commandNames.begin(), commandNames.end());
    return commandNames;
}

void CLIEngine::displayHelp(const std::string& commandName) const {
    if (commandName.empty()) {
        // Display general help
        std::cout << "=== NLM CLI Help ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Available commands:" << std::endl;
        
        std::cout << std::endl;
        std::cout << "Configuration Tools:" << std::endl;
        displayCommandList(CLICommand::CommandType::CONFIG);
        
        std::cout << std::endl;
        std::cout << "Experiment Management Tools:" << std::endl;
        displayCommandList(CLICommand::CommandType::EXPERIMENT);
        
        std::cout << std::endl;
        std::cout << "Brain Management Tools:" << std::endl;
        displayCommandList(CLICommand::CommandType::BRAIN);
        
        std::cout << std::endl;
        std::cout << "World/Environment Tools:" << std::endl;
        displayCommandList(CLICommand::CommandType::WORLD);
        
        std::cout << std::endl;
        std::cout << "Analysis Tools:" << std::endl;
        displayCommandList(CLICommand::CommandType::ANALYSIS);
        
        std::cout << std::endl;
        std::cout << "Utility Tools:" << std::endl;
        displayCommandList(CLICommand::CommandType::UTILITY);
        
        std::cout << std::endl;
        std::cout << "Visualization Tools:" << std::endl;
        displayCommandList(CLICommand::CommandType::VISUALIZATION);
        
        std::cout << std::endl;
        std::cout << "Development Tools:" << std::endl;
        displayCommandList(CLICommand::CommandType::DEVELOPMENT);
        
        std::cout << std::endl;
        std::cout << "Advanced Tools:" << std::endl;
        displayCommandList(CLICommand::CommandType::ADVANCED);
    } else {
        auto it = commands_.find(commandName);
        if (it != commands_.end()) {
            it->second->displayHelp();
        } else {
            logError("Unknown command: " + commandName);
        }
    }
}

void CLIEngine::displayCommandList(CLICommand::CommandType type) const {
    auto it = commandsByType_.find(type);
    if (it == commandsByType_.end() || it->second.empty()) {
        std::cout << "  (none)" << std::endl;
        return;
    }
    
    for (const auto& commandName : it->second) {
        auto cmdIt = commands_.find(commandName);
        if (cmdIt != commands_.end()) {
            std::cout << "  " << commandName << " - " 
                      << cmdIt->second->getDescription() << std::endl;
        }
    }
}

void CLIEngine::displayCommandCategories() const {
    std::cout << "=== Command Categories ===" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Configuration Tools:" << std::endl;
    std::cout << "  Manage configuration files, validation, and merging" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Experiment Management Tools:" << std::endl;
    std::cout << "  Run, compare, and export experiments" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Brain Management Tools:" << std::endl;
    std::cout << "  Save, load, and compare brain states" << std::endl;
    std::endl;
    
    std::cout << "World/Environment Tools:" << std::endl;
    std::cout << "  Manage world state and exploration" << std::endl;
    std::endl;
    
    std::cout << "Analysis Tools:" << std::endl;
    std::cout << "  Analyze metrics, performance, and neural properties" << std::endl;
    std::endl;
    
    std::cout << "Utility Tools:" << std::endl;
    std::cout << "  Version info, health checks, system stats" << std::endl;
    std::endl;
    
    std::cout << "Visualization Tools:" << std::endl;
    std::cout << "  Visualize simulations, results, and metrics" << std::endl;
    std::endl;
    
    std::cout << "Development Tools:" << std::endl;
    std::cout << "  Debug, trace, profile, monitor" << std::endl;
    std::endl;
    
    std::cout << "Advanced Tools:" << std::endl;
    std::cout << "  Batch and parallel execution" << std::endl;
    std::endl;
}

bool CLIEngine::loadConfiguration(const std::string& configFile, Config& config) {
    if (!std::filesystem::exists(configFile)) {
        logError("Configuration file does not exist: " + configFile);
        return false;
    }
    
    // Try to load as JSON first
    if (configFile.substr(configFile.find_last_of(".") + 1) == "json") {
        std::ifstream file(configFile);
        if (!file.is_open()) {
            logError("Failed to open configuration file: " + configFile);
            return false;
        }
        
        try {
            nlohmann::json jsonConfig;
            file >> jsonConfig;
            config.loadFromJson(jsonConfig);
            log("Loaded JSON configuration from: " + configFile);
        } catch (const std::exception& e) {
            logError("Failed to parse JSON configuration: " + std::string(e.what()));
            return false;
        }
    } else {
        // Try to load as traditional key=value format
        if (!config.loadFromFile(configFile)) {
            logError("Failed to load traditional configuration file: " + configFile);
            return false;
        }
        log("Loaded configuration from: " + configFile);
    }
    
    return true;
}

bool CLIEngine::saveConfiguration(const std::string& configFile, const Config& config) {
    std::ofstream file(configFile);
    if (!file.is_open()) {
        logError("Failed to open configuration file for writing: " + configFile);
        return false;
    }
    
    try {
        nlohmann::json jsonConfig;
        config.saveToJson(jsonConfig);
        file << jsonConfig.dump(2);
        log("Saved configuration to: " + configFile);
    } catch (const std::exception& e) {
        logError("Failed to save configuration: " + std::string(e.what()));
        return false;
    }
    
    return true;
}

void CLIEngine::setVerbose(bool verbose) {
    verbose_ = verbose;
}

void CLIEngine::setQuiet(bool quiet) {
    quiet_ = quiet;
}

bool CLIEngine::isVerbose() const {
    return verbose_;
}

bool CLIEngine::isQuiet() const {
    return quiet_;
}

void CLIEngine::log(const std::string& message, bool force) const {
    if (quiet_ && !force) return;
    std::cout << message << std::endl;
}

void CLIEngine::logError(const std::string& error) const {
    std::cerr << "[ERROR] " << error << std::endl;
}

void CLIEngine::logWarning(const std::string& warning) const {
    std::cerr << "[WARNING] " << warning << std::endl;
}

void CLIEngine::startTiming() {
    startTime_ = std::chrono::steady_clock::now();
}

double CLIEngine::stopTiming() const {
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
    return duration.count() / 1000000.0; // Convert to seconds
}

bool CLIEngine::executeBatch(const std::vector<std::string>& commands,
                            Config& config) {
    bool success = true;
    for (const auto& command : commands) {
        log("Executing batch command: " + command);
        if (!executeCommand(command, {}, config)) {
            success = false;
            logWarning("Batch command failed: " + command);
        }
    }
    return success;
}

bool CLIEngine::executeParallel(const std::string& command,
                               const std::vector<std::vector<std::string>>& argSets,
                               Config& config) {
    std::vector<std::future<bool>> futures;
    
    for (const auto& args : argSets) {
        futures.push_back(std::async(std::launch::async, [this, command, args, &config]() {
            return executeCommand(command, args, config);
        }));
    }
    
    bool allSuccess = true;
    for (auto& future : futures) {
        if (!future.get()) {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

void CLIEngine::categorizeCommand(const std::string& commandName, 
                                 CLICommand::CommandType type,
                                 const std::string& category) {
    commandsByType_[type].push_back(commandName);
    commandsByCategory_[category].push_back(commandName);
}

std::unique_ptr<CLICommand> CLIEngine::createCommand(const std::string& commandType) const {
    // TODO: Implement command factory pattern
    return nullptr;
}

} // namespace nlm