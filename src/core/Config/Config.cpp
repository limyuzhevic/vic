#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <chrono>
#include <ctime>

// JSON include
#include <nlohmann/json.hpp>

// YAML include
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::string> groupMap;
    std::map<std::string, ConfigValue> defaultValues;
    std::vector<std::pair<std::string, std::pair<ConfigValue, std::chrono::system_clock::time_point>>> scheduledUpdates;
};

// Initialize static members
std::vector<ConfigSchemaEntry> Config::schema_;
bool Config::schemaInitialized_ = false;
std::map<std::string, std::string> Config::keyToGroup_;
std::map<std::string, ConfigValue> Config::defaultValues_;
std::vector<std::pair<std::string, std::pair<ConfigValue, std::chrono::system_clock::time_point>>> Config::scheduledUpdates_;

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeSchema() {
    if (schemaInitialized_) return;
    
    // Initialize with typical NLM configuration parameters
    schema_.push_back(ConfigSchemaEntry("neuron_count", "int64_t", 
        "Number of neurons in the network", int64_t(1000), "count"));
    
    schema_.push_back(ConfigSchemaEntry("region_count", "int64_t",
        "Number of neural regions", int64_t(1), "count"));
    
    schema_.push_back(ConfigSchemaEntry("connection_probability", "double",
        "Probability of connection between neurons", 0.1, "probability"));
    
    schema_.push_back(ConfigSchemaEntry("simulation_timestep", "double",
        "Time step for simulation in seconds", 0.001, "seconds"));
    
    schema_.push_back(ConfigSchemaEntry("random_seed", "int64_t",
        "Random seed for reproducible simulations", int64_t(42), "count"));
    
    schema_.push_back(ConfigSchemaEntry("stdp_ltp_weight", "double",
        "STDP LTP weight factor", 0.01, "weight"));
    
    schema_.push_back(ConfigSchemaEntry("stdp_ltd_weight", "double",
        "STDP LTD weight factor", 0.012, "weight"));
    
    schema_.push_back(ConfigSchemaEntry("stdp_tau", "double",
        "STDP time constant", 20.0, "ms"));
    
    schema_.push_back(ConfigSchemaEntry("synaptogenesis_rate", "double",
        "Rate of synapse formation per step", 0.0001, "per_step"));
    
    schema_.push_back(ConfigSchemaEntry("pruning_rate", "double",
        "Rate of synapse pruning per step", 0.00001, "per_step"));
    
    schema_.push_back(ConfigSchemaEntry("enable_checkpointing", "bool",
        "Enable checkpoint saving", true, "enabled"));
    
    schema_.push_back(ConfigSchemaEntry("checkpoint_path", "string",
        "Path for checkpoint files", std::string("./checkpoint_test.bin"), "path"));
    
    schema_.push_back(ConfigSchemaEntry("enable_replay", "bool",
        "Enable memory replay during simulation", true, "enabled"));
    
    schema_.push_back(ConfigSchemaEntry("enable_development", "bool",
        "Enable developmental plasticity", true, "enabled"));
    
    schema_.push_back(ConfigSchemaEntry("max_steps", "int64_t",
        "Maximum simulation steps", int64_t(10000), "steps"));
    
    schema_.push_back(ConfigSchemaEntry("target_firing_rate", "double",
        "Target firing rate for neurons", 10.0, "Hz"));
    
    schemaInitialized_ = true;
}

ValidationResult Config::validateAllImpl() {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult singleResult = validateSingleImpl(entry);
        if (!singleResult.valid) {
            result.valid = false;
            result.errorMessage += "Key \"" + entry.key + "\": " + singleResult.errorMessage + "; ";
        }
        
        for (const auto& warning : singleResult.warnings) {
            result.warnings.push_back("Key \"" + entry.key + "\": " + warning);
        }
    }
    
    return result;
}

ValidationResult Config::validateSingleImpl(const ConfigEntry& entry) {
    ValidationResult result;
    
    // Check if key exists in schema
    auto schemaIt = std::find_if(schema_.begin(), schema_.end(),
        [&entry](const ConfigSchemaEntry& s) { return s.key == entry.key; });
    
    if (schemaIt == schema_.end()) {
        // Not in schema - this might be okay for extensibility
        result.warnings.push_back("Key not defined in schema");
        return result;
    }
    
    const auto& schema = *schemaIt;
    
    // Type validation
    bool typeMatches = false;
    if (schema.type == "int" && std::holds_alternative<int>(entry.value)) {
        typeMatches = true;
    } else if (schema.type == "int64_t" && std::holds_alternative<int64_t>(entry.value)) {
        typeMatches = true;
    } else if (schema.type == "double" && std::holds_alternative<double>(entry.value)) {
        typeMatches = true;
    } else if (schema.type == "bool" && std::holds_alternative<bool>(entry.value)) {
        typeMatches = true;
    } else if (schema.type == "string" && std::holds_alternative<std::string>(entry.value)) {
        typeMatches = true;
    } else if (schema.type == "vector<int>" && std::holds_alternative<std::vector<int>>(entry.value)) {
        typeMatches = true;
    } else if (schema.type == "vector<double>" && std::holds_alternative<std::vector<double>>(entry.value)) {
        typeMatches = true;
    } else if (schema.type == "vector<string>" && std::holds_alternative<std::vector<std::string>>(entry.value)) {
        typeMatches = true;
    }
    
    if (!typeMatches) {
        result.valid = false;
        result.errorMessage = "Type mismatch: expected " + schema.type + ", got ";
        if (std::holds_alternative<int>(entry.value)) result.errorMessage += "int";
        else if (std::holds_alternative<int64_t>(entry.value)) result.errorMessage += "int64_t";
        else if (std::holds_alternative<double>(entry.value)) result.errorMessage += "double";
        else if (std::holds_alternative<bool>(entry.value)) result.errorMessage += "bool";
        else if (std::holds_alternative<std::string>(entry.value)) result.errorMessage += "string";
        else result.errorMessage += "unknown";
        return result;
    }
    
    // Custom validator if provided
    if (schema.validator) {
        ValidationResult customResult = schema.validator.value()(entry.value);
        if (!customResult.valid) {
            result.valid = false;
            result.errorMessage = customResult.errorMessage;
        }
        for (const auto& warning : customResult.warnings) {
            result.warnings.push_back(warning);
        }
    }
    
    // Range validation for numeric types
    if (schema.type == "int" || schema.type == "int64_t" || schema.type == "double") {
        double value = 0.0;
        if (std::holds_alternative<int>(entry.value)) {
            value = std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            value = static_cast<double>(std::get<int64_t>(entry.value));
        } else if (std::holds_alternative<double>(entry.value)) {
            value = std::get<double>(entry.value);
        }
        
        if (value < 0.0) {
            result.warnings.push_back("Negative value may cause issues");
        }
        
        if (schema.type == "double" && schema.unit == "probability" && (value < 0.0 || value > 1.0)) {
            result.warnings.push_back("Probability should be between 0.0 and 1.0");
        }
    }
    
    return result;
}

ValidationResult Config::validate(const std::string& key, const ConfigValue& value) {
    ValidationResult result;
    
    auto schemaIt = std::find_if(schema_.begin(), schema_.end(),
        [&key](const ConfigSchemaEntry& s) { return s.key == key; });
    
    if (schemaIt == schema_.end()) {
        result.warnings.push_back("Key not in schema - extensibility allowed");
        return result;
    }
    
    const auto& schema = *schemaIt;
    
    // Type check
    bool typeMatches = false;
    if (schema.type == "int" && std::holds_alternative<int>(value)) typeMatches = true;
    else if (schema.type == "int64_t" && std::holds_alternative<int64_t>(value)) typeMatches = true;
    else if (schema.type == "double" && std::holds_alternative<double>(value)) typeMatches = true;
    else if (schema.type == "bool" && std::holds_alternative<bool>(value)) typeMatches = true;
    else if (schema.type == "string" && std::holds_alternative<std::string>(value)) typeMatches = true;
    
    if (!typeMatches) {
        result.valid = false;
        result.errorMessage = "Type mismatch for key: " + key;
    }
    
    return result;
}

Config::Config(const std::vector<std::string>& keys) {
    for (const auto& key : keys) {
        set(key, 0, ConfigSource::Default);
    }
}

void Config::applyTimeBasedUpdates() {
    auto now = std::chrono::system_clock::now();
    
    for (auto it = pImpl->scheduledUpdates.begin(); it != pImpl->scheduledUpdates.end(); ) {
        if (now >= it->second.second) {
            // Apply the scheduled update
            set(it->first, it->second.first, ConfigSource::Runtime);
            it = pImpl->scheduledUpdates.erase(it);
        } else {
            ++it;
        }
    }
}

void Config::scheduleUpdate(const std::string& key, const ConfigValue& value,
                           const std::chrono::system_clock::time_point& time) {
    pImpl->scheduledUpdates.emplace_back(key, std::make_pair(value, time));
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, source);
    }
}

ConfigValue Config::getDefault(const std::string& key) const {
    auto it = defaultValues_.find(key);
    if (it != defaultValues_.end()) {
        return it->second;
    }
    return {};
}

bool Config::hasDefault(const std::string& key) const {
    return defaultValues_.find(key) != defaultValues_.end();
}

void Config::setGroup(const std::string& key, const std::string& group) {
    pImpl->groupMap[key] = group;
}

std::vector<std::string> Config::getGroups() const {
    std::vector<std::string> groups;
    for (const auto& pair : pImpl->groupMap) {
        if (std::find(groups.begin(), groups.end(), pair.second) == groups.end()) {
            groups.push_back(pair.second);
        }
    }
    return groups;
}

std::vector<std::string> Config::getKeysInGroup(const std::string& group) const {
    std::vector<std::string> keys;
    for (const auto& pair : pImpl->groupMap) {
        if (pair.second == group) {
            keys.push_back(pair.first);
        }
    }
    return keys;
}

std::string Config::getGroup(const std::string& key) const {
    auto it = pImpl->groupMap.find(key);
    if (it != pImpl->groupMap.end()) {
        return it->second;
    }
    return "";
}

bool Config::exportToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json json;
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        std::string valueStr = variantToString(entry.value);
        json[key] = valueStr;
    }
    
    file << json.dump(2);
    return true;
}

bool Config::importFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json json = nlohmann::json::parse(file);
        clear();
        
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            std::string valueStr = it.value();
            // Simple parsing - could be enhanced
            try {
                if (valueStr == "true" || valueStr == "false") {
                    set(key, valueStr == "true", ConfigSource::File);
                } else {
                    try {
                        size_t pos;
                        int64_t intVal = std::stoll(valueStr, &pos);
                        if (pos == valueStr.size()) {
                            set(key, intVal, ConfigSource::File);
                            continue;
                        }
                    } catch (...) {}
                    
                    try {
                        size_t pos;
                        double doubleVal = std::stod(valueStr, &pos);
                        if (pos == valueStr.size()) {
                            set(key, doubleVal, ConfigSource::File);
                            continue;
                        }
                    } catch (...) {}
                    
                    set(key, valueStr, ConfigSource::File);
                }
            } catch (...) {
                // Skip invalid entries
                continue;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::exportToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        emitter << YAML::Value << variantToString(entry.value);
    }
    
    emitter << YAML::EndMap;
    
    file << emitter.c_str();
    return true;
}

bool Config::importFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node doc = YAML::Load(file);
        clear();
        
        for (const auto& key : doc) {
            std::string keyStr = key.first.as<std::string>();
            std::string valueStr = key.second.as<std::string>();
            // Simple parsing - could be enhanced
            try {
                if (valueStr == "true" || valueStr == "false") {
                    set(keyStr, valueStr == "true", ConfigSource::File);
                } else {
                    try {
                        size_t pos;
                        int64_t intVal = std::stoll(valueStr, &pos);
                        if (pos == valueStr.size()) {
                            set(keyStr, intVal, ConfigSource::File);
                            continue;
                        }
                    } catch (...) {}
                    
                    try {
                        size_t pos;
                        double doubleVal = std::stod(valueStr, &pos);
                        if (pos == valueStr.size()) {
                            set(keyStr, doubleVal, ConfigSource::File);
                            continue;
                        }
                    } catch (...) {}
                    
                    set(keyStr, valueStr, ConfigSource::File);
                }
            } catch (...) {
                // Skip invalid entries
                continue;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

double Config::getMemoryUsage() const {
    double total = sizeof(*pImpl);
    for (const auto& entry : pImpl->entries) {
        total += sizeof(entry.key) + sizeof(entry.value) + sizeof(entry.description) + sizeof(entry.unit);
    }
    return total;
}

} // namespace nlm