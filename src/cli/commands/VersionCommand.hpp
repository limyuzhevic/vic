#pragma once

#include "CLICommand.hpp"
#include <iostream>
#include <vector>
#include <string>

class VersionCommand : public CLICommand {
public:
    std::string getName() const override { return "version"; }
    std::string getDescription() const override { return "Show version and build information"; }
    std::string getUsage() const override { return "version"; }
    
    int execute(const std::vector<std::string>& args) override {
        std::cout << "NLM CLI Version 0.1.0" << std::endl;
        std::cout << "Phase 1: Core CLI Framework" << std::endl;
        std::cout << "Built with C++20" << std::endl;
        std::cout << "Framework implementation complete" << std::endl;
        return 0;
    }
    
    bool acceptsArguments() const override { return false; }
    int getRequiredArgCount() const override { return 0; }
    std::string getOptionalArgsHelp() const override { return ""; }
};