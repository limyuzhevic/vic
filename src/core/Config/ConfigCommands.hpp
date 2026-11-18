// Configuration Command System (Phase 2 Enhancement)
// Advanced configuration management with command-line interface

#ifndef NLM_CONFIG_COMMANDS_HPP
#define NLM_CONFIG_COMMANDS_HPP

#include <string>
#include <vector>
#include <map>
#include <memory>
#include "Config.hpp"

namespace nlm {

class ConfigCommandSystem {
public:
    ConfigCommandSystem();
    ~ConfigCommandSystem();
    
    // Command execution
    bool executeCommand(const std::string& command);
    bool executeFileCommand(const std::string& filepath);
    
    // Configuration management
    bool setConfig(const std::string& key, const std::string& value);
    bool getConfig(const std::string& key, std::string& value) const;
    bool removeConfig(const std::string& key);
    bool listConfig() const;
    
    // Configuration validation
    bool validateConfig() const;
    bool checkConfigConsistency() const;
    
    // Configuration commands
    bool setCommand(const std::vector<std::string>& args);
    bool getCommand(const std::vector<std::string>& args) const;
    bool removeCommand(const std::vector<std::string>& args);
    bool listCommand(const std::vector<std::string>& args) const;
    bool validateCommand(const std::vector<std::string>& args) const;
    bool saveCommand(const std::vector<std::string>& args);
    bool loadCommand(const std::vector<std::string>& args);
    bool backupCommand(const std::vector<std::string>& args);
    bool restoreCommand(const std::vector<std::string>& args);
    bool exportCommand(const std::vector<std::string>& args) const;
    bool importCommand(const std::vector<std::string>& args);
    
    // Configuration validation and checking
    bool validateFileFormat(const std::string& filepath) const;
    bool validateKeyFormat(const std::string& key) const;
    bool validateValueFormat(const std::string& key, const std::string& value) const;
    
    // Configuration analysis
    size_t getConfigCount() const;
    std::vector<std::string> getConfigKeys() const;
    std::string getConfigSummary() const;
    std::map<std::string, std::string> getConfigAsMap() const;
    
    // Configuration history
    bool addConfigHistory(const std::string& command, const std::string& timestamp);
    std::vector<std::string> getConfigHistory() const;
    bool clearConfigHistory();
    
    // Configuration diff
    std::string getConfigDiff(const std::string& otherConfigFile) const;
    bool compareConfigs(const std::string& file1, const std::string& file2) const;
    
    // Configuration validation rules
    bool setValidationRules(const std::string& rulesFile);
    bool validateAgainstRules(const std::string& configFile) const;
    
    // Configuration dependencies
    bool addConfigDependency(const std::string& key, const std::string& dependsOn);
    bool checkConfigDependencies() const;
    bool removeConfigDependency(const std::string& key);
    
    // Configuration templates
    bool saveConfigTemplate(const std::string& templateName);
    bool loadConfigTemplate(const std::string& templateName);
    bool listConfigTemplates() const;
    
    // Configuration export formats
    bool exportToJSON(const std::string& filepath) const;
    bool exportToYAML(const std::string& filepath) const;
    bool exportToINI(const std::string& filepath) const;
    bool exportToProperties(const std::string& filepath) const;
    
    // Configuration import formats
    bool importFromJSON(const std::string& filepath);
    bool importFromYAML(const std::string& filepath);
    bool importFromINI(const std::string& filepath);
    bool importFromProperties(const std::string& filepath);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helpers
    std::string trim(const std::string& str) const;
    bool isValidKey(const std::string& key) const;
    bool isValidValue(const std::string& value) const;
    std::string getTimestamp() const;
    bool parseCommandArgs(const std::string& command, std::vector<std::string>& args) const;
    std::string getErrorMessage(int errorCode) const;
    
    // Configuration validation
    bool validateNumeric(const std::string& value) const;
    bool validateInteger(const std::string& value) const;
    bool validateFloat(const std::string& value) const;
    bool validateBoolean(const std::string& value) const;
    bool validateString(const std::string& value) const;
    bool validateVector(const std::string& value) const;
    
    // Command parsing
    bool parseSetCommand(const std::vector<std::string>& args);
    bool parseGetCommand(const std::vector<std::string>& args) const;
    bool parseRemoveCommand(const std::vector<std::string>& args);
    bool parseListCommand(const std::vector<std::string>& args) const;
    bool parseSaveCommand(const std::vector<std::string>& args);
    bool parseLoadCommand(const std::vector<std::string>& args);
    
    // Configuration file operations
    bool saveConfigToFile(const std::string& filepath) const;
    bool loadConfigFromFile(const std::string& filepath);
    bool backupConfig(const std::string& filepath) const;
    bool restoreConfig(const std::string& filepath);
    
    // Configuration analysis
    float calculateConfigConsistencyScore() const;
    std::vector<std::string> findConfigConflicts() const;
    std::vector<std::string> findConfigDuplicates() const;
    
    // Configuration history
    bool logConfigChange(const std::string& key, const std::string& oldValue, const std::string& newValue);
    
    // Configuration export
    bool exportToFormat(const std::string& format, const std::string& filepath) const;
    bool importFromFormat(const std::string& format, const std::string& filepath);
};

} // namespace nlm

#endif // NLM_CONFIG_COMMANDS_HPP