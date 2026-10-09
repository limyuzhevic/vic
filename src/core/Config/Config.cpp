#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace nlm {

// Default configuration values
const std::unordered_map<std::string, ConfigValue> DefaultConfigValues = {
    // World dimensions
    {"world_width", 20.0f},
    {"world_height", 20.0f},
    {"world_radius", 0.0f},
    
    // Vision configuration
    {"vision_width", 16},
    {"vision_height", 16},
    {"vision_touch_count", 8},
    {"vision_internal_count", 4},
    {"vision_proprioception_count", 6},
    {"vision_audio_count", 0},
    
    // Energy parameters
    {"max_energy", 100.0f},
    {"energy_decay_rate", 0.01f},
    {"resource_energy_gain", 30.0f},
    
    // Object placement parameters
    {"num_resource_objects", 5},
    {"num_hazard_objects", 3},
    
    // Simulation parameters
    {"simulation_timestep", 0.001},
    {"max_action_interval", 0.1},
    {"movement_speed", 3.0f},
    {"turn_speed", 2.0f},
    {"interaction_range", 1.0f},
    
    // Network topology
    {"connection_probability", 0.1f},
    {"connection_weight_range_min", 0.2f},
    {"connection_weight_range_max", 0.1f},
    {"inter_region_weight", 0.1f},
    {"inter_region_delay", 1},
    
    // Memory parameters
    {"working_memory_capacity", 100},
    {"episodic_memory_max_episodes", 1000},
    {"working_memory_decay_rate", 0.99f},
    {"memory_consolidation_interval", 1000},
    {"memory_replay_interval", 100},
    
    // Neural parameters
    {"synapse_formation_rate", 0.0001f},
    {"synapse_pruning_rate", 0.00001f},
    {"stdp_ltp_weight", 0.01f},
    {"stdp_ltd_weight", 0.012f},
    {"stdp_tau", 20.0f},
    {"hebbian_learning_rate", 0.2f},
    {"plasticity_modifier", 1.0f},
    
    // Thresholds
    {"similarity_threshold", 0.75f},
    {"stability_threshold", 0.7f},
    {"formation_threshold", 0.75f},
    {"confidence_threshold", 0.5f},
    {"novelty_threshold", 0.3f},
    {"curiosity_threshold", 0.5f},
    
    // Learning rates
    {"prediction_learning_rate", 0.1f},
    {"reward_learning_rate", 0.05f},
    {"plasticity_decay_rate", 0.9f},
    
    // Random seed
    {"random_seed", 42},
    
    // Development parameters
    {"development_enabled", true},
    {"structural_plasticity_enabled", true},
    {"reward_modulation_enabled", true},
    {"curiosity_enabled", true},
    
    // Development stages
    {"initial_stage_duration", 60.0},
    {"critical_period_duration", 240.0},
    {"maturation_stage_duration", 540.0},
    
    // Checkpoint system
    {"checkpoint_dir", "./checkpoints"},
    {"checkpoint_max_files", 10000},
    {"checkpoint_compression_level", 5},
    {"checkpoint_autosave", true},
    
    // Logging
    {"log_level", "info"},
    {"log_file", ""},
    
    // Experimental
    {"neuron_count", 1000},
    {"region_count", 1},
    {"planning_depth", 5},
    {"attention_inhibition_strength", 0.5f},
    {"attention_excitation_strength", 1.5f},
    
    // Episode parameters
    {"episode_store_every", 10},
    {"episode_max_length", 1000},
    
    // General bounds
    {"min_vision_width", 8},
    {"max_vision_width", 64},
    {"min_vision_height", 8},
    {"max_vision_height", 64},
    {"min_world_size", 10.0f},
    {"max_world_size", 100.0f},
    {"min_energy", 0.0f},
    {"max_energy", 1000.0f},
    {"min_connection_probability", 0.0f},
    {"max_connection_probability", 1.0f},
    {"min_neuron_count", 10},
    {"max_neuron_count", 10000},
    {"min_timestep", 0.0001},
    {"max_timestep", 1.0},
    {"min_plasticity", 0.1f},
    {"max_plasticity", 2.0f},
    {"min_stability", 0.0f},
    {"max_stability", 1.0f}
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Load from file with JSON/YAML support
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Try JSON first
    try {
        auto jsonData = json::parse(content);
        return loadFromJson(jsonData);
    } catch (const json::parse_error&) {
        // Try simple key=value format
        std::istringstream iss(content);
        std::string line;
        while (std::getline(iss, line)) {
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
                
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
}

bool Config::loadFromJson(const json& jsonData) {
    clear();
    
    for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
        std::string key = it.key();
        
        try {
            // Try different types based on JSON value type
            if (it->is_string()) {
                set(key, it->get<std::string>(), ConfigSource::File);
            } else if (it->is_number_integer()) {
                set(key, it->get<int64_t>(), ConfigSource::File);
            } else if (it->is_number_unsigned()) {
                set(key, it->get<uint64_t>(), ConfigSource::File);
            } else if (it->is_number_float()) {
                set(key, it->get<double>(), ConfigSource::File);
            } else if (it->is_boolean()) {
                set(key, it->get<bool>(), ConfigSource::File);
            } else if (it->is_array()) {
                // For arrays, just store as string for now
                std::vector<std::string> strArray;
                for (const auto& item : *it) {
                    strArray.push_back(item.get<std::string>());
                }
                set(key, strArray, ConfigSource::File);
            }
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error parsing JSON key " + key + ": " + std::string(e.what()));
        }
    }
    
    // Apply defaults for missing keys
    for (const auto& [key, defaultValue] : DefaultConfigValues) {
        if (!has(key)) {
            set(key, defaultValue, ConfigSource::Default);
        }
    }
    
    // Validate configuration
    validateConfig();
    
    return true;
}

std::string Config::toJson() const {
    json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonData[entry.key] = arg;
            }
        }, entry.value);
    }
    
    return jsonData.dump(2);
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    // Save as JSON for better readability and compatibility
    file << toJson();
    
    return true;
}

void Config::validateConfig() const {
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        
        // Convert key to lowercase for case-insensitive lookup
        std::string lowerKey = toLower(key);
        
        // Check if this key has validation rules
        auto validator = getValidator(lowerKey);
        if (validator) {
            try {
                validator(entry);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Config validation error for key '" + key + "': " + std::string(e.what()));
            }
        }
    }
}

ConfigEntry::Validator Config::getValidator(const std::string& key) const {
    // Return appropriate validator for each key
    // This is a simplified implementation - in practice, you'd want more sophisticated validation
    
    if (key == "world_width" || key == "world_height" || key == "world_radius") {
        return [](const ConfigEntry& entry) {
            auto val = std::get<double>(entry.value);
            if (val < get<double>(DefaultConfigValues.at("min_world_size")).value_or(10.0f)) {
                throw std::runtime_error("Value must be >= " + 
                    std::to_string(get<double>(DefaultConfigValues.at("min_world_size")).value_or(10.0f)));
            }
            if (val > get<double>(DefaultConfigValues.at("max_world_size")).value_or(100.0f)) {
                throw std::runtime_error("Value must be <= " + 
                    std::to_string(get<double>(DefaultConfigValues.at("max_world_size")).value_or(100.0f)));
            }
        };
    }
    
    if (key == "vision_width" || key == "vision_height") {
        return [](const ConfigEntry& entry) {
            auto val = std::get<int64_t>(entry.value);
            if (val < get<int64_t>(DefaultConfigValues.at("min_vision_width")).value_or(8)) {
                throw std::runtime_error("Value must be >= " + 
                    std::to_string(get<int64_t>(DefaultConfigValues.at("min_vision_width")).value_or(8)));
            }
            if (val > get<int64_t>(DefaultConfigValues.at("max_vision_width")).value_or(64)) {
                throw std::runtime_error("Value must be <= " + 
                    std::to_string(get<int64_t>(DefaultConfigValues.at("max_vision_width")).value_or(64)));
            }
        };
    }
    
    if (key == "connection_probability") {
        return [](const ConfigEntry& entry) {
            auto val = std::get<double>(entry.value);
            if (val < get<double>(DefaultConfigValues.at("min_connection_probability")).value_or(0.0f)) {
                throw std::runtime_error("Value must be >= " + 
                    std::to_string(get<double>(DefaultConfigValues.at("min_connection_probability")).value_or(0.0f)));
            }
            if (val > get<double>(DefaultConfigValues.at("max_connection_probability")).value_or(1.0f)) {
                throw std::runtime_error("Value must be <= " + 
                    std::to_string(get<double>(DefaultConfigValues.at("max_connection_probability")).value_or(1.0f)));
            }
        };
    }
    
    if (key == "neuron_count") {
        return [](const ConfigEntry& entry) {
            auto val = std::get<int64_t>(entry.value);
            if (val < get<int64_t>(DefaultConfigValues.at("min_neuron_count")).value_or(10)) {
                throw std::runtime_error("Value must be >= " + 
                    std::to_string(get<int64_t>(DefaultConfigValues.at("min_neuron_count")).value_or(10)));
            }
            if (val > get<int64_t>(DefaultConfigValues.at("max_neuron_count")).value_or(10000)) {
                throw std::runtime_error("Value must be <= " + 
                    std::to_string(get<int64_t>(DefaultConfigValues.at("max_neuron_count")).value_or(10000)));
            }
        };
    }
    
    if (key == "simulation_timestep") {
        return [](const ConfigEntry& entry) {
            auto val = std::get<double>(entry.value);
            if (val < get<double>(DefaultConfigValues.at("min_timestep")).value_or(0.0001)) {
                throw std::runtime_error("Value must be >= " + 
                    std::to_string(get<double>(DefaultConfigValues.at("min_timestep")).value_or(0.0001)));
            }
            if (val > get<double>(DefaultConfigValues.at("max_timestep")).value_or(1.0)) {
                throw std::runtime_error("Value must be <= " + 
                    std::to_string(get<double>(DefaultConfigValues.at("max_timestep")).value_or(1.0)));
            }
        };
    }
    
    if (key == "similarity_threshold" || key == "stability_threshold" || 
        key == "formation_threshold" || key == "confidence_threshold" ||
        key == "novelty_threshold" || key == "curiosity_threshold") {
        return [](const ConfigEntry& entry) {
            auto val = std::get<double>(entry.value);
            if (val < get<double>(DefaultConfigValues.at("min_stability")).value_or(0.0f)) {
                throw std::runtime_error("Value must be >= " + 
                    std::to_string(get<double>(DefaultConfigValues.at("min_stability")).value_or(0.0f)));
            }
            if (val > get<double>(DefaultConfigValues.at("max_stability")).value_or(1.0f)) {
                throw std::runtime_error("Value must be <= " + 
                    std::to_string(get<double>(DefaultConfigValues.at("max_stability")).value_or(1.0f)));
            }
        };
    }
    
    if (key == "plasticity_modifier") {
        return [](const ConfigEntry& entry) {
            auto val = std::get<double>(entry.value);
            if (val < get<double>(DefaultConfigValues.at("min_plasticity")).value_or(0.1f)) {
                throw std::runtime_error("Value must be >= " + 
                    std::to_string(get<double>(DefaultConfigValues.at("min_plasticity")).value_or(0.1f)));
            }
            if (val > get<double>(DefaultConfigValues.at("max_plasticity")).value_or(2.0f)) {
                throw std::runtime_error("Value must be <= " + 
                    std::to_string(get<double>(DefaultConfigValues.at("max_plasticity")).value_or(2.0f)));
            }
        };
    }
    
    return nullptr; // No validation
}

Config::LoadResult Config::loadConfigFile(const std::string& filepath) {
    if (loadFromFile(filepath)) {
        NLM_LOG_INFO("Successfully loaded configuration from " + filepath);
        return LoadResult::Success;
    } else {
        NLM_LOG_ERROR("Failed to load configuration from " + filepath);
        return LoadResult::FileNotFound;
    }
}

Config::LoadResult Config::loadConfigArgs(int argc, char** argv) {
    if (loadFromArgs(argc, argv)) {
        NLM_LOG_INFO("Successfully loaded configuration from command line arguments");
        return LoadResult::Success;
    } else {
        NLM_LOG_ERROR("Failed to load configuration from command line arguments");
        return LoadResult::InvalidArgs;
    }
}

std::string Config::generateDefaultConfig() {
    json jsonData;
    
    for (const auto& [key, value] : DefaultConfigValues) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                jsonData[key] = arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                jsonData[key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonData[key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonData[key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonData[key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonData[key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonData[key] = arg;
            }
        }, value);
    }
    
    return jsonData.dump(2);
}

std::vector<std::string> Config::getAllKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getAvailableKeys() const {
    std::vector<std::string> keys;
    for (const auto& [key, value] : DefaultConfigValues) {
        keys.push_back(key);
    }
    return keys;
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
        pImpl->entries.emplace_back(key, value, source);
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
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
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
