// NLM Command Line Interface
// Phase 2: Real Neural Computation CLI
// Provides comprehensive command-line interface for NLM operations

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <memory>

namespace nlm {

// CLI modes
enum class CLIMode {
    Interactive,
    TestRun,
    Experiment,
    Benchmark,
    Visualize,
    ConfigEdit,
    Help,
    Version,
    ListTests,
    RunTest,
    Custom
};

// Output formats
enum class OutputFormat {
    Console,
    JSON,
    CSV,
    Detailed,
    Compact,
    MachineReadable
};

// CLI configuration
struct CLIConfig {
    CLIMode mode;
    std::string configFile;
    std::string outputFile;
    std::string logFile;
    std::string experimentFile;
    OutputFormat outputFormat;
    LogLevel logLevel;
    bool verbose;
    bool quiet;
    bool dryRun;
    bool resetBrain;
    bool continuous;
    std::vector<std::string> testNames;
    std::string customCommand;
    
    // Performance and debugging
    bool enableProfiling;
    bool enableValidation;
    bool enableLogging;
    size_t maxMemoryMB;
    double timeoutSeconds;
    
    // Neural parameters
    size_t neuronCount;
    size_t regionCount;
    double timestep;
    
    CLIConfig() : mode(CLIMode::Interactive), outputFormat(OutputFormat::Console),
                  logLevel(LogLevel::Info), verbose(false), quiet(false),
                  dryRun(false), resetBrain(false), continuous(false),
                  enableProfiling(false), enableValidation(false),
                  enableLogging(false), maxMemoryMB(1024), timeoutSeconds(300.0),
                  neuronCount(500), regionCount(1), timestep(0.001) {}
};

// Command line argument parser
class CLIParser {
public:
    CLIParser();
    ~CLIParser();
    
    // Parse command line arguments
    bool parse(int argc, char** argv, CLIConfig& config);
    
    // Print help information
    void printHelp() const;
    
    // Print version information
    void printVersion() const;
    
    // Validate configuration
    bool validate(const CLIConfig& config) const;
    
    // Get usage string
    std::string getUsage() const;
    
    // Get available test names
    std::vector<std::string> getAvailableTests() const;
    
    // Get available experiment names
    std::vector<std::string> getAvailableExperiments() const;
    
    // Get available benchmark modes
    std::vector<std::string> getAvailableBenchmarks() const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Help text sections
    const char* getGeneralHelp() const;
    const char* getTestHelp() const;
    const char* getExperimentHelp() const;
    const char* getBenchmarkHelp() const;
    const char* getConfigHelp() const;
    
    // Parse mode-specific arguments
    bool parseModeArguments(const std::string& mode, int argc, char** argv, 
                           CLIConfig& config, size_t& argIndex);
    
    // Parse test-related arguments
    bool parseTestArguments(int argc, char** argv, CLIConfig& config, 
                           size_t& argIndex);
    
    // Parse experiment-related arguments
    bool parseExperimentArguments(int argc, char** argv, CLIConfig& config, 
                                 size_t& argIndex);
    
    // Parse benchmark-related arguments
    bool parseBenchmarkArguments(int argc, char** argv, CLIConfig& config, 
                                size_t& argIndex);
    
    // Parse configuration-related arguments
    bool parseConfigArguments(int argc, char** argv, CLIConfig& config, 
                             size_t& argIndex);
    
    // Parse output-related arguments
    bool parseOutputArguments(int argc, char** argv, CLIConfig& config, 
                             size_t& argIndex);
    
    // Parse logging-related arguments
    bool parseLoggingArguments(int argc, char** argv, CLIConfig& config, 
                              size_t& argIndex);
    
    // Parse neural parameter arguments
    bool parseNeuralArguments(int argc, char** argv, CLIConfig& config, 
                             size_t& argIndex);
    
    // Parse performance arguments
    bool parsePerformanceArguments(int argc, char** argv, CLIConfig& config, 
                                   size_t& argIndex);
    
    // Resolve mode from string
    CLIMode resolveMode(const std::string& modeStr) const;
    
    // Validate mode-specific configuration
    bool validateModeSpecific(const CLIConfig& config) const;
};

// CLI execution engine
class CLIExecutor {
public:
    CLIExecutor();
    ~CLIExecutor();
    
    // Execute CLI command based on configuration
    int execute(const CLIConfig& config);
    
    // Execute test run
    int executeTestRun(const CLIConfig& config);
    
    // Execute experiment
    int executeExperiment(const CLIConfig& config);
    
    // Execute benchmark
    int executeBenchmark(const CLIConfig& config);
    
    // Execute visualization
    int executeVisualization(const CLIConfig& config);
    
    // Edit configuration
    int executeConfigEdit(const CLIConfig& config);
    
    // Run specific test
    int executeRunTest(const CLIConfig& config);
    
    // List available tests
    int executeListTests(const CLIConfig& config);
    
    // Custom command execution
    int executeCustom(const CLIConfig& config);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Initialize logging based on configuration
    void initializeLogging(const CLIConfig& config);
    
    // Load configuration file
    bool loadConfiguration(const CLIConfig& config, std::shared_ptr<Config> appConfig);
    
    // Apply command line overrides
    void applyOverrides(const CLIConfig& cliConfig, std::shared_ptr<Config> appConfig);
    
    // Create and configure brain
    std::shared_ptr<Brain> createBrain(const std::shared_ptr<Config>& config);
    
    // Run standard test suite
    int runStandardTests(std::shared_ptr<Brain> brain, const CLIConfig& config);
    
    // Run specific test
    int runSpecificTest(std::shared_ptr<Brain> brain, const std::string& testName, 
                       const CLIConfig& config);
    
    // Run experiment
    int runExperiment(std::shared_ptr<Brain> brain, const std::shared_ptr<Environment>& environment,
                     const CLIConfig& config);
    
    // Run benchmark
    int runBenchmark(const CLIConfig& config);
    
    // Format output based on format type
    std::string formatOutput(const std::string& data, OutputFormat format) const;
    
    // Write output to file
    bool writeOutput(const std::string& data, const std::string& filepath, 
                    OutputFormat format) const;
};

// CLI entry point
int runCLI(int argc, char** argv);

// Convenience functions for programmatic usage
namespace Programmatic {
    // Run NLM with configuration file
    int runWithConfig(const std::string& configFile);
    
    // Run specific test
    int runTest(const std::string& testName, const std::string& configFile = "");
    
    // Run experiment
    int runExperiment(const std::string& experimentName, const std::string& configFile = "");
    
    // Get brain state for analysis
    std::string getBrainState(const std::shared_ptr<Brain>& brain);
    
    // Save brain to file
    bool saveBrain(const std::shared_ptr<Brain>& brain, const std::string& filepath);
    
    // Load brain from file
    std::shared_ptr<Brain> loadBrain(const std::string& filepath, const std::shared_ptr<Config>& config);
}

} // namespace nlm
