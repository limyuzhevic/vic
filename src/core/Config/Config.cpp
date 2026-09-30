// Enhanced Configuration System Implementation
// Comprehensive configuration management with JSON/YAML support

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <stdexcept>
#include <unordered_map>
#include <chrono>
#include <iomanip>
#include <iostream>

namespace nlm {

// Implementation of Config helper functions

// Helper function to get current timestamp for history entries
static std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Simple JSON parser implementation
bool Config::parseJSON(const std::string& jsonStr, std::vector<ConfigEntry>& entries, ConfigSource source) {
    std::stringstream ss(jsonStr);
    std::string line;
    
    while (std::getline(ss, line)) {
        line = Config::trim(line);
        if (line.empty() || line[0] == '/') {
            continue;
        }
        
        // Handle JSON object format
        if (line[0] == '{' && line.back() == '}') {
            // Extract content inside braces
            line = line.substr(1, line.size() - 2);
            
            // Parse key-value pairs separated by commas
            size_t start = 0;
            while (start < line.size()) {
                size_t commaPos = line.find(',', start);
                if (commaPos == std::string::npos) {
                    commaPos = line.size();
                }
                
                std::string pair = line.substr(start, commaPos - start);
                size_t colonPos = pair.find(':');
                if (colonPos != std::string::npos) {
                    std::string key = Config::trim(pair.substr(0, colonPos));
                    std::string value = Config::trim(pair.substr(colonPos + 1));
                    
                    // Remove quotes from key and value
                    if (key.size() >= 2 && key[0] == '"' && key.back() == '"') {
                        key = key.substr(1, key.size() - 2);
                    }
                    if (value.size() >= 2 && value[0] == '"' && value.back() == '"') {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    // Convert string value to appropriate type
                    ConfigValue configValue;
                    if (value.find('.') != std::string::npos) {
                        configValue = std::stod(value);
                    } else if (value == "true" || value == "false") {
                        configValue = (value == "true");
                    } else {
                        try {
                            if (value.find("-") == 0 || std::all_of(value.begin(), value.end(), ::isdigit)) {
                                configValue = std::stoll(value);
                            } else {
                                configValue = value;
                            }
                        } catch (...) {
                            configValue = value;
                        }
                    }
                    
                    entries.push_back({key, configValue, source, "", "", "", false, ""});
                }
                
                start = commaPos + 1;
                if (start < line.size() && line[start] == ' ') {
                    start++;
                }
            }
        }
    }
    
    return true;
}

// Simple YAML parser implementation
bool Config::parseYAML(const std::string& yamlStr, std::vector<ConfigEntry>& entries, ConfigSource source) {
    std::stringstream ss(yamlStr);
    std::string line;
    
    while (std::getline(ss, line)) {
        line = Config::trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Handle YAML key-value pairs
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = Config::trim(line.substr(0, colonPos));
            std::string value = Config::trim(line.substr(colonPos + 1));
            
            // Handle multi-line values
            if (value.empty()) {
                std::string nextLine;
                if (std::getline(ss, nextLine)) {
                    value = Config::trim(nextLine);
                }
            }
            
            // Remove quotes from key and value
            if (key.size() >= 2 && key[0] == '"' && key.back() == '"') {
                key = key.substr(1, key.size() - 2);
            }
            if (value.size() >= 2 && value[0] == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            
            // Convert string value to appropriate type
            ConfigValue configValue;
            if (value.find('.') != std::string::npos) {
                configValue = std::stod(value);
            } else if (value == "true" || value == "false") {
                configValue = (value == "true");
            } else {
                try {
                    if (value.find("-") == 0 || std::all_of(value.begin(), value.end(), ::isdigit)) {
                        configValue = std::stoll(value);
                    } else {
                        configValue = value;
                    }
                } catch (...) {
                    configValue = value;
                }
            }
            
            entries.push_back({key, configValue, source, "", "", "", false, ""});
        }
    }
    
    return true;
}

// Validate configuration entry against schema
bool Config::validateAgainstSchema(const ConfigEntry& entry, const ConfigSchema& schema) {
    if (entry.key != schema.key) return false;
    
    // Check type matching
    if (schema.type == "string" && !std::holds_alternative<std::string>(entry.value)) return false;
    if (schema.type == "int" && !std::holds_alternative<int>(entry.value)) return false;
    if (schema.type == "int64" && !std::holds_alternative<int64_t>(entry.value)) return false;
    if (schema.type == "double" && !std::holds_alternative<double>(entry.value)) return false;
    if (schema.type == "bool" && !std::holds_alternative<bool>(entry.value)) return false;
    
    // Check validation regex
    if (!schema.validationRegex.empty()) {
        std::string valueStr;
        std::visit([&](auto&& arg) {
            if constexpr (std::is_same_v<decltype(arg), std::string>) {
                valueStr = arg;
            } else {
                std::stringstream ss;
                ss << arg;
                valueStr = ss.str();
            }
        }, entry.value);
        
        std::regex pattern(schema.validationRegex);
        if (!std::regex_match(valueStr, pattern)) {
            return false;
        }
    }
    
    // Check allowed values
    if (!schema.allowedValues.empty()) {
        std::string valueStr;
        std::visit([&](auto&& arg) {
            if constexpr (std::is_same_v<decltype(arg), std::string>) {
                valueStr = arg;
            } else {
                std::stringstream ss;
                ss << arg;
                valueStr = ss.str();
            }
        }, entry.value);
        
        if (std::find(schema.allowedValues.begin(), schema.allowedValues.end(), valueStr) == schema.allowedValues.end()) {
            return false;
        }
    }
    
    // Check min/max values for numeric types
    if (schema.type == "int" || schema.type == "int64" || schema.type == "double") {
        double value;
        std::visit([&](auto&& arg) {
            if constexpr (std::is_same_v<decltype(arg), int>) {
                value = static_cast<double>(arg);
            } else if constexpr (std::is_same_v<decltype(arg), int64_t>) {
                value = static_cast<double>(arg);
            } else if constexpr (std::is_same_v<decltype(arg), double>) {
                value = arg;
            }
        }, entry.value);
        
        if (value < schema.minValue || value > schema.maxValue) {
            return false;
        }
    }
    
    return true;
}

// Merge with inherited configuration entries
void Config::mergeWithInheritedConfig() {
    for (const auto& [key, baseKey] : pImpl->inheritanceChain) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&baseKey](const ConfigEntry& e) { return e.key == baseKey; });
        if (it != pImpl->entries.end()) {
            bool keyExists = false;
            for (const auto& entry : pImpl->entries) {
                if (entry.key == key) {
                    keyExists = true;
                    break;
                }
            }
            if (!keyExists) {
                ConfigEntry inheritedEntry;
                inheritedEntry.key = key;
                inheritedEntry.value = it->value;
                inheritedEntry.source = ConfigSource::Inherited;
                inheritedEntry.description = it->description;
                inheritedEntry.validationRegex = it->validationRegex;
                inheritedEntry.configType = it->configType;
                inheritedEntry.required = it->required;
                inheritedEntry.defaultValue = it->defaultValue;
                
                pImpl->entries.push_back(inheritedEntry);
            }
        }
    }
}

// Configuration implementation
Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default values
    pImpl->baseDir = ".";
    pImpl->backupDir = "backups";
    pImpl->autoSave = false;
    pImpl->enableValidation = true;
    pImpl->maxHistoryEntries = 1000;
    
    // Initialize default validators
    pImpl->validators["string"] = [](const ConfigValue& v) { return std::holds_alternative<std::string>(v); };
    pImpl->validators["int"] = [](const ConfigValue& v) { return std::holds_alternative<int>(v); };
    pImpl->validators["int64"] = [](const ConfigValue& v) { return std::holds_alternative<int64_t>(v); };
    pImpl->validators["double"] = [](const ConfigValue& v) { return std::holds_alternative<double>(v); };
    pImpl->validators["bool"] = [](const ConfigValue& v) { return std::holds_alternative<bool>(v); };
}

Config::~Config() = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Determine file type based on extension
    std::string extension = filepath.substr(filepath.find_last_of(".") + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    bool success = false;
    if (extension == "json") {
        success = parseJSON(content, pImpl->entries, ConfigSource::File);
    } else if (extension == "yaml" || extension == "yml") {
        success = parseYAML(content, pImpl->entries, ConfigSource::File);
    } else {
        // Fall back to legacy key=value format
        std::stringstream ss(content);
        std::string line;
        while (std::getline(ss, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (value.size() >= 2) {
                    char first = value.front();
                    char last = value.back();
                    if ((first == '\"' && last == '\"') || (first == '\'' && last == '\'')) {
                        size_t quoteCount = 0;
                        for (char c : value) {
                            if (c == first) {
                                quoteCount++;
                            } else {
                                break;
                            }
                        }
                        if (quoteCount >= 2) {
                            value = value.substr(1, value.size() - 2);
                        }
                    }
                }
                
                addEntry(key, ConfigValue(value), ConfigSource::File, "");
            }
        }
        success = true;
    }
    
    // Merge with inherited configurations
    mergeWithInheritedConfig();
    
    // Auto-save if enabled
    if (pImpl->autoSave && success) {
        backup();
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                addEntry(key, ConfigValue(value), ConfigSource::CommandLine, "");
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            addEntry(key, ConfigValue(value), ConfigSource::CommandLine, "");
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << formatValue(entry.value) << "\n";
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    addEntry(key, value, source, "");
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

std::optional<std::string> Config::getString(const std::string& key) const {
    const ConfigEntry* entry = findEntry(key);
    if (!entry) return std::nullopt;
    
    if (std::holds_alternative<std::string>(entry->value)) {
        return std::get<std::string>(entry->value);
    }
    return std::nullopt;
}

std::optional<int> Config::getInt(const std::string& key) const {
    const ConfigEntry* entry = findEntry(key);
    if (!entry) return std::nullopt;
    
    if (std::holds_alternative<int>(entry->value)) {
        return std::get<int>(entry->value);
    }
    return std::nullopt;
}

std::optional<int64_t> Config::getInt64(const std::string& key) const {
    const ConfigEntry* entry = findEntry(key);
    if (!entry) return std::nullopt;
    
    if (std::holds_alternative<int64_t>(entry->value)) {
        return std::get<int64_t>(entry->value);
    }
    return std::nullopt;
}

std::optional<double> Config::getDouble(const std::string& key) const {
    const ConfigEntry* entry = findEntry(key);
    if (!entry) return std::nullopt;
    
    if (std::holds_alternative<double>(entry->value)) {
        return std::get<double>(entry->value);
    }
    return std::nullopt;
}

std::optional<bool> Config::getBool(const std::string& key) const {
    const ConfigEntry* entry = findEntry(key);
    if (!entry) return std::nullopt;
    
    if (std::holds_alternative<bool>(entry->value)) {
        return std::get<bool>(entry->value);
    }
    return std::nullopt;
}

std::string Config::getStringOr(const std::string& key, const std::string& defaultValue) const {
    auto val = getString(key);
    return val.has_value() ? val.value() : defaultValue;
}

int Config::getIntOr(const std::string& key, int defaultValue) const {
    auto val = getInt(key);
    return val.has_value() ? val.value() : defaultValue;
}

int64_t Config::getInt64Or(const std::string& key, int64_t defaultValue) const {
    auto val = getInt64(key);
    return val.has_value() ? val.value() : defaultValue;
}

double Config::getDoubleOr(const std::string& key, double defaultValue) const {
    auto val = getDouble(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::getBoolOr(const std::string& key, bool defaultValue) const {
    auto val = getBool(key);
    return val.has_value() ? val.value() : defaultValue;
}

bool Config::has(const std::string& key) const {
    return findEntry(key) != nullptr;
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::remove(const std::string& key) {
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                           [&key](const ConfigEntry& e) { return e.key == key; });
    pImpl->entries.erase(it, pImpl->entries.end());
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Enhanced Configuration System Summary\n";
    oss << "====================================\n";
    oss << "Base directory: " << pImpl->baseDir << "\n";
    oss << "Backup directory: " << pImpl->backupDir << "\n";
    oss << "Validation enabled: " << (pImpl->enableValidation ? "yes" : "no") << "\n";
    oss << "Auto-save enabled: " << (pImpl->autoSave ? "yes" : "no") << "\n";
    oss << "Max history entries: " << pImpl->maxHistoryEntries << "\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "History entries: " << pImpl->history.size() << "\n\n";
    
    oss << "Configuration Entries:\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [" << formatValue(entry.value) << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    return oss.str();
}

bool Config::addSchema(const ConfigSchema& schema) {
    pImpl->schema.push_back(schema);
    return true;
}

bool Config::validateConfig() const {
    if (!pImpl->enableValidation) return true;
    
    std::vector<std::string> errors;
    for (const auto& entry : pImpl->entries) {
        bool isValid = false;
        
        // Check basic type validators
        for (const auto& validator : pImpl->validators) {
            if (validator.second(entry.value)) {
                isValid = true;
                break;
            }
        }
        
        // Check schema validation
        for (const auto& schema : pImpl->schema) {
            if (validateAgainstSchema(entry, schema)) {
                isValid = true;
                break;
            }
        }
        
        if (!isValid) {
            errors.push_back("Invalid value for key: " + entry.key);
        }
    }
    
    if (!errors.empty()) {
        std::cerr << "Configuration validation errors:" << std::endl;
        for (const auto& error : errors) {
            std::cerr << "  " << error << std::endl;
        }
    }
    
    return errors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    if (!validateConfig()) {
        errors.push_back("Configuration validation failed");
    }
    return errors;
}

bool Config::isValueValid(const std::string& key, const ConfigValue& value) const {
    if (!pImpl->enableValidation) return true;
    
    // Check basic type validators
    for (const auto& validator : pImpl->validators) {
        if (validator.second(value)) {
            return true;
        }
    }
    
    // Check schema validation
    for (const auto& schema : pImpl->schema) {
        if (schema.key == key && validateAgainstSchema({key, value, ConfigSource::Programmatic, "", "", "", false, ""}, schema)) {
            return true;
        }
    }
    
    return true;
}

void Config::setInheritance(const std::string& key, const std::string& baseKey) {
    pImpl->inheritanceChain[key] = baseKey;
}

void Config::mergeWithInheritedConfig() {
    for (const auto& [key, baseKey] : pImpl->inheritanceChain) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&baseKey](const ConfigEntry& e) { return e.key == baseKey; });
        if (it != pImpl->entries.end()) {
            bool keyExists = false;
            for (const auto& entry : pImpl->entries) {
                if (entry.key == key) {
                    keyExists = true;
                    break;
                }
            }
            if (!keyExists) {
                ConfigEntry inheritedEntry;
                inheritedEntry.key = key;
                inheritedEntry.value = it->value;
                inheritedEntry.source = ConfigSource::Inherited;
                inheritedEntry.description = it->description;
                inheritedEntry.validationRegex = it->validationRegex;
                inheritedEntry.configType = it->configType;
                inheritedEntry.required = it->required;
                inheritedEntry.defaultValue = it->defaultValue;
                
                pImpl->entries.push_back(inheritedEntry);
            }
        }
    }
}

std::vector<ConfigHistoryEntry> Config::getHistory() const {
    return pImpl->history;
}

std::vector<ConfigHistoryEntry> Config::getHistory(const std::string& key) const {
    std::vector<ConfigHistoryEntry> keyHistory;
    for (const auto& entry : pImpl->history) {
        if (entry.key == key) {
            keyHistory.push_back(entry);
        }
    }
    return keyHistory;
}

bool Config::backup(const std::string& filepath) {
    std::string backupFilepath;
    if (filepath.empty()) {
        backupFilepath = pImpl->baseDir + "/" + pImpl->backupDir + "/config_backup_" + getCurrentTimestamp() + ".cfg";
    } else {
        backupFilepath = filepath;
    }
    
    return saveToFile(backupFilepath);
}

bool Config::restore(const std::string& filepath) {
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    clear();
    return loadFromFile(filepath);
}

std::vector<std::string> Config::getConfigTemplates() const {
    std::vector<std::string> templates;
    
    // Basic configuration template
    templates.push_back("# Basic NLM Configuration Template\n");
    templates.push_back("sim_random_seed = 42\n");
    templates.push_back("sim_timestep = 0.001\n");
    templates.push_back("neuron_count = 1000\n");
    templates.push_back("region_count = 2\n");
    templates.push_back("log_level = INFO\n");
    templates.push_back("log_to_file = false\n\n");
    
    // CPU performance template
    templates.push_back("# CPU Performance Configuration Template\n");
    templates.push_back("cpu_cores = 4\n");
    templates.push_back("cpu_threads_per_core = 2\n");
    templates.push_back("simulation_parallel = true\n\n");
    
    // Memory-intensive template
    templates.push_back("# Memory-Intensive Configuration Template\n");
    templates.push_back("memory_pool_size = 1024\n");
    templates.push_back("cache_enabled = true\n");
    templates.push_back("compression_enabled = true\n\n");
    
    // Visualization template
    templates.push_back("# Visualization Configuration Template\n");
    templates.push_back("visualization_enabled = true\n");
    templates.push_back("visualization_update_rate = 30.0\n");
    templates.push_back("output_format = png\n");
    templates.push_back("output_directory = ./visualizations\n\n");
    
    // Production template
    templates.push_back("# Production Configuration Template\n");
    templates.push_back("log_level = WARNING\n");
    templates.push_back("log_to_file = true\n");
    templates.push_back("log_filename = production.log\n");
    templates.push_back("enable_monitoring = true\n");
    templates.push_back("health_check_interval = 60.0\n");
    
    return templates;
}

bool Config::applyTemplate(const std::string& templateName) {
    std::vector<std::string> templates = getConfigTemplates();
    
    for (const auto& templateContent : templates) {
        if (templateContent.find(templateName) != std::string::npos) {
            // Extract template content
            std::stringstream ss(templateContent);
            std::string line;
            while (std::getline(ss, line)) {
                if (line.empty() || line[0] == '#') {
                    continue;
                }
                
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    addEntry(key, ConfigValue(value), ConfigSource::Default, "");
                }
            }
            return true;
        }
    }
    
    return false;
}

void Config::setChangeCallback(ConfigChangeCallback callback) {
    pImpl->changeCallback = callback;
}

bool Config::importConfig(const std::string& configData) {
    try {
        std::string data = Config::trim(configData);
        
        if (data.find('{') != std::string::npos && data.find('}') != std::string::npos) {
            // JSON format
            return importConfigFromJSON(data);
        } else if (data.find('"') != std::string::npos || data.find(':') != std::string::npos) {
            // YAML format
            return importConfigFromYAML(data);
        } else {
            // Legacy key=value format
            std::stringstream iss(configData);
            std::string line;
            while (std::getline(iss, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#') {
                    continue;
                }
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    addEntry(key, ConfigValue(value), ConfigSource::Programmatic, "");
                }
            }
            return true;
        }
    } catch (const std::exception& e) {
        std::cerr << "Import error: " << e.what() << std::endl;
        return false;
    }
}

bool Config::importConfigFromJSON(const std::string& jsonStr) {
    return parseJSON(jsonStr, pImpl->entries, ConfigSource::Programmatic);
}

bool Config::importConfigFromYAML(const std::string& yamlStr) {
    return parseYAML(yamlStr, pImpl->entries, ConfigSource::Programmatic);
}

std::string Config::diff(const Config& other) const {
    std::ostringstream oss;
    oss << "Configuration Differences:\n";
    oss << "======================\n\n";
    
    // Check for keys in this config not in other
    std::vector<std::string> thisKeys = getKeys();
    std::vector<std::string> otherKeys = other.getKeys();
    
    std::sort(thisKeys.begin(), thisKeys.end());
    std::sort(otherKeys.begin(), otherKeys.end());
    
    std::vector<std::string> onlyInThis;
    std::vector<std::string> onlyInOther;
    std::vector<std::string> different;
    
    std::set_difference(thisKeys.begin(), thisKeys.end(), otherKeys.begin(), otherKeys.end(), std::inserter(onlyInThis, onlyInThis.begin()));
    std::set_difference(otherKeys.begin(), otherKeys.end(), thisKeys.begin(), thisKeys.end(), std::inserter(onlyInOther, onlyInOther.begin()));
    
    for (const auto& key : thisKeys) {
        if (!other.has(key)) continue;
        
        ConfigValue thisValue, otherValue;
        const ConfigEntry* thisEntry = findEntry(key);
        const ConfigEntry* otherEntry = other.findEntry(key);
        
        if (thisEntry && otherEntry) {
            thisValue = thisEntry->value;
            otherValue = otherEntry->value;
            
            if (thisValue != otherValue) {
                different.push_back(key);
            }
        }
    }
    
    if (!onlyInThis.empty()) {
        oss << "Keys only in current configuration:\n";
        for (const auto& key : onlyInThis) {
            oss << "  " << key << " = " << formatValue(findEntry(key)->value) << "\n";
        }
        oss << "\n";
    }
    
    if (!onlyInOther.empty()) {
        oss << "Keys only in other configuration:\n";
        for (const auto& key : onlyInOther) {
            const ConfigEntry* otherEntry = other.findEntry(key);
            oss << "  " << key << " = " << other.formatValue(otherEntry->value) << "\n";
        }
        oss << "\n";
    }
    
    if (!different.empty()) {
        oss << "Keys with different values:\n";
        for (const auto& key : different) {
            const ConfigEntry* thisEntry = findEntry(key);
            const ConfigEntry* otherEntry = other.findEntry(key);
            oss << "  " << key << ":\n";
            oss << "    current = " << formatValue(thisEntry->value) << "\n";
            oss << "    other   = " << other.formatValue(otherEntry->value) << "\n";
        }
    }
    
    if (onlyInThis.empty() && onlyInOther.empty() && different.empty()) {
        oss << "Configurations are identical.\n";
    }
    
    return oss.str();
}

bool Config::equals(const Config& other) const {
    return this->diff(other).find("Configurations are identical") != std::string::npos;
}

std::string Config::exportConfig(const std::string& format) const {
    std::transform(format.begin(), format.end(), format.begin(), ::tolower);
    
    if (format == "json") {
        std::ostringstream oss;
        oss << "{\n";
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            oss << "  \"" << entry.key << "\": ";
            
            std::visit([&oss](auto&& arg) {
                if constexpr (std::is_same_v<decltype(arg), std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<decltype(arg), int>) {
                    oss << arg;
                } else if constexpr (std::is_same_v<decltype(arg), int64_t>) {
                    oss << arg;
                } else if constexpr (std::is_same_v<decltype(arg), double>) {
                    oss << arg;
                } else if constexpr (std::is_same_v<decltype(arg), bool>) {
                    oss << (arg ? "true" : "false");
                }
            }, entry.value);
            
            if (i < pImpl->entries.size() - 1) {
                oss << ",\n";
            } else {
                oss << "\n";
            }
        }
        oss << "}\n";
        return oss.str();
    } else if (format == "yaml" || format == "yml") {
        std::ostringstream oss;
        oss << "# Configuration in YAML format\n";
        for (const auto& entry : pImpl->entries) {
            oss << entry.key << ": ";
            
            std::visit([&oss](auto&& arg) {
                if constexpr (std::is_same_v<decltype(arg), std::string>) {
                    oss << "\"" << arg << "\"\n";
                } else {
                    oss << arg << "\n";
                }
            }, entry.value);
        }
        return oss.str();
    } else {
        // Default to legacy format
        return exportConfig();
    }
}

bool Config::addEntry(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description) {
    if (!isValidKey(key)) {
        return false;
    }
    
    ConfigValue oldValue;
    bool existed = has(key);
    if (existed) {
        const ConfigEntry* entry = findEntry(key);
        oldValue = entry->value;
    }
    
    ConfigEntry newEntry;
    newEntry.key = key;
    newEntry.value = value;
    newEntry.source = source;
    newEntry.description = description;
    
    if (existed) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != pImpl->entries.end()) {
            oldValue = it->value;
            it->value = value;
            it->source = source;
        }
    } else {
        pImpl->entries.push_back(newEntry);
    }
    
    if (existed && oldValue != value) {
        // Add to history
        ConfigHistoryEntry historyEntry;
        historyEntry.timestamp = getCurrentTimestamp();
        historyEntry.key = key;
        historyEntry.oldValue = oldValue;
        historyEntry.newValue = value;
        historyEntry.action = "set";
        
        pImpl->history.push_back(historyEntry);
        
        // Limit history size
        if (pImpl->history.size() > pImpl->maxHistoryEntries) {
            pImpl->history.erase(pImpl->history.begin());
        }
        
        notifyChange(key, oldValue, value);
    }
    
    return true;
}

const ConfigEntry* Config::findEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    return (it != pImpl->entries.end()) ? &*it : nullptr;
}

bool Config::updateEntry(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    ConfigValue oldValue = it->value;
    it->value = value;
    it->source = source;
    
    if (oldValue != value) {
        // Add to history
        ConfigHistoryEntry historyEntry;
        historyEntry.timestamp = getCurrentTimestamp();
        historyEntry.key = key;
        historyEntry.oldValue = oldValue;
        historyEntry.newValue = value;
        historyEntry.action = "set";
        
        pImpl->history.push_back(historyEntry);
        
        // Limit history size
        if (pImpl->history.size() > pImpl->maxHistoryEntries) {
            pImpl->history.erase(pImpl->history.begin());
        }
        
        notifyChange(key, oldValue, value);
    }
    
    return true;
}

void Config::notifyChange(const std::string& key, const ConfigValue& oldValue, const ConfigValue& newValue) {
    if (pImpl->changeCallback) {
        pImpl->changeCallback(key, oldValue, newValue);
    }
}

std::string Config::formatValue(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool Config::isValidKey(const std::string& key) {
    if (key.empty()) return false;
    if (!std::isalpha(key[0]) && key[0] != '_') return false;
    return std::all_of(key.begin(), key.end(),
                      [](char c) { return std::isalnum(c) || c == '_'; });
}

} // namespace nlm