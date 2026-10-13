#pragma once

#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../development/DevelopmentSystem.hpp"

namespace nlm {

// Development console command
struct DevCommand {
    std::string name;
    std::string description;
    std::function<void(std::shared_ptr<Config>, std::shared_ptr<Brain>, std::shared_ptr<DevelopmentSystem>)> action;
};

// Development console entry (command history)
struct DevConsoleEntry {
    std::string input;
    std::string output;
    std::string timestamp;
};

// Development console interface
class DevConsole {
public:
    DevConsole();
    ~DevConsole();
    
    // Initialize console with brain and config
    void initialize(std::shared_ptr<Config> config, 
                    std::shared_ptr<Brain> brain,
                    std::shared_ptr<DevelopmentSystem> developmentSystem);
    
    // Process user input
    bool processInput(const std::string& input);
    
    // Run console loop (blocking)
    void run(bool continuous = false);
    
    // Stop console
    void stop();
    
    // Get command history
    std::vector<DevConsoleEntry> getHistory() const { return history_; }
    
    // Clear history
    void clearHistory() { history_.clear(); }
    
    // Get help for commands
    std::string getHelp() const;
    
    // Add custom command
    void addCommand(const DevCommand& command);
    
    // Remove command
    void removeCommand(const std::string& name);
    
    // Check if console is active
    bool isActive() const { return isActive_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool isActive_;
    std::vector<DevConsoleEntry> history_;
    std::vector<DevCommand> commands_;
    
    // Process special commands
    bool processSpecialCommand(const std::string& input);
    
    // Print help
    void printHelp(const std::string& command = "") const;
    
    // Print brain status
    void printBrainStatus() const;
    
    // Print development status
    void printDevelopmentStatus() const;
    
    // Print memory status
    void printMemoryStatus() const;
    
    // Print performance stats
    void printPerformanceStats() const;
    
    // Print configuration
    void printConfig() const;
    
    // Execute command
    void executeCommand(const std::string& command, const std::string& args);
};

} // namespace nlm