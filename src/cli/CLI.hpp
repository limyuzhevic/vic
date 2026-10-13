// NLM CLI - Neural Learning Machine Command Line Interface
// Enhanced CLI with comprehensive configuration management and operation modes

#pragma once

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <thread>
#include <atomic>
#include <map>

namespace nlm {

// Operation modes for NLM
enum class OperationMode {
    Simulation,
    Learning,
    Analysis,
    Benchmark,
    Batch
};

// CLI options configuration
struct CLIOptions {
    std::string configFile;
    std::string configJson;
    std::string paramsFile;
    OperationMode mode = OperationMode::Simulation;
    std::string outputFile;
    std::string logFile;
    std::string checkpointDir;
    bool enableVisualization = false;
    bool enableProfiling = false;
    bool enableMemoryAnalysis = false;
    bool batchMode = false;
    int numSteps = -1;  // -1 for unlimited
    double timestep = 0.001;
    uint64_t randomSeed = 42;
    bool help = false;
    bool version = false;
    std::vector<std::string> subsystemFilters;  // For enabling/disabling specific subsystems
    std::map<std::string, std::string> runtimeParams;  // Runtime parameter overrides
    std::string experimentName;
    bool continueLast = false;
    bool saveCheckpoint = false;
    bool loadCheckpoint = false;
    std::string command;
};

// Command line interface class
class CLI {
public:
    CLI();
    ~CLI();

    // Parse command line arguments
    bool parseArguments(int argc, char** argv);
    
    // Execute the parsed command
    int execute() const;
    
    // Get parsed options
    const CLIOptions& getOptions() const { return options; }

private:
    CLIOptions options;
    std::shared_ptr<Logger> logger;
    std::chrono::system_clock::time_point startTime;

    // Initialization and setup
    bool initialize();
    void setupLogger() const;
    void loadConfigurations() const;
    void applyRuntimeParameters() const;
    
    // Operation modes
    int runSimulation() const;
    int runLearning() const;
    int runAnalysis() const;
    int runBenchmark() const;
    int runBatch() const;
    
    // Utility functions
    void printHelp() const;
    void printVersion() const;
    void printProgress(int step, int totalSteps, double currentTime) const;
    void logSystemInfo() const;
    
    // Configuration loading
    bool loadConfigFile(const std::string& filepath) const;
    bool loadConfigJson(const std::string& json) const;
    bool loadParamsFile(const std::string& filepath) const;
    
    // Checkpoint management
    bool saveCurrentCheckpoint(const std::shared_ptr<Brain>& brain) const;
    bool loadCheckpoint(const std::shared_ptr<Brain>& brain) const;
    
    // Subsystem control
    void enableSubsystem(const std::string& subsystem) const;
    void disableSubsystem(const std::string& subsystem) const;
    bool isSubsystemEnabled(const std::string& subsystem) const;
    
    // Statistics and reporting
    void generateStatisticsReport(const std::shared_ptr<Brain>& brain) const;
    void generatePerformanceReport() const;
    void generateMemoryReport() const;
    
    // Threading and progress tracking
    void runWithProgress(int totalSteps, std::function<void(int, double)> stepCallback) const;
    
    // Signal handling
    void setupSignalHandlers() const;
    void cleanup() const;
};

// Stream-style CLI helper for easy usage
class CLIStream {
public:
    CLIStream() = default;
    ~CLIStream() = default;
    
    // Parse and execute (convenience method)
    static int Run(int argc, char** argv);
    
private:
    CLIStream(const CLIStream&) = delete;
    CLIStream& operator=(const CLIStream&) = delete;
};

} // namespace nlm
