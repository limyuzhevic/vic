// NLM Advanced CLI Tool
// Provides comprehensive command-line interface for advanced NLM users
// Supports phase selection, batch processing, checkpoint management, and experiment configuration

#ifndef NLM_ADVANCED_CLI_HPP
#define NLM_ADVANCED_CLI_HPP

#include <string>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <iomanip>
#include <sstream>
#include <algorithm>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"

namespace nlm {

// Advanced command-line options
struct AdvancedCLIOptions {
    // Phase selection
    std::string phase;
    bool runPhase2Demo = false;
    bool runPhase3Demo = false;
    bool runPhase4Demo = false;
    bool runPhase6Demo = false;
    bool runPhase5Experiment = false;
    bool runPhase6Experiment = false;
    
    // Experiment parameters
    size_t neurons = 500;
    uint64_t steps = 2000;
    size_t regions = 1;
    float connectionProbability = 0.1f;
    
    // Performance and optimization
    std::string performanceMode;
    bool enableSIMD = false;
    bool enableParallel = false;
    bool enableMemoryPools = false;
    bool enableEventQueues = false;
    
    // Logging and output
    std::string logLevel;
    std::string logFile;
    bool verbose = false;
    bool quiet = false;
    
    // Checkpoint and persistence
    std::string checkpointPath;
    std::string checkpointDir;
    bool saveCheckpoint = false;
    bool loadCheckpoint = false;
    bool cleanCheckpoints = false;
    
    // Batch processing
    std::string batchFile;
    std::string outputDirectory;
    bool headless = false;
    
    // Scripting and automation
    std::string scriptFile;
    bool dryRun = false;
    
    // Debug and analysis
    bool profile = false;
    bool validate = false;
    bool benchmark = false;
    std::string benchmarkMode;
    
    // Experiment management
    std::string experimentName;
    std::string experimentDescription;
    std::string metadataFile;
};

// Advanced CLI class
class AdvancedCLI {
public:
    AdvancedCLI() = default;
    ~AdvancedCLI() = default;
    
    // Parse command line arguments
    AdvancedCLIOptions parseArgs(int argc, char** argv);
    
    // Display help and usage information
    void printHelp() const;
    void printPhaseHelp() const;
    void printExperimentHelp() const;
    void printPerformanceHelp() const;
    
    // Display configuration information
    void printConfiguration(const AdvancedCLIOptions& options) const;
    void printStatus(const AdvancedCLIOptions& options) const;
    
    // Execute based on parsed options
    int execute(const AdvancedCLIOptions& options);
    
    // Phase-specific execution
    int executePhase2Demo(const AdvancedCLIOptions& options);
    int executePhase3Demo(const AdvancedCLIOptions& options);
    int executePhase4Demo(const AdvancedCLIOptions& options);
    int executePhase6Demo(const AdvancedCLIOptions& options);
    int executePhase5Experiment(const AdvancedCLIOptions& options);
    int executePhase6Experiment(const AdvancedCLIOptions& options);
    
    // Utility functions
    void createConfigurationFile(const AdvancedCLIOptions& options) const;
    void loadConfigurationFromFile(const std::string& filename, AdvancedCLIOptions& options) const;
    void saveConfigurationToFile(const AdvancedCLIOptions& options) const;
    
    // Batch processing
    void processBatch(const AdvancedCLIOptions& options);
    void processScript(const AdvancedCLIOptions& options);
    
    // Benchmark and profiling
    void runBenchmark(const AdvancedCLIOptions& options);
    void profilePerformance(const AdvancedCLIOptions& options);
    
    // Checkpoint management
    void manageCheckpoints(const AdvancedCLIOptions& options);
    void cleanCheckpointDirectory(const std::string& directory);
    
    // Validation and testing
    bool validateConfiguration(const AdvancedCLIOptions& options) const;
    void runValidationTests(const AdvancedCLIOptions& options);
    
private:
    // Helper functions
    std::string getCurrentTimestamp() const;
    std::string formatDuration(double seconds) const;
    void setupLogging(const AdvancedCLIOptions& options);
    void setupPerformanceOptions(const AdvancedCLIOptions& options);
    
    // Command line argument parsing helpers
    bool parseAdvancedOptions(const std::string& arg, AdvancedCLIOptions& options) const;
    bool parsePhaseOption(const std::string& arg, AdvancedCLIOptions& options) const;
    bool parseExperimentOption(const std::string& arg, AdvancedCLIOptions& options) const;
    bool parsePerformanceOption(const std::string& arg, AdvancedCLIOptions& options) const;
    bool parseLoggingOption(const std::string& arg, AdvancedCLIOptions& options) const;
    bool parseCheckpointOption(const std::string& arg, AdvancedCLIOptions& options) const;
    bool parseBatchOption(const std::string& arg, AdvancedCLIOptions& options) const;
    
    // Configuration file helpers
    std::map<std::string, std::string> loadConfigFile(const std::string& filename) const;
    void saveConfigToFile(const std::map<std::string, std::string>& config, const std::string& filename) const;
    std::string getConfigValue(const std::map<std::string, std::string>& config, const std::string& key, const std::string& defaultValue = "") const;
};

// Global CLI instance
extern AdvancedCLI g_cli;

} // namespace nlm

#endif // NLM_ADVANCED_CLI_HPP