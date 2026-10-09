#pragma once

#include "CLICommand.hpp"
#include <iostream>
#include <vector>
#include <string>

class ConfigValidateCommand : public CLICommand {
public:
    std::string getName() const override { return "config validate"; }
    std::string getDescription() const override { return "Validate configuration and fix issues"; }
    std::string getUsage() const override { return "config validate [--global] [--brain NAME] [--fix-duplicates]"; }
    
    int execute(const std::vector<std::string>& args) override {
        std::cout << "Config validate command executed" << std::endl;
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
    std::string getOptionalArgsHelp() const override { return "[--global] [--brain NAME] [--fix-duplicates]"; }
};