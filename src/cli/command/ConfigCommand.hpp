#pragma once

#include "cli/Command.hpp"
#include <string>
#include <vector>

namespace nlm {
namespace cli {

class ConfigCommand : public Command {
public:
    ConfigCommand(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
        : Command(brain, config) {}
    
    int execute(const std::vector<std::string>& args) override;
    std::string getName() const override { return "config"; }
    std::string getDescription() const override { return "Configuration management (show, set, export, import)"; }
    std::string getUsage() const override { return "[action] [key=value] [file] [--help]"; }
};

} // namespace cli
} // namespace nlm
