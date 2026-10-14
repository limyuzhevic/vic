#pragma once

/**
 * @file CommandLineInterface.hpp
 * @brief Enhanced command-line interface for NLM
 * @author NLM Project
 * @date Phase 4
 * 
 * This file defines the enhanced command-line interface for the NLM
 * computational neuroscience platform, providing a user-friendly
 * and extensible way to configure and run neural network simulations.
 * 
 * Key features:
 * - Comprehensive argument parsing with help system
 * - Configuration validation and type conversion
 * - Multiple operation modes (demo, experiment, debug, etc.)
 * - Enhanced output formatting and progress reporting
 * - Error handling with detailed usage information
 * - Plugin-like configuration management
 * - Compatibility with existing config file system
 * 
 * The CLI follows modern command-line design principles:
 * - Clear, intuitive flag names
 * - Consistent option formatting
 * - Comprehensive help and version information
 * - Graceful error handling
 * - Support for both short and long options
 */

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <memory>
#include <functional>
#include <stdexcept>
#include <map>
#include <set>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

/**
 * @brief Command-line option type
 */
enum class CLIType {
    Flag,           ///< Boolean flag (--enable-xyz)
    Value,          ///< Option with value (--xyz=value)
    Argument,       ///< Positional argument
    Count          ///< Count flag (-v, -vv)
};

/**
 * @brief Command-line option definition
 */
struct CLIConfig {
    std::string name;                    ///< Option name (--help)
    std::string shortName;               ///< Short option (-h)
    CLIType type;                        ///< Option type
    std::string description;             ///< Description for help
    std::string defaultValue;            ///< Default value if not specified
    std::string category;                ///< Option category for help organization
    bool required;                       ///< Whether option is required
    std::vector<std::string> valueTypes;  ///< Expected value types
    std::function<bool(const std::string&)> validator; ///< Custom validator
    
    CLIConfig(const std::string& n, const std::string& sn, CLIType t,
              const std::string& desc, const std::string& def = "",
              const std::string& cat = "General",
              bool req = false)
        : name(n), shortName(sn), type(t), description(desc),
          defaultValue(def), category(cat), required(req) {}
};

/**
 * @brief Command-line argument parser
 */
class CLIParser {
public:
    CLIParser();
    ~CLIParser() = default;
    
    /**
     * @brief Add a command-line option
     * 
     * @param config Option configuration
     */
    void addOption(const CLIConfig& config);
    
    /**
     * @brief Add positional argument
     * 
     * @param name Argument name
     * @param description Argument description
     * @param required Whether argument is required
     */
    void addArgument(const std::string& name, const std::string& description,
                    bool required = false);
    
    /**
     * @brief Add help option
     * 
     * @param helpEnabled Reference to flag that will be set if help requested
     */
    void addHelpOption(bool& helpEnabled);
    
    /**
     * @brief Add version option
     * 
     * @param versionInfo Version information
     */
    void addVersionOption(const std::string& versionInfo);
    
    /**
     * @brief Parse command-line arguments
     * 
     * @param argc Argument count
     * @param argv Argument values
     * @return true if parsing succeeded
     */
    bool parse(int argc, char** argv);
    
    /**
     * @brief Get option value
     * 
     * @tparam T Value type
     * @param name Option name
     * @param defaultValue Default value if option not found
     * @return Option value
     */
    template<typename T>
    T getValue(const std::string& name, const T& defaultValue = T()) const;
    
    /**
     * @brief Get option value as string
     * 
     * @param name Option name
     * @param defaultValue Default value if option not found
     * @return Option value
     */
    std::string getStringValue(const std::string& name,
                             const std::string& defaultValue = "") const;
    
    /**
     * @brief Get option value as boolean
     * 
     * @param name Option name
     * @param defaultValue Default value if option not found
     * @return Option value
     */
    bool getBoolValue(const std::string& name, bool defaultValue = false) const;
    
    /**
     * @brief Get option value as integer
     * 
     * @param name Option name
     * @param defaultValue Default value if option not found
     * @return Option value
     */
    int getIntValue(const std::string& name, int defaultValue = 0) const;
    
    /**
     * @brief Get option value as double
     * 
     * @param name Option name
     * @param defaultValue Default value if option not found
     * @return Option value
     */
    double getDoubleValue(const std::string& name, double defaultValue = 0.0) const;
    
    /**
     * @brief Get positional argument
     * 
     * @tparam T Value type
     * @param index Argument index
     * @param defaultValue Default value if argument not found
     * @return Argument value
     */
    template<typename T>
    T getArgument(size_t index, const T& defaultValue = T()) const;
    
    /**
     * @brief Get positional argument as string
     * 
     * @param index Argument index
     * @param defaultValue Default value if argument not found
     * @return Argument value
     */
    std::string getArgumentString(size_t index,
                                 const std::string& defaultValue = "") const;
    
    /**
     * @brief Check if option is present
     * 
     * @param name Option name
     * @return true if option is present
     */
    bool hasOption(const std::string& name) const;
    
    /**
     * @brief Check if help was requested
     * 
     * @return true if --help was specified
     */
    bool wasHelpRequested() const;
    
    /**
     * @brief Check if version was requested
     * 
     * @return true if --version was specified
     */
    bool wasVersionRequested() const;
    
    /**
     * @brief Get parsing error message
     * 
     * @return Error message if parsing failed
     */
    std::string getErrorMessage() const;
    
    /**
     * @brief Print help information
     */
    void printHelp() const;
    
    /**
     * @brief Print version information
     */
    void printVersion() const;
    
    /**
     * @brief Convert parsed arguments to config
     * 
     * @param config Reference to config object to update
     * @param configSource Source identifier for config values
     */
    void applyToConfig(Config& config, ConfigSource configSource = ConfigSource::CommandLine) const;
    
    /**
     * @brief Validate all options
     * 
     * @return true if validation passed
     */
    bool validateAll() const;
    
    /**
     * @brief Get validation errors
     * 
     * @return Vector of validation error messages
     */
    std::vector<std::string> getValidationErrors() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Enhanced command-line interface for NLM applications
 * 
 * This class provides a comprehensive command-line interface for NLM applications,
 * including argument parsing, configuration management, and output formatting.
 */
class NLMCommandLine {
public:
    NLMCommandLine(const std::string& version, const std::string& description);
    ~NLMCommandLine() = default;
    
    /**
     * @brief Add a standard configuration option group
     * 
     * @param parser CLI parser reference
     */
    static void addConfigOptions(CLIParser& parser);
    
    /**
     * @brief Add a standard experiment option group
     * 
     * @param parser CLI parser reference
     */
    static void addExperimentOptions(CLIParser& parser);
    
    /**
     * @brief Add a standard logging option group
     * 
     * @param parser CLI parser reference
     */
    static void addLoggingOptions(CLIParser& parser);
    
    /**
     * @brief Add a standard development option group
     * 
     * @param parser CLI parser reference
     */
    static void addDevelopmentOptions(CLIParser& parser);
    
    /**
     * @brief Add a standard output option group
     * 
     * @param parser CLI parser reference
     */
    static void addOutputOptions(CLIParser& parser);
    
    /**
     * @brief Run NLM application with command-line interface
     * 
     * @param argc Argument count
     * @param argv Argument values
     * @param version Application version
     * @param description Application description
     * @return Exit code (0 for success, non-zero for failure)
     */
    static int run(int argc, char** argv, const std::string& version,
                   const std::string& description);
    
    /**
     * @brief Run NLM application with custom configuration
     * 
     * @param argc Argument count
     * @param argv Argument values
     * @param config Reference to config object
     * @param version Application version
     * @param description Application description
     * @return Exit code (0 for success, non-zero for failure)
     */
    static int run(int argc, char** argv, Config& config,
                   const std::string& version, const std::string& description);
    
    /**
     * @brief Print formatted application information
     */
    static void printInfo(const std::string& version,
                         const std::string& description);
    
    /**
     * @brief Print formatted configuration summary
     * 
     * @param config Configuration to print
     */
    static void printConfigSummary(const Config& config);
    
    /**
     * @brief Print progress information
     */
    static void printProgress(const std::string& message, int current, int total);
    
    /**
     * @brief Print error with usage information
     */
    static void printError(const std::string& error);
    
    /**
     * @brief Print success message
     */
    static void printSuccess(const std::string& message);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Common CLI option categories
 */
namespace CLICategories {
    const std::string General = "General";
    const std::string Configuration = "Configuration";
    const std::string Experiment = "Experiment";
    const std::string Logging = "Logging";
    const std::string Development = "Development";
    const std::string Output = "Output";
    const std::string Debug = "Debug";
    const std::string Performance = "Performance";
}

/**
 * @brief Common CLI option names
 */
namespace CLIOptions {
    // General options
    const std::string Help = "help";
    const std::string Version = "version";
    const std::string Verbose = "verbose";
    const std::string Quiet = "quiet";
    const std::string Config = "config";
    const std::string Output = "output";
    
    // Experiment options
    const std::string Mode = "mode";
    const std::string Steps = "steps";
    const std::string Neurons = "neurons";
    const std::string Regions = "regions";
    const std::string Connections = "connections";
    const std::string LearningRate = "learning-rate";
    const std::string Plasticity = "plasticity";
    
    // Logging options
    const std::string LogLevel = "log-level";
    const std::string LogFile = "log-file";
    const std::string NoColors = "no-colors";
    
    // Development options
    const std::string Debug = "debug";
    const std::string Profile = "profile";
    const std::string Validate = "validate";
    const std::string Checkpoint = "checkpoint";
    const std::string Replay = "replay";
    
    // Output options
    const std::string Format = "format";
    const std::string Detailed = "detailed";
    const std::string Statistics = "statistics";
    const std::string Visualization = "visualization";
}

/**
 * @brief Common CLI modes
 */
namespace CLIModes {
    const std::string QuickDemo = "quick-demo";
    const std::string FullDemo = "full-demo";
    const std::string Experiment = "experiment";
    const std::string Debug = "debug";
    const std::string Benchmark = "benchmark";
    const std::string Validate = "validate";
}

/**
 * @brief Common log levels
 */
namespace CLILogLevels {
    const std::string Debug = "debug";
    const std::string Info = "info";
    const std::string Warning = "warning";
    const std::string Error = "error";
    const std::string Critical = "critical";
}

/**
 * @brief Common output formats
 */
namespace CLIFormats {
    const std::string Text = "text";
    const std::string JSON = "json";
    const std::string CSV = "csv";
    const std::string YAML = "yaml";
}

} // namespace nlm

// Explicit instantiations
namespace nlm {
    template int CLIParser::getValue<int>(const std::string&, const int&) const;
    template double CLIParser::getValue<double>(const std::string&, const double&) const;
    template std::string CLIParser::getValue<std::string>(const std::string&, const std::string&) const;
    template bool CLIParser::getValue<bool>(const std::string&, const bool&) const;
    
    template std::string CLIParser::getArgument<std::string>(size_t, const std::string&) const;
}