#pragma once

#include "CLICommand.hpp"
#include "HelpSystem.hpp"
#include <iostream>
#include <vector>
#include <string>

class HelpCommand : public CLICommand {
public:
    HelpCommand() : helpSystem(nullptr) {}
    
    ~HelpCommand() {
        delete helpSystem;
    }
    
    std::string getName() const override { return "help"; }
    std::string getDescription() const override { return "Show help information for commands"; }
    std::string getUsage() const override { return "help [command]"; }
    
    void setHelpSystem(HelpSystem* system) {
        helpSystem = system;
    }
    
    int execute(const std::vector<std::string>& args) override {
        if (!helpSystem) {
            std::cout << "Help system not initialized" << std::endl;
            return 1;
        }
        
        if (args.empty()) {
            helpSystem->showGeneralHelp();
        } else {
            return helpSystem->showCommandHelp(args[0]);
        }
        return 0;
    }
    
    bool acceptsArguments() const override { return true; }
    int getRequiredArgCount() const override { return 0; }
    std::string getOptionalArgsHelp() const override { return "[command]"; }
    
private:
    HelpSystem* helpSystem;
};