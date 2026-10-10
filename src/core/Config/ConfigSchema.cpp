#include "ConfigSchema.hpp"
#include "Config.hpp"
#include <iostream>
#include <iomanip>
#include <algorithm>

namespace nlm {

ConfigSchema::ConfigSchema() {
    initializeSchema();
}

void ConfigSchema::addEntry(const ConfigSchemaEntry& entry) {
    entries[entry.name] = entry;
}

ConfigSchemaEntry ConfigSchema::getEntry(const std::string& name) const {
    auto it = entries.find(name);
    if (it != entries.end()) {
        return it->second;
    }
    throw std::runtime_error("Schema entry not found: " + name);
}

bool ConfigSchema::hasEntry(const std::string& name) const {
    return entries.find(name) != entries.end();
}

std::vector<std::string> ConfigSchema::getEntryNames() const {
    std::vector<std::string> names;
    names.reserve(entries.size());
    for (const auto& pair : entries) {
        names.push_back(pair.first);
    }
    return names;
}

std::vector<ConfigSchemaEntry> ConfigSchema::getAllEntries() const {
    std::vector<ConfigSchemaEntry> allEntries;
    allEntries.reserve(entries.size());
    for (const auto& pair : entries) {
        allEntries.push_back(pair.second);
    }
    return allEntries;
}

std::vector<ConfigSchemaEntry> ConfigSchema::getEntriesByType(ConfigType type) const {
    std::vector<ConfigSchemaEntry> typeEntries;
    for (const auto& pair : entries) {
        if (pair.second.type == type) {
            typeEntries.push_back(pair.second);
        }
    }
    return typeEntries;
}

std::vector<ConfigSchemaEntry> ConfigSchema::getEntriesByCategory(const std::string& category) const {
    std::vector<ConfigSchemaEntry> categoryEntries;
    
    for (const auto& pair : entries) {
        const auto& entry = pair.second;
        // Extract category from key (e.g., "neuron_count" -> "network")
        std::string cat;
        if (entry.name.find("neuron") != std::string::npos) {
            cat = "network";
        } else if (entry.name.find("region") != std::string::npos) {
            cat = "network";
        } else if (entry.name.find("timestep") != std::string::npos || 
                   entry.name.find("simulation") != std::string::npos) {
            cat = "simulation";
        } else if (entry.name.find("weight") != std::string::npos || 
                   entry.name.find("stdp") != std::string::npos) {
            cat = "plasticity";
        } else if (entry.name.find("rate") != std::string::npos) {
            cat = "development";
        } else if (entry.name.find("seed") != std::string::npos) {
            cat = "random";
        } else if (entry.name.find("enable_") != std::string::npos || 
                   entry.name.find("disable_") != std::string::npos) {
            cat = "feature";
        } else if (entry.name.find("connection") != std::string::npos) {
            cat = "connectivity";
        } else {
            cat = "general";
        }
        
        if (cat == category) {
            categoryEntries.push_back(entry);
        }
    }
    
    return categoryEntries;
}

bool ConfigSchema::validate(const std::string& key, const ConfigValue& value) const {
    if (!hasEntry(key)) {
        return true; // Allow unknown keys
    }
    
    const auto& entry = entries.at(key);
    return entry.isValid(value);
}

bool ConfigSchema::validate(const std::map<std::string, ConfigValue>& config) const {
    for (const auto& pair : config) {
        if (!validate(pair.first, pair.second)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> ConfigSchema::getSuggestions(const std::string& partialKey) const {
    std::vector<std::string> suggestions;
    
    for (const auto& pair : entries) {
        if (pair.first.find(partialKey) != std::string::npos) {
            suggestions.push_back(pair.first);
        }
    }
    
    return suggestions;
}

std::string ConfigSchema::getSummary() const {
    std::ostringstream oss;
    oss << "Configuration Schema Summary:" << std::endl;
    oss << "  Total entries: " << entries.size() << std::endl;
    
    // Count by type
    std::map<ConfigType, int> typeCounts;
    for (const auto& pair : entries) {
        typeCounts[pair.second.type]++;
    }
    
    oss << "  By type:" << std::endl;
    oss << "    integer: " << (typeCounts[ConfigType::Integer] + typeCounts[ConfigType::Integer64]) << std::endl;
    oss << "    float: " << typeCounts[ConfigType::Float] << std::endl;
    oss << "    boolean: " << typeCounts[ConfigType::Boolean] << std::endl;
    oss << "    string: " << typeCounts[ConfigType::String] << std::endl;
    oss << "    vectors: " << (typeCounts[ConfigType::IntVector] + typeCounts[ConfigType::FloatVector] + typeCounts[ConfigType::StringVector]) << std::endl;
    
    // Count by category
    std::map<std::string, int> categoryCounts;
    for (const auto& pair : entries) {
        const auto& entry = pair.second;
        std::string category;
        if (entry.name.find("neuron") != std::string::npos) {
            category = "network";
        } else if (entry.name.find("region") != std::string::npos) {
            category = "network";
        } else if (entry.name.find("timestep") != std::string::npos || 
                   entry.name.find("simulation") != std::string::npos) {
            category = "simulation";
        } else if (entry.name.find("weight") != std::string::npos || 
                   entry.name.find("stdp") != std::string::npos) {
            category = "plasticity";
        } else if (entry.name.find("rate") != std::string::npos) {
            category = "development";
        } else if (entry.name.find("seed") != std::string::npos) {
            category = "random";
        } else if (entry.name.find("enable_") != std::string::npos || 
                   entry.name.find("disable_") != std::string::npos) {
            category = "feature";
        } else if (entry.name.find("connection") != std::string::npos) {
            category = "connectivity";
        } else {
            category = "general";
        }
        categoryCounts[category]++;
    }
    
    oss << "  By category:" << std::endl;
    for (const auto& pair : categoryCounts) {
        oss << "    " << pair.first << ": " << pair.second << std::endl;
    }
    
    return oss.str();
}

std::string ConfigSchema::getDescription(const std::string& key) const {
    if (hasEntry(key)) {
        return entries.at(key).description;
    }
    return "Key not found in schema: " + key;
}

ConfigValue ConfigSchema::getDefaultValue(const std::string& key) const {
    if (hasEntry(key)) {
        return entries.at(key).defaultValue;
    }
    throw std::runtime_error("Key not found in schema: " + key);
}

bool ConfigSchema::isRequired(const std::string& key) const {
    if (hasEntry(key)) {
        return entries.at(key).required;
    }
    return false;
}

bool ConfigSchema::isExperimental(const std::string& key) const {
    if (hasEntry(key)) {
        return entries.at(key).experimental;
    }
    return false;
}

ConfigSchema ConfigSchema::createDefaultSchema() {
    ConfigSchema schema;
    
    // Core network parameters
    schema.addEntry(ConfigSchemaEntry("neuron_count", ConfigType::Integer64,
                                     "Total number of neurons in the network",
                                     500LL, true, false)
        .setValidValues({50LL, 500LL, 1000LL, 5000LL, 10000LL}));
    
    schema.addEntry(ConfigSchemaEntry("region_count", ConfigType::Integer64,
                                     "Number of neural regions/regions",
                                     2LL, true, false)
        .setValidValues({1LL, 2LL, 3LL, 4LL, 5LL, 10LL}));
    
    schema.addEntry(ConfigSchemaEntry("connection_probability", ConfigType::Float,
                                     "Probability of synaptic connections between neurons",
                                     0.15, true, false)
        .setRange(0.01, 1.0));
    
    // Simulation parameters
    schema.addEntry(ConfigSchemaEntry("simulation_timestep", ConfigType::Float,
                                     "Time step for simulation updates",
                                     0.001, true, false)
        .setRange(0.0001, 0.1));
    
    // Random seed
    schema.addEntry(ConfigSchemaEntry("random_seed", ConfigType::Integer64,
                                     "Random seed for reproducible experiments",
                                     42LL, true, false)
        .setValidValues({0LL, 1LL, 42LL, 123LL, 456LL, 999LL}));
    
    // STDP parameters
    schema.addEntry(ConfigSchemaEntry("stdp_ltp_weight", ConfigType::Float,
                                     "Long-term potentiation weight for STDP",
                                     0.02, true, false)
        .setRange(0.0, 1.0)
        .setValidValues({0.005f, 0.01f, 0.02f, 0.03f, 0.05f}));
    
    schema.addEntry(ConfigSchemaEntry("stdp_ltd_weight", ConfigType::Float,
                                     "Long-term depression weight for STDP",
                                     0.015, true, false)
        .setRange(0.0, 1.0)
        .setValidValues({0.01f, 0.015f, 0.02f, 0.025f}));
    
    schema.addEntry(ConfigSchemaEntry("stdp_tau", ConfigType::Float,
                                     "Time constant for STDP",
                                     20.0, true, false)
        .setRange(1.0, 100.0)
        .setValidValues({5.0f, 10.0f, 15.0f, 20.0f, 50.0f}));
    
    // Development parameters
    schema.addEntry(ConfigSchemaEntry("synaptogenesis_rate", ConfigType::Float,
                                     "Rate of new synapse formation",
                                     0.0001, true, false)
        .setRange(0.00001, 0.001)
        .setValidValues({0.00001f, 0.00005f, 0.0001f, 0.0005f, 0.001f}));
    
    schema.addEntry(ConfigSchemaEntry("pruning_rate", ConfigType::Float,
                                     "Rate of synapse elimination",
                                     0.00001, true, false)
        .setRange(0.000001, 0.0001)
        .setValidValues({0.000001f, 0.000005f, 0.00001f, 0.00005f, 0.0001f}));
    
    // Feature flags
    schema.addEntry(ConfigSchemaEntry("enable_reward_modulation", ConfigType::Boolean,
                                     "Enable reward modulation learning",
                                     true, false, false));
    
    schema.addEntry(ConfigSchemaEntry("enable_structural_plasticity", ConfigType::Boolean,
                                     "Enable structural plasticity (synaptogenesis/pruning)",
                                     true, false, false));
    
    schema.addEntry(ConfigSchemaEntry("enable_development", ConfigType::Boolean,
                                     "Enable developmental processes",
                                     true, false, false));
    
    schema.addEntry(ConfigSchemaEntry("enable_curiosity", ConfigType::Boolean,
                                     "Enable curiosity-driven exploration",
                                     true, false, false));
    
    schema.addEntry(ConfigSchemaEntry("enable_prediction_error", ConfigType::Boolean,
                                     "Enable prediction error learning",
                                     true, false, false));
    
    // Environment parameters
    schema.addEntry(ConfigSchemaEntry("environment_width", ConfigType::Integer,
                                     "Width of the environment",
                                     20, false, false)
        .setValidValues({10, 20, 40, 80, 100}));
    
    schema.addEntry(ConfigSchemaEntry("environment_height", ConfigType::Integer,
                                     "Height of the environment",
                                     20, false, false)
        .setValidValues({10, 20, 40, 80, 100}));
    
    schema.addEntry(ConfigSchemaEntry("vision_width", ConfigType::Integer,
                                     "Vision sensor width",
                                     16, false, false)
        .setValidValues({8, 16, 32, 64}));
    
    schema.addEntry(ConfigSchemaEntry("vision_height", ConfigType::Integer,
                                     "Vision sensor height",
                                     16, false, false)
        .setValidValues({8, 16, 32, 64}));
    
    return schema;
}

std::map<std::string, ConfigValue> ConfigSchema::createConfigFromSchema(const ConfigSchema& schema) {
    std::map<std::string, ConfigValue> config;
    
    for (const auto& pair : schema.getAllEntries()) {
        const auto& entry = pair.second;
        config[entry.name] = entry.defaultValue;
    }
    
    return config;
}

void ConfigSchema::initializeSchema() {
    *this = createDefaultSchema();
}

} // namespace nlm