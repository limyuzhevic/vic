#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <memory>
#include <algorithm>
#include <functional>

namespace nlm {

// Experiment execution and results collection
class ExperimentRunner {
public:
    ExperimentRunner();
    ~ExperimentRunner();
    
    // Disable copying
    ExperimentRunner(const ExperimentRunner&) = delete;
    ExperimentRunner& operator=(const ExperimentRunner&) = delete;
    
    // Experiment configuration
    struct ExperimentConfig {
        std::string name;
        std::string description;
        std::map<std::string, std::string> parameters;
        bool enabled;
        uint64_t maxSteps;
        double timestep;
        std::string outputDirectory;
        
        ExperimentConfig()
            : enabled(true), maxSteps(1000), timestep(0.001) {}
        
        ExperimentConfig(const std::string& n, const std::string& desc)
            : name(n), description(desc), enabled(true), maxSteps(1000), timestep(0.001) {}
    };
    
    // Experiment result
    struct ExperimentResult {
        std::string experimentName;
        bool success;
        std::chrono::high_resolution_clock::duration executionTime;
        std::map<std::string, double> metrics;
        std::vector<std::string> messages;
        std::map<std::string, std::string> metadata;
        std::chrono::system_clock::time_point timestamp;
        
        ExperimentResult() : success(false), executionTime(0), timestamp(std::chrono::system_clock::now()) {}
    };
    
    // Experiment status
    enum class ExperimentStatus {
        PENDING,
        RUNNING,
        COMPLETED,
        FAILED,
        CANCELLED
    };
    
    // Experiment execution interface
    using ExperimentStepCallback = std::function<bool(uint64_t step, const std::map<std::string, std::string>& context)>;
    
    // Initialize experiment runner
    bool initialize();
    
    // Add experiment
    bool addExperiment(const ExperimentConfig& config);
    bool addExperiment(const std::string& name, const std::string& description,
                      const std::map<std::string, std::string>& parameters);
    
    // Remove experiment
    bool removeExperiment(const std::string& name);
    void removeAllExperiments();
    
    // Get experiments
    std::vector<ExperimentConfig> getExperiments() const;
    ExperimentConfig getExperiment(const std::string& name) const;
    bool hasExperiment(const std::string& name) const;
    
    // Experiment execution
    bool runExperiment(const std::string& name);
    bool runExperiment(const ExperimentConfig& config);
    bool runAllExperiments();
    
    // Experiment control
    void pauseExperiment(const std::string& name);
    void resumeExperiment(const std::string& name);
    void stopExperiment(const std::string& name);
    void cancelExperiment(const std::string& name);
    
    // Experiment status
    ExperimentStatus getExperimentStatus(const std::string& name) const;
    bool isExperimentRunning(const std::string& name) const;
    bool isExperimentCompleted(const std::string& name) const;
    
    // Experiment results
    ExperimentResult getExperimentResult(const std::string& name) const;
    std::vector<ExperimentResult> getAllExperimentResults() const;
    std::vector<ExperimentResult> getCompletedExperimentResults() const;
    
    // Result filtering
    std::vector<ExperimentResult> getExperimentResultsByStatus(ExperimentStatus status) const;
    std::vector<ExperimentResult> getExperimentResultsByMetric(const std::string& metric, double threshold) const;
    
    // Result statistics
    struct ExperimentStatistics {
        size_t totalExperiments;
        size_t completedExperiments;
        size_t failedExperiments;
        size_t runningExperiments;
        size_t pendingExperiments;
        double averageExecutionTime;
        double successRate;
        std::map<std::string, double> averageMetrics;
        
        ExperimentStatistics()
            : totalExperiments(0), completedExperiments(0), failedExperiments(0),
              runningExperiments(0), pendingExperiments(0), averageExecutionTime(0.0),
              successRate(0.0) {}
    };
    
    ExperimentStatistics getStatistics() const;
    
    // Result analysis
    std::map<std::string, double> analyzeResults(const std::vector<ExperimentResult>& results) const;
    std::string generateResultsReport(const std::vector<ExperimentResult>& results) const;
    
    // Result export
    bool exportResults(const std::string& filename, const std::vector<ExperimentResult>& results) const;
    bool exportResultsJSON(const std::string& filename, const std::vector<ExperimentResult>& results) const;
    bool exportResultsCSV(const std::string& filename, const std::vector<ExperimentResult>& results) const;
    
    // Result import
    bool importResults(const std::string& filename, std::vector<ExperimentResult>& results);
    
    // Result management
    void clearResults();
    void clearCompletedResults();
    void deleteExperimentResults(const std::string& name);
    
    // Experiment callbacks
    using ExperimentStartCallback = std::function<void(const std::string& name, const ExperimentConfig& config)>;
    using ExperimentCompleteCallback = std::function<void(const std::string& name, const ExperimentResult& result)>;
    using ExperimentFailCallback = std::function<void(const std::string& name, const std::string& error)>;
    
    void setStartCallback(const ExperimentStartCallback& callback);
    void setCompleteCallback(const ExperimentCompleteCallback& callback);
    void setFailCallback(const ExperimentFailCallback& callback);
    
    // Experiment step callback
    void setStepCallback(const ExperimentStepCallback& callback);
    
    // Experiment error handling
    using ExperimentErrorHandler = std::function<void(const std::string& name, const std::string& error)>;
    
    void setErrorHandler(const ExperimentErrorHandler& handler);
    
    // Experiment validation
    bool validateExperiment(const ExperimentConfig& config) const;
    std::vector<std::string> getExperimentValidationErrors(const ExperimentConfig& config) const;
    
    // Experiment configuration templates
    struct ExperimentTemplate {
        std::string name;
        std::string description;
        ExperimentConfig config;
        
        ExperimentTemplate() = default;
        ExperimentTemplate(const std::string& n, const std::string& desc,
                          const ExperimentConfig& c)
            : name(n), description(desc), config(c) {}
    };
    
    // Get available experiment templates
    std::vector<ExperimentTemplate> getAvailableExperimentTemplates() const;
    
    // Load experiment configuration from file
    bool loadExperimentConfiguration(const std::string& filepath);
    
    // Save experiment configuration to file
    bool saveExperimentConfiguration(const std::string& filepath) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Experiment execution helpers
    bool executeExperimentSteps(const std::string& name, const ExperimentConfig& config);
    bool executeSingleStep(const std::string& name, uint64_t step, 
                          const std::map<std::string, std::string>& context);
    
    // Result calculation helpers
    void calculateExperimentMetrics(ExperimentResult& result) const;
    void updateExperimentStatistics();
    
    // Experiment lifecycle management
    void onExperimentStarted(const std::string& name, const ExperimentConfig& config);
    void onExperimentCompleted(const std::string& name, const ExperimentResult& result);
    void onExperimentFailed(const std::string& name, const std::string& error);
    void onExperimentCancelled(const std::string& name);
    
    // Result export helpers
    std::string experimentResultToJSON(const ExperimentResult& result) const;
    std::string experimentResultToCSV(const ExperimentResult& result) const;
    
    // Configuration template helpers
    void loadDefaultExperimentTemplates();
    
    // Statistics helpers
    void updateGlobalStatistics();
    
    // Error handling helpers
    void handleExperimentError(const std::string& name, const std::string& error);
    
    // Validation helpers
    bool validateExperimentConfig(const ExperimentConfig& config) const;
}; // namespace nlm

// Global experiment runner instance
ExperimentRunner& getExperimentRunner();