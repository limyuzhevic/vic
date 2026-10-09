#pragma once

#include "CLICommand.hpp"
#include <iostream>
#include <vector>
#include <string>

class BrainSaveCommand : public CLICommand {
public:
    std::string getName() const override { return "brain save"; }
    std::string getDescription() const override { return "Save a brain state to a file"; }
    std::string getUsage() const override { return "brain save <name> <file> [--overwrite]"; }
    
    int execute(const std::vector<std::string>& args) override {
        std::cout << "Brain save command executed" << std::endl;
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
    std::string getOptionalArgsHelp() const override { return "[--overwrite]"; }
};