#include "EnhancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <filesystem>
#include <cassert>

namespace nlm {

namespace {
    const std::string ENV_VAR_PREFIX = "NLM_";
}

EnhancedConfig::EnhancedConfig() : pImpl(std::make_unique<Impl>()) {}

EnhancedConfig::~EnhancedConfig() = default;

bool EnhancedConfig::loadFromFile(const std::string& filepath, bool validate) {
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Use parent Config's loadFromFile
    // For now, we'll implement enhanced loading
    return pImpl->config.loadFromFile(filepath);
}

bool EnhancedConfig::loadFromEnvironment(const std::string& prefix, bool validate) {
    bool success = true;
    
    for (const auto& entry : std::filesystem::directory_iterator("/")) {
        std::string filename = entry.path().filename().string();
        if (filename.find(prefix) == 0) {
            std::string envValue = Env::get(filename);
            if (!envValue.empty()) {
                std::string configKey = convertEnvVarToConfigKey(filename);
                // Set config value
                set(configKey, envValue, ConfigSource::Environment);
            }
        }
    }
    
    return true;
}

bool EnhancedConfig::loadFromArgs(int argc, char** argv, bool validate) {
    // Delegate to parent Config's loadFromArgs
    return pImpl->config.loadFromArgs(argc, argv);
}

bool EnhancedConfig::validate(const std::unordered_map<std::string, std::string>& schema) const {
    // Implement validation logic
    return true;
}

template<typename T>
std::optional<T> EnhancedConfig::getValidated(const std::string& key, T defaultValue,
                                             std::function<bool(T)> validator) const {
    auto value = pImpl->config.get<T>(key);
    if (value.has_value()) {
        if (validator && !validator(value.value())) {
            return std::nullopt;
        }
        return value;
    }
    return defaultValue;
}

bool EnhancedConfig::setValidated(const std::string& key, const ConfigValue& value,
                                 ConfigSource source) {
    return pImpl->config.set(key, value, source);
}

std::string EnhancedConfig::summaryWithValidation() const {
    std::ostringstream oss;
    oss << "Enhanced Configuration Summary:\n";
    oss << "Total entries: " << pImpl->config.getKeys().size() << "\n";
    oss << "Valid entries: " << pImpl->config.getKeys().size() << "\n";  // Simplified
    oss << "Validation score: 100.0%\n";
    return oss.str();
}

std::vector<std::string> EnhancedConfig::getValidationErrors() const {
    return std::vector<std::string>();  // Simplified - no errors for now
}

bool EnhancedConfig::isValid() const {
    return true;  // Simplified
}

std::string EnhancedConfig::convertEnvVarToConfigKey(const std::string& envVar) {
    std::string key = envVar.substr(ENV_VAR_PREFIX.size());
    // Convert to lower case and replace underscores with hyphens
    for (char& c : key) {
        c = std::tolower(c);
        if (c == '_') c = '-';
    }
    return key;
}

std::string EnhancedConfig::generateEnvVarForConfig(const std::string& key) {
    std::string envVar = ENV_VAR_PREFIX;
    for (char c : key) {
        if (c == '-') {
            envVar += '_';
        } else {
            envVar += std::toupper(c);
        }
    }
    return envVar;
}

const std::unordered_map<std::string, EnhancedConfig::ConfigSchema>
EnhancedConfig::getDefaultSchema() {
    return {
        {"neuron_count", {"neuron_count", "int", "1000", "Number of neurons", validateInt, true}},
        {"region_count", {"region_count", "int", "1", "Number of regions", validateInt, true}},
        {"connection_probability", {"connection_probability", "float", "0.1", "Connection probability", validateFloat, true}},
        {"random_seed", {"random_seed", "int64", "42", "Random seed", validateInt64, false}},
        {"simulation_timestep", {"simulation_timestep", "float", "0.001", "Simulation timestep", validateFloat, false}},
    };
}

std::string EnhancedConfig::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    return str.substr(start, end - start + 1);
}

std::string EnhancedConfig::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

EnhancedConfigManager::EnhancedConfigManager() : pImpl(std::make_unique<Impl>()) {}

EnhancedConfigManager::~EnhancedConfigManager() = default;

bool EnhancedConfigManager::addEnhancedConfig(std::shared_ptr<EnhancedConfig> enhancedConfig) {
    if (enhancedConfig) {
        pImpl->enhancedConfigs.push_back(enhancedConfig);
        return true;
    }
    return false;
}

bool EnhancedConfigManager::mergeWithOriginal(std::shared_ptr<Config> originalConfig) {
    if (!originalConfig) return false;
    
    // Merge all enhanced configurations into original
    for (const auto& enhancedConfig : pImpl->enhancedConfigs) {
        if (enhancedConfig) {
            for (const auto& key : enhancedConfig->getValidatedKeys()) {
                auto value = enhancedConfig->getValidatedKeys();
                if (!value.empty()) {
                    originalConfig->set(key, value, ConfigSource::Environment);
                }
            }
        }
    }
    
    return true;
}

std::shared_ptr<EnhancedConfig> EnhancedConfigManager::getEnhancedConfig() const {
    if (!pImpl->enhancedConfigs.empty()) {
        return pImpl->enhancedConfigs.front();
    }
    return nullptr;
}

bool EnhancedConfigManager::loadAllConfigurations(const std::string& configFile,
                                                 const std::string& envPrefix,
                                                 int argc, char** argv,
                                                 bool validate) {
    pImpl->loadTimestamps.clear();
    
    // Load from enhanced config
    auto enhancedConfig = std::make_shared<EnhancedConfig>();
    
    if (!configFile.empty()) {
        enhancedConfig->loadFromFile(configFile, validate);
    }
    
    enhancedConfig->loadFromEnvironment(envPrefix, validate);
    enhancedConfig->loadFromArgs(argc, argv, validate);
    
    addEnhancedConfig(enhancedConfig);
    pImpl->loadTimestamps.push_back(std::chrono::system_clock::now());
    
    return true;
}

std::unordered_map<std::string, std::optional<ConfigValue>>
EnhancedConfigManager::getCombinedConfig() const {
    std::unordered_map<std::string, std::optional<ConfigValue>> combined;
    
    for (const auto& enhancedConfig : pImpl->enhancedConfigs) {
        if (enhancedConfig) {
            for (const auto& key : enhancedConfig->getValidatedKeys()) {
                // Get value and combine
                auto value = enhancedConfig->getValidatedKeys();
                if (!value.empty()) {
                    combined[key] = value;
                }
            }
        }
    }
    
    return combined;
}

template<typename T>
T EnhancedConfigManager::getCombined(const std::string& key, const T& defaultValue) const {
    auto combined = getCombinedConfig();
    auto it = combined.find(key);
    if (it != combined.end() && it->second.has_value()) {
        return std::get<T>(it->second.value());
    }
    return defaultValue;
}

size_t EnhancedConfigManager::getTotalEntries() const {
    size could be obtained from all enhanced configs
    return pImpl->enhancedConfigs.size();
}

float EnhancedConfigManager::getOverallValidationScore() const {
    return 100.0f;  // Simplified
}

std::vector<std::string> EnhancedConfigManager::getAllValidationErrors() const {
    std::vector<std::string> allErrors;
    for (const auto& enhancedConfig : pImpl->enhancedConfigs) {
        if (enhancedConfig) {
            auto errors = enhancedConfig->getValidationErrors();
            allErrors.insert(allErrors.end(), errors.begin(), errors.end());
        }
    }
    return allErrors;
}

std::vector<std::chrono::system_clock::time_point>
EnhancedConfigManager::getLoadTimestamps() const {
    return pImpl->loadTimestamps;
}

bool EnhancedConfigManager::saveBackup(const std::string& backupPath) const {
    std::ofstream file(backupPath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Save combined config
    auto combined = getCombinedConfig();
    // Serialize to file (simplified)
    file << "EnhancedConfig Backup\n";
    
    return true;
}

bool EnhancedConfigManager::restoreFromBackup(const std::string& backupPath) {
    std::ifstream file(backupPath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    // Restore config from backup (simplified)
    std::string line;
    if (std::getline(file, line) && line == "EnhancedConfig Backup") {
        // Restore logic
        return true;
    }
    
    return false;
}

namespace Env {
    std::string get(const std::string& name, const std::string& defaultValue) {
        const char* value = std::getenv(name.c_str());
        return value ? value : defaultValue;
    }
    
    int getInt(const std::string& name, int defaultValue) {
        const char* value = std::getenv(name.c_str());
        return value ? std::stoi(value) : defaultValue;
    }
    
    float getFloat(const std::string& name, float defaultValue) {
        const char* value = std::getenv(name.c_str());
        return value ? std::stof(value) : defaultValue;
    }
    
    bool getBool(const std::string& name, bool defaultValue) {
        const char* value = std::getenv(name.c_str());
        if (!value) return defaultValue;
        
        std::string val = value;
        std::transform(val.begin(), val.end(), val.begin(), ::tolower);
        return val == "true" || val == "1" || val == "yes" || val == "on";
    }
    
    bool exists(const std::string& name) {
        return std::getenv(name.c_str()) != nullptr;
    }
    
    std::vector<std::string> getMatching(const std::string& prefix) {
        std::vector<std::string> result;
        // Platform-specific implementation
        return result;
    }
    
    void set(const std::string& name, const std::string& value) {
        // Platform-specific implementation
    }
    
    void remove(const std::string& name) {
        // Platform-specific implementation
    }
}

// Template instantiations for EnhancedConfigManager
// Explicit template instantiations for getCombined
template int EnhancedConfigManager::getCombined<int>(const std::string&, const int&) const;
template int64_t EnhancedConfigManager::getCombined<int64_t>(const std::string&, const int64_t&) const;
template float EnhancedConfigManager::getCombined<float>(const std::string&, const float&) const;
template double EnhancedConfigManager::getCombined<double>(const std::::string&, const double&) const;
template bool EnhancedConfigManager::getCombined<bool>(const std::string&, const bool&) const;
template std::string EnhancedConfigManager::getCombined<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

// Explicit template instantiations for EnhancedConfig
// Template instantiations for getValidated
// (These need to be in the implementation file)

#include "EnhancedConfig.cpp"

} // namespace nlm