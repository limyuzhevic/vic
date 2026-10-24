#include "ConfigSchema.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iomanip>

namespace nlm {

// Global configuration schema instance - implemented in config.cpp
ConfigSchema& getGlobalConfigSchema() {
    static ConfigSchema schema;
    return schema;
}

void ConfigSchema::addEntry(const SchemaEntry& entry) {
    entries_[entry.key] = entry;
}

bool ConfigSchema::validate(const std::string& key, const ConfigValue& value) const {
    auto it = entries_.find(key);
    if (it == entries_.end()) {
        return false;  // Key not in schema
    }
    
    const auto& schema = it->second;
    
    // Check type
    std::string actualType;
    if (std::holds_alternative<int>(value)) actualType = "int";
    else if (std::holds_alternative<int64_t>(value)) actualType = "int64_t";
    else if (std::holds_alternative<double>(value)) actualType = "double";
    else if (std::holds_alternative<bool>(value)) actualType = "bool";
    else if (std::holds_alternative<std::string>(value)) actualType = "string";
    else return false;
    
    if (actualType != schema.type && 
        !(schema.type == "int" && (actualType == "int64_t" || actualType == "int"))) {
        return false;
    }
    
    // Check allowed values
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
    
    // Check numeric bounds
    if (schema.type == "int" || schema.type == "int64_t" || schema.type == "double") {
        double val;
        if (std::holds_alternative<int>(value)) val = std::get<int>(value);
        else if (std::holds_alternative<int64_t>(value)) val = std::get<int64_t>(value);
        else if (std::holds_alternative<double>(value)) val = std::get<double>(value);
        else return false;
        
        if (val < schema.minValue || val > schema.maxValue) {
            return false;
        }
    }
    
    return true;
}

const ConfigSchema::SchemaEntry* ConfigSchema::getEntry(const std::string& key) const {
    auto it = entries_.find(key);
    if (it == entries_.end()) return nullptr;
    return &it->second;
}

std::vector<std::string> ConfigSchema::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(entries_.size());
    for (const auto& pair : entries_) {
        keys.push_back(pair.first);
    }
    return keys;
}

ConfigSchema::ConfigValue ConfigSchema::getDefault(const std::string& key) const {
    auto it = entries_.find(key);
    if (it == entries_.end()) {
        throw std::runtime_error("Key not found in schema: " + key);
    }
    return it->second.defaultValue;
}

void ConfigSchema::clear() {
    entries_.clear();
}

void ConfigSchema::merge(const ConfigSchema& other) {
    for (const auto& pair : other.entries_) {
        entries_[pair.first] = pair.second;
    }
}

std::string ConfigSchema::summary() const {
    std::stringstream oss;
    oss << "Configuration Schema (" << entries_.size() << " entries):\n";
    for (const auto& pair : entries_) {
        const auto& entry = pair.second;
        oss << "  " << entry.key << " (" << entry.type << ")";
        if (entry.required) oss << " [REQUIRED]";
        oss << "\n    Default: ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.defaultValue);
        
        oss << "\n    Description: " << entry.description << "\n";
        
        if (!entry.allowedValues.empty()) {
            oss << "    Allowed: [";
            for (size_t i = 0; i < entry.allowedValues.size(); ++i) {
                oss << entry.allowedValues[i];
                if (i < entry.allowedValues.size() - 1) oss << ", ";
            }
            oss << "]\n";
        }
        
        if (entry.minValue != std::numeric_limits<double>::lowest() || 
            entry.maxValue != std::numeric_limits<double>::max()) {
            oss << "    Range: [" << entry.minValue << ", " << entry.maxValue << "]\n";
        }
    }
    return oss.str();
}

void registerDefaultConfigSchema() {
    auto& schema = getGlobalConfigSchema();
    
    // Clear existing entries
    schema.clear();
    
    // Add default configuration schema
    std::vector<ConfigSchema::SchemaEntry> defaultEntries = {
        {"random_seed", "int64_t", int64_t(42), "Random seed for reproducible experiments", false},
        {"simulation_timestep", "double", 0.001, "Simulation timestep in seconds", false, {}, 0.0, 0.1},
        {"neuron_count", "int64_t", int64_t(1000), "Number of neurons in the brain", false, {}, 1, 10000},
        {"region_count", "int64_t", int64_t(2), "Number of brain regions", false, {}, 1, 10},
        {"connection_probability", "double", 0.1, "Connection probability for random connectivity", false, {}, 0.0, 1.0},
        {"plasticity_learning_rate", "double", 0.01, "Plasticity learning rate", false, {}, 0.0, 1.0},
        {"stdp_ltp_weight", "double", 0.01, "STDP LTP weight", false, {}, 0.0, 1.0},
        {"stdp_ltd_weight", "double", 0.012, "STDP LTD weight", false, {}, 0.0, 1.0},
        {"stdp_time_constant", "double", 20.0, "STDP time constant", false, {}, 1.0, 100.0},
        {"development_synaptogenesis_rate", "double", 0.001, "Development synaptogenesis rate", false, {}, 0.0, 0.1},
        {"development_pruning_rate", "double", 0.0001, "Development pruning rate", false, {}, 0.0, 0.01},
        {"dopamine_baseline", "double", 0.1, "Dopamine baseline level", false, {}, 0.0, 1.0},
        {"reward_discount_factor", "double", 0.99, "Reward discount factor", false, {}, 0.0, 1.0},
        {"environment_name", "string", std::string("GridWorld"), "Environment name", false},
        {"environment_width", "int64_t", int64_t(10), "Environment width", false, {}, 1, 100},
        {"environment_height", "int64_t", int64_t(10), "Environment height", false, {}, 1, 100},
        {"log_level", "string", std::string("INFO"), "Log level", false},
        {"log_to_file", "bool", false, "Log to file", false},
        {"log_filename", "string", std::string("nlm.log"), "Log filename", false},
        {"max_simulation_steps", "int64_t", int64_t(10000), "Maximum simulation steps", false, {}, 1, 1000000},
        {"simulation_time_limit", "double", 0.0, "Simulation time limit (0 = unlimited)", false, {}, 0.0, 1000.0},
        {"visualization_enabled", "bool", false, "Enable visualization", false},
        {"visualization_update_rate", "double", 30.0, "Visualization update rate (Hz)", false, {}, 1.0, 1000.0},
    };
    
    for (const auto& entry : defaultEntries) {
        schema.addEntry(entry);
    }
}

} // namespace nlm