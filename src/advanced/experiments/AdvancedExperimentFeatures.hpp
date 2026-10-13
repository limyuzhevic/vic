#pragma once

#include <memory>
#include <string>
#include <map>
#include <vector>
#include "../core/Config/Config.hpp"
#include "../experiments/ExperimentRunner.hpp"
#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "SimulationStep"

namespace nlm {

// Parameter sweep definition
struct ParameterSweep {
    std::string name;
    std::map<std::string, std::vector<std::string>> parameters;
    std::string outputFile;
    SimulationStep maxSteps;
    bool parallel;
    
    ParameterSweep() : maxSteps(1000), parallel(false) {}
};

// Experiment configuration template
struct ExperimentTemplate {
    std::string name;
    std::map<std::string, std::string> config;
    std::vector<std::string> metrics;
    SimulationStep duration;
    
    ExperimentTemplate() : duration(1000) {}
};

// Batch run configuration
struct BatchRunConfig {
    std::string id;
    std::vector<ExperimentTemplate> experiments;
    std::string outputDirectory;
    bool parallel;
    bool resume;
    SimulationStep maxSteps;
    
    BatchRunConfig() : parallel(false), resume(false), maxSteps(1000) {}
};

// Advanced experiment features
class AdvancedExperimentFeatures {
public:
    AdvancedExperimentFeatures();
    ~AdvancedExperimentFeatures();
    
    // Custom experiment templates
    bool createExperimentTemplate(const std::string& name,
                                  const std::map<std::string, std::string>& config,
                                  const std::vector<std::string>& metrics,
                                  SimulationStep duration);
    
    bool loadExperimentTemplate(const std::string& filepath);
    bool saveExperimentTemplate(const std::string& filepath) const;
    
    std::shared_ptr<ExperimentTemplate> getExperimentTemplate(const std::string& name) const;
    std::vector<std::string> getExperimentTemplateNames() const;
    
    // Experiment parameter optimization
    bool optimizeParameters(const std::string& templateName,
                           const std::map<std::string, std::vector<double>>& parameterRanges,
                           const std::string& optimizationObjective,
                           const std::map<std::string, std::string>& constraints);
    
    // Cross-experiment result analysis
    bool analyzeCrossExperiments(const std::vector<std::string>& experimentFiles,
                                const std::vector<std::string>& commonMetrics,
                                const std::string& analysisType,
                                const std::string& outputFile);
    
    // Statistical analysis tools
    bool performStatisticalAnalysis(const std::vector<double>& data,
                                   const std::string& analysisType,
                                   const std::string& outputFile);
    
    // Configuration comparison tools
    bool compareConfigurations(const std::string& configFile1,
                             const std::string& configFile2,
                             const std::string& outputFile,
                             const std::vector<std::string>& sectionsToCompare = {});
    
    // Configuration inheritance and merging
    bool mergeConfigurations(const std::vector<std::string>& configFiles,
                           const std::string& outputFile,
                           bool mergeInOrder = true);
    
    bool validateConfiguration(const std::string& configFile,
                              const std::string& schemaFile = "",
                              const std::string& outputFile = "");
    
    // Configuration validation schemas
    bool createValidationSchema(const std::string& schemaName,
                               const std::map<std::string, std::string>& schema,
                               const std::string& description = "");
    
    bool loadValidationSchema(const std::string& filepath);
    bool saveValidationSchema(const std::string& filepath) const;
    
    bool validateConfigurationAgainstSchema(const std::string& configFile,
                                          const std::string& schemaName);
    
    // Batch processing
    bool createParameterSweep(const ParameterSweep& sweep);
    bool executeParameterSweep(const std::string& sweepName,
                              const std::string& outputDirectory,
                              bool parallel = false);
    
    bool createBatchRun(const BatchRunConfig& config);
    bool executeBatchRun(const std::string& batchId,
                        const std::map<std::string, std::string>& options = {});
    
    bool monitorBatchProgress(const std::string& batchId,
                            const std::string& outputFile);
    
    // Result aggregation and analysis
    bool aggregateResults(const std::vector<std::string>& resultFiles,
                         const std::string& outputFile,
                         const std::vector<std::string>& metrics = {});
    
    bool analyzeSweepResults(const std::string& sweepName,
                           const std::string& outputFile,
                           const std::vector<std::string>& analysisTypes = {});
    
    // Parallel experiment execution
    bool parallelExecute(const std::vector<std::shared_ptr<ExperimentTemplate>>& experiments,
                        const std::string& outputDirectory,
                        size_t maxParallel = std::thread::hardware_concurrency());
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::shared_ptr<Config> createConfigFromTemplate(const ExperimentTemplate& template);
    void logAnalysisResult(const std::string& analysisType,
                         const std::map<std::string, double>& results);
};

} // namespace nlm
