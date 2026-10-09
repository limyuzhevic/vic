#pragma once

#include "CLICommand.hpp"
#include <iostream>
#include <vector>
#include <string>

class ExperimentRunCommand : public CLICommand {
public:
    std::string getName() const override { return "experiment run"; }
    std::string getDescription() const override { return "Run an experiment phase with specified steps"; }
    std::string getUsage() const override { return "experiment run [--brain NAME] <phase> <steps> [--continue]"; }
    
    int execute(const std::vector<std::string>& args) override {
        std::cout << "Experiment run command executed" << std::endl;
        std::cout << "Arguments: ";
        for (size_t i = 0; i < args.size(); ++i) {
            if (i > 0) std::cout << " ";
            std::cout << args[i];
        }
        std::cout << std::endl;
        return 0;
    }
    
    bool acceptsArguments() const override { return true; }
    int getRequiredArgCount() const override { return 2; }
    std::string getOptionalArgsHelp() const override { return "[--brain NAME] [--continue]"; }
};