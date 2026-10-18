// NLM Advanced CLI Integration
// Main entry point for enhanced command-line interface

#pragma once

#include "AdvancedCLI.hpp"
#include "commands/MemoryManagementCommands.hpp"
#include "commands/NeuromodulationControlCommands.hpp"
#include "commands/StateAnalysisCommands.hpp"
#include "commands/SimulationControlCommands.hpp"
#include "commands/ScriptingAndDiagnosticsCommands.hpp"
#include <memory>

namespace nlm {
namespace cli {

// Advanced CLI manager that integrates with NLM brain system
class NLMAdvancedCLI {
public:
    NLMAdvancedCLI(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    ~NLMAdvancedCLI() = default;
    
    // Initialize CLI with all registered commands
    void initialize();
    
    // Process command line arguments
    int processArguments(int argc, char** argv);
    
    // Run interactive CLI session
    int runInteractiveSession();
    
    // Process single command line
    bool processCommandLine(const std::string& line);
    
    // Show help information
    void showHelp() const;
    
    // Get command history
    std::vector<std::string> getCommandHistory() const { return cli_.getCommandHistory(); }
    
    // Set verbose mode
    void setVerbose(bool verbose) { cli_.setVerbose(verbose); }
    
    // Set quiet mode
    void setQuiet(bool quiet) { cli_.setQuiet(quiet); }
    
    // Check if running in interactive mode
    bool isInteractiveMode() const { return interactiveMode_; }
    
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<Config> config_;
    AdvancedCLI cli_;
    bool interactiveMode_;
    
    // Command line parsing
    struct ParsedCommand {
        std::string commandName;
        std::vector<std::string> arguments;
        std::map<std::string, std::string> options;
    };
    
    // Helper methods
    ParsedCommand parseCommandLine(const std::vector<std::string>& tokens) const;
    bool executeCommand(const ParsedCommand& parsed) const;
    void setupCommandOptions(ParsedCommand& parsed, const std::vector<std::string>& tokens) const;
};

} // namespace cli
} // namespace nlm
