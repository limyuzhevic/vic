// Command Base Class - Abstract base for all CLI commands

#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

class Config;

// Forward declaration
class Config;

class CommandBase {
protected:
    std::string name;
    std::string description;
    
public:
    CommandBase(const std::string& cmdName, const std::string& cmdDesc)
        : name(cmdName), description(cmdDesc) {}
    
    virtual ~CommandBase() = default;
    
    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }
    
    virtual int execute(std::shared_ptr<Config> config, const CLIParser& parser) = 0;
    
    // Helper methods
    static void printError(const std::string& message) {
        std::cerr << "Error: " << message << std::endl;
    }
    
    static void printUsage(const std::string& commandName, const std::string& expectedArgs = "") {
        std::cerr << "Usage: nlm " << commandName;
        if (!expectedArgs.empty()) {
            std::cerr << " " << expectedArgs;
        }
        std::cerr << std::endl;
    }
};
