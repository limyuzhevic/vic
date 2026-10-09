#pragma once

#include "CLICommand.hpp"
#include <iostream>
#include <vector>
#include <string>

class MonitorAgentCommand : public CLICommand {
public:
    std::string getName() const override { return "monitor agent"; }
    std::string getDescription() const override { return "Monitor agent activity and neuromodulation"; }
    std::string getUsage() const override { return "monitor agent [--curiosity] [--reward-modulation] [--status]"; }
    
    int execute(const std::vector<std::string>& args) override {
        std::cout << "Monitor agent command executed" << std::endl;
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
    std::string getOptionalArgsHelp() const override { return "[--curiosity] [--reward-modulation] [--status]"; }
};