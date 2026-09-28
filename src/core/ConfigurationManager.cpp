#include "ConfigurationManager.hpp"
#include <fstream>
#include <filesystem>
#include <cmath>
#include <algorithm>
#include <unordered_set>
#include <sstream>

namespace nlm {

struct ConfigurationManager::Impl {
    std::map<Environment, std::shared_ptr<Config>> environmentConfigs;
    std::map<PresetType, ConfigurationManager> presets;
    std::map<std::string, ConfigurationSchema> schemas;
    std::map<std::string, std::unique_ptr<ConfigurationTemplate>> templates;
    std::vector<ConfigurationChange> changeHistory;
    Environment currentEnvironment;
    bool historyEnabled;
    bool changeTrackingEnabled;
    std::chrono::system_clock::time_point creationTime;
    std::string currentVersion;
    
    Impl() : currentEnvironment(Environment::Development), 
             historyEnabled(false), 
             changeTrackingEnabled(false),
             creationTime(std::chrono::system_clock::now()) {}
};

std::unique_ptr<ConfigurationManager> ConfigurationManager::create() {
    auto manager = std::unique_ptr<ConfigurationManager>(new ConfigurationManager());
    manager->pImpl->environmentConfigs[Environment::Development] = std::make_shared<Config>();
    return manager;
}

std::unique_ptr<ConfigurationManager> ConfigurationManager::create(Environment env) {
    auto manager = std::unique_ptr<ConfigurationManager>(new ConfigurationManager());
    manager->pImpl->environmentConfigs[env] = std::make_shared<Config>();
    manager->pImpl->currentEnvironment = env;
    return manager;
}

std::unique_ptr<ConfigurationManager> ConfigurationManager::create(const std::string& presetName) {
    auto manager = std::unique_ptr<ConfigurationManager>(new ConfigurationManager());
    manager->pImpl->environmentConfigs[Environment::Development] = std::make_shared<Config>();
    
    // Try to load preset
    if (!manager->loadPreset(presetName)) {
        // If preset not found, create default
        manager->pImpl->environmentConfigs[Environment::Development] = std::make_shared<Config>();
    }
    
    return manager;
}

ConfigurationManager::ConfigurationManager() : pImpl(std::make_unique<Impl>()) {}

ConfigurationManager::~ConfigurationManager() = default;

bool ConfigurationManager::loadFromFile(const std::string& filepath, Environment env) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string extension = std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return loadFromJSON(std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()));
    } else if (extension == ".yaml" || extension == ".yml") {
        return loadFromYAML(std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>()));
    } else {
        // Fallback to simple key=value format
        return pImpl->environmentConfigs[env]->loadFromFile(filepath);
    }
}

bool ConfigurationManager::loadFromDirectory(const std::string& directory, const std::string& extension) {
    if (!std::filesystem::exists(directory)) {
        return false;
    }
    
    bool success = true;
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_regular_file() && entry.path().extension() == extension) {
            Environment env = Environment::Development;
            std::string filename = entry.path().filename().string();
            
            // Infer environment from filename
            if (filename.find("dev") != std::string::npos) {
                env = Environment::Development;
            } else if (filename.find("test") != std::string::npos) {
                env = Environment::Test;
            } else if (filename.find("prod") != std::string::npos) {
                env = Environment::Production;
            }
            
            if (!loadFromFile(entry.path().string(), env)) {
                success = false;
            }
        }
    }
    
    return success;
}

bool ConfigurationManager::loadFromJSON(const std::string& jsonContent) {
    try {
        auto json = nlohmann::json::parse(jsonContent);
        
        for (auto& [key, value] : json.items()) {
            if (value.is_number_integer()) {
                pImpl->environmentConfigs[pImpl->currentEnvironment]->set(key, value.get<int64_t>());
            } else if (value.is_number()) {
                pImpl->environmentConfigs[pImpl->currentEnvironment]->set(key, value.get<double>());
            } else if (value.is_boolean()) {
                pImpl->environmentConfigs[pImpl->currentEnvironment]->set(key, value.get<bool>());
            } else if (value.is_string()) {
                pImpl->environmentConfigs[pImpl->currentEnvironment]->set(key, value.get<std::string>());
            } else if (value.is_array()) {
                std::vector<int> intArray;
                std::vector<double> doubleArray;
                std::vector<std::string> stringArray;
                
                for (const auto& item : value) {
                    if (item.is_number_integer()) {
                        intArray.push_back(item.get<int64_t>());
                    } else if (item.is_number()) {
                        doubleArray.push_back(item.get<double>());
                    } else if (item.is_string()) {
                        stringArray.push_back(item.get<std::string>());
                    }
                }
                
                if (!intArray.empty()) {
                    pImpl->environmentConfigs[pImpl->currentEnvironment]->set(key, intArray);
                } else if (!doubleArray.empty()) {
                    pImpl->environmentConfigs[pImpl->currentEnvironment]->set(key, doubleArray);
                } else if (!stringArray.empty()) {
                    pImpl->environmentConfigs[pImpl->currentEnvironment]->set(key, stringArray);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ConfigurationManager::loadFromYAML(const std::string& yamlContent) {
    try {
        auto yaml = YAML::Load(yamlContent);
        
        for (const auto& item : yaml) {
            std::string key = item.first.as<std::string>();
            YAML::Node value = item.second;
            
            if (value.IsScalar()) {
                if (value.IsInteger()) {
                    pImpl->environmentConfigs[pImpl->currentEnvironment]->set(key, value.as<int64_t>());
                } else if (value.IsReal()) {
                    pImpl->environmentConfigs[pImpl->currentEnvironment]->set(key, value.as<double>());
                } else if (value.IsBoolean()) {
                    pImpl->environmentConfigs[pImpl->currentEnvironment]->set(key, value.as<bool>());
                } else {
                    pImpl->environmentConfigs[pImpl->currentEnvironment]->set(key, value.as<std::string>());
                }
            } else if (value.IsSequence()) {
                std::vector<int> intArray;
                std::vector<double> doubleArray;
                std::vector<std::string> stringArray;
                
                for (const auto& item : value) {
                    if (item.IsInteger()) {
                        intArray.push_back(item.as<int64_t>());
                    } else if (item.IsReal()) {
                        doubleArray.push_back(item.as<double>());
                    } else if (item.IsScalar()) {
                        stringArray.push_back(item.as<std::string>());
                    }
                }
                
                if (!intArray.empty()) {
                    pImpl->environmentConfigs[pImpl->currentEnvironment]->set(key, intArray);
                } else if (!doubleArray.empty()) {
                    pImpl->environmentConfigs[pImpl->currentEnvironment]->set(key, doubleArray);
                } else if (!stringArray.empty()) {
                    pImpl->environmentConfigs[pImpl->currentEnvironment]->set(key, stringArray);
                }
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool ConfigurationManager::saveToFile(const std::string& filepath, Environment env) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string extension = std::filesystem::path(filepath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return saveToJSON(filepath);
    } else if (extension == ".yaml" || extension == ".yml") {
        return saveToYAML(filepath);
    } else {
        // Fallback to simple key=value format
        return pImpl->environmentConfigs[env]->saveToFile(filepath);
    }
}

bool ConfigurationManager::saveToJSON(const std::string& filepath) const {
    nlohmann::json json;
    
    for (const auto& [env, config] : pImpl->environmentConfigs) {
        if (config && config->getKeys().size() > 0) {
            std::string envKey = "environment_" + std::to_string(static_cast<int>(env));
            
            for (const auto& key : config->getKeys()) {
                auto value = config->get<ConfigValue>(key);
                if (value.has_value()) {
                    json[envKey][key] = value.value();
                }
            }
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << std::setw(2) << json << std::endl;
    return true;
}

bool ConfigurationManager::saveToYAML(const std::string& filepath) const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& [env, config] : pImpl->environmentConfigs) {
        if (config && config->getKeys().size() > 0) {
            emitter << YAML::Key << "environment_" + std::to_string(static_cast<int>(env));
            emitter << YAML::Value << YAML::BeginMap;
            
            for (const auto& key : config->getKeys()) {
                auto value = config->get<ConfigValue>(key);
                if (value.has_value()) {
                    emitter << YAML::Key << key;
                    
                    if (std::holds_alternative<int64_t>(value.value())) {
                        emitter << YAML::Value << std::get<int64_t>(value.value());
                    } else if (std::holds_alternative<double>(value.value())) {
                        emitter << YAML::Value << std::get<double>(value.value());
                    } else if (std::holds_alternative<bool>(value.value())) {
                        emitter << YAML::Value << std::get<bool>(value.value());
                    } else if (std::holds_alternative<std::string>(value.value())) {
                        emitter << YAML::Value << std::get<std::string>(value.value());
                    } else if (std::holds_alternative<std::vector<int>>(value.value())) {
                        emitter << YAML::Value << std::get<std::vector<int>>(value.value());
                    } else if (std::holds_alternative<std::vector<double>>(value.value())) {
                        emitter << YAML::Value << std::get<std::vector<double>>(value.value());
                    } else if (std::holds_alternative<std::vector<std::string>>(value.value())) {
                        emitter << YAML::Value << std::get<std::vector<std::string>>(value.value());
                    }
                }
            }
            
            emitter << YAML::EndMap;
        }
    }
    
    emitter << YAML::EndMap;
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << emitter.c_str();
    return true;
}

std::string ConfigurationManager::toJSON() const {
    nlohmann::json json;
    
    for (const auto& [env, config] : pImpl->environmentConfigs) {
        if (config && config->getKeys().size() > 0) {
            std::string envKey = "environment_" + std::to_string(static_cast<int>(env));
            
            for (const auto& key : config->getKeys()) {
                auto value = config->get<ConfigValue>(key);
                if (value.has_value()) {
                    json[envKey][key] = value.value();
                }
            }
        }
    }
    
    return json.dump(2);
}
}

std::string ConfigurationManager::toYAML() const {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& [env, config] : pImpl->environmentConfigs) {
        if (config && config->getKeys().size() > 0) {
            emitter << YAML::Key << "environment_" + std::to_string(static_cast<int>(env));
            emitter << YAML::Value << YAML::BeginMap;
            
            for (const auto& key : config->getKeys()) {
                auto value = config->get<ConfigValue>(key);
                if (value.has_value()) {
                    emitter << YAML::Key << key;
                    
                    if (std::holds_alternative<int64_t>(value.value())) {
                        emitter << YAML::Value << std::get<int64_t>(value.value());
                    } else if (std::holds_alternative<double>(value.value())) {
                        emitter << YAML::Value << std::get<double>(value.value());
                    } else if (std::holds_alternative<bool>(value.value())) {
                        emitter << YAML::Value << std::get<bool>(value.value());
                    } else if (std::holds_alternative<std::string>(value.value())) {
                        emitter << YAML::Value << std::get<std::string>(value.value());
                    } else if (std::holds_alternative<std::vector<int>>(value.value())) {
                        emitter << YAML::Value << std::get<std::vector<int>>(value.value());
                    } else if (std::holds_alternative<std::vector<double>>(value.value())) {
                        emitter << YAML::Value << std::get<std::vector<double>>(value.value());
                    } else if (std::holds_alternative<std::vector<std::string>>(value.value())) {
                        emitter << YAML::Value << std::get<std::vector<std::string>>(value.value());
                    }
                }
            }
            
            emitter << YAML::EndMap;
        }
    }
    
    emitter << YAML::EndMap;
    return emitter.c_str();
}

void ConfigurationManager::clear(Environment env) {
    if (pImpl->environmentConfigs.find(env) != pImpl->environmentConfigs.end()) {
        pImpl->environmentConfigs[env]->clear();
    }
}

void ConfigurationManager::merge(const ConfigurationManager& other, Environment env) {
    if (pImpl->environmentConfigs.find(env) != pImpl->environmentConfigs.end() &&
        other.pImpl->environmentConfigs.find(env) != other.pImpl->environmentConfigs.end()) {
        
        auto sourceConfig = other.pImpl->environmentConfigs[env];
        for (const auto& key : sourceConfig->getKeys()) {
            auto value = sourceConfig->get<ConfigValue>(key);
            if (value.has_value()) {
                // Only set if not already present in target config
                if (!pImpl->environmentConfigs[env]->has(key)) {
                    pImpl->environmentConfigs[env]->set(key, value.value(), ConfigSource::File);
                }
            }
        }
    }
}

void ConfigurationManager::override(const ConfigurationManager& other, Environment env) {
    if (pImpl->environmentConfigs.find(env) != pImpl->environmentConfigs.end() &&
        other.pImpl->environmentConfigs.find(env) != other.pImpl->environmentConfigs.end()) {
        
        auto sourceConfig = other.pImpl->environmentConfigs[env];
        for (const auto& key : sourceConfig->getKeys()) {
            auto value = sourceConfig->get<ConfigValue>(key);
            if (value.has_value()) {
                pImpl->environmentConfigs[env]->set(key, value.value(), ConfigSource::File);
            }
        }
    }
}

void ConfigurationManager::setEnvironment(Environment env) {
    pImpl->currentEnvironment = env;
}

Environment ConfigurationManager::getEnvironment() const {
    return pImpl->currentEnvironment;
}

bool ConfigurationManager::isEnvironmentCompatible(const ConfigurationSchema& schema) const {
    // Basic compatibility check - can be extended
    return true;
}

bool ConfigurationManager::savePreset(const std::string& name, PresetType type) {
    // Create a copy of current configuration as preset
    auto preset = std::make_unique<ConfigurationManager>();
    preset->pImpl->environmentConfigs = this->pImpl->environmentConfigs;
    preset->pImpl->currentEnvironment = this->pImpl->currentEnvironment;
    
    pImpl->presets[type] = std::move(*preset);
    return true;
}

bool ConfigurationManager::loadPreset(const std::string& name) {
    // Find preset by name
    for (auto& [type, preset] : pImpl->presets) {
        if (name == preset.getCurrentVersion() || 
            name == "preset_" + std::to_string(static_cast<int>(type))) {
            
            clear();
            preset.getEnvironment();
            return true;
        }
    }
    
    return false;
}

bool ConfigurationManager::deletePreset(const std::string& name) {
    for (auto it = pImpl->presets.begin(); it != pImpl->presets.end(); ++it) {
        if (name == it->second.getCurrentVersion() || 
            name == "preset_" + std::to_string(static_cast<int>(it->first))) {
            
            pImpl->presets.erase(it);
            return true;
        }
    }
    
    return false;
}

std::vector<std::string> ConfigurationManager::getPresetNames() const {
    std::vector<std::string> names;
    for (const auto& [type, preset] : pImpl->presets) {
        names.push_back(preset.getCurrentVersion());
        names.push_back("preset_" + std::to_string(static_cast<int>(type)));
    }
    return names;
}

bool ConfigurationManager::applyPreset(const std::string& name, bool merge) {
    if (merge) {
        return loadPreset(name);
    } else {
        clear();
        return loadPreset(name);
    }
}

bool ConfigurationManager::validate() const {
    std::vector<std::string> errors;
    
    for (const auto& [env, config] : pImpl->environmentConfigs) {
        if (!config) continue;
        
        for (const auto& key : config->getKeys()) {
            auto schemaIt = pImpl->schemas.find(key);
            if (schemaIt != pImpl->schemas.end()) {
                auto value = config->get<ConfigValue>(key);
                if (value.has_value()) {
                    // Validate against schema
                    if (!validateValue(value.value(), schemaIt->second)) {
                        errors.push_back("Invalid value for key '" + key + "' in environment " + std::to_string(static_cast<int>(env)));
                    }
                } else if (schemaIt->second.required) {
                    errors.push_back("Missing required key '" + key + "' in environment " + std::to_string(static_cast<int>(env)));
                }
            }
        }
    }
    
    if (!errors.empty()) {
        // Store errors for later retrieval
        return false;
    }
    
    return true;
}

bool ConfigurationManager::validateEnvironment(Environment env) const {
    std::vector<std::string> errors;
    
    auto configIt = pImpl->environmentConfigs.find(env);
    if (configIt == pImpl->environmentConfigs.end() || !configIt->second) {
        return false;
    }
    
    auto config = configIt->second;
    for (const auto& key : config->getKeys()) {
        auto schemaIt = pImpl->schemas.find(key);
        if (schemaIt != pImpl->schemas.end()) {
            auto value = config->get<ConfigValue>(key);
            if (value.has_value()) {
                // Validate against schema
                if (!validateValue(value.value(), schemaIt->second)) {
                    errors.push_back("Invalid value for key '" + key + "' in environment " + std::to_string(static_cast<int>(env)));
                }
            } else if (schemaIt->second.required) {
                errors.push_back("Missing required key '" + key + "' in environment " + std::to_string(static_cast<int>(env)));
            }
        }
    }
    
    if (!errors.empty()) {
        // Store errors for later retrieval
        return false;
    }
    
    return true;
}

std::vector<std::string> ConfigurationManager::getValidationErrors() const {
    // This would store and return accumulated errors
    // For simplicity, returning empty vector
    return std::vector<std::string>();
}

bool ConfigurationManager::addSchema(const ConfigurationSchema& schema) {
    pImpl->schemas[schema.key] = schema;
    return true;
}

bool ConfigurationManager::removeSchema(const std::string& key) {
    auto it = pImpl->schemas.find(key);
    if (it != pImpl->schemas.end()) {
        pImpl->schemas.erase(it);
        return true;
    }
    return false;
}

std::vector<ConfigurationSchema> ConfigurationManager::getSchemas() const {
    std::vector<ConfigurationSchema> schemas;
    schemas.reserve(pImpl->schemas.size());
    for (const auto& pair : pImpl->schemas) {
        schemas.push_back(pair.second);
    }
    return schemas;
}

bool ConfigurationManager::createTemplate(const std::string& name, const std::vector<std::string>& keys) {
    auto templatePtr = std::make_unique<ConfigurationTemplate>(name);
    for (const auto& key : keys) {
        // Find schema for this key to get type and description
        auto schemaIt = pImpl->schemas.find(key);
        if (schemaIt != pImpl->schemas.end()) {
            templatePtr->addKey(key, schemaIt->second.type, schemaIt->second.description);
        } else {
            templatePtr->addKey(key, "string", "");
        }
    }
    
    pImpl->templates[name] = std::move(templatePtr);
    return true;
}

std::unique_ptr<ConfigurationTemplate> ConfigurationManager::getTemplate(const std::string& name) const {
    auto it = pImpl->templates.find(name);
    if (it != pImpl->templates.end()) {
        return std::make_unique<ConfigurationTemplate>(*it->second);
    }
    return nullptr;
}

bool ConfigurationManager::generateFromTemplate(const std::string& templateName, Environment env) {
    auto templatePtr = getTemplate(templateName);
    if (!templatePtr) {
        return false;
    }
    
    // Clear existing config for this environment
    clear(env);
    
    // Generate from template
    for (const auto& key : templatePtr->getKeys()) {
        auto defaultValue = templatePtr->getDefaultValue(key);
        if (defaultValue.has_value()) {
            // Convert default value to appropriate type and set
            setConfigValue(env, key, defaultValue.value());
        }
    }
    
    return true;
}

bool ConfigurationManager::enableHistory(bool enable) {
    pImpl->historyEnabled = enable;
    return true;
}

bool ConfigurationManager::trackChanges(bool track) {
    pImpl->changeTrackingEnabled = track;
    return true;
}

std::vector<ConfigurationChange> ConfigurationManager::getChanges() const {
    return pImpl->changeHistory;
}

bool ConfigurationManager::rollbackToChange(size_t index) {
    if (index >= pImpl->changeHistory.size()) {
        return false;
    }
    
    const auto& change = pImpl->changeHistory[index];
    
    if (pImpl->environmentConfigs.find(pImpl->currentEnvironment) != pImpl->environmentConfigs.end()) {
        if (change.oldValue.has_value()) {
            setConfigValue(pImpl->currentEnvironment, change.key, change.oldValue.value());
        } else {
            pImpl->environmentConfigs[pImpl->currentEnvironment]->remove(change.key);
        }
    }
    
    return true;
}

std::string ConfigurationManager::getCurrentVersion() const {
    return pImpl->currentVersion;
}

bool ConfigurationManager::saveVersion(const std::string& versionName) {
    pImpl->currentVersion = versionName;
    
    // Create preset for this version
    savePreset(versionName, PresetType::Custom);
    
    return true;
}

OptimizationMetrics ConfigurationManager::optimize() const {
    OptimizationMetrics metrics;
    
    for (const auto& [env, config] : pImpl->environmentConfigs) {
        if (!config) continue;
        
        // Calculate memory usage
        metrics.memoryUsage += static_cast<double>(config->getKeys().size() * sizeof(ConfigEntry));
        
        // Calculate update time (simplified)
        metrics.updateTime += 0.001 * config->getKeys().size();
        
        // Calculate validation time (simplified)
        metrics.validationTime += 0.01 * config->getKeys().size();
    }
    
    // Generate warnings if needed
    if (metrics.memoryUsage > 1024 * 1024) { // 1MB
        metrics.warnings.push_back("High memory usage detected");
    }
    
    if (metrics.updateTime > 0.1) {
        metrics.warnings.push_back("Slow update time detected");
    }
    
    if (metrics.validationTime > 1.0) {
        metrics.warnings.push_back("Slow validation time detected");
    }
    
    return metrics;
}

bool ConfigurationManager::compress() const {
    // Simple compression implementation
    // In a real implementation, this would be more sophisticated
    return true;
}

bool ConfigurationManager::validateRelationships() const {
    for (const auto& [key, schema] : pImpl->schemas) {
        for (const auto& [depKey, constraint] : schema.dependencies) {
            // Check if dependency exists
            bool depExists = false;
            for (const auto& [env, config] : pImpl->environmentConfigs) {
                if (config && config->has(depKey)) {
                    depExists = true;
                    break;
                }
            }
            
            if (!depExists) {
                return false;
            }
        }
    }
    
    return true;
}

std::string ConfigurationManager::diff(const ConfigurationManager& other) const {
    std::stringstream diffStream;
    
    // Compare environments
    for (const auto& [env, config] : pImpl->environmentConfigs) {
        diffStream << "Environment: " << static_cast<int>(env) << "\n";
        
        auto otherConfigIt = other.pImpl->environmentConfigs.find(env);
        if (otherConfigIt != other.pImpl->environmentConfigs.end() && otherConfigIt->second) {
            
            // Get all keys from both configs
            std::vector<std::string> allKeys;
            for (const auto& key : config->getKeys()) {
                if (std::find(allKeys.begin(), allKeys.end(), key) == allKeys.end()) {
                    allKeys.push_back(key);
                }
            }
            for (const auto& key : otherConfigIt->second->getKeys()) {
                if (std::find(allKeys.begin(), allKeys.end(), key) == allKeys.end()) {
                    allKeys.push_back(key);
                }
            }
            
            for (const auto& key : allKeys) {
                bool hasInCurrent = config->has(key);
                bool hasInOther = otherConfigIt->second->has(key);
                
                if (hasInCurrent && hasInOther) {
                    auto currentValue = config->get<ConfigValue>(key);
                    auto otherValue = otherConfigIt->second->get<ConfigValue>(key);
                    
                    if (currentValue != otherValue) {
                        diffStream << "  Modified: " << key << "\n";
                        diffStream << "    From: ";
                        printConfigValue(diffStream, otherValue.value());
                        diffStream << "\n    To: ";
                        printConfigValue(diffStream, currentValue.value());
                        diffStream << "\n";
                    }
                } else if (hasInCurrent) {
                    diffStream << "  Added: " << key << "\n";
                    diffStream << "    Value: ";
                    printConfigValue(diffStream, config->get<ConfigValue>(key).value());
                    diffStream << "\n";
                } else if (hasInOther) {
                    diffStream << "  Removed: " << key << "\n";
                    diffStream << "    Was: ";
                    printConfigValue(diffStream, otherConfigIt->second->get<ConfigValue>(key).value());
                    diffStream << "\n";
                }
            }
        }
        
        diffStream << "\n";
    }
    
    return diffStream.str();
}

bool ConfigurationManager::compare(const ConfigurationManager& other) const {
    // Compare all environments
    if (pImpl->environmentConfigs.size() != other.pImpl->environmentConfigs.size()) {
        return false;
    }
    
    for (const auto& [env, config] : pImpl->environmentConfigs) {
        auto otherConfigIt = other.pImpl->environmentConfigs.find(env);
        if (otherConfigIt == other.pImpl->environmentConfigs.end()) {
            return false;
        }
        
        if (!config || !otherConfigIt->second) {
            return false;
        }
        
        // Compare keys
        std::vector<std::string> currentKeys = config->getKeys();
        std::vector<std::string> otherKeys = otherConfigIt->second->getKeys();
        
        if (currentKeys.size() != otherKeys.size()) {
            return false;
        }
        
        for (const auto& key : currentKeys) {
            auto currentValue = config->get<ConfigValue>(key);
            auto otherValue = otherConfigIt->second->get<ConfigValue>(key);
            
            if (currentValue != otherValue) {
                return false;
            }
        }
    }
    
    return true;
}

std::vector<std::string> ConfigurationManager::getAddedKeys(const ConfigurationManager& other) const {
    std::vector<std::string> addedKeys;
    
    for (const auto& [env, config] : pImpl->environmentConfigs) {
        auto otherConfigIt = other.pImpl->environmentConfigs.find(env);
        if (otherConfigIt != other.pImpl->environmentConfigs.end() && otherConfigIt->second) {
            for (const auto& key : otherConfigIt->second->getKeys()) {
                if (!config->has(key)) {
                    addedKeys.push_back(key);
                }
            }
        }
    }
    
    return addedKeys;
}

std::vector<std::string> ConfigurationManager::getRemovedKeys(const ConfigurationManager& other) const {
    std::vector<std::string> removedKeys;
    
    for (const auto& [env, config] : pImpl->environmentConfigs) {
        auto otherConfigIt = other.pImpl->environmentConfigs.find(env);
        if (otherConfigIt != other.pImpl->environmentConfigs.end() && otherConfigIt->second) {
            for (const auto& key : config->getKeys()) {
                if (!otherConfigIt->second->has(key)) {
                    removedKeys.push_back(key);
                }
            }
        }
    }
    
    return removedKeys;
}

std::map<std::string, std::pair<ConfigValue, ConfigValue>> ConfigurationManager::getModifiedKeys(const ConfigurationManager& other) const {
    std::map<std::string, std::pair<ConfigValue, ConfigValue>> modifiedKeys;
    
    for (const auto& [env, config] : pImpl->environmentConfigs) {
        auto otherConfigIt = other.pImpl->environmentConfigs.find(env);
        if (otherConfigIt != other.pImpl->environmentConfigs.end() && otherConfigIt->second) {
            
            for (const auto& key : config->getKeys()) {
                if (otherConfigIt->second->has(key)) {
                    auto currentValue = config->get<ConfigValue>(key);
                    auto otherValue = otherConfigIt->second->get<ConfigValue>(key);
                    
                    if (currentValue.has_value() && otherValue.has_value() && currentValue.value() != otherValue.value()) {
                        modifiedKeys[key] = std::make_pair(otherValue.value(), currentValue.value());
                    }
                }
            }
        }
    }
    
    return modifiedKeys;
}

size_t ConfigurationManager::getEntryCount(Environment env) const {
    auto configIt = pImpl->environmentConfigs.find(env);
    if (configIt != pImpl->environmentConfigs.end() && configIt->second) {
        return configIt->second->getKeys().size();
    }
    return 0;
}

double ConfigurationManager::getMemoryUsage(Environment env) const {
    auto configIt = pImpl->environmentConfigs.find(env);
    if (configIt != pImpl->environmentConfigs.end() && configIt->second) {
        return static_cast<double>(configIt->second->getKeys().size() * sizeof(ConfigEntry));
    }
    return 0.0;
}

double ConfigurationManager::getLastUpdateTime(Environment env) const {
    auto configIt = pImpl->environmentConfigs.find(env);
    if (configIt != pImpl->environmentConfigs.end() && configIt->second) {
        // Simplified - in real implementation would track last update time
        return std::chrono::system_clock::now().time_since_epoch().count();
    }
    return 0.0;
}

std::chrono::system_clock::time_point ConfigurationManager::getCreationTime() const {
    return pImpl->creationTime;
}

bool ConfigurationManager::exportToFile(const std::string& filepath, const std::string& format) const {
    std::transform(format.begin(), format.end(), format.begin(), ::tolower);
    
    if (format == "json") {
        return saveToJSON(filepath);
    } else if (format == "yaml" || format == "yml") {
        return saveToYAML(filepath);
    } else {
        // Default to simple format
        return pImpl->environmentConfigs[pImpl->currentEnvironment]->saveToFile(filepath);
    }
}

bool ConfigurationManager::importFromFile(const std::string& filepath, const std::string& format) {
    std::transform(format.begin(), format.end(), format.begin(), ::tolower);
    
    if (format == "json") {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return loadFromJSON(content);
    } else if (format == "yaml" || format == "yml") {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return loadFromYAML(content);
    } else {
        return loadFromFile(filepath);
    }
}

// Predefined preset creation methods
std::unique_ptr<ConfigurationManager> ConfigurationManager::createDevelopmentPreset() {
    auto manager = std::unique_ptr<ConfigurationManager>(new ConfigurationManager());
    manager->pImpl->environmentConfigs[Environment::Development] = std::make_shared<Config>();
    
    // Development-specific settings
    manager->pImpl->environmentConfigs[Environment::Development]->set("brain.neuronCount", 1000);
    manager->pImpl->environmentConfigs[Environment::Development]->set("brain.regionCount", 5);
    manager->pImpl->environmentConfigs[Environment::Development]->set("simulation.dt", 0.1);
    manager->pImpl->environmentConfigs[Environment::Development]->set("simulation.maxTime", 100.0);
    manager->pImpl->environmentConfigs[Environment::Development]->set("neuromodulation.enabled", true);
    manager->pImpl->environmentConfigs[Environment::Development]->set("environment.debug", true);
    manager->pImpl->environmentConfigs[Environment::Development]->set("world.width", 100.0f);
    manager->pImpl->environmentConfigs[Environment::Development]->set("world.height", 100.0f);
    
    manager->saveVersion("Development");
    
    return manager;
}

std::unique_ptr<ConfigurationManager> ConfigurationManager::createDebugPreset() {
    auto manager = std::unique_ptr<ConfigurationManager>(new ConfigurationManager());
    manager->pImpl->environmentConfigs[Environment::Development] = std::make_shared<Config>();
    
    // Debug-specific settings
    manager->pImpl->environmentConfigs[Environment::Development]->set("brain.neuronCount", 100);
    manager->pImpl->environmentConfigs[Environment::Development]->set("brain.regionCount", 2);
    manager->pImpl->environmentConfigs[Environment::Development]->set("simulation.dt", 0.01);
    manager->pImpl->environmentConfigs[Environment::Development]->set("simulation.maxTime", 10.0);
    manager->pImpl->environmentConfigs[Environment::Development]->set("neuromodulation.enabled", false);
    manager->pImpl->environmentConfigs[Environment::Development]->set("environment.debug", true);
    manager->pImpl->environmentConfigs[Environment::Development]->set("environment.logLevel", "debug");
    manager->pImpl->environmentConfigs[Environment::Development]->set("performance.profile", "debug");
    
    manager->saveVersion("Debug");
    
    return manager;
}

std::unique_ptr<ConfigurationManager> ConfigurationManager::createPerformancePreset() {
    auto manager = std::unique_ptr<ConfigurationManager>(new ConfigurationManager());
    manager->pImpl->environmentConfigs[Environment::Development] = std::make_shared<Config>();
    
    // Performance-specific settings
    manager->pImpl->environmentConfigs[Environment::Development]->set("brain.neuronCount", 5000);
    manager->pImpl->environmentConfigs[Environment::Development]->set("brain.regionCount", 10);
    manager->pImpl->environmentConfigs[Environment::Development]->set("simulation.dt", 0.5);
    manager->pImpl->environmentConfigs[Environment::Development]->set("simulation.maxTime", 1000.0);
    manager->pImpl->environmentConfigs[Environment::Development]->set("neuromodulation.enabled", true);
    manager->pImpl->environmentConfigs[Environment::Development]->set("environment.debug", false);
    manager->pImpl->environmentConfigs[Environment::Development]->set("environment.logLevel", "error");
    manager->pImpl->environmentConfigs[Environment::Development]->set("performance.profile", "optimized");
    manager->pImpl->environmentConfigs[Environment::Development]->set("performance.maxThreads", std::thread::hardware_concurrency());
    
    manager->saveVersion("Performance");
    
    return manager;
}

std::unique_ptr<ConfigurationManager> ConfigurationManager::createResearchPreset() {
    auto manager = std::unique_ptr<ConfigurationManager>(new ConfigurationManager());
    manager->pImpl->environmentConfigs[Environment::Development] = std::make_shared<Config>();
    
    // Research-specific settings
    manager->pImpl->environmentConfigs[Environment::Development]->set("brain.neuronCount", 10000);
    manager->pImpl->environmentConfigs[Environment::Development]->set("brain.regionCount", 15);
    manager->pImpl->environmentConfigs[Environment::Development]->set("simulation.dt", 0.05);
    manager->pImpl->environmentConfigs[Environment::Development]->set("simulation.maxTime", 5000.0);
    manager->pImpl->environmentConfigs[Environment::Development]->set("neuromodulation.enabled", true);
    manager->pImpl->environmentConfigs[Environment::Development]->set("environment.debug", false);
    manager->pImpl->environmentConfigs[Environment::Development]->set("environment.logLevel", "info");
    manager->pImpl->environmentConfigs[Environment::Development]->set("research.experiment.mode", "active");
    manager->pImpl->environmentConfigs[Environment::Development]->set("research.performanceRecording", true);
    
    manager->saveVersion("Research");
    
    return manager;
}

std::unique_ptr<ConfigurationManager> ConfigurationManager::createProductionPreset() {
    auto manager = std::unique_ptr<ConfigurationManager>(new ConfigurationManager());
    manager->pImpl->environmentConfigs[Environment::Development] = std::make_shared<Config>();
    
    // Production-specific settings
    manager->pImpl->environmentConfigs[Environment::Development]->set("brain.neuronCount", 5000);
    manager->pImpl->environmentConfigs[Environment::Development]->set("brain.regionCount", 10);
    manager->pImpl->environmentConfigs[Environment::Development]->set("simulation.dt", 0.2);
    manager->pImpl->environmentConfigs[Environment::Development]->set("simulation.maxTime", 3600.0);
    manager->pImpl->environmentConfigs[Environment::Development]->set("neuromodulation.enabled", true);
    manager->pImpl->environmentConfigs[Environment::Development]->set("environment.debug", false);
    manager->pImpl->environmentConfigs[Environment::Development]->set("environment.logLevel", "warning");
    manager->pImpl->environmentConfigs[Environment::Development]->set("performance.profile", "production");
    manager->pImpl->environmentConfigs[Environment::Development]->set("performance.maxMemory", 4096);
    
    manager->saveVersion("Production");
    
    return manager;
}

// Factory methods for specific use cases
std::unique_ptr<ConfigurationManager> ConfigurationManager::createBrainSimulationConfig() {
    auto manager = createDevelopmentPreset();
    
    // Add brain-specific configuration
    manager->pImpl->environmentConfigs[Environment::Development]->set("brain.synapticStrength", 0.5);
    manager->pImpl->environmentConfigs[Environment::Development]->set("brain.connectionProbability", 0.15);
    manager->pImpl->environmentConfigs[Environment::Development]->set("brain.excitationInhibitionRatio", 1.0f);
    manager->pImpl->environmentConfigs[Environment::Development]->set("brain.spikeThreshold", -55.0f);
    manager->pImpl->environmentConfigs[Environment::Development]->set("brain.resetPotential", -65.0f);
    manager->pImpl->environmentConfigs[Environment::Development]->set("brain.refractoryPeriod", 5.0f);
    
    return manager;
}

std::unique_ptr<ConfigurationManager> ConfigurationManager::createMemorySystemConfig() {
    auto manager = createDevelopmentPreset();
    
    // Add memory-specific configuration
    manager->pImpl->environmentConfigs[Environment::Development]->set("memory.working.capacity", 1000);
    manager->pImpl->environmentConfigs[Environment::Development]->set("memory.working.decayRate", 0.01);
    manager->pImpl->environmentConfigs[Environment::Development]->set("memory.episodic.capacity", 10000);
    manager->pImpl->environmentConfigs[Environment::Development]->set("memory.episodic.consolidationRate", 0.05);
    manager->pImpl->environmentConfigs[Environment::Development]->set("memory.episodic.recencyWeight", 0.7);
    
    return manager;
}

std::unique_ptr<ConfigurationManager> ConfigurationManager::createNeuromodulationConfig() {
    auto manager = createDevelopmentPreset();
    
    // Add neuromodulation-specific configuration
    manager->pImpl->environmentConfigs[Environment::Development]->set("neuromodulation.dopamine.level", 0.5);
    manager->pImpl->environmentConfigs[Environment::Development]->set("neuromodulation.dopamine.noise", 0.1);
    manager->pImpl->environmentConfigs[Environment::Development]->set("neuromodulation.reward.scale", 1.0);
    manager->pImpl->environmentConfigs[Environment::Development]->set("neuromodulation.error.threshold", 0.1);
    manager->pImpl->environmentConfigs[Environment::Development]->set("neuromodulation.curiosity.drive", 0.5);
    manager->pImpl->environmentConfigs[Environment::Development]->set("neuromodulation.novelty.threshold", 0.2);
    
    return manager;
}

std::unique_ptr<ConfigurationManager> ConfigurationManager::createEnvironmentConfig() {
    auto manager = createDevelopmentPreset();
    
    // Add environment-specific configuration
    manager->pImpl->environmentConfigs[Environment::Development]->set("world.width", 100.0f);
    manager->pImpl->environmentConfigs[Environment::Development]->set("world.height", 100.0f);
    manager->pImpl->environmentConfigs[Environment::Development]->set("world.cellSize", 1.0f);
    manager->pImpl->environmentConfigs[Environment::Development]->set("world.obstacleDensity", 0.1);
    manager->pImpl->environmentConfigs[Environment::Development]->set("world.rewardSpacing", 10.0f);
    manager->pImpl->environmentConfigs[Environment::Development]->set("world.punishmentSpacing", 5.0f);
    
    return manager;
}

// Get internal Config instance for compatibility
std::shared_ptr<Config> ConfigurationManager::getConfig(Environment env) const {
    auto configIt = pImpl->environmentConfigs.find(env);
    if (configIt != pImpl->environmentConfigs.end()) {
        return configIt->second;
    }
    return nullptr;
}

// Helper functions
bool ConfigurationManager::validateValue(const ConfigValue& value, const ConfigurationSchema& schema) const {
    if (schema.validator && !schema.validator(value)) {
        return false;
    }
    
    if (!schema.validationPattern.empty()) {
        std::string valueStr;
        if (std::holds_alternative<std::string>(value)) {
            valueStr = std::get<std::string>(value);
        } else {
            std::stringstream ss;
            ss << value;
            valueStr = ss.str();
        }
        
        // Simple regex validation (would use proper regex in real implementation)
        if (schema.validationPattern == "int" && !std::holds_alternative<int64_t>(value)) {
            return false;
        } else if (schema.validationPattern == "double" && !std::holds_alternative<double>(value)) {
            return false;
        } else if (schema.validationPattern == "string" && !std::holds_alternative<std::string>(value)) {
            return false;
        } else if (schema.validationPattern == "bool" && !std::holds_alternative<bool>(value)) {
            return false;
        }
    }
    
    if (!schema.allowedValues.empty()) {
        std::string valueStr;
        if (std::holds_alternative<std::string>(value)) {
            valueStr = std::get<std::string>(value);
        } else {
            std::stringstream ss;
            ss << value;
            valueStr = ss.str();
        }
        
        if (std::find(schema.allowedValues.begin(), schema.allowedValues.end(), valueStr) == schema.allowedValues.end()) {
            return false;
        }
    }
    
    if (valueStr.empty()) {
        return false;
    }
    
    return true;
}

void ConfigurationManager::printConfigValue(std::stringstream& ss, const ConfigValue& value) {
    if (std::holds_alternative<int64_t>(value)) {
        ss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        ss << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        ss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        ss << "\"" << std::get<std::string>(value) << "\"";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        ss << "[";
        const auto& intArray = std::get<std::vector<int>>(value);
        for (size_t i = 0; i < intArray.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << intArray[i];
        }
        ss << "]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        ss << "[";
        const auto& doubleArray = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < doubleArray.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << doubleArray[i];
        }
        ss << "]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        ss << "[";
        const auto& stringArray = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < stringArray.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << stringArray[i] << "\"";
        }
        ss << "]";
    }
}

void ConfigurationManager::setConfigValue(Environment env, const std::string& key, const ConfigValue& value) {
    if (pImpl->environmentConfigs.find(env) != pImpl->environmentConfigs.end()) {
        pImpl->environmentConfigs[env]->set(key, value, ConfigSource::Runtime);
    }
}
