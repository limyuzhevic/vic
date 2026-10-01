#include "PluginConfigSystem.hpp"
#include "PluginCore.hpp"
#include "../core/Logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

namespace nlm {

struct PluginConfigSystem::Impl {
    bool active{false};
    
    std::mutex configMutex;
    
    std::map<std::string, std::map<std::string, PluginConfigEntry>> pluginConfigs;
    std::map<std::string, std::map<std::string, PluginConfigSchema>> pluginSchemas;
    std::map<std::string, std::map<std::string, PluginConfigValue>> pluginDefaults;
    
    std::map<std::string, PluginConfigValue> globalConfig;
    
    std::vector<std::string> systemMessages;
    std::vector<std::string> systemWarnings;
    std::vector<std::string> systemErrors;
    
    std::atomic<size_t> totalConfigs{0};
    std::atomic<double> systemMemoryUsage{0.0};
    
    Impl() {
        // Initialize with default schemas
    }
    
    void logMessage(const std::string& message, const std::string& level = "info") {
        if (level == "error") {
            systemErrors.push_back(message);
            NLM_LOG_ERROR(message);
        } else if (level == "warning") {
            systemWarnings.push_back(message);
            NLM_LOG_WARNING(message);
        } else {
            systemMessages.push_back(message);
            NLM_LOG_INFO(message);
        }
        
        // Keep only recent messages
        if (systemMessages.size() > 1000) {
            systemMessages.erase(systemMessages.begin());
        }
        if (systemWarnings.size() > 500) {
            systemWarnings.erase(systemWarnings.begin());
        }
        if (systemErrors.size() > 500) {
            systemErrors.erase(systemErrors.begin());
        }
    }
    
    size_t getPluginConfigCount(const std::string& pluginName) const {
        auto it = pluginConfigs.find(pluginName);
        if (it != pluginConfigs.end()) {
            return it->second.size();
        }
        return 0;
    }
    
    bool hasConfig(const std::map<std::string, PluginConfigEntry>& configEntries, const std::string& key) const {
        return configEntries.find(key) != configEntries.end();
    }
    
    PluginConfigValue getConfigValue(const std::map<std::string, PluginConfigEntry>& configEntries, const std::string& key) const {
        auto it = configEntries.find(key);
        if (it != configEntries.end()) {
            return it->second.value;
        }
        return PluginConfigValue();
    }
    
    bool setConfigValue(std::map<std::string, PluginConfigEntry>& configEntries, const std::string& key, 
                       const PluginConfigValue& value, const std::string& source, bool& modified) {
        auto it = configEntries.find(key);
        if (it != configEntries.end()) {
            // Update existing entry
            it->second.value = value;
            it->second.source = source;
            it->second.timestamp = std::chrono::system_clock::now();
            modified = (it->second.source != source) || (it->second.value != value);
        } else {
            // Create new entry
            PluginConfigSchema schema; // TODO: Get schema from plugin system
            configEntries[key] = PluginConfigEntry(key, schema, value);
            configEntries[key].source = source;
            modified = true;
        }
        
        return true;
    }
};

PluginConfigSystem::PluginConfigSystem() : pImpl(std::make_unique<Impl>()) {}

PluginConfigSystem::~PluginConfigSystem() {
    shutdown();
}

PluginConfigSystem::PluginConfigSystem(PluginConfigSystem&& other) noexcept : pImpl(std::move(other.pImpl)) {}

PluginConfigSystem& PluginConfigSystem::operator=(PluginConfigSystem&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool PluginConfigSystem::initialize() {
    if (pImpl->active) {
        logMessage("PluginConfigSystem: Already initialized", "warning");
        return true;
    }
    
    pImpl->active = true;
    
    logMessage("PluginConfigSystem: Initialized successfully");
    
    onSystemInitialized();
    
    return true;
}

void PluginConfigSystem::shutdown() {
    if (!pImpl->active) {
        return;
    }
    
    logMessage("PluginConfigSystem: Shutting down...");
    
    // Clear all configurations
    pImpl->pluginConfigs.clear();
    pImpl->pluginSchemas.clear();
    pImpl->pluginDefaults.clear();
    pImpl->globalConfig.clear();
    
    pImpl->active = false;
    
    logMessage("PluginConfigSystem: Shutdown successfully");
    
    onSystemShutdown();
}

bool PluginConfigSystem::isActive() const {
    return pImpl->active;
}

bool PluginConfigSystem::setPluginConfig(const std::string& name, const std::string& key, const PluginConfigValue& value,
                                        const std::string& source) {
    return setPluginConfigInternal(name, key, value, source);
}

bool PluginConfigSystem::setPluginConfig(const std::string& name, const std::map<std::string, PluginConfigValue>& config,
                                        const std::string& source) {
    bool success = true;
    
    for (const auto& pair : config) {
        if (!setPluginConfigInternal(name, pair.first, pair.second, source)) {
            success = false;
        }
    }
    
    return success;
}

PluginConfigValue PluginConfigSystem::getPluginConfig(const std::string& name, const std::string& key) const {
    return getPluginConfigInternal(name, key);
}

std::map<std::string, PluginConfigValue> PluginConfigSystem::getPluginConfig(const std::string& name) const {
    std::map<std::string, PluginConfigValue> config;
    
    auto it = pImpl->pluginConfigs.find(name);
    if (it != pImpl->pluginConfigs.end()) {
        for (const auto& pair : it->second) {
            config[pair.first] = pair.second.value;
        }
    }
    
    return config;
}

bool PluginConfigSystem::registerConfigSchema(const std::string& pluginName, const PluginConfigSchema& schema) {
    return registerConfigSchemaInternal(pluginName, schema);
}

bool PluginConfigSystem::unregisterConfigSchema(const std::string& pluginName, const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    auto it = pImpl->pluginSchemas.find(pluginName);
    if (it == pImpl->pluginSchemas.end()) {
        return false;
    }
    
    it->second.erase(key);
    
    logMessage("PluginConfigSystem: Unregistered config schema for plugin " + pluginName + ", key " + key);
    
    return true;
}

PluginConfigSchema PluginConfigSystem::getConfigSchema(const std::string& pluginName, const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    auto it1 = pImpl->pluginSchemas.find(pluginName);
    if (it1 == pImpl->pluginSchemas.end()) {
        return PluginConfigSchema();
    }
    
    auto it2 = it1->second.find(key);
    if (it2 == it1->second.end()) {
        return PluginConfigSchema();
    }
    
    return it2->second;
}

std::vector<PluginConfigSchema> PluginConfigSystem::getPluginConfigSchemas(const std::string& pluginName) const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    auto it = pImpl->pluginSchemas.find(pluginName);
    if (it != pImpl->pluginSchemas.end()) {
        std::vector<PluginConfigSchema> schemas;
        for (const auto& pair : it->second) {
            schemas.push_back(pair.second);
        }
        return schemas;
    }
    
    return std::vector<PluginConfigSchema>();
}

bool PluginConfigSystem::validatePluginConfig(const std::string& name, const std::string& key, const PluginConfigValue& value) const {
    return validatePluginConfigInternal(name, key, value);
}

bool PluginConfigSystem::validatePluginConfig(const std::string& name, const std::map<std::string, PluginConfigValue>& config) const {
    return validatePluginConfigInternal(name, config);
}

std::vector<std::string> PluginConfigSystem::getPluginConfigValidationErrors(const std::string& name) const {
    std::vector<std::string> errors;
    
    auto it = pImpl->pluginConfigs.find(name);
    if (it != pImpl->pluginConfigs.end()) {
        for (const auto& pair : it->second) {
            std::vector<std::string> keyErrors = getPluginConfigValidationErrors(name, pair.first);
            errors.insert(errors.end(), keyErrors.begin(), keyErrors.end());
        }
    }
    
    return errors;
}

std::vector<std::string> PluginConfigSystem::getPluginConfigValidationErrors(const std::string& name, const std::string& key) const {
    std::vector<std::string> errors;
    
    PluginConfigValue value = getPluginConfigInternal(name, key);
    PluginConfigSchema schema = getConfigSchema(name, key);
    
    if (!schema.validate(value)) {
        errors.push_back("Configuration value does not match schema for " + key);
    }
    
    return errors;
}

bool PluginConfigSystem::setPluginDefaultConfig(const std::string& name, const std::map<std::string, PluginConfigValue>& config) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    pImpl->pluginDefaults[name] = config;
    
    logMessage("PluginConfigSystem: Set default config for plugin " + name);
    
    return true;
}

std::map<std::string, PluginConfigValue> PluginConfigSystem::getPluginDefaultConfig(const std::string& name) const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    auto it = pImpl->pluginDefaults.find(name);
    if (it != pImpl->pluginDefaults.end()) {
        return it->second;
    }
    
    return std::map<std::string, PluginConfigValue>();
}

bool PluginConfigSystem::resetPluginConfigToDefaults(const std::string& name) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    auto itDefaults = pImpl->pluginDefaults.find(name);
    if (itDefaults == pImpl->pluginDefaults.end()) {
        return false;
    }
    
    auto itConfig = pImpl->pluginConfigs.find(name);
    if (itConfig != pImpl->pluginConfigs.end()) {
        // Replace current config with defaults
        itConfig->second.clear();
        for (const auto& pair : itDefaults->second) {
            itConfig->second[pair.first] = PluginConfigEntry(pair.first, PluginConfigSchema(), pair.second);
            itConfig->second[pair.first].source = "default";
        }
    }
    
    logMessage("PluginConfigSystem: Reset config for plugin " + name + " to defaults");
    
    return true;
}

bool PluginConfigSystem::resetAllPluginConfigsToDefaults() {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    for (const auto& pair : pImpl->pluginDefaults) {
        resetPluginConfigToDefaults(pair.first);
    }
    
    logMessage("PluginConfigSystem: Reset all plugin configs to defaults");
    
    return true;
}

bool PluginConfigSystem::savePluginConfig(const std::string& name, const std::string& filepath) const {
    return savePluginConfigInternal(name, filepath);
}

bool PluginConfigSystem::loadPluginConfig(const std::string& name, const std::string& filepath) {
    return loadPluginConfigInternal(name, filepath);
}

bool PluginConfigSystem::saveAllPluginConfigs(const std::string& directory) const {
    return saveAllPluginConfigsInternal(directory);
}

bool PluginConfigSystem::loadAllPluginConfigs(const std::string& directory) {
    return loadAllPluginConfigsInternal(directory);
}

bool PluginConfigSystem::markPluginConfigModified(const std::string& name, const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    auto it1 = pImpl->pluginConfigs.find(name);
    if (it1 == pImpl->pluginConfigs.end()) {
        return false;
    }
    
    auto it2 = it1->second.find(key);
    if (it2 == it1->second.end()) {
        return false;
    }
    
    it2->second.modified = true;
    it2->second.timestamp = std::chrono::system_clock::now();
    
    return true;
}

bool PluginConfigSystem::isPluginConfigModified(const std::string& name, const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    auto it1 = pImpl->pluginConfigs.find(name);
    if (it1 == pImpl->pluginConfigs.end()) {
        return false;
    }
    
    auto it2 = it1->second.find(key);
    if (it2 == it1->second.end()) {
        return false;
    }
    
    return it2->second.modified;
}

std::chrono::system_clock::time_point PluginConfigSystem::getPluginConfigTimestamp(const std::string& name, const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    auto it1 = pImpl->pluginConfigs.find(name);
    if (it1 == pImpl->pluginConfigs.end()) {
        return std::chrono::system_clock::time_point();
    }
    
    auto it2 = it1->second.find(key);
    if (it2 == it1->second.end()) {
        return std::chrono::system_clock::time_point();
    }
    
    return it2->second.timestamp;
}

size_t PluginConfigSystem::getPluginConfigCount(const std::string& name) const {
    return pImpl->getPluginConfigCount(name);
}

size_t PluginConfigSystem::getSystemConfigCount() const {
    return pImpl->totalConfigs;
}

double PluginConfigSystem::getSystemMemoryUsage() const {
    return pImpl->systemMemoryUsage;
}

bool PluginConfigSystem::setGlobalConfig(const std::map<std::string, PluginConfigValue>& config) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    pImpl->globalConfig = config;
    
    logMessage("PluginConfigSystem: Set global configuration");
    
    return true;
}

std::map<std::string, PluginConfigValue> PluginConfigSystem::getGlobalConfig() const {
    return pImpl->globalConfig;
}

bool PluginConfigSystem::clearGlobalConfig() {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    pImpl->globalConfig.clear();
    
    logMessage("PluginConfigSystem: Cleared global configuration");
    
    return true;
}

bool PluginConfigSystem::executeConfigCommand(const std::string& name, const std::string& command,
                                             const std::map<std::string, std::string>& args) {
    // TODO: Implement config command execution
    return true;
}

bool PluginConfigSystem::executeSystemConfigCommand(const std::string& command,
                                                   const std::map<std::string, std::string>& args) {
    // TODO: Implement system config command execution
    return true;
}

bool PluginConfigSystem::cleanConfigCache() {
    // TODO: Implement cache cleaning
    return true;
}

bool PluginConfigSystem::validateAllConfigs() {
    return validateAllConfigsInternal();
}

bool PluginConfigSystem::repairAllConfigs() {
    // TODO: Implement config repair
    return true;
}

bool PluginConfigSystem::optimizeConfigSystem() {
    // TODO: Implement system optimization
    return true;
}

std::string PluginConfigSystem::getConfigStatus() const {
    std::stringstream ss;
    ss << "PluginConfigSystem Status:" << std::endl;
    ss << "  Active: " << (isActive() ? "true" : "false") << std::endl;
    ss << "  Total Configs: " << getSystemConfigCount() << std::endl;
    ss << "  System Memory Usage: " << getSystemMemoryUsage() << " MB" << std::endl;
    
    for (const auto& pair : pImpl->pluginConfigs) {
        ss << "  Plugin " << pair.first << ": " << pair.second.size() << " configs" << std::endl;
    }
    
    return ss.str();
}

std::string PluginConfigSystem::getConfigInfo() const {
    std::stringstream ss;
    ss << "PluginConfigSystem Information:" << std::endl;
    ss << "  Global Config Entries: " << pImpl->globalConfig.size() << std::endl;
    ss << "  Plugin Config Entries: " << pImpl->pluginConfigs.size() << std::endl;
    ss << "  Plugin Schema Entries: " << pImpl->pluginSchemas.size() << std::endl;
    ss << "  Default Config Entries: " << pImpl->pluginDefaults.size() << std::endl;
    
    return ss.str();
}

std::vector<std::string> PluginConfigSystem::getConfigMessages() const {
    return pImpl->systemMessages;
}

std::vector<std::string> PluginConfigSystem::getConfigWarnings() const {
    return pImpl->systemWarnings;
}

std::vector<std::string> PluginConfigSystem::getConfigErrors() const {
    return pImpl->systemErrors;
}

bool PluginConfigSystem::exportConfigData(const std::string& filename, const std::string& format) const {
    // TODO: Implement config data export
    return true;
}

bool PluginConfigSystem::importConfigData(const std::string& filename, const std::string& format) {
    // TODO: Implement config data import
    return true;
}

std::string PluginConfigSystem::getAbout() const {
    return "PluginConfigSystem - Plugin Configuration Management System for NLM";
}

std::string PluginConfigSystem::getVersion() const {
    return "1.0.0";
}

std::string PluginConfigSystem::getBuildInfo() const {
    return "PluginConfigSystem Build Information";
}

void PluginConfigSystem::onConfigChanged(const std::string& pluginName, const std::string& key, const PluginConfigValue& value) {
    // Default implementation - can be overridden
    logMessage("PluginConfigSystem: Configuration changed for plugin " + pluginName + ", key " + key);
}

void PluginConfigSystem::onConfigValidationError(const std::string& pluginName, const std::string& key, const std::string& error) {
    // Default implementation - can be overridden
    logMessage("PluginConfigSystem: Configuration validation error for plugin " + pluginName + ", key " + key + ": " + error, "warning");
}

void PluginConfigSystem::onConfigSaved(const std::string& pluginName, const std::string& filepath) {
    // Default implementation - can be overridden
    logMessage("PluginConfigSystem: Configuration saved for plugin " + pluginName + ", file " + filepath);
}

void PluginConfigSystem::onConfigLoaded(const std::string& pluginName, const std::string& filepath) {
    // Default implementation - can be overridden
    logMessage("PluginConfigSystem: Configuration loaded for plugin " + pluginName + ", file " + filepath);
}

void PluginConfigSystem::onSystemInitialized() {
    // Default implementation - can be overridden
    logMessage("PluginConfigSystem: System initialized");
}

void PluginConfigSystem::onSystemShutdown() {
    // Default implementation - can be overridden
    logMessage("PluginConfigSystem: System shutting down");
}

void PluginConfigSystem::onSystemError(const std::string& error) {
    // Default implementation - can be overridden
    logMessage("PluginConfigSystem: System error: " + error, "error");
}

void PluginConfigSystem::onSystemWarning(const std::string& warning) {
    // Default implementation - can be overridden
    logMessage("PluginConfigSystem: System warning: " + warning, "warning");
}

// Private helper methods implementation
bool PluginConfigSystem::setPluginConfigInternal(const std::string& name, const std::string& key, const PluginConfigValue& value,
                                               const std::string& source) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    bool modified = false;
    if (!setConfigValue(pImpl->pluginConfigs[name], key, value, source, modified)) {
        return false;
    }
    
    if (modified) {
        onConfigChanged(name, key, value);
    }
    
    pImpl->totalConfigs++;
    
    logMessage("PluginConfigSystem: Set config for plugin " + name + ", key " + key + ", value " + configValueToString(value));
    
    return true;
}

PluginConfigValue PluginConfigSystem::getPluginConfigInternal(const std::string& name, const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    auto it1 = pImpl->pluginConfigs.find(name);
    if (it1 == pImpl->pluginConfigs.end()) {
        return PluginConfigValue();
    }
    
    return getConfigValue(it1->second, key);
}

bool PluginConfigSystem::registerConfigSchemaInternal(const std::string& pluginName, const PluginConfigSchema& schema) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    pImpl->pluginSchemas[pluginName][schema.name] = schema;
    
    logMessage("PluginConfigSystem: Registered config schema for plugin " + pluginName + ", key " + schema.name);
    
    return true;
}

bool PluginConfigSystem::validatePluginConfigInternal(const std::string& name, const std::string& key, const PluginConfigValue& value) const {
    PluginConfigSchema schema = getConfigSchema(name, key);
    
    if (schema.validate(value)) {
        return true;
    }
    
    logMessage("PluginConfigSystem: Config validation failed for plugin " + name + ", key " + key, "warning");
    
    return false;
}

bool PluginConfigSystem::validatePluginConfigInternal(const std::string& name, const std::map<std::string, PluginConfigValue>& config) const {
    bool success = true;
    
    for (const auto& pair : config) {
        if (!validatePluginConfigInternal(name, pair.first, pair.second)) {
            success = false;
        }
    }
    
    return success;
}

bool PluginConfigSystem::savePluginConfigInternal(const std::string& name, const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    auto it = pImpl->pluginConfigs.find(name);
    if (it == pImpl->pluginConfigs.end()) {
        logMessage("PluginConfigSystem: No config found for plugin " + name, "error");
        return false;
    }
    
    nlohmann::json json = configToJson(it->second);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        logMessage("PluginConfigSystem: Failed to open file for writing: " + filepath, "error");
        return false;
    }
    
    file << json.dump(2);
    file.close();
    
    logMessage("PluginConfigSystem: Saved config for plugin " + name + " to " + filepath);
    
    onConfigSaved(name, filepath);
    
    return true;
}

bool PluginConfigSystem::loadPluginConfigInternal(const std::string& name, const std::string& filepath) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    
    if (!std::filesystem::exists(filepath)) {
        logMessage("PluginConfigSystem: Config file does not exist: " + filepath, "error");
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        logMessage("PluginConfigSystem: Failed to open file for reading: " + filepath, "error");
        return false;
    }
    
    nlohmann::json json;
    try {
        file >> json;
    } catch (const std::exception& e) {
        logMessage("PluginConfigSystem: Failed to parse JSON from file " + filepath + ": " + std::string(e.what()), "error");
        file.close();
        return false;
    }
    
    file.close();
    
    std::string loadedPluginName;
    std::map<std::string, PluginConfigValue> loadedConfig;
    if (!jsonToPluginConfig(json, loadedPluginName, loadedConfig)) {
        logMessage("PluginConfigSystem: Failed to parse config from JSON" + filepath, "error");
        return false;
    }
    
    if (loadedPluginName != name) {
        logMessage("PluginConfigSystem: Plugin name mismatch: expected " + name + ", found " + loadedPluginName, "warning");
    }
    
    // Replace current config with loaded config
    pImpl->pluginConfigs[name].clear();
    for (const auto& pair : loadedConfig) {
        setConfigValue(pImpl->pluginConfigs[name], pair.first, pair.second, "file", true);
    }
    
    logMessage("PluginConfigSystem: Loaded config for plugin " + name + " from " + filepath);
    
    onConfigLoaded(name, filepath);
    
    return true;
}

bool PluginConfigSystem::saveAllPluginConfigsInternal(const std::string& directory) const {
    if (!std::filesystem::exists(directory)) {
        std::filesystem::create_directories(directory);
    }
    
    bool success = true;
    
    for (const auto& pair : pImpl->pluginConfigs) {
        std::string filepath = directory + "/" + pair.first + ".config";
        if (!savePluginConfigInternal(pair.first, filepath)) {
            logMessage("PluginConfigSystem: Failed to save config for plugin " + pair.first, "warning");
            success = false;
        }
    }
    
    return success;
}

bool PluginConfigSystem::loadAllPluginConfigsInternal(const std::string& directory) {
    if (!std::filesystem::exists(directory)) {
        return true;
    }
    
    bool success = true;
    
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            if (filename.find(".config") != std::string::npos) {
                std::string pluginName = filename.substr(0, filename.find_last_of("."));
                if (!loadPluginConfigInternal(pluginName, entry.path().string())) {
                    logMessage("PluginConfigSystem: Failed to load config for plugin " + pluginName, "warning");
                    success = false;
                }
            }
        }
    }
    
    return success;
}

bool PluginConfigSystem::validateAllConfigsInternal() const {
    bool success = true;
    
    for (const auto& pair : pImpl->pluginConfigs) {
        for (const auto& configPair : pair.second) {
            if (!configPair.second.schema.validate(configPair.second.value)) {
                logMessage("PluginConfigSystem: Config validation failed for plugin " + pair.first + ", key " + configPair.first, "warning");
                success = false;
            }
        }
    }
    
    return success;
}

nlohmann::json PluginConfigSystem::configToJson(const std::map<std::string, PluginConfigEntry>& configEntries) const {
    nlohmann::json json;
    
    for (const auto& pair : configEntries) {
        json[pair.first] = configValueToJson(pair.second.value);
    }
    
    return json;
}

bool PluginConfigSystem::jsonToPluginConfig(const nlohmann::json& json, std::string& pluginName, std::map<std::string, PluginConfigValue>& config) {
    // For now, we'll assume the JSON is just a map of key-value pairs
    // In a real implementation, we would need to store and retrieve the plugin name
    for (const auto& item : json.items()) {
        config[item.key()] = jsonToConfigValue(item.value(), "string"); // Default type
    }
    
    pluginName = "unknown";
    
    return true;
}

bool PluginConfigSystem::validateConfigValueInternal(const PluginConfigValue& value, const std::string& type) const {
    return validateConfigValue(value, type);
}

bool PluginConfigSystem::validateConfigPattern(const std::string& value, const std::string& pattern) const {
    std::regex regex(pattern);
    return std::regex_match(value, regex);
}

// Static helper methods implementation
bool PluginConfigSystem::validateConfigValue(const PluginConfigValue& value, const std::string& type) {
    switch (value.index()) {
        case 0: // int
            if (type == "int" || type == "integer" || type == "number") {
                return true;
            }
            break;
        case 1: // int64_t
            if (type == "int" || type == "integer" || type == "number") {
                return true;
            }
            break;
        case 2: // double
            if (type == "number" || type == "float" || type == "double") {
                return true;
            }
            break;
        case 3: // bool
            if (type == "boolean" || type == "bool") {
                return true;
            }
            break;
        case 4: // std::string
            if (type == "string" || type == "text") {
                return true;
            }
            break;
        case 5: // std::vector<int>
            if (type == "array" || type == "list" || type == "array<int>") {
                return true;
            }
            break;
        case 6: // std::vector<double>
            if (type == "array" || type == "list" || type == "array<double>") {
                return true;
            }
            break;
        case 7: // std::vector<std::string>
            if (type == "array" || type == "list" || type == "array<string>") {
                return true;
            }
            break;
    }
    return false;
}

std::string PluginConfigSystem::configValueToString(const PluginConfigValue& value) {
    return PluginConfigSystem::configValueToString(value);
}

PluginConfigValue PluginConfigSystem::configValueFromString(const std::string& str, const std::string& type) {
    return PluginConfigValue();
}

bool PluginConfigSystem::matchConfigPattern(const std::string& value, const std::string& pattern) {
    return PluginConfigSystem::matchConfigPattern(value, pattern);
}

nlohmann::json PluginConfigSystem::configValueToJson(const PluginConfigValue& value) {
    // Delegate to the function defined in PluginCore.hpp
    return configValueToJson(value);
}

PluginConfigValue PluginConfigSystem::jsonToConfigValue(const nlohmann::json& json, const std::string& type) {
    // Delegate to the function defined in PluginCore.hpp
    return jsonToConfigValue(json, type);
}

} // namespace nlm