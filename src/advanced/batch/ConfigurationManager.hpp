#pragma once

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include "../core/Config/Config.hpp"

namespace nlm {

class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    void setConfig(std::shared_ptr<Config> config);
    
    bool analyzeConfiguration(const std::string& outputFile);
    bool validateConfiguration(const std::string& configFile, const std::string& schemaFile = "");
    bool generateConfigurationReport(const std::string& outputFile);
    
    std::vector<std::string> getConfigurationIssues() const;
    std::map<std::string, double> getConfigurationStatistics() const;
    
    // Configuration comparison
    bool compareConfigurations(const std::string& configFile1, const std::string& configFile2,
                              const std::string& outputFile);
    
    // Configuration inheritance analysis
    bool analyzeConfigurationInheritance(const std::string& configFile);
    bool validateInheritanceChain(const std::string& configFile);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    void generateConfigurationAnalysis(std::ofstream& file);
    void generateConfigurationReport(std::ofstream& file);
    void compareConfigurations(std::ofstream& file, const Config& config1, const Config& config2,
                              const std::vector<std::string>& sectionsToCompare);
    void analyzeInheritance(const Config& config);
    void validateInheritanceChain(const Config& config);
    void loadSchemaFromFile(const std::string& filepath);
};

} // namespace nlm
