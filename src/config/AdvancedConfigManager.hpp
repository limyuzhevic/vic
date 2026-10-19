// Advanced Configuration Manager
// This class provides advanced configuration management features

#pragma once

#include "nlm/core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>

namespace nlm {
namespace config {

class AdvancedConfigManager {
public:
    AdvancedConfigManager();
    ~AdvancedConfigManager();
    
    // Configuration loading and validation
    bool loadConfiguration(const std::string& filepath);
    bool loadFromJSON(const std::string& filepath);
    bool loadFromYAML(const std::string& filepath);
    bool loadFromKeyValue(const std::string& filepath);
    
    // Configuration validation
    bool validateConfiguration();
    bool validateAgainstSchema(const std::string& schemaFile);
    
    // Configuration comparison and diff
    bool compareConfigurations(const std::string& file1, const std::string& file2);
    std::string generateDiff(const std::string& file1, const std::string& file2);
    
    // Configuration export and import
    bool exportConfiguration(const std::string& format, const std::string& outputFile);
    bool importConfiguration(const std::string& format, const std::string& inputFile);
    std::string toJSON() const;
    std::string toYAML() const;
    std::string toKeyValue() const;
    
    // Configuration rollback
    bool saveCurrentVersion(const std::string& versionName);
    bool rollbackVersion(const std::string& versionName);
    std::vector<std::string> getAvailableVersions() const;
    
    // Configuration inheritance and merging
    bool inheritFromTemplate(const std::string& templateFile, const std::string& outputFile);
    bool mergeConfigurations(const std::string& file1, const std::string& file2, const std::string& outputFile);
    
    // Schema management
    bool setSchema(const std::string& schemaFile);
    bool validateSchema(const std::string& schemaFile);
    std::string getSchemaHelp() const;
    
    // Configuration analysis
    bool analyzeConfiguration(const std::string& filepath);
    bool detectInconsistencies();
    bool suggestImprovements();
    
    // Getters
    const nlm::Config& getConfig() const { return config_; }
    nlm::Config& getConfig() { return config_; }
    std::string getLastError() const { return lastError_; }
    bool isLoaded() const { return configLoaded_; }
    
private:
    nlm::Config config_;
    std::string lastError_;
    bool configLoaded_;
    std::map<std::string, std::string> versionHistory_;
    std::string currentVersion_;
    
    // Helper methods
    bool parseConfiguration(const std::string& content, const std::string& format);
    bool saveVersion(const std::string& versionName);
    bool loadVersion(const std::string& versionName);
    std::string generateConfigurationId() const;
    bool validateConfigurationSyntax(const std::string& content);
    bool checkCircularReferences();
};

} // namespace config
} // namespace nlm
