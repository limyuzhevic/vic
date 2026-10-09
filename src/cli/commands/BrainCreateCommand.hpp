#pragma once

#include "CLICommand.hpp"
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <vector>
#include <string>

class BrainCreateCommand : public CLICommand {
public:
    std::string getName() const override { return "brain create"; }
    std::string getDescription() const override { return "Create a new brain instance"; }
    std::string getUsage() const override { return "brain create <name> [--neurons N] [--regions R] [--config FILE]"; }
    
    int execute(const std::vector<std::string>& args) override {
        std::cout << "Brain create command executed" << std::endl;
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
    std::string getOptionalArgsHelp() const override { return "[--neurons N] [--regions R] [--config FILE]"; }
};