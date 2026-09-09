#pragma once
/**
 * @file Args.hpp
 * @brief Command-line argument parsing and help system for NLM
 * 
 * This file provides a comprehensive argument parsing system for the NLM neural
 * simulation library. It supports modern C++ features, provides detailed help
 * generation, and integrates seamlessly with the existing configuration system.
 * 
 * Features:
 * - Standardized command-line argument parsing
 * - Support for short (-h) and long (--help) options
 * - Positional arguments for execution modes
 * - Optional arguments with defaults
 * - Comprehensive help system with usage documentation
 * - Version information and contact details
 * - Execution mode detection
 * 
 * @author NLM Development Team
 * @version 1.0
 * @date 2026
 */

#include <string>
#include <vector>
#include <map>
#include <set>
#include <optional>
#include <variant>
#include <functional>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <stdexcept>

namespace nlm {

/**
 * @enum ExecutionMode
 * @brief Execution modes for NLM
 * 
 * Defines the available execution modes for the neural simulation system:
 * - Standard: Standard neural computation and simulation
 * - Visualization: Interactive visualization mode
 * - Experiment: Run integration experiments
 * - Benchmark: Performance benchmark mode
 * - Test: Run unit tests
 */
enum class ExecutionMode {
    /** Standard neural computation and simulation */
    Standard,
    /** Interactive visualization mode */
    Visualization,
    /** Run integration experiments */
    Experiment,
    /** Performance benchmark mode */
    Benchmark,
    /** Run unit tests */
    Test
};

/**
 * @brief Convert execution mode to string representation
 * 
 * @param mode Execution mode
 * @return std::string String representation of the mode
 */
std::string executionModeToString(ExecutionMode mode);

/**
 * @brief Convert string to execution mode
 * 
 * @param mode String representation of mode
 * @return ExecutionMode Parsed execution mode
 * @throw std::invalid_argument if mode is invalid
 */
ExecutionMode stringToExecutionMode(const std::string& mode);

/**
 * @struct ArgumentInfo
 * @brief Information about a command-line argument
 * 
 * Stores metadata about command-line arguments for help generation and validation.
 * 
 * @param name Long option name (e.g., "help")
 * @param shortName Short option name (e.g., "h") or empty string
 * @param description Description of what the option does
 * @param type Type of argument (Value, Flag, or Mode)
 * @param defaultValue Default value if provided
 * @param category Category for organizing options in help
 * @param required Whether the argument is required
 * @param helpFormatter Custom help formatter function
 */
struct ArgumentInfo {
    std::string name;
    std::string shortName;
    std::string description;
    std::variant<std::string, bool, ExecutionMode> type;
    std::optional<std::variant<std::string, bool, ExecutionMode>> defaultValue;
    std::string category;
    bool required = false;
    std::function<std::string(const std::variant<std::string, bool, ExecutionMode>&)> helpFormatter;
};

/**
 * @class ArgumentParser
 * @brief Modern command-line argument parser with comprehensive help system
 * 
 * Provides standardized command-line argument parsing for NLM applications.
 * Supports both short (-h) and long (--help) options, positional arguments,
 * optional arguments with defaults, and comprehensive help generation.
 * 
 * Features:
 * - Option registration with validation
 * - Positional argument handling
 * - Comprehensive help generation with categories
 * - Version and help information
 * - Integration with execution modes
 * 
 * @note Thread-safe for concurrent use
 */
class ArgumentParser {
public:
    /**
     * @brief Constructs an argument parser with default options
     * 
     * Initializes the parser with standard NLM options (help, version, config,
     * output, mode, verbose, quiet) and allows for custom option registration.
     */
    ArgumentParser();
    
    /**
     * @brief Destructor
     */
    ~ArgumentParser() = default;
    
    /**
     * @brief Copy constructor (deleted)
     */
    ArgumentParser(const ArgumentParser&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     */
    ArgumentParser& operator=(const ArgumentParser&) = delete;
    
    /**
     * @brief Move constructor (noexcept)
     * 
     * @param other Argument parser to move from
     */
    ArgumentParser(ArgumentParser&&) noexcept = default;
    
    /**
     * @brief Move assignment operator (noexcept)
     * 
     * @param other Argument parser to move from
     * @return Reference to this object
     */
    ArgumentParser& operator=(ArgumentParser&&) noexcept = default;
    
    /**
     * @brief Add a command-line option
     * 
     * Registers a new command-line option with the specified properties.
     * 
     * @param name Long option name (e.g., "help")
     * @param shortName Short option name (e.g., "h") or empty string
     * @param description Description of what the option does
     * @param category Category for organizing options in help (default: "Common")
     * @return ArgumentParser& Reference to this parser for method chaining
     */
    ArgumentParser& addOption(const std::string& name, 
                            const std::string& shortName = "",
                            const std::string& description = "",
                            const std::string& category = "Common");
    
    /**
     * @brief Add a flag option (boolean)
     * 
     * Registers a boolean flag option that can be enabled or disabled.
     * 
     * @param name Long option name (e.g., "verbose")
     * @param shortName Short option name (e.g., "v") or empty string
     * @param description Description of what the flag does
     * @param defaultValue Default value for the flag
     * @param category Category for organizing options in help
     * @return ArgumentParser& Reference to this parser for method chaining
     */
    ArgumentParser& addFlag(const std::string& name,
                          const std::string& shortName = "",
                          const std::string& description = "",
                          bool defaultValue = false,
                          const std::string& category = "Common");
    
    /**
     * @brief Add a value option
     * 
     * Registers an option that requires a value (e.g., --config file.cfg).
     * 
     * @param name Long option name (e.g., "config")
     * @param shortName Short option name (e.g., "c") or empty string
     * @param description Description of what the option does
     * @param defaultValue Default value if provided
     * @param category Category for organizing options in help
     * @param required Whether the option is required
     * @return ArgumentParser& Reference to this parser for method chaining
     */
    ArgumentParser& addValueOption(const std::string& name,
                                 const std::string& shortName = "",
                                 const std::string& description = "",
                                 const std::optional<std::string>& defaultValue = std::nullopt,
                                 const std::string& category = "Common",
                                 bool required = false);
    
    /**
     * @brief Add an execution mode option
     * 
     * Registers the execution mode argument (positional argument).
     * 
     * @param description Description of the mode argument
     * @return ArgumentParser& Reference to this parser for method chaining
     */
    ArgumentParser& addModeOption(const std::string& description = "Execution mode");
    
    /**
     * @brief Parse command-line arguments
     * 
     * Parses the command-line arguments using the registered options.
     * Validates arguments and stores parsed values.
     * 
     * @param argc Number of command-line arguments
     * @param argv Command-line argument array
     * @return bool true if parsing succeeded, false otherwise
     */
    bool parse(int argc, char** argv);
    
    /**
     * @brief Check if help was requested
     * 
     * @return bool true if --help or -h was specified
     */
    bool wasHelpRequested() const;
    
    /**
     * @brief Check if version was requested
     * 
     * @return bool true if --version or -v was specified
     */
    bool wasVersionRequested() const;
    
    /**
     * @brief Get the execution mode
     * 
     * @return ExecutionMode The parsed execution mode
     * @throw std::runtime_error if mode was not specified
     */
    ExecutionMode getMode() const;
    
    /**
     * @brief Get a flag value
     * 
     * @param name Long option name
     * @return bool Value of the flag
     * @throw std::runtime_error if option was not found
     */
    bool getFlag(const std::string& name) const;
    
    /**
     * @brief Get a value option
     * 
     * @param name Long option name
     * @return std::string Value of the option
     * @throw std::runtime_error if option was not found
     */
    std::string getValue(const std::string& name) const;
    
    /**
     * @brief Get a value option (string version)
     * 
     * @param name Long option name
     * @return std::string Value of the option
     * @throw std::runtime_error if option was not found
     */
    std::string getString(const std::string& name) const;
    
    /**
     * @brief Get a value option (optional)
     * 
     * @param name Long option name
     * @return std::optional<std::string> Value of the option if set
     */
    std::optional<std::string> getOptionalString(const std::string& name) const;
    
    /**
     * @brief Generate help information
     * 
     * @return std::string Formatted help information
     */
    std::string generateHelp() const;
    
    /**
     * @brief Generate version information
     * 
     * @return std::string Formatted version information
     */
    static std::string generateVersion();
    
    /**
     * @brief Generate contact information
     * 
     * @return std::string Formatted contact information
     */
    static std::string generateContactInfo();
    
    /**
     * @brief Get all parsed arguments
     * 
     * @return std::map<std::string, std::variant<std::string, bool, ExecutionMode>> Map of all parsed arguments
     */
    std::map<std::string, std::variant<std::string, bool, ExecutionMode>> getAllArgs() const;

private:
    std::vector<std::string> positionalArgs_;
    std::map<std::string, std::variant<std::string, bool, ExecutionMode>> parsedArgs_;
    std::map<std::string, ArgumentInfo> optionInfo_;
    bool helpRequested_ = false;
    bool versionRequested_ = false;
    std::string modeDescription_;
    std::vector<std::pair<std::string, std::string>> shortToLong_;
    
    /**
     * @brief Normalize option name to lowercase
     * 
     * @param name Option name to normalize
     * @return std::string Lowercase version of the name
     */
    static std::string normalize(const std::string& name);
    
    /**
     * @brief Check if an option exists
     * 
     * @param name Option name to check
     * @return bool true if option exists
     */
    bool hasOption(const std::string& name) const;
    
    /**
     * @brief Parse a single argument
     * 
     * @param arg Argument to parse
     * @param argc Total argument count
     * @param argv Argument array
     * @param index Current argument index
     * @return size_t Number of arguments consumed
     */
    size_t parseSingleArg(const std::string& arg, int argc, char** argv, size_t index);
    
    /**
     * @brief Show help and exit
     */
    void showHelpAndExit() const;
    
    /**
     * @brief Show version and exit
     */
    void showVersionAndExit() const;
};

} // namespace nlm