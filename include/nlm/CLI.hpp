#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <iostream>

namespace nlm {

// CLI argument structure
struct CLIArgs {
    std::string command;
    std::vector<std::string> args;
    std::map<std::string, std::string> flags;
    std::map<std::string, std::string> options;
    bool help = false;
    bool version = false;
    std::string configFile;
};

// Command interface
class CLICommand {
public:
    virtual ~CLICommand() = default;
    virtual std::string name() const = 0;
    virtual std::string description() const = 0;
    virtual bool execute(const CLIArgs& args) = 0;
    virtual void printHelp() const = 0;
};

// Main CLI class
class CLI {
public:
    CLI();
    ~CLI();
    
    // Add command
    void addCommand(std::unique_ptr<CLICommand> command);
    
    // Parse arguments and execute
    bool run(int argc, char** argv);
    
    // Print help
    void printHelp(const std::string& command = "") const;
    
    // Print version
    void printVersion() const;
    
private:
    CLIArgs parseArgs(int argc, char** argv);
    void setupCommands();
    
    std::vector<std::unique_ptr<CLICommand>> commands_;
};

} // namespace nlm