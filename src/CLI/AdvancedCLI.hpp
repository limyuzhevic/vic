#pragma once

#include "core/Config/Config.hpp"
#include "core/Exception/NLMException.hpp"
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <functional>
#include <optional>

namespace nlm {

/**
 * @class AdvancedCLI
 * @brief Enhanced command-line interface for NLM
 * 
 * This class provides advanced command-line interface capabilities for the
 * NLM framework, including:
 * - Comprehensive argument parsing with type validation
 * - Configuration file management
 * - Interactive help and documentation
 * - Execution mode selection
 * - Performance profiling and monitoring
 * - Experimental parameter sets
 * 
 * The CLI supports various execution modes:
 * - Standard simulation (phase 2)
 * - Phase 3: World interaction
 * - Phase 4: Prediction systems
 * - Phase 6: Full integration
 * - Benchmark and profiling
 * - Validation and testing
 * 
 * Configuration can be loaded from files, command line arguments,
 * or interactively, with clear precedence rules (command line > file > defaults).
 */
class AdvancedCLI {
public:
    /**
     * @brief Construct a new AdvancedCLI object
     * 
     * Initializes the CLI system with default configuration and
     * available command sets.
     */
    AdvancedCLI();
    
    /**
     * @brief Destroy the AdvancedCLI object
     * 
     * Cleans up CLI resources and saves any session data.
     */
    ~AdvancedCLI();
    
    // Disable copying
    AdvancedCLI(const AdvancedCLI&) = delete;
    AdvancedCLI& operator=(const AdvancedCLI&) = delete;
    
    /**
     * @brief Parse command line arguments
     * 
     * Parses the provided command line arguments and configures the NLM
     * system accordingly. Supports both short and long form arguments
     * with comprehensive validation.
     * 
     * @param argc Number of command line arguments
     * @param argv Command line argument array
     * 
     * @return true if parsing succeeded and system is configured for execution,
     *         false if there were errors (system will exit with error code)
     * 
     * @throws NLMException if there are critical errors in parsing
     */
    bool parseArguments(int argc, char** argv);
    
    /**
     * @brief Get the parsed configuration
     * 
     * @return std::shared_ptr<Config> Shared pointer to the parsed configuration
     */
    std::shared_ptr<const Config> getConfiguration() const;
    
    /**
     * @brief Get the execution mode
     * 
     * @return std::string Execution mode (e.g., "standard", "benchmark", "validate")
     */
    std::string getExecutionMode() const;
    
    /**
     * @brief Check if help was requested
     * 
     * @return true if help was requested via --help or -h
     */
    bool isHelpRequested() const;
    
    /**
     * @brief Check if version was requested
     * 
     * @return true if version was requested via --version
     */
    bool isVersionRequested() const;
    
    /**
     * @brief Get the configuration file path
     * 
     * @return std::string Path to the configuration file, or empty string if not specified
     */
    std::string getConfigFilePath() const;
    
    /**
     * @brief Get help text for a specific command
     * 
     * @param command Command to get help for
     * 
     * @return std::string Help text for the command
     */
    std::string getCommandHelp(const std::string& command) const;
    
    /**
     * @brief Get all available commands
     * 
     * @return std::vector<std::string> List of all available command names
     */
    std::vector<std::string> getAvailableCommands() const;
    
    /**
     * @brief Print help information
     * 
     * @param command Optional specific command to get help for, or empty for general help
     */
    void printHelp(const std::string& command = "") const;
    
    /**
     * @brief Print version information
     * 
     * Prints the NLM version and build information.
     */
    void printVersion() const;
    
    /**
     * @brief Check if arguments are valid
     * 
     * Performs validation of the parsed arguments to ensure they are
     * consistent and valid for the selected execution mode.
     * 
     * @return true if arguments are valid, false otherwise
     */
    bool validateArguments() const;
    
    /**
     * @brief Get execution summary
     * 
     * @return std::string Human-readable summary of the execution configuration
     */
    std::string getExecutionSummary() const;

private:
    /**
     * @brief Initialize available commands and their handlers
     */
    void initializeCommands();
    
    /**
     * @brief Parse and validate a single argument
     * 
     * @param arg Argument string to parse
     * @param value Optional value for the argument
     * 
     * @return bool true if argument was successfully parsed and validated
     */
    bool parseArgument(const std::string& arg, const std::string& value);
    
    /**
     * @brief Handle special arguments (--help, --version, etc.)
     * 
     * @param arg Argument to handle
     * @param value Optional value
     * 
     * @return bool true if argument was handled as special
     */
    bool handleSpecialArguments(const std::string& arg, const std::string& value);
    
    /**
     * @brief Add a configuration option
     * 
     * @param key Configuration key
     * @param value Configuration value
     * @param source Configuration source
     * @param description Description of the configuration option
     */
    void addConfigOption(const std::string& key, const std::string& value, ConfigSource source, const std::string& description);
    
    /**
     * @brief Process configuration overrides
     * 
     * @param config Configuration to process overrides for
     */
    void processConfigurationOverrides(std::shared_ptr<Config> config);
    
    /**
     * @brief Get experimental parameter set
     * 
     * @param name Name of experimental parameter set
     * 
     * @return std::unordered_map<std::string, std::string> Parameter set
     */
    std::unordered_map<std::string, std::string> getExperimentalParameters(const std::string& name) const;
    
    /**
     * @brief Validate configuration consistency
     * 
     * @param config Configuration to validate
     * 
     * @return bool true if configuration is consistent
     */
    bool validateConfiguration(const Config& config) const;
    
    /**
     * @brief Apply experimental parameters to configuration
     * 
     * @param config Configuration to apply parameters to
     * @param params Experimental parameters
     */
    void applyExperimentalParameters(std::shared_ptr<Config> config, const std::unordered_map<std::string, std::string>& params);
    
    /**
     * @brief Get default configuration values
     * 
     * @return std::unordered_map<std::string, std::string> Default configuration
     */
    std::unordered_map<std::string, std::string> getDefaultConfiguration() const;
    
    /**
     * @brief Check if value is a boolean
     * 
     * @param value String value to check
     * @return bool true if value represents a boolean
     */
    bool isBooleanValue(const std::string& value) const;
    
    /**
     * @brief Convert string to boolean
     * 
     * @param value String value to convert
     * @return bool Converted boolean value
     */
    bool stringToBool(const std::string& value) const;
    
    /**
     * @brief Check if value is a number
     * 
     * @param value String value to check
     * @return bool true if value represents a number
     */
    bool isNumericValue(const std::string& value) const;
    
    /**
     * @brief Convert string to double
     * 
     * @param value String value to convert
     * @return double Converted numeric value
     */
    double stringToDouble(const std::string& value) const;
    
    /**
     * @brief Get error message for invalid arguments
     * 
     * @param arg Invalid argument
     * @param value Invalid value
     * 
     * @return std::string Error message
     */
    std::string getErrorMessage(const std::string& arg, const std::string& value) const;
    
    /**
     * @brief Log parsing progress
     * 
     * @param level Log level
     * @param message Message to log
     */
    void logProgress(const std::string& level, const std::string& message) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Parse command line arguments for NLM
 * 
 * Convenience function that creates an AdvancedCLI instance, parses arguments,
 * and returns the configuration. This is the recommended way to use the advanced
 * CLI functionality.
 * 
 * @param argc Number of command line arguments
 * @param argv Command line argument array
 * 
 * @return std::shared_ptr<const Config> Parsed configuration
 * 
 * @throws NLMException if there are critical errors in parsing
 */
std::shared_ptr<const Config> parseNLMArguments(int argc, char** argv);

/**
 * @brief Execute NLM with advanced CLI configuration
 * 
 * This function creates an AdvancedCLI instance, parses arguments, configures
 * the NLM system, and runs the simulation. This is the main entry point for
 * advanced CLI usage.
 * 
 * @param argc Number of command line arguments
 * @param argv Command line argument array
 * 
 * @return int Exit code (0 for success, non-zero for errors)
 */
int executeNLMWithAdvancedCLI(int argc, char** argv);

} // namespace nlm