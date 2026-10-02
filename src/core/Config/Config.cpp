// Enhanced NLM Configuration with Validation and Better Defaults

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cassert>

namespace nlm {

// Default configuration values
namespace DefaultConfig {
    const int64_t random_seed = 42;
    const double simulation_timestep = 0.001;
    const int64_t neuron_count = 1000;
    const int64_t region_count = 4;
    const float connection_probability = 0.1f;
    const float stdp_ltp_weight = 0.01f;
    const float stdp_ltd_weight = 0.012f;
    const float stdp_tau = 20.0f;
    const float synaptogenesis_rate = 0.0001f;
    const float pruning_rate = 0.00001f;
    const int64_t replay_interval = 100;
    const int64_t consolidation_interval = 1000;
    const int64_t max_neurons_per_region = 1000;
    const float neuron_v_thresh = -50.0f;
    const float neuron_v_rest = -70.0f;
    const float neuron_v_reset = -65.0f;
    const float neuron_tau_mem = 20.0f;
    const float neuron_tau_ref = 5.0f;
    const float neuron_leak_conductance = 10.0f;
    const bool enable_stdp = true;
    const bool enable_hebbian = true;
    const bool enable_structural_plasticity = true;
    const bool enable_reward_modulation = false;
    const bool enable_curiosity = false;
    const bool enable_novelty = false;
    const float curiosity_threshold = 0.5f;
    const float novelty_threshold = 0.1f;
    const float reward_scale = 1.0f;
    const int64_t max_history_size = 100;
}

// Configuration validation rules
struct ValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ValidationRule> validationRules;
    
    Impl() {
        // Initialize validation rules
        setupValidationRules();
        // Set up default values
        setupDefaults();
    }
    
    void setupValidationRules() {
        // Neuron count validation
        validationRules.push_back({
            "neuron_count",
            [](const ConfigValue& v) {
                if (const int64_t* val = std::get_if<int64_t>(&v)) {
                    return *val > 0 && *val <= 100000; // Reasonable limits
                }
                return false;
            },
            "neuron_count must be positive and <= 100000"
        });
        
        // Connection probability validation
        validationRules.push_back({
            "connection_probability", 
            [](const ConfigValue& v) {
                if (const float* val = std::get_if<float>(&v)) {
                    return *val >= 0.0f && *val <= 1.0f;
                }
                return false;
            },
            "connection_probability must be between 0.0 and 1.0"
        });
        
        // Time constants validation
        validationRules.push_back({
            "simulation_timestep",
            [](const ConfigValue& v) {
                if (const double* val = std::get_if<double>(&v)) {
                    return *val > 0.0 && *val <= 1.0; // Up to 1 second timestep
                }
                return false;
            },
            "simulation_timestep must be positive and <= 1.0"
        });
        
        // STDP parameters validation
        validationRules.push_back({
            "stdp_ltp_weight",
            [](const ConfigValue& v) {
                if (const float* val = std::get_if<float>(&v)) {
                    return *val >= 0.0f && *val <= 10.0f;
                }
                return false;
            },
            "stdp_ltp_weight must be between 0.0 and 10.0"
        });
        
        validationRules.push_back({
            "stdp_ltd_weight",
            [](const ConfigValue& v) {
                if (const float* val = std::get_if<float>(&v)) {
                    return *val >= 0.0f && *val <= 10.0f;
                }
                return false;
            },
            "stdp_ltd_weight must be between 0.0 and 10.0"
        });
        
        // Plasticity rates validation
        validationRules.push_back({
            "synaptogenesis_rate",
            [](const ConfigValue& v) {
                if (const float* val = std::get_if<float>(&v)) {
                    return *val >= 0.0f && *val <= 1.0f;
                }
                return false;
            },
            "synaptogenesis_rate must be between 0.0 and 1.0"
        });
        
        validationRules.push_back({
            "pruning_rate",
            [](const ConfigValue& v) {
                if (const float* val = std::get_if<float>(&v)) {
                    return *val >= 0.0f && *val <= 1.0f;
                }
                return false;
            },
            "pruning_rate must be between 0.0 and 1.0"
        });
    }
    
    void setupDefaults() {
        // Set up default configuration values
        set("random_seed", DefaultConfig::random_seed, ConfigSource::Default);
        set("simulation_timestep", DefaultConfig::simulation_timestep, ConfigSource::Default);
        set("neuron_count", DefaultConfig::neuron_count, ConfigSource::Default);
        set("region_count", DefaultConfig::region_count, ConfigSource::Default);
        set("connection_probability", DefaultConfig::connection_probability, ConfigSource::Default);
        set("stdp_ltp_weight", DefaultConfig::stdp_ltp_weight, ConfigSource::Default);
        set("stdp_ltd_weight", DefaultConfig::stdp_ltd_weight, ConfigSource::Default);
        set("stdp_tau", DefaultConfig::stdp_tau, ConfigSource::Default);
        set("synaptogenesis_rate", DefaultConfig::synaptogenesis_rate, ConfigSource::Default);
        set("pruning_rate", DefaultConfig::pruning_rate, ConfigSource::Default);
        set("replay_interval", DefaultConfig::replay_interval, ConfigSource::Default);
        set("consolidation_interval", DefaultConfig::consolidation_interval, ConfigSource::Default);
        set("max_neurons_per_region", DefaultConfig::max_neurons_per_region, ConfigSource::Default);
        set("neuron_v_thresh", DefaultConfig::neuron_v_thresh, ConfigSource::Default);
        set("neuron_v_rest", DefaultConfig::neuron_v_rest, ConfigSource::Default);
        set("neuron_v_reset", DefaultConfig::neuron_v_reset, ConfigSource::Default);
        set("neuron_tau_mem", DefaultConfig::neuron_tau_mem, ConfigSource::Default);
        set("neuron_tau_ref", DefaultConfig::neuron_tau_ref, ConfigSource::Default);
        set("neuron_leak_conductance", DefaultConfig::neuron_leak_conductance, ConfigSource::Default);
        set("enable_stdp", DefaultConfig::enable_stdp, ConfigSource::Default);
        set("enable_hebbian", DefaultConfig::enable_hebbian, ConfigSource::Default);
        set("enable_structural_plasticity", DefaultConfig::enable_structural_plasticity, ConfigSource::Default);
        set("enable_reward_modulation", DefaultConfig::enable_reward_modulation, ConfigSource::Default);
        set("enable_curiosity", DefaultConfig::enable_curiosity, ConfigSource::Default);
        set("enable_novelty", DefaultConfig::enable_novelty, ConfigSource::Default);
        set("curiosity_threshold", DefaultConfig::curiosity_threshold, ConfigSource::Default);
        set("novelty_threshold", DefaultConfig::novelty_threshold, ConfigSource::Default);
        set("reward_scale", DefaultConfig::reward_scale, ConfigSource::Default);
        set("max_history_size", DefaultConfig::max_history_size, ConfigSource::Default);
    }
    
    bool validateKey(const std::string& key) const {
        // Basic key validation
        if (key.empty()) {
            return false;
        }
        if (key.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_.-") == std::string::npos) {
            return true;
        }
        return false;
    }
    
    bool validateValue(const std::string& key, const ConfigValue& value) const {
        // Find validation rule for this key
        auto it = std::find_if(validationRules.begin(), validationRules.end(),
                              [&key](const ValidationRule& rule) { return rule.key == key; });n        
        if (it == validationRules.end()) {
            // No specific validation for this key, perform basic checks
            if (std::holds_alternative<int>(value)) {
                int val = std::get<int>(value);
                return val >= std::numeric_limits<int>::min() && val <= std::numeric_limits<int>::max();
            } else if (std::holds_alternative<int64_t>(value)) {
                int64_t val = std::get<int64_t>(value);
                return val >= std::numeric_limits<int64_t>::min() && val <= std::numeric_limits<int64_t>::max();
            } else if (std::holds_alternative<double>(value)) {
                double val = std::get<double>(value);
                return !std::isnan(val) && !std::isinf(val);
            } else if (std::holds_alternative<bool>(value)) {
                return true; // bool is always valid
            } else if (std::holds_alternative<std::string>(value)) {
                const std::string& val = std::get<std::string>(value);
                return !val.empty(); // Should not be empty string
            }
            return false;
        }
        
        // Apply validation rule
        return it->validator(value);
    }
    
    std::string getValidationError(const std::string& key, const ConfigValue& value) const {
        auto it = std::find_if(validationRules.begin(), validationRules.end(),
                              [&key](const ValidationRule& rule) { return rule.key == key; });
        
        if (it == validationRules.end()) {
            return "No validation rule found for key: " + key;
        }
        
        if (!it->validator(value)) {
            return it->errorMessage;
        }
        
        return "";
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Validate line format
            if (line.find('=') == std::string::npos) {
                continue; // Skip malformed lines but don't crash
            }
            
            // Parse simple key=value pairs
            std::string key = trim(line.substr(0, line.find('=')));
            std::string value = trim(line.substr(line.find('=') + 1));
            
            // Validate key format
            if (!pImpl->validateKey(key)) {
                continue; // Skip malformed keys
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate value
            ConfigValue configValue(value);
            if (!pImpl->validateValue(key, configValue)) {
                continue; // Skip invalid values
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    // Validate key format
                    if (!pImpl->validateKey(key)) {
                        continue;
                    }
                    
                    ConfigValue configValue(value);
                    if (!pImpl->validateValue(key, configValue)) {
                        continue;
                    }
                    
                    set(key, configValue, ConfigSource::CommandLine);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                // Validate key format
                if (!pImpl->validateKey(key)) {
                    continue;
                }
                
                ConfigValue configValue(value);
                if (!pImpl->validateValue(key, configValue)) {
                    continue;
                }
                
                set(key, configValue, ConfigSource::CommandLine);
            }
        }
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << "# NLM Configuration File\n";
        file << "# Generated by NLM Configuration System\n";
        file << "# Format: key = value\n\n";
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << " (Source: " << static_cast<int>(entry.source) << ")\n";
            file << entry.key << " = \"" << std::get<std::string>(entry.value) << "\"\n\n";
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    assert(pImpl && "Config implementation is null");
    
    if (!pImpl->validateKey(key)) {
        return std::nullopt;
    }
    
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
    assert(pImpl && "Config implementation is null");
    
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    assert(pImpl && "Config implementation is null");
    
    if (!pImpl->validateKey(key)) {
        return;
    }
    
    if (!pImpl->validateValue(key, value)) {
        return;
    }
    
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
    assert(pImpl && "Config implementation is null");
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    assert(pImpl && "Config implementation is null");
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    assert(pImpl && "Config implementation is null");
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    assert(pImpl && "Config implementation is null");
    pImpl->entries.clear();
}

std::string Config::summary() const {
    assert(pImpl && "Config implementation is null");
    std::ostringstream oss;
    oss << "Enhanced NLM Configuration (" << pImpl->entries.size() << " entries):\n";
    oss << "Validation: ENABLED\n";
    oss << "Defaults: AUTO-LOADED\n\n";
    
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
        oss << "] (Source: " << static_cast<int>(entry.source) << ")\n";
    }
    
    oss << "\nConfiguration Status: VALID\n";
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