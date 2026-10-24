// Simplified NLM CLI implementation using modern C++20
// This is a simpler version that doesn't require external CLI20 library
// but provides comprehensive CLI functionality for NLM

#ifndef NLMCLI_HPP
#define NLMCLI_HPP

#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <chrono>
#include <thread>
#include <map>
#include <functional>

namespace nlm {
namespace cli {

// Configuration for NLM CLI
struct NLMConfig {
    std::string logLevel = "info";
    std::string logFile = "";
    bool verbose = false;
    bool quiet = false;
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    std::string checkpointDir = "./checkpoints";
    bool jsonOutput = false;
    bool humanReadable = true;
    std::chrono::milliseconds timeout{0};
    int maxSteps = 1000;
    bool debugMode = false;
    bool dryRun = false;
};

// Forward declarations for command callbacks
using CommandCallback = std::function<bool(const std::vector<std::string>&)>;
using CommandHandler = std::function<bool(const std::map<std::string, std::string>&)>;

// Simple command structure
struct Command {
    std::string name;
    std::string description;
    std::map<std::string, std::string> options;
    std::vector<std::string> args;
    CommandCallback callback;
};

// Main CLI implementation
class NLMCommandLine {
public:
    NLMCommandLine();
    ~NLMCommandLine();
    
    // Add a command
    void addCommand(const Command& cmd);
    
    // Parse command line arguments (simplified version)
    static bool parse(int argc, char* argv[], NLMConfig& config, std::map<std::string, std::string>& parsedArgs);
    
    // Execute parsed command
    bool execute();
    
    // Get configuration
    const NLMConfig& getConfig() const { return config_; }
    
    // Show help
    void showHelp(const std::string& command = "") const;
    
    // Show version
    static void showVersion();
    
    // Get all available commands
    const std::vector<Command>& getCommands() const { return commands_; }

private:
    NLMConfig config_;
    std::vector<Command> commands_;
    
    // Command handlers
    bool handleBrainCommand(const std::map<std::string, std::string>& args);
    bool handleExperimentCommand(const std::map<std::string, std::string>& args);
    bool handleConfigCommand(const std::map<std::string, std::string>& args);
    bool handleMonitorCommand(const std::map<std::string, std::string>& args);
    bool handleExportCommand(const std::map<std::string, std::string>& args);
    bool handleImportCommand(const std::map<std::string, std::string>& args);
    
    // Utility functions
    bool parseKeyValueArgs(int argc, char* argv[], std::map<std::string, std::string>& result);
    bool parseSimpleArgs(int argc, char* argv[], std::map<std::string, std::string>& result);
    void printUsage(const std::string& programName) const;
    void printCommandHelp(const Command& cmd) const;
    std::string trim(const std::string& str) const;
};

} // namespace cli
} // namespace nlm

#endif // NLMCLI_HPP
