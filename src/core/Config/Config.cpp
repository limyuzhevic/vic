#include "Config.hpp"
#include "../core/Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <chrono>

#ifdef JSON_SUPPORT
#include "nlohmann/json.hpp"
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> typedValueCache;
    bool autoSave;
    std::string autoSavePath;
    uint64_t modificationCounter;
    
    Impl() : autoSave(false), modificationCounter(0) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Add built-in defaults for known configuration parameters
    addBuiltInDefaults();
}

Config::~Config() = default;

void Config::addBuiltInDefaults() {
    // Core simulation parameters
    set("random_seed", 42, ConfigSource::Default, "Random seed for reproducible experiments");
    set("simulation_timestep", 0.001, ConfigSource::Default, "Simulation timestep in seconds");
    set("neuron_count", 1000, ConfigSource::Default, "Number of neurons in the brain");
    set("region_count", 1, ConfigSource::Default, "Number of brain regions");
    set("connection_probability", 0.1, ConfigSource::Default, "Connection probability for random connectivity");
    
    // Plasticity parameters
    set("plasticity_learning_rate", 0.01, ConfigSource::Default, "Global plasticity learning rate");
    set("stdp_ltp_weight", 0.02f, ConfigSource::Default, "STDP long-term potentiation weight");
    set("stdp_ltd_weight", 0.015f, ConfigSource::Default, "STDP long-term depression weight");
    set("stdp_tau", 20.0f, ConfigSource::Default, "STDP time constant");
    set("hebbian_learning_rate", 0.001f, ConfigSource::Default, "Hebbian learning rate");
    
    // Development parameters
    set("synaptogenesis_rate", 0.0001f, ConfigSource::Default, "Synaptogenesis rate");
    set("pruning_rate", 0.00001f, ConfigSource::Default, "Pruning rate");
    set("developmental_stage_duration", 100.0, ConfigSource::Default, "Duration of each developmental stage");
    
    // Neuromodulation parameters
    set("dopamine_baseline", 0.0f, ConfigSource::Default, "Baseline dopamine level");
    set("dopamine_sensitivity", 1.0f, ConfigSource::Default, "Dopamine sensitivity factor");
    set("curiosity_sensitivity", 0.5f, ConfigSource::Default, "Curiosity sensitivity factor");
    set("novelty_threshold", 0.1f, ConfigSource::Default, "Novelty detection threshold");
    
    // Reward and prediction parameters
    set("reward_discount_factor", 0.95f, ConfigSource::Default, "Reward discount factor (gamma)");
    set("prediction_error_decay", 0.9f, ConfigSource::Default, "Prediction error decay rate");
    set("reward_prediction_error_precision", 0.1f, ConfigSource::Default, "Precision of reward prediction error");
    
    // Memory system parameters
    set("working_memory_capacity", 100, ConfigSource::Default, "Working memory capacity (number of traces)");
    set("episodic_memory_max_episodes", 1000, ConfigSource::Default, "Maximum number of episodes to store");
    set("episodic_memory_decay_rate", 0.01f, ConfigSource::Default, "Episodic memory decay rate");
    set("associative_memory_association_strength", 0.5f, ConfigSource::Default, "Associative memory association strength");
    
    // Cognitive parameters
    set("attention_inhibition_strength", 0.5f, ConfigSource::Default, "Attention inhibition strength");
    set("attention_excitation_strength", 1.5f, ConfigSource::Default, "Attention excitation strength");
    set("planning_depth", 5, ConfigSource::Default, "Planning depth for neural planner");
    set("concept_formation_threshold", 0.3f, ConfigSource::Default, "Threshold for concept formation");
    
    // Development parameters
    set("development_enabled", true, ConfigSource::Default, "Whether development system is enabled");
    set("structural_plasticity_enabled", true, ConfigSource::Default, "Whether structural plasticity is enabled");
    set("reward_modulation_enabled", true, ConfigSource::Default, "Whether reward modulation is enabled");
    set("curiosity_enabled", true, ConfigSource::Default, "Whether curiosity-driven exploration is enabled");
    
    // System parameters
    set("replay_interval", 100, ConfigSource::Default, "Interval (steps) between memory replay");
    set("consolidation_interval", 1000, ConfigSource::Default, "Interval (steps) between memory consolidation");
    set("max_simulation_steps", 10000, ConfigSource::Default, "Maximum number of simulation steps");
    set("simulation_time_limit", 0.0, ConfigSource::Default, "Time limit for simulation (0 = unlimited)");
    
    // Visualization parameters
    set("visualization_enabled", false, ConfigSource::Default, "Whether visualization is enabled");
    set("visualization_update_rate", 30.0f, ConfigSource::Default, "Visualization update rate in Hz");
    set("visualization_data_points", 1000, ConfigSource::Default, "Number of data points for visualization");
    
    // Logging parameters
    set("log_level", "INFO", ConfigSource::Default, "Log level (DEBUG, INFO, WARNING, ERROR, CRITICAL)");
    set("log_to_file", false, ConfigSource::Default, "Whether to log to file");
    set("log_filename", "nlm.log", ConfigSource::Default, "Log filename");
    set("log_max_size", 10485760, ConfigSource::Default, "Maximum log file size in bytes (10MB)");
    
    // Performance parameters
    set("performance_monitoring_enabled", true, ConfigSource::Default, "Whether performance monitoring is enabled");
    set("performance_sample_interval", 1000, ConfigSource::Default, "Performance sampling interval in steps");
    set("memory_pool_size", 1000000, ConfigSource::Default, "Size of memory pool for allocations");
    set("event_queue_size", 10000, ConfigSource::Default, "Size of event queue");
}

bool Config::loadFromFile(const std::string& filepath) {
    #ifdef JSON_SUPPORT
    // Try JSON parsing first
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return loadFromLegacyFormat(filepath);
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        bool success = true;
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            const std::string& key = it.key();
            const nlohmann::json& value = it.value();
            
            try {
                if (value.is_string()) {
                    set(key, value.get<std::string>(), ConfigSource::File, "Parsed from JSON");
                } else if (value.is_number_integer()) {
                    set(key, value.get<int64_t>(), ConfigSource::File, "Parsed from JSON");
                } else if (value.is_number_float()) {
                    set(key, value.get<double>(), ConfigSource::File, "Parsed from JSON");
                } else if (value.is_boolean()) {
                    set(key, value.get<bool>(), ConfigSource::File, "Parsed from JSON");
                } else if (value.is_array()) {
                    // Handle arrays
                    std::vector<std::string> arrayValues;
                    for (const auto& item : value) {
                        arrayValues.push_back(item.get<std::string>());
                    }
                    set(key, arrayValues, ConfigSource::File, "Parsed from JSON array");
                } else {
                    // Try to parse as string for unknown types
                    set(key, value.dump(), ConfigSource::File, "Parsed from JSON (dump)");
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to parse JSON key '" + key + "': " + e.what());
                success = false;
            }
        }
        
        return success;
    } catch (const std::exception& e) {
        // Fall back to legacy format
        return loadFromLegacyFormat(filepath);
    }
    #else
    return loadFromLegacyFormat(filepath);
    #endif
}

bool Config::loadFromLegacyFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        line = trim(line);
        
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Remove trailing comments
            size_t commentPos = value.find('#');
            if (commentPos != std::string::npos) {
                value = value.substr(0, commentPos);
                value = trim(value);
            }
            
            try {
                setFromString(key, value, ConfigSource::File);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error parsing config file " + filepath + ":" + std::to_string(lineNumber) + 
                            " key '" + key + "': " + e.what());
                continue;
            }
        } else {
            NLM_LOG_WARNING("Invalid config line (no '=') in " + filepath + ":" + std::to_string(lineNumber));
        }
    }
    
    return true;
}

void Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Try to parse value based on key hints
    std::string keyLower = toLower(key);
    
    if (keyLower.find("bool") != std::string::npos || keyLower == "enabled" || keyLower == "disabled" || 
        keyLower == "true" || keyLower == "false") {
        // Boolean value
        bool boolVal = (valueStr == "true" || valueStr == "1" || valueStr == "yes" || 
                       valueStr == "on" || valueStr == "enabled");
        set(key, boolVal, source, "Parsed from config");
    } else if (keyLower.find("int") != std::string::npos || keyLower.find("count") != std::string::npos || 
               keyLower.find("step") != std::string::npos) {
        // Integer value
        int intVal = std::stoi(valueStr);
        set(key, intVal, source, "Parsed from config");
    } else if (keyLower.find("float") != std::string::npos || keyLower.find("rate") != std::string::npos || 
               keyLower.find("weight") != std::string::npos || keyLower.find("time") != std::string::npos || 
               keyLower.find("delay") != std::string::npos) {
        // Float value
        double floatVal = std::stod(valueStr);
        set(key, floatVal, source, "Parsed from config");
    } else {
        // Default to string
        set(key, valueStr, source, "Parsed from config");
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                setFromString(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Invalid command line argument format: " + arg);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            setFromString(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open config file for writing: " + filepath);
        return false;
    }
    
    #ifdef JSON_SUPPORT
    // Save as JSON
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&](const auto& val) {
            jsonData[entry.key] = val;
        }, entry.value);
    }
    
    file << jsonData.dump(2);
    #else
    // Save as legacy format
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else {
                file << val;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    #endif
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Type mismatch for key '" + key + "': expected " + typeid(T).name() + 
                     " but got " + getVariantTypeName(it->value));
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        if (!description.empty() && it->description.empty()) {
            it->description = description;
        }
    } else {
        pImpl->entries.emplace_back(key, value, source, description.empty() ? ("Auto-generated from set()") : description);
    }
    
    pImpl->modificationCounter++;
    
    // If auto-save is enabled, save the config
    if (pImpl->autoSave) {
        saveToFile(pImpl->autoSavePath);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    pImpl->modificationCounter++;
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->modificationCounter++;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " << pImpl->modificationCounter 
        << " modifications):\n";
    
    // Group entries by source
    std::vector<const ConfigEntry*> defaultEntries;
    std::vector<const ConfigEntry*> fileEntries;
    std::vector<const ConfigEntry*> runtimeEntries;
    
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default:
                defaultEntries.push_back(&entry);
                break;
            case ConfigSource::File:
                fileEntries.push_back(&entry);
                break;
            case ConfigSource::CommandLine:
            case ConfigSource::Runtime:
                runtimeEntries.push_back(&entry);
                break;
        }
    }
    
    if (!defaultEntries.empty()) {
        oss << "Default values:\n";
        for (const auto* entry : defaultEntries) {
            oss << "  " << entry->key << " = ";
            visitValue(oss, entry->value);
            oss << " (default)\n";
        }
    }
    
    if (!fileEntries.empty()) {
        oss << "\nFile values:\n";
        for (const auto* entry : fileEntries) {
            oss << "  " << entry->key << " = ";
            visitValue(oss, entry->value);
            oss << " (file)\n";
        }
    }
    
    if (!runtimeEntries.empty()) {
        oss << "\nRuntime values:\n";
        for (const auto* entry : runtimeEntries) {
            oss << "  " << entry->key << " = ";
            visitValue(oss, entry->value);
            oss << " (" << static_cast<int>(entry->source) << ")\n";
        }
    }
    
    return oss.str();
}

std::string Config::getVariantTypeName(const ConfigValue& value) const {
    std::string typeName;
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, int>) typeName = "int";
        else if constexpr (std::is_same_v<T, int64_t>) typeName = "int64_t";
        else if constexpr (std::is_same_v<T, double>) typeName = "double";
        else if constexpr (std::is_same_v<T, bool>) typeName = "bool";
        else if constexpr (std::is_same_v<T, std::string>) typeName = "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) typeName = "vector<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) typeName = "vector<double>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) typeName = "vector<string>";
        else typeName = "unknown";
    }, value);
    return typeName;
}

void Config::visitValue(std::ostringstream& oss, const ConfigValue& value) const {
    std::visit([&oss](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                oss << val[i];
                if (i < val.size() - 1) oss << ", ";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                oss << val[i];
                if (i < val.size() - 1) oss << ", ";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                oss << "\"" << val[i] << "\"";
                if (i < val.size() - 1) oss << ", ";
            }
            oss << "]";
        } else {
            oss << val;
        }
    }, value);
}

void Config::enableAutoSave(const std::string& filepath) {
    pImpl->autoSave = true;
    pImpl->autoSavePath = filepath;
}

void Config::disableAutoSave() {
    pImpl->autoSave = false;
}

uint64_t Config::getModificationCounter() const {
    return pImpl->modificationCounter;
}

std::vector<ConfigEntry> Config::exportConfig() const {
    return pImpl->entries;
}

void Config::importConfig(const std::vector<ConfigEntry>& entries) {
    clear();
    pImpl->entries = entries;
    pImpl->modificationCounter++;
}

bool Config::validateConfig() const {
    bool isValid = true;
    
    // Check for required keys
    const std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count"
    };
    
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            NLM_LOG_WARNING("Missing required config key: " + key);
            isValid = false;
        }
    }
    
    // Validate specific value ranges
    auto neuronCount = getOr<size_t>("neuron_count", 1000);
    if (neuronCount == 0 || neuronCount > 100000) {
        NLM_LOG_WARNING("Invalid neuron_count: " + std::to_string(neuronCount));
        isValid = false;
    }
    
    auto timestep = getOr<double>("simulation_timestep", 0.001);
    if (timestep <= 0.0 || timestep > 1.0) {
        NLM_LOG_WARNING("Invalid simulation_timestep: " + std::to_string(timestep));
        isValid = false;
    }
    
    return isValid;
}

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

} // namespace nlm
