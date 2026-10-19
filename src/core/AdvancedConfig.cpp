// Advanced configuration implementation
#include "AdvancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <cctype>

namespace nlm {

AdvancedConfig::AdvancedConfig() = default;

AdvancedConfig::~AdvancedConfig() = default;

bool AdvancedConfig::loadFromFile(const std::string& filepath, bool validate) {
    if (coreConfig_.loadFromFile(filepath)) {
        if (validate && !validate()) {
            return false;
        }
        return true;
    }
    return false;
}

bool AdvancedConfig::loadFromArgs(int argc, char** argv, bool validate) {
    coreConfig_.loadFromArgs(argc, argv);
    
    if (validate && !validate()) {
        return false;
    }
    
    return true;
}

bool AdvancedConfig::loadFromEnvironment(bool validate) {
    bool success = true;
    
    // Load all environment variables with NLM_ prefix
    for (const auto& envEntry : std::filesystem::environment()) {
        std::string envVar = envEntry.first;
        std::string envValue = envEntry.second;
        
        // Check if this is an NLM configuration variable
        if (envVar.rfind(ENV_PREFIX, 0) == 0) {
            if (!loadFromEnvironmentVariable(envVar, validate)) {
                success = false;
            }
        }
    }
    
    return success;
}

bool AdvancedConfig::loadFromEnvironmentVariable(const std::string& envVar, bool validate) {
    std::string key = envVarToKey(envVar);
    std::string value = std::getenv(envVar.c_str());
    
    if (!value.empty()) {
        // Try to parse the value based on the key type
        if (key.find("bool") != std::string::npos || key.find("enable") != std::string::npos) {
            // Parse boolean values
            std::transform(value.begin(), value.end(), value.begin(), ::tolower);
            bool boolValue = (value == "true" || value == "1" || value == "yes" || value == "on");
            coreConfig_.set(key, boolValue, ConfigSource::Environment);
        } else if (key.find("int") != std::string::npos || key.find("count") != std::string::npos) {
            // Try to parse as integer
            try {
                int intValue = std::stoi(value);
                coreConfig_.set(key, intValue, ConfigSource::Environment);
            } catch (const std::exception&) {
                // Fall back to string
                coreConfig_.set(key, value, ConfigSource::Environment);
            }
        } else if (key.find("float") != std::string::npos || key.find("rate") != std::string::npos || 
                   key.find("size") != std::string::npos || key.find("time") != std::string::npos) {
            // Try to parse as float
            try {
                double doubleValue = std::stod(value);
                coreConfig_.set(key, doubleValue, ConfigSource::Environment);
            } catch (const std::exception&) {
                // Fall back to string
                coreConfig_.set(key, value, ConfigSource::Environment);
            }
        } else {
            // Use string value
            coreConfig_.set(key, value, ConfigSource::Environment);
        }
        
        return true;
    }
    
    return false;
}

bool AdvancedConfig::loadPreset(Preset preset, bool validate) {
    auto presetValues = getPresetValues(preset);
    return applyPreset(presetValues, validate);
}

void AdvancedConfig::merge(const AdvancedConfig& other, ConfigSource source) {
    for (const auto& key : other.coreConfig_.getKeys()) {
        auto value = other.coreConfig_.get<std::string>(key);
        if (value) {
            // Try to preserve the original type
            auto originalValue = coreConfig_.get(key);
            if (originalValue) {
                // Use the same type as original
                std::visit([&](auto&& val) {
                    using T = std::decay_t<decltype(val)>;
                    if constexpr (std::is_same_v<T, int>) {
                        coreConfig_.set(key, *value, source);
                    } else if constexpr (std::is_same_v<T, double>) {
                        try {
                            double doubleVal = std::stod(*value);
                            coreConfig_.set(key, doubleVal, source);
                        } catch (...) {
                            coreConfig_.set(key, *value, source);
                        }
                    } else if constexpr (std::is_same_v<T, bool>) {
                        std::string lowerVal = *value;
                        std::transform(lowerVal.begin(), lowerVal.end(), lowerVal.begin(), ::tolower);
                        bool boolVal = (lowerVal == "true" || lowerVal == "1" || lowerVal == "yes" || lowerVal == "on");
                        coreConfig_.set(key, boolVal, source);
                    } else {
                        coreConfig_.set(key, *value, source);
                    }
                }, *originalValue);
            } else {
                coreConfig_.set(key, *value, source);
            }
        }
    }
}

bool AdvancedConfig::validate() const {
    validationErrors_.clear();
    
    auto validationRules = getValidationRules();
    for (const auto& rule : validationRules) {
        if (coreConfig_.has(rule.key)) {
            std::string error;
            if (!validateValue(rule.key, coreConfig_.get<ConfigValue>(rule.key).value(), error)) {
                validationErrors_.push_back(rule.key + ": " + error + " (" + rule.errorMessage + ")");
            }
        }
    }
    
    return validationErrors_.empty();
}

std::vector<std::string> AdvancedConfig::getValidationErrors() const {
    return validationErrors_;
}

const std::map<std::string, std::pair<double, double>> AdvancedConfig::getKeyRanges() {
    static const std::map<std::string, std::pair<double, double>> ranges = {
        {"neuron_count", {1, 10000}},
        {"region_count", {1, 100}},
        {"connection_probability", {0.0, 1.0}},
        {"plasticity_learning_rate", {0.0, 1.0}},
        {"stdp_ltp_weight", {0.0, 1.0}},
        {"stdp_ltd_weight", {0.0, 1.0}},
        {"stdp_tau", {1.0, 1000.0}},
        {"synaptogenesis_rate", {0.0, 0.1}},
        {"pruning_rate", {0.0, 0.1}},
        {"simulation_timestep", {0.001, 1.0}},
        {"random_seed", {0, 2147483647}}
    };
    
    return ranges;
}

const std::map<std::string, std::vector<std::string>> AdvancedConfig::getAllowedValues() {
    static const std::map<std::string, std::vector<std::string>> allowed = {
        {"environment_name", {"GridWorld", "Maze", "Network"}},
        {"log_level", {"ERROR", "WARN", "INFO", "DEBUG"}},
        {"neuron_type", {"Excitatory", "Inhibitory", "Sensory", "Motor", "Modulatory", "Internal"}},
        {"development_stage", {"Initial", "CriticalPeriod", "Maturation", "Adult", "Aging"}},
        {"action_type", {"MoveForward", "MoveBackward", "TurnLeft", "TurnRight", "Interact", "Wait", "LookLeft", "LookRight"}}
    };
    
    return allowed;
}

std::vector<ValidationRule> AdvancedConfig::getValidationRules() {
    static const std::vector<ValidationRule> rules = {
        {"neuron_count", 
         [](const ConfigValue& val) { return std::get<int>(val) > 0 && std::get<int>(val) <= 10000; },
         "must be between 1 and 10000"},
        {"region_count",
         [](const ConfigValue& val) { return std::get<int>(val) > 0 && std::get<int>(val) <= 100; },
         "must be between 1 and 100"},
        {"connection_probability",
         [](const ConfigValue& val) { return std::get<double>(val) >= 0.0 && std::get<double>(val) <= 1.0; },
         "must be between 0.0 and 1.0"},
        {"plasticity_learning_rate",
         [](const ConfigValue& val) { return std::get<double>(val) >= 0.0 && std::get<double>(val) <= 1.0; },
         "must be between 0.0 and 1.0"},
        {"stdp_ltp_weight",
         [](const ConfigValue& val) { return std::get<double>(val) >= 0.0 && std::get<double>(val) <= 1.0; },
         "must be between 0.0 and 1.0"},
        {"stdp_ltd_weight",
         [](const ConfigValue& val) { return std::get<double>(val) >= 0.0 && std::get<double>(val) <= 1.0; },
         "must be between 0.0 and 1.0"},
        {"stdp_tau",
         [](const ConfigValue& val) { return std::get<double>(val) > 0.0 && std::get<double>(val) <= 1000.0; },
         "must be between 0.0 and 1000.0"},
        {"synaptogenesis_rate",
         [](const ConfigValue& val) { return std::get<double>(val) >= 0.0 && std::get<double>(val) <= 0.1; },
         "must be between 0.0 and 0.1"},
        {"pruning_rate",
         [](const ConfigValue& val) { return std::get<double>(val) >= 0.0 && std::get<double>(val) <= 0.1; },
         "must be between 0.0 and 0.1"},
        {"simulation_timestep",
         [](const ConfigValue& val) { return std::get<double>(val) >= 0.001 && std::get<double>(val) <= 1.0; },
         "must be between 0.001 and 1.0"},
        {"random_seed",
         [](const ConfigValue& val) { return std::get<int>(val) >= 0 && std::get<int>(val) <= 2147483647; },
         "must be between 0 and 2147483647"},
        {"log_level",
         [](const ConfigValue& val) {
             const std::string& level = std::get<std::string>(val);
             return level == "ERROR" || level == "WARN" || level == "INFO" || level == "DEBUG";
         },
         "must be ERROR, WARN, INFO, or DEBUG"}
    };
    
    return rules;
}

bool AdvancedConfig::validateValue(const std::string& key, const ConfigValue& value, std::string& error) const {
    auto ranges = getKeyRanges();
    auto allowed = getAllowedValues();
    
    // Check numeric ranges
    if (ranges.find(key) != ranges.end()) {
        const auto& range = ranges.at(key);
        double val = 0.0;
        
        if (std::holds_alternative<int>(value)) {
            val = std::get<int>(value);
        } else if (std::holds_alternative<double>(value)) {
            val = std::get<double>(value);
        }
        
        if (val < range.first || val > range.second) {
            error = "value " + std::to_string(val) + " is outside range [" + std::to_string(range.first) + ", " + std::to_string(range.second) + "]";
            return false;
        }
    }
    
    // Check allowed values
    if (allowed.find(key) != allowed.end()) {
        const auto& values = allowed.at(key);
        std::string valStr;
        
        if (std::holds_alternative<std::string>(value)) {
            valStr = std::get<std::string>(value);
        }
        
        bool found = false;
        for (const auto& allowedVal : values) {
            if (valStr == allowedVal) {
                found = true;
                break;
            }
        }
        
        if (!found) {
            error = "value '" + valStr + "' is not in allowed list";
            return false;
        }
    }
    
    return true;
}

bool AdvancedConfig::applyPreset(const std::map<std::string, ConfigValue>& presetValues, bool validate) {
    for (const auto& pair : presetValues) {
        coreConfig_.set(pair.first, pair.second, ConfigSource::Preset);
    }
    
    if (validate && !validate()) {
        coreConfig_.clear();
        return false;
    }
    
    return true;
}

std::string AdvancedConfig::envVarToKey(const std::string& envVar) const {
    // Convert NLM_PREFIX to snake_case
    std::string key = envVar.substr(strlen(ENV_PREFIX));
    
    // Replace underscores and convert to lowercase
    for (size_t i = 0; i < key.size(); ++i) {
        if (key[i] == '_') {
            key[i] = '_';
        }
    }
    
    // Convert to lowercase
    std::transform(key.begin(), key.end(), key.begin(), ::tolower);
    
    return key;
}

// Helper function to get preset values
std::map<std::string, ConfigValue> AdvancedConfig::getPresetValues(Preset preset) {
    static const std::map<Preset, std::map<std::string, ConfigValue>> presets = {
        {Preset::Default, {
            {"neuron_count", 1000},
            {"region_count", 2},
            {"connection_probability", 0.1},
            {"plasticity_learning_rate", 0.01},
            {"stdp_ltp_weight", 0.01},
            {"stdp_ltd_weight", 0.012},
            {"stdp_tau", 20.0},
            {"synaptogenesis_rate", 0.001},
            {"pruning_rate", 0.0001},
            {"dopamine_baseline", 0.1},
            {"reward_discount_factor", 0.99},
            {"environment_name", "GridWorld"},
            {"environment_width", 10},
            {"environment_height", 10},
            {"log_level", "INFO"},
            {"log_to_file", false},
            {"log_filename", "nlm.log"},
            {"max_simulation_steps", 10000},
            {"simulation_time_limit", 0.0},
            {"visualization_enabled", false},
            {"visualization_update_rate", 30.0}
        }},
        {Preset::Phase2, {
            {"neuron_count", 500},
            {"region_count", 1},
            {"connection_probability", 0.15},
            {"plasticity_learning_rate", 0.01},
            {"stdp_ltp_weight", 0.02},
            {"stdp_ltd_weight", 0.015},
            {"stdp_tau", 20.0},
            {"synaptogenesis_rate", 0.0001},
            {"pruning_rate", 0.00001},
            {"simulation_timestep", 0.001}
        }},
        {Preset::Phase6, {
            {"neuron_count", 500},
            {"region_count", 2},
            {"connection_probability", 0.1},
            {"plasticity_learning_rate", 0.01},
            {"stdp_ltp_weight", 0.01},
            {"stdp_ltd_weight", 0.012},
            {"stdp_tau", 20.0},
            {"synaptogenesis_rate", 0.0001},
            {"pruning_rate", 0.00001},
            {"enable_checkpointing", true},
            {"enable_replay", true},
            {"enable_development", true},
            {"checkpoint_path", "./checkpoint_test.bin"}
        }},
        {Preset::Performance, {
            {"neuron_count", 200},
            {"region_count", 1},
            {"connection_probability", 0.05},
            {"plasticity_learning_rate", 0.005},
            {"stdp_ltp_weight", 0.005},
            {"stdp_ltd_weight", 0.006},
            {"stdp_tau", 15.0},
            {"synaptogenesis_rate", 0.00001},
            {"pruning_rate", 0.000001},
            {"simulation_timestep", 0.001},
            {"max_simulation_steps", 1000}
        }},
        {Preset::Development, {
            {"neuron_count", 1000},
            {"region_count", 3},
            {"connection_probability", 0.2},
            {"plasticity_learning_rate", 0.05},
            {"stdp_ltp_weight", 0.05},
            {"stdp_ltd_weight", 0.06},
            {"stdp_tau", 30.0},
            {"synaptogenesis_rate", 0.01},
            {"pruning_rate", 0.01},
            {"enable_development", true},
            {"enable_curiosity", true},
            {"enable_novelty", true}
        }},
        {Preset::Production, {
            {"neuron_count", 500},
            {"region_count", 2},
            {"connection_probability", 0.1},
            {"plasticity_learning_rate", 0.01},
            {"stdp_ltp_weight", 0.01},
            {"stdp_ltd_weight", 0.012},
            {"stdp_tau", 20.0},
            {"synaptogenesis_rate", 0.0001},
            {"pruning_rate", 0.00001},
            {"log_level", "WARN"},
            {"log_to_file", true},
            {"log_filename", "nlm_production.log"},
            {"enable_checkpointing", true},
            {"enable_replay", true}
        }}
    };
    
    return presets.at(preset);
}

} // namespace nlm
