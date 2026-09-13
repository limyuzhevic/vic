// Advanced command-line interface for NLM
// Provides enhanced user experience and advanced configuration options

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <map>
#include "core/Config/Config.hpp"

namespace nlm {

// Advanced command-line parser for NLM
class AdvancedCommandLineParser {
public:
    AdvancedCommandLineParser(int argc, char** argv);
    ~AdvancedCommandLineParser() = default;
    
    // Parse command line arguments
    bool parse();
    
    // Get parsed configuration
    std::shared_ptr<Config> getConfig() const { return config; }
    
    // Print help information
    static void printHelp();
    
    // Print version information
    static void printVersion();
    
    // Check if a specific flag is present
    bool hasFlag(const std::string& flag) const;
    
    // Get flag value if present
    std::string getFlagValue(const std::string& flag) const;
    
    // Get positional arguments
    const std::vector<std::string>& getPositionalArgs() const { return positionalArgs; }
    
    // Check if running in verbose mode
    bool isVerbose() const { return verbose; }
    
    // Check if running in debug mode
    bool isDebug() const { return debug; }
    
    // Check if running in benchmark mode
    bool isBenchmark() const { return benchmark; }
    
    // Get output file path
    const std::string& getOutputFile() const { return outputFile; }
    
    // Get checkpoint file path
    const std::string& getCheckpointFile() const { return checkpointFile; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    int argc;
    char** argv;
    
    std::shared_ptr<Config> config;
    std::vector<std::string> positionalArgs;
    std::map<std::string, std::string> flagValues;
    
    bool verbose;
    bool debug;
    bool benchmark;
    std::string outputFile;
    std::string checkpointFile;
    
    // Parse standard flags
    void parseStandardFlags();
    
    // Parse advanced configuration flags
    void parseAdvancedConfig();
    
    // Parse experiment-specific flags
    void parseExperimentFlags();
    
    // Parse performance flags
    void parsePerformanceFlags();
    
    // Validate configuration
    bool validateConfig();
    
    // Set default configuration
    void setDefaults();
};

// Command-line based NLM execution
class CommandLineInterface {
public:
    CommandLineInterface(int argc, char** argv);
    ~CommandLineInterface() = default;
    
    // Run the NLM with parsed configuration
    int run();
    
    // Execute specific commands
    int executeCreateCommand(const std::map<std::string, std::string>& args);
    int executeRunCommand(const std::map<std::string, std::string>& args);
    int executeAnalyzeCommand(const std::map<std::string, std::string>& args);
    int executeBenchmarkCommand(const std::map<std::string, std::string>& args);
    int executeExportCommand(const std::map<std::string, std::string>& args);
    int executeImportCommand(const std::map<std::string, std::string>& args);
    
    // Show help for specific command
    void showCommandHelp(const std::string& command) const;
    
    // Show general help
    static void showGeneralHelp();
    
    // Show version information
    static void showVersionInfo();
    
private:
    std::unique_ptr<AdvancedCommandLineParser> parser;
    
    // Command execution methods
    int executeCommand(const std::string& command, const std::map<std::string, std::string>& args);
    
    // Configuration utilities
    void setupConfiguration(std::shared_ptr<Config> config);
    
    // Validation methods
    bool validateExecutionParameters() const;
    bool validateFilePaths() const;
    
    // Error handling
    void handleError(const std::string& error) const;
    void handleWarning(const std::string& warning) const;
};

// Quick help functions
inline void showQuickStart() {
    std::cout << "=== NLM Quick Start ===" << std::endl;
    std::cout << "Run basic simulation:" << std::endl;
    std::cout << "  nlm --neuron_count=1000 --region_count=2" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Run with custom configuration:" << std::endl;
    std::cout << "  nlm --config=my_config.cfg --verbose" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Run benchmark test:" << std::endl;
    std::cout << "  nlm --benchmark --steps=10000" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "For more options: nlm --help" << std::endl;
}

inline void showGettingStarted() {
    std::cout << "=== NLM Getting Started ===" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "1. Basic Usage:" << std::endl;
    std::cout << "   nlm" << std::endl;
    std::cout << "   Run the default Phase 2 neural computation test" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "2. Custom Configuration:" << std::endl;
    std::cout << "   nlm --config=my_config.cfg" << std::endl;
    std::cout << "   Load configuration from file" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "3. Experiment Modes:" << std::endl;
    std::cout << "   nlm --experiment=plasticity" << std::endl;
    std::cout << "   Run specific plasticity experiments" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "4. Advanced Features:" << std::endl;
    std::cout << "   nlm --verbose --benchmark --output=results.json" << std::endl;
    std::cout << "   Enable verbose output, benchmark mode, and file output" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "5. Neural Network Design:" << std::endl;
    std::cout << "   nlm --neuron_count=5000 --region_count=4 --plasticity=full" << std::endl;
    std::cout << "   Design large-scale neural networks with advanced plasticity" << std::endl;
}

} // namespace nlm