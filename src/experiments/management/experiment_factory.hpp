#pragma once

#include "enhanced_experiments.hpp"
#include "base_experiment.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <mutex>
#include <functional>

namespace nlm {

// Experiment factory for creating standardized experiment types
class ExperimentFactory {
public:
    ExperimentFactory(std::shared_ptr<Config> config = nullptr) : config_(config) {
        registerDefaultExperiments();
    }

    // Create an experiment by type
    std::shared_ptr<BaseExperiment> createExperiment(const std::string& type, const std::string& name);

    // Get list of available experiment types
    std::vector<std::string> getAvailableExperimentTypes() const {
        std::vector<std::string> types;
        for (const auto& pair : experimentCreators_) {
            types.push_back(pair.first);
        }
        return types;
    }

    // Register a new experiment type
    void registerExperimentType(const std::string& type,
                                std::function<std::shared_ptr<BaseExperiment>(const std::string&, std::shared_ptr<Config>)> creator) {
        std::lock_guard<std::mutex> lock(mutex_);
        experimentCreators_[type] = creator;
    }

    // Create experiment templates with parameters
    std::shared_ptr<BaseExperiment> createExperimentFromTemplate(
        const std::string& templateName,
        const std::map<std::string, std::string>& parameters);

private:
    void registerDefaultExperiments();
    std::shared_ptr<BaseExperiment> createPlasticityExperiment(const std::string& name);
    std::shared_ptr<BaseExperiment> createLearningCurveExperiment(const std::string& name);
    std::shared_ptr<BaseExperiment> createNetworkConnectivityExperiment(const std::string& name);
    std::shared_ptr<BaseExperiment> createDevelopmentalExperiment(const std::string& name);
    std::shared_ptr<BaseExperiment> createNeuromodulationExperiment(const std::string& name);

    std::unordered_map<std::string, std::function<std::shared_ptr<BaseExperiment>(const std::string&, std::shared_ptr<Config>)>> experimentCreators_;
    mutable std::mutex mutex_;
    std::shared_ptr<Config> config_;
};

// Parameter sweep manager for automated parameter variation
class ParameterSweepManager {
public:
    ParameterSweepManager(std::shared_ptr<Config> config = nullptr) : config_(config) {
    }

    // Run parameter sweep for an experiment template
    std::vector<SweepResult> runSweep(const std::string& templateName,
                                      const std::map<std::string, std::vector<std::string>>& parameterMatrix,
                                      size_t numIterations = 1);

    // Optimize experiment parameters for best performance
    OptimizationResult optimizeParameters(const std::string& templateName,
                                           const std::map<std::string, std::vector<std::string>>& parameterSpace,
                                           const std::string& objective = "performance");

    // Get sweep results for an experiment
    const std::vector<SweepResult>& getSweepResults(const std::string& experimentName) const;

    // Save sweep results
    bool saveSweepResults(const std::string& experimentName, const std::string& filepath) const;

    // Load sweep results
    bool loadSweepResults(const std::string& experimentName, const std::string& filepath);

    // Clear all sweep results
    void clearSweepResults() { sweepResults_.clear(); }

private:
    std::vector<SweepResult> runSingleSweep(const std::string& templateName,
                                            const std::map<std::string, std::string>& parameters,
                                            size_t iteration);

    std::map<std::string, std::vector<SweepResult>> sweepResults_;
    std::shared_ptr<Config> config_;
    mutable std::mutex mutex_;
};

// Result analyzer for statistical analysis and visualization
class ResultAnalyzer {
public:
    ResultAnalyzer() {
    }

    // Calculate statistics for experiment results
    struct ExperimentStatistics {
        double mean;
        double median;
        double stddev;
        double min;
        double max;
        double percentile25;
        double percentile75;
        size_t count;
        std::vector<double> rawData;
    };

    ExperimentStatistics calculateStatistics(const std::vector<double>& data) const;

    // Compare two experiment results
    struct ComparisonResult {
        std::string experiment1Name;
        std::string experiment2Name;
        double performanceDifference;
        double confidenceLevel;
        bool isSignificant;
        std::string interpretation;
    };

    ComparisonResult compareExperiments(const std::vector<double>& results1,
                                        const std::vector<double>& results2) const;

    // Generate visualization data
    struct VisualizationData {
        std::vector<double> xValues;
        std::vector<double> yValues;
        std::string chartType;
        std::string title;
        std::string xAxisLabel;
        std::string yAxisLabel;
    };

    VisualizationData generateTimeSeriesData(const std::shared_ptr<BaseExperiment>& experiment) const;
    VisualizationData generateMetricComparisonData(const std::vector<std::shared_ptr<BaseExperiment>>& experiments,
                                                   const std::string& metricName) const;

    // Generate CSV export
    std::string generateCSV(const std::vector<std::shared_ptr<BaseExperiment>>& experiments) const;

    // Generate JSON export
    std::string generateJSON(const std::vector<std::shared_ptr<BaseExperiment>>& experiments) const;

    // Perform statistical tests
    enum class StatisticalTest { TTest, MannWhitneyU, KolmogorovSmirnov };

    struct StatisticalTestResult {
        StatisticalTest test;
        double pValue;
        bool isSignificant;
        std::string description;
    };

    StatisticalTestResult performStatisticalTest(const std::vector<double>& data1,
                                                 const std::vector<double>& data2,
                                                 StatisticalTest test = StatisticalTest::TTest) const;

    // Export analysis results
    bool exportResults(const std::string& filepath, const std::string& format = "csv") const;

private:
    std::string doubleToString(double value, int precision = 6) const;
};

// Experiment scheduler for managing experiment execution
class ExperimentScheduler {
public:
    ExperimentScheduler(std::shared_ptr<Config> config = nullptr) : config_(config), nextExperimentId_(1) {
        // Set up worker threads
        numWorkers_ = (config_ && config_->has("max_threads")) ? 
                     std::stoul(config_->getValue("max_threads")) : 4;

        for (size_t i = 0; i < numWorkers_; ++i) {
            workers_.emplace_back(&ExperimentScheduler::workerThread, this, i);
        }
    }

    ~ExperimentScheduler() {
        shutdown();
    }

    // Add an experiment to the queue
    void scheduleExperiment(std::shared_ptr<BaseExperiment> experiment);

    // Add multiple experiments to the queue
    void scheduleBatch(const std::vector<std::shared_ptr<BaseExperiment>>& experiments);

    // Remove an experiment from the queue
    bool unscheduleExperiment(const std::string& experimentName);

    // Get current queue status
    struct QueueStatus {
        size_t pendingExperiments;
        size_t runningExperiments;
        size_t completedExperiments;
        size_t failedExperiments;
        double averageProgress;
        std::chrono::duration<double> estimatedTimeRemaining;
    };

    QueueStatus getQueueStatus() const;

    // Start/pause/stop the scheduler
    void start();
    void pause();
    void stop();
    bool isRunning() const { return running_; }

    // Get current experiment (for progress monitoring)
    std::shared_ptr<BaseExperiment> getCurrentExperiment() const;

    // Get completed experiments
    std::vector<std::shared_ptr<BaseExperiment>> getCompletedExperiments() const;

    // Save/load scheduler state
    bool saveState(const std::string& filepath) const;
    bool loadState(const std::string& filepath);

    // Cleanup completed experiments
    void cleanupCompletedExperiments();

private:
    void workerThread(size_t workerId);
    void processExperimentQueue();
    std::shared_ptr<BaseExperiment> createExperimentFromTemplate(const std::string& templateName,
                                                                  const std::map<std::string, std::string>& parameters);

    struct ExperimentQueueEntry {
        std::shared_ptr<BaseExperiment> experiment;
        std::chrono::time_point<std::chrono::high_resolution_clock> scheduledTime;
        bool priority;
    };

    std::vector<ExperimentQueueEntry> experimentQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCondition_;

    std::vector<std::thread> workers_;
    std::atomic<bool> running_;
    std::atomic<bool> paused_;

    std::shared_ptr<BaseExperiment> currentExperiment_;
    std::mutex currentExperimentMutex_;

    std::vector<std::shared_ptr<BaseExperiment>> completedExperiments_;
    std::vector<std::shared_ptr<BaseExperiment>> failedExperiments_;

    size_t nextExperimentId_;
    size_t numWorkers_;
    std::shared_ptr<Config> config_;
};

// Reproducibility manager for version control and tracking
class ReproducibilityManager {
public:
    ReproducibilityManager(std::shared_ptr<Config> config = nullptr) : config_(config) {
    }

    // Create experiment snapshot
    struct ExperimentSnapshot {
        std::string experimentName;
        std::string timestamp;
        std::string gitCommit;
        std::string configHash;
        std::string experimentParameters;
        std::string neuralNetworkState;
        std::string results;
        std::string dependencies;
    };

    ExperimentSnapshot createSnapshot(std::shared_ptr<BaseExperiment> experiment);

    // Restore experiment from snapshot
    bool restoreSnapshot(const ExperimentSnapshot& snapshot, std::shared_ptr<BaseExperiment>& experiment);

    // Create experiment version
    struct ExperimentVersion {
        std::string versionId;
        std::string timestamp;
        std::string description;
        std::string parentVersion;
        ExperimentSnapshot snapshot;
    };

    std::string createVersion(const std::string& experimentName, const std::string& description);

    // Compare experiment versions
    struct VersionComparison {
        std::string version1;
        std::string version2;
        std::vector<std::string> differences;
        double similarityScore;
    };

    VersionComparison compareVersions(const std::string& version1, const std::string& version2);

    // Get experiment version history
    std::vector<std::string> getVersionHistory(const std::string& experimentName) const;

    // Get experiment snapshot
    ExperimentSnapshot getSnapshot(const std::string& experimentName, const std::string& version = "latest") const;

    // Clean up old versions
    void cleanupOldVersions(size_t maxVersionsToKeep = 10);

    // Export experiment for sharing
    bool exportExperiment(const std::string& experimentName, const std::string& outputPath, const std::string& format = "json") const;

    // Import experiment from file
    bool importExperiment(const std::string& inputPath, const std::string& versionDescription = "Imported");

private:
    std::string generateVersionId() const;
    std::string generateConfigHash(const std::map<std::string, std::string>& config) const;
    std::string getCurrentGitCommit() const;

    std::map<std::string, std::vector<ExperimentVersion>> versionHistory_;
    std::map<std::string, ExperimentSnapshot> currentSnapshots_;
    mutable std::mutex mutex_;
    std::shared_ptr<Config> config_;
};

// Common experiment utilities
class ExperimentUtils {
public:
    // String utilities
    static std::string toLowerCase(const std::string& str);
    static std::string toUpperCase(const std::string& str);
    static std::string trim(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string generateExperimentName(const std::string& prefix = "exp", size_t suffix = 0);

    // Parameter parsing utilities
    static std::map<std::string, std::string> parseParameterString(const std::string& paramString);
    static std::string parameterToString(const std::map<std::string, std::string>& parameters);

    // File path utilities
    static std::string getDefaultResultDirectory();
    static std::string getDefaultCheckpointDirectory();
    static std::string getDefaultParameterFile();

    // Validation utilities
    static bool isValidParameterName(const std::string& name);
    static bool isValidParameterValue(const std::string& value);
    static bool validateExperimentName(const std::string& name);

    // Time utilities
    static std::string getCurrentTimestamp();
    static double calculateTimeRemaining(double progress, double elapsedTime);

    // Statistics utilities
    static double calculateMean(const std::vector<double>& values);
    static double calculateStandardDeviation(const std::vector<double>& values, double mean);
    static double calculatePercentile(const std::vector<double>& values, double percentile);

    // JSON utilities
    static std::string jsonEscape(const std::string& str);
    static std::string generateJSONFromExperiment(const std::shared_ptr<BaseExperiment>& experiment);

    // Export utilities
    static bool exportToCSV(const std::string& filepath, const std::vector<std::shared_ptr<BaseExperiment>>& experiments);
    static bool exportToJSON(const std::string& filepath, const std::vector<std::shared_ptr<BaseExperiment>>& experiments);
    static bool exportToMarkdown(const std::string& filepath, const std::vector<std::shared_ptr<BaseExperiment>>& experiments);

    // IO utilities
    static bool fileExists(const std::string& filepath);
    static bool createDirectory(const std::string& path);
    static bool copyFile(const std::string& source, const std::string& destination);
    static std::vector<std::string> listFilesInDirectory(const std::string& directory, const std::string& extension = "*");
};

// Parameter parser for parsing experiment parameters from various sources
class ParameterParser {
public:
    ParameterParser() {
    }

    // Parse parameters from string (key=value format)
    std::map<std::string, std::string> parseString(const std::string& paramString) const;

    // Parse parameters from JSON
    std::map<std::string, std::string> parseJSON(const std::string& jsonString) const;

    // Parse parameters from command line
    std::map<std::string, std::string> parseCommandLine(int argc, char** argv) const;

    // Parse parameters from file
    std::map<std::string, std::string> parseFile(const std::string& filepath) const;

    // Validate parameter values against expected types
    bool validateParameter(const std::string& key, const std::string& value, const std::string& typeHint = "auto") const;

    // Get parameter type hint
    std::string getParameterType(const std::string& key, const std::map<std::string, std::string>& parameters) const;

private:
    std::map<std::string, std::string> parseKeyValuePairs(const std::string& input) const;
};

// Result exporter for exporting experiment results in various formats
class ResultExporter {
public:
    ResultExporter() {
    }

    // Export experiment results to CSV format
    bool exportToCSV(const std::shared_ptr<BaseExperiment>& experiment, const std::string& filepath) const;

    // Export experiment results to JSON format
    bool exportToJSON(const std::shared_ptr<BaseExperiment>& experiment, const std::string& filepath) const;

    // Export experiment results to Markdown format
    bool exportToMarkdown(const std::shared_ptr<BaseExperiment>& experiment, const std::string& filepath) const;

    // Export experiment results to HTML format
    bool exportToHTML(const std::shared_ptr<BaseExperiment>& experiment, const std::string& filepath) const;

    // Export experiment results to PDF format (requires external library)
    bool exportToPDF(const std::shared_ptr<BaseExperiment>& experiment, const std::string& filepath) const;

    // Batch export multiple experiments
    bool exportBatch(const std::vector<std::shared_ptr<BaseExperiment>>& experiments, 
                    const std::string& outputDirectory, const std::string& format = "csv") const;

private:
    std::string escapeCSVValue(const std::string& value) const;
    std::string generateHTMLReport(const std::shared_ptr<BaseExperiment>& experiment) const;
};

} // namespace nlm