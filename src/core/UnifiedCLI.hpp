// unified_cli.hpp
#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <variant>
#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "experiments/ExperimentRunner.hpp"

namespace nlm {

// Command line options structure
struct UnifiedCLIOptions {
    std::string mode;                    // "phase2", "phase6", "phase3", "phase4", "help", "version"
    std::string configFile;              // Configuration file path
    bool verbose = false;                // Enable verbose logging
    bool quiet = false;                  // Suppress output
    std::string outputFile;              // Output file for results
    bool listConfigs = false;            // List available configurations
    bool validateConfig = false;         // Validate configuration
    std::vector<std::string> extraArgs;  // Additional arguments for specific modes
};

// Unified CLI class
class UnifiedCLI {
public:
    UnifiedCLI();
    ~UnifiedCLI();
    
    // Main entry point
    int run(int argc, char** argv);
    
    // Parse command line arguments
    UnifiedCLIOptions parseArguments(int argc, char** argv);
    
    // Display help information
    void showHelp(const std::string& programName);
    
    // Display version information
    void showVersion();
    
    // List available configurations
    void listAvailableConfigurations();
    
    // Validate configuration file
    bool validateConfiguration(const std::string& configFile);
    
    // Execute based on mode
    int executeMode(const UnifiedCLIOptions& options);
    
    // Phase 2: Real Neural Computation Demo
    int runPhase2Demo(const UnifiedCLIOptions& options);
    
    // Phase 3: Experiment
    int runPhase3Experiment(const UnifiedCLIOptions& options);
    
    // Phase 4: Experiment
    int runPhase4Experiment(const UnifiedCLIOptions& options);
    
    // Phase 6: Integration Demo
    int runPhase6Demo(const UnifiedCLIOptions& options);
    
    // Initialize logging based on options
    void setupLogging(bool verbose, bool quiet);
    
private:
    // Available configuration templates
    std::unordered_map<std::string, std::string> getConfigurationTemplates() const;
    
    // Create default configuration
    std::shared_ptr<Config> createDefaultConfig() const;
    
    // Load and validate configuration
    std::shared_ptr<Config> loadConfig(const std::string& configFile) const;
    
    // Display progress
    void displayProgress(const std::string& message, bool forceOutput = false);
    
    // Check if output should be shown
    bool shouldOutput() const;
    
    // Current mode
    std::string currentMode_;
    
    // Output suppression flag
    bool outputSuppressed_ = false;
};

// Command line argument parsing functions
UnifiedCLIOptions parseCommandLine(int argc, char** argv);

// Global instance for easy access
inline UnifiedCLI g_cli;