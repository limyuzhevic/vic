#include "CLICommand.hpp"
#include <stdexcept>

namespace nlm {

CLICommand::CLICommand(const std::string& name, const std::string& description,
                       CommandType type, std::function<void()> handler)
    : name_(name), description_(description), type_(type), handler_(handler) {}

BasicCommand::BasicCommand(const std::string& name, const std::string& description,
                           std::function<void()> handler)
    : CLICommand(name, description, CommandType::Basic, handler) {}

Phase6Command::Phase6Command(const std::string& name, const std::string& description,
                             std::function<void()> handler)
    : CLICommand(name, description, CommandType::Phase6, handler) {}

AdvancedCommand::AdvancedCommand(const std::string& name, const std::string& description,
                                 std::function<void()> handler)
    : CLICommand(name, description, CommandType::Advanced, handler) {}

ConfigCommand::ConfigCommand(const std::string& name, const std::string& description,
                             std::function<void()> handler)
    : CLICommand(name, description, CommandType::Config, handler) {}

void CLICommandRegistry::registerCommand(std::unique_ptr<CLICommand> cmd) {
    commands_.push_back(std::move(cmd));
    commandMap_[cmd->getName()] = cmd.get();
}

std::vector<CLICommand*> CLICommandRegistry::getCommands(CLICommand::CommandType type) const {
    std::vector<CLICommand*> result;
    for (const auto& cmd : commands_) {
        if (cmd->getType() == type) {
            result.push_back(cmd.get());
        }
    }
    return result;
}

CLICommand* CLICommandRegistry::getCommand(const std::string& name) const {
    auto it = commandMap_.find(name);
    if (it != commandMap_.end()) {
        return it->second;
    }
    return nullptr;
}

} // namespace nlm