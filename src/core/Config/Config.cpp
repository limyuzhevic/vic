#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <cmath>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default values based on improved configs/default.cfg
    initializeDefaults();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaults() {
    // Core simulation parameters
    set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    set("simulation_timestep", 0.001, ConfigSource::Default);
    set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    set("connection_probability", 0.1f, ConfigSource::Default);
    
    // Plasticity parameters
    set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    set("stdp_time_constant", 20.0f, ConfigSource::Default);
    set("plasticity_learning_rate", 0.01f, ConfigSource::Default);
    
    // Development parameters
    set("development_synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    set("development_pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Neuromodulation
    set("dopamine_baseline", 0.1f, ConfigSource::Default);
    
    // Reward processing
    set("reward_discount_factor", 0.99f, ConfigSource::Default);
    
    // Environment
    set("environment_name", std::string("GridWorld"), ConfigSource::Default);
    set("environment_width", static_cast<int64_t>(10), ConfigSource::Default);
    set("environment_height", static_cast<int64_t>(10), ConfigSource::Default);
    
    // Logging
    set("log_level", std::string("INFO"), ConfigSource::Default);
    set("log_to_file", false, ConfigSource::Default);
    set("log_filename", std::string("nlm.log"), ConfigSource::Default);
    
    // Simulation
    set("max_simulation_steps", static_cast<int64_t>(10000), ConfigSource::Default);
    set("simulation_time_limit", 0.0, ConfigSource::Default);
    
    // Visualization
    set("visualization_enabled", false, ConfigSource::Default);
    set("visualization_update_rate", 30.0f, ConfigSource::Default);
    
    // Advanced features
    set("enable_reward_modulation", true, ConfigSource::Default);
    set("enable_structural_plasticity", true, ConfigSource::Default);
    set("enable_development", true, ConfigSource::Default);
    set("enable_curiosity", true, ConfigSource::Default);
    set("enable_prediction", true, ConfigSource::Default);
    set("enable_working_memory", true, ConfigSource::Default);
    set("enable_episodic_memory", true, ConfigSource::Default);
    set("enable_associative_memory", true, ConfigSource::Default);
    
    // Working memory settings
    set("working_memory_capacity", 100.0f, ConfigSource::Default);
    set("working_memory_trace_decay", 0.95f, ConfigSource::Default);
    
    // Episodic memory settings
    set("episodic_memory_capacity", static_cast<int64_t>(100), ConfigSource::Default);
    set("episodic_memory_decay_rate", 0.01f, ConfigSource::Default);
    set("episodic_memory_replay_strength", 0.5f, ConfigSource::Default);
    
    // Prediction system
    set("prediction_horizon", static_cast<int64_t>(10), ConfigSource::Default);
    set("prediction_error_threshold", 0.1f, ConfigSource::Default);
    set("prediction_confidence_decay", 0.9f, ConfigSource::Default);
    
    // Curiosity settings
    set("curiosity_scaling", 0.5f, ConfigSource::Default);
    set("exploration_rate", 0.3f, ConfigSource::Default);
    set("novelty_threshold", 0.2f, ConfigSource::Default);
    
    // Development parameters
    set("critical_period_duration", 300.0, ConfigSource::Default);
    set("synaptogenesis_intensity", 0.0005f, ConfigSource::Default);
    set("pruning_efficiency", 0.2f, ConfigSource::Default);
    set("maturation_rate", 0.01f, ConfigSource::Default);
    
    // Checkpointing
    set("checkpoint_path", std::string("nlm_checkpoint.bin"), ConfigSource::Default);
    set("checkpoint_interval", static_cast<int64_t>(100), ConfigSource::Default);
    set("enable_checkpointing", true, ConfigSource::Default);
    
    // Replay system
    set("enable_replay", true, ConfigSource::Default);
    set("replay_frequency", 0.1f, ConfigSource::Default);
    set("replay_strength", 0.7f, ConfigSource::Default);
    
    // Additional useful defaults
    set("v_thresh", -50.0f, ConfigSource::Default);  // Neuron threshold
    set("v_rest", -70.0f, ConfigSource::Default);    // Resting potential
    set("v_reset", -75.0f, ConfigSource::Default);   // Reset potential
    set("tau_mem", 20.0f, ConfigSource::Default);    // Membrane time constant
    set("tau_ref", 2.0f, ConfigSource::Default);     // Refractory period
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON first
    try {
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        // Recursively load from JSON
        loadFromJson(jsonConfig, ConfigSource::File);
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        // If JSON parsing fails, try old key=value format
        file.clear();
        file.seekg(0);
        
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                setFromString(key, value, ConfigSource::File);
            }
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                setFromString(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
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
        return false;
    }
    
    // Create JSON object
    nlohmann::json jsonConfig;
    
    // Convert all config entries to JSON
    for (const auto& entry : pImpl->entries) {
        // Store with description as comment
        if (!entry.description.empty()) {
            jsonConfig[entry.key] = nlohmann::json::object({
                {"value", configValueToJson(entry.value)},
                {"source", static_cast<int>(entry.source)},
                {"description", entry.description}
            });
        } else {
            jsonConfig[entry.key] = configValueToJson(entry.value);
        }
    }
    
    // Write to file with indentation
    file << jsonConfig.dump(2);
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source, getDefaultDescription(key));
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
    // Re-initialize defaults
    initializeDefaults();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "==============================================\n";
    
    // Sort by source priority (Runtime > CommandLine > File > Default)
    std::vector<const ConfigEntry*> sortedEntries;
    for (const auto& entry : pImpl->entries) {
        sortedEntries.push_back(&entry);
    }
    
    std::sort(sortedEntries.begin(), sortedEntries.end(),
        [](const ConfigEntry* a, const ConfigEntry* b) {
            return static_cast<int>(a->source) < static_cast<int>(b->source);
        });
    
    for (const auto* entry : sortedEntries) {
        oss << std::setw(30) << std::left << entry->key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << std::fixed << std::setprecision(3) << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry->value);
        oss << "]";
        
        // Show source
        std::string sourceStr;
        switch (entry->source) {
            case ConfigSource::Default: sourceStr = "Default"; break;
            case ConfigSource::File: sourceStr = "File"; break;
            case ConfigSource::CommandLine: sourceStr = "CommandLine"; break;
            case ConfigSource::Runtime: sourceStr = "Runtime"; break;
        }
        
        oss << " (" << std::setw(12) << std::left << sourceStr << ")";
        
        if (!entry->description.empty()) {
            oss << " - " << entry->description;
        }
        
        oss << "\n";
    }
    
    return oss.str();
}

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

// Helper methods
void Config::loadFromJson(const nlohmann::json& json, ConfigSource source) {
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        
        // Check if this is an object with description
        if (it->is_object() && it->contains("value")) {
            auto value = jsonValueFromJson((*it)["value"]);
            std::string description = it->value("description", "");
            set(key, value, source);
            if (!description.empty()) {
                auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                    [&key](const ConfigEntry& e) { return e.key == key; });
                if (entryIt != pImpl->entries.end()) {
                    entryIt->description = description;
                }
            }
        } else {
            auto value = jsonValueFromJson(*it);
            set(key, value, source);
        }
    }
}

ConfigValue Config::jsonValueFromJson(const nlohmann::json& json) {
    if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_number_integer()) {
        return json.get<int64_t>();
    } else if (json.is_number_float()) {
        return json.get<double>();
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_array()) {
        // For arrays, determine type based on first element
        if (!json.empty()) {
            if (json[0].is_number_integer()) {
                std::vector<int64_t> intArray;
                for (const auto& item : json) {
                    intArray.push_back(item.get<int64_t>());
                }
                return intArray;
            } else if (json[0].is_number_float()) {
                std::vector<double> doubleArray;
                for (const auto& item : json) {
                    doubleArray.push_back(item.get<double>());
                }
                return doubleArray;
            } else if (json[0].is_string()) {
                std::vector<std::string> stringArray;
                for (const auto& item : json) {
                    stringArray.push_back(item.get<std::string>());
                }
                return stringArray;
            }
        }
        return std::vector<double>{}; // Empty fallback
    }
    return std::string(); // Empty string fallback
}

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            nlohmann::json arr = arg;
            return arr;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json arr = arg;
            return arr;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json arr = arg;
            return arr;
        } else {
            return nlohmann::json();
        }
    }, value);
}

void Config::setFromString(const std::string& key, const std::string& value, ConfigSource source) {
    // Try to infer type from value string
    bool isNumber = true;
    double numValue = 0.0;
    try {
        size_t pos;
        numValue = std::stod(value, &pos);
        if (pos == value.size()) {
            isNumber = true;
        } else {
            isNumber = false;
        }
    } catch (...) {
        isNumber = false;
    }
    
    bool isBool = false;
    bool boolValue = false;
    if (toLower(value) == "true" || toLower(value) == "false") {
        isBool = true;
        boolValue = (toLower(value) == "true");
    }
    
    if (isBool) {
        set(key, boolValue, source);
    } else if (isNumber) {
        // Check if it's an integer
        try {
            size_t pos;
            int64_t intValue = std::stoll(value, &pos);
            if (pos == value.size()) {
                set(key, intValue, source);
                return;
            }
        } catch (...) {}
        
        set(key, numValue, source);
    } else {
        set(key, value, source);
    }
}

std::string Config::getDefaultDescription(const std::string& key) {
    // Map keys to descriptions
    static const std::unordered_map<std::string, std::string> descriptions = {
        {"random_seed", "Random seed for reproducible experiments"},
        {"simulation_timestep", "Simulation timestep in seconds"},
        {"neuron_count", "Number of neurons in the brain"},
        {"region_count", "Number of brain regions"},
        {"connection_probability", "Connection probability for random connectivity"},
        {"stdp_ltp_weight", "STDP Long-Term Potentiation weight"},
        {"stdp_ltd_weight", "STDP Long-Term Depression weight"},
        {"stdp_time_constant", "STDP time constant"},
        {"plasticity_learning_rate", "Hebbian plasticity learning rate"},
        {"development_synaptogenesis_rate", "Rate of new synapse formation"},
        {"development_pruning_rate", "Rate of synapse elimination"},
        {"dopamine_baseline", "Baseline dopamine level for neuromodulation"},
        {"reward_discount_factor", "Reward discount factor for temporal discounting"},
        {"environment_name", "Name of the simulation environment"},
        {"environment_width", "Width of the environment"},
        {"environment_height", "Height of the environment"},
        {"log_level", "Logging level (DEBUG, INFO, WARNING, ERROR)"},
        {"log_to_file", "Whether to log to file"},
        {"log_filename", "Name of the log file"},
        {"max_simulation_steps", "Maximum number of simulation steps"},
        {"simulation_time_limit", "Time limit for simulation (seconds)"},
        {"visualization_enabled", "Whether visualization is enabled"},
        {"visualization_update_rate", "Visualization update rate (Hz)"},
        {"enable_reward_modulation", "Enable reward-based learning"},
        {"enable_structural_plasticity", "Enable structural plasticity"},
        {"enable_development", "Enable developmental processes"},
        {"enable_curiosity", "Enable curiosity-driven exploration"},
        {"enable_prediction", "Enable predictive processing"},
        {"enable_working_memory", "Enable working memory system"},
        {"enable_episodic_memory", "Enable episodic memory system"},
        {"enable_associative_memory", "Enable associative memory system"},
        {"working_memory_capacity", "Working memory capacity"},
        {"working_memory_trace_decay", "Working memory trace decay rate"},
        {"episodic_memory_capacity", "Episodic memory capacity"},
        {"episodic_memory_decay_rate", "Episodic memory decay rate"},
        {"episodic_memory_replay_strength", "Episodic memory replay strength"},
        {"prediction_horizon", "Prediction system time horizon"},
        {"prediction_error_threshold", "Prediction error threshold"},
        {"prediction_confidence_decay", "Prediction confidence decay rate"},
        {"curiosity_scaling", "Curiosity system scaling factor"},
        {"exploration_rate", "Base exploration rate"},
        {"novelty_threshold", "Novelty detection threshold"},
        {"critical_period_duration", "Critical period duration"},
        {"synaptogenesis_intensity", "Synaptogenesis intensity"},
        {"pruning_efficiency", "Pruning efficiency"},
        {"maturation_rate", "Brain maturation rate"},
        {"checkpoint_path", "Path for checkpoint files"},
        {"checkpoint_interval", "Interval between checkpoints"},
        {"enable_checkpointing", "Enable checkpoint saving"},
        {"enable_replay", "Enable memory replay"},
        {"replay_frequency", "Replay system frequency"},
        {"replay_strength", "Replay system strength"},
        {"v_thresh", "Neuron firing threshold (mV)"},
        {"v_rest", "Neuron resting potential (mV)"},
        {"v_reset", "Neuron reset potential after spike (mV)"},
        {"tau_mem", "Membrane time constant (ms)"},
        {"tau_ref", "Refractory period (ms)"}
    };
    
    auto it = descriptions.find(key);
    return it != descriptions.end() ? it->second : "";
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
