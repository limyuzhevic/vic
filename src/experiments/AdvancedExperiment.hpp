// Advanced Experiment Class Header
#pragma once

#include "AdvancedExperimentConfig.hpp"

namespace nlm {

class AdvancedExperimentRunner;

class AdvancedExperiment {
public:
    AdvancedExperiment();
    AdvancedExperiment(const std::string& name, const std::string& description = "");
    ~AdvancedExperiment();
    
    // Rule of five
    AdvancedExperiment(const AdvancedExperiment& other);
    AdvancedExperiment& operator=(const AdvancedExperiment& other);
    AdvancedExperiment(AdvancedExperiment&& other) noexcept;
    AdvancedExperiment& operator=(AdvancedExperiment&& other) noexcept;
    
    // Configuration management
    void setConfig(const ExperimentConfig& config);
    const ExperimentConfig& getConfig() const { return config; }
    
    // Parameter management
    void addParameter(const Parameter& param);
    bool updateParameter(const std::string& name, double value);
    double getParameter(const std::string& name) const;
    const std::vector<Parameter>& getParameters() const { return config.parameters; }
    
    // Dynamic parameter adjustment
    void registerParameterCallback(const std::string& paramName,
                                   std::function<void(double)> callback);
    void triggerParameterUpdate(const std::string& paramName, double value);
    
    // Experiment execution
    bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment,
             SimulationStep maxSteps, std::function<bool(uint64_t step, double progress)> progressCallback = nullptr);
    
    // Parameter sweep capabilities
    std::vector<std::shared_ptr<AdvancedExperiment>> generateSweepConfigurations(const std::string& sweepType,
                                                                                  size_t numCombinations = 10);
    std::vector<std::shared_ptr<AdvancedExperiment>> generateGridSearch(const std::vector<std::string>& paramNames,
                                                                        const std::vector<std::vector<double>>& paramValues);
    std::vector<std::shared_ptr<AdvancedExperiment>> generateRandomSearch(size_t numCombinations);
    
    // Experiment metadata
    void setVersion(const std::string& version) { config.version = version; }
    void setAuthor(const std::string& author) { config.author = author; }
    void setDescription(const std::string& desc) { config.description = desc; }
    std::string getAuthor() const { return config.author; }
    std::string getDescription() const { return config.description; }
    std::string getVersion() const { return config.version; }
    
    // Checkpoint and recovery
    bool saveCheckpoint(const std::string& filepath) const;
    bool loadCheckpoint(const std::string& filepath);
    
    // Configuration templates
    static ExperimentConfig createTemplate(const std::string& type,
                                           const std::unordered_map<std::string, double>& baseValues = {});
    static std::vector<ExperimentConfig> getAvailableTemplates();
    
    // Comparison with other experiments
    double compareWith(const AdvancedExperiment& other) const;
    
    // Export and import
    std::string exportToJSON() const;
    bool importFromJSON(const std::string& jsonStr);
    
    // Get experiment ID
    const std::string& getId() const { return id; }
    
    // Friend class for internal access
    friend class AdvancedExperimentRunner;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string id;
    ExperimentConfig config;
    
    // Generate unique ID
    static std::string generateId();
};

class AdvancedExperimentRunner {
public:
    AdvancedExperimentRunner();
    ~AdvancedExperimentRunner();
    
    // Rule of five
    AdvancedExperimentRunner(const AdvancedExperimentRunner&) = delete;
    AdvancedExperimentRunner& operator=(const AdvancedExperimentRunner&) = delete;
    AdvancedExperimentRunner(AdvancedExperimentRunner&&) = delete;
    AdvancedExperimentRunner& operator=(AdvancedExperimentRunner&&) = delete;
    
    // Create advanced experiment
    std::shared_ptr<AdvancedExperiment> createExperiment(const std::string& name, const std::string& description = "");
    
    // Run experiments with advanced options
    std::vector<std::shared_ptr<AdvancedExperiment>> runBatch(const std::vector<std::shared_ptr<AdvancedExperiment>>& experiments,
                                                               std::shared_ptr<Brain> brainTemplate,
                                                               std::shared_ptr<Environment> environmentTemplate,
                                                               SimulationStep maxSteps,
                                                               size_t maxParallel = 1,
                                                               std::function<bool(uint64_t, uint64_t)> progressCallback = nullptr);
    
    // Parameter sweep execution
    std::vector<std::shared_ptr<AdvancedExperiment>> runParameterSweep(
        const std::vector<std::shared_ptr<AdvancedExperiment>>& baseExperiments,
        const std::string& sweepType, size_t numCombinations,
        std::shared_ptr<Brain> brainTemplate, std::shared_ptr<Environment> environmentTemplate,
        SimulationStep maxSteps);
    
    // Multi-seed execution for statistical significance
    std::vector<std::shared_ptr<AdvancedExperiment>> runMultiSeed(const std::shared_ptr<AdvancedExperiment>& experiment,
                                                                   size_t numSeeds,
                                                                   std::shared_ptr<Brain> brainTemplate,
                                                                   std::shared_ptr<Environment> environmentTemplate,
                                                                   SimulationStep maxSteps);
    
    // Get all experiments
    const std::vector<std::shared_ptr<AdvancedExperiment>>& getExperiments() const;
    
    // Get experiment by ID
    std::shared_ptr<AdvancedExperiment> getExperiment(const std::string& id) const;
    
    // Clear all experiments
    void clearExperiments();
    
    // Configuration management
    void setGlobalParameter(const std::string& name, double value);
    double getGlobalParameter(const std::string& name) const;
    
    // Results management
    bool saveAllResults(const std::string& directory) const;
    bool loadResults(const std::string& directory);
    
    // Experiment versioning
    void createVersion(const std::string& experimentId, const std::string& versionName);
    std::vector<std::string> getExperimentVersions(const std::string& experimentId) const;
    std::shared_ptr<AdvancedExperiment> getExperimentVersion(const std::string& experimentId, const std::string& versionName) const;
    
    // Batch operations
    void deleteExperiments(const std::vector<std::string>& experimentIds);
    void exportExperiments(const std::vector<std::string>& experimentIds, const std::string& directory) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Helper functions
namespace AdvancedExperimentUtils {
    std::string generateExperimentId();
    std::string generateVersionName();
    std::string getTimestampString();
    std::string sanitizeFilename(const std::string& name);
    
    // Validation functions
    bool validateParameterValue(const Parameter& param, double value);
    bool validateConfiguration(const ExperimentConfig& config);
    
    // File operations
    std::vector<std::string> listFilesInDirectory(const std::string& directory);
    bool createDirectory(const std::string& directory);
    std::string getFileExtension(const std::string& filename);
    
    // Progress reporting
    void reportProgress(double progress, const std::string& message = "");
    void reportError(const std::string& error);
    void reportWarning(const std::string& warning);
}

// Convenience functions
std::shared_ptr<AdvancedExperiment> createAdvancedExperiment(const std::string& name, const std::string& description = "");
std::shared_ptr<AdvancedExperimentRunner> createAdvancedExperimentRunner();

} // namespace nlm
