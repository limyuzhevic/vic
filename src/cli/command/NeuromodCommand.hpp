#pragma once

#include "cli/Command.hpp"
#include <string>
#include <vector>

namespace nlm {
namespace cli {

class NeuromodCommand : public Command {
public:
    NeuromodCommand(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
        : Command(brain, config) {}
    
    int execute(const std::vector<std::string>& args) override;
    std::string getName() const override { return "neuromod"; }
    std::string getDescription() const override { return "Neuromodulation system commands (show, set)"; }
    std::string getUsage() const override { return "[action] [key=value] [--help]"; }
};

} // namespace cli
} // namespace nlm
