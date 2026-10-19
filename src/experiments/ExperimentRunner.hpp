#pragma once

#include "Experiment.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include <memory>
#include <vector>

namespace nlm {

// ExperimentRunner: Runs experiments and collects data
class ExperimentRunner {
public:
    ExperimentRunner();
    ~ExperimentRunner();
    
    // Create new experiment
    std::shared_ptr<Experiment> createExperiment(const std::string& name);
    
    // Run single experiment
    bool runExperiment(std::shared_ptr<Experiment> experiment,
                      std::shared_ptr<Brain> brain,
                      std::shared_ptr<Environment> environment,
                      SimulationStep maxSteps);
    
    // Run batch of experiments
    std::vector<std::shared_ptr<Experiment>> runBatch(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps);
    
    // Get all experiments
    const std::vector<std::shared_ptr<Experiment>>& getExperiments() const;
    
    // Get experiment by name
    std::shared_ptr<Experiment> getExperiment(const std::string& name) const;
    
    // Clear all experiments
    void clearExperiments();
    
    // Set random seed for reproducibility
    void setGlobalSeed(uint64_t seed);
    
    // Advanced experiment management features
    
    // Create experiment with specific configuration
    std::shared_ptr<Experiment> createExperiment(const std::string& name,
                                                 const std::shared_ptr<Config>& config);
    
    // Export experiment results to various formats
    bool exportResults(const std::string& filepath, ExportFormat format = ExportFormat::JSON) const;
    
    // Compare experiments
    std::vector<std::pair<std::shared_ptr<Experiment>, std::shared_ptr<Experiment>>>
        compareExperiments(const std::vector<std::shared_ptr<Experiment>>& experiments,
                          const std::string& metric) const;
    
    // Statistical analysis
    struct ExperimentStats {
        double mean;
        double stdDev;
        double min;
        double max;
        double median;
        size_t count;
    };
    
    ExperimentStats computeStatistics(const std::vector<std::shared_ptr<Experiment>>& experiments,
                                     const std::string& metric) const;
    
    // Experiment filtering and selection
    std::vector<std::shared_ptr<Experiment>> filterExperiments(
        const std::function<bool(const std::shared_ptr<Experiment>&)>& predicate) const;
    
    // Best experiment selection
    std::shared_ptr<Experiment> getBestExperiment(const std::string& metric, bool higherIsBetter = true) const;
    
    // Experiment validation
    bool validateExperiment(const std::shared_ptr<Experiment>& experiment) const;
    
    // Progress tracking
    void setProgressCallback(std::function<void(double)> callback);
    
    // Advanced run options
    struct RunOptions {
        SimulationStep maxSteps = 1000;
        bool recordDetailed = true;
        bool saveIntermediate = false;
        std::string outputDirectory = "./experiments";
        double convergenceThreshold = 0.01;
        size_t maxMemoryUsage = 1024 * 1024 * 1024;  // 1GB
        bool useGPUAcceleration = false;
        bool parallelExecution = false;
    };
    
    // Run with advanced options
    std::vector<std::shared_ptr<Experiment>> runBatchAdvanced(
        const std::vector<std::shared_ptr<Experiment>>& experiments,
        std::shared_ptr<Brain> brainTemplate,
        std::shared_ptr<Environment> environmentTemplate,
        const RunOptions& options);
    
    // Results analysis and reporting
    std::string generateReport(const std::vector<std::shared_ptr<Experiment>>& experiments) const;
    std::string generateComparisonReport(const std::vector<std::shared_ptr<Experiment>>& experiments,
                                         const std::vector<std::string>& metrics) const;
    
    // Backup and restore
    bool backupExperiments(const std::string& backupPath) const;
    bool restoreExperiments(const std::string& backupPath);
    
    // Version control integration
    void setVersionControlEnabled(bool enabled);
    bool commitExperimentResults(const std::string& message);
    bool restoreExperimentFromVersion(const std::string& versionHash);
    
    // Resource monitoring
    struct ResourceUsage {
        size_t memoryUsage;
        double cpuUsage;
        double gpuUsage;
        double diskIO;
        double networkIO;
    };
    
    ResourceUsage getResourceUsage() const;
    
    // Performance profiling
    void enableProfiling(bool enable);
    std::map<std::string, double> getProfileData(const std::shared_ptr<Experiment>& experiment) const;
    
    // Advanced visualization
    bool generateVisualization(const std::string& experimentName, const std::string& outputPath,
                               VisualizationType type = VisualizationType::NEURAL_NETWORK);
    
    // Machine learning integration
    void setMLModelEnabled(bool enabled);
    bool predictOutcome(const std::shared_ptr<Experiment>& experiment, double& prediction);
    bool trainModel(const std::vector<std::shared_ptr<Experiment>>& trainingData);
    
    // Error handling
    enum class ExperimentError {
        SUCCESS,
        NOT_FOUND,
        INVALID_CONFIGURATION,
        MEMORY_LIMIT_EXCEEDED,
        INVALID_PARAMETER,
        VERSION_CONTROL_ERROR,
        EXPORT_FORMAT_ERROR,
        VISUALIZATION_ERROR,
        ML_MODEL_ERROR
    };
    
    ExperimentError getLastError() const;
    std::string getErrorMessage(ExperimentError error) const;

} // namespace nlm
