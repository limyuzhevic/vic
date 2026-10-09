#pragma once

#include "CLICommand.hpp"
#include <iostream>
#include <vector>
#include <string>

class AgentEnableCommand : public CLICommand {
public:
    std::string getName() const override { return "agent enable"; }
    std::string getDescription() const override { return "Enable agent with specific neuromodulation features"; }
    std::string getUsage() const override { return "agent enable [--curiosity] [--reward-modulation] [--neuromodulation]"; }
    
    int execute(const std::vector<std::string>& args) override {
        std::cout << "Agent enable command executed" << std::endl;
        std::cout << "Arguments: ";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) std::cout << " ";
            std::cout << args[i];
        }
        std::cout << std::endl;
        return 0;
    }
    
    bool acceptsArguments() const override { return true; }
    int getRequiredArgCount() const override { return 0; }
    std::string getOptionalArgsHelp() const override { return "[--curiosity] [--reward-modulation] [--neuromodulation]"; }
};