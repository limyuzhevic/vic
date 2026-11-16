#include "ToolsCommon.hpp"
#include "Brain.hpp"
#include "Config.hpp"
#include "CheckpointSystem.hpp"
#include "Metrics.hpp"
#include "Experiment.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <iomanip>
#include <chrono>

namespace nlm {

/**
 * Brain Configuration Tool
 * 
 * Command-line interface to create, load, and save brain configurations
 */
class BrainConfigTool {
public:
    bool create(const std::vector<std::pair<std::string, std::string>>& args);
    bool load(const std::string& filepath);
    bool save(const std::string& filepath, bool pretty = false);
    bool list(const std::string& pattern = "");
    bool set(const std::string& key, const std::string& value);
    bool get(const std::string& key);
    bool delete(const std::string& key);
    bool validate(const std::string& filepath);
    
    const Config& getConfig() const { return config_; }
    
private:
    Config config_;
    std::string currentFile_;
};

/**
 * Batch Simulation Tool
 * 
 * Run multiple simulation scenarios with different configurations
 */
class BatchSimulationTool {
public:
    bool run(const std::string& configFile, const std::string& outputDir, 
             int maxConcurrent = 1, bool dryRun = false);
    bool runSingle(const std::vector<std::pair<std::string, std::string>>& config,
                  const std::string& outputPrefix = "");
    bool generateConfig(const std::string& baseConfig, 
                       const std::map<std::string, std::vector<std::string>>& variations);
    
private:
    bool runSimulation(const Config& config, const std::string& outputPath);
    bool saveExperimentResults(const Experiment& experiment, 
                              const std::string& outputPath);
    double measurePerformance(std::function<void()> simulationFunc);
};

/**
 * Analysis Tool
 * 
 * Analyze simulation results and extract statistics
 */
class AnalysisTool {
public:
    bool analyze(const std::string& inputDir, const std::string& outputFile = "",
                const std::vector<std::string>& metricNames = {});
    bool compare(const std::vector<std::string>& directories,
                 const std::vector<std::string>& metricNames = {});
    bool extract(const std::string& inputFile, const std::string& metricName,
                 double threshold = 0.0);
    bool generateReport(const std::string& inputDir, const std::string& format = "text");
    
    const Metrics& getMetrics() const { return metrics_; }
    
private:
    Metrics metrics_;
    std::string lastAnalysisDir_;
};

/**
 * Visualization Tool
 * 
 * Basic visualization of brain activity (text-based output)
 */
class VisualizationTool {
public:
    bool visualize(const std::string& checkpointFile, const std::string& outputFile = "",
                  bool detailed = false, int timeStep = 1);
    bool visualizeTimeline(const std::vector<std::string>& checkpointFiles,
                         const std::string& outputFile = "");
    bool visualizeNetwork(const std::string& checkpointFile, const std::string& outputFile = "");
    bool generateStatsReport(const std::string& checkpointFile, const std::string& outputFile = "");
    
private:
    bool formatNeuronData(const NeuronCheckpointData& data, std::ostringstream& oss);
    bool formatSynapseData(const SynapseCheckpointData& data, std::ostringstream& oss);
    std::string formatNumber(double value, int precision = 6);
    std::string getSpikeStateSymbol(uint8_t state);
};

/**
 * Checkpoint Management Tool
 * 
 * Tools to save/load brain states efficiently
 */
class CheckpointTool {
public:
    bool save(const std::string& outputFile, const std::string& inputFile = "",
             CheckpointManager& manager = getDefaultManager());
    bool load(const std::string& inputFile, const std::string& outputFile = "",
             CheckpointManager& manager = getDefaultManager());
    bool list(const std::string& directory, bool recursive = false);
    bool delete(const std::string& checkpointFile);
    bool prune(const std::string& directory, size_t keepCount = 5);
    bool info(const std::string& checkpointFile);
    
    static CheckpointManager& getDefaultManager();
    
private:
    static CheckpointManager defaultManager_;
};

/**
 * Performance Profiler
 * 
 * Simple profiling of simulation performance
 */
class PerformanceProfiler {
public:
    struct ProfileResult {
        std::string name;
        double totalTime;
        double cpuTime;
        double wallTime;
        uint64_t memoryUsed;
        std::map<std::string, double> metrics;
    };
    
    bool profile(const std::string& configFile, const std::string& outputFile = "",
                const std::vector<std::string>& stages = {});
    bool profileSingle(const std::vector<std::pair<std::string, std::string>>& config,
                     const std::string& outputFile = "");
    bool compareProfiles(const std::vector<std::string>& profileFiles);
    bool generateReport(const std::vector<ProfileResult>& results, 
                       const std::string& outputFile = "");
    
    const std::vector<ProfileResult>& getResults() const { return results_; }
    
private:
    std::vector<ProfileResult> results_;
    std::chrono::high_resolution_clock::time_point startTime_;
    uint64_t getMemoryUsage();
};

} // namespace nlm
