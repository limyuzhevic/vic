#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <sstream>
#include <algorithm>

namespace nlm {

class ConfigManager {
public:
    ConfigManager() = default;
    
    // Configuration editing operations
    bool editConfig(const std::string& key, const std::string& value);
    bool editConfig(const std::string& key, double value);
    bool editConfig(const std::string& key, int value);
    bool editConfig(const std::string& key, bool value);
    
    // Configuration listing operations
    std::vector<std::string> listConfigKeys() const;
    std::vector<std::string> listConfigKeys(const std::string& prefix) const;
    std::string getConfigValue(const std::string& key) const;
    
    // Configuration saving operations
    bool saveConfig(const std::string& filename) const;
    bool loadConfig(const std::string& filename);
    
    // Configuration query operations
    bool hasConfig(const std::string& key) const;
    bool isConfigValid() const;
    
    // Utility methods
    std::string formatConfig(const std::map<std::string, std::string>& config) const;
    std::map<std::string, std::string> parseConfigFile(const std::string& filename) const;
    
    // Command-line interface helpers
    static std::string configEditHelp();
    static std::string configListHelp();
    static std::string configSaveHelp();
    
private:
    std::map<std::string, std::string> configValues;
    std::string lastError;
};

} // namespace nlm
