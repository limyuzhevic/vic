#pragma once

#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <unordered_map>
#include <iostream>
#include <stdexcept>
#include <algorithm>

using namespace nlm;

// Forward declarations
class CLI;
class CLIConfig;

/**
 * Command line argument parser for NLM
 */
class CLI {
public:
    CLI(const std::string& programName = "nlm");
    ~CLI();
    
    // Add a command (subcommand)
    void addCommand(const std::string& name, const std::string& description,
                    std::function<void(const CLI&)> handler);
    
    // Add an option (global or per-command)
    void addOption(const std::string& name, const std::string& description,
                   char shortOpt = 0, bool required = false);
    
    // Set option values
    void setOption(const std::string& name, const std::string& value);
    
    // Get option value
    std::string getOption(const std::string& name) const;
    bool hasOption(const std::string& name) const;
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Get the current command name
    std::string getCommand() const;
    
    // Get all arguments
    std::vector<std::string> getArgs() const;
    
    // Print help
    void printHelp() const;
    
    // Print version
    void printVersion() const;
    
    // Get program name
    std::string getProgramName() const;
    
    // Validate required options
    bool validate() const;
    
    // Get error message
    std::string getError() const;
    
    // Check if help was requested
    bool hasHelp() const { return helpRequested; }
    
    // Check if version was requested
    bool hasVersion() const { return versionRequested; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool helpRequested;
    bool versionRequested;
};

/**
 * Configuration manager for CLI
 */
class CLIConfig {
public:
    CLIConfig();
    ~CLIConfig();
    
    // Load configuration from file and CLI
    bool load(const std::string& configFile, const CLI& cli);
    
    // Get configuration value with CLI override
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue) const;
    
    // Save configuration
    bool save(const std::string& filepath) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Common CLI options for all NLM commands
 */
namespace CLICommon {
    // Standard option definitions
    static const std::string HELP = "--help";
    static const std::string VERSION = "--version";
    static const std::string CONFIG = "--config";
    static const std::string OUTPUT = "--output";
    static const std::string LOG_LEVEL = "--log-level";
    static const std::string VERBOSE = "-v";
    static const std::string QUIET = "-q";
    
    // Common descriptions
    static const std::string DESC_HELP = "Show help message";
    static const std::string DESC_VERSION = "Show version information";
    static const std::string DESC_CONFIG = "Configuration file path";
    static const std::string DESC_OUTPUT = "Output file path";
    static const std::string DESC_LOG_LEVEL = "Log level (DEBUG, INFO, WARN, ERROR)";
    static const std::string DESC_VERBOSE = "Verbose output";
    static const std::string DESC_QUIET = "Quiet output";
}

/**
 * NLM CLI application base class
 */
class NLMApplication {
public:
    NLMApplication(const std::string& name, const std::string& version);
    virtual ~NLMApplication();
    
    // Run the application
    virtual int run(int argc, char** argv) = 0;
    
    // Get the CLI instance
    CLI& getCLI();
    
    // Get the program name
    std::string getName() const;
    
    // Get the version
    std::string getVersion() const;
    
protected:
    // Register commands and options
    virtual void registerCommands() = 0;
    virtual void registerOptions() = 0;
    
    // Handle common options
    virtual void handleHelp() const;
    virtual void handleVersion() const;
    virtual void handleConfig() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Command handler function type
 */
using CommandHandler = std::function<void(const CLI& cli)>;

/**
 * Option handler function type
 */
using OptionHandler = std::function<void(const std::string& value)>;

} // namespace nlm
