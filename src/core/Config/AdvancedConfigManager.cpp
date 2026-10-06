// Advanced Configuration Manager for NLM System
// 
// This file implements the AdvancedConfigManager class that provides:
// 1. Configuration Profiles: Create, manage, and switch between different configuration profiles
// 2. Scenario-Based Configuration: Apply predefined scenarios (quick_test, long_term_learning, real_time_control)
// 3. Configuration Validation: Validate configurations against constraints and best practices
// 4. Configuration Comparison: Compare different configurations and analyze differences
// 5. Configuration Optimization: Automatically optimize configurations for performance
// 6. Export/Import: Export configurations to JSON and import from files

// Implementation follows the existing Config system patterns and integrates seamlessly
// with the NLM core, brain, and Python bindings.

#include "AdvancedConfigManager.hpp"
#include "src/core/Config/Config.hpp"
#include <nlohmann/json.hpp>
#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace nlm {

// Helper function to convert ConfigValue to JSON
static nlohmann::json configValueToJson(const ConfigValue& value) {
    nlohmann::json result;
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, int>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = arg;
        }
    }, value);
    return result;
}

// Helper function to convert JSON to ConfigValue
static ConfigValue jsonToConfigValue(const nlohmann::json& jsonValue) {
    if (jsonValue.is_string()) {
        return jsonValue.get<std::string>();
    } else if (jsonValue.is_number_integer()) {
        return jsonValue.get<int>();
    } else if (jsonValue.is_number_unsigned()) {
        return jsonValue.get<int64_t>();
    } else if (jsonValue.is_number_float()) {
        return jsonValue.get<double>();
    } else if (jsonValue.is_boolean()) {
        return jsonValue.get<bool>();
    } else if (jsonValue.is_array()) {
        if (!jsonValue.empty()) {
            if (jsonValue[0].is_number_integer()) {
                return jsonValue.get<std::vector<int>>();
            } else if (jsonValue[0].is_number_float()) {
                return jsonValue.get<std::vector<double>>();
            } else if (jsonValue[0].is_string()) {
                return jsonValue.get<std::vector<std::string>>();
            }
        }
        return std::vector<int>();
    }
    return std::string();
}

AdvancedConfigManager::AdvancedConfigManager(std::shared_ptr<Config> config)
    : config_(std::move(config)), currentProfile_("default"), nextProfileId_(0), nextScenarioId_(0), nextValidationId_(0), nextOptimizationId_(0) {
    if (!config_) {
        config_ = std::make_shared<Config>();
    }
}

AdvancedConfigManager::AdvancedConfigManager() : AdvancedConfigManager(nullptr) {}

bool AdvancedConfigManager::createProfile(const std::string& name, const std::string& description,
                                        const std::unordered_map<std::string, ConfigValue>& settings) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (profiles_.find(name) != profiles_.end()) {
        return false;
    }
    
    ConfigProfile profile;
    profile.id = ++nextProfileId_;
    profile.name = name;
    profile.description = description;
    profile.createdAt = std::chrono::system_clock::now();
    profile.lastModified = profile.createdAt;
    profile.isActive = false;
    profile.settings = settings;
    
    return true;
}

bool AdvancedConfigManager::updateProfile(const std::string& name, const std::string& description,
                                        const std::unordered_map<std::string, ConfigValue>& settings) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = profiles_.find(name);
    if (it == profiles_.end()) {
        return false;
    }
    
    ConfigProfile& profile = it->second;
    profile.description = description;
    profile.settings = settings;
    profile.lastModified = std::chrono::system_clock::now();
    
    if (profile.isActive) {
        applyProfile(name);
    }
    
    return true;
}

bool AdvancedConfigManager::deleteProfile(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = profiles_.find(name);
    if (it == profiles_.end()) {
        return false;
    }
    
    if (it->second.isActive) {
        return false;
    }
    
    profiles_.erase(it);
    return true;
}

bool AdvancedConfigManager::switchToProfile(const std::string& name) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = profiles_.find(name);
    if (it == profiles_.end()) {
        return false;
    }
    
    if (currentProfile_ != "default" && profiles_.find(currentProfile_) != profiles_.end()) {
        profiles_[currentProfile_].isActive = false;
    }
    
    it->second.isActive = true;
    currentProfile_ = name;
    
    return applyProfile(name);
}

AdvancedConfigManager::ConfigProfile AdvancedConfigManager::getProfile(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = profiles_.find(name);
    if (it == profiles_.end()) {
        return ConfigProfile();
    }
    
    return it->second;
}

std::vector<std::string> AdvancedConfigManager::getProfileNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> names;
    names.reserve(profiles_.size());
    for (const auto& pair : profiles_) {
        names.push_back(pair.first);
    }
    return names;
}

bool AdvancedConfigManager::applyScenario(const std::string& scenarioName,
                                         const std::unordered_map<std::string, ConfigValue>& overrideSettings) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = scenarios_.find(scenarioName);
    if (it == scenarios_.end()) {
        return false;
    }
    
    const auto& scenario = it->second;
    
    config_->clear();
    
    for (const auto& setting : scenario.settings) {
        config_->set(setting.first, setting.second, ConfigSource::Runtime);
    }
    
    for (const auto& setting : overrideSettings) {
        config_->set(setting.first, setting.second, ConfigSource::Runtime);
    }
    
    std::string profileName = "scenario_" + scenarioName;
    if (profiles_.find(profileName) == profiles_.end()) {
        createProfile(profileName, scenario.description + " (auto-generated)", scenario.settings);
    }
    
    return switchToProfile(profileName);
}

bool AdvancedConfigManager::registerScenario(const std::string& name, const std::string& description,
                                            const std::unordered_map<std::string, ConfigValue>& settings,
                                            const std::string& category) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    Scenario scenario;
    scenario.id = ++nextScenarioId_;
    scenario.name = name;
    scenario.description = description;
    scenario.category = category;
    scenario.createdAt = std::chrono::system_clock::now();
    scenario.settings = settings;
    
    scenarios_[name] = scenario;
    return true;
}

AdvancedConfigManager::Scenario AdvancedConfigManager::getScenario(const std::string& name) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = scenarios_.find(name);
    if (it == scenarios_.end()) {
        return Scenario();
    }
    
    return it->second;
}

std::vector<std::string> AdvancedConfigManager::getScenarioNames() const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> names;
    names.reserve(scenarios_.size());
    for (const auto& pair : scenarios_) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<std::string> AdvancedConfigManager::getScenarioNamesByCategory(const std::string& category) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::vector<std::string> names;
    for (const auto& pair : scenarios_) {
        if (pair.second.category == category) {
            names.push_back(pair.first);
        }
    }
    return names;
}

ValidationResult AdvancedConfigManager::validateProfile(const std::string& profileName) const {
    std::lock_guard<std::mutex> lock(mutex_);
    
    ValidationResult result;
    result.id = ++nextValidationId_;
    result.profileName = profileName;
    result.timestamp = std::chrono::system_clock::now();
    result.isValid = true;
    
    auto it = profiles_.find(profileName);
    if (it == profiles_.end()) {
        result.isValid = false;
        result.errors.push_back("Profile does not exist");
        return result;
    }
    
    const auto& profile = it->second;
    
    if (!config_->has("neuron_count")) {
        result.warnings.push_back("Missing optional key: neuron_count");
    }
    
    auto validateIntRange = [&result](const std::string& key, int min, int max) {
        auto val = config_->get<int>(key);
        if (val.has_value()) {
            if (val.value() < min || val.value() > max) {
                result.errors.push_back("Invalid range for " + key + ": " + 
                                      std::to_string(val.value()) + " (expected " +
                                      std::to_string(min) + "-" + std::to_string(max) + ")");
                result.isValid = false;
            }
        }
    };
    
    auto validateFloatRange = [&result](const std::string& key, double min, double max) {
        auto val = config_->get<double>(key);
        if (val.has_value()) {
            if (val.value() < min || val.value() > max) {
                result.errors.push_back("Invalid range for " + key + ": " + 
                                      std::to_string(val.value()) + " (expected " +
                                      std::to_string(min) + "-" + std::to_string(max) + ")");
                result.isValid = false;
            }
        }
    };
    
    validateIntRange("neuron_count", 1, 10000);
    validateFloatRange("simulation_timestep", 0.0001, 0.1);
    validateFloatRange("connection_probability", 0.0, 1.0);
    validateFloatRange("plasticity_learning_rate", 0.0, 1.0);
    validateFloatRange("stdp_ltp_weight", 0.0, 1.0);
    validateFloatRange("stdp_ltd_weight", 0.0, 1.0);
    
    return result;
}

ValidationResult AdvancedConfigManager::validateConfiguration() const {
    ValidationResult result;
    result.id = ++nextValidationId_;
    result.profileName = currentProfile_;
    result.timestamp = std::chrono::system_clock::now();
    result.isValid = true;
    
    result = validateProfile(currentProfile_);
    
    return result;
}

DifferenceReport AdvancedConfigManager::compareConfigurations(const std::string& profile1, const std::string& profile2) const {
    DifferenceReport report;
    report.profile1 = profile1;
    report.profile2 = profile2;
    report.timestamp = std::chrono::system_clock::now();
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it1 = profiles_.find(profile1);
    auto it2 = profiles_.find(profile2);
    
    if (it1 == profiles_.end() || it2 == profiles_.end()) {
        report.errors.push_back("One or both profiles do not exist");
        return report;
    }
    
    const auto& settings1 = it1->second.settings;
    const auto& settings2 = it2->second.settings;
    
    std::unordered_set<std::string> allKeys;
    for (const auto& setting : settings1) {
        allKeys.insert(setting.first);
    }
    for (const auto& setting : settings2) {
        allKeys.insert(setting.first);
    }
    
    for (const auto& key : allKeys) {
        auto it1_it = settings1.find(key);
        auto it2_it = settings2.find(key);
        
        std::string value1, value2;
        bool has1 = it1_it != settings1.end();
        bool has2 = it2_it != settings2.end();
        
        if (has1) {
            value1 = configValueToString(it1_it->second);
        }
        if (has2) {
            value2 = configValueToString(it2_it->second);
        }
        
        if (has1 && has2 && value1 == value2) {
            report.similarKeys.push_back(key);
        } else if (has1 && has2) {
            Difference diff;
            diff.key = key;
            diff.value1 = value1;
            diff.value2 = value2;
            diff.changeType = determineChangeType(it1_it->second, it2_it->second);
            report.differences.push_back(diff);
        } else if (has1) {
            Difference diff;
            diff.key = key;
            diff.value1 = value1;
            diff.value2 = "(absent)";
            diff.changeType = ChangeType::Removed;
            report.differences.push_back(diff);
        } else {
            Difference diff;
            diff.key = key;
            diff.value1 = "(absent)";
            diff.value2 = value2;
            diff.changeType = ChangeType::Added;
            report.differences.push_back(diff);
        }
    }
    
    return report;
}

OptimizationResult AdvancedConfigManager::optimizeConfiguration(const std::string& profileName) {
    OptimizationResult result;
    result.id = ++nextOptimizationId_;
    result.profileName = profileName;
    result.timestamp = std::chrono::system_clock::now();
    result.success = false;
    
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = profiles_.find(profileName);
    if (it == profiles_.end()) {
        result.errors.push_back("Profile does not exist");
        return result;
    }
    
    ConfigProfile& profile = it->second;
    
    bool improved = false;
    
    if (profile.settings.count("neuron_count")) {
        auto neuronCount = std::get<int>(profile.settings["neuron_count"]);
        
        if (neuronCount > 500) {
            profile.settings["neuron_count"] = 500;
            improved = true;
            result.improvements.push_back("Reduced neuron_count from " + std::to_string(neuronCount) + " to 500 for better performance");
        }
    }
    
    if (profile.settings.count("simulation_timestep")) {
        auto timestep = std::get<double>(profile.settings["simulation_timestep"]);
        
        if (timestep < 0.001) {
            profile.settings["simulation_timestep"] = 0.001;
            improved = true;
            result.improvements.push_back("Increased simulation_timestep from " + 
                                          std::to_string(timestep) + " to 0.001 for better performance");
        } else if (timestep > 0.01) {
            profile.settings["simulation_timestep"] = 0.01;
            improved = true;
            result.improvements.push_back("Decreased simulation_timestep from " + 
                                          std::to_string(timestep) + " to 0.01 for better performance");
        }
    }
    
    if (profile.settings.count("connection_probability")) {
        auto connProb = std::get<double>(profile.settings["connection_probability"]);
        
        if (connProb < 0.05) {
            profile.settings["connection_probability"] = 0.05;
            improved = true;
            result.improvements.push_back("Increased connection_probability from " + 
                                          std::to_string(connProb) + " to 0.05 for better connectivity");
        } else if (connProb > 0.2) {
            profile.settings["connection_probability"] = 0.2;
            improved = true;
            result.improvements.push_back("Decreased connection_probability from " + 
                                          std::to_string(connProb) + " to 0.2 for better performance");
        }
    }
    
    if (improved) {
        profile.lastModified = std::chrono::system_clock::now();
        if (profile.isActive) {
            applyProfile(profileName);
        }
        result.success = true;
        result.message = "Configuration optimized successfully";
    } else {
        result.message = "Configuration already optimized";
    }
    
    return result;
}

bool AdvancedConfigManager::exportToJson(const std::string& filepath, const std::string& profileName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    nlohmann::json jsonData;
    
    if (!profileName.empty() && profiles_.find(profileName) != profiles_.end()) {
        const auto& profile = profiles_[profileName];
        
        jsonData["profile_name"] = profile.name;
        jsonData["description"] = profile.description;
        jsonData["created_at"] = std::chrono::system_clock::to_time_t(profile.createdAt);
        jsonData["last_modified"] = std::chrono::system_clock::to_time_t(profile.lastModified);
        jsonData["is_active"] = profile.isActive;
        
        jsonData["settings"] = nlohmann::json::object();
        for (const auto& setting : profile.settings) {
            jsonData["settings"][setting.first] = configValueToJson(setting.second);
        }
    } else {
        jsonData["profile_name"] = currentProfile_;
        jsonData["description"] = "Current configuration";
        
        jsonData["settings"] = nlohmann::json::object();
        auto keys = config_->getKeys();
        for (const auto& key : keys) {
            jsonData["settings"][key] = configValueToJson(config_->get(ConfigValue(key)).value());
        }
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << jsonData.dump(2);
    file.close();
    return true;
}

bool AdvancedConfigManager::importFromJson(const std::string& filepath) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json jsonData;
    try {
        file >> jsonData;
    } catch (const std::exception& e) {
        file.close();
        return false;
    }
    file.close();
    
    config_->clear();
    
    if (jsonData.contains("settings") && jsonData["settings"].is_object()) {
        for (auto it = jsonData["settings"].begin(); it != jsonData["settings"].end(); ++it) {
            ConfigValue value = jsonToConfigValue(it.value());
            config_->set(it.key(), value, ConfigSource::Runtime);
        }
    }
    
    std::string profileName = "imported_" + std::to_string(std::chrono::system_clock::now().time_since_epoch().count());
    std::string description = "Imported from " + filepath;
    
    std::unordered_map<std::string, ConfigValue> settings;
    if (jsonData.contains("settings") && jsonData["settings"].is_object()) {
        for (auto it = jsonData["settings"].begin(); it != jsonData["settings"].end(); ++it) {
            settings[it.key()] = jsonToConfigValue(it.value());
        }
    }
    
    createProfile(profileName, description, settings);
    
    return true;
}

bool AdvancedConfigManager::applyProfile(const std::string& profileName) {
    std::lock_guard<std::mutex> lock(mutex_);
    
    auto it = profiles_.find(profileName);
    if (it == profiles_.end()) {
        return false;
    }
    
    ConfigProfile& profile = it->second;
    
    config_->clear();
    
    for (const auto& setting : profile.settings) {
        config_->set(setting.first, setting.second, ConfigSource::Runtime);
    }
    
    profile.isActive = true;
    currentProfile_ = profileName;
    
    return true;
}

std::string AdvancedConfigManager::configValueToString(const ConfigValue& value) const {
    std::stringstream ss;
    std::visit([&ss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            ss << arg;
        } else if constexpr (std::is_same_v<T, int>) {
            ss << arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            ss << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            ss << std::fixed << std::setprecision(6) << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            ss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                ss << arg[i];
                if (i < arg.size() - 1) ss << ", ";
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                ss << std::fixed << std::setprecision(6) << arg[i];
                if (i < arg.size() - 1) ss << ", ";
            }
            ss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            ss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                ss << "\"" << arg[i] << "\"";
                if (i < arg.size() - 1) ss << ", ";
            }
            ss << "]";
        }
    }, value);
    return ss.str();
}

AdvancedConfigManager::ChangeType AdvancedConfigManager::determineChangeType(const ConfigValue& oldValue, const ConfigValue& newValue) const {
    if (oldValue == newValue) {
        return ChangeType::Unchanged;
    }
    
    bool oldNumeric = false, newNumeric = false;
    double oldNum = 0.0, newNum = 0.0;
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> ||
                     std::is_same_v<T, double>) {
            oldNumeric = true;
            oldNum = (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) ? 
                    static_cast<double>(std::get<T>(arg)) : std::get<double>(arg);
        }
    }, oldValue);
    
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> ||
                     std::is_same_v<T, double>) {
            newNumeric = true;
            newNum = (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) ? 
                    static_cast<double>(std::get<T>(arg)) : std::get<double>(arg);
        }
    }, newValue);
    
    if (oldNumeric && newNumeric) {
        if (newNum > oldNum) {
            return ChangeType::Increased;
        } else {
            return ChangeType::Decreased;
        }
    }
    
    return ChangeType::Changed;
}

std::shared_ptr<Config> AdvancedConfigManager::getConfig() const {
    return config_;
}

std::string AdvancedConfigManager::getCurrentProfile() const {
    return currentProfile_;
}

void AdvancedConfigManager::setCurrentProfile(const std::string& profileName) {
    std::lock_guard<std::mutex> lock(mutex_);
    currentProfile_ = profileName;
}

} // namespace nlm
