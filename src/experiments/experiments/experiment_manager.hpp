#pragma once

#include "enhanced_experiments.hpp"
#include "experiment_factory.hpp"
#include "parameter_sweep.hpp"
#include "result_analyzer.hpp"
#include "experiment_scheduler.hpp"
#include "reproducibility_manager.hpp"
#include "experiment_utils.hpp"
#include "parameter_parser.hpp"
#include "result_exporter.hpp"
#include "legacy_experiment.hpp"
#include "legacy_runner.hpp"
#include "../core/Config/Config.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <mutex>

namespace nlm {

// Main experiment management system
class ExperimentManager {
public:
    ExperimentManager(std::shared_ptr<Config> config = nullptr);
    ~ExperimentManager() = default;

    // Experiment management
    std::shared_ptr<BaseExperiment> createExperiment(const std::string& type, const std::string& name);
    std::shared_ptr<BaseExperiment> createExperimentFromTemplate(
        const std::string& templateName,
        const std::map<std::string, std::string>& parameters);
    
    std::vector<std::string> getAvailableExperimentTypes() const;
    std::vector<std::string> getExperimentTemplates() const;

    // Experiment execution
    bool runExperiment(const std::string& experimentName);
    bool runExperimentToCompletion(const std::string& experimentName, size_t maxSteps);
    bool pauseExperiment(const std::string& experimentName);
    bool resumeExperiment(const std::string& experimentName);
    bool stopExperiment(const std::string& experimentName);

    // Experiment management
    bool deleteExperiment(const std::string& experimentName);
    bool renameExperiment(const std::string& oldName, const std::string& newName);
    
    std::shared_ptr<BaseExperiment> getExperiment(const std::string& experimentName) const;
    const std::vector<std::shared_ptr<BaseExperiment>>& getAllExperiments() const;
    void clearExperiments();

    // Parameter sweep
    std::vector<SweepResult> runParameterSweep(const std::string& templateName,
                                              const std::map<std::string, std::vector<std::string>>& parameterMatrix,
                                              size_t numIterations = 1);
    
    OptimizationResult optimizeParameters(const std::string& templateName,
                                         const std::map<std::string, std::vector<std::string>>& parameterSpace,
                                         const std::string& objective = "performance");

    // Results analysis
    ResultAnalyzer::ExperimentStatistics analyzeExperiment(const std::string& experimentName) const;
    ResultAnalyzer::ComparisonResult compareExperiments(const std::vector<std::string>& experimentNames) const;
    std::string generateReport(const std::vector<std::string>& experimentNames, const std::string& format = "markdown") const;

    // Scheduling
    void startScheduler();
    void pauseScheduler();
    void stopScheduler();
    bool isSchedulerRunning() const;
    
    ExperimentScheduler::QueueStatus getSchedulerStatus() const;
    std::shared_ptr<BaseExperiment> getCurrentExperiment() const;
    std::vector<std::shared_ptr<BaseExperiment>> getCompletedExperiments() const;

    // Reproducibility
    ReproducibilityManager::ExperimentSnapshot createSnapshot(const std::string& experimentName);
    bool restoreSnapshot(const ReproducibilityManager::ExperimentSnapshot& snapshot);
    std::string createVersion(const std::string& experimentName, const std::string& description);
    ReproducibilityManager::VersionComparison compareVersions(const std::string& experimentName,
                                                             const std::string& version1,
                                                             const std::string& version2);
    std::vector<std::string> getVersionHistory(const std::string& experimentName) const;

    // Results export
    bool exportExperimentResults(const std::string& experimentName, const std::string& filepath, const std::string& format = "csv");
    bool exportBatchResults(const std::vector<std::string>& experimentNames, const std::string& outputDirectory, const std::string& format = "csv");

    // Configuration
    void setConfig(std::shared_ptr<Config> config) { config_ = config; }
    std::shared_ptr<Config> getConfig() const { return config_; }

    // Statistics and monitoring
    struct ManagerStatistics {
        size_t totalExperimentsCreated;
        size_t totalExperimentsCompleted;
        size_t totalExperimentsFailed;
        double averageExperimentPerformance;
        std::chrono::duration<double> totalExecutionTime;
        std::chrono::duration<double> uptime;
    };

    ManagerStatistics getStatistics() const;

    // Legacy compatibility
    std::shared_ptr<LegacyExperiment> createLegacyExperiment(const std::string& name);
    bool runLegacyExperiment(std::shared_ptr<LegacyExperiment> experiment,
                            std::shared_ptr<Brain> brain,
                            std::shared_ptr<Environment> environment,
                            SimulationStep maxSteps);
    
    // Migration
    bool migrateLegacyExperiments();
    std::vector<std::string> getLegacyExperimentNames() const;

private:
    void initializeComponents();
    bool loadExperimentConfigurations();
    void cleanupCompletedExperiments();

    std::shared_ptr<Config> config_;
    
    // Component instances
    std::unique_ptr<ExperimentFactory> experimentFactory_;
    std::unique_ptr<ParameterSweepManager> parameterSweepManager_;
    std::unique_ptr<ResultAnalyzer> resultAnalyzer_;
    std::unique_ptr<ExperimentScheduler> experimentScheduler_;
    std::unique_ptr<ReproducibilityManager> reproducibilityManager_;
    std::unique_ptr<ExperimentUtils> experimentUtils_;
    std::unique_ptr<ParameterParser> parameterParser_;
    std::unique_ptr<ResultExporter> resultExporter_;
    std::unique_ptr<LegacyExperimentRunner> legacyRunner_;
    
    // Experiment storage
    std::vector<std::shared_ptr<BaseExperiment>> experiments_;
    mutable std::mutex experimentsMutex_;
    
    // Statistics
    mutable ManagerStatistics stats_;
    mutable std::mutex statsMutex_;
    
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime_;
};

} // namespace nlm