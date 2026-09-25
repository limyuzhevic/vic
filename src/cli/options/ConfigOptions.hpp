#pragma once

#include <string>
#include <map>

namespace nlm {
namespace cli {
namespace options {

class ConfigOptions {
public:
    ConfigOptions();
    ~ConfigOptions() = default;
    
    // Parse options from argument list
    void parse(const std::vector<std::string>& args);
    
    // Get parsed values
    std::string getConfigFile() const { return configFile; }
    std::string getConfigKeyValue() const { return configKeyValue; }
    std::string getConfigExportFile() const { return configExportFile; }
    std::string getConfigImportFile() const { return configImportFile; }
    bool getConfigValidate() const { return configValidate; }
    
    // Set values
    void setConfigFile(const std::string& file) { configFile = file; }
    void setConfigKeyValue(const std::string& keyValue) { configKeyValue = keyValue; }
    void setConfigExportFile(const std::string& file) { configExportFile = file; }
    void setConfigImportFile(const std::string& file) { configImportFile = file; }
    void setConfigValidate(bool validate) { configValidate = validate; }
    
private:
    std::string configFile;
    std::string configKeyValue;
    std::string configExportFile;
    std::string configImportFile;
    bool configValidate;
};

} // namespace options
} // namespace cli
} // namespace nlm
