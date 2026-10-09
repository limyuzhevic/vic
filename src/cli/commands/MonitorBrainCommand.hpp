#pragma once

#include "CLICommand.hpp"
#include <iostream>
#include <vector>
#include <string>

class MonitorBrainCommand : public CLICommand {
public:
    std::string getName() const override { return "monitor brain"; }
    std::string getDescription() const override { return "Monitor a brain's activity and statistics"; }
    std::string getUsage() const override { return "monitor brain <name> [--stats] [--neuromodulation] [--spikes] [--activity]"; }
    
    int execute(const std::vector<std::string>& args) override {
        std::cout << "Monitor brain command executed" << std::endl;
        std::cout << "Arguments: ";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) std::cout << " ";
            std::cout << args[i];
        }
        std::cout << std::endl;
        return 0;
    }
    
    bool acceptsArguments() const override { return true; }
    int getRequiredArgCount() const override { return 1; }
    std::string getOptionalArgsHelp() const override { return "[--stats] [--neuromodulation] [--spikes] [--activity]"; }
};