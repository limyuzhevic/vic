#include "AdvancedAgentBrain.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>

namespace nlm {

AdvancedAgentBrain::AdvancedAgentBrain(std::shared_ptr<AgentBrain> agent)
    : agent_(agent)
    , commandManager_(std::make_unique<CommandManager>(agent->getBrain()))
    , explorationMode_(false)
    , learningMode_(false) {
    
    NLM_LOG_INFO("AdvancedAgentBrain initialized");
}

AdvancedAgentBrain::~AdvancedAgentBrain() {
    NLM_LOG_INFO("AdvancedAgentBrain destroyed");
}

CommandResult AdvancedAgentBrain::execute(const std::string& commandLine) {
    return commandManager_->executeCommand(commandLine);
}

CommandResult AdvancedAgentBrain::execute(ComplexAction action) {
    return commandManager_->executeComplexAction(action);
}

bool AdvancedAgentBrain::loadScript(const std::string& filename) {
    return commandManager_->loadScript(filename);
}

bool AdvancedAgentBrain::executeScript(const std::string& scriptName) {
    return commandManager_->executeScript(scriptName);
}

bool AdvancedAgentBrain::scheduleScript(const std::string& scriptName, double delay) {
    return commandManager_->scheduleScript(scriptName, delay);
}

void AdvancedAgentBrain::addToHistory(const std::string& command) {
    commandManager_->addToHistory(command);
}

std::vector<std::string> AdvancedAgentBrain::getCommandHistory(size_t limit) const {
    return commandManager_->getCommandHistory(limit);
}

std::vector<std::string> AdvancedAgentBrain::getCommandCompletions(const std::string& partial) const {
    return commandManager_->getCommandCompletions(partial);
}

void AdvancedAgentBrain::registerCommand(const std::string& name, std::unique_ptr<Command> command) {
    commandManager_->registerCommand(name, std::move(command));
}

bool AdvancedAgentBrain::hasCommand(const std::string& name) const {
    return commandManager_->hasCommand(name);
}

void AdvancedAgentBrain::reset() {
    commandManager_->reset();
    explorationMode_ = false;
    learningMode_ = false;
}

void AdvancedAgentBrain::saveState(const std::string& filename) const {
    commandManager_->saveState(filename);
}

bool AdvancedAgentBrain::loadState(const std::string& filename) {
    return commandManager_->loadState(filename);
}

void AdvancedAgentBrain::setVariable(const std::string& name, float value) {
    commandManager_->setVariable(name, value);
}

float AdvancedAgentBrain::getVariable(const std::string& name) const {
    return commandManager_->getVariable(name);
}

bool AdvancedAgentBrain::hasVariable(const std::string& name) const {
    return commandManager_->hasVariable(name);
}

void AdvancedAgentBrain::setContextVariable(const std::string& name, const std::string& value) {
    commandManager_->setContextVariable(name, value);
}

std::string AdvancedAgentBrain::getContextVariable(const std::string& name) const {
    return commandManager_->getContextVariable(name);
}

CommandResult AdvancedAgentBrain::executeActionSequence(const std::vector<ComplexAction>& actions) {
    CommandResult result;
    
    try {
        std::string sequenceDescription = "Executing action sequence: ";
        for (size_t i = 0; i < actions.size(); ++i) {
            if (i > 0) sequenceDescription += ", ";
            sequenceDescription += std::to_string(static_cast<int>(actions[i]));
        }
        
        NLM_LOG_INFO(sequenceDescription);
        
        std::vector<CommandResult> actionResults;
        for (ComplexAction action : actions) {
            CommandResult actionResult = execute(action);
            actionResults.push_back(actionResult);
            
            if (!actionResult.success) {
                // Stop on first failure
                result = CommandResult(false, "Action sequence failed at action " + 
                                      std::to_string(static_cast<int>(action)) + ": " + 
                                      actionResult.message);
                return result;
            }
        }
        
        result = CommandResult(true, "Action sequence completed successfully");
        
        // Aggregate results
        for (const auto& actionResult : actionResults) {
            if (actionResult.executionTime > result.executionTime) {
                result.executionTime = actionResult.executionTime;
            }
            for (const auto& metric : actionResult.metrics) {
                result.metrics[metric.first] = metric.second;
            }
        }
        
    } catch (const std::exception& e) {
        result = CommandResult(false, std::string("Action sequence failed: ") + e.what());
    }
    
    return result;
}

CommandResult AdvancedAgentBrain::createAndExecuteMacro(const std::string& macroName,
                                                        const std::vector<std::string>& commands) {
    CommandResult result;
    
    try {
        NLM_LOG_INFO("Creating macro: " + macroName);
        
        // Store macro
        macros_[macroName] = commands;
        
        // Execute macro
        result = CommandResult(true, "Macro created and executed: " + macroName);
        
        // Execute commands
        std::string executionLog = "Macro execution log for " + macroName + ":\n";
        for (size_t i = 0; i < commands.size(); ++i) {
            CommandResult cmdResult = execute(commands[i]);
            executionLog += "  " + std::to_string(i) + ". " + commands[i] + 
                           " -> " + (cmdResult.success ? "SUCCESS" : "FAILED") + "\n";
            
            if (!cmdResult.success) {
                result = CommandResult(false, "Macro execution failed at command " + 
                                      std::to_string(i) + ": " + cmdResult.message);
                result.message = executionLog + result.message;
                return result;
            }
        }
        
        result.message = executionLog + "Macro executed successfully";
        
    } catch (const std::exception& e) {
        result = CommandResult(false, std::string("Macro creation/execution failed: ") + e.what());
    }
    
    return result;
}

} // namespace nlm
