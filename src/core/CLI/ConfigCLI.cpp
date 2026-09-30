// Enhanced Configuration CLI Handler Implementation
// Command-line interface for enhanced configuration management

#include "CLI/ConfigCLI.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <cstring>
#include <ctime>

namespace nlm {

// Implementation of ConfigCLI helper functions

// Check if string ends with suffix
bool ConfigCLI::endsWith(const std::string& str, const std::string& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

// Convert string to lowercase
std::string ConfigCLI::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Remove file with error handling
void ConfigCLI::removeFile(const std::string& filepath) {
    if (std::filesystem::exists(filepath)) {
        try {
            std::filesystem::remove(filepath);
        } catch (const std::exception& e) {
            std::cerr << "Warning: Could not remove backup file " << filepath << ": " << e.what() << std::endl;
        }
    }
}

// Generate timestamped filename
std::string ConfigCLI::generateTimestampedFilename(const std::string& baseName) {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
    return baseName + "_" + ss.str();
}

// Format file size
std::string ConfigCLI::formatFileSize(uint64_t bytes) {
    const char* suffixes[] = {"B", "KB", "MB", "GB"};
    int suffixIndex = 0;
    double size = static_cast<double>(bytes);
    
    while (size >= 1024.0 && suffixIndex < 3) {
        size /= 1024.0;
        suffixIndex++;
    }
    
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << size << " " << suffixes[suffixIndex];
    return ss.str();
}

// Initialize static members
const std::map<std::string, std::string> ConfigCLI::FILE_FORMATS = {
    {".cfg", "legacy"},
    {".json", "json"},
    {".yaml", "yaml"},
    {".yml", "yaml"}
};

const std::map<std::string, double> ConfigCLI::DEFAULT_PRIORITIES = {
    {".cfg", 1.0},
    {".json", 1.5},
    {".yaml", 2.0},
    {".yml", 2.0}
};

// Default configuration file patterns
std::vector<std::string> ConfigCLI::getDefaultConfigPatterns() {
    return {
        "config.json",
        "config.yaml",
        "config.yml",
        "config.cfg",
        "nlm.conf",
        "nlm.cfg",
        "settings.json",
        "settings.yaml",
        "settings.yml",
        "settings.cfg"
    };
}

// Preferred configuration file paths
std::vector<std::string> ConfigCLI::getPreferredConfigPaths(const std::string& baseDir) {
    std::vector<std::string> paths;
    
    // User directory
    paths.push_back(baseDir + "/user.config");
    
    // Application directory
    paths.push_back(baseDir + "/app.config");
    
    // Home directory
    const char* home = std::getenv("HOME");
    if (home) {
        paths.push_back(std::string(home) + "/.nlm/config");
    }
    
    // Current directory
    paths.push_back(baseDir + "/default.config");
    
    return paths;
}

// Discover configuration files in directory
std::vector<ConfigCLI::ConfigFileInfo> ConfigCLI::discoverConfigFiles(const std::string& baseDir) const {
    std::vector<ConfigFileInfo> configFiles;
    
    if (!std::filesystem::exists(baseDir)) {
        return configFiles;
    }
    
    for (const auto& pattern : getDefaultConfigPatterns()) {
        std::string fullPath = baseDir + "/" + pattern;
        if (std::filesystem::exists(fullPath)) {
            ConfigFileInfo info;
            info.path = fullPath;
            info.exists = true;
            info.isReadable = std::filesystem::is_regular_file(fullPath) && 
                            std::filesystem::status(fullPath).permissions() & 
                            std::filesystem::perms::owner_read;
            info.format = detectFileFormat(fullPath);
            info.priority = DEFAULT_PRIORITIES.at(info.format);
            
            configFiles.push_back(info);
        }
    }
    
    // Sort by priority (higher priority first)
    std::sort(configFiles.begin(), configFiles.end(),
              [](const ConfigFileInfo& a, const ConfigFileInfo& b) {
                  return a.priority > b.priority;
              });
    
    return configFiles;
}

// Find configuration file
std::string ConfigCLI::findConfigFile(const std::string& preferredFile, const std::string& baseDir) const {
    if (!preferredFile.empty()) {
        if (isValidConfigFile(preferredFile)) {
            return preferredFile;
        }
    }
    
    // Search in preferred paths
    auto preferredPaths = getPreferredConfigPaths(baseDir);
    for (const auto& path : preferredPaths) {
        if (isValidConfigFile(path)) {
            return path;
        }
    }
    
    // Search using default patterns
    auto configFiles = discoverConfigFiles(baseDir);
    if (!configFiles.empty()) {
        return configFiles[0].path;
    }
    
    return "";
}

// Check if configuration file is valid
bool ConfigCLI::isValidConfigFile(const std::string& filepath) const {
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    if (!std::filesystem::is_regular_file(filepath)) {
        return false;
    }
    
    if (!(std::filesystem::status(filepath).permissions() & std::filesystem::perms::owner_read)) {
        return false;
    }
    
    std::string extension = filepath.substr(filepath.find_last_of("."));
    if (FILE_FORMATS.find(extension) == FILE_FORMATS.end()) {
        return false;
    }
    
    return true;
}

// Detect file format based on extension
std::string ConfigCLI::detectFileFormat(const std::string& filepath) const {
    std::string extension = filepath.substr(filepath.find_last_of("."));
    
    if (FILE_FORMATS.find(extension) != FILE_FORMATS.end()) {
        return FILE_FORMATS.at(extension);
    }
    
    return "unknown";
}

// Load configuration with enhanced file discovery
bool ConfigCLI::loadConfig(Config& config, const std::string& preferredFile, bool merge) const {
    std::string configFile = preferredFile.empty() ? 
        findConfigFile("", "configs") : preferredFile;
    
    if (configFile.empty()) {
        return false;
    }
    
    return config.loadFromFile(configFile);
}

// Merge two configuration objects
bool ConfigCLI::mergeConfigs(Config& baseConfig, const Config& overrideConfig) const {
    // Get keys from override config
    auto overrideKeys = overrideConfig.getKeys();
    
    for (const auto& key : overrideKeys) {
        auto overrideValue = overrideConfig.getString(key);
        if (overrideValue.has_value()) {
            baseConfig.set(key, overrideValue.value(), ConfigSource::CommandLine);
        } else {
            auto overrideInt = overrideConfig.getInt(key);
            if (overrideInt.has_value()) {
                baseConfig.set(key, overrideInt.value(), ConfigSource::CommandLine);
            } else {
                auto overrideInt64 = overrideConfig.getInt64(key);
                if (overrideInt64.has_value()) {
                    baseConfig.set(key, overrideInt64.value(), ConfigSource::CommandLine);
                } else {
                    auto overrideDouble = overrideConfig.getDouble(key);
                    if (overrideDouble.has_value()) {
                        baseConfig.set(key, overrideDouble.value(), ConfigSource::CommandLine);
                    } else {
                        auto overrideBool = overrideConfig.getBool(key);
                        if (overrideBool.has_value()) {
                            baseConfig.set(key, overrideBool.value(), ConfigSource::CommandLine);
                        }
                    }
                }
            }
        }
    }
    
    return true;
}

// Load configuration files with merging
bool ConfigCLI::loadConfigFiles(Config& config, const std::vector<std::string>& configFiles, bool merge) const {
    bool success = true;
    
    for (const auto& file : configFiles) {
        if (isValidConfigFile(file)) {
            auto tempConfig = Config();
            if (tempConfig.loadFromFile(file)) {
                if (merge) {
                    success &= mergeConfigs(config, tempConfig);
                } else {
                    // Override existing entries
                    auto keys = tempConfig.getKeys();
                    for (const auto& key : keys) {
                        if (tempConfig.has(key)) {
                            auto value = tempConfig.getString(key);
                            if (value.has_value()) {
                                config.set(key, value.value(), ConfigSource::File);
                            }
                        }
                    }
                }
            } else {
                success = false;
            }
        }
    }
    
    return success;
}

// Save configuration
bool ConfigCLI::saveConfig(const Config& config, const std::string& outputFile, const std::string& format) const {
    if (outputFile.empty()) {
        return false;
    }
    
    return const_cast<Config&>(config).saveToFile(outputFile);
}

// Export configuration
bool ConfigCLI::exportConfig(const Config& config, const std::string& format) const {
    std::string exported = const_cast<Config&>(config).exportConfig(format);
    if (exported.empty()) {
        return false;
    }
    
    std::cout << exported;
    return true;
}

// Apply configuration template
bool ConfigCLI::applyTemplate(Config& config, const std::string& templateName) const {
    return config.applyTemplate(templateName);
}

// Set inheritance rules
bool ConfigCLI::setInheritance(Config& config, const std::vector<std::string>& rules) const {
    for (const auto& rule : rules) {
        size_t colonPos = rule.find(':');
        if (colonPos != std::string::npos) {
            std::string key = Config::trim(rule.substr(0, colonPos));
            std::string baseKey = Config::trim(rule.substr(colonPos + 1));
            config.setInheritance(key, baseKey);
        }
    }
    return true;
}

// Validate configuration
bool ConfigCLI::validateConfig(Config& config) const {
    return config.validateConfig();
}

// Create backup
bool ConfigCLI::createBackup(const Config& config, const std::string& backupFile) const {
    std::string backupPath;
    if (backupFile.empty()) {
        backupPath = generateTimestampedFilename("config_backup");
    } else {
        backupPath = backupFile;
    }
    
    return const_cast<Config&>(config).saveToFile(backupPath);
}

// Restore backup
bool ConfigCLI::restoreBackup(Config& config, const std::string& backupFile) const {
    return config.loadFromFile(backupFile);
}

// Show configuration history
bool ConfigCLI::showHistory(const Config& config, const std::string& key) const {
    std::vector<ConfigHistoryEntry> history;
    if (key.empty()) {
        history = config.getHistory();
    } else {
        history = config.getHistory(key);
    }
    
    if (history.empty()) {
        std::cout << "No history found." << std::endl;
        return true;
    }
    
    std::cout << "Configuration History:" << std::endl;
    std::cout << "=====================" << std::endl;
    
    for (const auto& entry : history) {
        std::cout << "Timestamp: " << entry.timestamp << std::endl;
        std::cout << "Key: " << entry.key << std::endl;
        std::cout << "Action: " << entry.action << std::endl;
        std::cout << "Old Value: " << config.formatValue(entry.oldValue) << std::endl;
        std::cout << "New Value: " << config.formatValue(entry.newValue) << std::endl;
        std::cout << "----------------------" << std::endl;
    }
    
    return true;
}

// Show diff between two configurations
bool ConfigCLI::showDiff(const Config& config1, const Config& config2) const {
    std::string diff = config1.diff(config2);
    std::cout << diff;
    return true;
}

// Show configuration
bool ConfigCLI::showConfig(const Config& config) const {
    std::cout << config.summary();
    return true;
}

// Parse CLI arguments
ConfigCLI::CLIOptions ConfigCLI::parseArgs(int argc, char** argv) {
    CLIOptions options;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.showHelp = true;
        } else if (arg == "--version") {
            options.version = true;
        } else if (arg == "--validate") {
            options.validate = true;
        } else if (arg == "--merge") {
            options.merge = true;
        } else if (arg == "--export") {
            options.exportAll = true;
        } else if (arg.substr(0, 2) == "--config") {
            if (arg.find('=') != std::string::npos) {
                options.configFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.configFile = argv[++i];
            }
        } else if (arg.substr(0, 2) == "--output") {
            if (arg.find('=') != std::string::npos) {
                options.outputFile = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.outputFile = argv[++i];
            }
        } else if (arg.substr(0, 2) == "--format") {
            if (arg.find('=') != std::string::npos) {
                options.format = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.format = argv[++i];
            }
        } else if (arg.substr(0, 2) == "--template") {
            if (arg.find('=') != std::string::npos) {
                options.templateName = arg.substr(arg.find('=') + 1);
            } else if (i + 1 < argc) {
                options.templateName = argv[++i];
            }
        } else if (arg == "--verbose") {
            options.verbose = true;
        } else if (arg == "--backup") {
            options.backup = true;
        } else if (arg.substr(0, 2) == "--inherit") {
            if (arg.find('=') != std::string::npos) {
                options.inheritanceRules.push_back(arg.substr(arg.find('=') + 1));
            } else if (i + 1 < argc) {
                options.inheritanceRules.push_back(argv[++i]);
            }
        } else if (arg[0] == '-') {
            // Unknown option
            options.showHelp = true;
        }
    }
    
    return options;
}

// Print help
void ConfigCLI::printHelp(const CLIOptions& options) {
    std::cout << "=== NLM Configuration CLI ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Usage: nlm-config [OPTIONS]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -h, --help                  Show this help message" << std::endl;
    std::cout << "  --version                   Show version information" << std::endl;
    std::cout << "  --config <file>             Configuration file to load" << std::endl;
    std::cout << "  --output <file>             Output file for saving" << std::endl;
    std::cout << "  --format <format>           Output format (legacy, json, yaml)" << std::endl;
    std::cout << "  --template <name>           Apply configuration template" << std::endl;
    std::cout << "  --validate                  Validate configuration" << std::endl;
    std::cout << "  --merge                     Merge configuration files" << std::endl;
    std::cout << "  --export                    Export configuration" << std::endl;
    std::cout << "  --verbose                   Enable verbose output" << std::endl;
    std::cout << "  --backup                    Create backup" << std::endl;
    std::cout << "  --inherit <rules>           Set inheritance rules" << std::endl;
    std::cout << std::endl;
    std::cout << "Examples:" << std::endl;
    std::cout << "  nlm-config --config config.json" << std::endl;
    std::cout << "  nlm-config --template cpu_config" << std::endl;
    std::cout << "  nlm-config --format json --output config.json" << std::endl;
    std::cout << "  nlm-config --validate --config user.cfg" << std::endl;
    std::cout << std::endl;
}

// Print version
void ConfigCLI::printVersion() {
    std::cout << "NLM Configuration CLI v1.0.0" << std::endl;
    std::cout << "Enhanced Configuration System with JSON/YAML Support" << std::endl;
    std::cout << "Phase 2 Completion: JSON/YAML Parser Implementation" << std::endl;
}

// Get available templates
std::vector<std::string> ConfigCLI::getAvailableTemplates() const {
    // This would read from templates directory
    return {"basic", "cpu", "memory", "visualization", "production"};
}

// Show template
bool ConfigCLI::showTemplate(const std::string& templateName) const {
    std::cout << "Template: " << templateName << std::endl;
    std::cout << "Description: Configuration template for " << templateName << std::endl;
    
    auto templates = getConfigTemplates();
    for (const auto& templateContent : templates) {
        if (templateContent.find(templateName) != std::string::npos) {
            std::cout << templateContent << std::endl;
            break;
        }
    }
    
    return true;
}

// Validate configuration file
bool ConfigCLI::validateConfigFile(const std::string& filepath) const {
    if (!isValidConfigFile(filepath)) {
        std::cout << "Invalid configuration file: " << filepath << std::endl;
        return false;
    }
    
    auto tempConfig = Config();
    if (!tempConfig.loadFromFile(filepath)) {
        std::cout << "Failed to load configuration file: " << filepath << std::endl;
        return false;
    }
    
    if (!tempConfig.validateConfig()) {
        std::cout << "Configuration validation failed:" << std::endl;
        auto errors = tempConfig.getValidationErrors();
        for (const auto& error : errors) {
            std::cout << "  " << error << std::endl;
        }
        return false;
    }
    
    std::cout << "Configuration file " << filepath << " is valid." << std::endl;
    return true;
}

// Validate configuration data
bool ConfigCLI::validateConfigData(const std::string& data) const {
    auto tempConfig = Config();
    if (!tempConfig.importConfig(data)) {
        std::cout << "Failed to parse configuration data." << std::endl;
        return false;
    }
    
    if (!tempConfig.validateConfig()) {
        std::cout << "Configuration validation failed:" << std::endl;
        auto errors = tempConfig.getValidationErrors();
        for (const auto& error : errors) {
            std::cout << "  " << error << std::endl;
        }
        return false;
    }
    
    std::cout << "Configuration data is valid." << std::endl;
    return true;
}

} // namespace nlm