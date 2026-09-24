#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Validate filepath before attempting to open
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config file path is empty");
        return false;
    }
    
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Config file not found: " + filepath + ". Using defaults.");
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
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Log parsing for debugging
            NLM_LOG_DEBUG("Parsed config line " + std::to_string(lineNumber) + 
                         ": " + key + " = " + value);
            
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Skipping malformed config line " + std::to_string(lineNumber) + ": " + line);
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
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Generated on " << getCurrentTimeString() << "\n\n";
    
    // Write entries
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.key << " [Source: " << static_cast<int>(entry.source) << "]\n";
        std::visit([&file, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        file << "\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        NLM_LOG_WARNING("Attempt to get config with empty key");
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Config key not found: " + key);
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Config key " + key + " has value of incompatible type for requested type");
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        NLM_LOG_DEBUG("Config key " + key + " not found, using default value");
        return defaultValue;
    }
    return val.value();
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        NLM_LOG_WARNING("Attempt to set config with empty key");
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Updated config key: " + key + " from source: " + std::to_string(static_cast<int>(source)));
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Added new config key: " + key);
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

// Known configuration parameters for validation
struct KnownConfigParameters {
    std::vector<std::string> required;
    std::vector<std::string> optional;
    std::map<std::string, std::function<bool(const ConfigValue&)>> validators;
};

KnownConfigParameters getKnownParameters() {
    KnownConfigParameters params;
    
    // Required parameters
    params.required = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count",
        "connection_probability"
    };
    
    // Optional parameters with default values
    params.optional = {
        "plasticity_learning_rate",
        "stdp_ltp_weight",
        "stdp_ltd_weight",
        "stdp_time_constant",
        "development_synaptogenesis_rate",
        "development_pruning_rate",
        "dopamine_baseline",
        "reward_discount_factor",
        "environment_name",
        "environment_width",
        "environment_height",
        "log_level",
        "log_to_file",
        "log_filename",
        "max_simulation_steps",
        "simulation_time_limit",
        "visualization_enabled",
        "visualization_update_rate",
        "replay_interval",
        "consolidation_interval",
        "enable_checkpointing",
        "enable_replay",
        "enable_development"
    };
    
    // Validators
    params.validators = {
        {"random_seed", [](const ConfigValue& v) {
            try { return std::get<int64_t>(v) >= 0; } catch (...) { return false; }
        }},
        {"simulation_timestep", [](const ConfigValue& v) {
            try { return std::get<double>(v) > 0.0; } catch (...) { return false; }
        }},
        {"neuron_count", [](const ConfigValue& v) {
            try { return std::get<int64_t>(v) > 0; } catch (...) { return false; }
        }},
        {"region_count", [](const ConfigValue& v) {
            try { return std::get<int64_t>(v) > 0; } catch (...) { return false; }
        }},
        {"connection_probability", [](const ConfigValue& v) {
            try { 
                double p = std::get<double>(v); 
                return p >= 0.0 && p <= 1.0; 
            } catch (...) { return false; }
        }},
        {"stdp_ltp_weight", [](const ConfigValue& v) {
            try { 
                float w = std::get<float>(v); 
                return w >= 0.0f && w <= 1.0f; 
            } catch (...) { return false; }
        }},
        {"stdp_ltd_weight", [](const ConfigValue& v) {
            try { 
                float w = std::get<float>(v); 
                return w >= 0.0f && w <= 1.0f; 
            } catch (...) { return false; }
        }},
        {"stdp_time_constant", [](const ConfigValue& v) {
            try { return std::get<double>(v) > 0.0; } catch (...) { return false; }
        }},
        {"dopamine_baseline", [](const ConfigValue& v) {
            try { return std::get<double>(v) >= -10.0 && std::get<double>(v) <= 10.0; } catch (...) { return false; }
        }},
        {"reward_discount_factor", [](const ConfigValue& v) {
            try { return std::get<double>(v) >= 0.0 && std::get<double>(v) <= 1.0; } catch (...) { return false; }
        }},
        {"log_level", [](const ConfigValue& v) {
            try { 
                std::string level = std::get<std::string>(v); 
                return level == "DEBUG" || level == "INFO" || level == "WARNING" || 
                       level == "ERROR" || level == "CRITICAL";
            } catch (...) { return false; }
        }},
        {"log_to_file", [](const ConfigValue& v) {
            try { return std::get<bool>(v); } catch (...) { return false; }
        }},
        {"visualization_enabled", [](const ConfigValue& v) {
            try { return std::get<bool>(v); } catch (...) { return false; }
        }},
        {"enable_checkpointing", [](const ConfigValue& v) {
            try { return std::get<bool>(v); } catch (...) { return false; }
        }},
        {"enable_replay", [](const ConfigValue& v) {
            try { return std::get<bool>(v); } catch (...) { return false; }
        }},
        {"enable_development", [](const ConfigValue& v) {
            try { return std::get<bool>(v); } catch (...) { return false; }
        }},
        {"replay_interval", [](const ConfigValue& v) {
            try { return std::get<size_t>(v) > 0; } catch (...) { return false; }
        }},
        {"consolidation_interval", [](const ConfigValue& v) {
            try { return std::get<size_t>(v) > 0; } catch (...) { return false; }
        }}
    };
    
    return params;
}

// Validate configuration parameters
bool Config::validate() const {
    KnownConfigParameters known = getKnownParameters();
    bool allValid = true;
    
    // Check required parameters
    for (const auto& key : known.required) {
        if (!has(key)) {
            NLM_LOG_WARNING("Missing required configuration parameter: " + key);
            allValid = false;
        }
    }
    
    // Check optional parameters with validators
    for (const auto& key : known.optional) {
        if (has(key)) {
            auto val = get<ConfigValue>(key);
            auto it = known.validators.find(key);
            if (it != known.validators.end() && !it->second(val)) {
                NLM_LOG_ERROR("Invalid value for configuration parameter: " + key);
                allValid = false;
            }
        }
    }
    
    return allValid;
}

// Get configuration parameters for help
std::string Config::getHelpText() const {
    KnownConfigParameters known = getKnownParameters();
    std::ostringstream oss;
    oss << "NLM Configuration Parameters\n";
    oss << "==========================\n\n";
    
    oss << "Required parameters:\n";
    for (const auto& key : known.required) {
        oss << "  " << key << "\n";
    }
    oss << "\n";
    
    oss << "Optional parameters:\n";
    for (const auto& key : known.optional) {
        oss << "  " << key << "\n";
    }
    
    return oss.str();
}

// Helper function to get current time string
std::string getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Helper function to get current time string
std::string getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
