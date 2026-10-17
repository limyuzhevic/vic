#pragma once

#include "ConfigAdvanced.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstring>
#include <cstdint>

namespace nlm {

struct ConfigAdvanced::Impl {
    // Core configuration data
    std::unordered_map<std::string, ConfigValue> configData;
    
    // Schema and validation
    std::unique_ptr<ConfigSchema> schema;
    
    // Templates
    std::unordered_map<std::string, ConfigAdvanced> templates;
    
    // Profiles
    std::unordered_map<std::string, std::pair<std::string, std::string>> profiles;
    
    // Version control
    std::vector<VersionRecord> versionHistory;
    
    // Modification tracking
    std::vector<ModificationRecord> modificationHistory;
    
    // Caching
    mutable std::unordered_map<std::string, ConfigValue> cache;
    mutable bool cacheDirty = true;
    
    // Statistics
    size_t modificationCount = 0;
    size_t versionCount = 0;
    
    // Thread safety
    mutable std::mutex mutex;
};

ConfigAdvanced::ConfigAdvanced() : pImpl(std::make_unique<Impl>()) {
    pImpl->schema = std::make_unique<ConfigSchema>();
}

ConfigAdvanced::~ConfigAdvanced() = default;

ConfigAdvanced::ConfigAdvanced(ConfigAdvanced&& other) noexcept : 
    pImpl(std::move(other.pImpl)),
    validationErrors(std::move(other.validationErrors)),
    currentSchema(std::move(other.currentSchema)),
    environment(std::move(other.environment)),
    currentPlatform(std::move(other.currentPlatform)),
    currentVersion(std::move(other.currentVersion)),
    modificationHistory(std::move(other.modificationHistory)),
    versionHistory(std::move(other.versionHistory)),
    templates(std::move(other.templates)),
    profiles(std::move(other.profiles)) {
    other.trackModifications = false;
    other.cacheEnabled = false;
    other.canRollback = false;
    other.backwardCompatEnabled = false;
}

ConfigAdvanced& ConfigAdvanced::operator=(ConfigAdvanced&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        validationErrors = std::move(other.validationErrors);
        currentSchema = std::move(other.currentSchema);
        environment = std::move(other.environment);
        currentPlatform = std::move(other.currentPlatform);
        currentVersion = std::move(other.currentVersion);
        modificationHistory = std::move(other.modificationHistory);
        versionHistory = std::move(other.versionHistory);
        templates = std::move(other.templates);
        profiles = std::move(other.profiles);
        
        other.trackModifications = false;
        other.cacheEnabled = false;
        other.canRollback = false;
        other.backwardCompatEnabled = false;
    }
    return *this;
}

bool ConfigAdvanced::setSchema(const ConfigSchema& schema) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->schema = std::make_unique<ConfigSchema>(schema);
    currentSchema = pImpl->schema;
    clearCache();
    validationErrors.clear();
    return validate();
}

bool ConfigAdvanced::validate() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    validationErrors.clear();
    
    if (pImpl->schema) {
        return pImpl->schema->validate(*this, validationErrors);
    }
    
    return true;
}

bool ConfigAdvanced::inheritFrom(const ConfigAdvanced& parent, bool overrideDefaults) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Create a copy of parent's config to inherit from
    ConfigAdvanced temp(parent);
    
    if (!overrideDefaults) {
        // Merge, preserving existing values
        for (const auto& entry : temp.pImpl->configData) {
            if (!has(entry.first)) {
                set(entry.first, entry.second, ConfigSource::Default);
            }
        }
    } else {
        // Merge with override
        for (const auto& entry : temp.pImpl->configData) {
            set(entry.first, entry.second, ConfigSource::Default);
        }
    }
    
    updateCache();
    trackModifications("inheritance", ConfigValue("inherited from parent"), ConfigValue("inherited"), "system");
    return true;
}

bool ConfigAdvanced::merge(const ConfigAdvanced& other, bool overrideExisting) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (const auto& entry : other.pImpl->configData) {
        if (overrideExisting || !has(entry.first)) {
            set(entry.first, entry.second, ConfigSource::Runtime);
        }
    }
    
    updateCache();
    return true;
}

bool ConfigAdvanced::setTemplate(const std::string& name, const ConfigAdvanced& templateConfig) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    templates[name] = templateConfig;
    return true;
}

bool ConfigAdvanced::applyTemplate(const std::string& templateName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = templates.find(templateName);
    if (it == templates.end()) {
        return false;
    }
    
    clear();
    for (const auto& entry : it->second.pImpl->configData) {
        set(entry.first, entry.second, ConfigSource::Default);
    }
    
    updateCache();
    return true;
}

bool ConfigAdvanced::removeTemplate(const std::string& templateName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = templates.find(templateName);
    if (it == templates.end()) {
        return false;
    }
    
    templates.erase(it);
    return true;
}

std::vector<std::string> ConfigAdvanced::getTemplateNames() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    names.reserve(templates.size());
    for (const auto& pair : templates) {
        names.push_back(pair.first);
    }
    return names;
}

bool ConfigAdvanced::createProfile(const std::string& profileName, const std::string& description) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (profiles.find(profileName) != profiles.end()) {
        return false;
    }
    
    profiles[profileName] = std::make_pair(description, currentVersion);
    return true;
}

bool ConfigAdvanced::applyProfile(const std::string& profileName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = profiles.find(profileName);
    if (it == profiles.end()) {
        return false;
    }
    
    // Store current config as snapshot if rollback is enabled
    if (canRollback) {
        addVersionRecord("profile_restore", "Restoring profile " + profileName);
    }
    
    // Restore profile version
    currentVersion = it->second.second;
    return true;
}

bool ConfigAdvanced::removeProfile(const std::string& profileName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = profiles.find(profileName);
    if (it == profiles.end()) {
        return false;
    }
    
    profiles.erase(it);
    return true;
}

bool ConfigAdvanced::renameProfile(const std::string& oldName, const std::string& newName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = profiles.find(oldName);
    if (it == profiles.end()) {
        return false;
    }
    
    if (profiles.find(newName) != profiles.end()) {
        return false;
    }
    
    profiles[newName] = it->second;
    profiles.erase(it);
    return true;
}

std::vector<std::string> ConfigAdvanced::getProfileNames() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    names.reserve(profiles.size());
    for (const auto& pair : profiles) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<ConfigEntry> ConfigAdvanced::getModificationHistory(size_t maxEntries) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<ConfigEntry> history;
    history.reserve(std::min(maxEntries, modificationHistory.size()));
    
    size_t start = modificationHistory.size() > maxEntries ? 
                   modificationHistory.size() - maxEntries : 0;
    
    for (size_t i = start; i < modificationHistory.size(); ++i) {
        const auto& record = modificationHistory[i];
        history.emplace_back(record.key, record.newValue, ConfigSource::Runtime, 
                           "Modified at " + std::to_string(i + 1));
    }
    
    return history;
}

void ConfigAdvanced::clearCache() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->cache.clear();
    pImpl->cacheDirty = true;
}

void ConfigAdvanced::trackModification(const std::string& key, const ConfigValue& oldValue, 
                                     const ConfigValue& newValue, const std::string& modifiedBy) {
    if (!trackModifications) return;
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    modificationHistory.emplace_back(
        std::chrono::system_clock::now(),
        key,
        oldValue,
        newValue,
        modifiedBy
    );
    pImpl->modificationCount++;
}

void ConfigAdvanced::updateCache() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    if (!cacheEnabled) return;
    
    if (pImpl->cacheDirty) {
        pImpl->cache.clear();
        for (const auto& entry : pImpl->configData) {
            pImpl->cache[entry.first] = entry.second;
        }
        pImpl->cacheDirty = false;
    }
}

const ConfigValue* ConfigAdvanced::getCachedValue(const std::string& key) const {
    updateCache();
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = pImpl->cache.find(key);
    if (it != pImpl->cache.end()) {
        return &it->second;
    }
    return nullptr;
}

std::string ConfigAdvanced::diff(const ConfigAdvanced& other) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ostringstream oss;
    
    // Get all keys from both configurations
    std::vector<std::string> allKeys;
    for (const auto& entry : pImpl->configData) {
        allKeys.push_back(entry.first);
    }
    
    for (const auto& entry : other.pImpl->configData) {
        if (std::find(allKeys.begin(), allKeys.end(), entry.first) == allKeys.end()) {
            allKeys.push_back(entry.first);
        }
    }
    
    bool hasDiff = false;
    for (const auto& key : allKeys) {
        const ConfigValue* val1 = getCachedValue(key);
        const ConfigValue* val2 = other.getCachedValue(key);
        
        if (!val1 && !val2) continue;
        if ((val1 && !val2) || (!val1 && val2)) {
            if (!hasDiff) hasDiff = true;
            oss << "  " << key << ": [REMOVED] -> [ADDED]\n";
        } else if (val1 && val2 && !ConfigValidation::compareValues(*val1, *val2)) {
            if (!hasDiff) hasDiff = true;
            oss << "  " << key << ": " << ConfigValidation::formatDiff(key, *val1, *val2) << "\n";
        }
    }
    
    return hasDiff ? oss.str() : "No differences found.\n";
}

bool ConfigAdvanced::equals(const ConfigAdvanced& other) const {
    return diff(other) == "No differences found.\n";
}

bool ConfigAdvanced::saveVersion(const std::string& versionName, const std::string& comment) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (profiles.find(versionName) != profiles.end()) {
        return false;
    }
    
    VersionRecord record;
    record.name = versionName;
    record.comment = comment;
    record.timestamp = std::chrono::system_clock::now();
    record.configSnapshot = pImpl->configData;
    
    versionHistory.push_back(record);
    pImpl->versionCount++;
    
    return createProfile(versionName, comment);
}

bool ConfigAdvanced::restoreVersion(const std::string& versionName) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = profiles.find(versionName);
    if (it == profiles.end()) {
        return false;
    }
    
    // Find corresponding version record
    for (const auto& record : versionHistory) {
        if (record.name == versionName) {
            pImpl->configData = record.configSnapshot;
            currentVersion = versionName;
            updateCache();
            return true;
        }
    }
    
    return false;
}

std::vector<std::string> ConfigAdvanced::getVersionNames() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> names;
    names.reserve(versionHistory.size());
    for (const auto& record : versionHistory) {
        names.push_back(record.name);
    }
    return names;
}

bool ConfigAdvanced::rollback(size_t steps) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    if (!canRollback || versionHistory.empty() || steps == 0) {
        return false;
    }
    
    size_t restoreIndex = versionHistory.size() - steps;
    if (restoreIndex >= versionHistory.size()) {
        return false;
    }
    
    const auto& record = versionHistory[restoreIndex];
    pImpl->configData = record.configSnapshot;
    currentVersion = record.name;
    updateCache();
    
    return true;
}

bool ConfigAdvanced::validateDependencies() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Build dependency graph
    std::unordered_map<std::string, std::vector<std::string>> graph;
    
    for (const auto& entry : pImpl->configData) {
        // In a real implementation, we would check if the key exists
        // For now, we'll just return true
        graph[entry.first] = std::vector<std::string>();
    }
    
    std::unordered_set<std::string> visiting;
    std::unordered_set<std::string> visited;
    
    for (const auto& pair : graph) {
        if (!checkCircularDependencies(pair.first, graph, visiting, visited)) {
            return false;
        }
    }
    
    return true;
}

bool ConfigAdvanced::checkCircularDependencies(const std::string& key,
                                               const std::unordered_map<std::string, std::vector<std::string>>& graph,
                                               std::unordered_set<std::string>& visiting,
                                               std::unordered_set<std::string>& visited) const {
    if (visiting.find(key) != visiting.end()) {
        return false; // Circular dependency detected
    }
    
    if (visited.find(key) != visited.end()) {
        return true; // Already processed
    }
    
    visiting.insert(key);
    
    for (const auto& dep : graph.at(key)) {
        if (!checkCircularDependencies(dep, graph, visiting, visited)) {
            return false;
        }
    }
    
    visiting.erase(key);
    visited.insert(key);
    return true;
}

void ConfigAdvanced::addVersionRecord(const std::string& versionName, const std::string& comment) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    VersionRecord record;
    record.name = versionName;
    record.comment = comment;
    record.timestamp = std::chrono::system_clock::now();
    record.configSnapshot = pImpl->configData;
    
    versionHistory.push_back(record);
    pImpl->versionCount++;
}

} // namespace nlm