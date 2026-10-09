#pragma once

#include "cli/CLI.hpp"
#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <thread>

namespace nlm {

class ExperimentCLI {
public:
    ExperimentCLI();
    ~ExperimentCLI();
    
    // Disable copying, enable moving
    ExperimentCLI(const ExperimentCLI&) = delete;
    ExperimentCLI& operator=(const ExperimentCLI&) = delete;
    ExperimentCLI(ExperimentCLI&&) noexcept;
    ExperimentCLI& operator=(ExperimentCLI&&) noexcept;
    
    // Initialization
    bool initialize();
    
    // Experiment configuration commands
    bool experimentCommand(const std::vector<std::string>& args);
    bool parameterCommand(const std::vector<std::string>& args);
    bool patternCommand(const std::vector<std::string>& args);
    bool scenarioCommand(const std::vector<std::string>& args);
    bool setupCommand(const std::vector<std::string>& args);
    bool configureCommand(const std::vector<std::string>& args);
    bool validateCommand(const std::vector<std::string>& args);
    
    // Experiment execution commands
    bool runCommand(const std::vector<std::string>& args);
    bool executeCommand(const std::vector<std::string>& args);
    bool startCommand(const std::vector<std::string>& args);
    bool stopCommand(const std::vector<std::string>& args);
    bool pauseCommand(const std::vector<std::string>& args);
    bool resumeCommand(const std::vector<std::string>& args);
    
    // Experiment results and analysis
    bool resultsCommand(const std::vector<std::string>& args);
    bool analysisCommand(const std::vector<std::string>& args);
    bool metricsCommand(const std::vector<std::string>& args);
    bool outputCommand(const std::vector<std::string>& args);
    bool exportCommand(const std::vector<std::string>& args);
    
    // Experiment management
    bool listCommand(const std::vector<std::string>& args);
    bool infoCommand(const std::vector<std::string>& args);
    bool compareCommand(const std::vector<std::string>& args);
    bool snapshotCommand(const std::vector<std::string>& args);
    bool revertCommand(const std::vector<std::string>& args);
    
    // Batch and parallel experiments
    bool batchCommand(const std::vector<std::string>& args);
    bool parallelCommand(const std::vector<std::string>& args);
    bool sweepCommand(const std::vector<std::string>& args);
    bool gridSearchCommand(const std::vector<std::string>& args);
    
    // Experiment automation
    bool scriptCommand(const std::vector<std::string>& args);
    bool automateCommand(const std::vector<std::string>& args);
    bool scheduleCommand(const std::vector<std::string>& args);
    bool queueCommand(const std::vector<std::string>& args);
    
    // Help and documentation
    void showHelp() const;
    void showCommandHelp(const std::string& command) const;
    
    // Integration
    void setConfig(std::shared_ptr<Config> config) { this->config = config; }
    void setBrain(std::shared_ptr<Brain> brain) { this->brain = brain; }
    void setCLI(CLI* cli) { this->cli = cli; }
    
    // State
    bool isInitialized() const { return initialized; }
    bool isRunning() const { return running; }
    bool isPaused() const { return paused; }
    
    // Experiment control
    void startExperiment() { running = true; }
    void stopExperiment() { running = false; }
    void pauseExperiment() { paused = true; }
    void resumeExperiment() { paused = false; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool initialized;
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    CLI* cli;
    
    // Experiment state
    bool running;
    bool paused;
    bool batchMode;
    bool parallelMode;
    bool scriptMode;
    std::string currentExperiment;
    std::vector<std::string> experimentQueue;
    std::chrono::steady_clock::time_point experimentStartTime;
    
    // Experiment configuration
    struct ExperimentConfig {
        std::string name;
        std::string description;
        std::vector<std::string> parameters;
        std::vector<float> parameterValues;
        std::string inputPattern;
        std::string outputCapture;
        size_t duration;
        size_t steps;
        double timestep;
        bool enabledPlasticity;
        bool enabledSTDP;
        bool enabledStructuralPlasticity;
        double learningRate;
        double threshold;
        std::vector<std::string> results;
        std::vector<std::string> metrics;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
    } currentConfig;
    
    // Batch experiment configuration
    struct BatchConfig {
        std::string name;
        std::vector<ExperimentConfig> experiments;
        bool parallel;
        size_t maxConcurrent;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
        std::vector<std::string> results;
    } batchConfig;
    
    // Helper methods
    bool parseExperimentParameters(const std::vector<std::string>& args, ExperimentConfig& config) const;
    bool parsePatternParameters(const std::vector<std::string>& args, std::string& pattern) const;
    bool parseOutputParameters(const std::vector<std::string>& args, std::string& output) const;
    bool loadExperimentFromFile(const std::string& filename, ExperimentConfig& config);
    bool saveExperimentToFile(const std::string& filename, const ExperimentConfig& config);
    bool runSingleExperiment(const ExperimentConfig& config);
    bool runBatchExperiment(const BatchConfig& config);
    bool runParallelExperiments(const std::vector<ExperimentConfig>& experiments);
    bool executePattern(const std::string& pattern, const ExperimentConfig& config);
    bool captureOutput(const ExperimentConfig& config);
    bool analyzeExperimentResults(const ExperimentConfig& config);
    bool exportExperimentResults(const ExperimentConfig& config, const std::string& format = "text");
    void printExperimentHeader(const std::string& title) const;
    void printExperimentConfig(const ExperimentConfig& config) const;
    void printExperimentResults(const ExperimentConfig& config) const;
    void printBatchResults(const BatchConfig& config) const;
    void printPerformanceHeader() const;
    void printPatternHeader() const;
    void printOutputHeader() const;
    void printAnalysisHeader() const;
    void displayExperimentResults(const ExperimentConfig& config, const std::string& format = "text") const;
    std::string formatExperimentResults(const ExperimentConfig& config, const std::string& format = "text") const;
    std::string formatExperimentAsText(const ExperimentConfig& config) const;
    std::string formatExperimentAsJSON(const ExperimentConfig& config) const;
    std::string formatExperimentAsCSV(const ExperimentConfig& config) const;
    std::string formatExperimentAsHTML(const ExperimentConfig& config) const;
    std::string getExperimentStatusString(const ExperimentConfig& config) const;
    std::string getPatternTypeString(const std::string& pattern) const;
    std::string getOutputFormatString(const std::string& output) const;
    std::string getMetricString(const std::string& metric) const;
    bool validateExperimentParameters(const std::vector<std::string>& args) const;
    void setupExperimentConfiguration() const;
    bool createDefaultExperimentConfig(ExperimentConfig& config) const;
    bool validatePattern(const std::string& pattern) const;
    bool validateOutput(const std::string& output) const;
    bool validateMetric(const std::string& metric) const;
    bool isExperimentInProgress(const ExperimentConfig& config) const;
    bool saveExperimentSnapshot(const std::string& filename, const ExperimentConfig& config);
    bool loadExperimentSnapshot(const std::string& filename, ExperimentConfig& config);
    std::string getExperimentFilename(const std::string& name) const;
    std::string getExperimentResultsFilename(const std::string& experimentName, const std::string& format) const;
    bool ensureExperimentDirectory() const;
    std::vector<std::string> getAvailablePatterns() const;
    std::vector<std::string> getAvailableMetrics() const;
    std::vector<std::string> getAvailableOutputs() const;
    bool validateFilePath(const std::string& path) const;
    bool createDefaultExperiment(const std::string& name);
    bool cloneExperiment(const std::string& source, const std::string& target);
    bool deleteExperiment(const std::string& name);
    bool exportExperimentToArchive(const std::string& name, const std::string& archiveFile);
    bool importExperimentFromArchive(const std::string& archiveFile);
};
