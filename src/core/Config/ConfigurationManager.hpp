#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>

namespace nlm {

class Config;

class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    // Configuration validation
    bool validateConfiguration(std::shared_ptr<Config> config) const;
    std::vector<std::string> getValidationErrors(std::shared_ptr<Config> config) const;
    std::vector<std::string> getValidationWarnings(std::shared_ptr<Config> config) const;
    
    // Configuration analysis
    std::string analyzeConfiguration(std::shared_ptr<Config> config) const;
    std::vector<std::string> getOptimizationSuggestions(std::shared_ptr<Config> config) const;
    
    // Configuration comparison
    std::vector<std::string> compareConfigurations(std::shared_ptr<Config> config1, std::shared_ptr<Config> config2) const;
    
    // Configuration generation
    std::string generateConfigurationReport(std::shared_ptr<Config> config) const;
    
    // Configuration optimization
    bool optimizeConfiguration(std::shared_ptr<Config> config, const std::string& optimizationType = "balanced");
    
    // Baseline management
    bool setBaselineConfiguration(const std::string& name, std::shared_ptr<Config> config);
    std::shared_ptr<Config> getBaselineConfiguration(const std::string& name) const;
    std::vector<std::string> getAvailableBaselines() const;
    
    // Configuration history and comparison
    void recordConfigurationChange(std::shared_ptr<Config> config, const std::string& reason = "");
    std::vector<std::pair<Timestamp, std::shared_ptr<Config>>> getConfigurationHistory() const;
    std::vector<std::string> getConfigurationDiff(const std::string& config1Name, const std::string& config2Name) const;
    
    // Performance analysis
    std::map<std::string, double> analyzePerformanceMetrics(std::shared_ptr<Config> config) const;
    double calculatePerformanceScore(std::shared_ptr<Config> config) const;
    
    // Temperature scaling and adaptation
    bool adaptConfiguration(std::shared_ptr<Config> config, double temperatureFactor);
    std::shared_ptr<Config> generateTemperatureVariant(std::shared_ptr<Config> config, double temperature);
    
    // Configuration validation and safety
    bool validateForProduction(std::shared_ptr<Config> config) const;
    std::vector<std::string> getProductionReadinessIssues(std::shared_ptr<Config> config) const;
    
    // Configuration documentation
    std::string generateDocumentation(std::shared_ptr<Config> config) const;
    std::string generateAPIReference(std::shared_ptr<Config> config) const;
    
    // Configuration export/import
    bool exportConfiguration(std::shared_ptr<Config> config, const std::string& filepath, const std::string& format = "json") const;
    std::shared_ptr<Config> importConfiguration(const std::string& filepath, const std::string& format = "auto") const;
    
    // Configuration comparison with external sources
    std::vector<std::string> compareWithBestPractices(std::shared_ptr<Config> config) const;
    std::vector<std::string> compareWithStateOfTheArt(std::shared_ptr<Config> config) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
