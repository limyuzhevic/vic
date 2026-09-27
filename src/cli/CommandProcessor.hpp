// Command processor for NLM CLI
#pragma once

#include "CLIParser.hpp"
#include "BaseCommand.hpp"
#include <memory>
#include <unordered_map>

namespace nlm {
namespace cli {

class CommandProcessor {
private:
    std::unordered_map<std::string, BaseCommand*> commands;
    std::shared_ptr<Logger> logger;
    std::shared_ptr<Config> config;
    
public:
    CommandProcessor() = default;
    
    virtual ~CommandProcessor() {
        for (auto& pair : commands) {
            delete pair.second;
        }
    }
    
    void setLogger(std::shared_ptr<Logger> log) {
        logger = log;
        for (auto& pair : commands) {
            pair.second->setLogger(log);
        }
    }
    
    void setConfig(std::shared_ptr<Config> cfg) {
        config = cfg;
        for (auto& pair : commands) {
            pair.second->setConfig(cfg);
        }
    }
    
    void registerCommand(BaseCommand* cmd) {
        commands[cmd->getName()] = cmd;
        cmd->setLogger(logger);
        cmd->setConfig(config);
    }
    
    CommandResult execute(Subcommand& subcommand) {
        auto it = commands.find(subcommand.getName());
        if (it == commands.end()) {
            return CommandResult(1, "Unknown command: " + subcommand.getName());
        }
        
        return it->second->execute();
    }
};

} // namespace cli
} // namespace nlm
