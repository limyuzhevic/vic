#pragma once

#include "Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <memory>
#include <vector>
#include <string>
#include <functional>
#include <chrono>
#include <map>
#include <algorithm>

namespace nlm {

// Experiment status enumeration for better experiment lifecycle management
enum class ExperimentStatus {
    NotStarted,     // Experiment hasn't begun yet
    Running,        // Experiment is currently executing
    Completed,      // Experiment finished successfully
    Failed,         // Experiment failed with an error
    Paused,         // Experiment is paused
    Cancelled       // Experiment was cancelled
};

// Experiment metadata for better organization and tracking
struct ExperimentMetadata {
    std::string name;                        // Human-readable name
    std::string description;                  // Detailed description
    std::string version;                      // Version number
    std::string author;                       // Author name
    std::chrono::system_clock::time_point created;    // When experiment was created
    std::chrono::system_clock::time_point modified;   // When experiment was last modified
    std::map<std::string, std::string> tags;   // Key-value tags for categorization
    std::map<std::string, std::string> parameters;   // Experiment-specific parameters
    
    ExperimentMetadata() 
        : created(std::chrono::system_clock::now()), 
          modified(std::chrono::system_clock::now()) {}
};

// Experiment progress tracking for real-time monitoring
struct ExperimentProgress {
    float percentage;                         // Completion percentage (0.0 to 1.0)
    size_t currentStep;                       // Current simulation step
    size_t totalSteps;                        // Total simulation steps
    std::string currentPhase;                 // Current phase of execution
    std::chrono::system_clock::time_point startTime; // When execution started
    std::chrono::system_clock::time_point lastUpdate; // When progress was last updated
    std::string statusMessage;                // Human-readable status message
    
    ExperimentProgress() 
        : percentage(0.0f), currentStep(0), totalSteps(0),
          startTime(std::chrono::system_clock::now()),
          lastUpdate(std::chrono::system_clock::now()) {}
};

// Advanced experiment configuration with enhanced settings
struct AdvancedExperimentConfig {
    // Core settings
    size_t maxSteps;                          // Maximum simulation steps
    double timestep;                          // Time step duration
    uint64_t randomSeed;                      // Random seed for reproducibility
    bool enableLogging;                       // Enable detailed logging
    std::string logFile;                      // Log file path
    
    // Performance settings
    size_t parallelThreads;                   // Number of parallel threads
    size_t memoryLimit;                       // Memory limit in MB
    size_t cpuAffinity;                       // CPU core affinity
    
    // Monitoring settings
    bool enableProgressReporting;             // Enable real-time progress reporting
    double progressReportInterval;            // Progress report interval in seconds
    bool enableMetricsCollection;             // Enable metrics collection
    std::vector<std::string> metricsToCollect; // Metrics to collect
    
    // Output settings
    bool saveIntermediateResults;             // Save intermediate results
    std::string resultsFormat;                // Results file format (JSON, CSV, etc.)
    std::string resultsPath;                  // Results output path
    
    // Error handling
    bool continueOnError;                    // Continue execution on errors
    size_t maxRetries;                        // Maximum retry attempts
    double retryDelay;                        // Delay between retries
    
    // Resource management
    bool enableResourceMonitoring;            // Monitor system resources
    double maxCpuUsage;                       // Maximum CPU usage percentage
    double maxMemoryUsage;                    // Maximum memory usage percentage
    
    AdvancedExperimentConfig()
        : maxSteps(1000), timestep(0.001), randomSeed(42),
          enableLogging(true), logFile("experiment.log"),
          parallelThreads(1), memoryLimit(1024), cpuAffinity(0),
          enableProgressReporting(true), progressReportInterval(1.0),
          enableMetricsCollection(true), enableResourceMonitoring(true),
          maxCpuUsage(90.0), maxMemoryUsage(90.0),
          continueOnError(true), maxRetries(3), retryDelay(1.0) {}
};

// ExperimentRunner: Advanced experiment runner with enhanced features
class ExperimentRunner {
public:
    ExperimentRunner();
    ~ExperimentRunner();
    
    // Create new experiment with metadata
    std::shared_ptr<Experiment> createExperiment(
        const std::string& name,
        const std::string& description = "",
        const std::map<std::string, std::string>& tags = {});
    
    // Create experiment with advanced configuration
    std::shared_ptr<Experiment> createExperiment(
        const std::string& name,
        const AdvancedExperimentConfig& config,
        const ExperimentMetadata& metadata = ExperimentMetadata());
    
    // Run single experiment with enhanced error handling and logging
    bool runExperiment(std::shared_ptr<Experiment> experiment,
                      std::shared_ptr<Brain> brain,
                      std::shared_ptr<Environment> environment,
                      SimulationStep maxSteps,
                      const AdvancedExperimentConfig& config = AdvancedExperimentConfig());
    
    // Run experiment with progress callback
    bool runExperimentWithProgress(std::shared_ptr<Experiment> experiment,
                                  std::shared_ptr<Brain> brain,
                                  std::shared_ptr<Environment> environment,
                                  SimulationStep maxSteps,
                                  const std::function<void(const ExperimentProgress&)>& progressCallback,
                                  const AdvancedExperimentConfig& config = AdvancedExperimentConfig());
    
    // Run batch of experiments with advanced scheduling
    std::vector<std::shared_ptr<Experiment>> runBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps,
        const AdvancedExperimentConfig& config = AdvancedExperimentConfig());
    
    // Get experiment by name (case-insensitive)
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const;
    
    // Get experiments by tag
    std::vector<std::shared_ptr<Experiment>> getExperimentsByTag(const std::string& tag) const;
    
    // Get all experiments
    const std::vector<std::shared_ptr<Experiment>>& getExperiments() const;
    
    // Get experiment metadata by name
    ExperimentMetadata getExperimentMetadata(const std::string& name) const;
    
    // Update experiment metadata
    bool updateExperimentMetadata(const std::string& name, const ExperimentMetadata& metadata);
    
    // Clear all experiments
    void clearExperiments();
    
    // Set random seed for reproducibility
    void setGlobalSeed(uint64_t seed);
    
    // Get global random seed
    uint64_t getGlobalSeed() const;
    
    // Save experiment results with enhanced format
    bool saveResults(const std::string& filepath, bool includeMetadata = true) const;
    
    // Load experiment results
    bool loadResults(const std::string& filepath);
    
    // Export results to different formats
    bool exportResults(const std::string& filepath, const std::string& format = "json") const;
    
    // Get experiment statistics
    struct ExperimentStats {
        size_t totalExperiments;
        size_t completedExperiments;
        size_t failedExperiments;
        size_t averageDuration;
        double successRate;
        std::map<std::string, size_t> experimentsByTag;
    };
    
    ExperimentStats getStats() const;
    
    // Advanced experiment search and filtering
    std::vector<std::shared_ptr<Experiment>> searchExperiments(
        const std::string& namePattern = "",
        const std::map<std::string, std::string>& tags = {}) const;
    
    // Experiment comparison utility
    static double compareExperiments(std::shared_ptr<Experiment> exp1, std::shared_ptr<Experiment> exp2);
    
    // Get current experiments directory
    static std::string getExperimentsDirectory();
    
    // Set experiments directory
    static void setExperimentsDirectory(const std::string& directory);
    
    // Validate experiment configuration
    static bool validateExperimentConfig(const AdvancedExperimentConfig& config);
    
    // Get default experiment configuration
    static AdvancedExperimentConfig getDefaultConfig();
    
    // Get list of supported experiment formats
    static std::vector<std::string> getSupportedFormats();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
