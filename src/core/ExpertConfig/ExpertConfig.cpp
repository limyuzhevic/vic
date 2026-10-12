#pragma once

#include "core/ExpertConfig/ExpertConfig.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <unordered_set>
#include <chrono>
#include <thread>
#include <iomanip>
#include <numeric>
#include <cmath>
#include <regex>
#include <sstream>
#include <map>
#include <set>

namespace nlm {

ExpertConfig::ExpertConfig() {
    initialize();
}

ExpertConfig::~ExpertConfig() {
    cleanup();
}

ExpertConfig::ExpertConfig(ExpertConfig&& other) noexcept {
    pImpl = std::move(other.pImpl);
}

ExpertConfig& ExpertConfig::operator=(ExpertConfig&& other) noexcept {
    if (this != &other) {
        cleanup();
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

void ExpertConfig::initialize() {
    pImpl = std::make_unique<Impl>();
    
    // Initialize default settings
    pImpl->hotReloadEnabled = false;
    pImpl->webUIEnabled = false;
    pImpl->performanceMonitoringEnabled = false;
    pImpl->memoryTrackingEnabled = false;
    pImpl->auditLoggingEnabled = false;
    pImpl->historyEnabled = false;
    pImpl->isInCluster = false;
    pImpl->syncStatus = "not_synced";
    
    // Initialize expert features
    pImpl->expertFeaturesEnabled = true;
    pImpl->scriptingSupported = true;
    pImpl->macroSupportEnabled = true;
    pImpl->secretsManagementEnabled = true;
    pImpl->hotReloadEnabled = false;
    pImpl->webUIEnabled = false;
    pImpl->distributionEnabled = true;
    pImpl->advancedTypesEnabled = true;
    pImpl->uiFeaturesEnabled = true;
    
    // Initialize analytics
    pImpl->totalConfigures = 0;
    pImpl->totalExports = 0;
    pImpl->totalImports = 0;
    pImpl->totalValidates = 0;
    pImpl->totalOptimizations = 0;
    pImpl->totalAnalyses = 0;
    pImpl->totalPrescriptions = 0;
}

void ExpertConfig::cleanup() {
    if (pImpl) {
        pImpl->hotReloadListeners.clear();
        pImpl->customFunctions.clear();
        pImpl->macroDefinitions.clear();
        pImpl->templateFunctions.clear();
        pImpl->neuralNetworkConfigs.clear();
        pImpl->simulationConfigs.clear();
        pImpl->experimentConfigs.clear();
        pImpl->performanceProfiles.clear();
        pImpl->memoryProfiles.clear();
    }
}

// Configuration value management
void ExpertConfig::set(const std::string& key, const ExpertConfigValue& value) {
    pImpl->configValues[key] = value;
    logOperation("set", key, "success");
    
    // Notify hot reload listeners
    if (pImpl->hotReloadEnabled) {
        notifyHotReload(key, value);
    }
}

void ExpertConfig::set(const std::string& key, const std::string& value) {
    set(key, ExpertConfigValue(value));
}

void ExpertConfig::set(const std::string& key, int value) {
    set(key, ExpertConfigValue(value));
}

void ExpertConfig::set(const std::string& key, double value) {
    set(key, ExpertConfigValue(value));
}

void ExpertConfig::set(const std::string& key, bool value) {
    set(key, ExpertConfigValue(value));
}

ExpertConfigValue ExpertConfig::get(const std::string& key) const {
    auto it = pImpl->configValues.find(key);
    if (it != pImpl->configValues.end()) {
        return it->second;
    }
    throw ConfigError("Key not found: " + key, ConfigError::ErrorType::MissingKey);
}

bool ExpertConfig::has(const std::string& key) const {
    return pImpl->configValues.find(key) != pImpl->configValues.end();
}

void ExpertConfig::remove(const std::string& key) {
    if (pImpl->configValues.erase(key) > 0) {
        logOperation("remove", key, "success");
        
        // Notify hot reload listeners
        if (pImpl->hotReloadEnabled) {
            ExpertConfigValue emptyValue;
            notifyHotReload(key, emptyValue);
        }
    }
}

std::vector<std::string> ExpertConfig::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->configValues.size());
    for (const auto& pair : pImpl->configValues) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::vector<std::string> ExpertConfig::getKeysByCategory(const std::string& category) const {
    std::vector<std::string> keys;
    for (const auto& pair : pImpl->configValues) {
        auto it = pImpl->entryMetadata.find(pair.first);
        if (it != pImpl->entryMetadata.end() && it->second.category == category) {
            keys.push_back(pair.first);
        }
    }
    return keys;
}

std::vector<std::string> ExpertConfig::getKeysByType(const std::string& type) const {
    std::vector<std::string> keys;
    for (const auto& pair : pImpl->configValues) {
        auto it = pImpl->entryMetadata.find(pair.first);
        if (it != pImpl->entryMetadata.end() && it->second.type == type) {
            keys.push_back(pair.first);
        }
    }
    return keys;
}

// Configuration templates
std::string ExpertConfig::createTemplate(const ConfigTemplate& templateConfig) {
    std::string templateName = templateConfig.name;
    pImpl->templates[templateName] = templateConfig;
    
    logOperation("create_template", templateName, "success");
    return templateName;
}

bool ExpertConfig::updateTemplate(const std::string& templateName, const ConfigTemplate& newTemplate) {
    auto it = pImpl->templates.find(templateName);
    if (it == pImpl->templates.end()) {
        return false;
    }
    
    it->second = newTemplate;
    logOperation("update_template", templateName, "success");
    return true;
}

bool ExpertConfig::deleteTemplate(const std::string& templateName) {
    size_t count = pImpl->templates.erase(templateName);
    if (count > 0) {
        logOperation("delete_template", templateName, "success");
        return true;
    }
    return false;
}

bool ExpertConfig::addTemplate(const ConfigTemplate& templateConfig) {
    return createTemplate(templateConfig) == templateConfig.name;
}

std::optional<ConfigTemplate> ExpertConfig::getTemplate(const std::string& templateName) const {
    auto it = pImpl->templates.find(templateName);
    if (it != pImpl->templates.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<ConfigTemplate> ExpertConfig::getAllTemplates() const {
    std::vector<ConfigTemplate> templates;
    templates.reserve(pImpl->templates.size());
    for (const auto& pair : pImpl->templates) {
        templates.push_back(pair.second);
    }
    return templates;
}

std::vector<ConfigTemplate> ExpertConfig::getTemplatesByCategory(const std::string& category) const {
    std::vector<ConfigTemplate> templates;
    for (const auto& pair : pImpl->templates) {
        if (pair.second.category == category) {
            templates.push_back(pair.second);
        }
    }
    return templates;
}

std::vector<ConfigTemplate> ExpertConfig::getTemplatesByType(const std::string& type) const {
    std::vector<ConfigTemplate> templates;
    for (const auto& pair : pImpl->templates) {
        if (pair.second.type == type) {
            templates.push_back(pair.second);
        }
    }
    return templates;
}

// Configuration presets
std::string ExpertConfig::createPreset(const ConfigPreset& preset) {
    std::string presetName = preset.name;
    pImpl->presets[presetName] = preset;
    
    // Update last modified
    pImpl->presets[presetName].lastModified = std::chrono::system_clock::now();
    
    logOperation("create_preset", presetName, "success");
    return presetName;
}

bool ExpertConfig::updatePreset(const std::string& presetName, const ConfigPreset& newPreset) {
    auto it = pImpl->presets.find(presetName);
    if (it == pImpl->presets.end()) {
        return false;
    }
    
    it->second = newPreset;
    it->second.lastModified = std::chrono::system_clock::now();
    
    logOperation("update_preset", presetName, "success");
    return true;
}

bool ExpertConfig::deletePreset(const std::string& presetName) {
    size_t count = pImpl->presets.erase(presetName);
    if (count > 0) {
        logOperation("delete_preset", presetName, "success");
        return true;
    }
    return false;
}

bool ExpertConfig::addPreset(const ConfigPreset& preset) {
    return createPreset(preset) == preset.name;
}

std::optional<ConfigPreset> ExpertConfig::getPreset(const std::string& presetName) const {
    auto it = pImpl->presets.find(presetName);
    if (it != pImpl->presets.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<ConfigPreset> ExpertConfig::getAllPresets() const {
    std::vector<ConfigPreset> presets;
    presets.reserve(pImpl->presets.size());
    for (const auto& pair : pImpl->presets) {
        presets.push_back(pair.second);
    }
    return presets;
}

std::vector<ConfigPreset> ExpertConfig::getPresetsByAuthor(const std::string& author) const {
    std::vector<ConfigPreset> presets;
    for (const auto& pair : pImpl->presets) {
        if (pair.second.author == author) {
            presets.push_back(pair.second);
        }
    }
    return presets;
}

std::vector<ConfigPreset> ExpertConfig::getPresetsByTags(const std::string& tags) const {
    std::vector<ConfigPreset> presets;
    for (const auto& pair : pImpl->presets) {
        if (pair.second.tags.find(tags) != std::string::npos) {
            presets.push_back(pair.second);
        }
    }
    return presets;
}

// Configuration inheritance
bool ExpertConfig::applyPreset(const std::string& presetName) {
    auto preset = getPreset(presetName);
    if (!preset) {
        return false;
    }
    
    // Clear current config
    clear();
    
    // Apply preset values
    for (const auto& pair : preset->values) {
        set(pair.first, pair.second);
    }
    
    logOperation("apply_preset", presetName, "success");
    return true;
}

bool ExpertConfig::mergePreset(const std::string& presetName, bool overwrite) {
    auto preset = getPreset(presetName);
    if (!preset) {
        return false;
    }
    
    for (const auto& pair : preset->values) {
        if (overwrite || !has(pair.first)) {
            set(pair.first, pair.second);
        }
    }
    
    logOperation("merge_preset", presetName, "success");
    return true;
}

std::unordered_map<std::string, ExpertConfigValue> ExpertConfig::inheritFromPreset(const std::string& presetName) const {
    std::unordered_map<std::string, ExpertConfigValue> result;
    auto preset = getPreset(presetName);
    if (preset) {
        result = preset->values;
    }
    return result;
}

bool ExpertConfig::resolveDependencies(const std::string& templateName) {
    auto templateConfig = getTemplate(templateName);
    if (!templateConfig) {
        return false;
    }
    
    // Implement dependency resolution logic here
    // For now, just return true if template exists
    return true;
}

// Configuration comparison and diffing
ConfigDiffResult ExpertConfig::diffWith(const std::string& otherConfigPath) const {
    ExpertConfig otherConfig;
    otherConfig.loadFromFile(otherConfigPath);
    return diffWith(otherConfig);
}

ConfigDiffResult ExpertConfig::diffWith(const ExpertConfig& otherConfig) const {
    ConfigDiffResult result;
    
    // Get all keys from both configs
    std::set<std::string> allKeys;
    for (const auto& pair : pImpl->configValues) {
        allKeys.insert(pair.first);
    }
    for (const auto& pair : otherConfig.pImpl->configValues) {
        allKeys.insert(pair.first);
    }
    
    // Compare each key
    for (const auto& key : allKeys) {
        ExpertConfigValue value1, value2;
        bool has1 = false, has2 = false;
        
        if (pImpl->configValues.find(key) != pImpl->configValues.end()) {
            value1 = pImpl->configValues.at(key);
            has1 = true;
        }
        
        if (otherConfig.pImpl->configValues.find(key) != otherConfig.pImpl->configValues.end()) {
            value2 = otherConfig.pImpl->configValues.at(key);
            has2 = true;
        }
        
        if (has1 && has2) {
            if (value1 != value2) {
                result.modifiedKeys[key] = std::make_pair(value1, value2);
            } else {
                result.unchangedKeys[key] = value1;
            }
        } else if (has1) {
            result.addedKeys[key] = std::make_pair(value1, ExpertConfigValue());
        } else if (has2) {
            result.removedKeys[key] = std::make_pair(ExpertConfigValue(), value2);
        }
    }
    
    // Calculate similarity score
    size_t totalKeys = allKeys.size();
    size_t unchangedCount = result.unchangedKeys.size();
    size_t removedCount = result.removedKeys.size();
    
    if (totalKeys > 0) {
        result.similarityScore = static_cast<double>(unchangedCount) / static_cast<double>(totalKeys);
    }
    
    result.isIdentical = (result.addedKeys.empty() && result.removedKeys.empty() && result.modifiedKeys.empty());
    
    return result;
}

double ExpertConfig::calculateSimilarity(const std::string& otherConfigPath) const {
    ExpertConfig otherConfig;
    otherConfig.loadFromFile(otherConfigPath);
    return calculateSimilarity(otherConfig);
}

double ExpertConfig::calculateSimilarity(const ExpertConfig& otherConfig) const {
    ConfigDiffResult diff = diffWith(otherConfig);
    return diff.similarityScore;
}

std::string ExpertConfig::generateDiffReport(const ConfigDiffResult& diff) const {
    std::stringstream report;
    
    report << "Configuration Comparison Report" << std::endl;
    report << "=============================" << std::endl;
    
    if (diff.isIdentical) {
        report << "Configurations are identical." << std::endl;
        return report.str();
    }
    
    if (!diff.addedKeys.empty()) {
        report << "Added Keys:" << std::endl;
        for (const auto& pair : diff.addedKeys) {
            report << "  " << pair.first << " = " << valueToString(pair.second.first) << std::endl;
        }
        report << std::endl;
    }
    
    if (!diff.removedKeys.empty()) {
        report << "Removed Keys:" << std::endl;
        for (const auto& pair : diff.removedKeys) {
            report << "  " << pair.first << " = " << valueToString(pair.second.second) << std::endl;
        }
        report << std::endl;
    }
    
    if (!diff.modifiedKeys.empty()) {
        report << "Modified Keys:" << std::endl;
        for (const auto& pair : diff.modifiedKeys) {
            report << "  " << pair.first << ":" << std::endl;
            report << "    Old: " << valueToString(pair.second.first) << std::endl;
            report << "    New: " << valueToString(pair.second.second) << std::endl;
        }
        report << std::endl;
    }
    
    report << std::endl;
    report << "Similarity Score: " << (diff.similarityScore * 100) << "%" << std::endl;
    
    return report.str();
}

// Configuration history and versioning
bool ExpertConfig::enableHistory(bool enable) {
    bool oldValue = pImpl->historyEnabled;
    pImpl->historyEnabled = enable;
    
    if (enable && !oldValue) {
        // Create initial history entry
        ConfigHistoryEntry entry;
        entry.version = "v1.0.0";
        entry.author = "system";
        entry.operation = "create";
        entry.description = "Initial configuration creation";
        entry.snapshot = pImpl->configValues;
        entry.checksum = generateChecksum(pImpl->configValues);
        
        pImpl->history.push_back(entry);
    }
    
    logOperation("enable_history", "history", (enable ? "enabled" : "disabled"));
    return oldValue;
}

bool ExpertConfig::addHistoryEntry(const ConfigHistoryEntry& entry) {
    if (!pImpl->historyEnabled) {
        return false;
    }
    
    pImpl->history.push_back(entry);
    logOperation("add_history_entry", entry.version, "success");
    return true;
}

bool ExpertConfig::addHistoryEntry(const std::string& version, const std::string& author, const std::string& operation, const std::string& description) {
    ConfigHistoryEntry entry;
    entry.version = version;
    entry.author = author;
    entry.operation = operation;
    entry.description = description;
    entry.snapshot = pImpl->configValues;
    entry.checksum = generateChecksum(pImpl->configValues);
    
    return addHistoryEntry(entry);
}

std::optional<ConfigHistoryEntry> ExpertConfig::getHistoryEntry(const std::string& version) const {
    for (const auto& entry : pImpl->history) {
        if (entry.version == version) {
            return entry;
        }
    }
    return std::nullopt;
}

std::vector<ConfigHistoryEntry> ExpertConfig::getHistory() const {
    return pImpl->history;
}

std::vector<ConfigHistoryEntry> ExpertConfig::getHistoryByOperation(const std::string& operation) const {
    std::vector<ConfigHistoryEntry> result;
    for (const auto& entry : pImpl->history) {
        if (entry.operation == operation) {
            result.push_back(entry);
        }
    }
    return result;
}

std::vector<ConfigHistoryEntry> ExpertConfig::getHistoryByAuthor(const std::string& author) const {
    std::vector<ConfigHistoryEntry> result;
    for (const auto& entry : pImpl->history) {
        if (entry.author == author) {
            result.push_back(entry);
        }
    }
    return result;
}

bool ExpertConfig::rollback(const std::string& version) {
    auto entry = getHistoryEntry(version);
    if (!entry) {
        return false;
    }
    
    // Clear current config
    clear();
    
    // Restore from history entry
    pImpl->configValues = entry->snapshot;
    
    logOperation("rollback", version, "success");
    return true;
}

bool ExpertConfig::createVersion(const std::string& version, const std::string& author, const std::string& description) {
    ConfigHistoryEntry entry;
    entry.version = version;
    entry.author = author;
    entry.operation = "version_create";
    entry.description = description;
    entry.snapshot = pImpl->configValues;
    entry.checksum = generateChecksum(pImpl->configValues);
    
    return addHistoryEntry(entry);
}

bool ExpertConfig::compareVersions(const std::string& version1, const std::string& version2) {
    auto entry1 = getHistoryEntry(version1);
    auto entry2 = getHistoryEntry(version2);
    
    if (!entry1 || !entry2) {
        return false;
    }
    
    ConfigDiffResult diff;
    diff = diffWith(entry1->snapshot);
    
    logOperation("compare_versions", version1 + " vs " + version2, "success");
    return true;
}

// Configuration audit trail
bool ExpertConfig::enableAudit(bool enable) {
    bool oldValue = pImpl->auditLoggingEnabled;
    pImpl->auditLoggingEnabled = enable;
    
    logOperation("enable_audit", "audit", (enable ? "enabled" : "disabled"));
    return oldValue;
}

bool ExpertConfig::logAudit(const ConfigAuditEntry& entry) {
    if (!pImpl->auditLoggingEnabled) {
        return false;
    }
    
    pImpl->auditLogs.push_back(entry);
    logOperation("log_audit", entry.operation, entry.result, entry.details);
    return true;
}

bool ExpertConfig::logAudit(const std::string& userId, const std::string& operation, const std::string& target, const std::string& action, const std::string& result, const std::string& details) {
    ConfigAuditEntry entry;
    entry.userId = userId;
    entry.operation = operation;
    entry.target = target;
    entry.action = action;
    entry.result = result;
    entry.details = details;
    entry.checksum = generateChecksum(pImpl->configValues);
    
    return logAudit(entry);
}

std::vector<ConfigAuditEntry> ExpertConfig::getAuditLogs() const {
    return pImpl->auditLogs;
}

std::vector<ConfigAuditEntry> ExpertConfig::getAuditLogsByUser(const std::string& userId) const {
    std::vector<ConfigAuditEntry> result;
    for (const auto& entry : pImpl->auditLogs) {
        if (entry.userId == userId) {
            result.push_back(entry);
        }
    }
    return result;
}

std::vector<ConfigAuditEntry> ExpertConfig::getAuditLogsByOperation(const std::string& operation) const {
    std::vector<ConfigAuditEntry> result;
    for (const auto& entry : pImpl->auditLogs) {
        if (entry.operation == operation) {
            result.push_back(entry);
        }
    }
    return result;
}

std::vector<ConfigAuditEntry> ExpertConfig::getAuditLogsByTimeRange(const std::chrono::system_clock::time_point& start, const std::chrono::system_clock::time_point& end) const {
    std::vector<ConfigAuditEntry> result;
    for (const auto& entry : pImpl->auditLogs) {
        if (entry.timestamp >= start && entry.timestamp <= end) {
            result.push_back(entry);
        }
    }
    return result;
}

bool ExpertConfig::exportAuditLogs(const std::string& filepath, const std::string& format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "json") {
        nlohmann::json auditJson;
        for (const auto& entry : pImpl->auditLogs) {
            nlohmann::json entryJson;
            entryJson["timestamp"] = std::chrono::system_clock::to_time_t(entry.timestamp);
            entryJson["userId"] = entry.userId;
            entryJson["operation"] = entry.operation;
            entryJson["target"] = entry.target;
            entryJson["action"] = entry.action;
            entryJson["result"] = entry.result;
            entryJson["details"] = entry.details;
            entryJson["checksum"] = entry.checksum;
            auditJson.push_back(entryJson);
        }
        
        file << auditJson.dump(2);
    } else {
        // Default to text format
        for (const auto& entry : pImpl->auditLogs) {
            file << entry.timestamp << " | " << entry.userId << " | " << entry.operation 
                 << " | " << entry.target << " | " << entry.action << " | " << entry.result
                 << " | " << entry.details << std::endl;
        }
    }
    
    logOperation("export_audit_logs", filepath, "success");
    return true;
}

// Configuration validation
bool ExpertConfig::validate(const std::string& key) const {
    // Basic validation - check if key exists
    return pImpl->configValues.find(key) != pImpl->configValues.end();
}

bool ExpertConfig::validateStrict(const std::string& key) const {
    // Strict validation - check if key exists and has a valid value
    auto it = pImpl->configValues.find(key);
    if (it == pImpl->configValues.end()) {
        return false;
    }
    
    // TODO: Add more strict validation logic
    return true;
}

bool ExpertConfig::validateAll() const {
    bool allValid = true;
    for (const auto& pair : pImpl->configValues) {
        if (!validate(pair.first)) {
            allValid = false;
        }
    }
    return allValid;
}

bool ExpertConfig::validateAllStrict() const {
    bool allValid = true;
    for (const auto& pair : pImpl->configValues) {
        if (!validateStrict(pair.first)) {
            allValid = false;
        }
    }
    return allValid;
}

std::string ExpertConfig::validateAndReport() const {
    std::stringstream report;
    report << "Configuration Validation Report" << std::endl;
    report << "==============================" << std::endl;
    
    bool allValid = validateAll();
    report << "Overall Status: " << (allValid ? "VALID" : "INVALID") << std::endl;
    report << std::endl;
    
    for (const auto& pair : pImpl->configValues) {
        bool valid = validate(pair.first);
        report << pair.first << ": " << (valid ? "VALID" : "INVALID") << std::endl;
    }
    
    return report.str();
}

std::string ExpertConfig::validateStrictAndReport() const {
    std::stringstream report;
    report << "Strict Configuration Validation Report" << std::endl;
    report << "==================================" << std::endl;
    
    bool allValid = validateAllStrict();
    report << "Overall Status: " << (allValid ? "VALID" : "INVALID") << std::endl;
    report << std::endl;
    
    for (const auto& pair : pImpl->configValues) {
        bool valid = validateStrict(pair.first);
        report << pair.first << ": " << (valid ? "VALID" : "INVALID") << std::endl;
    }
    
    return report.str();
}

// Configuration export/import
bool ExpertConfig::exportConfig(const std::string& filepath, const std::string& format, bool includeSecrets) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "json") {
        nlohmann::json configJson;
        for (const auto& pair : pImpl->configValues) {
            configJson[pair.first] = expertConfigValueToJSON(pair.second);
        }
        
        file << configJson.dump(2);
    } else if (format == "yaml") {
        // TODO: Implement YAML export
        return false;
    } else {
        // Default to key=value format
        for (const auto& pair : pImpl->configValues) {
            file << pair.first << " = " << valueToString(pair.second) << std::endl;
        }
    }
    
    logOperation("export_config", filepath, "success");
    pImpl->totalExports++;
    return true;
}

bool ExpertConfig::importConfig(const std::string& filepath, const std::string& format, bool merge, bool validateOnImport) {
    ExpertConfig importedConfig;
    
    if (format == "json") {
        // TODO: Implement JSON import
        return false;
    } else if (format == "yaml") {
        // TODO: Implement YAML import
        return false;
    } else {
        // Default to key=value format
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line.find("=") == std::string::npos) {
                continue;
            }
            
            size_t pos = line.find("=");
            std::string key = trim(line.substr(0, pos));
            std::string valueStr = trim(line.substr(pos + 1));
            
            // Remove quotes
            if (valueStr.size() >= 2 && ((valueStr.front() == '"' && valueStr.back() == '"') ||
                (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            ExpertConfigValue value = stringToExpertConfigValue(valueStr);
            importedConfig.set(key, value);
        }
    }
    
    if (merge) {
        // Merge with current config
        for (const auto& pair : importedConfig.pImpl->configValues) {
            if (overwrite || !has(pair.first)) {
                set(pair.first, pair.second);
            }
        }
    } else {
        // Replace current config
        pImpl->configValues = importedConfig.pImpl->configValues;
    }
    
    if (validateOnImport && !validateAll()) {
        return false;
    }
    
    logOperation("import_config", filepath, "success");
    pImpl->totalImports++;
    return true;
}

std::string ExpertConfig::exportConfigString(const std::string& format, bool includeSecrets) const {
    if (format == "json") {
        nlohmann::json configJson;
        for (const auto& pair : pImpl->configValues) {
            configJson[pair.first] = expertConfigValueToJSON(pair.second);
        }
        
        return configJson.dump(2);
    } else if (format == "yaml") {
        // TODO: Implement YAML export to string
        return "";
    } else {
        // Default to key=value format
        std::stringstream ss;
        for (const auto& pair : pImpl->configValues) {
            ss << pair.first << " = " << valueToString(pair.second) << std::endl;
        }
        return ss.str();
    }
}

bool ExpertConfig::importConfigString(const std::string& configString, const std::string& format, bool merge, bool validateOnImport) {
    ExpertConfig importedConfig;
    
    if (format == "json") {
        nlohmann::json configJson = nlohmann::json::parse(configString);
        for (const auto& pair : configJson.items()) {
            importedConfig.set(pair.key(), jsonToExpertConfigValue(pair.value()));
        }
    } else if (format == "yaml") {
        // TODO: Implement YAML import from string
        return false;
    } else {
        // Default to key=value format
        std::istringstream iss(configString);
        std::string line;
        while (std::getline(iss, line)) {
            line = trim(line);
            if (line.empty() || line.find("=") == std::string::npos) {
                continue;
            }
            
            size_t pos = line.find("=");
            std::string key = trim(line.substr(0, pos));
            std::string valueStr = trim(line.substr(pos + 1));
            
            // Remove quotes
            if (valueStr.size() >= 2 && ((valueStr.front() == '"' && valueStr.back() == '"') ||
                (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            ExpertConfigValue value = stringToExpertConfigValue(valueStr);
            importedConfig.set(key, value);
        }
    }
    
    if (merge) {
        // Merge with current config
        for (const auto& pair : importedConfig.pImpl->configValues) {
            if (overwrite || !has(pair.first)) {
                set(pair.first, pair.second);
            }
        }
    } else {
        // Replace current config
        pImpl->configValues = importedConfig.pImpl->configValues;
    }
    
    if (validateOnImport && !validateAll()) {
        return false;
    }
    
    logOperation("import_config_string", "config_string", "success");
    pImpl->totalImports++;
    return true;
}

// Configuration comparison
ConfigDiffResult ExpertConfig::compare(const std::string& otherConfigPath) const {
    return diffWith(otherConfigPath);
}

ConfigDiffResult ExpertConfig::compare(const ExpertConfig& otherConfig) const {
    return diffWith(otherConfig);
}

std::string ExpertConfig::compareAndReport(const std::string& otherConfigPath) const {
    ConfigDiffResult diff = compare(otherConfigPath);
    return generateDiffReport(diff);
}

std::string ExpertConfig::compareAndReport(const ExpertConfig& otherConfig) const {
    ConfigDiffResult diff = compare(otherConfig);
    return generateDiffReport(diff);
}

// Configuration profiling
std::string ExpertConfig::profile() const {
    std::stringstream report;
    report << "Configuration Profile" << std::endl;
    report << "==================" << std::endl;
    report << "Total Keys: " << pImpl->configValues.size() << std::endl;
    report << "History Enabled: " << (pImpl->historyEnabled ? "Yes" : "No") << std::endl;
    report << "Audit Enabled: " << (pImpl->auditLoggingEnabled ? "Yes" : "No") << std::endl;
    report << "Hot Reload Enabled: " << (pImpl->hotReloadEnabled ? "Yes" : "No") << std::endl;
    report << "Web UI Enabled: " << (pImpl->webUIEnabled ? "Yes" : "No") << std::endl;
    report << std::endl;
    
    report << "Configuration Categories:" << std::endl;
    std::unordered_map<std::string, int> categoryCounts;
    for (const auto& pair : pImpl->configValues) {
        auto it = pImpl->entryMetadata.find(pair.first);
        if (it != pImpl->entryMetadata.end()) {
            categoryCounts[it->second.category]++;
        }
    }
    
    for (const auto& pair : categoryCounts) {
        report << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    return report.str();
}

std::string ExpertConfig::profilePerformance() const {
    std::stringstream report;
    report << "Performance Profile" << std::endl;
    report << "=================" << std::endl;
    report << "Total Operations: " << pImpl->totalConfigures + pImpl->totalExports + pImpl->totalImports + pImpl->totalValidates << std::endl;
    report << "Configuration Operations: " << pImpl->totalConfigures << std::endl;
    report << "Export Operations: " << pImpl->totalExports << std::endl;
    report << "Import Operations: " << pImpl->totalImports << std::endl;
    report << "Validation Operations: " << pImpl->totalValidates << std::endl;
    report << "Optimization Operations: " << pImpl->totalOptimizations << std::endl;
    report << "Analysis Operations: " << pImpl->totalAnalyses << std::endl;
    report << "Prescription Operations: " << pImpl->totalPrescriptions << std::endl;
    
    return report.str();
}

std::string ExpertConfig::profileMemory() const {
    std::stringstream report;
    report << "Memory Profile" << std::endl;
    report << "=============" << std::endl;
    
    // Estimate memory usage
    size_t totalMemory = 0;
    for (const auto& pair : pImpl->configValues) {
        totalMemory += sizeof(std::string) + sizeof(ExpertConfigValue);
    }
    
    report << "Estimated Memory Usage: " << totalMemory << " bytes" << std::endl;
    report << "Configuration Values: " << pImpl->configValues.size() << std::endl;
    report << "Templates: " << pImpl->templates.size() << std::endl;
    report << "Presets: " << pImpl->presets.size() << std::endl;
    report << "History Entries: " << pImpl->history.size() << std::endl;
    report << "Audit Logs: " << pImpl->auditLogs.size() << std::endl;
    
    return report.str();
}

std::string ExpertConfig::profileValidation() const {
    std::stringstream report;
    report << "Validation Profile" << std::endl;
    report << "=================" << std::endl;
    report << "Total Keys: " << pImpl->configValues.size() << std::endl;
    report << "Valid Keys: " << count_if(pImpl->configValues.begin(), pImpl->configValues.end(), 
        [this](const auto& pair) { return validate(pair.first); }) << std::endl;
    report << "Validation Rate: " << 
        (pImpl->configValues.empty() ? 0.0 : static_cast<double>(count_if(pImpl->configValues.begin(), pImpl->configValues.end(), 
            [this](const auto& pair) { return validate(pair.first); })) / pImpl->configValues.size() * 100) << "%" << std::endl;
    
    return report.str();
}

std::string ExpertConfig::profileDependencies() const {
    std::stringstream report;
    report << "Dependency Profile" << std::endl;
    report << "====================" << std::endl;
    
    // Count templates with dependencies
    int templatesWithDeps = 0;
    for (const auto& pair : pImpl->templates) {
        if (!pair.second.dependencies.empty()) {
            templatesWithDeps++;
        }
    }
    
    report << "Templates: " << pImpl->templates.size() << std::endl;
    report << "Templates with Dependencies: " << templatesWithDeps << std::endl;
    report << "Presets with Dependencies: " << 
        count_if(pImpl->presets.begin(), pImpl->presets.end(),
            [](const auto& pair) { return !pair.second.basePresets.empty(); }) << std::endl;
    
    return report.str();
}

// Configuration optimization
bool ExpertConfig::optimize() {
    // Basic optimization - remove unused entries
    // TODO: Implement more sophisticated optimization
    return true;
}

bool ExpertConfig::optimizeMemory() {
    // Memory optimization
    // TODO: Implement memory optimization
    return true;
}

bool ExpertConfig::optimizePerformance() {
    // Performance optimization
    // TODO: Implement performance optimization
    return true;
}

bool ExpertConfig::optimizeValidation() {
    // Validation optimization
    // TODO: Implement validation optimization
    return true;
}

bool ExpertConfig::optimizeDependencies() {
    // Dependency optimization
    // TODO: Implement dependency optimization
    return true;
}

std::string ExpertConfig::getOptimizationReport() const {
    std::stringstream report;
    report << "Optimization Report" << std::endl;
    report << "==================" << std::endl;
    report << "Memory Optimization: " << (optimizeMemory() ? "Enabled" : "Disabled") << std::endl;
    report << "Performance Optimization: " << (optimizePerformance() ? "Enabled" : "Disabled") << std::endl;
    report << "Validation Optimization: " << (optimizeValidation() ? "Enabled" : "Disabled") << std::endl;
    report << "Dependency Optimization: " << (optimizeDependencies() ? "Enabled" : "Disabled") << std::endl;
    
    return report.str();
}

// Configuration analysis
std::string ExpertConfig::analyze() const {
    std::stringstream report;
    report << "Configuration Analysis Report" << std::endl;
    report << "==========================" << std::endl;
    report << "Type Analysis:" << std::endl;
    
    std::unordered_map<std::string, int> typeCounts;
    for (const auto& pair : pImpl->configValues) {
        std::string type = getValueType(pair.second);
        typeCounts[type]++;
    }
    
    for (const auto& pair : typeCounts) {
        report << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    report << std::endl << "Value Analysis:" << std::endl;
    report << "  Min Value: " << getMinValue() << std::endl;
    report << "  Max Value: " << getMaxValue() << std::endl;
    report << "  Average Value: " << getAverageValue() << std::endl;
    
    return report.str();
}

std::string ExpertConfig::analyzeUsage() const {
    std::stringstream report;
    report << "Usage Analysis Report" << std::endl;
    report << "====================" << std::endl;
    report << "Most Used Keys: " << std::endl;
    // TODO: Implement usage analysis
    report << "  [TODO: Usage analysis not implemented]" << std::endl;
    
    return report.str();
}

std::string ExpertConfig::analyzeDependencies() const {
    std::stringstream report;
    report << "Dependency Analysis Report" << std::endl;
    report << "==========================" << std::endl;
    report << "Dependency Graph: " << std::endl;
    // TODO: Implement dependency analysis
    report << "  [TODO: Dependency analysis not implemented]" << std::endl;
    
    return report.str();
}

std::string ExpertConfig::analyzePerformance() const {
    std::stringstream report;
    report << "Performance Analysis Report" << std::endl;
    report << "==========================" << std::endl;
    report << "Performance Metrics: " << std::endl;
    // TODO: Implement performance analysis
    report << "  [TODO: Performance analysis not implemented]" << std::endl;
    
    return report.str();
}

std::string ExpertConfig::analyzeMemory() const {
    std::stringstream report;
    report << "Memory Analysis Report" << std::endl;
    report << "=====================" << std::endl;
    report << "Memory Usage: " << std::endl;
    // TODO: Implement memory analysis
    report << "  [TODO: Memory analysis not implemented]" << std::endl;
    
    return report.str();
}

std::string ExpertConfig::analyzeValidation() const {
    std::stringstream report;
    report << "Validation Analysis Report" << std::endl;
    report << "=========================" << std::endl;
    report << "Validation Metrics: " << std::endl;
    // TODO: Implement validation analysis
    report << "  [TODO: Validation analysis not implemented]" << std::endl;
    
    return report.str();
}

std::unordered_map<std::string, double> ExpertConfig::calculateMetrics() const {
    std::unordered_map<std::string, double> metrics;
    metrics["validity_rate"] = (pImpl->configValues.empty() ? 0.0 : 
        static_cast<double>(count_if(pImpl->configValues.begin(), pImpl->configValues.end(),
            [this](const auto& pair) { return validate(pair.first); })) / pImpl->configValues.size());
    metrics["completeness_rate"] = 1.0; // TODO: Implement completeness calculation
    metrics["dependency_score"] = 0.0; // TODO: Implement dependency score
    metrics["performance_score"] = 0.0; // TODO: Implement performance score
    metrics["memory_efficiency"] = 0.0; // TODO: Implement memory efficiency
    
    return metrics;
}

// Configuration scripting
bool ExpertConfig::executeScript(const std::string& script, const std::string& language) {
    if (language == "javascript") {
        return executeJavaScript(script, *this);
    } else if (language == "lua") {
        return executeLua(script, *this);
    } else if (language == "python") {
        return executePython(script, *this);
    }
    return false;
}

std::string ExpertConfig::evaluateScript(const std::string& script, const std::string& language) const {
    if (language == "javascript") {
        return expert_config::executeJavaScript(script, *this);
    } else if (language == "lua") {
        return expert_config::executeLua(script, *this);
    } else if (language == "python") {
        return expert_config::executePython(script, *this);
    }
    return "Unsupported language: " + language;
}

bool ExpertConfig::loadScriptFromFile(const std::string& filepath, const std::string& language) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    std::string script = buffer.str();
    return executeScript(script, language);
}

bool ExpertConfig::registerCustomFunction(const std::string& name, const std::function<ExpertConfigValue(const std::vector<ExpertConfigValue>&)>& func) {
    pImpl->customFunctions[name] = func;
    return true;
}

// Configuration macros and templates
std::string ExpertConfig::defineMacro(const std::string& name, const std::string& templateString) {
    pImpl->macroDefinitions[name] = templateString;
    return name;
}

ExpertConfigValue ExpertConfig::expandMacro(const std::string& macroName, const std::unordered_map<std::string, ExpertConfigValue>& context) const {
    auto it = pImpl->macroDefinitions.find(macroName);
    if (it == pImpl->macroDefinitions.end()) {
        return ExpertConfigValue(); // Empty value
    }
    
    // TODO: Implement macro expansion
    return ExpertConfigValue();
}

bool ExpertConfig::loadMacroFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // TODO: Implement macro file parsing
    return false;
}

bool ExpertConfig::registerTemplateFunction(const std::string& name, const std::function<ExpertConfigValue(const std::vector<ExpertConfigValue>&)>& func) {
    pImpl->templateFunctions[name] = func;
    return true;
}

// Configuration secrets and environment variable handling
bool ExpertConfig::setSecret(const std::string& key, const std::string& value, bool encrypt) {
    // TODO: Implement encryption
    set(key, value);
    
    auto& entry = pImpl->entryMetadata[key];
    entry.isSecret = true;
    entry.encrypted = encrypt;
    
    logOperation("set_secret", key, "success");
    return true;
}

std::string ExpertConfig::getSecret(const std::string& key) const {
    auto it = pImpl->entryMetadata.find(key);
    if (it == pImpl->entryMetadata.end() || !it->second.isSecret) {
        return "";
    }
    
    auto configIt = pImpl->configValues.find(key);
    if (configIt == pImpl->configValues.end()) {
        return "";
    }
    
    // TODO: Implement decryption
    return std::get<std::string>(configIt->second);
}

bool ExpertConfig::isSecret(const std::string& key) const {
    auto it = pImpl->entryMetadata.find(key);
    return (it != pImpl->entryMetadata.end() && it->second.isSecret);
}

bool ExpertConfig::removeSecret(const std::string& key) {
    auto it = pImpl->entryMetadata.find(key);
    if (it != pImpl->entryMetadata.end()) {
        it->second.isSecret = false;
        it->second.encrypted = false;
        logOperation("remove_secret", key, "success");
        return true;
    }
    return false;
}

bool ExpertConfig::setEnvironmentVariable(const std::string& key, const std::string& envVarName) {
    const char* envValue = std::getenv(envVarName.c_str());
    if (envValue) {
        set(key, std::string(envValue));
        
        auto& entry = pImpl->entryMetadata[key];
        entry.isFromEnvironment = true;
        entry.environmentVariable = envVarName;
        
        logOperation("set_environment_variable", key, "success");
        return true;
    }
    return false;
}

bool ExpertConfig::isFromEnvironment(const std::string& key) const {
    auto it = pImpl->entryMetadata.find(key);
    return (it != pImpl->entryMetadata.end() && it->second.isFromEnvironment);
}

std::string ExpertConfig::getEnvironmentVariable(const std::string& key) const {
    auto it = pImpl->entryMetadata.find(key);
    if (it != pImpl->entryMetadata.end() && it->second.isFromEnvironment) {
        return it->second.environmentVariable;
    }
    return "";
}

// Configuration hot-reloading and live updates
bool ExpertConfig::enableHotReload(bool enable) {
    bool oldValue = pImpl->hotReloadEnabled;
    pImpl->hotReloadEnabled = enable;
    
    logOperation("enable_hot_reload", "hot_reload", (enable ? "enabled" : "disabled"));
    return oldValue;
}

bool ExpertConfig::addHotReloadListener(const std::function<void(const std::string&, const ExpertConfigValue&)>& listener) {
    static int listenerId = 0;
    std::string listenerIdStr = std::to_string(listenerId++);
    
    pImpl->hotReloadListeners[listenerIdStr] = listener;
    return true;
}

bool ExpertConfig::removeHotReloadListener(const std::string& listenerId) {
    size_t count = pImpl->hotReloadListeners.erase(listenerId);
    return count > 0;
}

bool ExpertConfig::notifyHotReload(const std::string& key, const ExpertConfigValue& value) {
    for (auto& pair : pImpl->hotReloadListeners) {
        pair.second(key, value);
    }
    return !pImpl->hotReloadListeners.empty();
}

bool ExpertConfig::startHotReloadServer(int port) {
    // TODO: Implement hot reload server
    return false;
}

bool ExpertConfig::stopHotReloadServer() {
    // TODO: Implement hot reload server stop
    return false;
}

bool ExpertConfig::isHotReloadEnabled() const {
    return pImpl->hotReloadEnabled;
}

// Configuration distribution
bool ExpertConfig::distribute(const std::string& target, const std::string& protocol) {
    // TODO: Implement distribution
    return false;
}

bool ExpertConfig::receiveFrom(const std::string& source, const std::string& protocol) {
    // TODO: Implement receiving
    return false;
}

bool ExpertConfig::syncWith(const std::string& otherConfigPath) {
    ExpertConfig otherConfig;
    if (!otherConfig.loadFromFile(otherConfigPath)) {
        return false;
    }
    
    // Sync with other config
    // TODO: Implement synchronization logic
    pImpl->syncStatus = "synced";
    
    logOperation("sync_with", otherConfigPath, "success");
    return true;
}

bool ExpertConfig::createCluster(const std::vector<std::string>& nodes) {
    pImpl->clusterNodes = nodes;
    pImpl->isInCluster = true;
    pImpl->syncStatus = "cluster_created";
    
    logOperation("create_cluster", "cluster", "success");
    return true;
}

bool ExpertConfig::joinCluster(const std::string& nodeId) {
    pImpl->clusterNodes.push_back(nodeId);
    pImpl->isInCluster = true;
    pImpl->syncStatus = "joined_cluster";
    
    logOperation("join_cluster", nodeId, "success");
    return true;
}

bool ExpertConfig::leaveCluster() {
    pImpl->clusterNodes.clear();
    pImpl->isInCluster = false;
    pImpl->syncStatus = "left_cluster";
    
    logOperation("leave_cluster", "cluster", "success");
    return true;
}

bool ExpertConfig::isInCluster() const {
    return pImpl->isInCluster;
}

std::vector<std::string> ExpertConfig::getClusterNodes() const {
    return pImpl->clusterNodes;
}

// Advanced configuration types - Neural network hyperparameter optimization
bool ExpertConfig::addNeuralNetworkConfig(const std::string& name, const std::unordered_map<std::string, ExpertConfigValue>& params) {
    pImpl->neuralNetworkConfigs[name] = params;
    
    // Also add to main config
    for (const auto& pair : params) {
        set(pair.first, pair.second);
    }
    
    // Set metadata
    auto& metadata = pImpl->entryMetadata[name];
    metadata.type = "neural_network";
    metadata.category = "neural_network";
    
    logOperation("add_neural_network_config", name, "success");
    return true;
}

std::optional<std::unordered_map<std::string, ExpertConfigValue>> ExpertConfig::getNeuralNetworkConfig(const std::string& name) const {
    auto it = pImpl->neuralNetworkConfigs.find(name);
    if (it != pImpl->neuralNetworkConfigs.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<std::string> ExpertConfig::getNeuralNetworkConfigs() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->neuralNetworkConfigs) {
        names.push_back(pair.first);
    }
    return names;
}

bool ExpertConfig::optimizeNeuralNetwork(const std::string& name, const std::string& objective) {
    auto config = getNeuralNetworkConfig(name);
    if (!config) {
        return false;
    }
    
    // TODO: Implement neural network optimization
    // For now, just log the operation
    logOperation("optimize_neural_network", name, "success");
    return true;
}

// Advanced configuration types - Simulation parameter sets
bool ExpertConfig::addSimulationConfig(const std::string& name, const std::unordered_map<std::string, ExpertConfigValue>& params) {
    pImpl->simulationConfigs[name] = params;
    
    // Also add to main config
    for (const auto& pair : params) {
        set(pair.first, pair.second);
    }
    
    // Set metadata
    auto& metadata = pImpl->entryMetadata[name];
    metadata.type = "simulation";
    metadata.category = "simulation";
    
    logOperation("add_simulation_config", name, "success");
    return true;
}

std::optional<std::unordered_map<std::string, ExpertConfigValue>> ExpertConfig::getSimulationConfig(const std::string& name) const {
    auto it = pImpl->simulationConfigs.find(name);
    if (it != pImpl->simulationConfigs.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<std::string> ExpertConfig::getSimulationConfigs() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->simulationConfigs) {
        names.push_back(pair.first);
    }
    return names;
}

bool ExpertConfig::optimizeSimulation(const std::string& name, const std::string& objective) {
    auto config = getSimulationConfig(name);
    if (!config) {
        return false;
    }
    
    // TODO: Implement simulation optimization
    // For now, just log the operation
    logOperation("optimize_simulation", name, "success");
    return true;
}

// Advanced configuration types - Experiment configuration bundles
bool ExpertConfig::addExperimentConfig(const std::string& name, const std::unordered_map<std::string, ExpertConfigValue>& params) {
    pImpl->experimentConfigs[name] = params;
    
    // Also add to main config
    for (const auto& pair : params) {
        set(pair.first, pair.second);
    }
    
    // Set metadata
    auto& metadata = pImpl->entryMetadata[name];
    metadata.type = "experiment";
    metadata.category = "experiment";
    
    logOperation("add_experiment_config", name, "success");
    return true;
}

std::optional<std::unordered_map<std::string, ExpertConfigValue>> ExpertConfig::getExperimentConfig(const std::string& name) const {
    auto it = pImpl->experimentConfigs.find(name);
    if (it != pImpl->experimentConfigs.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<std::string> ExpertConfig::getExperimentConfigs() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->experimentConfigs) {
        names.push_back(pair.first);
    }
    return names;
}

bool ExpertConfig::optimizeExperiment(const std::string& name, const std::string& objective) {
    auto config = getExperimentConfig(name);
    if (!config) {
        return false;
    }
    
    // TODO: Implement experiment optimization
    // For now, just log the operation
    logOperation("optimize_experiment", name, "success");
    return true;
}

// Advanced configuration types - Performance tuning profiles
bool ExpertConfig::addPerformanceProfile(const std::string& name, const std::unordered_map<std::string, ExpertConfigValue>& params) {
    pImpl->performanceProfiles[name] = params;
    
    // Also add to main config
    for (const auto& pair : params) {
        set(pair.first, pair.second);
    }
    
    // Set metadata
    auto& metadata = pImpl->entryMetadata[name];
    metadata.type = "performance_profile";
    metadata.category = "performance";
    
    logOperation("add_performance_profile", name, "success");
    return true;
}

std::optional<std::unordered_map<std::string, ExpertConfigValue>> ExpertConfig::getPerformanceProfile(const std::string& name) const {
    auto it = pImpl->performanceProfiles.find(name);
    if (it != pImpl->performanceProfiles.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<std::string> ExpertConfig::getPerformanceProfiles() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->performanceProfiles) {
        names.push_back(pair.first);
    }
    return names;
}

bool ExpertConfig::optimizePerformanceProfile(const std::string& name, const std::string& objective) {
    auto config = getPerformanceProfile(name);
    if (!config) {
        return false;
    }
    
    // TODO: Implement performance profile optimization
    // For now, just log the operation
    logOperation("optimize_performance_profile", name, "success");
    return true;
}

// Advanced configuration types - Memory management profiles
bool ExpertConfig::addMemoryProfile(const std::string& name, const std::unordered_map<std::string, ExpertConfigValue>& params) {
    pImpl->memoryProfiles[name] = params;
    
    // Also add to main config
    for (const auto& pair : params) {
        set(pair.first, pair.second);
    }
    
    // Set metadata
    auto& metadata = pImpl->entryMetadata[name];
    metadata.type = "memory_profile";
    metadata.category = "memory";
    
    logOperation("add_memory_profile", name, "success");
    return true;
}

std::optional<std::unordered_map<std::string, ExpertConfigValue>> ExpertConfig::getMemoryProfile(const std::string& name) const {
    auto it = pImpl->memoryProfiles.find(name);
    if (it != pImpl->memoryProfiles.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<std::string> ExpertConfig::getMemoryProfiles() const {
    std::vector<std::string> names;
    for (const auto& pair : pImpl->memoryProfiles) {
        names.push_back(pair.first);
    }
    return names;
}

bool ExpertConfig::optimizeMemoryProfile(const std::string& name, const std::string& objective) {
    auto config = getMemoryProfile(name);
    if (!config) {
        return false;
    }
    
    // TODO: Implement memory profile optimization
    // For now, just log the operation
    logOperation("optimize_memory_profile", name, "success");
    return true;
}

// Configuration UI integration
std::string ExpertConfig::generateWebUI() const {
    std::stringstream html;
    html << "<!DOCTYPE html>" << std::endl;
    html << "<html>" << std::endl;
    html << "<head>" << std::endl;
    html << "  <title>NLM Expert Configuration</title>" << std::endl;
    html << "  <style>" << std::endl;
    html << "    body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
    html << "    .config-section { margin: 20px 0; padding: 20px; border: 1px solid #ddd; }" << std::endl;
    html << "    .key { font-weight: bold; color: #333; }" << std::endl;
    html << "    .value { color: #666; margin-left: 20px; }" << std::endl;
    html << "    .section-title { font-size: 1.2em; color: #2c3e50; }" << std::endl;
    html << "  </style>" << std::endl;
    html << "</head>" << std::endl;
    html << "<body>" << std::endl;
    html << "  <h1>NLM Expert Configuration</h1>" << std::endl;
    html << "  <div class=\"config-section\">" << std::endl;
    html << "    <h2 class=\"section-title\">Configuration Summary</h2>" << std::endl;
    html << "    <p>Total Keys: " << pImpl->configValues.size() << "</p>" << std::endl;
    html << "    <p>Valid Keys: " << count_if(pImpl->configValues.begin(), pImpl->configValues.end(), 
        [this](const auto& pair) { return validate(pair.first); }) << "</p>" << std::endl;
    html << "  </div>" << std::endl;
    html << "  <div class=\"config-section\">" << std::endl;
    html << "    <h2 class=\"section-title\">Configuration Values</h2>" << std::endl;
    
    for (const auto& pair : pImpl->configValues) {
        html << "    <div><span class=\"key\">" << pair.first << ":</span> <span class=\"value\">" 
             << valueToString(pair.second) << "</span></div>" << std::endl;
    }
    
    html << "  </div>" << std::endl;
    html << "</body>" << std::endl;
    html << "</html>" << std::endl;
    
    return html.str();
}

bool ExpertConfig::startWebUI(int port) {
    // TODO: Implement Web UI
    return false;
}

bool ExpertConfig::stopWebUI() {
    // TODO: Implement Web UI stop
    return false;
}

bool ExpertConfig::isWebUIEnabled() const {
    return pImpl->webUIEnabled;
}

// Expert configuration commands (CLI integration)
bool ExpertConfig::validateCommand(const std::string& format) const {
    std::string report = validateAndReport();
    
    if (format == "json") {
        // Output JSON report
        // TODO: Implement JSON report output
    } else {
        // Output text report
        std::cout << report << std::endl;
    }
    
    logOperation("validate_command", "validation", "success");
    pImpl->totalValidates++;
    return true;
}

bool ExpertConfig::validateStrictCommand(const std::string& format) const {
    std::string report = validateStrictAndReport();
    
    if (format == "json") {
        // Output JSON report
        // TODO: Implement JSON report output
    } else {
        // Output text report
        std::cout << report << std::endl;
    }
    
    logOperation("validate_strict_command", "validation", "success");
    return true;
}

bool ExpertConfig::exportCommand(const std::string& filepath, const std::string& format) const {
    return exportConfig(filepath, format, false);
}

bool ExpertConfig::importCommand(const std::string& filepath, const std::string& format) {
    return importConfig(filepath, format, true, true);
}

bool ExpertConfig::diffCommand(const std::string& otherConfigPath) const {
    ConfigDiffResult diff = diffWith(otherConfigPath);
    std::cout << generateDiffReport(diff) << std::endl;
    
    logOperation("diff_command", otherConfigPath, "success");
    return true;
}

bool ExpertConfig::compareCommand(const std::string& otherConfigPath) const {
    double similarity = calculateSimilarity(otherConfigPath);
    std::cout << "Similarity Score: " << (similarity * 100) << "%" << std::endl;
    
    logOperation("compare_command", otherConfigPath, "success");
    return true;
}

bool ExpertConfig::profileCommand(const std::string& outputFormat) const {
    std::string report = profile();
    
    if (outputFormat == "json") {
        // Output JSON report
        // TODO: Implement JSON report output
    } else {
        // Output text report
        std::cout << report << std::endl;
    }
    
    logOperation("profile_command", "profiling", "success");
    return true;
}

bool ExpertConfig::optimizeCommand(const std::string& optimizationType) {
    if (optimizationType == "all") {
        optimize();
        optimizeMemory();
        optimizePerformance();
        optimizeValidation();
        optimizeDependencies();
    } else if (optimizationType == "memory") {
        optimizeMemory();
    } else if (optimizationType == "performance") {
        optimizePerformance();
    } else if (optimizationType == "validation") {
        optimizeValidation();
    } else if (optimizationType == "dependencies") {
        optimizeDependencies();
    }
    
    logOperation("optimize_command", optimizationType, "success");
    pImpl->totalOptimizations++;
    return true;
}

bool ExpertConfig::analyzeCommand(const std::string& analysisType) {
    if (analysisType == "all") {
        analyze();
        analyzeUsage();
        analyzeDependencies();
        analyzePerformance();
        analyzeMemory();
        analyzeValidation();
    } else if (analysisType == "usage") {
        analyzeUsage();
    } else if (analysisType == "dependencies") {
        analyzeDependencies();
    } else if (analysisType == "performance") {
        analyzePerformance();
    } else if (analysisType == "memory") {
        analyzeMemory();
    } else if (analysisType == "validation") {
        analyzeValidation();
    }
    
    logOperation("analyze_command", analysisType, "success");
    pImpl->totalAnalyses++;
    return true;
}

// Integration with existing systems
bool ExpertConfig::integrateWithCLI() {
    // TODO: Implement CLI integration
    return false;
}

bool ExpertConfig::integrateWithValidation() {
    // TODO: Implement validation integration
    return false;
}

bool ExpertConfig::integrateWithLogging() {
    // TODO: Implement logging integration
    return false;
}

bool ExpertConfig::integrateWithSerialization() {
    // TODO: Implement serialization integration
    return false;
}

bool ExpertConfig::integrateWithAdvancedCLI() {
    // TODO: Implement Advanced CLI integration
    return false;
}

// Backward compatibility with existing Config API
bool ExpertConfig::loadFromFile(const std::string& filepath) {
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return parseFromFile(filepath, "json");
    } else if (ext == ".yaml" || ext == ".yml") {
        return parseFromFile(filepath, "yaml");
    } else {
        return parseFromFile(filepath, "keyvalue");
    }
}

bool ExpertConfig::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool ExpertConfig::saveToFile(const std::string& filepath, const std::string& format) const {
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (format == "json" || ext == ".json") {
        nlohmann::json j = toJSON();
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ConfigError("Failed to open file for writing: " + filepath, ConfigError::ErrorType::FileIOError);
        }
        file << std::setw(2) << j;
        return true;
    } else if (format == "yaml" || ext == ".yaml" || ext == ".yml") {
        YAML::Emitter emitter;
        toYAML(emitter);
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ConfigError("Failed to open file for writing: " + filepath, ConfigError::ErrorType::FileIOError);
        }
        file << emitter.c_str();
        return true;
    } else {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        for (const auto& pair : pImpl->configValues) {
            file << pair.first << " = " << valueToString(pair.second) << std::endl;
        }
        
        return true;
    }
}

template<typename T>
std::optional<T> ExpertConfig::get(const std::string& key) const {
    try {
        ExpertConfigValue value = get(key);
        return std::get<T>(value);
    } catch (const ConfigError& e) {
        return std::nullopt;
    }
}

template<typename T>
T ExpertConfig::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value) {
        return *value;
    }
    return defaultValue;
}

void ExpertConfig::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Convert ConfigValue to ExpertConfigValue
    ExpertConfigValue expertValue;
    
    std::visit([&expertValue](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::map<std::string, nlohmann::json>>) {
            // Convert nested config
            expertValue = ExpertConfigValue();
            // TODO: Implement conversion
        } else {
            expertValue = ExpertConfigValue(arg);
        }
    }, value);
    
    set(key, expertValue);
}

void ExpertConfig::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ExpertConfigValue(value));
}

void ExpertConfig::set(const std::string& key, int value, ConfigSource source) {
    set(key, ExpertConfigValue(value));
}

void ExpertConfig::set(const std::string& key, double value, ConfigSource source) {
    set(key, ExpertConfigValue(value));
}

void ExpertConfig::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ExpertConfigValue(value));
}

void ExpertConfig::setFromEnvironment(const std::string& key, const std::string& envVarName) {
    const char* envValue = std::getenv(envVarName.c_str());
    if (envValue) {
        set(key, std::string(envValue), ConfigSource::EnvironmentVariable);
        
        auto& metadata = pImpl->entryMetadata[key];
        metadata.isFromEnvironment = true;
        metadata.environmentVariable = envVarName;
    }
}

bool ExpertConfig::isFromEnvironment(const std::string& key) const {
    auto it = pImpl->entryMetadata.find(key);
    return (it != pImpl->entryMetadata.end() && it->second.isFromEnvironment);
}

bool ExpertConfig::has(const std::string& key) const {
    return pImpl->configValues.find(key) != pImpl->configValues.end();
}

void ExpertConfig::remove(const std::string& key) {
    pImpl->configValues.erase(key);
    pImpl->entryMetadata.erase(key);
    
    logOperation("remove", key, "success");
}

std::vector<std::string> ExpertConfig::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->configValues.size());
    for (const auto& pair : pImpl->configValues) {
        keys.push_back(pair.first);
    }
    return keys;
}

void ExpertConfig::clear() {
    pImpl->configValues.clear();
    pImpl->entryMetadata.clear();
    
    logOperation("clear", "config", "success");
}

std::string ExpertConfig::summary() const {
    std::ostringstream oss;
    oss << "Expert Configuration (" << pImpl->configValues.size() << " entries):\n";
    for (const auto& pair : pImpl->configValues) {
        oss << "  " << pair.first << " = [" << valueToString(pair.second) << "]\n";
    }
    return oss.str();
}

// Expert configuration statistics
std::string ExpertConfig::getStatistics() const {
    std::stringstream ss;
    ss << "Expert Configuration Statistics:" << std::endl;
    ss << "===========================" << std::endl;
    ss << "Total Configurations: " << pImpl->totalConfigures << std::endl;
    ss << "Total Exports: " << pImpl->totalExports << std::endl;
    ss << "Total Imports: " << pImpl->totalImports << std::endl;
    ss << "Total Validations: " << pImpl->totalValidates << std::endl;
    ss << "Total Optimizations: " << pImpl->totalOptimizations << std::endl;
    ss << "Total Analyses: " << pImpl->totalAnalyses << std::endl;
    ss << "Total Prescriptions: " << pImpl->totalPrescriptions << std::endl;
    ss << std::endl;
    ss << "Storage:" << std::endl;
    ss << "  Templates: " << pImpl->templates.size() << std::endl;
    ss << "  Presets: " << pImpl->presets.size() << std::endl;
    ss << "  History Entries: " << pImpl->history.size() << std::endl;
    ss << "  Audit Logs: " << pImpl->auditLogs.size() << std::endl;
    ss << std::endl;
    ss << "Features:" << std::endl;
    ss << "  Expert Features: " << (pImpl->expertFeaturesEnabled ? "Enabled" : "Disabled") << std::endl;
    ss << "  Scripting: " << (pImpl->scriptingSupported ? "Enabled" : "Disabled") << std::endl;
    ss << "  Macro Support: " << (pImpl->macroSupportEnabled ? "Enabled" : "Disabled") << std::endl;
    ss << "  Secrets Management: " << (pImpl->secretsManagementEnabled ? "Enabled" : "Disabled") << std::endl;
    ss << "  Hot Reload: " << (pImpl->hotReloadEnabled ? "Enabled" : "Disabled") << std::endl;
    ss << "  Web UI: " << (pImpl->webUIEnabled ? "Enabled" : "Disabled") << std::endl;
    ss << "  Distribution: " << (pImpl->distributionEnabled ? "Enabled" : "Disabled") << std::endl;
    ss << "  Advanced Types: " << (pImpl->advancedTypesEnabled ? "Enabled" : "Disabled") << std::endl;
    ss << "  UI Features: " << (pImpl->uiFeaturesEnabled ? "Enabled" : "Disabled") << std::endl;
    
    return ss.str();
}

std::string ExpertConfig::getHealthStatus() const {
    std::stringstream ss;
    ss << "Expert Configuration Health Status:" << std::endl;
    ss << "============================" << std::endl;
    ss << "Status: " << (isHealthy() ? "HEALTHY" : "UNHEALTHY") << std::endl;
    ss << "Hot Reload: " << (pImpl->hotReloadEnabled ? "Enabled" : "Disabled") << std::endl;
    ss << "Web UI: " << (pImpl->webUIEnabled ? "Enabled" : "Disabled") << std::endl;
    ss << "Performance Monitoring: " << (pImpl->performanceMonitoringEnabled ? "Enabled" : "Disabled") << std::endl;
    ss << "Memory Tracking: " << (pImpl->memoryTrackingEnabled ? "Enabled" : "Disabled") << std::endl;
    ss << "Audit Logging: " << (pImpl->auditLoggingEnabled ? "Enabled" : "Disabled") << std::endl;
    ss << "History: " << (pImpl->historyEnabled ? "Enabled" : "Disabled") << std::endl;
    ss << "Cluster: " << (pImpl->isInCluster ? "Active" : "Inactive") << std::endl;
    ss << "Sync Status: " << pImpl->syncStatus << std::endl;
    
    return ss.str();
}

bool ExpertConfig::isHealthy() const {
    // Check if all critical components are working
    bool healthy = true;
    
    if (!pImpl->hotReloadEnabled && !pImpl->webUIEnabled) {
        healthy = false; // Both features disabled
    }
    
    // Check if there are any critical errors
    // TODO: Add more health checks
    
    return healthy;
}

bool ExpertConfig::enablePerformanceMonitoring(bool enable) {
    bool oldValue = pImpl->performanceMonitoringEnabled;
    pImpl->performanceMonitoringEnabled = enable;
    return oldValue;
}

bool ExpertConfig::enableMemoryTracking(bool enable) {
    bool oldValue = pImpl->memoryTrackingEnabled;
    pImpl->memoryTrackingEnabled = enable;
    return oldValue;
}

bool ExpertConfig::enableAuditLogging(bool enable) {
    bool oldValue = pImpl->auditLoggingEnabled;
    pImpl->auditLoggingEnabled = enable;
    return oldValue;
}

bool ExpertConfig::enableHotReload(bool enable) {
    return enableHotReload(enable);
}

// Configuration backup and restore
bool ExpertConfig::createBackup(const std::string& name) {
    std::string backupName = name.empty() ? "backup_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) : name;
    
    // TODO: Implement backup creation
    // For now, just log the operation
    logOperation("create_backup", backupName, "success");
    return true;
}

bool ExpertConfig::restoreBackup(const std::string& name) {
    // TODO: Implement backup restoration
    return false;
}

std::vector<std::string> ExpertConfig::getBackups() const {
    // TODO: Implement backup listing
    return std::vector<std::string>();
}

bool ExpertConfig::deleteBackup(const std::string& name) {
    // TODO: Implement backup deletion
    return false;
}

// Configuration synchronization
bool ExpertConfig::syncTo(const std::string& target, const std::string& protocol) {
    return distribute(target, protocol);
}

bool ExpertConfig::syncFrom(const std::string& source, const std::string& protocol) {
    return receiveFrom(source, protocol);
}

bool ExpertConfig::isSynced() const {
    return syncStatus == "synced";
}

std::string ExpertConfig::getSyncStatus() const {
    return syncStatus;
}

// Private helper functions
void ExpertConfig::logOperation(const std::string& operation, const std::string& target, const std::string& result, const std::string& details) {
    std::string message = "ExpertConfig: " + operation + "(" + target + ") = " + result;
    if (!details.empty()) {
        message += " - " + details;
    }
    
    // Use global logger
    nlm::Logger::getGlobal().info(message);
    
    pImpl->totalConfigures++;
}

std::string ExpertConfig::generateChecksum(const std::unordered_map<std::string, ExpertConfigValue>& config) const {
    // TODO: Implement checksum generation
    return "checksum_placeholder";
}

bool ExpertConfig::validateConfigStructure() const {
    // Validate configuration structure
    for (const auto& pair : pImpl->configValues) {
        if (pair.first.empty()) {
            return false;
        }
        
        // Check if value type is valid
        if (!isValidExpertConfigValue(pair.second)) {
            return false;
        }
    }
    
    return true;
}

std::unordered_map<std::string, ExpertConfigValue> ExpertConfig::convertToExpertConfig(const std::unordered_map<std::string, ConfigValue>& config) const {
    std::unordered_map<std::string, ExpertConfigValue> expertConfig;
    
    for (const auto& pair : config) {
        expertConfig[pair.first] = ExpertConfigValue(pair.second);
    }
    
    return expertConfig;
}

std::unordered_map<std::string, ConfigValue> ExpertConfig::convertFromExpertConfig(const std::unordered_map<std::string, ExpertConfigValue>& config) const {
    std::unordered_map<std::string, ConfigValue> convertedConfig;
    
    for (const auto& pair : config) {
        convertedConfig[pair.first] = ConfigValue(pair.second);
    }
    
    return convertedConfig;
}

// Additional helper functions for ExpertConfig
std::string ExpertConfig::valueToString(const ExpertConfigValue& value) const {
    std::stringstream ss;
    
    std::visit([&ss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            ss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            ss << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            ss << std::boolalpha << arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>> || std::is_same_v<T, std::vector<double>> || std::is_same_v<T, std::vector<std::string>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << arg[i];
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::map<std::string, nlohmann::json>>) {
            ss << "{";
            for (auto it = arg.begin(); it != arg.end(); ++it) {
                if (it != arg.begin()) ss << ", ";
                ss << "\"" << it->first << "\": " << it->second.dump();
            }
            ss << "}";
        } else {
            ss << "[unknown type]";
        }
    }, value);
    
    return ss.str();
}

std::string ExpertConfig::getValueType(const ExpertConfigValue& value) const {
    std::string type;
    
    std::visit([&type](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            type = "string";
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            type = "integer";
        } else if constexpr (std::is_same_v<T, double>) {
            type = "float";
        } else if constexpr (std::is_same_v<T, bool>) {
            type = "boolean";
        } else if constexpr (std::is_same_v<T, std::vector<int>> || std::is_same_v<T, std::vector<double>> || std::is_same_v<T, std::vector<std::string>>) {
            type = "array";
        } else if constexpr (std::is_same_v<T, std::map<std::string, nlohmann::json>>) {
            type = "object";
        } else {
            type = "unknown";
        }
    }, value);
    
    return type;
}

ExpertConfigValue ExpertConfig::stringToExpertConfigValue(const std::string& str) {
    // Try to parse as boolean
    if (str == "true") {
        return ExpertConfigValue(true);
    } else if (str == "false") {
        return ExpertConfigValue(false);
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int intValue = std::stoi(str, &pos);
        if (pos == str.size()) {
            return ExpertConfigValue(intValue);
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleValue = std::stod(str, &pos);
        if (pos == str.size()) {
            return ExpertConfigValue(doubleValue);
        }
    } catch (...) {}
    
    // Return as string (removing quotes)
    std::string value = str;
    if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
        (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    return ExpertConfigValue(value);
}

bool ExpertConfig::isValidExpertConfigValue(const ExpertConfigValue& value) const {
    return std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        return std::is_same_v<T, std::string> ||
               std::is_same_v<T, int> || std::is_same_v<T, int64_t> ||
               std::is_same_v<T, double> ||
               std::is_same_v<T, bool> ||
               std::is_same_v<T, std::vector<int>> ||
               std::is_same_v<T, std::vector<double>> ||
               std::is_same_v<T, std::vector<std::string>> ||
               std::is_same_v<T, std::map<std::string, nlohmann::json>>;
    }, value);
}

nlohmann::json ExpertConfig::expertConfigValueToJSON(const ExpertConfigValue& value) const {
    nlohmann::json j;
    
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::map<std::string, nlohmann::json>>) {
            j = arg;
        } else {
            j = "[unknown type]";
        }
    }, value);
    
    return j;
}

ExpertConfigValue ExpertConfig::jsonToExpertConfigValue(const nlohmann::json& j) const {
    if (j.is_string()) {
        return ExpertConfigValue(j.get<std::string>());
    } else if (j.is_number_integer()) {
        return ExpertConfigValue(j.get<int>());
    } else if (j.is_number_unsigned()) {
        return ExpertConfigValue(j.get<int64_t>());
    } else if (j.is_number_float()) {
        return ExpertConfigValue(j.get<double>());
    } else if (j.is_boolean()) {
        return ExpertConfigValue(j.get<bool>());
    } else if (j.is_array()) {
        if (j.empty()) {
            return ExpertConfigValue(std::vector<int>{});
        }
        
        bool allInts = true;
        bool allDoubles = true;
        bool allStrings = true;
        
        for (const auto& elem : j) {
            if (!elem.is_number_integer()) allInts = false;
            if (!elem.is_number()) allDoubles = false;
            if (!elem.is_string()) allStrings = false;
        }
        
        if (allInts) {
            return ExpertConfigValue(j.get<std::vector<int>>());
        } else if (allDoubles) {
            return ExpertConfigValue(j.get<std::vector<double>>());
        } else if (allStrings) {
            return ExpertConfigValue(j.get<std::vector<std::string>>());
        } else {
            return ExpertConfigValue(j.get<std::map<std::string, nlohmann::json>>());
        }
    } else if (j.is_object()) {
        return ExpertConfigValue(j.get<std::map<std::string, nlohmann::json>>());
    }
    
    throw ConfigError("Unsupported JSON type for expert config value", ConfigError::ErrorType::TypeMismatch);
}

std::string ExpertConfig::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Struct implementation
struct ExpertConfig::Impl {
    // Configuration data
    std::unordered_map<std::string, ExpertConfigValue> configValues;
    std::unordered_map<std::string, ConfigEntry> entryMetadata;
    
    // Expert features
    bool expertFeaturesEnabled;
    bool scriptingSupported;
    bool macroSupportEnabled;
    bool secretsManagementEnabled;
    bool hotReloadEnabled;
    bool webUIEnabled;
    bool distributionEnabled;
    bool advancedTypesEnabled;
    bool uiFeaturesEnabled;
    
    // History and audit
    bool historyEnabled;
    bool auditLoggingEnabled;
    bool performanceMonitoringEnabled;
    bool memoryTrackingEnabled;
    bool isInCluster;
    std::string syncStatus;
    
    // Advanced configuration types
    std::unordered_map<std::string, std::unordered_map<std::string, ExpertConfigValue>> neuralNetworkConfigs;
    std::unordered_map<std::string, std::unordered_map<std::string, ExpertConfigValue>> simulationConfigs;
    std::unordered_map<std::string, std::unordered_map<std::string, ExpertConfigValue>> experimentConfigs;
    std::unordered_map<std::string, std::unordered_map<std::string, ExpertConfigValue>> performanceProfiles;
    std::unordered_map<std::string, std::unordered_map<std::string, ExpertConfigValue>> memoryProfiles;
    
    // Templates and presets
    std::unordered_map<std::string, ConfigTemplate> templates;
    std::unordered_map<std::string, ConfigPreset> presets;
    
    // History and audit logs
    std::vector<ConfigHistoryEntry> history;
    std::vector<ConfigAuditEntry> auditLogs;
    
    // Hot reload
    bool hotReloadEnabled;
    std::unordered_map<std::string, std::function<void(const std::string&, const ExpertConfigValue&)>> hotReloadListeners;
    
    // Custom functions
    std::unordered_map<std::string, std::function<ExpertConfigValue(const std::vector<ExpertConfigValue>&)>> customFunctions;
    std::unordered_map<std::string, std::function<ExpertConfigValue(const std::vector<ExpertConfigValue>&)>> templateFunctions;
    std::unordered_map<std::string, std::string> macroDefinitions;
    
    // Cluster nodes
    std::vector<std::string> clusterNodes;
    
    // Statistics
    uint64_t totalConfigures;
    uint64_t totalExports;
    uint64_t totalImports;
    uint64_t totalValidates;
    uint64_t totalOptimizations;
    uint64_t totalAnalyses;
    uint64_t totalPrescriptions;
    
    Impl() : expertFeaturesEnabled(true),
             scriptingSupported(true),
             macroSupportEnabled(true),
             secretsManagementEnabled(true),
             hotReloadEnabled(false),
             webUIEnabled(false),
             distributionEnabled(true),
             advancedTypesEnabled(true),
             uiFeaturesEnabled(true),
             historyEnabled(false),
             auditLoggingEnabled(false),
             performanceMonitoringEnabled(false),
             memoryTrackingEnabled(false),
             isInCluster(false),
             syncStatus("not_synced"),
             hotReloadEnabled(false),
             totalConfigures(0),
             totalExports(0),
             totalImports(0),
             totalValidates(0),
             totalOptimizations(0),
             totalAnalyses(0),
             totalPrescriptions(0) {}
};

// Expert configuration utility functions
namespace expert_config {
    bool executeJavaScript(const std::string& script, ExpertConfig& config) {
        // TODO: Implement JavaScript execution
        return false;
    }
    
    bool executeLua(const std::string& script, ExpertConfig& config) {
        // TODO: Implement Lua execution
        return false;
    }
    
    bool executePython(const std::string& script, ExpertConfig& config) {
        // TODO: Implement Python execution
        return false;
    }
    
    ConfigDiffResult compareConfigs(const ExpertConfig& config1, const ExpertConfig& config2) {
        return config1.diffWith(config2);
    }
    
    double calculateSimilarity(const ExpertConfig& config1, const ExpertConfig& config2) {
        return config1.calculateSimilarity(config2);
    }
    
    std::unordered_map<std::string, double> analyzeConfig(const ExpertConfig& config) {
        return config.calculateMetrics();
    }
    
    std::string generateConfigReport(const ExpertConfig& config) {
        return config.analyze();
    }
    
    bool optimizeConfig(ExpertConfig& config, const std::string& objective) {
        return config.optimize();
    }
    
    std::unordered_map<std::string, ExpertConfigValue> getOptimalConfig(const ExpertConfig& config, const std::string& objective) {
        // TODO: Implement optimal config retrieval
        return std::unordered_map<std::string, ExpertConfigValue>();
    }
    
    bool createTemplateFromConfig(const std::string& templateName, const ExpertConfig& config) {
        // TODO: Implement template creation
        return false;
    }
    
    ExpertConfig createConfigFromTemplate(const std::string& templateName) {
        // TODO: Implement config creation from template
        return ExpertConfig();
    }
    
    bool createVersionFromConfig(ExpertConfig& config, const std::string& version, const std::string& author, const std::string& description) {
        return config.createVersion(version, author, description);
    }
    
    ExpertConfig restoreVersion(const ExpertConfig& config, const std::string& version) {
        return config; // TODO: Implement version restoration
    }
    
    bool createAuditReport(const std::vector<ConfigAuditEntry>& auditLogs, const std::string& outputFile, const std::string& format) {
        // TODO: Implement audit report creation
        return false;
    }
    
    bool createAuditSummary(const std::vector<ConfigAuditEntry>& auditLogs, const std::string& outputFile, const std::string& format) {
        // TODO: Implement audit summary creation
        return false;
    }
    
    bool distributeConfig(const ExpertConfig& config, const std::string& target, const std::string& protocol) {
        return config.distribute(target, protocol);
    }
    
    ExpertConfig receiveConfig(const std::string& source, const std::string& protocol) {
        // TODO: Implement config reception
        return ExpertConfig();
    }
    
    bool validateConfig(const ExpertConfig& config, const std::string& validationRule) {
        return config.validateAll();
    }
    
    bool validateConfigStrict(const ExpertConfig& config, const std::string& validationRule) {
        return config.validateAllStrict();
    }
    
    std::string validateConfigAndReport(const ExpertConfig& config, const std::string& validationRule) {
        return config.validateAndReport();
    }
    
    bool exportConfig(const ExpertConfig& config, const std::string& filepath, const std::string& format, bool includeSecrets) {
        return config.exportConfig(filepath, format, includeSecrets);
    }
    
    ExpertConfig importConfig(const std::string& filepath, const std::string& format, bool merge, bool validateOnImport) {
        ExpertConfig config;
        config.importConfig(filepath, format, merge, validateOnImport);
        return config;
    }
}

// Convenience functions
ExpertConfig createExpertConfig(const Config& config) {
    ExpertConfig expertConfig;
    
    // Convert from Config to ExpertConfig
    // TODO: Implement conversion
    
    return expertConfig;
}

ExpertConfig loadExpertConfig(const std::string& filepath, const std::string& format) {
    ExpertConfig config;
    config.loadFromFile(filepath);
    return config;
}

ExpertConfig loadExpertConfigFromJSON(const std::string& jsonString) {
    ExpertConfig config;
    // TODO: Implement JSON parsing
    return config;
}

ExpertConfig loadExpertConfigFromYAML(const std::string& yamlString) {
    ExpertConfig config;
    // TODO: Implement YAML parsing
    return config;
}

ExpertConfig loadExpertConfigFromKeyValue(const std::string& keyValueString) {
    ExpertConfig config;
    // TODO: Implement key=value parsing
    return config;
}

Config exportToConfig(const ExpertConfig& expertConfig) {
    Config config;
    
    // Convert from ExpertConfig to Config
    // TODO: Implement conversion
    
    return config;
}

bool mergeIntoConfig(Config& config, const ExpertConfig& expertConfig, bool overwrite) {
    // TODO: Implement merge operation
    return false;
}

} // namespace nlm

// Explicit template instantiations
template std::optional<int> ExpertConfig::get<int>(const std::string&) const;
template std::optional<int64_t> ExpertConfig::get<int64_t>(const std::string&) const;
template std::optional<double> ExpertConfig::get<double>(const std::string&) const;
template std::optional<bool> ExpertConfig::get<bool>(const std::string&) const;
template std::optional<std::string> ExpertConfig::get<std::string>(const std::string&) const;

// TODO: Add more template instantiations as needed

} // namespace nlm

// Additional helper functions for ExpertConfig
namespace nlm {
    // Function to get minimum value from ExpertConfigValue
    double getMinValue(const ExpertConfigValue& value) {
        std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                return static_cast<double>(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                                 std::is_same_v<T, std::vector<double>>) {
                if (!arg.empty()) {
                    return *std::min_element(arg.begin(), arg.end(), [](const auto& a, const auto& b) {
                        return static_cast<double>(a) < static_cast<double>(b);
                    });
                }
            }
            return 0.0;
        }, value);
        return 0.0;
    }
    
    // Function to get maximum value from ExpertConfigValue
    double getMaxValue(const ExpertConfigValue& value) {
        std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                return static_cast<double>(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                                 std::is_same_v<T, std::vector<double>>) {
                if (!arg.empty()) {
                    return *std::max_element(arg.begin(), arg.end(), [](const auto& a, const auto& b) {
                        return static_cast<double>(a) < static_cast<double>(b);
                    });
                }
            }
            return 0.0;
        }, value);
        return 0.0;
    }
    
    // Function to get average value from ExpertConfigValue
    double getAverageValue(const ExpertConfigValue& value) {
        std::visit([](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                return static_cast<double>(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                return arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>> || 
                                 std::is_same_v<T, std::vector<double>>) {
                if (!arg.empty()) {
                    double sum = 0.0;
                    for (const auto& elem : arg) {
                        sum += static_cast<double>(elem);
                    }
                    return sum / arg.size();
                }
            }
            return 0.0;
        }, value);
        return 0.0;
    }
}