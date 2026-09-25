#pragma once

#include "cli/Command.hpp"
#include <string>
#include <vector>

namespace nlm {
namespace cli {

class ExperimentCommand : public Command {
public:
    ExperimentCommand(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
        : Command(brain, config) {}
    
    int execute(const std::vector<std::string>& args) override;
    std::string getName() const override { return "experiment"; }
    std::string getDescription() const override { return "Run specific experiments"; }
    std::string getUsage() const override { return "[name] [--help]"; }
};

} // namespace cli
} // namespace nlm
