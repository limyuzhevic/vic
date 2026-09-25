#pragma once

#include "cli/Command.hpp"
#include <string>
#include <vector>

namespace nlm {
namespace cli {

class DevelopCommand : public Command {
public:
    DevelopCommand(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
        : Command(brain, config) {}
    
    int execute(const std::vector<std::string>& args) override;
    std::string getName() const override { return "develop"; }
    std::string getDescription() const override { return "Development system commands (show, stage)"; }
    std::string getUsage() const override { return "[action] [--help]"; }
};

} // namespace cli
} // namespace nlm
