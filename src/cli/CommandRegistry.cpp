#include "CommandRegistry.h"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <sstream>

namespace nlm {

struct CommandRegistry::Impl {
    std::unordered_map<std::string, std::shared_ptr<CLICommandBase>> commands;
    std::vector<CommandResult> executionHistory;
    std::unordered_map<std::string, std::vector<CommandResult>> commandHistories;
    std::ostream* outputStream;
    std::ostream* errorStream;
    mutable std::mutex mutex;
    
    Impl() : outputStream(nullptr), errorStream(nullptr) {}
};

CommandRegistry::CommandRegistry() : pImpl(std::make_unique<Impl>()) {
    pImpl->outputStream = &std::cout;
    pImpl->errorStream = &std::cerr;
}

CommandRegistry::~CommandRegistry() = default;

template<typename T>
void CommandRegistry::registerCommand(std::shared_ptr<TypedCLICommand<T>> command) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->commands[command->getName()] = command;
}

CommandResult CommandRegistry::executeCommand(const std::string& commandName,
                                            const std::vector<std::string>& args,
                                            CommandExecutionContext& context) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->commands.find(commandName);
    if (it == pImpl->commands.end()) {
        return CommandResult(false, "Unknown command: " + commandName);
    }
    
    auto startTime = std::chrono::steady_clock::now();
    CommandResult result;
    
    try {
        if constexpr (std::is_same<T, CommandExecutionContext>::value) {
            result = it->second->execute(args, context);
        } else {
            result = static_cast<TypedCLICommand<T>*>(it->second.get())->execute(context, args);
        }
    } catch (const std::exception& e) {
        result = CommandResult(false, std::string("Exception: ") + e.what());
    }
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    result.executionTime = duration.count() / 1000.0;
    
    pImpl->executionHistory.push_back(result);
    pImpl->commandHistories[commandName].push_back(result);
    
    if (result.success) {
        (*pImpl->outputStream) << "[" << context.getCurrentTime() << "] Command " << commandName << " completed successfully in "
                              << result.executionTime << " seconds" << std::endl;
        if (!result.message.empty()) {
            (*pImpl->outputStream) << "  " << result.message << std::endl;
        }
    } else {
        (*pImpl->errorStream) << "[" << context.getCurrentTime() << "] Command " << commandName << " failed: "
                             << result.message << " (in " << result.executionTime << " seconds)" << std::endl;
    }
    
    return result;
}

std::vector<std::string> CommandRegistry::getCommandNames() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    for (const auto& pair : pImpl->commands) {
        names.push_back(pair.first);
    }
    return names;
}

std::string CommandRegistry::getCommandHelp(const std::string& commandName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->commands.find(commandName);
    if (it == pImpl->commands.end()) {
        return "Unknown command: " + commandName;
    }
    
    return it->second->getHelp();
}

bool CommandRegistry::hasCommand(const std::string& commandName) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return pImpl->commands.find(commandName) != pImpl->commands.end();
}

std::vector<std::shared_ptr<CLICommandBase>> CommandRegistry::getAllCommands() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::shared_ptr<CLICommandBase>> commands;
    for (const auto& pair : pImpl->commands) {
        commands.push_back(pair.second);
    }
    return commands;
}

void CommandRegistry::setOutputStream(std::ostream* outputStream) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->outputStream = outputStream;
}

void CommandRegistry::setErrorStream(std::ostream* errorStream) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->errorStream = errorStream;
}

void CommandRegistry::cleanup() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->commands.clear();
    pImpl->executionHistory.clear();
    pImpl->commandHistories.clear();
}

std::vector<CommandResult> CommandRegistry::getCommandHistory(const std::string& commandName, size_t maxCount) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->commandHistories.find(commandName);
    if (it == pImpl->commandHistories.end()) {
        return std::vector<CommandResult>();
    }
    
    auto& history = it->second;
    size_t startIndex = history.size() > maxCount ? history.size() - maxCount : 0;
    return std::vector<CommandResult>(history.begin() + startIndex, history.end());
}

void CommandRegistry::clearHistory(const std::string& commandName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->commandHistories[commandName].clear();
}

} // namespace nlm