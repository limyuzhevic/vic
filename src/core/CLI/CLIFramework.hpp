// NLM Command Line Interface Framework
// Provides modern argument parsing and unified interface for all NLM executables

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <optional>
#include <variant>
#include <iostream>

namespace nlm {

// Command-line options structure
struct CLIConfig {
    // General options
    bool help = false;
    bool version = false;
    bool dry_run = false;
    std::string config_file;
    std::string log_level;
    std::string log_file;
    bool quiet = false;
    bool verbose = false;
    
    // Command
    std::string command;
    std::vector<std::string> command_args;
    
    // Mode flags
    bool benchmark_mode = false;
    bool script_mode = false;
    bool test_mode = false;
    bool validate_mode = false;
    bool debug_mode = false;
    
    // Demo mode options
    std::string demo_type;
    int demo_episodes = 5;
    int demo_steps = 200;
    
    // Benchmark mode options
    std::vector<std::pair<size_t, size_t>> benchmark_configs;
    std::string benchmark_output;
    bool benchmark_detailed = false;
    
    // Script mode options
    std::string script_file;
    bool script_continue = false;
    
    // Advanced script options
    std::string script_mode;  // "file", "sweep", "grid", "sequential", "parallel"
    std::vector<std::string> script_files;
    std::vector<std::pair<std::string, std::string>> sweep_parameters;
    std::vector<std::pair<std::string, std::string>> grid_parameters;
    bool script_verbose = false;
    std::string script_output_format;  // "json", "csv", "text"
    std::string script_output_file;
    double script_timeout = 0.0;  // 0 = no timeout
    bool script_parallel = false;
    size_t script_max_threads = std::thread::hardware_concurrency();
    std::string script_parameter_type;  // "linear", "geometric", "random", "grid", "fixed"
    std::string script_start_value;
    std::string script_end_value;
    size_t script_steps = 0;
    std::string script_seed;
    
    // Validation mode options
    std::string config_schema;
    bool validate_configs = false;
    bool validate_format = false;
    
    // Dry run mode
    std::string dry_run_command;
};

// Command definitions
enum class Command {
    None,
    Demo,
    Benchmark,
    Script,
    Test,
    Validate,
    Help,
    Version
};

// CLI parser class
class CLIParser {
public:
    CLIParser();
    ~CLIParser();
    
    // Parse command line arguments
    bool parse(int argc, char** argv, CLIConfig& config);
    
    // Print help information
    void printHelp(const std::string& programName = "nlm") const;
    
    // Print version information
    void printVersion() const;
    
    // Validate configuration
    bool validate(const CLIConfig& config) const;
    
    // Get command string from enum
    static std::string commandToString(Command cmd);
    
    // Get command enum from string
    static Command stringToCommand(const std::string& str);
    
    // Detect configuration file format
    static ConfigFormat detectConfigFormat(const std::string& filepath);
    
    // Parse log level string
    static LogLevel stringToLogLevel(const std::string& level);
    
    // Get available commands
    static std::vector<std::string> getAvailableCommands();
    
    // Get demo types
    static std::vector<std::string> getAvailableDemoTypes();
    
    // Get default benchmark configurations
    static std::vector<std::pair<size_t, size_t>> getDefaultBenchmarkConfigs();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Help information strings
    std::unordered_map<std::string, std::string> helpTexts;
    
    // Version information
    std::string versionString;
    
    // Initialize help texts
    void initializeHelpTexts();
    
    // Parse command and options
    bool parseCommand(const std::vector<std::string>& args, CLIConfig& config);
    
    // Parse general options
    bool parseOptions(const std::vector<std::string>& args, CLIConfig& config);
    
    // Parse command arguments
    bool parseCommandArgs(const std::vector<std::string>& args, CLIConfig& config);
    
    // Parse demo arguments
    bool parseDemoArgs(const std::vector<std::string>& args, CLIConfig& config);
    
    // Parse benchmark arguments
    bool parseBenchmarkArgs(const std::vector<std::string>& args, CLIConfig& config);
    
    // Parse script arguments
    bool parseScriptArgs(const std::vector<std::string>& args, CLIConfig& config);
    
    // Error handling
    void printError(const std::string& error) const;
    
    // Help for specific command
    void printCommandHelp(Command cmd) const;
};

// CLI factory class - provides unified interface for all executables
class CLIFactory {
public:
    static std::unique_ptr<CLIParser> createParser();
    
    // Create configuration from CLI
    static std::shared_ptr<Config> createConfig(const CLIConfig& cliConfig);
    
    // Create logger from CLI configuration
    static std::shared_ptr<Logger> createLogger(const CLIConfig& cliConfig);
    
    // Execute command based on CLI configuration
    static int executeCommand(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig);
    
    // Load configuration from file or create default
    static std::shared_ptr<Config> loadConfiguration(const std::string& configFile, const CLIConfig& cliConfig);
    
    // Setup logging
    static void setupLogging(const CLIConfig& config);
    
    // Print configuration summary
    static void printConfigurationSummary(const std::shared_ptr<Config>& config);
    
    // Save configuration to file
    static bool saveConfiguration(const std::shared_ptr<Config>& config, const std::string& outputFile);

private:
    // Command execution functions
    static int executeDemoCommand(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig);
    static int executeBenchmarkCommand(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig);
    static int executeScriptCommand(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig);
    static int executeTestCommand(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig);
    static int executeValidateCommand(const CLIConfig& config, const std::shared_ptr<Config>& nlmConfig);
};

// Convenience functions for existing executables
class CLIAdapter {
public:
    // Adapt existing main() functions to work with new CLI
    static int adaptPhase3Demo(int argc, char* argv[]);
    static int adaptPhase4Demo(int argc, char* argv[]);
    static int adaptPhase6Demo(int argc, char* argv[]);
    static int adaptBenchmark(int argc, char* argv[]);
    static int adaptMain(int argc, char* argv[]);
    
    // Check if help or version was requested
    static bool shouldShowHelpOrVersion(int argc, char* argv[]);
    
    // Print help for backward compatibility
    static void printHelp() { std::cout << "Run with --help for usage information" << std::endl; }
    
private:
    static std::unique_ptr<CLIParser> parser;
    static std::unique_ptr<CLIFactory> factory;
};

} // namespace nlm