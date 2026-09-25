#pragma once

#include "cli/Command.hpp"
#include <string>
#include <vector>

namespace nlm {
namespace cli {

class RunCommand : public Command {
public:
    RunCommand(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
        : Command(brain, config) {}
    
    int execute(const std::vector<std::string>& args) override;
    std::string getName() const override { return "run"; }
    std::string getDescription() const override { return "Run the NLM brain simulation"; }
    std::string getUsage() const override { return "[steps] [--help]"; }
};

} // namespace cli
} // namespace nlm
