// NLM (熙然) - Neural Learning Machine CLI
// Advanced Command-Line Interface Implementation
//
// This module provides:
// - Rich command-line interface with subcommands
// - Configuration management with file support
// - Advanced logging and verbosity control
// - Help system with comprehensive documentation
// - Progress monitoring and status reporting
// - Support for batch operations and scripting

#ifndef NLM_CLI_HPP
#define NLM_CLI_HPP

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <functional>
#include <variant>
#include <cctype>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {
namespace cli {

// Command types for the CLI
enum class CommandType {
    Run,
    Configure,
    Experiment,
    Analyze,
    Benchmark,
    Help,
    Version,
    ListPresets,
    ShowConfig,
    SaveConfig
};

// Helper function to parse command type
inline CommandType parseCommandType(const std::string& commandStr) {
    std::string cmd = commandStr;
    std::transform(cmd.begin(), cmd.end(), cmd.begin(), ::tolower);
    
    if (cmd == "run") return CommandType::Run;
    if (cmd == "configure") return CommandType::Configure;
    if (cmd == "experiment") return CommandType::Experiment;
    if (cmd == "analyze") return CommandType::Analyze;
    if (cmd == "benchmark") return CommandType::Benchmark;
    if (cmd == "help") return CommandType::Help;
    if (cmd == "version") return CommandType::Version;
    if (cmd == "list-presets") return CommandType::ListPresets;
    if (cmd == "show-config") return CommandType::ShowConfig;
    if (cmd == "save-config") return CommandType::SaveConfig;
    
    return CommandType::Run; // Default
}

// Verbosity levels for logging control
enum class VerbosityLevel {
    Quiet = 0,
    Error = 1,
    Warning = 2,
    Info = 3,
    Debug = 4,
    Trace = 5
};

// Command handler function type
using CommandHandler = std::function<bool(const std::map<std::string, std::string>& options)>;

// CLI Parser Class
class CLIParser {
public:
    CLIParser();
    ~CLIParser() = default;
    
    // Parse command line arguments
    bool parse(int argc, char** argv);
    
    // Get parsed command type
    CommandType getCommand() const { return currentCommand; }
    
    // Get parsed options
    const std::map<std::string, std::string>& getOptions() const { return parsedOptions; }
    
    // Get positional arguments
    const std::vector<std::string>& getArgs() const { return positionalArgs; }
    
    // Get verbosity level
    VerbosityLevel getVerbosity() const { return verbosity; }
    
    // Get configuration file
    const std::string& getConfigFile() const { return configFile; }
    
    // Get output file
    const std::string& getOutputFile() const { return outputFile; }
    
    // Get log file
    const std::string& getLogFile() const { return logFile; }
    
    // Get help flag
    bool showHelp() const { return helpRequested; }
    
    // Get version flag
    bool showVersion() const { return versionRequested; }
    
    // Get presets flag
    bool listPresets() const { return listPresetsRequested; }
    
    // Display help information
    void displayHelp() const;
    
    // Display version information
    void displayVersion() const;
    
    // Display available presets
    void displayPresets() const;

private:
    CommandType currentCommand;
    std::map<std::string, std::string> parsedOptions;
    std::vector<std::string> positionalArgs;
    VerbosityLevel verbosity;
    std::string configFile;
    std::string outputFile;
    std::string logFile;
    bool helpRequested;
    bool versionRequested;
    bool listPresetsRequested;
    
    // Parse common options
    bool parseCommonOptions(std::vector<std::string>::iterator& it, const std::vector<std::string>::iterator& end);
    
    // Parse command-specific options
    bool parseCommandOptions(const std::string& command, std::vector<std::string>::iterator& it, const std::vector<std::string>::iterator& end);
    
    // Parse subcommand
    CommandType parseCommand(const std::string& commandStr) const;
    
    // Parse verbosity level
    VerbosityLevel parseVerbosity(const std::string& levelStr) const;
};

// Configuration Manager for CLI
class ConfigManager {
public:
    ConfigManager();
    ~ConfigManager() = default;
    
    // Load configuration from file
    bool loadFromFile(const std::string& filename);
    
    // Save configuration to file
    bool saveToFile(const std::string& filename) const;
    
    // Apply command line overrides
    bool applyOverrides(const std::map<std::string, std::string>& options);
    
    // Validate configuration
    bool validate() const;
    
    // Get the configuration
    std::shared_ptr<Config> getConfig() const { return config; }
    
    // Get configuration value with type conversion
    template<typename T>
    T getValue(const std::string& key, const T& defaultValue) const {
        return config->getOr<T>(key, defaultValue);
    }
    
    // Set configuration value
    template<typename T>
    void setValue(const std::string& key, const T& value) {
        config->set(key, value, ConfigSource::CommandLine);
    }
    
    // Apply preset configuration
    bool applyPreset(const std::string& presetName);
    
    // Get available presets
    static std::vector<std::string> getAvailablePresets();

private:
    std::shared_ptr<Config> config;
    
    // Built-in presets
    void loadPresets();
    std::map<std::string, std::map<std::string, std::string>> presets;
};

// Command Handler for executing operations
class CommandHandler {
public:
    CommandHandler();
    ~CommandHandler() = default;
    
    // Execute run command
    bool executeRunCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config);
    
    // Execute experiment command
    bool executeExperimentCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config);
    
    // Execute analyze command
    bool executeAnalyzeCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config);
    
    // Execute benchmark command
    bool executeBenchmarkCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config);
    
    // Execute configure command
    bool executeConfigureCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config);
    
    // Execute show-config command
    bool executeShowConfigCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config);
    
    // Execute save-config command
    bool executeSaveConfigCommand(const std::map<std::string, std::string>& options, std::shared_ptr<Config> config);

private:
    // Setup brain with configuration
    bool setupBrain(std::shared_ptr<Config> config);
    
    // Run basic simulation
    bool runSimulation(std::shared_ptr<Config> config);
    
    // Run plasticity experiment
    bool runPlasticityExperiment(std::shared_ptr<Config> config);
    
    // Run STDP verification
    bool runStdpVerification(std::shared_ptr<Config> config);
    
    // Run development experiment
    bool runDevelopmentExperiment(std::shared_ptr<Config> config);
    
    // Run cognitive experiment
    bool runCognitiveExperiment(std::shared_ptr<Config> config);
    
    // Run social experiment
    bool runSocialExperiment(std::shared_ptr<Config> config);
    
    // Run memory experiment
    bool runMemoryExperiment(std::shared_ptr<Config> config);
    
    // Run attention experiment
    bool runAttentionExperiment(std::shared_ptr<Config> config);
    
    // Output results in different formats
    bool outputResults(const std::string& filename, const std::string& format, 
                      const std::string& data) const;
    
    // Format output as JSON
    std::string formatJsonOutput(const std::string& data) const;
    
    // Format output as CSV
    std::string formatCsvOutput(const std::string& data) const;
    
    // Format output as text
    std::string formatTextOutput(const std::string& data) const;
};

// Help System Implementation
class HelpSystem {
public:
    HelpSystem();
    ~HelpSystem() = default;
    
    // Display comprehensive help information
    void displayFullHelp() const;
    
    // Display command-specific help
    void displayCommandHelp(const std::string& command) const;
    
    // Display preset information
    void displayPresetInfo(const std::string& presetName) const;
    
    // Display examples
    void displayExamples() const;
    
    // Display available commands
    void displayAvailableCommands() const;
    
    // Display available options
    void displayAvailableOptions() const;

private:
    // Command descriptions
    const std::map<CommandType, std::string> commandDescriptions;
    
    // Option descriptions
    const std::map<std::string, std::string> optionDescriptions;
    
    // Preset descriptions
    const std::map<std::string, std::string> presetDescriptions;
};

// Logging Configuration for CLI
class LoggerConfig {
public:
    LoggerConfig();
    ~LoggerConfig() = default;
    
    // Configure logging based on verbosity level and options
    void configure(VerbosityLevel verbosity, const std::string& logFile = "",
                   const std::string& outputFile = "");
    
    // Set log level
    void setLogLevel(LogLevel level);
    
    // Add console logger
    void addConsoleLogger(LogLevel level = LogLevel::Info);
    
    // Add file logger
    void addFileLogger(const std::string& filename, LogLevel level = LogLevel::Info);
    
    // Enable or disable logging to file
    void enableFileLogging(bool enable);
    
    // Get global logger
    std::shared_ptr<Logger> getLogger() const { return globalLogger; }

private:
    std::shared_ptr<Logger> globalLogger;
    std::shared_ptr<ConsoleLogger> consoleLogger;
    std::shared_ptr<FileLogger> fileLogger;
    bool fileLoggingEnabled;
};

// Progress Monitor for long-running operations
class ProgressMonitor {
public:
    ProgressMonitor();
    ~ProgressMonitor() = default;
    
    // Start monitoring progress
    void start(const std::string& operation, int totalSteps);
    
    // Update progress
    void update(int currentStep, const std::string& message = "");
    
    // Stop monitoring
    void stop();
    
    // Set update interval
    void setUpdateInterval(int interval);
    
    // Enable or disable progress display
    void enable(bool enable);

private:
    bool enabled;
    int updateInterval;
    int totalSteps;
    int currentStep;
    std::string currentOperation;
};

// Main CLI Application
class CLIApp {
public:
    CLIApp();
    ~CLIApp() = default;
    
    // Run the CLI application
    int run(int argc, char** argv);
    
    // Main application loop
    int execute();

private:
    CLIParser parser;
    ConfigManager configManager;
    CommandHandler commandHandler;
    HelpSystem helpSystem;
    LoggerConfig loggerConfig;
    ProgressMonitor progressMonitor;
    
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimulationClock> clock;
};

} // namespace cli
} // namespace nlm

#endif // NLM_CLI_HPP
