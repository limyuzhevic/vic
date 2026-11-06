#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        // Log error but don't crash - allow fallback to defaults
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        // Skip empty lines and comments
        std::string trimmedLine = trim(line);
        if (trimmedLine.empty() || 
            trimmedLine[0] == '#' || 
            trimmedLine[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = trimmedLine.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config format at line " + std::to_string(lineNumber) + ": missing '='") ;
            hasErrors = true;
            continue;
        }
        
        std::string key = trim(trimmedLine.substr(0, pos));
        std::string value = trim(trimmedLine.substr(pos + 1));
        
        if (key.empty()) {
            NLM_LOG_WARNING("Empty key at line " + std::to_string(lineNumber));
            hasErrors = true;
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        try {
            set(key, value, ConfigSource::File);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error parsing config key '" + key + "' at line " + 
                         std::to_string(lineNumber) + ": " + e.what());
            hasErrors = true;
        }
    }
    
    if (hasErrors) {
        NLM_LOG_WARNING("Config file parsing completed with errors");
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool hasErrors = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Convert value based on key name (basic type inference)
                try {
                    setConfigValue(key, value);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error parsing CLI argument --" + key + "=" + value + ": " + e.what());
                    hasErrors = true;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            try {
                setConfigValue(key, value);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error parsing CLI argument -" + key + " " + value + ": " + e.what());
                hasErrors = true;
            }
        }
    }
    
    return !hasErrors;
}

// Helper function to set config value based on key
void Config::setConfigValue(const std::string& key, const std::string& value) {
    // Convert value based on key name (basic type inference)
    if (key == "neuron-count" || key == "random-seed") {
        try {
            int64_t intVal = std::stoll(value);
            set(key, intVal, ConfigSource::CommandLine);
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid integer value for " + key + ": " + value);
        }
    } else if (key == "simulation-timestep" || key == "stdp-tau" || 
               key == "synaptogenesis-rate" || key == "pruning-rate" ||
               key == "connection-prob" || key == "stdp-ltp-weight" ||
               key == "stdp-ltd-weight") {
        try {
            float floatVal = std::stof(value);
            set(key, floatVal, ConfigSource::CommandLine);
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid float value for " + key + ": " + value);
        }
    } else {
        // Try as string first
        set(key, value, ConfigSource::CommandLine);
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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

// Add validation functions after the trim() and toLower() methods

// Validate configuration values
bool Config::validate() const {
    bool isValid = true;
    
    // Validate numeric ranges for known configuration keys
    if (has("neuron_count")) {
        int64_t neurons = getOr<int64_t>("neuron_count", -1);
        if (neurons <= 0) {
            NLM_LOG_ERROR("Invalid neuron_count: must be positive, got " + std::to_string(neurons));
            isValid = false;
        } else if (neurons > 1000000) {
            NLM_LOG_WARNING("Very large neuron_count: " + std::to_string(neurons));
        }
    }
    
    if (has("region_count")) {
        int64_t regions = getOr<int64_t>("region_count", -1);
        if (regions <= 0) {
            NLM_LOG_ERROR("Invalid region_count: must be positive, got " + std::to_string(regions));
            isValid = false;
        }
    }
    
    if (has("connection_probability")) {
        float prob = getOr<float>("connection_probability", -1.0f);
        if (prob < 0.0f || prob > 1.0f) {
            NLM_LOG_ERROR("Invalid connection_probability: must be between 0.0 and 1.0, got " + std::to_string(prob));
            isValid = false;
        }
    }
    
    if (has("simulation_timestep")) {
        double timestep = getOr<double>("simulation_timestep", -1.0);
        if (timestep <= 0.0) {
            NLM_LOG_ERROR("Invalid simulation_timestep: must be positive, got " + std::to_string(timestep));
            isValid = false;
        } else if (timestep > 1.0) {
            NLM_LOG_WARNING("Very large simulation_timestep: " + std::to_string(timestep));
        }
    }
    
    if (has("random_seed")) {
        int64_t seed = getOr<int64_t>("random_seed", -1);
        if (seed < 0) {
            NLM_LOG_ERROR("Invalid random_seed: must be non-negative, got " + std::to_string(seed));
            isValid = false;
        }
    }
    
    // Validate STDP parameters
    if (has("stdp_ltp_weight")) {
        float ltp = getOr<float>("stdp_ltp_weight", -1.0f);
        if (ltp < 0.0f) {
            NLM_LOG_ERROR("Invalid stdp_ltp_weight: must be non-negative, got " + std::to_string(ltp));
            isValid = false;
        }
    }
    
    if (has("stdp_ltd_weight")) {
        float ltd = getOr<float>("stdp_ltd_weight", -1.0f);
        if (ltd < 0.0f) {
            NLM_LOG_ERROR("Invalid stdp_ltd_weight: must be non-negative, got " + std::to_string(ltd));
            isValid = false;
        }
    }
    
    if (has("stdp_tau")) {
        float tau = getOr<float>("stdp_tau", -1.0f);
        if (tau <= 0.0f) {
            NLM_LOG_ERROR("Invalid stdp_tau: must be positive, got " + std::to_string(tau));
            isValid = false;
        }
    }
    
    return isValid;
}

// Get configuration summary with validation
std::string Config::summaryWithValidation() const {
    std::ostringstream oss;
    oss << "Configuration Validation Summary:" << std::endl;
    
    if (validate()) {
        oss << "✓ Configuration is valid" << std::endl;
    } else {
        oss << "✗ Configuration has validation errors" << std::endl;
    }
    
    oss << "\nDetailed Configuration (" << pImpl->entries.size() << " entries):" << std::endl;
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
