#pragma once

#include "CLICommand.hpp"
#include <iostream>
#include <vector>
#include <string>

class ConfigSetCommand : public CLICommand {
public:
    std::string getName() const override { return "config set"; }
    std::string getDescription() const override { return "Set configuration values"; }
    std::string getUsage() const override { return "config set [--global] [--brain NAME] <key> <value>"; }
    
    int execute(const std::vector<std::string>& args) override {
        std::cout << "Config set command executed" << std::endl;
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
    std::string getOptionalArgsHelp() const override { return "[--global] [--brain NAME]"; }
};