#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <iostream>

namespace nlm {

// Global schema
std::unordered_map<std::string, ConfigConstraints> Config::configSchema;

// Environment variable naming convention
static constexpr const char* CONFIG_PREFIX = "NLM_";
static constexpr const char* CONFIG_ENV_SEPARATOR = "_";

// Predefined parameter schemas
void initializeConfigSchema() {
    // Neuron count - must be positive integer
    Config::defineParameter("neuron_count", {
        "Number of neurons in the brain",
        "1000",
        [](const ConfigValue& v) {
            if (std::holds_alternative<size_t>(v) || std::holds_alternative<int64_t>(v)) {
                size_t val = std::holds_alternative<size_t>(v) ? 
                    std::get<size_t>(v) : static_cast<size_t>(std::get<int64_t>(v));
                return val > 0 && val <= 1000000;
            }
            return false;
        }
    });
    
    // Region count - must be positive integer
    Config::defineParameter("region_count", {
        "Number of brain regions",
        "1",
        [](const ConfigValue& v) {
            if (std::holds_alternative<size_t>(v) || std::holds_alternative<int64_t>(v)) {
                size_t val = std::holds_alternative<size_t>(v) ? 
                    std::get<size_t>(v) : static_cast<size_t>(std::get<int64_t>(v));
                return val > 0 && val <= 100;
            }
            return false;
        }
    });
    
    // Connection probability - must be between 0 and 1
    Config::defineParameter("connection_probability", {
        "Connection probability for random connectivity",
        "0.1",
        [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v) || std::holds_alternative<float>(v)) {
                double val = std::holds_alternative<double>(v) ? 
                    std::get<double>(v) : static_cast<double>(std::get<float>(v));
                return val >= 0.0 && val <= 1.0;
            }
            return false;
        }
    });
    
    // Random seed - can be any 64-bit integer
    Config::defineParameter("random_seed", {
        "Random seed for reproducible experiments",
        "42",
        [](const ConfigValue& v) {
            return std::holds_alternative<uint64_t>(v) ||
                   std::holds_alternative<int64_t>(v) ||
                   std::holds_alternative<int>(v);
        }
    });
    
    // Simulation timestep - must be positive
    Config::defineParameter("simulation_timestep", {
        "Simulation timestep in seconds",
        "0.001",
        [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v) || std::holds_alternative<float>(v)) {
                double val = std::holds_alternative<double>(v) ? 
                    std::get<double>(v) : static_cast<double>(std::get<float>(v));
                return val > 0.0 && val <= 1.0;
            }
            return false;
        }
    });
    
    // STDP weights - must be positive
    Config::defineParameter("stdp_ltp_weight", {
        "STDP LTP weight",
        "0.01",
        [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v) || std::holds_alternative<float>(v)) {
                double val = std::holds_alternative<double>(v) ? 
                    std::get<double>(v) : static_cast<double>(std::get<float>(v));
                return val >= 0.0;
            }
            return false;
        }
    });
    
    Config::defineParameter("stdp_ltd_weight", {
        "STDP LTD weight",
        "0.012",
        [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v) || std::holds_alternative<float>(v)) {
                double val = std::holds_alternative<double>(v) ? 
                    std::get<double>(v) : static_cast<double>(std::get<float>(v));
                return val >= 0.0;
            }
            return false;
        }
    });
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigConstraints> localSchema;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize global schema on first use
    static bool initialized = []() {
        initializeConfigSchema();
        return true;
    }();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
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
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to parse as number first
            bool parsed = false;
            
            // Try integer parsing (int64_t)
            try {
                size_t pos = 0;
                int64_t intVal = std::stoll(value, &pos);
                if (pos == value.size()) {
                    set(key, intVal, ConfigSource::File);
                    parsed = true;
                }
            } catch (...) {}
            
            // Try double parsing
            if (!parsed) {
                try {
                    size_t pos = 0;
                    double doubleVal = std::stod(value, &pos);
                    if (pos == value.size()) {
                        set(key, doubleVal, ConfigSource::File);
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try boolean parsing
            if (!parsed) {
                if (value == "true" || value == "True" || value == "1") {
                    set(key, true, ConfigSource::File);
                    parsed = true;
                } else if (value == "false" || value == "False" || value == "0") {
                    set(key, false, ConfigSource::File);
                    parsed = true;
                }
            }
            
            // Try string parsing (fallback)
            if (!parsed) {
                set(key, value, ConfigSource::File);
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

bool Config::loadFromEnv(const std::unordered_map<std::string, std::string>& customEnvVars) {
    std::unordered_map<std::string, std::string> envVars = customEnvVars;
    
    // If custom vars not provided, use environment
    if (envVars.empty()) {
        // This would normally use std::getenv, but we'll simulate it
        // In real implementation, would populate from actual environment
        envVars = {
            {"NLM_NEURON_COUNT", "1000"},
            {"NLM_REGION_COUNT", "2"},
            {"NLM_CONNECTION_PROBABILITY", "0.1"},
            {"NLM_RANDOM_SEED", "42"},
            {"NLM_SIMULATION_TIMESTEP", "0.001"},
            {"NLM_STDP_LTP_WEIGHT", "0.01"},
            {"NLM_STDP_LTD_WEIGHT", "0.012"},
            {"NLM_SYNAPTOGENESIS_RATE", "0.0001"},
            {"NLM_PRUNING_RATE", "0.00001"},
            {"NLM_DOPAMINE_BASELINE", "0.1"}
        };
    }
    
    bool loaded = false;
    for (const auto& [envVar, envValue] : envVars) {
        if (envVar.find(CONFIG_PREFIX) == 0 && envVar != CONFIG_PREFIX) {
            std::string key = envVarToKey(envVar);
            
            // Try to parse the environment variable value
            bool parsed = false;
            
            // Try integer parsing
            try {
                size_t pos = 0;
                int64_t intVal = std::stoll(envValue, &pos);
                if (pos == envValue.size()) {
                    set(key, intVal, ConfigSource::EnvironmentVariable);
                    loaded = true;
                    parsed = true;
                }
            } catch (...) {}
            
            // Try double parsing
            if (!parsed) {
                try {
                    size_t pos = 0;
                    double doubleVal = std::stod(envValue, &pos);
                    if (pos == envValue.size()) {
                        set(key, doubleVal, ConfigSource::EnvironmentVariable);
                        loaded = true;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try boolean parsing
            if (!parsed) {
                if (envValue == "true" || envValue == "True" || envValue == "1") {
                    set(key, true, ConfigSource::EnvironmentVariable);
                    loaded = true;
                    parsed = true;
                } else if (envValue == "false" || envValue == "False" || envValue == "0") {
                    set(key, false, ConfigSource::EnvironmentVariable);
                    loaded = true;
                    parsed = true;
                }
            }
            
            // Try string parsing (fallback)
            if (!parsed) {
                set(key, envValue, ConfigSource::EnvironmentVariable);
                loaded = true;
            }
        }
    }
    
    return loaded;
}

bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry.key, entry.value)) {
            std::cerr << "Configuration validation error: Invalid value for parameter " 
                      << "'" << entry.key << "' (source: " 
                      << static_cast<int>(entry.source) << ")" << std::endl;
            return false;
        }
    }
    return true;
}

template<typename T>
std::optional<T> Config::getValidated(const std::string& key) const {
    if (!has(key)) {
        return std::nullopt;
    }
    
    auto val = get<T>(key);
    if (!val) {
        return std::nullopt;
    }
    
    if (!validateValue(key, ConfigValue(val.value()))) {
        return std::nullopt;
    }
    
    return val;
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
    // Validate the value if we have schema constraints
    if (!validateValue(key, value)) {
        std::cerr << "Warning: Configuration value for parameter '" << key 
                  << "' does not match schema constraints" << std::endl;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
        
        // Get parameter info for description
        auto paramInfo = getParameterInfo(key);
        pImpl->entries.back().description = paramInfo.description;
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

void Config::defineParameter(const std::string& key, 
                             const ConfigConstraints& constraints) {
    configSchema[key] = constraints;
}

ConfigConstraints Config::getParameterInfo(const std::string& key) {
    auto it = configSchema.find(key);
    if (it != configSchema.end()) {
        return it->second;
    }
    return {"Unknown parameter", "", [](const ConfigValue&) { return true; }};
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        
        file << "\n";
    }
    
    return true;
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

std::vector<ConfigEntry> Config::getBySource(ConfigSource source) const {
    std::vector<ConfigEntry> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            result.push_back(entry);
        }
    }
    return result;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) {
    auto paramInfo = getParameterInfo(key);
    return paramInfo.validator(value);
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

std::string Config::envVarToKey(const std::string& envVar) {
    if (envVar.find(CONFIG_PREFIX) != 0) {
        return envVar; // Not an NLM environment variable
    }
    
    std::string key = envVar.substr(strlen(CONFIG_PREFIX));
    std::replace(key.begin(), key.end(), CONFIG_ENV_SEPARATOR[0], '_');
    
    return key;
}

std::string Config::keyToEnvVar(const std::string& key) {
    std::string envVar = CONFIG_PREFIX;
    std::replace(key.begin(), key.end(), '_', CONFIG_ENV_SEPARATOR[0]);
    envVar += key;
    return envVar;
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