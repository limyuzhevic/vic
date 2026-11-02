#pragma once

#include "../core/Config/Config.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

class ExperimentConfigManager {
private:
    std::shared_ptr<Config> config;
    
    // Configuration schema for validation
    struct ConfigSchema {
        std::string key;
        std::string type;
        std::string description;
        std::string defaultValue;
        bool required;
        std::string validationPattern;
        
        ConfigSchema(const std::string& k, const std::string& t, const std::string& d, 
                     const std::string& dv, bool req = false, const std::string& pattern = "")
            : key(k), type(t), description(d), defaultValue(dv), required(req), validationPattern(pattern) {}
    };
    
    static const std::vector<ConfigSchema> getConfigSchema();
    
    bool validateConfigValue(const ConfigSchema& schema, const std::string& value);
    std::string getConfigValueAsString(const std::string& key);
    
public:
    ExperimentConfigManager();
    ~ExperimentConfigManager();
    
    void run(int argc, char* argv[]);
    void printBanner();
    void showHelp();
    void listConfigOptions();
    void setConfigValue(const std::string& key, const std::string& value);
    void getConfigValue(const std::string& key);
    void loadConfigFromFile(const std::string& filename);
    void saveConfigToFile(const std::string& filename);
    void validateAllConfig();
    void applyDefaultConfig();
};

} // namespace nlm
