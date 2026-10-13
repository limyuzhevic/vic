#pragma once

#include <memory>
#include <vector>
#include <string>
#include <map>
#include <simulation_step>

namespace nlm {

// Forward declarations
class Config;
class ExperimentRunner;
class Brain;
class Environment;

class AdvancedCLI {
public:
    AdvancedCLI();
    ~AdvancedCLI();
    
    // Configure AdvancedCLI with components
    void setConfig(std::shared_ptr<Config> config);
    void setExperimentRunner(std::shared_ptr<ExperimentRunner> runner);
    void setVerbose(bool verbose);
    void setDryRun(bool dryRun);
    
    // Advanced CLI functionality
    bool runParameterSweep(const std::map<std::string, std::vector<std::string>>& params,
                         const std::string& outputFile,
                         SimulationStep maxSteps);
    
    bool loadConfiguration(const std::string& filepath, const std::string& profile = "");
    bool saveConfiguration(const std::string& filepath, const std::string& format = "legacy",
                          bool includeDefaults = false, bool prettyPrint = true);
    
    bool compareResults(const std::vector<std::string>& resultFiles,
                      const std::string& outputFile,
                      const std::vector<std::string>& metricsToCompare = {});
    
    bool createBatchRun(const std::string& batchConfigFile,
                       const std::string& outputDir,
                       bool parallel = false);
    
    bool manageBatchRun(const std::string& operation,
                      const std::string& batchId,
                      const std::map<std::string, std::string>& options = {});
    
    // Helper methods
    std::map<std::string, double> loadResultFile(const std::string& filepath);
    void generateComparisonReport(const std::vector<std::map<std::string, double>>& results,
                                 const std::vector<std::string>& metrics,
                                 const std::string& outputFile);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void applyConfigurationProfile(const std::string& profile);
    void filterNonDefaultValues(Config& config);
    void generateParameterCombinations(
        const std::map<std::string, std::vector<std::string>>& params,
        std::vector<std::map<std::string, std::string>>& combinations);
    
    void generateParameterCombinationsRecursive(
        const std::map<std::string, std::vector<std::string>>& params,
        const std::vector<std::string>& keys,
        size_t index,
        std::map<std::string, std::string> current,
        std::vector<std::map<std::string, std::string>>& combinations);
    
    bool queryBatchStatus(const std::string& batchId);
    bool resumeBatchRun(const std::string& batchId);
    bool cancelBatchRun(const std::string& batchId);
    bool monitorBatchRun(const std::string& batchId,
                       const std::map<std::string, std::string>& options);
};

} // namespace nlm
