// src/cli/AdvancedCLI.hpp
#pragma once

#include <string>
#include <memory>
#include <vector>
#include "../core/Config/Config.hpp"

namespace nlm {

/**
 * CLI status enumeration
 */
enum class CLIStatus {
    Success,
    Error,
    HelpRequested,
    VersionRequested
};

/**
 * Advanced command-line interface for NLM (Neural Learning Machine)
 * 
 * Provides enhanced simulation controls, configuration management,
 * and interactive capabilities while maintaining backward compatibility
 * with existing NLM demonstrations.
 */
class AdvancedCLI {
public:
    AdvancedCLI();
    ~AdvancedCLI();
    
    /**
     * Parse command-line arguments and execute appropriate commands
     * 
     * @param argc Argument count
     * @param argv Argument values
     * @return CLI status
     */
    CLIStatus parseArguments(int argc, char* argv[]);
    
    /**
     * Set configuration file path
     * 
     * @param configPath Path to configuration file
     * @return true if configuration file exists and can be parsed
     */
    bool setConfigFile(const std::string& configPath);
    
    /**
     * Get configuration file path
     * 
     * @return Current configuration file path
     */
    std::string getConfigFile() const;
    
    /**
     * Load configuration from file
     * 
     * @param config Shared pointer to configuration object
     * @return true if configuration loaded successfully
     */
    bool loadConfiguration(std::shared_ptr<Config> config);
    
    /**
     * Save configuration to file
     * 
     * @param config Shared pointer to configuration object
     * @param outputPath Output file path
     * @return true if configuration saved successfully
     */
    bool saveConfiguration(std::shared_ptr<Config> config, const std::string& outputPath);
    
    /**
     * Display help information
     * 
     * @param programName Name of the program
     */
    void displayHelp(const std::string& programName);
    
    /**
     * Display version information
     * 
     * @param programName Name of the program
     */
    void displayVersion(const std::string& programName);
    
    /**
     * Get list of available subcommands
     * 
     * @return Vector of available subcommand names
     */
    std::vector<std::string> getAvailableCommands() const;
    
    /**
     * Execute a specific command with arguments
     * 
     * @param command Command name
     * @param arguments Command arguments
     * @return CLI status from command execution
     */
    CLIStatus executeCommand(const std::string& command, const std::vector<std::string>& arguments);
    
    /**
     * Check if a specific command is available
     * 
     * @param command Command name to check
     * @return true if command is available
     */
    bool isCommandAvailable(const std::string& command) const;
    
    /**
     * Execute main CLI functionality
     * 
     * @return CLI status
     */
    CLIStatus execute();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm