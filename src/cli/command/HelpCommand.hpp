#pragma once

#include "cli/Command.hpp"
#include <string>
#include <vector>

namespace nlm {
namespace cli {

class HelpCommand : public Command {
public:
    HelpCommand(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
        : Command(brain, config) {}
    
    int execute(const std::vector<std::string>& args) override;
    std::string getName() const override { return "help"; }
    std::string getDescription() const override { return "Display command help and usage"; }
    std::string getUsage() const override { return "[command] [--help]"; }
};

} // namespace cli
} // namespace nlm
