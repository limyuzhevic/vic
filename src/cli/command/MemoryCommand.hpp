#pragma once

#include "cli/Command.hpp"
#include <string>
#include <vector>

namespace nlm {
namespace cli {

class MemoryCommand : public Command {
public:
    MemoryCommand(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
        : Command(brain, config) {}
    
    int execute(const std::vector<std::string>& args) override;
    std::string getName() const override { return "memory"; }
    std::string getDescription() const override { return "Memory system commands (show, clear, info)"; }
    std::string getUsage() const override { return "[action] [name] [--help]"; }
};

} // namespace cli
} // namespace nlm
