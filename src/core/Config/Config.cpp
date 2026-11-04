#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <numeric>
#include <functional>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Enhanced configuration with validation and diagnostics
    std::map<std::string, ValidationRule> validationRules;
    std::vector<std::string> configurationHistory;
    size_t accessCount = 0;
    double averageAccessTime = 0.0;
    std::chrono::system_clock::time_point lastAccessed;
    size_t memoryUsage = 0;
    std::map<std::string, std::string> backups;
    std::string currentChecksum;
    bool isCorrupted = false;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default validation rules
    addDefaultValidationRules();
    lastAccessed = std::chrono::system_clock::now();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Validate file path before attempting to open
    if (!validateFilePath(filepath)) {
        return false;
    }
    
    // Check if file exists and is readable
    if (!std::filesystem::exists(filepath) || !std::filesystem::is_regular_file(filepath)) {
        return false;
    }
    
    try {
        // Create backup before loading
        std::string backupPath = filepath + ".backup." + std::to_string(std::time(nullptr));
        if (!saveToFile(backupPath)) {
            // Backup failed but continue
        }
        
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        size_t lineNumber = 0;
        bool hasContent = false;
        
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse simple key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Validate key format
                if (key.empty() || !validateKey(key)) {
                    continue; // Skip invalid entries
                }
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Set value with validation
                auto validationResult = validateConfigValue(key, value);
                if (validationResult.isValid()) {
                    set(key, value, ConfigSource::File);
                    trackConfigurationChange("Loaded from file: " + key + " = " + value);
                    hasContent = true;
                } else {
                    // Log validation warnings
                    for (const auto& warning : validationResult.warnings) {
                        // Log warning
                    }
                    if (validationResult.hasErrors()) {
                        // Log error but continue processing other entries
                    }
                }
            }
        }
        
        // Calculate checksum for integrity check
        currentChecksum = calculateChecksum();
        
        // Update performance metrics
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - lastAccessed).count();
        averageAccessTime = (averageAccessTime * accessCount + duration) / (accessCount + 1);
        accessCount++;
        lastAccessed = now;
        
        return hasContent;
        
    } catch (const std::exception& e) {
        // Log exception
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    // Validate and set
                    auto validationResult = validateConfigValue(key, value);
                    if (validationResult.isValid()) {
                        set(key, value, ConfigSource::CommandLine);
                        trackConfigurationChange("Loaded from args: " + key + " = " + value);
                    }
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                // Validate and set
                auto validationResult = validateConfigValue(key, value);
                if (validationResult.isValid()) {
                    set(key, value, ConfigSource::CommandLine);
                    trackConfigurationChange("Loaded from args: " + key + " = " + value);
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        // Log exception
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Create backup if file exists
        if (std::filesystem::exists(filepath)) {
            std::string backupPath = filepath + ".auto_backup." + std::to_string(std::time(nullptr));
            if (!saveToFile(backupPath)) {
                // Log backup failure but continue
            }
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write configuration header
        file << "# NLM Configuration File\n";
        file << "# Generated on: " << getCurrentTimestamp() << "\n";
        file << "# Checksum: " << currentChecksum << "\n\n";
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << "# Source: " << static_cast<int>(entry.source) << "\n";
            
            // Format value based on type
            file << entry.key << " = ";
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                     std::is_same_v<T, std::vector<double>> ||
                                     std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i) file << ", ";
                        if constexpr (std::is_same_v<T, std::vector<std::string>) {
                            file << "\"" << arg[i] << "\"";
                        } else {
                            file << arg[i];
                        }
                    }
                    file << "]";
                } else {
                    file << arg;
                }
            }, entry.value);
            file << "\n\n";
        }
        
        return true;
        
    } catch (const std::exception& e) {
        // Log exception
        return false;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key format
    if (!validateKey(key)) {
        throw ConfigurationException("Invalid configuration key: " + key);
    }
    
    // Validate value based on key
    auto validationResult = validateConfigValue(key, value);
    if (!validationResult.isValid() && validationResult.hasErrors()) {
        throw ValidationRuleException("Configuration validation failed: " + validationResult.errors.front());
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        
        // Update checksum
        currentChecksum = calculateChecksum();
    } else {
        pImpl->entries.emplace_back(key, value, source);
        
        // Update checksum
        currentChecksum = calculateChecksum();
        
        // Track change
        trackConfigurationChange("Added new key: " + key);
    }
    
    // Update performance metrics
    auto now = std::chrono::system_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - lastAccessed).count();
    averageAccessTime = (averageAccessTime * accessCount + duration) / (accessCount + 1);
    accessCount++;
    lastAccessed = now;
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

bool Config::has(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    return it != pImpl->entries.end();
}

void Config::remove(const std::string& key) {
    // Validate key before removal
    if (!validateKey(key)) {
        throw ConfigurationException("Invalid configuration key: " + key);
    }
    
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    // Update checksum
    currentChecksum = calculateChecksum();
    
    // Track change
    trackConfigurationChange("Removed key: " + key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    // Create backup before clearing
    std::string backupPath = "config_backup_clear." + std::to_string(std::time(nullptr));
    saveToFile(backupPath);
    
    pImpl->entries.clear();
    currentChecksum = calculateChecksum();
    isCorrupted = false;
    
    trackConfigurationChange("Cleared all configuration");
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    oss << "Checksum: " << currentChecksum << "\n";
    oss << "Last accessed: " << getTimeString(lastAccessed) << "\n";
    oss << "Access count: " << accessCount << "\n";
    oss << "Average access time: " << averageAccessTime << " microseconds\n";
    oss << "Memory usage: " << memoryUsage << " bytes\n\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        oss << "\"" << arg[i] << "\"";
                    } else {
                        oss << arg[i];
                    }
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

ValidationResult Config::validate() const {
    ValidationResult result;
    
    // Validate each entry
    for (const auto& entry : pImpl->entries) {
        auto validationResult = validateConfigValue(entry.key, entry.value);
        
        if (!validationResult.isValid()) {
            if (validationResult.hasErrors()) {
                result.errors.push_back("Key '" + entry.key + "': " + validationResult.errors.front());
                result.status = ValidationResult::Status::Critical;
            }
            if (validationResult.hasWarnings()) {
                for (const auto& warning : validationResult.warnings) {
                    result.warnings.push_back("Key '" + entry.key + "': " + warning);
                }
                if (result.status == ValidationResult::Status::Valid) {
                    result.status = ValidationResult::Status::Warning;
                }
            }
        }
    }
    
    // Check for configuration integrity
    if (currentChecksum != calculateChecksum()) {
        result.errors.push_back("Configuration integrity check failed - checksum mismatch");
        result.status = ValidationResult::Status::Critical;
    }
    
    // Validate with custom rules
    for (const auto& rule : pImpl->validationRules) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&rule](const ConfigEntry& e) { return e.key == rule.key; });
        if (it != pImpl->entries.end()) {
            if (!rule.validator(it->value)) {
                if (rule.isCritical) {
                    result.errors.push_back("Validation rule failed for key '" + rule.key + "': " + rule.errorMessage);
                    result.status = ValidationResult::Status::Critical;
                } else {
                    result.warnings.push_back("Validation rule warning for key '" + rule.key + "': " + rule.warningMessage);
                    if (result.status == ValidationResult::Status::Valid) {
                        result.status = ValidationResult::Status::Warning;
                    }
                }
            }
        }
    }
    
    // Calculate confidence
    double totalChecks = pImpl->entries.size() + pImpl->validationRules.size();
    double passedChecks = totalChecks - (result.errors.size() + result.warnings.size());
    result.confidence = totalChecks > 0 ? (passedChecks / totalChecks) : 1.0;
    
    result.getSummary();
    
    return result;
}

ValidationResult Config::validateSection(const std::string& section) const {
    ValidationResult result;
    
    // Simple section validation based on section name
    if (section == "brain") {
        return validateBrainConfiguration();
    } else if (section == "simulation") {
        return validateSimulationConfiguration();
    } else if (section == "memory") {
        return validateMemoryConfiguration();
    } else if (section == "neuromodulation") {
        return validateNeuromodulationConfiguration();
    }
    
    // Generic section validation
    result.warnings.push_back("Unknown section: " + section + " - no specific validation rules");
    return result;
}

ValidationResult Config::validateWithRules(const std::vector<ValidationRule>& rules) const {
    ValidationResult result;
    
    // Clear existing validation rules
    pImpl->validationRules.clear();
    
    // Add new rules
    for (const auto& rule : rules) {
        pImpl->validationRules[rule.key] = rule;
    }
    
    // Run validation with new rules
    return validate();
}

void Config::addValidationRule(const ValidationRule& rule) {
    pImpl->validationRules[rule.key] = rule;
}

void Config::addDefaultValidationRules() {
    // Add default validation rules for common keys
    pImpl->validationRules["neuron_count"] = ValidationRule(
        "neuron_count",
        [](const ConfigValue& value) -> bool {
            auto opt = std::get_if<int64_t>(&value);
            return opt && (*opt > 0 && *opt <= 100000);
        },
        "Neuron count must be between 1 and 100,000",
        "Neuron count is outside recommended range",
        true
    );
    
    pImpl->validationRules["simulation_timestep"] = ValidationRule(
        "simulation_timestep",
        [](const ConfigValue& value) -> bool {
            auto opt = std::get_if<double>(&value);
            return opt && (*opt > 0.0 && *opt <= 1.0);
        },
        "Simulation timestep must be between 0.0 and 1.0 seconds",
        "Simulation timestep outside typical range",
        true
    );
    
    pImpl->validationRules["connection_probability"] = ValidationRule(
        "connection_probability",
        [](const ConfigValue& value) -> bool {
            auto opt = std::get_if<double>(&value);
            return opt && (*opt >= 0.0 && *opt <= 1.0);
        },
        "Connection probability must be between 0.0 and 1.0",
        "Connection probability outside typical range",
        false
    );
    
    pImpl->validationRules["region_count"] = ValidationRule(
        "region_count",
        [](const ConfigValue& value) -> bool {
            auto opt = std::get_if<int64_t>(&value);
            return opt && (*opt > 0 && *opt <= 100);
        },
        "Region count must be between 1 and 100",
        "Region count outside typical range",
        false
    );
}

bool Config::isHealthy() const {
    auto validationResult = validate();
    return validationResult.isValid() && !isCorrupted;
}

std::vector<std::string> Config::getHealthIssues() const {
    std::vector<std::string> issues;
    
    auto validationResult = validate();
    
    if (validationResult.hasErrors()) {
        for (const auto& error : validationResult.errors) {
            issues.push_back("Validation Error: " + error);
        }
    }
    
    if (validationResult.hasWarnings()) {
        for (const auto& warning : validationResult.warnings) {
            issues.push_back("Validation Warning: " + warning);
        }
    }
    
    if (isCorrupted) {
        issues.push_back("Configuration is marked as corrupted");
    }
    
    if (currentChecksum != calculateChecksum()) {
        issues.push_back("Configuration integrity check failed");
    }
    
    return issues;
}

Config::PerformanceMetrics Config::getPerformanceMetrics() const {
    PerformanceMetrics metrics;
    metrics.totalAccesses = accessCount;
    metrics.averageAccessTime = averageAccessTime;
    metrics.lastAccessed = lastAccessed;
    metrics.memoryUsage = memoryUsage;
    
    if (accessCount > 0) {
        metrics.cacheHits = static_cast<size_t>(averageAccessTime / 10.0); // Approximation
    }
    
    return metrics;
}

std::string Config::diff(const Config& other) const {
    std::ostringstream oss;
    oss << "Configuration Comparison:\n";
    
    // Find keys in this config but not in other
    std::set<std::string> thisKeys(getKeys());
    std::set<std::string> otherKeys(other.getKeys());
    
    std::set<std::string> onlyInThis;
    std::set_difference(thisKeys.begin(), thisKeys.end(), otherKeys.begin(), otherKeys.end(),
                        std::inserter(onlyInThis, onlyInThis.begin()));
    
    std::set<std::string> onlyInOther;
    std::set_difference(otherKeys.begin(), otherKeys.end(), thisKeys.begin(), otherKeys.end(),
                        std::inserter(onlyInOther, onlyInOther.begin()));
    
    std::set<std::string> commonKeys;
    std::set_intersection(thisKeys.begin(), thisKeys.end(), otherKeys.begin(), otherKeys.end(),
                         std::inserter(commonKeys, commonKeys.begin()));
    
    oss << "Keys only in this config:" << std::endl;
    for (const auto& key : onlyInThis) {
        oss << "  " << key << std::endl;
    }
    
    oss << "\nKeys only in other config:" << std::endl;
    for (const auto& key : onlyInOther) {
        oss << "  " << key << std::endl;
    }
    
    oss << "\nCommon keys with differences:" << std::endl;
    for (const auto& key : commonKeys) {
        auto it1 = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                               [&key](const ConfigEntry& e) { return e.key == key; });
        auto it2 = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                               [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it1 != pImpl->entries.end() && it2 != other.pImpl->entries.end()) {
            if (it1->value != it2->value) {
                oss << "  " << key << ": " << (std::get<std::string>(it1->value)) << " != " 
                    << (std::get<std::string>(it2->value)) << std::endl;
            }
        }
    }
    
    return oss.str();
}

bool Config::merge(const Config& other, bool overwrite) {
    try {
        for (const auto& entry : other.pImpl->entries) {
            if (overwrite || !has(entry.key)) {
                set(entry.key, entry.value, entry.source);
            }
        }
        
        // Update checksum after merge
        currentChecksum = calculateChecksum();
        trackConfigurationChange("Merged with another configuration");
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::createBackup(const std::string& backupPath) const {
    try {
        if (!saveToFile(backupPath)) {
            return false;
        }
        
        pImpl->backups[backupPath] = getCurrentTimestamp();
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::restoreFromBackup(const std::string& backupPath) {
    try {
        if (!std::filesystem::exists(backupPath)) {
            return false;
        }
        
        // Clear current configuration
        clear();
        
        // Load from backup
        if (!loadFromFile(backupPath)) {
            return false;
        }
        
        // Mark as restored
        trackConfigurationChange("Restored from backup: " + backupPath);
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

std::vector<std::string> Config::getBackupFiles() const {
    std::vector<std::string> backups;
    for (const auto& pair : pImpl->backups) {
        backups.push_back(pair.first);
    }
    return backups;
}

void Config::trackConfigurationChange(const std::string& changeDescription) {
    pImpl->configurationHistory.push_back(getCurrentTimestamp() + " - " + changeDescription);
    
    // Keep history manageable (last 1000 changes)
    if (pImpl->configurationHistory.size() > 1000) {
        pImpl->configurationHistory.erase(
            pImpl->configurationHistory.begin(),
            pImpl->configurationHistory.begin() + 100
        );
    }
}

std::vector<std::string> Config::getConfigurationHistory() const {
    return pImpl->configurationHistory;
}

bool Config::compress() {
    try {
        // Simple compression: remove duplicate consecutive entries
        std::vector<ConfigEntry> compressed;
        std::string lastKey;
        
        for (const auto& entry : pImpl->entries) {
            if (entry.key != lastKey) {
                compressed.push_back(entry);
                lastKey = entry.key;
            }
        }
        
        pImpl->entries = std::move(compressed);
        
        // Update checksum
        currentChecksum = calculateChecksum();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::optimize() {
    try {
        // Simple optimization: reorder entries by key
        std::sort(pImpl->entries.begin(), pImpl->entries.end(),
                  [](const ConfigEntry& a, const ConfigEntry& b) {
                      return a.key < b.key;
                  });
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

ValidationResult Config::validateBrainConfiguration() const {
    ValidationResult result;
    
    // Validate brain-specific parameters
    auto neuronCount = get<int64_t>("neuron_count");
    if (!neuronCount) {
        result.errors.push_back("Missing required parameter: neuron_count");
        result.status = ValidationResult::Status::Critical;
    } else if (*neuronCount <= 0) {
        result.errors.push_back("Invalid neuron_count: must be positive");
        result.status = ValidationResult::Status::Critical;
    } else if (*neuronCount > 100000) {
        result.warnings.push_back("Large neuron count may impact performance");
        if (result.status == ValidationResult::Status::Valid) {
            result.status = ValidationResult::Status::Warning;
        }
    }
    
    auto regionCount = get<int64_t>("region_count");
    if (!regionCount) {
        result.warnings.push_back("region_count not set - using default");
    } else if (*regionCount <= 0) {
        result.errors.push_back("Invalid region_count: must be positive");
        result.status = ValidationResult::Status::Critical;
    }
    
    auto connectionProb = get<double>("connection_probability");
    if (!connectionProb) {
        result.warnings.push_back("connection_probability not set - using default");
    } else if (*connectionProb < 0.0 || *connectionProb > 1.0) {
        result.errors.push_back("Invalid connection_probability: must be between 0.0 and 1.0");
        result.status = ValidationResult::Status::Critical;
    }
    
    // Validate optional brain parameters
    auto developmentEnabled = get<bool>("development_enabled");
    if (developmentEnabled && !*developmentEnabled) {
        result.warnings.push_back("Development system is disabled");
        if (result.status == ValidationResult::Status::Valid) {
            result.status = ValidationResult::Status::Warning;
        }
    }
    
    return result;
}

ValidationResult Config::validateSimulationConfiguration() const {
    ValidationResult result;
    
    auto timestep = get<double>("simulation_timestep");
    if (!timestep) {
        result.errors.push_back("Missing required parameter: simulation_timestep");
        result.status = ValidationResult::Status::Critical;
    } else if (*timestep <= 0.0) {
        result.errors.push_back("Invalid simulation_timestep: must be positive");
        result.status = ValidationResult::Status::Critical;
    } else if (*timestep > 1.0) {
        result.warnings.push_back("Large timestep may affect simulation accuracy");
        if (result.status == ValidationResult::Status::Valid) {
            result.status = ValidationResult::Status::Warning;
        }
    }
    
    auto maxSteps = get<int64_t>("max_steps");
    if (!maxSteps) {
        result.warnings.push_back("max_steps not set - using default");
    } else if (*maxSteps <= 0) {
        result.errors.push_back("Invalid max_steps: must be positive");
        result.status = ValidationResult::Status::Critical;
    }
    
    return result;
}

ValidationResult Config::validateMemoryConfiguration() const {
    ValidationResult result;
    
    // Memory configuration validation would go here
    // For now, just check for memory-related parameters
    auto memoryCapacity = get<int64_t>("memory_capacity");
    if (memoryCapacity && *memoryCapacity <= 0) {
        result.errors.push_back("Invalid memory_capacity: must be positive");
        result.status = ValidationResult::Status::Critical;
    }
    
    return result;
}

ValidationResult Config::validateNeuromodulationConfiguration() const {
    ValidationResult result;
    
    // Neuromodulation configuration validation would go here
    // For now, just check for neuromodulation-related parameters
    auto neuromodulationEnabled = get<bool>("neuromodulation_enabled");
    if (neuromodulationEnabled && !*neuromodulationEnabled) {
        result.warnings.push_back("Neuromodulation system is disabled");
        if (result.status == ValidationResult::Status::Valid) {
            result.status = ValidationResult::Status::Warning;
        }
    }
    
    return result;
}

std::vector<std::string> Config::getRecommendations() const {
    std::vector<std::string> recommendations;
    
    auto validationResult = validate();
    
    if (!validationResult.hasErrors()) {
        recommendations.push_back("Configuration is valid and ready for use");
    }
    
    if (validationResult.hasWarnings()) {
        recommendations.push_back("Address validation warnings to improve configuration stability");
    }
    
    auto neuronCount = get<int64_t>("neuron_count");
    if (neuronCount && *neuronCount > 10000) {
        recommendations.push_back("Large neuron count detected - consider performance implications");
    }
    
    auto timestep = get<double>("simulation_timestep");
    if (timestep && *timestep > 0.01) {
        recommendations.push_back("Large timestep may affect simulation accuracy - consider smaller values");
    }
    
    return recommendations;
}

std::string Config::exportConfiguration() const {
    std::ostringstream oss;
    oss << "# NLM Configuration Export\n";
    oss << "# Exported on: " << getCurrentTimestamp() << "\n\n";
    
    oss << "config {\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << ";\n";
    }
    oss << "}\n";
    
    return oss.str();
}

bool Config::importConfiguration(const std::string& configString) {
    try {
        // Parse configuration string
        std::istringstream iss(configString);
        std::string line;
        
        bool inConfigBlock = false;
        std::string currentKey;
        
        while (std::getline(iss, line)) {
            line = trim(line);
            
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            if (line == "config {") {
                inConfigBlock = true;
                continue;
            }
            
            if (inConfigBlock && line == "}") {
                inConfigBlock = false;
                continue;
            }
            
            if (inConfigBlock && line.find('=') != std::string::npos) {
                size_t pos = line.find('=');
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (!value.empty() && value.back() == ';') {
                    value = value.substr(0, value.size() - 1);
                }
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value);
            }
        }
        
        // Update checksum
        currentChecksum = calculateChecksum();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::checkIntegrity() const {
    return currentChecksum == calculateChecksum() && !isCorrupted;
}

std::string Config::calculateChecksum() const {
    std::hash<std::string> hasher;
    std::string checksumData;
    
    for (const auto& entry : pImpl->entries) {
        checksumData += entry.key + ":";
        std::visit([&checksumData](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                checksumData += arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                checksumData += (arg ? "true" : "false");
            } else {
                checksumData += std::to_string(arg);
            }
            checksumData += ";";
        }, entry.value);
    }
    
    return std::to_string(hasher(checksumData));
}

bool Config::recoverFromCorruption(const std::string& corruptionPath) {
    try {
        // Mark as corrupted
        isCorrupted = true;
        
        // Try to restore from backup
        if (!backupPath.empty() && std::filesystem::exists(backupPath)) {
            clear();
            loadFromFile(backupPath);
            
            // Update checksum
            currentChecksum = calculateChecksum();
            isCorrupted = false;
            
            trackConfigurationChange("Recovered from corruption: " + corruptionPath);
            
            return true;
        }
        
        // If no backup, try to load default configuration
        auto defaultConfig = std::make_shared<Config>();
        *this = std::move(*defaultConfig);
        
        trackConfigurationChange("Recovered from corruption with default config");
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Validation helper functions
ValidationResult Config::validateConfigValue(const std::string& key, const ConfigValue& value) {
    ValidationResult result;
    
    // Validate key format
    if (!validateKey(key)) {
        result.errors.push_back("Invalid key format: " + key);
        return result;
    }
    
    // Validate value based on key
    if (key == "neuron_count") {
        auto opt = std::get_if<int64_t>(&value);
        if (!opt || *opt <= 0) {
            result.errors.push_back("Neuron count must be a positive integer");
        } else if (*opt > 100000) {
            result.warnings.push_back("Neuron count is very large - may impact performance");
        }
    } else if (key == "simulation_timestep") {
        auto opt = std::get_if<double>(&value);
        if (!opt || *opt <= 0.0) {
            result.errors.push_back("Simulation timestep must be positive");
        } else if (*opt > 1.0) {
            result.warnings.push_back("Simulation timestep is very large - may affect accuracy");
        }
    } else if (key == "connection_probability") {
        auto opt = std::get_if<double>(&value);
        if (!opt || *opt < 0.0 || *opt > 1.0) {
            result.errors.push_back("Connection probability must be between 0.0 and 1.0");
        }
    }
    
    return result;
}

bool Config::validateKey(const std::string& key) {
    // Key validation rules
    if (key.empty()) {
        return false;
    }
    
    // Check for valid characters
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-') {
            return false;
        }
    }
    
    return true;
}

bool Config::validateFilePath(const std::string& path) {
    // Basic file path validation
    if (path.empty() || path.find('\0') != std::string::npos) {
        return false;
    }
    
    // Check for directory traversal attempts
    if (path.find("..") != std::string::npos) {
        return false;
    }
    
    return true;
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << ms.count();
    
    return ss.str();
}

std::string Config::getTimeString(const std::chrono::system_clock::time_point& tp) {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string ValidationResult::getSummary() const {
    std::stringstream ss;
    ss << "Validation Result: ";
    
    switch (status) {
        case Status::Valid:
            ss << "PASSED";
            break;
        case Status::Warning:
            ss << "PASSED WITH WARNINGS";
            break;
        case Status::Invalid:
            ss << "FAILED";
            break;
        case Status::Critical:
            ss << "CRITICAL FAILURE";
            break;
    }
    
    ss << " (Confidence: " << (confidence * 100) << "%)";
    
    if (!messages.empty()) {
        ss << " - " << messages[0];
    }
    
    return ss.str();
}

std::string ValidationResult::getFullReport() const {
    std::stringstream ss;
    ss << "=== Validation Report ===\n";
    ss << "Status: " << getSummary() << "\n";
    ss << "Timestamp: " << getCurrentTimestamp() << "\n\n";
    
    if (!messages.empty()) {
        ss << "Messages:" << std::endl;
        for (const auto& msg : messages) {
            ss << "  - " << msg << std::endl;
        }
        ss << std::endl;
    }
    
    if (!errors.empty()) {
        ss << "Errors:" << std::endl;
        for (const auto& error : errors) {
            ss << "  - " << error << std::endl;
        }
        ss << std::endl;
    }
    
    if (!warnings.empty()) {
        ss << "Warnings:" << std::endl;
        for (const auto& warning : warnings) {
            ss << "  - " << warning << std::endl;
        }
        ss << std::endl;
    }
    
    if (!details.empty()) {
        ss << "Details:" << std::endl;
        for (const auto& pair : details) {
            ss << "  " << pair.first << ": " << pair.second << std::endl;
        }
    }
    
    return ss.str();
}

namespace ConfigValidation {
    
    ComparisonResult compare(const Config& config1, const Config& config2) {
        ComparisonResult result;
        
        auto keys1 = config1.getKeys();
        auto keys2 = config2.getKeys();
        
        std::set<std::string> set1(keys1.begin(), keys1.end());
        std::set<std::string> set2(keys2.begin(), keys2.end());
        
        std::set_symmetric_difference(set1.begin(), set1.end(),
                                       set2.begin(), set2.end(),
                                       std::inserter(result.differences, result.differences.begin()));
        
        std::set_intersection(set1.begin(), set1.end(),
                              set2.begin(), set2.end(),
                              std::inserter(result.similarities, result.similarities.begin()));
        
        result.isIdentical = set1 == set2;
        result.similarityScore = result.similarities.size() / static_cast<double>(set1.size() + set2.size());
        
        return result;
    }
    
    ValidationResult validateBrainConfig(const Config& config) {
        ValidationResult result;
        
        auto neuronCount = config.get<int64_t>("neuron_count");
        if (!neuronCount) {
            result.errors.push_back("Missing required parameter: neuron_count");
        } else if (*neuronCount <= 0) {
            result.errors.push_back("Invalid neuron_count: must be positive");
        }
        
        return result;
    }
    
    ValidationResult validateSimulationConfig(const Config& config) {
        ValidationResult result;
        
        auto timestep = config.get<double>("simulation_timestep");
        if (!timestep) {
            result.errors.push_back("Missing required parameter: simulation_timestep");
        } else if (*timestep <= 0.0) {
            result.errors.push_back("Invalid simulation_timestep: must be positive");
        }
        
        return result;
    }
    
    ValidationResult validateMemoryConfig(const Config& config) {
        ValidationResult result;
        
        // Memory configuration validation
        return result;
    }
    
    ValidationResult validateNeuromodulationConfig(const Config& config) {
        ValidationResult result;
        
        // Neuromodulation configuration validation
        return result;
    }
    
    Config fixConfiguration(const Config& config) {
        Config fixed = config;
        
        // Fix common issues
        auto neuronCount = config.get<int64_t>("neuron_count");
        if (!neuronCount || *neuronCount <= 0) {
            fixed.set("neuron_count", static_cast<int64_t>(100), ConfigSource::Runtime);
        }
        
        auto timestep = config.get<double>("simulation_timestep");
        if (!timestep || *timestep <= 0.0) {
            fixed.set("simulation_timestep", 0.001, ConfigSource::Runtime);
        }
        
        return fixed;
    }
    
    std::vector<std::string> getConfigurationFixes(const Config& config) {
        std::vector<std::string> fixes;
        
        auto validationResult = config.validate();
        
        if (validationResult.hasErrors()) {
            fixes.push_back("Fix configuration errors");
        }
        
        if (validationResult.hasWarnings()) {
            fixes.push_back("Address validation warnings");
        }
        
        auto neuronCount = config.get<int64_t>("neuron_count");
        if (neuronCount && *neuronCount > 10000) {
            fixes.push_back("Reduce neuron count for better performance");
        }
        
        return fixes;
    }
    
    std::vector<std::string> getAvailableScenarios() {
        return {"quick", "balanced", "full"};
    }
    
    Config createScenarioConfig(const std::string& scenario) {
        Config config;
        
        if (scenario == "quick") {
            config.set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
            config.set("simulation_timestep", 0.001, ConfigSource::Default);
            config.set("connection_probability", 0.05f, ConfigSource::Default);
        } else if (scenario == "balanced") {
            config.set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            config.set("simulation_timestep", 0.001, ConfigSource::Default);
            config.set("connection_probability", 0.1f, ConfigSource::Default);
        } else if (scenario == "full") {
            config.set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
            config.set("simulation_timestep", 0.001, ConfigSource::Default);
            config.set("connection_probability", 0.15f, ConfigSource::Default);
        }
        
        return config;
    }
}

} // namespace nlm