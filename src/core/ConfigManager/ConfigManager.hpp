#pragma once

#include "core/Config/Config.hpp"
#include "core/CommandLineParser/CommandLineParser.hpp"
#include <string>
#include <map>
#include <set>

namespace nlm {

class ConfigManager {
public:
    struct ExperimentOptions {
        int phase = 1;
        std::string experimentType;
        bool demoMode = false;
        bool benchmarkMode = false;
        bool analyzeMode = false;
    };

    struct SimulationOptions {
        int steps = 1000;
        double timestep = 0.001;
        int64_t neuronCount = 500;
        int64_t regionCount = 1;
    };

    struct OutputOptions {
        bool verbose = false;
        bool quiet = false;
        std::string outputFile;
        bool noBanner = false;
    };

    struct CheckpointOptions {
        bool loadCheckpoint = false;
        bool saveCheckpoint = false;
        std::string checkpointPath;
    };

    struct DebugOptions {
        bool debug = false;
    };

    struct PerformanceOptions {
        bool benchmark = false;
        bool perfMetrics = false;
        bool profile = false;
        bool compareConfigs = false;
        std::string profileOutputFormat;  // "json", "human", "csv"
        std::string benchmarkOutputFile;
        std::string metricsOutputFile;
        std::string profileOutputFile;
        std::string comparisonOutputFile;
        bool verbosePerformance = false;
    };

    ConfigManager();
    
    // Initialize from command line arguments
    bool initialize(int argc, char** argv);
    
    // Get options
    const ExperimentOptions& getExperimentOptions() const { return experimentOptions; }
    const SimulationOptions& getSimulationOptions() const { return simulationOptions; }
    const OutputOptions& getOutputOptions() const { return outputOptions; }
    const CheckpointOptions& getCheckpointOptions() const { return checkpointOptions; }
    const DebugOptions& getDebugOptions() const { return debugOptions; }
    const PerformanceOptions& getPerformanceOptions() const { return performanceOptions; }
    
    // Configuration access
    const std::shared_ptr<Config>& getConfig() const { return config; }
    
    // Process and apply all options
    bool processOptions();
    
    // Print help information
    void printHelp() const;
    
    // Print version information
    void printVersion() const;
    
private:
    std::shared_ptr<Config> config;
    
    ExperimentOptions experimentOptions;
    SimulationOptions simulationOptions;
    OutputOptions outputOptions;
    CheckpointOptions checkpointOptions;
    DebugOptions debugOptions;
    PerformanceOptions performanceOptions;
    
    std::unique_ptr<CommandLineParser> parser;
    
    // Option processing methods
    void processExperimentOptions();
    void processSimulationOptions();
    void processOutputOptions();
    void processCheckpointOptions();
    void processDebugOptions();
    void processPerformanceOptions();
    void processConfigOverrides();
    
    // Helper methods
    std::string trim(const std::string& str) const;
    bool parseBool(const std::string& value) const;
    int parseInt(const std::string& value, int defaultValue) const;
    double parseDouble(const std::string& value, double defaultValue) const;
    
    // Option validation
    bool validateOptions() const;
    
    // Constants
    static constexpr int MIN_PHASE = 1;
    static constexpr int MAX_PHASE = 6;
    static constexpr int MIN_STEPS = 0;
    static constexpr int MAX_STEPS = 1000000;
    static constexpr double MIN_TIMESTEP = 0.0001;\n    static constexpr double MAX_TIMESTEP = 1.0;
    static constexpr int64_t MIN_NEURON_COUNT = 1;
    static constexpr int64_t MAX_NEURON_COUNT = 100000;
    static constexpr int64_t MIN_REGION_COUNT = 1;
    static constexpr int64_t MAX_REGION_COUNT = 1000;
};

} // namespace nlm
