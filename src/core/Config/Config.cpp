#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <cmath>
#include <queue>
#include <mutex>
#include <thread>

using namespace nlm;

namespace {
    // Helper functions for configuration validation and processing
    std::string currentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
    
    double stringToDouble(const std::string& str) {
        try {
            return std::stod(str);
        } catch (const std::exception&) {
            return 0.0;
        }
    }
    
    int64_t stringToInt64(const std::string& str) {
        try {
            return std::stoll(str);
        } catch (const std::exception&) {
            return 0;
        }
    }
    
    bool stringToBool(const std::string& str) {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        return lower == "true" || lower == "1" || lower == "yes" || lower == "enabled";
    }
    
    std::string validateAndNormalizeKey(const std::string& key) {
        std::string result = key;
        // Remove leading/trailing whitespace
        size_t start = result.find_first_not_of(" \t\r\n");
        size_t end = result.find_last_not_of(" \t\r\n");
        if (start != std::string::npos && end != std::string::npos) {
            result = result.substr(start, end - start + 1);
        }
        return result;
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::function<bool(const std::string&)>> validators;
    std::map<std::string, ConfigValue> defaults;
    std::mutex mutex;
    
    Impl() {
        // Initialize built-in configuration options with validation
        initializeDefaults();
        initializeValidators();
    }
    
    void initializeDefaults() {
        // Core simulation parameters
        defaults["random_seed"] = int64_t(42);
        defaults["simulation_timestep"] = 0.001;
        defaults["neuron_count"] = size_t(1000);
        defaults["region_count"] = size_t(2);
        defaults["connection_probability"] = 0.1f;
        defaults["max_simulation_steps"] = size_t(10000);
        defaults["simulation_time_limit"] = 0.0; // unlimited
        
        // Plasticity parameters
        defaults["plasticity_learning_rate"] = 0.01f;
        defaults["stdp_ltp_weight"] = 0.01f;
        defaults["stdp_ltd_weight"] = 0.012f;
        defaults["stdp_time_constant"] = 20.0f;
        defaults["synaptogenesis_rate"] = 0.001f;
        defaults["pruning_rate"] = 0.0001f;
        defaults["structural_plasticity_enabled"] = false;
        defaults["plasticity_mode"] = std::string("combined"); // hebbian, stdp, combined, none
        
        // Neuromodulation parameters
        defaults["dopamine_baseline"] = 0.0f;
        defaults["curiosity_baseline"] = 0.0f;
        defaults["novelty_threshold"] = 0.5f;
        defaults["prediction_error_threshold"] = 0.1f;
        defaults["neuromodulation_enabled"] = false;
        
        // Memory system parameters
        defaults["working_memory_capacity"] = size_t(100);
        defaults["episodic_memory_max_episodes"] = size_t(1000);
        defaults["associative_memory_pattern_count"] = size_t(500);
        defaults["memory_systems_enabled"] = false;
        
        // Cognitive parameters
        defaults["planning_depth"] = size_t(5);
        defaults["attention_inhibition_strength"] = 0.5f;
        defaults["attention_excitation_strength"] = 1.5f;
        defaults["concept_formation_enabled"] = false;
        defaults["planning_enabled"] = false;
        defaults["attention_enabled"] = false;
        
        // Development parameters
        defaults["development_stage"] = std::string("initial"); // initial, critical, maturation, adult
        defaults["development_synaptogenesis_rate"] = 0.001f;
        defaults["development_pruning_rate"] = 0.0001f;
        defaults["development_enabled"] = false;
        
        // Environmental parameters
        defaults["environment_name"] = std::string("GridWorld");
        defaults["environment_width"] = size_t(10);
        defaults["environment_height"] = size_t(10);
        defaults["vision_width"] = size_t(8);
        defaults["vision_height"] = size_t(8);
        
        // Logging parameters
        defaults["log_level"] = std::string("INFO"); // DEBUG, INFO, WARN, ERROR
        defaults["log_to_file"] = false;
        defaults["log_filename"] = std::string("nlm.log");
        defaults["log_max_size"] = size_t(10 * 1024 * 1024); // 10MB
        
        // Visualization parameters
        defaults["visualization_enabled"] = false;
        defaults["visualization_update_rate"] = 30.0f;
        defaults["plot_spikes"] = false;
        defaults["plot_synapses"] = false;
        defaults["plot_neurons"] = false;
        
        // Performance parameters
        defaults["checkpoint_interval"] = size_t(1000);
        defaults["checkpoint_enabled"] = true;
        defaults["checkpoint_dir"] = std::string("./checkpoints");
        defaults["max_checkpoints"] = size_t(10);
        defaults["performance_profiling_enabled"] = false;
        defaults["memory_profiling_enabled"] = false;
        defaults["performance_metrics_enabled"] = false;
        
        // Advanced parameters
        defaults["device_type"] = std::string("CPU"); // CPU, GPU
        defaults["num_threads"] = size_t(0); // 0 = auto
        defaults["precision_mode"] = std::string("float"); // float, double, half
        defaults["sparse_connectivity_enabled"] = false;
        defaults["simd_enabled"] = true;
        defaults["parallel_enabled"] = true;
        
        // Experimental parameters
        defaults["reward_scale"] = 1.0f;
        defaults["reward_discount_factor"] = 0.99f;
        defaults["curiosity_strength"] = 1.0f;
        defaults["novelty_detection_sensitivity"] = 0.5f;
        defaults["prediction_error_sensitivity"] = 0.1f;
        defaults["attention_sensitivity"] = 1.0f;
        
        // Safety parameters
        defaults["max_weight"] = 1.0f;
        defaults["min_weight"] = -1.0f;
        defaults["weight_stability_threshold"] = 0.001f;
        defaults["spike_threshold_violation_limit"] = size_t(10);
        defaults["neuron_reset_violation_limit"] = size_t(10);
        
        // Output parameters
        defaults["output_format"] = std::string("text"); // text, binary, json
        defaults["output_compression"] = std::string("none"); // none, gzip, bz2
        defaults["output_interval"] = size_t(100);
        
        // System parameters
        defaults["strict_validation"] = true;
        defaults["validate_on_set"] = true;
        defaults["auto_save_interval"] = size_t(1000);
        defaults["backup_enabled"] = false;
    }
    
    void initializeValidators() {
        // Core validation
        validators["random_seed"] = [](const std::string& v) {
            try {
                int64_t val = std::stoll(v);
                return val >= 0;
            } catch (...) { return false; }
        };
        
        validators["simulation_timestep"] = [](const std::string& v) {
            double val = stringToDouble(v);
            return val > 0.0 && val < 1.0; // Reasonable timestep range
        };
        
        validators["neuron_count"] = [](const std::string& v) {
            size_t val = std::stoul(v);
            return val > 0 && val <= 1000000; // 1M neuron limit
        };
        
        validators["region_count"] = [](const std::string& v) {
            size_t val = std::stoul(v);
            return val > 0 && val <= 100;
        };
        
        validators["connection_probability"] = [](const std::string& v) {
            double val = stringToDouble(v);
            return val >= 0.0 && val <= 1.0;
        };
        
        // Plasticity validation
        validators["plasticity_learning_rate"] = [](const std::string& v) {
            float val = stringToDouble(v);
            return val > 0.0 && val <= 10.0;
        };
        
        validators["stdp_ltp_weight"] = [](const std::string& v) {
            float val = stringToDouble(v);
            return val >= 0.0 && val <= 10.0;
        };
        
        validators["plasticity_mode"] = [](const std::string& v) {
            return v == "hebbian" || v == "stdp" || v == "combined" || v == "none";
        };
        
        // Development validation
        validators["development_stage"] = [](const std::string& v) {
            return v == "initial" || v == "critical" || v == "maturation" || v == "adult";
        };
        
        // Environment validation
        validators["environment_width"] = [](const std::string& v) {
            size_t val = std::stoul(v);
            return val > 0 && val <= 1000;
        };
        
        validators["environment_height"] = [](const std::string& v) {
            size_t val = std::stoul(v);
            return val > 0 && val <= 1000;
        };
        
        // Log level validation
        validators["log_level"] = [](const std::string& v) {
            return toLower(v) == "debug" || toLower(v) == "info" || 
                   toLower(v) == "warn" || toLower(v) == "error";
        };
        
        // Visualization validation
        validators["visualization_update_rate"] = [](const std::string& v) {
            double val = stringToDouble(v);
            return val > 0.0 && val <= 1000.0;
        };
        
        // Performance validation
        validators["checkpoint_interval"] = [](const std::string& v) {
            size_t val = std::stoul(v);
            return val == 0 || (val > 0 && val <= 100000);
        };
        
        validators["max_checkpoints"] = [](const std::string& v) {
            size_t val = std::stoul(v);
            return val > 0 && val <= 100;
        };
        
        // Advanced validation
        validators["device_type"] = [](const std::string& v) {
            return toLower(v) == "cpu" || toLower(v) == "gpu";
        };
        
        validators["precision_mode"] = [](const std::string& v) {
            return toLower(v) == "float" || toLower(v) == "double" || toLower(v) == "half";
        };
        
        // Output validation
        validators["output_format"] = [](const std::string& v) {
            return toLower(v) == "text" || toLower(v) == "binary" || toLower(v) == "json";
        };
        
        validators["output_compression"] = [](const std::string& v) {
            return toLower(v) == "none" || toLower(v) == "gzip" || toLower(v) == "bz2";
        };
    }
    
    bool validateKey(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Check if key exists in defaults
        if (defaults.find(key) == defaults.end()) {
            // Custom key - allow any value for flexibility
            return true;
        }
        
        // Check if there's a validator
        auto validatorIt = validators.find(key);
        if (validatorIt == validators.end()) {
            return true; // No validator, allow the value
        }
        
        // Apply validation
        return validatorIt->second(value);
    }
    
    ConfigValue normalizeValue(const std::string& key, const std::string& value) {
        std::lock_guard<std::mutex> lock(mutex);
        
        // Check what type the default value is
        auto defaultIt = defaults.find(key);
        if (defaultIt == defaults.end()) {
            // No default, treat as string
            return value;
        }
        
        const ConfigValue& defaultVal = defaultIt->second;
        
        // Try to match the type
        if (std::holds_alternative<int>(defaultVal)) {
            try {
                return std::stoi(value);
            } catch (...) {
                return 0;
            }
        } else if (std::holds_alternative<int64_t>(defaultVal)) {
            try {
                return std::stoll(value);
            } catch (...) {
                return int64_t(0);
            }
        } else if (std::holds_alternative<double>(defaultVal)) {
            try {
                return std::stod(value);
            } catch (...) {
                return 0.0;
            }
        } else if (std::holds_alternative<bool>(defaultVal)) {
            return stringToBool(value);
        } else if (std::holds_alternative<size_t>(defaultVal)) {
            try {
                return std::stoul(value);
            } catch (...) {
                return size_t(0);
            }
        }
        
        // Default to string if type doesn't match
        return value;
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARN("Configuration file not found: " + filepath);
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs with enhanced support
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
            
            key = validateAndNormalizeKey(key);
            
            // Validate the value
            if (!validateKey(key, value)) {
                NLM_LOG_WARN("Invalid configuration value at line " + std::to_string(lineNumber) + 
                           ": key=\"" + key + "\", value=\"" + value + "\"");
                hasErrors = true;
                continue;
            }
            
            // Normalize and set the value
            ConfigValue normalizedValue = pImpl->normalizeValue(key, value);
            set(key, normalizedValue, ConfigSource::File);
            
            NLM_LOG_DEBUG("Loaded config: " + key + " = " + value);
        } else {
            NLM_LOG_WARN("Malformed configuration line at line " + std::to_string(lineNumber) + ": " + line);
            hasErrors = true;
        }
    }
    
    if (hasErrors) {
        NLM_LOG_WARN("Configuration file had validation errors");
    }
    
    // Fill in defaults for missing keys
    for (const auto& defaultPair : pImpl->defaults) {
        const std::string& key = defaultPair.first;
        if (!has(key)) {
            set(key, defaultPair.second, ConfigSource::Default);
            NLM_LOG_DEBUG("Set default config: " + key + " = [" + 
                         std::to_string(std::visit([](auto&& arg) { return std::to_string(arg); }, defaultPair.second)) + "]");
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate before setting
                if (pImpl->validateKey(key, value)) {
                    ConfigValue normalizedValue = pImpl->normalizeValue(key, value);
                    set(key, normalizedValue, ConfigSource::CommandLine);
                    NLM_LOG_DEBUG("Loaded config from args: " + key + " = " + value);
                } else {
                    NLM_LOG_WARN("Invalid command line argument: " + arg);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Skip boolean flags (true/false values)
            if (value.substr(0, 1) == "-") {
                // This is actually another flag, put it back
                i--;
                continue;
            }
            
            // Validate before setting
            if (pImpl->validateKey(key, value)) {
                ConfigValue normalizedValue = pImpl->normalizeValue(key, value);
                set(key, normalizedValue, ConfigSource::CommandLine);
                NLM_LOG_DEBUG("Loaded config from args: " + key + " = " + value);
            } else {
                NLM_LOG_WARN("Invalid command line argument: --" + key + " " + value);
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot save configuration to file: " + filepath);
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Generated: " << currentTimestamp() << "\n";
    file << "# Source: Runtime configuration\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        file << "\n";
    }
    
    // Write summary
    file << "\n# Configuration Summary\n";
    file << "# Total entries: " << pImpl->entries.size() << "\n";
    
    std::map<ConfigSource, size_t> sourceCount;
    for (const auto& entry : pImpl->entries) {
        sourceCount[entry.source]++;
    }
    
    for (const auto& pair : sourceCount) {
        file << "# Source " << static_cast<int>(pair.first) << ": " << pair.second << " entries\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_WARN("Type mismatch when accessing config key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Normalize and validate the key
    std::string normalizedKey = validateAndNormalizeKey(key);
    
    // Check if we need to validate the value
    auto valueStr = std::visit([](auto&& arg) { return std::to_string(arg); }, value);
    if (!pImpl->validateKey(normalizedKey, valueStr)) {
        NLM_LOG_WARN("Invalid configuration value for key: " + normalizedKey);
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Updated config: " + normalizedKey + " = " + valueStr);
    } else {
        pImpl->entries.emplace_back(normalizedKey, value, source);
        NLM_LOG_DEBUG("Added config: " + normalizedKey + " = " + valueStr);
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
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "Generated: " << currentTimestamp() << "\n\n";
    
    // Group by source
    std::map<ConfigSource, std::vector<ConfigEntry>> entriesBySource;
    for (const auto& entry : pImpl->entries) {
        entriesBySource[entry.source].push_back(entry);
    }
    
    for (const auto& sourcePair : entriesBySource) {
        oss << "=== Source " << static_cast<int>(sourcePair.first) << " ===\n";
        for (const auto& entry : sourcePair.second) {
            oss << "  " << entry.key << " = [";
            std::visit([&oss](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (arg ? "true" : "false");
                } else {
                    oss << arg;
                }
            }, entry.value);
            oss << "]\n";
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

// Explicit template instantiations
#include <typeinfo>

// Register common template specializations
namespace {
    // Helper for registration
    template<typename T>
    struct TypeRegistrar {
        static void registerType() {}
    };
    
    // Template instantiation for all types we support
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
    
    template std::optional<size_t> Config::get<size_t>(const std::string&) const;
    template size_t Config::getOr<size_t>(const std::string&, const size_t&) const;
}

} // namespace nlm
