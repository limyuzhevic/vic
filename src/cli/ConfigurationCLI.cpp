// Configuration CLI implementation for NLM
#include "cli/ConfigurationCLI.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>

namespace nlm {

struct ConfigurationCLI::Impl {
    std::string lastLoadedFile;
    std::string lastSavedFile;
    std::vector<std::string> recentFiles;
    size_t maxRecentFiles;
    bool backupOnSave;
    bool validateOnLoad;
    std::string defaultConfigDir;
    std::vector<std::string> templateFiles;
    std::map<std::string, std::string> fileExtensions;
    
    Impl() : maxRecentFiles(10), backupOnSave(true), validateOnLoad(true) {
        defaultConfigDir = getConfigDirectory();
        fileExtensions[".cfg"] = "Plain text configuration";
        fileExtensions[".json"] = "JSON configuration";
        fileExtensions[".yaml"] = "YAML configuration";
        fileExtensions[".yml"] = "YAML configuration";
    }
};

ConfigurationCLI::ConfigurationCLI() : initialized(false), config(nullptr), brain(nullptr), cli(nullptr) {
    pImpl = std::make_unique<Impl>();
}

ConfigurationCLI::~ConfigurationCLI() = default;

ConfigurationCLI::ConfigurationCLI(ConfigurationCLI&&) noexcept = default;

ConfigurationCLI& ConfigurationCLI::operator=(ConfigurationCLI&&) noexcept = default;

bool ConfigurationCLI::initialize() {
    if (initialized) return true;
    
    try {
        if (!config) {
            printError("Configuration not set");
            return false;
        }
        
        initialized = true;
        return true;
    } catch (const std::exception& e) {
        printError("Failed to initialize ConfigurationCLI: " + std::string(e.what()));
        return false;
    }
}

bool ConfigurationCLI::loadCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("ConfigurationCLI not initialized");
        return false;
    }
    
    std::string filename;
    bool validate = pImpl->validateOnLoad;
    
    if (args.size() >= 2) {
        filename = args[1];
        if (args.size() >= 3 && args[2] == "--validate") {
            validate = true;
        }
    } else {
        printError("Usage: load <filename> [--validate]");
        return false;
    }
    
    bool success = loadConfigWithValidation(filename, validate);
    
    if (success) {
        pImpl->lastLoadedFile = filename;
        if (std::find(pImpl->recentFiles.begin(), pImpl->recentFiles.end(), filename) == pImpl->recentFiles.end()) {
            pImpl->recentFiles.insert(pImpl->recentFiles.begin(), filename);
            if (pImpl->recentFiles.size() > pImpl->maxRecentFiles) {
                pImpl->recentFiles.pop_back();
            }
        }
        printSuccess("Configuration loaded from: " + filename);
    } else {
        printError("Failed to load configuration from: " + filename);
    }
    
    return success;
}

bool ConfigurationCLI::saveCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("ConfigurationCLI not initialized");
        return false;
    }
    
    std::string filename;
    bool prettyPrint = true;
    bool createBackup = pImpl->backupOnSave;
    
    if (args.size() >= 2) {
        filename = args[1];
        if (args.size() >= 3 && args[2] == "--no-pretty") {
            prettyPrint = false;
        }
        if (args.size() >= 4 && args[3] == "--no-backup") {
            createBackup = false;
        }
    } else {
        printError("Usage: save <filename> [--no-pretty] [--no-backup]");
        return false;
    }
    
    bool success = saveConfigWithBackup(filename, prettyPrint, createBackup);
    
    if (success) {
        pImpl->lastSavedFile = filename;
        printSuccess("Configuration saved to: " + filename);
    } else {
        printError("Failed to save configuration to: " + filename);
    }
    
    return success;
}

bool ConfigurationCLI::validateCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("ConfigurationCLI not initialized");
        return false;
    }
    
    std::string key;
    
    if (args.size() >= 2) {
        key = args[1];
    } else {
        printError("Usage: validate [key]");
        printInfo("Validating entire configuration...");
        
        std::vector<std::string> errors = config->getValidationErrors();
        if (errors.empty()) {
            printSuccess("Configuration is valid!");
            return true;
        } else {
            printError("Configuration validation failed:");
            for (const auto& error : errors) {
                printError("  " + error);
            }
            return false;
        }
    }
    
    if (!config->validate(key)) {
        printError("Validation failed for key: " + key);
        
        std::vector<std::string> errors = config->getValidationErrors(key);
        for (const auto& error : errors) {
            printError("  " + error);
        }
        return false;
    } else {
        printSuccess("Configuration key \"" + key + "\" is valid.");
        return true;
    }
}

bool ConfigurationCLI::infoCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("ConfigurationCLI not initialized");
        return false;
    }
    
    printConfigSummary("Configuration Information");
    return true;
}

bool ConfigurationCLI::listCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("ConfigurationCLI not initialized");
        return false;
    }
    
    std::string pattern;
    if (args.size() >= 2) {
        pattern = args[1];
    }
    
    auto entries = config->getEntries();
    if (!pattern.empty()) {
        entries = config->getEntriesByPattern(pattern);
    }
    
    print("Configuration entries:");
    for (const auto& entry : entries) {
        printConfigEntry(entry, true);
    }
    
    printInfo("Total entries: " + std::to_string(entries.size()));
    return true;
}

bool ConfigurationCLI::exportCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("ConfigurationCLI not initialized");
        return false;
    }
    
    std::string filename;
    std::vector<std::string> filterKeys;
    
    if (args.size() >= 2) {
        filename = args[1];
        if (args.size() >= 3) {
            filterKeys = std::vector<std::string>(args.begin() + 2, args.end());
        }
    } else {
        printError("Usage: export <filename> [key1 key2 ...]");
        return false;
    }
    
    bool success = config->exportConfig(filename, filterKeys);
    
    if (success) {
        printSuccess("Configuration exported to: " + filename);
    } else {
        printError("Failed to export configuration to: " + filename);
    }
    
    return success;
}

bool ConfigurationCLI::importCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("ConfigurationCLI not initialized");
        return false;
    }
    
    std::string filename;
    
    if (args.size() >= 2) {
        filename = args[1];
    } else {
        printError("Usage: import <filename>");
        return false;
    }
    
    bool success = loadConfigWithValidation(filename, true);
    
    if (success) {
        printSuccess("Configuration imported from: " + filename);
    } else {
        printError("Failed to import configuration from: " + filename);
    }
    
    return success;
}

bool ConfigurationCLI::templateCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("ConfigurationCLI not initialized");
        return false;
    }
    
    std::string action = args.size() >= 2 ? args[1] : "";
    
    if (action == "list" || action == "ls") {
        print("Available templates:");
        auto templates = config->getAvailableTemplates();
        for (const auto& templateName : templates) {
            print("  " + templateName);
        }
        
    } else if (action == "create" && args.size() >= 3) {
        std::string name = args[2];
        std::vector<std::string> includeKeys;
        if (args.size() >= 4) {
            includeKeys = std::vector<std::string>(args.begin() + 3, args.end());
        }
        
        bool success = config->saveTemplate(name, includeKeys);
        if (success) {
            printSuccess("Template created: " + name);
        } else {
            printError("Failed to create template: " + name);
        }
        
    } else if (action == "load" && args.size() >= 3) {
        std::string name = args[2];
        bool success = config->loadTemplate(name);
        if (success) {
            printSuccess("Template loaded: " + name);
        } else {
            printError("Failed to load template: " + name);
        }
        
    } else {
        printError("Usage: template list|create <name> [key1 key2 ...]|load <name>");
    }
    
    return true;
}

bool ConfigurationCLI::setCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("ConfigurationCLI not initialized");
        return false;
    }
    
    if (args.size() < 3) {
        printError("Usage: set <key> <value>");
        return false;
    }
    
    std::string key = args[1];
    std::string value = args[2];
    
    if (validateConfigValue(key, value)) {
        config->set(key, value, ConfigSource::Runtime);
        printSuccess("Set configuration \"" + key + "\" = \"" + value + "\"");
        return true;
    } else {
        printError("Invalid value for key: " + key);
        return false;
    }
}

bool ConfigurationCLI::getCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("ConfigurationCLI not initialized");
        return false;
    }
    
    if (args.size() < 2) {
        printError("Usage: get <key>");
        return false;
    }
    
    std::string key = args[1];
    
    if (config->has(key)) {
        ConfigValue value;
        if (auto val = config->get<std::string>(key)) {
            print(key + " = \"" + val.value() + "\"");
        } else if (auto val = config->get<int>(key)) {
            print(key + " = " + std::to_string(val.value()));
        } else if (auto val = config->get<double>(key)) {
            print(key + " = " + std::to_string(val.value()));
        } else if (auto val = config->get<bool>(key)) {
            print(key + " = " + (val.value() ? "true" : "false"));
        } else {
            printError("Could not retrieve value for key: " + key);
        }
    } else {
        printError("Key not found: " + key);
    }
    
    return true;
}

bool ConfigurationCLI::removeCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("ConfigurationCLI not initialized");
        return false;
    }
    
    if (args.size() < 2) {
        printError("Usage: remove <key>");
        return false;
    }
    
    std::string key = args[1];
    
    config->remove(key);
    printSuccess("Removed configuration key: " + key);
    return true;
}

bool ConfigurationCLI::resetCommand(const std::vector<std::string>& args) {
    if (!initialized) {
        printError("ConfigurationCLI not initialized");
        return false;
    }
    
    std::string key;
    if (args.size() >= 2) {
        key = args[1];
    }
    
    if (key.empty()) {
        config->clear();
        printSuccess("All configuration reset.");
    } else {
        config->resetToDefault(key);
        printSuccess("Configuration key reset to default: " + key);
    }
    
    return true;
}

bool ConfigurationCLI::configFileCommand(const std::vector<std::string>& args) {
    print("Configuration file management commands:");
    print("  config file list - List configuration files");
    print("  config file create <name> - Create new config file");
    print("  config file delete <name> - Delete config file");
    print("  config file copy <source> <target> - Copy config file");
    print("  config file merge <source> <target> - Merge config files");
    return true;
}

bool ConfigurationCLI::createConfigCommand(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        printError("Usage: create-config <name> [description]");
        return false;
    }
    
    std::string name = args[2];
    std::string description = args.size() >= 4 ? args[3] : "";
    
    std::string filepath = getConfigPath(name);
    if (std::filesystem::exists(filepath)) {
        printError("Configuration file already exists: " + filepath);
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        printError("Failed to create configuration file: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Name: " << name << "\n";
    if (!description.empty()) {
        file << "# Description: " << description << "\n";
    }
    file << "# Created: " << getFormattedTime() << "\n\n";
    
    printSuccess("Created configuration file: " + filepath);
    return true;
}

bool ConfigurationCLI::deleteConfigCommand(const std::vector<std::string>& args) {
    if (args.size() < 3) {
        printError("Usage: delete-config <name>");
        return false;
    }
    
    std::string name = args[2];
    std::string filepath = getConfigPath(name);
    
    if (!std::filesystem::exists(filepath)) {
        printError("Configuration file not found: " + filepath);
        return false;
    }
    
    if (!std::filesystem::remove(filepath)) {
        printError("Failed to delete configuration file: " + filepath);
        return false;
    }
    
    printSuccess("Deleted configuration file: " + filepath);
    return true;
}

bool ConfigurationCLI::copyConfigCommand(const std::vector<std::string>& args) {
    if (args.size() < 5) {
        printError("Usage: copy-config <source> <target> [as newname]");
        return false;
    }
    
    std::string source = args[2];
    std::string target = args[3];
    std::string newname = args.size() >= 5 ? args[4] : "";
    
    std::string sourcePath = getConfigPath(source);
    std::string targetPath = getConfigPath(target);
    
    if (!std::filesystem::exists(sourcePath)) {
        printError("Source configuration file not found: " + sourcePath);
        return false;
    }
    
    if (std::filesystem::exists(targetPath)) {
        printError("Target configuration file already exists: " + targetPath);
        return false;
    }
    
    std::filesystem::copy_file(sourcePath, targetPath);
    
    if (!newname.empty()) {
        std::string renamedPath = getConfigPath(newname);
        std::filesystem::rename(targetPath, renamedPath);
        printSuccess("Configuration copied and renamed: " + newname);
    } else {
        printSuccess("Configuration copied: " + target);
    }
    
    return true;
}

bool ConfigurationCLI::mergeConfigCommand(const std::vector<std::string>& args) {
    if (args.size() < 5) {
        printError("Usage: merge-config <source> <target> [overwrite=true]");
        return false;
    }
    
    std::string source = args[2];
    std::string target = args[3];
    bool overwrite = args.size() >= 5 && (args[4] == "true" || args[4] == "1" || args[4] == "yes");
    
    std::string sourcePath = getConfigPath(source);
    std::string targetPath = getConfigPath(target);
    
    if (!std::filesystem::exists(sourcePath)) {
        printError("Source configuration file not found: " + sourcePath);
        return false;
    }
    
    if (!std::filesystem::exists(targetPath)) {
        printError("Target configuration file not found: " + targetPath);
        return false;
    }
    
    auto config2 = std::make_shared<Config>();
    if (!config2->loadFromFile(targetPath)) {
        printError("Failed to load target configuration");
        return false;
    }
    
    Config tempConfig = *config.get();
    tempConfig.merge(*config2.get());
    
    *config.get() = std::move(tempConfig);
    
    if (config->saveToFile(targetPath)) {
        printSuccess("Configuration merged into: " + target);
    } else {
        printError("Failed to save merged configuration");
    }
    
    return true;
}

bool ConfigurationCLI::analyzeCommand(const std::vector<std::string>& args) {
    printConfigSummary("Configuration Analysis");
    
    std::string stats = config->getStats();
    print("=== Statistics ===");
    print(stats);
    
    auto missing = config->getMissingRequiredKeys();
    if (!missing.empty()) {
        print("\nMissing required keys:");
        for (const auto& key : missing) {
            print("  - " + key);
        }
    }
    
    return true;
}

bool ConfigurationCLI::compareCommand(const std::vector<std::string>& args) {
    if (args.size() < 4) {
        printError("Usage: compare <file1> <file2> [--detailed]");
        return false;
    }
    
    std::string file1 = args[2];
    std::string file2 = args[3];
    bool detailed = args.size() >= 5 && (args[4] == "--detailed" || args[4] == "-d");
    
    auto config1 = std::make_shared<Config>();
    auto config2 = std::make_shared<Config>();
    
    bool success1 = config1->loadFromFile(file1);
    bool success2 = config2->loadFromFile(file2);
    
    if (!success1 || !success2) {
        printError("Failed to load configuration files");
        return false;
    }
    
    std::string diff = config1->diff(*config2.get());
    print("=== Configuration Comparison ===");
    print(diff);
    
    return true;
}

bool ConfigurationCLI::diffCommand(const std::vector<std::string>& args) {
    return compareCommand(args);
}

bool ConfigurationCLI::statsCommand(const std::vector<std::string>& args) {
    return analyzeCommand(args);
}

bool ConfigurationCLI::batchCommand(const std::vector<std::string>& args) {
    print("Batch configuration operations:");
    print("  batch load <directory> - Load multiple config files");
    print("  batch save <directory> - Save multiple config files");
    print("  batch import <archive> - Import config archive");
    print("  batch export <archive> - Export config archive");
    return true;
}

bool ConfigurationCLI::profileCommand(const std::vector<std::string>& args) {
    print("Profiling configuration operations:");
    print("  profile memory - Analyze memory usage");
    print("  profile time - Analyze execution time");
    print("  profile changes - Track configuration changes");
    return true;
}

bool ConfigurationCLI::optimizeCommand(const std::vector<std::string>& args) {
    print("Optimization commands:");
    print("  optimize compact - Compact configuration");
    print("  optimize validate - Validate configuration");
    print("  optimize backup - Create backup");
    return true;
}

void ConfigurationCLI::showHelp() const {
    print("=== Configuration CLI Commands ===");
    print("  load <filename> [--validate] - Load configuration from file");
    print("  save <filename> [--no-pretty] [--no-backup] - Save configuration to file");
    print("  validate [key] - Validate configuration");
    print("  info - Show configuration information");
    print("  list [pattern] - List configuration entries");
    print("  export <filename> [key1 key2 ...] - Export configuration to file");
    print("  import <filename> - Import configuration from file");
    print("  template list|create <name> [keys...]|load <name> - Template management");
    print("  set <key> <value> - Set configuration value");
    print("  get <key> - Get configuration value");
    print("  remove <key> - Remove configuration key");
    print("  reset [key] - Reset configuration");
    print("  analyze - Analyze configuration");
    print("  compare <file1> <file2> [--detailed] - Compare configurations");
    print("  diff <file1> <file2> - Show differences");
    print("  stats - Show configuration statistics");
    print("  config file list|create <name>|delete <name>|copy <src> <dst>|merge <src> <dst> - File management");
    print("  batch - Batch operations");
    print("  profile - Profiling operations");
    print("  optimize - Optimization operations");
}

void ConfigurationCLI::showCommandHelp(const std::string& command) const {
    std::map<std::string, std::string> helpTexts = {
        {"load", "Load configuration from file\nUsage: load <filename> [--validate]"},
        {"save", "Save configuration to file\nUsage: save <filename> [--no-pretty] [--no-backup]"},
        {"validate", "Validate configuration\nUsage: validate [key]"},
        {"info", "Show configuration information\nUsage: info"},
        {"list", "List configuration entries\nUsage: list [pattern]"},
        {"export", "Export configuration to file\nUsage: export <filename> [key1 key2 ...]"},
        {"import", "Import configuration from file\nUsage: import <filename>"},
        {"template", "Template management\nUsage: template list|create <name> [keys...]|load <name>"},
        {"set", "Set configuration value\nUsage: set <key> <value>"},
        {"get", "Get configuration value\nUsage: get <key>"},
        {"remove", "Remove configuration key\nUsage: remove <key>"},
        {"reset", "Reset configuration\nUsage: reset [key]"},
        {"analyze", "Analyze configuration\nUsage: analyze"},
        {"compare", "Compare configurations\nUsage: compare <file1> <file2> [--detailed]"},
        {"diff", "Show differences\nUsage: diff <file1> <file2>"},
        {"stats", "Show configuration statistics\nUsage: stats"},
        {"config file", "File management commands\nUsage: config file list|create <name>|delete <name>|copy <src> <dst>|merge <src> <dst>"},
        {"batch", "Batch operations\nUsage: batch"},
        {"profile", "Profiling operations\nUsage: profile"},
        {"optimize", "Optimization operations\nUsage: optimize"}
    };
    
    auto it = helpTexts.find(command);
    if (it != helpTexts.end()) {
        print(it->second);
    } else {
        printError("No help available for command: " + command);
    }
}

bool ConfigurationCLI::parseKeyValue(const std::string& arg, std::string& key, std::string& value) const {
    size_t pos = arg.find('=');
    if (pos != std::string::npos) {
        key = arg.substr(0, pos);
        value = arg.substr(pos + 1);
        return true;
    }
    return false;
}

std::string ConfigurationCLI::getConfigPath(const std::string& name) const {
    std::string extension = "";
    if (name.size() >= 4 && name.substr(name.size() - 4) == ".cfg") {
        extension = "";
    } else if (name.size() >= 5 && (name.substr(name.size() - 5) == ".json" || name.size() >= 5 && name.substr(name.size() - 5) == ".yaml" || name.size() >= 5 && name.substr(name.size() - 5) == ".yml")) {
        extension = "." + name.substr(name.size() - 4);
    }
    
    return pImpl->defaultConfigDir + "/" + name + extension;
}

bool ConfigurationCLI::saveConfigWithBackup(const std::string& filepath, bool prettyPrint, bool createBackup) const {
    std::string backupPath = filepath + ".backup";
    
    if (createBackup && std::filesystem::exists(filepath)) {
        try {
            std::filesystem::copy_file(filepath, backupPath, std::filesystem::copy_options::overwrite_existing);
        } catch (const std::filesystem::filesystem_error& e) {
            printWarning("Failed to create backup: " + std::string(e.what()));
        }
    }
    
    return config->saveToFile(filepath, prettyPrint);
}

bool ConfigurationCLI::loadConfigWithValidation(const std::string& filepath, bool validate) {
    bool success = config->loadFromFile(filepath);
    
    if (success && validate) {
        std::vector<std::string> errors = config->getValidationErrors();
        if (!errors.empty()) {
            printError("Configuration validation failed:");
            for (const auto& error : errors) {
                printError("  " + error);
            }
            return false;
        }
    }
    
    return success;
}

void ConfigurationCLI::printConfigEntry(const ConfigEntry& entry, bool verbose) const {
    print("  " + entry.key + " = " + formatValue(entry.value));
    
    if (verbose) {
        print("    Source: " + formatSource(entry.source));
        if (!entry.description.empty()) {
            print("    Description: " + entry.description);
        }
        if (!entry.validationPattern.empty()) {
            print("    Pattern: " + entry.validationPattern);
        }
        print("    Required: " + std::string(entry.required ? "yes" : "no"));
    }
}

void ConfigurationCLI::printConfigSummary(const std::string& title) const {
    print("=== " + title + " ===");
    
    std::string summary = config->summary();
    print(summary);
    
    std::string debugInfo = config->debugInfo();
    if (!debugInfo.empty()) {
        print("\n=== Debug Information ===");
        print(debugInfo);
    }
}

std::vector<std::string> ConfigurationCLI::getConfigCompletions(const std::string& partial) const {
    std::vector<std::string> completions;
    auto keys = config->getKeys();
    for (const auto& key : keys) {
        if (key.find(partial) == 0) {
            completions.push_back(key);
        }
    }
    return completions;
}

std::vector<std::string> ConfigurationCLI::getFileCompletions(const std::string& partial) const {
    std::vector<std::string> completions;
    
    if (std::filesystem::exists(pImpl->defaultConfigDir)) {
        for (const auto& entry : std::filesystem::directory_iterator(pImpl->defaultConfigDir)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.find(partial) == 0) {
                    completions.push_back(filename);
                }
            }
        }
    }
    
    return completions;
}

bool ConfigurationCLI::validateConfigValue(const std::string& key, const std::string& value) const {
    std::string tempKey = key + "_temp";
    config->set(tempKey, value, ConfigSource::Runtime);
    
    bool valid = config->validate(tempKey);
    
    config->remove(tempKey);
    
    return valid;
}

std::string ConfigurationCLI::formatValue(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](const auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

std::string ConfigurationCLI::formatSource(ConfigSource source) const {
    switch (source) {
        case ConfigSource::Default: return "Default";
        case ConfigSource::File: return "File";
        case ConfigSource::CommandLine: return "Command Line";
        case ConfigSource::Runtime: return "Runtime";
        case ConfigSource::Environment: return "Environment";
        default: return "Unknown";
    }
}

std::string ConfigurationCLI::getFormattedTime() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}
