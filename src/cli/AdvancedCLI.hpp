// NLM Advanced CLI Framework
// Enhanced command-line interface for advanced users

#pragma once

#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>

namespace nlm {
namespace cli {

// Command execution context
struct CommandContext {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    std::ostream& output;
    std::istream& input;
    std::map<std::string, std::string> options;
    bool verbose;
    bool quiet;
    
    CommandContext(std::shared_ptr<Brain> b, std::shared_ptr<Config> c, 
                  std::ostream& out, std::istream& in)
        : brain(b), config(c), output(out), input(in), verbose(false), quiet(false) {}
};

// Base command class
class Command {
public:
    virtual ~Command() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual std::string getUsage() const = 0;
    virtual void execute(const CommandContext& context) = 0;
    virtual bool validate(const CommandContext& context) const { return true; }
};

// Advanced CLI class
class AdvancedCLI {
public:
    AdvancedCLI(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    ~AdvancedCLI() = default;
    
    // Register a command
    void registerCommand(std::unique_ptr<Command> command);
    
    // Execute command by name
    bool executeCommand(const std::string& commandName, const std::vector<std::string>& args);
    
    // Parse and execute command line
    bool executeLine(const std::string& line);
    
    // Show help for command or all commands
    void showHelp(const std::string& commandName = "") const;
    
    // Show command list
    void showCommands() const;
    
    // Auto-complete for commands
    std::vector<std::string> completeCommand(const std::string& partial) const;
    
    // Get command history
    std::vector<std::string> getCommandHistory() const { return history_; }
    
    // Clear command history
    void clearHistory() { history_.clear(); }
    
    // Set verbose mode
    void setVerbose(bool verbose) { verbose_ = verbose; }
    
    // Set quiet mode
    void setQuiet(bool quiet) { quiet_ = quiet; }
    
private:
    std::map<std::string, std::unique_ptr<Command>> commands_;
    std::vector<std::string> history_;
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
    bool verbose_;
    bool quiet_;
    
    // Helper methods
    std::string trim(const std::string& str) const;
    std::string toLower(const std::string& str) const;
    std::vector<std::string> split(const std::string& str) const;
};

} // namespace cli
} // namespace nlm
