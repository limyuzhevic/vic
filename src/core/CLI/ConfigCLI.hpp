// Enhanced Configuration CLI Handler
// Command-line interface for enhanced configuration management

#pragma once

#include "core/Config/Config.hpp"
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <filesystem>
#include <sstream>

namespace nlm {

class ConfigCLI {
public:
    struct CLIOptions {
        std::string configFile;
        std::string outputFile;
        std::string format; // "legacy", "json", "yaml"
        std::string templateName;
        bool help;
        bool version;
        bool validate;
        bool merge;
        bool exportAll;
        bool showHelp;
        bool verbose;
        bool backup;
        std::vector<std::string> inheritanceRules;
    };
    
    struct ConfigFileInfo {
        std::string path;
        bool exists;
        bool isReadable;
        std::string format;
        double priority;
    };
    
    ConfigCLI() = default;
    ~ConfigCLI() = default;
    
    // Configuration file discovery
    std::vector<ConfigFileInfo> discoverConfigFiles(const std::string& baseDir = ".") const;
    std::string findConfigFile(const std::string& preferredFile = "", const std::string& baseDir = ".") const;
    bool isValidConfigFile(const std::string& filepath) const;
    std::string detectFileFormat(const std::string& filepath) const;
    
    // Configuration loading with merging
    bool loadConfig(Config& config, const std::string& preferredFile = "", bool merge = false) const;
    bool mergeConfigs(Config& baseConfig, const Config& overrideConfig) const;
    bool loadConfigFiles(Config& config, const std::vector<std::string>& configFiles, bool merge = false) const;
    
    // Configuration output
    bool saveConfig(const Config& config, const std::string& outputFile = "", const std::string& format = "legacy") const;
    bool exportConfig(const Config& config, const std::string& format = "legacy") const;
    
    // Configuration manipulation
    bool applyTemplate(Config& config, const std::string& templateName) const;
    bool setInheritance(Config& config, const std::vector<std::string>& rules) const;
    bool validateConfig(Config& config) const;
    
    // Configuration history and backup
    bool createBackup(const Config& config, const std::string& backupFile = "") const;
    bool restoreBackup(Config& config, const std::string& backupFile) const;
    bool showHistory(const Config& config, const std::string& key = "") const;
    
    // Configuration diff and comparison
    bool showDiff(const Config& config1, const Config& config2) const;
    bool showConfig(const Config& config) const;
    
    // CLI argument parsing
    static CLIOptions parseArgs(int argc, char** argv);
    static void printHelp(const CLIOptions& options);
    static void printVersion();
    
    // Configuration templates
    std::vector<std::string> getAvailableTemplates() const;
    bool showTemplate(const std::string& templateName) const;
    
    // Configuration validation
    bool validateConfigFile(const std::string& filepath) const;
    bool validateConfigData(const std::string& data) const;
    
    // File discovery patterns
    static std::vector<std::string> getDefaultConfigPatterns();
    static std::vector<std::string> getPreferredConfigPaths(const std::string& baseDir = ".");
    
private:
    // Helper functions
    static bool endsWith(const std::string& str, const std::string& suffix);
    static std::string toLower(const std::string& str);
    static void removeFile(const std::string& filepath);
    static std::string generateTimestampedFilename(const std::string& baseName);
    static std::string formatFileSize(uint64_t bytes);
    
    static const std::map<std::string, std::string> FILE_FORMATS;
    static const std::map<std::string, double> DEFAULT_PRIORITIES;
};

} // namespace nlm