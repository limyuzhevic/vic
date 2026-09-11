// NLM Advanced Command Line Interface
// Expert-level command processing for NLM

#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>

namespace nlm {

// Command line arguments structure
struct AdvancedCommandOptions {
    std::string command;
    std::vector<std::string> args;
    std::map<std::string, std::string> flags;
    bool verbose;
    bool debug;
    std::string configFile;
    std::string outputFile;
    int verbosityLevel;
    bool jsonOutput;
};

// Advanced Command Processor class
class AdvancedCommandProcessor {
public:
    AdvancedCommandProcessor();
    ~AdvancedCommandProcessor();
    
    // Parse command line arguments
    AdvancedCommandOptions parse(int argc, char** argv);
    
    // Execute command
    bool execute(const AdvancedCommandOptions& options);
    
    // Register command handler
    void registerCommand(const std::string& name,
                        const std::string& description,
                        std::function<bool(const AdvancedCommandOptions&)> handler);
    
    // Show help for a specific command
    void showCommandHelp(const std::string& command) const;
    
    // Show all available commands
    void showHelp() const;
    
    // Initialize command system
    void initialize();
    
    // Cleanup
    void shutdown();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
