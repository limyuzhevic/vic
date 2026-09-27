// Phase 6 Command-Line Interface Enhancements
// Provides enhanced command-line interface for Phase 6 systems

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <memory>
#include <algorithm>
#include <sstream>

namespace nlm {

/**
 * Command-line option definition
 */
struct CommandLineOption {
    std::string name;
    char shortOpt;
    std::string longOpt;
    std::string description;
    std::string defaultValue;
    std::string type;  // "string", "int", "float", "bool", "choice"
    std::string category;  // "neural", "memory", "plasticity", "neuromodulation", "simulation", "checkpoint", "visualization"
    std::string constraint;  // Validation regex or range
    std::vector<std::string> choices;  // For choice type
    bool required;
    bool (*validator)(const std::string&);  // Custom validator
};

/**
 * Phase 6 command-line interface manager
 */
class Phase6CLI {
public:
    Phase6CLI();
    ~Phase6CLI();
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Get option value
    template<typename T>
    std::optional<T> getOption(const std::string& name) const;
    
    // Check if option was provided
    bool hasOption(const std::string& name) const;
    
    // Get all options
    std::vector<std::string> getOptionNames() const;
    
    // Show help
    void showHelp() const;
    
    // Show version
    void showVersion() const;
    
    // Get configuration file path
    std::string getConfigFile() const { return configFile_; }
    
    // Get verbosity level
    int getVerbosity() const { return verbosity_; }
    
    // Get experiment configuration
    std::string getExperimentConfig() const { return experimentConfig_; }
    
    // Get checkpoint configuration
    std::string getCheckpointConfig() const { return checkpointConfig_; }
    
    // Get simulation configuration
    std::string getSimulationConfig() const { return simulationConfig_; }
    
    // Get visualization configuration
    std::string getVisualizationConfig() const { return visualizationConfig_; }
    
    // Apply options to configuration
    bool applyToConfig(std::shared_ptr<Config> config) const;
    
    // Validate all options
    bool validate() const;
    
    // Get validation errors
    std::vector<std::string> getValidationErrors() const { return validationErrors_; }
    
    // Set verbosity level
    void setVerbosity(int level) { verbosity_ = level; }
    
private:
    // Define Phase 6 command-line options
    void initializeOptions();
    
    // Parse option string (e.g., "--neuron-count=1000")
    bool parseOption(const std::string& arg, std::string& key, std::string& value);
    
    // Convert option value to appropriate type
    std::string convertValue(const std::string& value, const std::string& type) const;
    
    // Validate option value
    bool validateOption(const std::string& name, const std::string& value) const;
    
    // Check if option exists
    bool optionExists(const std::string& name) const;
    
    // Get option definition
    CommandLineOption getOptionDefinition(const std::string& name) const;
    
    // Show option help
    void showOptionHelp(const std::string& name) const;
    
    // Apply single option to configuration
    bool applyOptionToConfig(const std::string& name, const std::string& value, 
                           std::shared_ptr<Config> config) const;
    
    // Map option names to config keys
    static std::map<std::string, std::string> getOptionToConfigKeyMap();
    
    // Registered options
    std::vector<CommandLineOption> options_;
    
    // Parsed option values
    std::map<std::string, std::string> parsedOptions_;
    
    // Configuration file
    std::string configFile_;
    
    // Verbosity level
    int verbosity_;
    
    // Experiment configuration
    std::string experimentConfig_;
    
    // Checkpoint configuration
    std::string checkpointConfig_;
    
    // Simulation configuration
    std::string simulationConfig_;
    
    // Visualization configuration
    std::string visualizationConfig_;
    
    // Validation errors
    std::vector<stdشرات� string> validationErrors_;
};

/**
 * Command-line interface for Phase 6 demo
 */
class Phase6DemoCLI : public Phase6CLI {
public:
    Phase6DemoCLI(int argc, char** argv);
    
    // Run demo with parsed options
    bool runDemo() const;
    
    // Get demo configuration
    Phase6Config getDemoConfig() const;
    
    // Show demo status
    void showDemoStatus() const;
    
private:
    // Parse demo-specific options
    void parseDemoOptions();
    
    // Convert string to experiment type
    static std::string experimentTypeToString(int type);
    
    // Convert string to demo mode
    static std::string demoModeToString(int mode);
    
    // Demo configuration
    Phase6Config demoConfig_;
};

/**
 * Command-line interface for experiment runner
 */
class ExperimentRunnerCLI : public Phase6CLI {
public:
    ExperimentRunnerCLI(int argc, char** argv);
    
    // Run experiment with parsed options
    bool runExperiment() const;
    
    // Get experiment configuration
    ExperimentConfig getExperimentConfig() const;
    
    // Show experiment status
    void showExperimentStatus() const;
    
private:
    // Parse experiment-specific options
    void parseExperimentOptions();
    
    // Convert experiment type
    static std::string experimentTypeToString(int type);
    
    // Experiment configuration
    ExperimentConfig experimentConfig_;
};

/**
 * Option validators
 */
namespace OptionValidators {
    
    // Validate integer range
    bool validateInt(const std::string& value, int min, int max);
    
    // Validate float range
    bool validateFloat(const std::string& value, float min, float max);
    
    // Validate probability
    bool validateProbability(const std::string& value);
    
    // Validate file path
    bool validateFilePath(const std::string& value);
    
    // Validate choice
    bool validateChoice(const std::string& value, const std::vector<std::string>& choices);
    
    // Validate positive integer
    bool validatePositiveInt(const std::string& value);
    
    // Validate positive float
    bool validatePositiveFloat(const std::string& value);
    
    // Validate string length
    bool validateStringLength(const std::string& value, size_t minLength, size_t maxLength);
    
    // Validate JSON-like structure
    bool validateJSON(const std::string& value);
    
    // Validate comma-separated values
    bool validateCSV(const std::string& value);
}

/**
 * Help text generator
 */
class HelpGenerator {
public:
    static std::string generateOptionHelp(const CommandLineOption& option);
    static std::string generateSectionHelp(const std::string& title, const std::vector<CommandLineOption>& options);
    static std::string generateAllHelp(const std::vector<CommandLineOption>& options);
    static std::string generateUsage(const std::string& programName);
    static std::string generateExample(const std::string& programName);
};

/**
 * Configuration file utilities
 */
class ConfigFileUtils {
public:
    // Generate example configuration file
    static std::string generateExampleConfig();
    
    // Validate configuration file
    static std::vector<std::string> validateConfigFile(const std::string& filepath);
    
    // Merge configuration files
    static bool mergeConfigFiles(const std::string& baseFile, const std::string& overrideFile, 
                                const std::string& outputFile);
    
    // Parse environment variables from configuration file
    static std::map<std::string, std::string> parseEnvironmentVariables(const std::string& configContent);
};

/**
 * Command-line parser utilities
 */
class CLIUtils {
public:
    // Parse comma-separated values
    static std::vector<std::string> parseCSV(const std::string& value);
    
    // Parse key-value pairs
    static std::map<std::string, std::string> parseKeyValuePairs(const std::string& value);
    
    // Parse boolean value
    static bool parseBool(const std::string& value);
    
    // Parse optional value
    static std::string trim(const std::string& value);
    
    // Check if string starts with prefix
    static bool startsWith(const std::string& str, const std::string& prefix);
    
    // Check if string ends with suffix
    static bool endsWith(const std::string& str, const std::string& suffix);
    
    // Convert string to lowercase
    static std::string toLower(const std::string& str);
    
    // Convert string to uppercase
    static std::string toUpper(const std::string& str);
    
    // Replace all occurrences
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    
    // Split string by delimiter
    static std::vector<std::string> split(const std::string& str, char delimiter);
};

} // namespace nlm
