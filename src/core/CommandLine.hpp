#pragma once

/**
 * @file CommandLine.hpp
 * @brief Unified command-line argument parsing utilities for NLM
 * @author NLM Development Team
 * @date 2026
 * @version 0.1.0
 *
 * This file provides a unified interface for command-line argument parsing
 * across all NLM demo executables and tools. It offers:
 *
 * - Consistent argument parsing across all demos
 * - Help and usage documentation generation
 * - Configuration option validation
 * - Flexible argument formats (--option=value or --option value)
 * - Common command-line options (--help, --version, --config)
 *
 * The API is designed to be simple yet powerful, supporting both basic
 * command-line interfaces and advanced usage scenarios.
 *
 * @note This replaces the ad-hoc argument parsing in individual demo files
 * with a standardized approach.
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <optional>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

/**
 * @class CommandLine
 * @brief Unified command-line argument parsing for NLM applications
 * 
 * This class provides a comprehensive interface for parsing command-line arguments
 * with support for:
 * - Short and long option names
 * - Flexible argument formats (--option=value or --option value)
 * - Value type conversion (int, double, string, bool)
 * - Help and usage documentation
 * - Configuration file handling
 * - Validation and error handling
 * 
 * The parser is designed to be intuitive and Pythonic in its API while
 * providing the power and flexibility needed for complex applications.
 * 
 * Example usage:
 * @code
 * CommandLine cli(argc, argv);
 * cli.addOption("--help", "Print help information", false);
 * cli.addOption("--config", "Configuration file path", false);
 * cli.addOption("--neuron-count", "Number of neurons", false, 1000);
 * 
 * if (cli.get<bool>("--help")) {
 *     cli.printHelp();
 *     return 0;
 * }
 * 
 * std::string configFile = cli.getOr("--config", "configs/default.cfg");
 * int neuronCount = cli.getOr("--neuron-count", 1000);
 * @endcode
 */
class CommandLine {
public:
    /**
     * @brief Constructor
     * 
     * @param argc Argument count (from main())
     * @param argv Argument vector (from main())
     
     * @note Parses arguments in the constructor, so all validation
     * and processing happens during initialization.
     */
    CommandLine(int argc, char** argv);
    
    /**
     * @brief Destructor
     */
    ~CommandLine() = default;
    
    /**
     * @brief Add a command-line option
     * 
     * @param name Option name (e.g., "--help", "--neuron-count")
     * @param description Human-readable description of the option
     * @param hasValue Whether the option requires a value
     * @param defaultValue Default value if option not provided
     * 
     * @note Options can be added before or after parsing (adding after
     * parsing will affect future parsing operations on the same instance).
     */
    void addOption(const std::string& name, const std::string& description, 
                   bool hasValue = false, const std::string& defaultValue = "");
    
    /**
     * @brief Add a boolean option (no value, just presence)
     * 
     * @param name Option name (e.g., "--verbose", "--help")
     * @param description Human-readable description
     * 
     * @note Boolean options don't take values; their presence indicates true.
     */
    void addBooleanOption(const std::string& name, const std::string& description);
    
    /**
     * @brief Check if an option is present
     * 
     * @param name Option name to check
     * @return true if the option is present in command line
     */
    bool hasOption(const std::string& name) const;
    
    /**
     * @brief Get option value as string
     * 
     * @tparam T Type to convert to (string, int, double, bool)
     * @param name Option name
     * @return Optional containing converted value, empty if not present
     * 
     * @note The value is automatically converted to the requested type.
     * For boolean types, returns true if option is present (regardless of value).
     */
    template<typename T>
    std::optional<T> get(const std::string& name) const;
    
    /**
     * @brief Get option value with default fallback
     * 
     * @tparam T Type to convert to (string, int, double, bool)
     * @param name Option name
     * @param defaultValue Default value if option not present
     * @return Option value if present, otherwise defaultValue
     */
    template<typename T>
    T getOr(const std::string& name, const T& defaultValue) const;
    
    /**
     * @brief Get configuration file path
     * 
     * @param defaultConfig Default configuration file path
     * @return Configuration file path (from --config option or default)
     */
    std::string getConfigFile(const std::string& defaultConfig = "configs/default.cfg") const;
    
    /**
     * @brief Print help information
     * 
     * @param programName Name of the program (defaults to argv[0])
     */
    void printHelp(const std::string& programName = "nlm") const;
    
    /**
     * @brief Print usage information
     * 
     * @param programName Name of the program (defaults to argv[0])
     */
    void printUsage(const std::string& programName = "nlm") const;
    
    /**
     * @brief Get all parsed options
     * 
     * @return Vector of option names that were provided
     */
    std::vector<std::string> getOptionNames() const;
    
    /**
     * @brief Get command line as string (for debugging)
     * 
     * @return String representation of command line arguments
     */
    std::string getCommandLineString() const;
    
    /**
     * @brief Check if help was requested
     * 
     * @return true if --help option was provided
     */
    bool helpRequested() const;
    
    /**
     * @brief Get version information
     * 
     * @return Version string
     */
    static std::string getVersion();
    
    /**
     * @brief Get usage examples
     * 
     * @return Vector of usage examples
     */
    static std::vector<std::string> getUsageExamples();

private:
    struct OptionInfo {
        std::string name;
        std::string description;
        bool hasValue;
        std::string defaultValue;
        std::optional<std::string> providedValue;
        
        OptionInfo(const std::string& n, const std::string& desc, bool hv, 
                   const std::string& def = "")
            : name(n), description(desc), hasValue(hv), defaultValue(def) {}
    };
    
    int argc;
    char** argv;
    std::vector<OptionInfo> options;
    std::unordered_map<std::string, std::string> parsedValues;
    
    /**
     * @brief Parse command line arguments
     */
    void parseArguments();
    
    /**
     * @brief Add standard NLM options
     * 
     * @param hasConfig Whether to add --config option
     * @param hasHelp Whether to add --help option
     * @param hasVersion Whether to add --version option
     */
    void addStandardOptions(bool hasConfig = true, bool hasHelp = true, 
                           bool hasVersion = true);
    
    /**
     * @brief Validate parsed arguments
     */
    void validateArguments() const;
    
    /**
     * @brief Trim whitespace from string
     * 
     * @param str Input string
     * @return Trimmed string
     */
    static std::string trim(const std::string& str);
    
    /**
     * @brief Convert string to lowercase
     * 
     * @param str Input string
     * @return Lowercase version of input
     */
    static std::string toLower(const std::string& str);
    
    /**
     * @brief Convert string to boolean
     * 
     * @param str String to convert ("true", "false", "1", "0", etc.)
     * @return Converted boolean value
     */
    static bool stringToBool(const std::string& str);
    
    /**
     * @brief Check if string represents a boolean value
     * 
     * @param str String to check
     * @return true if string can be interpreted as boolean
     */
    static bool isBoolString(const std::string& str);
    
    /**
     * @brief Check if string represents an integer
     * 
     * @param str String to check
     * @return true if string can be interpreted as integer
     */
    static bool isIntegerString(const std::string& str);
    
    /**
     * @brief Check if string represents a floating-point number
     * 
     * @param str String to check
     * @return true if string can be interpreted as floating-point
     */
    static bool isDoubleString(const std::string& str);
    
    /**
     * @brief Get option value by name
     * 
     * @param name Option name
     * @return Parsed value or empty optional if not found
     */
    std::optional<std::string> getRawValue(const std::string& name) const;
}; 

/**
 * @brief Helper function to parse configuration from file
 * 
 * @param configFile Configuration file path
 * @return true if configuration was successfully loaded
 * 
 * @note This is a convenience function that can be used with CommandLine
 * to load additional configuration from files.
 */
bool loadConfigFromFile(const std::string& configFile);

/**
 * @brief Helper function to save configuration to file
 * 
 * @param configFile Configuration file path
 * @return true if configuration was successfully saved
 * 
 * @note This is a convenience function that can be used with CommandLine
 * to save configuration to files.
 */
bool saveConfigToFile(const std::string& configFile);

/**
 * @brief Parse common brain configuration options
 * 
 * @param cli CommandLine instance
 * @return Standard brain configuration parameters
 * 
 * @note This function extracts common brain configuration options
 * from the command line and returns them as a structured configuration.
 */
struct BrainConfig {
    int64_t neuronCount;
    int64_t regionCount;
    float connectionProbability;
    int64_t randomSeed;
    double simulationTimestep;
    float stdpLtpWeight;
    float stdpLtdWeight;
    float stdpTau;
    float synaptogenesisRate;
    float pruningRate;
    
    BrainConfig() : neuronCount(1000), regionCount(1), connectionProbability(0.15f),
                   randomSeed(42), simulationTimestep(0.001),
                   stdpLtpWeight(0.02f), stdpLtdWeight(0.015f), stdpTau(20.0f),
                   synaptogenesisRate(0.0001f), pruningRate(0.00001f) {}
};

/**
 * @brief Extract brain configuration from command line
 * 
 * @param cli CommandLine instance
 * @return Brain configuration struct
 * 
 * @note This is a convenience function that provides sensible defaults
 * for common brain configuration options.
 */
BrainConfig parseBrainConfig(const CommandLine& cli);

/**
 * @brief Print brain configuration
 * 
 * @param config Brain configuration to print
 */
void printBrainConfig(const BrainConfig& config);

} // namespace nlm

// Template implementations (in header for inline compilation)
template<typename T>
std::optional<T> nlm::CommandLine::get(const std::string& name) const {
    auto rawValue = getRawValue(name);
    if (!rawValue) {
        return std::nullopt;
    }
    
    std::stringstream ss(*rawValue);
    T result;
    if (ss >> result) {
        return result;
    }
    return std::nullopt;
}

template<typename T>
T nlm::CommandLine::getOr(const std::string& name, const T& defaultValue) const {
    auto value = get<T>(name);
    if (value) {
        return *value;
    }
    return defaultValue;
}