// Brain CLI Configuration System
// Provides configuration management for the NLM Brain CLI

#ifndef BRAINCLICONFIG_HPP
#define BRAINCLICONFIG_HPP

#include <string>
#include <map>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include "BrainCLI.hpp"

namespace nlm {
namespace cli {

// Configuration value types
enum class ConfigValueType {
    STRING,
    INTEGER,
    DOUBLE,
    BOOLEAN,
    FLOAT
};

// Configuration option definition
struct ConfigOption {
    std::string key;
    std::string description;
    ConfigValueType type;
    std::string defaultValue;
    std::string category;
    bool required;
};

// CLI configuration system
class CLIConfig {
public:
    CLIConfig();
    ~CLIConfig();
    
    // Configuration management
    void loadFromFile(const std::string& filename);
    void saveToFile(const std::string& filename) const;
    void loadFromArgs(int argc, char** argv);
    
    // Value access
    std::string getString(const std::string& key) const;
    int64_t getInt(const std::string& key) const;
    double getDouble(const std::string& key) const;
    bool getBool(const std::string& key) const;
    float getFloat(const std::string& key) const;
    
    // Value setting
    void setString(const std::string& key, const std::string& value);
    void setInt(const std::string& key, int64_t value);
    void setDouble(const std::string& key, double value);
    void setBool(const std::string& key, bool value);
    void setFloat(const std::string& key, float value);
    
    // Configuration validation
    bool validate() const;
    std::vector<std::string> getErrors() const;
    
    // Configuration templates
    void loadTemplate(const std::string& templateName);
    void saveTemplate(const std::string& templateName) const;
    
    // Configuration search
    std::vector<std::string> search(const std::string& pattern) const;
    void showHelp() const;
    void showCategories() const;
    
    // Configuration merging
    void merge(const CLIConfig& other);
    void reset();
    
    // Application configuration
    std::string getApplicationName() const { return applicationName_; }
    std::string getVersion() const { return version_; }
    std::string getDescription() const { return description_; }
    
    void setApplicationName(const std::string& name) { applicationName_ = name; }
    void setVersion(const std::string& ver) { version_ = ver; }
    void setDescription(const std::string& desc) { description_ = desc; }
    
    // File paths
    std::string getDefaultConfigDir() const { return defaultConfigDir_; }
    std::string getDefaultLogDir() const { return defaultLogDir_; }
    std::string getDefaultDataDir() const { return defaultDataDir_; }
    
    void setDefaultConfigDir(const std::string& dir) { defaultConfigDir_ = dir; }
    void setDefaultLogDir(const std::string& dir) { defaultLogDir_ = dir; }
    void setDefaultDataDir(const std::string& dir) { defaultDataDir_ = dir; }
    
    // Plugin paths
    std::vector<std::string> getPluginPaths() const { return pluginPaths_; }
    void addPluginPath(const std::string& path) { pluginPaths_.push_back(path); }
    
    // Command line options
    std::string getCommandLine() const { return commandLine_; }
    void setCommandLine(const std::string& cmd) { commandLine_ = cmd; }
    
protected:
    std::map<std::string, std::string> configValues_;
    std::vector<ConfigOption> configOptions_;
    std::vector<std::string> errors_;
    
    std::string applicationName_;
    std::string version_;
    std::string description_;
    std::string defaultConfigDir_;
    std::string defaultLogDir_;
    std::string defaultDataDir_;
    std::vector<std::string> pluginPaths_;
    std::string commandLine_;
    
    // Configuration categories
    void initDefaultOptions();
    void initApplicationOptions();
    void initBrainOptions();
    void initVisualizationOptions();
    void initAnalysisOptions();
    void initPluginOptions();
    void initPerformanceOptions();
    void initLoggingOptions();
    
    // Helper functions
    std::string trim(const std::string& str) const;
    bool parseBool(const std::string& str) const;
    int64_t parseInt(const std::string& str) const;
    double parseDouble(const std::string& str) const;
    float parseFloat(const std::string& str) const;
    std::string toLower(const std::string& str) const;
};

// Global configuration instance
extern CLIConfig& getGlobalConfig();

} // namespace cli
} // namespace nlm

#endif // BRAINCLICONFIG_HPP
