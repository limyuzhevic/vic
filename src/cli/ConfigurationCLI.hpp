#pragma once

#include "cli/CLI.hpp"
#include <string>
#include <vector>
#include <memory>

namespace nlm {

class ConfigurationCLI {
public:
    ConfigurationCLI();
    ~ConfigurationCLI();
    
    // Disable copying, enable moving
    ConfigurationCLI(const ConfigurationCLI&) = delete;
    ConfigurationCLI& operator=(const ConfigurationCLI&) = delete;
    ConfigurationCLI(ConfigurationCLI&&) noexcept;
    ConfigurationCLI& operator=(ConfigurationCLI&&) noexcept;
    
    // Initialization
    bool initialize();
    
    // Configuration commands
    bool loadCommand(const std::vector<std::string>& args);
    bool saveCommand(const std::vector<std::string>& args);
    bool validateCommand(const std::vector<std::string>& args);
    bool infoCommand(const std::vector<std::string>& args);
    bool listCommand(const std::vector<std::string>& args);
    bool exportCommand(const std::vector<std::string>& args);
    bool importCommand(const std::vector<std::string>& args);
    bool templateCommand(const std::vector<std::string>& args);
    bool setCommand(const std::vector<std::string>& args);
    bool getCommand(const std::vector<std::string>& args);
    bool removeCommand(const std::vector<std::string>& args);
    bool resetCommand(const std::vector<std::string>& args);
    
    // Configuration file management
    bool configFileCommand(const std::vector<std::string>& args);
    bool createConfigCommand(const std::vector<std::string>& args);
    bool deleteConfigCommand(const std::vector<std::string>& args);
    bool copyConfigCommand(const std::vector<std::string>& args);
    bool mergeConfigCommand(const std::vector<std::string>& args);
    
    // Configuration analysis
    bool analyzeCommand(const std::vector<std::string>& args);
    bool compareCommand(const std::vector<std::string>& args);
    bool diffCommand(const std::vector<std::string>& args);
    bool statsCommand(const std::vector<std::string>& args);
    
    // Advanced configuration operations
    bool batchCommand(const std::vector<std::string>& args);
    bool profileCommand(const std::vector<std::string>& args);
    bool optimizeCommand(const std::vector<std::string>& args);
    
    // Help and documentation
    void showHelp() const;
    void showCommandHelp(const std::string& command) const;
    
    // Integration
    void setConfig(std::shared_ptr<Config> config) { this->config = config; }
    void setBrain(std::shared_ptr<Brain> brain) { this->brain = brain; }
    void setCLI(CLI* cli) { this->cli = cli; }
    
    // State
    bool isInitialized() const { return initialized; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool initialized;
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    CLI* cli;
    
    // Helper methods
    bool parseKeyValue(const std::string& arg, std::string& key, std::string& value) const;
    std::string getConfigPath(const std::string& name) const;
    bool saveConfigWithBackup(const std::string& filepath) const;
    bool loadConfigWithValidation(const std::string& filepath);
    void printConfigEntry(const ConfigEntry& entry, bool verbose = false) const;
    void printConfigSummary(const std::string& title) const;
    std::vector<std::string> getConfigCompletions(const std::string& partial) const;
    std::vector<std::string> getFileCompletions(const std::string& partial) const;
    bool validateConfigValue(const std::string& key, const std::string& value) const;
    std::string formatValue(const ConfigValue& value) const;
    std::string formatSource(ConfigSource source) const;
};
