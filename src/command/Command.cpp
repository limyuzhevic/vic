#include "Command.hpp"
#include "../command/CommandManager.hpp"
#include "../core/ErrorHandling.hpp"
#include <memory>
#include <random>
#include <algorithm>

namespace nlm {

CommandManager::~CommandManager() = default;

bool CommandManager::removeCommand(const std::string& name) {
    try {
        auto it = std::remove_if(commands_.begin(), commands_.end(),
                               [&name](const std::unique_ptr<Command>& cmd) {
                                   return cmd->getName() == name;
                               });
        if (it != commands_.end()) {
            commands_.erase(it, commands_.end());
            NLM_LOG_INFO("Command removed: " + name);
            return true;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error removing command " + name + ": " + std::string(e.what()));
    }
    return false;
}

std::unique_ptr<Action> CommandManager::executeCommands(std::shared_ptr<Brain> brain) {
    try {
        sortCommandsByPriority();
        
        // Execute commands in priority order
        for (const auto& command : commands_) {
            if (command && command->isEnabled()) {
                auto action = command->execute();
                if (action && action->getType() != ActionType::Wait) {
                    NLM_LOG_INFO("Command executed: " + command->getName() + " -> " + action->getName());
                    return action;
                }
            }
        }
        
        // No enabled command produced an action, return wait
        NLM_LOG_DEBUG("No active commands found, returning wait action");
        return std::make_unique<Action>(ActionType::Wait);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error executing commands: " + std::string(e.what()));
        return std::make_unique<Action>(ActionType::Wait);
    }
}

Command* CommandManager::getCommand(const std::string& name) {
    try {
        for (auto& command : commands_) {
            if (command && command->getName() == name) {
                return command.get();
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting command " + name + ": " + std::string(e.what()));
    }
    return nullptr;
}

bool CommandManager::setCommandEnabled(const std::string& name, bool enabled) {
    Command* cmd = getCommand(name);
    if (cmd) {
        cmd->setEnabled(enabled);
        NLM_LOG_INFO((enabled ? "Command enabled: " : "Command disabled: ") + name);
        return true;
    }
    NLM_LOG_WARNING("Command not found: " + name);
    return false;
}

bool CommandManager::setCommandPriority(const std::string& name, float priority) {
    Command* cmd = getCommand(name);
    if (cmd) {
        cmd->setPriority(priority);
        NLM_LOG_INFO("Command priority set: " + name + " = " + std::to_string(priority));
        return true;
    }
    NLM_LOG_WARNING("Command not found: " + name);
    return false;
}

std::vector<Command*> CommandManager::getEnabledCommands() const {
    std::vector<Command*> enabled;
    try {
        for (const auto& command : commands_) {
            if (command && command->isEnabled()) {
                enabled.push_back(command.get());
            }
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error getting enabled commands: " + std::string(e.what()));
    }
    return enabled;
}

} // namespace nlm