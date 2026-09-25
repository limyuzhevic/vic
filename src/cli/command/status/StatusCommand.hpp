#pragma once

#include "cli/Command.hpp"
#include <string>
#include <vector>

namespace nlm {
namespace cli {

class StatusCommand : public Command {
public:
    StatusCommand(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
        : Command(brain, config) {}
    
    int execute(const std::vector<std::string>& args) override;
    std::string getName() const override { return "status"; }
    std::string getDescription() const override { return "Display brain system status and statistics"; }
    std::string getUsage() const override { return "[details] [--help]"; }
};

} // namespace cli
} // namespace nlm
