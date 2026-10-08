#include "AdvancedAgent.hpp"
#include "../core/Logger/Logger.hpp"
#include <sstream>
#include <algorithm>
#include <cmath>
#include <fstream>

namespace nlm {

CommandResult MoveCommand::execute(AgentBrain& agent, const std::map<std::string, std::string>& context) {
    CommandResult result;
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        // For now, use the basic motor command system
        // In a full implementation, this would convert to complex movements
        MotorCommand motorCmd;
        if (angle_ == 0.0f) {
            if (distance_ > 0) motorCmd = MotorCommand::MoveForward;
            else motorCmd = MotorCommand::MoveBackward;
        } else if (angle_ > 0 && angle_ < 90.0f) {
            motorCmd = MotorCommand::TurnLeft;
        } else {
            motorCmd = MotorCommand::TurnRight;
        }
        
        // Simulate movement (in real implementation, this would call world functions)
        NLM_LOG_INFO("Executing move command: distance=" + std::to_string(distance_) + 
                    ", angle=" + std::to_string(angle_));
        
        result = CommandResult(true, "Movement completed successfully");
        
    } catch (const std::exception& e) {
        result = CommandResult(false, std::string("Move command failed: ") + e.what());
    }
    
    auto endTime = std::chrono::steady_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    return result;
}

CommandResult WaitCommand::execute(AgentBrain& agent, const std::map<std::string, std::string>& context) {
    CommandResult result;
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        NLM_LOG_INFO("Executing wait command for " + std::to_string(duration_) + " seconds");
        
        // Simulate waiting
        // In real implementation, this would pause simulation
        result = CommandResult(true, "Wait command completed successfully");
        
    } catch (const std::exception& e) {
        result = CommandResult(false, std::string("Wait command failed: ") + e.what());
    }
    
    auto endTime = std::chrono::steady_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    return result;
}

CommandResult ExploreCommand::execute(AgentBrain& agent, const std::map<std::string, std::string>& context) {
    CommandResult result;
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        NLM_LOG_INFO("Executing explore command");
        
        // Create exploration behavior pattern
        // This would use the agent's curiosity and neuromodulation systems
        float curiosity = agent.getCuriosityLevel();
        float novelty = agent.getNoveltyLevel();
        
        // Higher exploration with higher curiosity/novelty
        float exploreMultiplier = 1.0f + curiosity * novelty * 2.0f;
        
        // Simulate exploration by injecting random currents into sensory neurons
        // In real implementation, this would modify the brain's sensory processing
        
        result = CommandResult(true, "Exploration completed successfully");
        result.metrics["curiosity_level"] = curiosity;
        result.metrics["novelty_level"] = novelty;
        result.metrics["explore_multiplier"] = exploreMultiplier;
        
    } catch (const std::exception& e) {
        result = CommandResult(false, std::string("Explore command failed: ") + e.what());
    }
    
    auto endTime = std::chrono::steady_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    return result;
}

CommandResult LearnCommand::execute(AgentBrain& agent, const std::map<std::string, std::string>& context) {
    CommandResult result;
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        NLM_LOG_INFO("Executing learn command");
        
        // Use the agent's reward modulation system for learning
        // This would simulate reward prediction error and plasticity
        
        float predictionError = agent.getPredictionError();
        float dopamine = agent.getNeuromodulationLevel();
        
        // Learning effectiveness based on prediction error
        float learningEffectiveness = std::abs(predictionError) * 0.5f + 0.5f;
        
        // Simulate learning by updating developmental state
        agent.applyRewardModulation(0.1f, 0.0f); // Small positive reward
        
        result = CommandResult(true, "Learning completed successfully");
        result.metrics["prediction_error"] = predictionError;
        result.metrics["dopamine_level"] = dopamine;
        result.metrics["learning_effectiveness"] = learningEffectiveness;
        
    } catch (const std::exception& e) {
        result = CommandResult(false, std::string("Learn command failed: ") + e.what());
    }
    
    auto endTime = std::chrono::steady_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    return result;
}

CommandManager::CommandManager(std::shared_ptr<AgentBrain> agent)
    : agent_(agent), verbose_(false), maxHistorySize_(100), commandCount_(0), totalExecutionTime_(0.0) {
    
    // Register default commands
    registerCommand("move", std::make_unique<MoveCommand>(1.0f, 0.0f));
    registerCommand("wait", std::make_unique<WaitCommand>(1.0));
    registerCommand("explore", std::make_unique<ExploreCommand>());
    registerCommand("learn", std::make_unique<LearnCommand>());
    registerCommand("help", std::make_unique<WaitCommand>(0.0)); // Placeholder
    
    NLM_LOG_INFO("CommandManager initialized");
}

CommandManager::~CommandManager() {
    NLM_LOG_INFO("CommandManager destroyed");
}

CommandResult CommandManager::executeCommand(const std::string& commandLine) {
    CommandResult result;
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        if (verbose_) {
            NLM_LOG_INFO("Executing command: " + commandLine);
        }
        
        // Parse command line
        std::string command = parseCommand(commandLine);
        std::vector<std::string> args = parseArguments(commandLine);
        
        // Check if command exists
        if (!hasCommand(command)) {
            std::string errorMsg = "Unknown command: " + command;
            if (verbose_) {
                NLM_LOG_WARNING(errorMsg);
            }
            result = CommandResult(false, errorMsg);
        } else {
            // Get command context
            std::map<std::string, std::string> context;
            for (size_t i = 0; i < args.size(); ++i) {
                context["arg_" + std::to_string(i)] = args[i];
            }
            
            // Execute command
            result = commands_[command]->execute(*agent_, context);
            
            if (verbose_ && result.success) {
                NLM_LOG_INFO("Command executed successfully: " + command);
            }
        }
        
        // Add to history
        addToHistory(commandLine);
        commandCount_++;
        totalExecutionTime_ += result.executionTime;
        
    } catch (const std::exception& e) {
        result = CommandResult(false, std::string("Command execution failed: ") + e.what());
    }
    
    auto endTime = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - startTime).count();
    result.executionTime = elapsed;
    
    return result;
}

CommandResult CommandManager::executeComplexAction(ComplexAction action) {
    CommandResult result;
    auto startTime = std::chrono::steady_clock::now();
    
    try {
        // Map complex actions to sequences of simple commands
        std::vector<std::string> actionSequence;
        
        switch (action) {
            case ComplexAction::MoveForwardThenTurnLeft:
                actionSequence = {"move 1.0 0.0", "turn -45.0"};
                break;
            case ComplexAction::ExploreArea:
                actionSequence = {"explore", "move 2.0 90.0", "explore", "move 2.0 -90.0"};
                break;
            case ComplexAction::ReturnToStart:
                actionSequence = {"move 3.0 180.0"}; // Simplified
                break;
            default:
                actionSequence = {"wait 1.0"}; // Default action
        }
        
        // Execute action sequence
        std::string fullResult = "Action " + std::to_string(static_cast<int>(action)) + " executed:\n";
        
        for (const std::string& cmd : actionSequence) {
            CommandResult cmdResult = executeCommand(cmd);
            fullResult += "  " + cmd + " -> " + (cmdResult.success ? "SUCCESS" : "FAILED") + "\n";
        }
        
        result = CommandResult(true, fullResult);
        
    } catch (const std::exception& e) {
        result = CommandResult(false, std::string("Complex action failed: ") + e.what());
    }
    
    auto endTime = std::chrono::steady_clock::now();
    result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
    
    return result;
}

bool CommandManager::loadScript(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to load script: " + filename);
            return false;
        }
        
        std::string scriptName = filename;
        size_t dotPos = scriptName.find_last_of('.');
        if (dotPos != std::string::npos) {
            scriptName = scriptName.substr(0, dotPos);
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        loadedScripts_[scriptName] = content;
        
        NLM_LOG_INFO("Script loaded: " + filename + " (name: " + scriptName + ")");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to load script: ") + e.what());
        return false;
    }
}

bool CommandManager::executeScript(const std::string& scriptName) {
    if (loadedScripts_.find(scriptName) == loadedScripts_.end()) {
        NLM_LOG_ERROR("Script not found: " + scriptName);
        return false;
    }
    
    try {
        std::string script = loadedScripts_[scriptName];
        
        // Very simple script parsing - in real implementation would be more sophisticated
        std::istringstream stream(script);
        std::string line;
        int lineNum = 0;
        
        while (std::getline(stream, line)) {
            lineNum++;
            
            // Skip empty lines and comments
            if (line.empty() || line.find('#') == 0) {
                continue;
            }
            
            // Parse simple script commands
            std::string trimmedLine = line;
            // Remove trailing whitespace
            trimmedLine.erase(trimmedLine.find_last_not_of(" \t\r\n") + 1);
            
            NLM_LOG_INFO("Script line " + std::to_string(lineNum) + ": " + trimmedLine);
            
            CommandResult cmdResult = executeCommand(trimmedLine);
            
            if (!cmdResult.success) {
                NLM_LOG_WARNING("Script line " + std::to_string(lineNum) + " failed: " + cmdResult.message);
                // Continue with other lines for now
            }
        }
        
        NLM_LOG_INFO("Script executed: " + scriptName);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to execute script: ") + e.what());
        return false;
    }
}

bool CommandManager::scheduleScript(const std::string& scriptName, double delay) {
    if (loadedScripts_.find(scriptName) == loadedScripts_.end()) {
        NLM_LOG_ERROR("Script not found for scheduling: " + scriptName);
        return false;
    }
    
    try {
        scheduledScripts_[scriptName].push_back(ScriptCommand(scriptName));
        
        if (delay > 0.0) {
            // In real implementation, would use scheduler
            NLM_LOG_INFO("Script scheduled: " + scriptName + " (delay: " + std::to_string(delay) + "s)");
        } else {
            NLM_LOG_INFO("Script scheduled: " + scriptName + " (immediate)");
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to schedule script: ") + e.what());
        return false;
    }
}

void CommandManager::addToHistory(const std::string& command) {
    commandHistory_.push_back(command);
    
    // Trim history if too large
    if (commandHistory_.size() > maxHistorySize_) {
        commandHistory_.erase(commandHistory_.begin());
    }
}

std::vector<std::string> CommandManager::getCommandHistory(size_t limit) const {
    size_t start = std::max(0, (int)commandHistory_.size() - (int)limit);
    return std::vector<std::string>(commandHistory_.begin() + start, commandHistory_.end());
}

std::vector<std::string> CommandManager::getCommandCompletions(const std::string& partial) const {
    std::vector<std::string> completions;
    
    for (const auto& pair : commands_) {
        if (pair.first.find(partial) == 0) {
            completions.push_back(pair.first);
        }
    }
    
    return completions;
}

void CommandManager::registerCommand(const std::string& name, std::unique_ptr<Command> command) {
    commands_[name] = std::move(command);
}

bool CommandManager::hasCommand(const std::string& name) const {
    return commands_.find(name) != commands_.end();
}

void CommandManager::reset() {
    commandHistory_.clear();
    variables_.clear();
    contextVariables_.clear();
    scheduledScripts_.clear();
    NLM_LOG_INFO("CommandManager reset");
}

void CommandManager::saveState(const std::string& filename) const {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to save state: " + filename);
            return;
        }
        
        file << "# CommandManager state\n";
        file << "# Command count: " << commandCount_ << "\n";
        file << "# Total execution time: " << totalExecutionTime_ << "\n";
        
        file << "\n# Command history:\n";
        for (const auto& cmd : commandHistory_) {
            file << cmd << "\n";
        }
        
        file << "\n# Variables:\n";
        for (const auto& var : variables_) {
            file << var.first << " = " << var.second << "\n";
        }
        
        NLM_LOG_INFO("CommandManager state saved: " + filename);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to save state: ") + e.what());
    }
}

bool CommandManager::loadState(const std::string& filename) {
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to load state: " + filename);
            return false;
        }
        
        std::string line;
        std::string section = "";
        
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            if (line.empty() || line.find('#') == 0) {
                continue;
            }
            
            // Simple parsing for now
            if (line.find(" = ") != std::string::npos) {
                size_t pos = line.find(" = ");
                std::string name = line.substr(0, pos);
                std::string valueStr = line.substr(pos + 3);
                
                try {
                    float value = std::stof(valueStr);
                    variables_[name] = value;
                } catch (...) {
                    // Try as string
                    variables_[name] = std::stof(valueStr); // Fallback
                }
            }
        }
        
        NLM_LOG_INFO("CommandManager state loaded: " + filename);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Failed to load state: ") + e.what());
        return false;
    }
}

void CommandManager::setVariable(const std::string& name, float value) {
    variables_[name] = value;
}

float CommandManager::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    if (it != variables_.end()) {
        return it->second;
    }
    return 0.0f;
}

bool CommandManager::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

void CommandManager::setContextVariable(const std::string& name, const std::string& value) {
    contextVariables_[name] = value;
}

std::string CommandManager::getContextVariable(const std::string& name) const {
    auto it = contextVariables_.find(name);
    if (it != contextVariables_.end()) {
        return it->second;
    }
    return "";
}

std::string CommandManager::parseCommand(const std::string& commandLine) const {
    std::string command = commandLine;
    
    // Remove leading/trailing whitespace
    size_t start = command.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    
    size_t end = command.find_last_not_of(" \t\r\n");
    command = command.substr(start, end - start + 1);
    
    // Find first space to separate command from arguments
    size_t spacePos = command.find(' ');
    if (spacePos != std::string::npos) {
        command = command.substr(0, spacePos);
    }
    
    return command;
}

std::vector<std::string> CommandManager::parseArguments(const std::string& commandLine) const {
    std::vector<std::string> args;
    std::string argsStr = commandLine;
    
    // Remove command part
    size_t spacePos = argsStr.find(' ');
    if (spacePos != std::string::npos) {
        argsStr = argsStr.substr(spacePos + 1);
    }
    
    // Simple argument parsing - handles quoted strings and basic types
    std::istringstream stream(argsStr);
    std::string token;
    
    while (stream >> token) {
        // Remove quotes if present
        if (token.size() >= 2 && token.front() == '"' && token.back() == '"') {
            token = token.substr(1, token.size() - 2);
        }
        args.push_back(token);
    }
    
    return args;
}

std::string CommandManager::generateCompletion(const std::string& partial) const {
    std::string completion;
    
    for (const auto& pair : commands_) {
        if (pair.first.find(partial) == 0) {
            completion += pair.first + " ";
        }
    }
    
    return completion;
}

} // namespace nlm
