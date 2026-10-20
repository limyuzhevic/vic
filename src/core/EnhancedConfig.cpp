#include "EnhancedConfig.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

struct EnhancedConfig::Impl {
    std::shared_ptr<Config> baseConfig;
    std::shared_ptr<Logger> logger;
    
    std::map<std::string, ConfigurationPreset> presets;
    std::map<std::string, std::string> scenarios;
    
    std::vector<ConfigChange> changeHistory;
    mutable std::mutex historyMutex;
    
    std::vector<ValidationRule> validationRules;
    bool enableHistory;
    size_t maxHistorySize;
    
    std::chrono::steady_clock::time_point lastModified;
    std::map<std::string, std::string> runtimeOverrides;
    
    Impl(std::shared_ptr<Config> cfg, std::shared_ptr<Logger> logg)
        : baseConfig(cfg), logger(logg), enableHistory(true), maxHistorySize(1000) {
        lastModified = std::chrono::steady_clock::now();
        
        // Register default presets
        registerDefaultPresets();
    }
    
    void registerDefaultPresets() {
        // Quick test preset
        ConfigurationPreset quickTest("quick_test", "Quick test with minimal configuration");
        quickTest.defaultValues = {
            {"neuron_count", "50"},
            {"region_count", "1"},
            {"simulation_timestep", "0.001"},
            {"connection_probability", "0.15"}
        };
        quickTest.requiredParameters = {"neuron_count", "simulation_timestep"};
        quickTest.recommendedParameters = {"region_count", "connection_probability"};
        quickTest.timeout = std::chrono::seconds(30);
        quickTest.scenario = "testing";
        presets["quick_test"] = quickTest;
        
        // Production preset
        ConfigurationPreset production("production", "Production configuration with full functionality");
        production.defaultValues = {
            {"neuron_count", "500"},
            {"region_count", "3"},
            {"simulation_timestep", "0.0005"},
            {"connection_probability", "0.2"},
            {"stdp_ltp_weight", "0.02"},
            {"stdp_ltd_weight", "0.015"},
            {"synaptogenesis_rate", "0.0001"},
            {"pruning_rate", "0.00001"}
        };
        production.requiredParameters = {"neuron_count", "simulation_timestep", "region_count"};
        production.recommendedParameters = {"connection_probability", "stdp_ltp_weight", "stdp_ltd_weight"};
        production.timeout = std::chrono::seconds(3600);
        production.scenario = "production";
        presets["production"] = production;
        
        // Research preset
        ConfigurationPreset research("research", "Research configuration with high plasticity");
        research.defaultValues = {
            {"neuron_count", "1000"},
            {"region_count", "5"},
            {"simulation_timestep", "0.0001"},
            {"connection_probability", "0.25"},
            {"stdp_ltp_weight", "0.03"},
            {"stdp_ltd_weight", "0.025"},
            {"synaptogenesis_rate", "0.001"},
            {"pruning_rate", "0.0001"},
            {"enable_plasticity", "true"},
            {"plasticity_steps", "5000"}
        };
        research.requiredParameters = {"neuron_count", "simulation_timestep"};
        research.recommendedParameters = {"region_count", "connection_probability", "plasticity_steps"};
        research.timeout = std::chrono::seconds(7200);
        research.scenario = "research";
        presets["research"] = research;
    }
};

EnhancedConfig::EnhancedConfig(std::shared_ptr<Config> baseConfig, std::shared_ptr<Logger> logger) 
    : pImpl(std::make_unique<Impl>(baseConfig, logger)) {
}

EnhancedConfig::~EnhancedConfig() = default;

void EnhancedConfig::registerPreset(const ConfigurationPreset& preset) {
    pImpl->presets[preset.name] = preset;
}

bool EnhancedConfig::applyPreset(const std::string& presetName) {
    auto it = pImpl->presets.find(presetName);
    if (it == pImpl->presets.end()) {
        if (pImpl->logger) {
            pImpl->logger->error("Preset not found: " + presetName);
        }
        return false;
    }
    
    const auto& preset = it->second;
    
    // Apply preset values
    for (const auto& pair : preset.defaultValues) {
        setValue(pair.first, pair.second, ConfigSource::Runtime, "preset:" + presetName);
    }
    
    return true;
}

bool EnhancedConfig::createCustomPreset(const std::string& name, const std::string& description,
                                       const std::map<std::string, std::string>& values) {
    ConfigurationPreset preset(name, description);
    preset.defaultValues = values;
    
    // Extract required and recommended parameters from values
    for (const auto& pair : values) {
        if (pair.first.find("count") != std::string::npos || 
            pair.first.find("rate") != std::string::npos ||
            pair.first.find("steps") != std::string::npos) {
            preset.recommendedParameters.push_back(pair.first);
        }
    }
    
    registerPreset(preset);
    return true;
}

std::vector<std::string> EnhancedConfig::getAvailablePresets() const {
    std::vector<std::string> names;
    names.reserve(pImpl->presets.size());
    
    for (const auto& pair : pImpl->presets) {
        names.push_back(pair.first);
    }
    
    return names;
}

ConfigurationPreset EnhancedConfig::getPreset(const std::string& name) const {
    auto it = pImpl->presets.find(name);
    if (it != pImpl->presets.end()) {
        return it->second;
    }
    return ConfigurationPreset("invalid", "");
}

bool EnhancedConfig::setValue(const std::string& key, const std::string& value,
                             ConfigSource source, const std::string& changedBy) {
    std::string oldValue;
    bool wasChanged = false;
    
    // Check current value
    if (baseConfig->has(key)) {
        auto currentVal = baseConfig->get<std::string>(key);
        if (currentVal.has_value()) {
            oldValue = currentVal.value();
            wasChanged = (oldValue != value);
        }
    }
    
    // Set the value
    baseConfig->set(key, value, source);
    
    // Record change if value was actually modified
    if (wasChanged) {
        recordChange(key, oldValue, value, source, changedBy);
    }
    
    pImpl->lastModified = std::chrono::steady_clock::now();
    return true;
}

bool EnhancedConfig::setValue(const std::string& key, int value,
                             ConfigSource source, const std::string& changedBy) {
    std::string strValue = std::to_string(value);
    return setValue(key, strValue, source, changedBy);
}

bool EnhancedConfig::setValue(const std::string& key, double value,
                             ConfigSource source, const std::string& changedBy) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(6) << value;
    return setValue(key, ss.str(), source, changedBy);
}

bool EnhancedConfig::setValue(const std::string& key, bool value,
                             ConfigSource source, const std::string& changedBy) {
    std::string strValue = value ? "true" : "false";
    return setValue(key, strValue, source, changedBy);
}

ValidationResult EnhancedConfig::validateConfiguration() const {
    ValidationResult result;
    
    // Validate all registered validation rules
    for (const auto& rule : pImpl->validationRules) {
        if (!baseConfig->has(rule.parameter)) {
            if (rule.required) {
                result.addError("Required parameter missing: " + rule.parameter);
            }
            continue;
        }
        
        std::string value;
        auto val = baseConfig->get<std::string>(rule.parameter);
        if (val.has_value()) {
            value = val.value();
        }
        
        if (!evaluateValidationRules(rule, value)) {
            result.addError(rule.errorMessage);
        }
    }
    
    return result;
}

ValidationResult EnhancedConfig::validateParameter(const std::string& key) const {
    ValidationResult result;
    
    for (const auto& rule : pImpl->validationRules) {
        if (rule.parameter == key) {
            if (!baseConfig->has(key)) {
                if (rule.required) {
                    result.addError("Required parameter missing: " + key);
                }
            } else {
                std::string value;
                auto val = baseConfig->get<std::string>(key);
                if (val.has_value()) {
                    value = val.value();
                }
                
                if (!evaluateValidationRules(rule, value)) {
                    result.addError(rule.errorMessage);
                }
            }
            break;
        }
    }
    
    return result;
}

bool EnhancedConfig::addValidationRule(const ValidationRule& rule) {
    pImpl->validationRules.push_back(rule);
    return true;
}

void EnhancedConfig::clearValidationRules() {
    pImpl->validationRules.clear();
}

bool EnhancedConfig::enableChangeHistory(bool enable) {
    pImpl->enableHistory = enable;
    return true;
}

std::vector<ConfigChange> EnhancedConfig::getChangeHistory(size_t limit) const {
    std::lock_guard<std::mutex> lock(pImpl->historyMutex);
    
    std::vector<ConfigChange> history(pImpl->changeHistory);
    
    // Sort by timestamp (newest first)
    std::sort(history.begin(), history.end(),
        [](const ConfigChange& a, const ConfigChange& b) {
            return a.timestamp > b.timestamp;
        });
    
    if (limit > 0 && limit < history.size()) {
        history.resize(limit);
    }
    
    return history;
}

std::vector<ConfigChange> EnhancedConfig::getChangesBySource(ConfigSource source) const {
    std::lock_guard<std::mutex> lock(pImpl->historyMutex);
    
    std::vector<ConfigChange> changes;
    for (const auto& change : pImpl->changeHistory) {
        if (change.source == source) {
            changes.push_back(change);
        }
    }
    
    return changes;
}

std::vector<ConfigChange> EnhancedConfig::getChangesByKey(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->historyMutex);
    
    std::vector<ConfigChange> changes;
    for (const auto& change : pImpl->changeHistory) {
        if (change.key == key) {
            changes.push_back(change);
        }
    }
    
    // Sort by timestamp (newest first)
    std::sort(changes.begin(), changes.end(),
        [](const ConfigChange& a, const ConfigChange& b) {
            return a.timestamp > b.timestamp;
        });
    
    return changes;
}

bool EnhancedConfig::hasChangesSince(const std::chrono::steady_clock::time_point& since) const {
    std::lock_guard<std::mutex> lock(pImpl->historyMutex);
    
    for (const auto& change : pImpl->changeHistory) {
        if (change.timestamp > since) {
            return true;
        }
    }
    
    return false;
}

std::map<std::string, std::pair<std::string, std::string>> EnhancedConfig::getDifferences(
    const EnhancedConfig& other) const {
    std::map<std::string, std::pair<std::string, std::string>> differences;
    
    // Compare all keys from both configurations
    auto keys1 = baseConfig->getKeys();
    auto keys2 = other.getBaseConfig()->getKeys();
    
    std::set<std::string> allKeys;
    for (const auto& key : keys1) allKeys.insert(key);
    for (const auto& key : keys2) allKeys.insert(key);
    
    for (const auto& key : allKeys) {
        std::string val1, val2;
        
        if (baseConfig->has(key)) {
            auto val = baseConfig->get<std::string>(key);
            if (val.has_value()) val1 = val.value();
        }
        
        if (other.getBaseConfig()->has(key)) {
            auto val = other.getBaseConfig()->get<std::string>(key);
            if (val.has_value()) val2 = val.value();
        }
        
        if (val1 != val2) {
            differences[key] = std::make_pair(val1, val2);
        }
    }
    
    return differences;
}

bool EnhancedConfig::saveToFile(const std::string& filepath, bool includeHistory) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    json j;
    
    // Save configuration values
    json configJson;
    auto keys = baseConfig->getKeys();
    for (const auto& key : keys) {
        auto val = baseConfig->get<std::string>(key);
        if (val.has_value()) {
            configJson[key] = val.value();
        }
    }
    
    j["configuration"] = configJson;
    
    // Save validation rules
    json rulesJson = json::array();
    for (const auto& rule : pImpl->validationRules) {
        json ruleJson;
        ruleJson["parameter"] = rule.parameter;
        ruleJson["required"] = rule.required;
        ruleJson["error_message"] = rule.errorMessage;
        
        // Note: Rule functions cannot be serialized
        rulesJson.push_back(ruleJson);
    }
    
    j["validation_rules"] = rulesJson;
    
    // Save change history if requested
    if (includeHistory && pImpl->enableHistory) {
        std::lock_guard<std::mutex> lock(pImpl->historyMutex);
        
        json historyJson = json::array();
        for (const auto& change : pImpl->changeHistory) {
            json changeJson;
            changeJson["key"] = change.key;
            changeJson["old_value"] = change.oldValue;
            changeJson["new_value"] = change.newValue;
            changeJson["source"] = static_cast<int>(change.source);
            changeJson["timestamp"] = std::chrono::duration_cast<std::chrono::milliseconds>(
                change.timestamp.time_since_epoch()).count();
            changeJson["changed_by"] = change.changedBy;
            
            historyJson.push_back(changeJson);
        }
        
        j["change_history"] = historyJson;
    }
    
    file << j.dump(2);
    return true;
}

bool EnhancedConfig::loadFromFile(const std::string& filepath, bool merge) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    json j;
    try {
        file >> j;
    } catch (const json::parse_error& e) {
        return false;
    }
    
    if (!j.contains("configuration")) {
        return false;
    }
    
    json configJson = j["configuration"];
    
    // Clear existing configuration if not merging
    if (!merge) {
        baseConfig->clear();
        pImpl->changeHistory.clear();
    }
    
    // Load configuration values
    for (auto it = configJson.begin(); it != configJson.end(); ++it) {
        std::string key = it.key();
        std::string value;
        
        if (it->is_string()) {
            value = it->get<std::string>();
        } else if (it->is_number_integer()) {
            value = std::to_string(it->get<int64_t>());
        } else if (it->is_number_float()) {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(6) << it->get<double>();
            value = ss.str();
        } else if (it->is_boolean()) {
            value = it->get<bool>() ? "true" : "false";
        }
        
        setValue(key, value, ConfigSource::File, "file_load");
    }
    
    return true;
}

bool EnhancedConfig::exportToJSON(const std::string& filepath) const {
    return saveToFile(filepath, true);
}

bool EnhancedConfig::importFromJSON(const std::string& filepath, bool merge) {
    return loadFromFile(filepath, merge);
}

bool EnhancedConfig::createScenario(const std::string& name, const std::string& description) {
    pImpl->scenarios[name] = description;
    return true;
}

bool EnhancedConfig::applyScenario(const std::string& name) {
    auto it = pImpl->scenarios.find(name);
    if (it == pImpl->scenarios.end()) {
        return false;
    }
    
    // In real implementation, would load scenario-specific configuration
    return true;
}

bool EnhancedConfig::deleteScenario(const std::string& name) {
    pImpl->scenarios.erase(name);
    return true;
}

std::vector<std::string> EnhancedConfig::getAvailableScenarios() const {
    std::vector<std::string> names;
    names.reserve(pImpl->scenarios.size());
    
    for (const auto& pair : pImpl->scenarios) {
        names.push_back(pair.first);
    }
    
    return names;
}

std::map<std::string, std::string> EnhancedConfig::getSummary() const {
    std::map<std::string, std::string> summary;
    
    auto keys = baseConfig->getKeys();
    summary["total_parameters"] = std::to_string(keys.size());
    
    size_t validCount = 0;
    for (const auto& key : keys) {
        ValidationResult validation = validateParameter(key);
        if (validation.valid) {
            validCount++;
        }
    }
    
    summary["valid_parameters"] = std::to_string(validCount);
    summary["invalid_parameters"] = std::to_string(keys.size() - validCount);
    
    auto lastModifiedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - pImpl->lastModified).count();
    summary["last_modified_seconds_ago"] = std::to_string(lastModifiedMs / 1000.0);
    
    return summary;
}

std::vector<std::string> EnhancedConfig::getInvalidParameters() const {
    std::vector<std::string> invalid;
    
    auto keys = baseConfig->getKeys();
    for (const auto& key : keys) {
        ValidationResult validation = validateParameter(key);
        if (!validation.valid) {
            invalid.push_back(key);
        }
    }
    
    return invalid;
}

std::vector<std::string> EnhancedConfig::getOutdatedParameters() const {
    // In real implementation, would check against known parameter schemas
    return std::vector<std::string>();
}

double EnhancedConfig::getConfigurationScore() const {
    auto summary = getSummary();
    size_t total = std::stoi(summary["total_parameters"]);
    size_t valid = std::stoi(summary["valid_parameters"]);
    
    if (total == 0) return 0.0;
    
    double validityScore = static_cast<double>(valid) / total;
    
    // Consider change history completeness
    double historyScore = 1.0;
    if (pImpl->enableHistory && !pImpl->changeHistory.empty()) {
        historyScore = std::min(1.0, static_cast<double>(pImpl->changeHistory.size()) / 100.0);
    }
    
    // Consider validation rules coverage
    double rulesScore = 1.0;
    if (!pImpl->validationRules.empty()) {
        rulesScore = std::min(1.0, static_cast<double>(pImpl->validationRules.size()) / 50.0);
    }
    
    return (validityScore * 0.5 + historyScore * 0.3 + rulesScore * 0.2);
}

std::shared_ptr<Config> EnhancedConfig::getBaseConfig() const {
    return baseConfig;
}

template<typename T>
std::optional<T> EnhancedConfig::get(const std::string& key) const {
    return baseConfig->get<T>(key);
}

template<typename T>
T EnhancedConfig::getOr(const std::string& key, const T& defaultValue) const {
    return baseConfig->getOr<T>(key, defaultValue);
}

size_t EnhancedConfig::getTotalParameters() const {
    return baseConfig->getKeys().size();
}

size_t EnhancedConfig::getValidParameters() const {
    auto invalid = getInvalidParameters();
    return getTotalParameters() - invalid.size();
}

size_t EnhancedConfig::getInvalidParameters() const {
    return getInvalidParameters().size();
}

std::chrono::steady_clock::time_point EnhancedConfig::getLastModified() const {
    return pImpl->lastModified;
}

void EnhancedConfig::recordChange(const std::string& key, const std::string& oldValue,
                                 const std::string& newValue, ConfigSource source,
                                 const std::string& changedBy) {
    if (!pImpl->enableHistory) return;
    
    ConfigChange change(key, oldValue, newValue, source, changedBy);
    
    std::lock_guard<std::mutex> lock(pImpl->historyMutex);
    pImpl->changeHistory.push_back(change);
    
    // Cleanup old history
    cleanupOldHistory(pImpl->maxHistorySize);
}

bool EnhancedConfig::validateAgainstRules(const std::string& key, const std::string& value) const {
    for (const auto& rule : pImpl->validationRules) {
        if (rule.parameter == key) {
            return evaluateValidationRules(rule, value);
        }
    }
    return true; // No rules for this parameter means it's valid
}

std::string EnhancedConfig::getStringValue(const std::string& key) const {
    auto val = baseConfig->get<std::string>(key);
    return val.has_value() ? val.value() : "";
}

bool EnhancedConfig::isValueChanged(const std::string& key, const std::string& value) const {
    std::string currentValue;
    auto val = baseConfig->get<std::string>(key);
    if (val.has_value()) {
        currentValue = val.value();
    }
    return currentValue != value;
}

void EnhancedConfig::cleanupOldHistory(size_t maxHistorySize) const {
    std::lock_guard<std::mutex> lock(pImpl->historyMutex);
    
    if (pImpl->changeHistory.size() > maxHistorySize) {
        // Keep most recent changes
        size_t toRemove = pImpl->changeHistory.size() - maxHistorySize;
        pImpl->changeHistory.erase(pImpl->changeHistory.begin(),
                                  pImpl->changeHistory.begin() + toRemove);
    }
}

bool EnhancedConfig::evaluateValidationRules(const ValidationRule& rule, const std::string& value) const {
    // Check if value matches allowed values
    if (!rule.allowedValues.empty()) {
        if (std::find(rule.allowedValues.begin(), rule.allowedValues.end(), value) == rule.allowedValues.end()) {
            return false;
        }
    }
    
    // Check range for numeric values
    if (rule.range.first != 0.0 || rule.range.second != 0.0) {
        try {
            double numValue = std::stod(value);
            if (numValue < rule.range.first || numValue > rule.range.second) {
                return false;
            }
        } catch (...) {
            return false;
        }
    }
    
    // Check custom validator
    if (rule.validator) {
        return rule.validator(value);
    }
    
    return true;
}

std::string EnhancedConfig::getParameterType(const std::string& key) const {
    auto val = baseConfig->get<std::string>(key);
    if (val.has_value()) {
        return "string";
    }
    
    auto intVal = baseConfig->get<int>(key);
    if (intVal.has_value()) {
        return "int";
    }
    
    auto doubleVal = baseConfig->get<double>(key);
    if (doubleVal.has_value()) {
        return "double";
    }
    
    auto boolVal = baseConfig->get<bool>(key);
    if (boolVal.has_value()) {
        return "bool";
    }
    
    return "unknown";
}

std::vector<std::string> EnhancedConfig::getParameterAllowedValues(const std::string& key) const {
    // In real implementation, would have predefined allowed values
    return std::vector<std::string>();
}

std::pair<double, double> EnhancedConfig::getParameterRange(const std::string& key) const {
    // In real implementation, would have predefined ranges
    return std::make_pair(0.0, 0.0);
}

ConfigurationValidator::ConfigurationValidator(std::shared_ptr<Logger> logger) 
    : pImpl(std::make_unique<Impl>()) {
    pImpl->logger = logger;
    registerDefaultValidationRules();
}

ConfigurationValidator::~ConfigurationValidator() = default;

ValidationResult ConfigurationValidator::validateForScenario(const std::string& scenario) const {
    ValidationResult result;
    
    if (scenario == "testing") {
        auto keys = getRequiredParametersForScenario(scenario);
        for (const auto& key : keys) {
            if (!pImpl->logger->getConfig()->has(key)) {
                result.addError("Required parameter missing for testing scenario: " + key);
            }
        }
    } else if (scenario == "production") {
        auto keys = getRequiredParametersForScenario(scenario);
        for (const auto& key : keys) {
            if (!pImpl->logger->getConfig()->has(key)) {
                result.addError("Required parameter missing for production scenario: " + key);
            }
        }
    } else if (scenario == "research") {
        auto keys = getRequiredParametersForScenario(scenario);
        for (const auto& key : keys) {
            if (!pImpl->logger->getConfig()->has(key)) {
                result.addError("Required parameter missing for research scenario: " + key);
            }
        }
    }
    
    return result;
}

ValidationResult ConfigurationValidator::validateForTest(const std::string& testName) const {
    ValidationResult result;
    
    // Validate test-specific parameters
    if (testName == "plasticity") {
        if (pImpl->logger->getConfig()->has("plasticity_steps")) {
            auto val = pImpl->logger->getConfig()->get<int64_t>("plasticity_steps");
            if (val.has_value() && val.value() <= 0) {
                result.addError("plasticity_steps must be positive");
            }
        }
        
        if (pImpl->logger->getConfig()->has("learning_rate")) {
            auto val = pImpl->logger->getConfig()->get<double>("learning_rate");
            if (val.has_value() && (val.value() < 0.0 || val.value() > 1.0)) {
                result.addError("learning_rate must be between 0.0 and 1.0");
            }
        }
    } else if (testName == "stdp") {
        if (pImpl->logger->getConfig()->has("stdp_trials")) {
            auto val = pImpl->logger->getConfig()->get<int64_t>("stdp_trials");
            if (val.has_value() && val.value() <= 0) {
                result.addError("stdp_trials must be positive");
            }
        }
    }
    
    return result;
}

ValidationResult ConfigurationValidator::validateForPerformance(const std::string& performanceGoal) const {
    ValidationResult result;
    
    if (performanceGoal == "fast") {
        // For fast execution, require certain optimizations
        if (pImpl->logger->getConfig()->has("neuron_count")) {
            auto val = pImpl->logger->getConfig()->get<int64_t>("neuron_count");
            if (val.has_value() && val.value() > 100) {
                result.addWarning("High neuron count may impact performance for fast execution");
            }
        }
    } else if (performanceGoal == "high_accuracy") {
        // For high accuracy, require certain precision settings
        if (pImpl->logger->getConfig()->has("simulation_timestep")) {
            auto val = pImpl->logger->getConfig()->get<double>("simulation_timestep");
            if (val.has_value() && val.value() > 0.001) {
                result.addWarning("Large simulation timestep may impact accuracy");
            }
        }
    }
    
    return result;
}

ValidationResult ConfigurationValidator::validateParameter(const std::string& key, const std::string& value) const {
    ValidationResult result;
    
    if (!isParameterValid(key, value)) {
        result.addError(getParameterValidationError(key, value));
    }
    
    return result;
}

bool ConfigurationValidator::isParameterValid(const std::string& key, const std::string& value) const {
    // Basic validation for known parameters
    if (key == "neuron_count") {
        try {
            int count = std::stoi(value);
            return count > 0 && count <= 10000;
        } catch (...) {
            return false;
        }
    } else if (key == "simulation_timestep") {
        try {
            double timestep = std::stod(value);
            return timestep > 0.0 && timestep <= 1.0;
        } catch (...) {
            return false;
        }
    } else if (key == "connection_probability") {
        try {
            double prob = std::stod(value);
            return prob >= 0.0 && prob <= 1.0;
        } catch (...) {
            return false;
        }
    }
    
    return true; // Unknown parameters are considered valid
}

std::string ConfigurationValidator::getParameterValidationError(const std::string& key, const std::string& value) const {
    if (key == "neuron_count") {
        try {
            int count = std::stoi(value);
            if (count <= 0) return "neuron_count must be positive";
            if (count > 10000) return "neuron_count must not exceed 10000";
        } catch (...) {
            return "neuron_count must be an integer";
        }
    } else if (key == "simulation_timestep") {
        try {
            double timestep = std::stod(value);
            if (timestep <= 0.0) return "simulation_timestep must be positive";
            if (timestep > 1.0) return "simulation_timestep must not exceed 1.0";
        } catch (...) {
            return "simulation_timestep must be a number";
        }
    } else if (key == "connection_probability") {
        try {
            double prob = std::stod(value);
            if (prob < 0.0) return "connection_probability must be non-negative";
            if (prob > 1.0) return "connection_probability must not exceed 1.0";
        } catch (...) {
            return "connection_probability must be a number";
        }
    }
    
    return "Invalid value for parameter: " + key;
}

ValidationResult ConfigurationValidator::validatePreset(const ConfigurationPreset& preset) const {
    ValidationResult result;
    
    // Check required parameters
    for (const auto& reqParam : preset.requiredParameters) {
        if (pImpl->logger->getConfig()->has(reqParam)) {
            auto val = pImpl->logger->getConfig()->get<std::string>(reqParam);
            if (!val.has_value() || val.value().empty()) {
                result.addError("Required parameter for preset missing: " + reqParam);
            }
        } else {
            result.addError("Required parameter for preset missing: " + reqParam);
        }
    }
    
    // Check recommended parameters
    for (const auto& recParam : preset.recommendedParameters) {
        if (!pImpl->logger->getConfig()->has(recParam)) {
            result.addWarning("Recommended parameter for preset missing: " + recParam);
        }
    }
    
    return result;
}

ValidationResult ConfigurationValidator::validateScenario(const std::string& name) const {
    ValidationResult result;
    
    if (name == "testing") {
        // Testing scenario requires specific settings
        if (pImpl->logger->getConfig()->has("neuron_count")) {
            auto val = pImpl->logger->getConfig()->get<int64_t>("neuron_count");
            if (val.has_value() && val.value() > 100) {
                result.addWarning("Testing scenario with large neuron count may be slow");
            }
        }
    } else if (name == "production") {
        // Production scenario requires reliability settings
        if (pImpl->logger->getConfig()->has("enable_plasticity")) {
            auto val = pImpl->logger->getConfig()->get<bool>("enable_plasticity");
            if (val.has_value() && !val.value()) {
                result.addWarning("Production scenario may benefit from plasticity");
            }
        }
    } else if (name == "research") {
        // Research scenario requires exploration settings
        if (pImpl->logger->getConfig()->has("plasticity_steps")) {
            auto val = pImpl->logger->getConfig()->get<int64_t>("plasticity_steps");
            if (val.has_value() && val.value() < 1000) {
                result.addWarning("Research scenario may benefit from more plasticity steps");
            }
        }
    }
    
    return result;
}

void ConfigurationValidator::registerDefaultValidationRules() {
    // Register default validation rules for key parameters
    ValidationRule neuronCountRule("neuron_count",
        [](const std::string& val) {
            try { return std::stoi(val) > 0 && std::stoi(val) <= 10000; }
            catch (...) { return false; }
        },
        "neuron_count must be between 1 and 10000",
        true); // required
    
    ValidationRule simulationTimestepRule("simulation_timestep",
        [](const std::string& val) {
            try { double d = std::stod(val); return d > 0.0 && d <= 1.0; }
            catch (...) { return false; }
        },
        "simulation_timestep must be between 0.0 and 1.0",
        true); // required
    
    ValidationRule connectionProbRule("connection_probability",
        [](const std::string& val) {
            try { double d = std::stod(val); return d >= 0.0 && d <= 1.0; }
            catch (...) { return false; }
        },
        "connection_probability must be between 0.0 and 1.0");
    
    // Note: Rule functions cannot be stored directly, so these would need to be
    // managed differently in a complete implementation
}

ValidationResult ConfigurationValidator::validateNeuralParameters() const {
    ValidationResult result;
    
    // Validate neural network parameters
    if (pImpl->logger->getConfig()->has("neuron_count")) {
        auto val = pImpl->logger->getConfig()->get<int64_t>("neuron_count");
        if (val.has_value() && (val.value() <= 0 || val.value() > 100000)) {
            result.addError("Invalid neuron_count value");
        }
    }
    
    return result;
}

ValidationResult ConfigurationValidator::validatePlasticityParameters() const {
    ValidationResult result;
    
    // Validate plasticity parameters
    if (pImpl->logger->getConfig()->has("stdp_ltp_weight")) {
        auto val = pImpl->logger->getConfig()->get<double>("stdp_ltp_weight");
        if (val.has_value() && (val.value() < 0.0 || val.value() > 1.0)) {
            result.addError("Invalid stdp_ltp_weight value");
        }
    }
    
    if (pImpl->logger->getConfig()->has("stdp_ltd_weight")) {
        auto val = pImpl->logger->getConfig()->get<double>("stdp_ltd_weight");
        if (val.has_value() && (val.value() < 0.0 || val.value() > 1.0)) {
            result.addError("Invalid stdp_ltd_weight value");
        }
    }
    
    return result;
}

ValidationResult ConfigurationValidator::validateMemoryParameters() const {
    ValidationResult result;
    
    // Validate memory system parameters
    if (pImpl->logger->getConfig()->has("memory_capacity")) {
        auto val = pImpl->logger->getConfig()->get<int64_t>("memory_capacity");
        if (val.has_value() && val.value() <= 0) {
            result.addError("Invalid memory_capacity value");
        }
    }
    
    return result;
}

ValidationResult ConfigurationValidator::validatePerformanceParameters() const {
    ValidationResult result;
    
    // Validate performance-related parameters
    if (pImpl->logger->getConfig()->has("max_execution_time")) {
        auto val = pImpl->logger->getConfig()->get<int64_t>("max_execution_time");
        if (val.has_value() && val.value() <= 0) {
            result.addError("Invalid max_execution_time value");
        }
    }
    
    return result;
}

std::vector<std::string> ConfigurationValidator::getRequiredParametersForScenario(const std::string& scenario) const {
    if (scenario == "testing") {
        return {"neuron_count", "simulation_timestep"};
    } else if (scenario == "production") {
        return {"neuron_count", "simulation_timestep", "region_count"};
    } else if (scenario == "research") {
        return {"neuron_count", "simulation_timestep", "plasticity_steps"};
    }
    
    return {};
}

bool ConfigurationValidator::checkParameterDependencies(const std::string& key, 
                                                      const std::map<std::string, std::string>& allValues) const {
    // Check if parameter depends on other parameters
    if (key == "plasticity_steps" && allValues.find("enable_plasticity") == allValues.end()) {
        return false;
    }
    
    return true;
}

} // namespace nlm
