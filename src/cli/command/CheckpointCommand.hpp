#pragma once

#include "cli/Command.hpp"
#include <string>
#include <vector>

namespace nlm {
namespace cli {

class CheckpointCommand : public Command {
public:
    CheckpointCommand(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
        : Command(brain, config) {}
    
    int execute(const std::vector<std::string>& args) override;
    std::string getName() const override { return "checkpoint"; }
    std::string getDescription() const override { return "Checkpoint management commands (save, load, list)"; }
    std::string getUsage() const override { return "[action] [name] [--help]"; }
};

} // namespace cli
} // namespace nlm
