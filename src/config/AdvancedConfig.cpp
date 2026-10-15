#include "AdvancedConfig.hpp"
#include "../core/Logger/Logger.hpp"
#include <yaml-cpp/yaml.h>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <filesystem>

namespace nlm {

struct AdvancedConfig::Impl {
    // Configuration storage
    Config currentConfig;
    std::map<std::string, Config> pluginConfigs;
    
    // Environment variables
    std::map<std::string, std::string> environmentVars;
    
    // Inheritance system
    std::vector<std::string> inheritancePaths;
    std::map<std::string, Config> inheritedConfigs;
    
    // Schema validation
    std::vector<std::pair<std::string, std::function<bool(const Config&)>>> validators;
    std::vector<std::string> validationErrors;
    
    // Runtime updates
    struct ScheduledUpdate {
        Config config;
        double scheduledTime;
        bool processed;
    };
    std::vector<ScheduledUpdate> scheduledUpdates;
    
    // Hot reloading
    bool hotReloadingEnabled;
    std::chrono::system_clock::duration reloadInterval;
    std::chrono::system_clock::time_point lastReload;
    std::string currentConfigHash;
    
    // Statistics
    size_t version;
    std::chrono::system_clock::time_point lastModified;
    
    Logger* logger;
};

AdvancedConfig::AdvancedConfig() : pImpl(std::make_unique<Impl>()) {
    pImpl->logger = Logger::getInstance();
    pImpl->version = 1;
    pImpl->lastModified = std::chrono::system_clock::now();
    pImpl->hotReloadingEnabled = false;
    pImpl->reloadInterval = std::chrono::seconds(30);
    pImpl->lastReload = pImpl->lastModified;
}

AdvancedConfig::~AdvancedConfig() {
    shutdown();
}

AdvancedConfig::AdvancedConfig(AdvancedConfig&& other) noexcept : pImpl(std::move(other.pImpl)) {}

AdvancedConfig& AdvancedConfig::operator=(AdvancedConfig&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool AdvancedConfig::initialize(const std::string& configFile) {
    bool success = true;
    
    if (!configFile.empty()) {
        success = loadFromFile(configFile);
    }
    
    loadFromEnvironment();
    pImpl->logger->info("Advanced configuration initialized");
    return success;
}

void AdvancedConfig::update(double timestep) {
    // Process scheduled updates
    auto now = std::chrono::system_clock::now();
    double currentTime = std::chrono::duration<double>(now.time_since_epoch()).count();
    
    std::vector<ScheduledUpdate> completedUpdates;
    for (auto& update : pImpl->scheduledUpdates) {
        if (!update.processed && currentTime >= update.scheduledTime) {
            applyConfiguration(update.config);
            update.processed = true;
            completedUpdates.push_back(update);
        }
    }
    
    // Remove completed updates
    for (const auto& update : completedUpdates) {
        auto it = std::find_if(pImpl->scheduledUpdates.begin(), pImpl->scheduledUpdates.end(),
                              [&update](const ScheduledUpdate& u) { return &u == &update; });
        if (it != pImpl->scheduledUpdates.end()) {
            pImpl->scheduledUpdates.erase(it);
        }
    }
    
    // Check for hot reloads
    if (pImpl->hotReloadingEnabled) {
        auto lastCheck = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(lastCheck - pImpl->lastReload);
        if (duration >= pImpl->reloadInterval) {
            // In real implementation, check for file changes
            pImpl->lastReload = std::chrono::system_clock::now();
        }
    }
}

void AdvancedConfig::shutdown() {
    pImpl->logger->info("Advanced configuration shutdown");
    pImpl->currentConfig.clear();
    pImpl->pluginConfigs.clear();
    pImpl->environmentVars.clear();
    pImpl->inheritedConfigs.clear();
    pImpl->scheduledUpdates.clear();
}

bool AdvancedConfig::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file) {
            pImpl->logger->error("Failed to open config file: " + filepath);
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        bool success = loadFromString(buffer.str());
        
        if (success) {
            pImpl->logger->info("Configuration loaded from: " + filepath);
        }
        
        return success;
    } catch (const std::exception& e) {
        pImpl->logger->error("Failed to load config file: " + std::string(e.what()));
        return false;
    }
}

bool AdvancedConfig::loadFromString(const std::string& yamlString) {
    try {
        YAML::Node root = YAML::Load(yamlString);
        
        // Clear existing config
        pImpl->currentConfig.clear();
        
        // Parse YAML configuration
        if (root["config"]) {
            YAML::Node config = root["config"];
            for (auto it = config.begin(); it != config.end(); ++it) {
                std::string key = it->first.as<std::string>();
                
                if (it->IsScalar()) {
                    pImpl->currentConfig.set(key, it->as<std::string>());
                } else if (it->IsSequence()) {
                    std::vector<double> vec;
                    for (const auto& item : *it) {
                        vec.push_back(item.as<double>());
                    }
                    pImpl->currentConfig.set(key, vec);
                } else if (it->IsMap()) {
                    // Nested configuration (not directly supported by base Config)
                    // Would need extension
                }
            }
        }
        
        pImpl->version++;
        pImpl->lastModified = std::chrono::system_clock::now();
        pImpl->currentConfigHash = getConfigurationHash();
        
        return true;
    } catch (const YAML::Exception& e) {
        pImpl->logger->error("YAML parsing error: " + std::string(e.what()));
        return false;
    }
}

bool AdvancedConfig::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file) {
            pImpl->logger->error("Failed to open config file for writing: " + filepath);
            return false;
        }
        
        file << exportToYAML();
        pImpl->logger->info("Configuration saved to: " + filepath);
        return true;
    } catch (const std::exception& e) {
        pImpl->logger->error("Failed to save config file: " + std::string(e.what()));
        return false;
    }
}

std::string AdvancedConfig::exportToYAML() const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    emitter << YAML::Key << "config";
    emitter << YAML::Value << YAML::BeginMap;
    
    auto keys = pImpl->currentConfig.getKeys();
    for (const auto& key : keys) {
        auto value = pImpl->currentConfig.get<ConfigValue>(key);
        emitter << YAML::Key << key;
        
        if (std::holds_alternative<int>(value)) {
            emitter << std::get<int>(value);
        } else if (std::holds_alternative<int64_t>(value)) {
            emitter << std::get<int64_t>(value);
        } else if (std::holds_alternative<double>(value)) {
            emitter << std::get<double>(value);
        } else if (std::holds_alternative<bool>(value)) {
            emitter << std::get<bool>(value);
        } else if (std::holds_alternative<std::string>(value)) {
            emitter << std::get<std::string>(value);
        } else if (std::holds_alternative<std::vector<int>>(value)) {
            emitter << YAML::Flow << std::get<std::vector<int>>(value);
        } else if (std::holds_alternative<std::vector<double>>(value)) {
            emitter << YAML::Flow << std::get<std::vector<double>>(value);
        } else if (std::holds_alternative<std::vector<std::string>>(value)) {
            emitter << YAML::Flow << std::get<std::vector<std::string>>(value);
        }
    }
    
    emitter << YAML::EndMap;
    emitter << YAML::EndMap;
    
    return emitter.c_str();
}

bool AdvancedConfig::importFromYAML(const std::string& yamlString) {
    return loadFromString(yamlString);
}

void AdvancedConfig::loadFromEnvironment() {
    // Load from environment variables
    for (const auto& envVar : environmentVars) {
        const char* value = std::getenv(envVar.first.c_str());
        if (value) {
            setEnvironmentVariable(envVar.first, value);
        }
    }
}

bool AdvancedConfig::setEnvironmentVariable(const std::string& name, const std::string& value) {
    if (name.empty() || value.empty()) return false;
    
    pImpl->environmentVars[name] = value;
    
    // Map environment variable to config key
    std::string configKey = "env_" + name;
    pImpl->currentConfig.set(configKey, value);
    
    return true;
}

std::string AdvancedConfig::getEnvironmentVariable(const std::string& name) const {
    auto it = pImpl->environmentVars.find(name);
    if (it != pImpl->environmentVars.end()) {
        return it->second;
    }
    
    const char* value = std::getenv(name.c_str());
    return value ? value : "";
}

void AdvancedConfig::applyConfiguration(const Config& config) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Merge new configuration with existing
    for (const auto& key : config.getKeys()) {
        auto value = config.get<ConfigValue>(key);
        pImpl->currentConfig.set(key, value);
    }
    
    pImpl->version++;
    pImpl->lastModified = std::chrono::system_clock::now();
    pImpl->currentConfigHash = getConfigurationHash();
    
    pImpl->logger->info("Configuration updated at runtime");
}

void AdvancedConfig::scheduleConfigurationUpdate(const Config& config, double delay) {
    ScheduledUpdate update;
    update.config = config;
    update.scheduledTime = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count() + delay;
    update.processed = false;
    
    pImpl->scheduledUpdates.push_back(update);
}

void AdvancedConfig::cancelScheduledUpdates() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    for (auto& update : pImpl->scheduledUpdates) {
        update.processed = true;
    }
}

bool AdvancedConfig::inheritFrom(const std::string& baseConfigPath) {
    if (!std::filesystem::exists(baseConfigPath)) {
        pImpl->logger->error("Base config file does not exist: " + baseConfigPath);
        return false;
    }
    
    // Load base configuration
    Config baseConfig;
    if (!baseConfig.loadFromFile(baseConfigPath)) {
        return false;
    }
    
    pImpl->inheritedConfigs[baseConfigPath] = baseConfig;
    pImpl->inheritancePaths.push_back(baseConfigPath);
    
    pImpl->logger->info("Configuration inheritance added: " + baseConfigPath);
    return true;
}

void AdvancedConfig::addInheritancePath(const std::string& path) {
    pImpl->inheritancePaths.push_back(path);
}

void AdvancedConfig::removeInheritancePath(const std::string& path) {
    auto it = std::find(pImpl->inheritancePaths.begin(), pImpl->inheritancePaths.end(), path);
    if (it != pImpl->inheritancePaths.end()) {
        pImpl->inheritancePaths.erase(it);
    }
}

std::vector<std::string> AdvancedConfig::getInheritanceChain() const {
    return pImpl->inheritancePaths;
}

std::map<std::string, Config> AdvancedConfig::getMergedConfiguration() const {
    std::map<std::string, Config> merged;
    
    // Start with base config
    for (const auto& path : pImpl->inheritancePaths) {
        if (pImpl->inheritedConfigs.find(path) != pImpl->inheritedConfigs.end()) {
            merged[path] = pImpl->inheritedConfigs.at(path);
        }
    }
    
    // Add current config (overrides inherited)
    merged["current"] = pImpl->currentConfig;
    
    return merged;
}

bool AdvancedConfig::validateConfiguration() const {
    pImpl->validationErrors.clear();
    
    // Run all validators
    for (const auto& validator : pImpl->validators) {
        if (!validator.second(pImpl->currentConfig)) {
            pImpl->validationErrors.push_back("Validation failed for: " + validator.first);
        }
    }
    
    return pImpl->validationErrors.empty();
}

bool AdvancedConfig::addSchemaValidator(const std::string& name, std::function<bool(const Config&)> validator) {
    if (!validator) {
        pImpl->logger->error("Cannot add null validator: " + name);
        return false;
    }
    
    pImpl->validators.push_back({name, validator});
    pImpl->logger->info("Schema validator added: " + name);
    return true;
}

std::vector<std::string> AdvancedConfig::getValidationErrors() const {
    return pImpl->validationErrors;
}

std::string AdvancedConfig::getValidationReport() const {
    std::stringstream ss;
    ss << "Configuration Validation Report:\n";
    ss << "==============================\n";
    
    if (pImpl->validationErrors.empty()) {
        ss << "Configuration is valid.\n";
    } else {
        ss << "Validation Errors:\n";
        for (const auto& error : pImpl->validationErrors) {
            ss << "  - " << error << "\n";
        }
    }
    
    return ss.str();
}

template<typename T>
std::optional<T> AdvancedConfig::get(const std::string& key) const {
    return pImpl->currentConfig.get<T>(key);
}

template<typename T>
T AdvancedConfig::getOr(const std::string& key, const T& defaultValue) const {
    return pImpl->currentConfig.getOr(key, defaultValue);
}

bool AdvancedConfig::has(const std::string& key) const {
    return pImpl->currentConfig.has(key);
}

std::vector<std::string> AdvancedConfig::getKeys() const {
    return pImpl->currentConfig.getKeys();
}

std::map<std::string, ConfigValue> AdvancedConfig::getAllValues() const {
    return pImpl->currentConfig.getAllValues();
}

size_t AdvancedConfig::getVersion() const {
    return pImpl->version;
}

std::chrono::system_clock::time_point AdvancedConfig::getLastModified() const {
    return pImpl->lastModified;
}

std::string AdvancedConfig::getConfigurationHash() const {
    std::string data;
    auto keys = pImpl->currentConfig.getKeys();n    
    for (const auto& key : keys) {
        auto value = pImpl->currentConfig.get<ConfigValue>(key);
        data += key + "=";
        
        if (std::holds_alternative<std::string>(value)) {
            data += std::get<std::string>(value);
        } else {
            // Convert other types to string representation
            std::stringstream ss;
            if (std::holds_alternative<int>(value)) ss << std::get<int>(value);
            else if (std::holds_alternative<int64_t>(value)) ss << std::get<int64_t>(value);
            else if (std::holds_alternative<double>(value)) ss << std::get<double>(value);
            else if (std::holds_alternative<bool>(value)) ss << (std::get<bool>(value) ? "true" : "false");
            
            data += ss.str();
        }
        
        data += "&";
    }
    
    // Generate SHA256 hash
    SHA256_CTX ctx;
    SHA256_Init(&ctx);
    SHA256_Update(&ctx, data.c_str(), data.size());
    
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_Final(hash, &ctx);
    
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    
    return ss.str();
}

void AdvancedConfig::enableHotReloading(bool enable) {
    pImpl->hotReloadingEnabled = enable;
}

bool AdvancedConfig::isHotReloadingEnabled() const {
    return pImpl->hotReloadingEnabled;
}

std::chrono::system_clock::duration AdvancedConfig::getReloadInterval() const {
    return pImpl->reloadInterval;
}

void AdvancedConfig::setReloadInterval(std::chrono::system_clock::duration interval) {
    pImpl->reloadInterval = interval;
}

bool AdvancedConfig::loadPluginConfiguration(const std::string& pluginName) {
    std::string configPath = pluginName + ".config.yaml";
    if (!std::filesystem::exists(configPath)) {
        pImpl->logger->warning("Plugin configuration file not found: " + configPath);
        return false;
    }
    
    Config config;
    if (config.loadFromFile(configPath)) {
        pImpl->pluginConfigs[pluginName] = config;
        pImpl->logger->info("Plugin configuration loaded: " + pluginName);
        return true;
    }
    
    return false;
}

bool AdvancedConfig::savePluginConfiguration(const std::string& pluginName, const Config& config) {
    std::string configPath = pluginName + ".config.yaml";
    if (config.saveToFile(configPath)) {
        pImpl->pluginConfigs[pluginName] = config;
        pImpl->logger->info("Plugin configuration saved: " + pluginName);
        return true;
    }
    
    return false;
}

std::map<std::string, Config> AdvancedConfig::getAllPluginConfigurations() const {
    return pImpl->pluginConfigs;
}

} // namespace nlm
