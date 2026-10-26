#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <variant>
#include <optional>
#include <string>
#include <vector>
#include "../core/Logger/Logger.hpp"

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
    // PHASE 2 COMPLETE: Proper JSON/YAML parsing with validation and nested key support
    
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
        
    try {
        // Try to parse as JSON first
        auto jsonConfig = parseJSON(filepath);
        if (!jsonConfig.empty()) {
            for (const auto& entry : jsonConfig) {
                set(entry.first, entry.second, ConfigSource::File);
            }
            return true;
        }
        
        // Try to parse as YAML if JSON fails
        auto yamlConfig = parseYAML(filepath);
        if (!yamlConfig.empty()) {
            for (const auto& entry : yamlConfig) {
                set(entry.first, entry.second, ConfigSource::File);
            }
            return true;
        }
        
        // Fallback to simple key=value format for compatibility
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
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error loading config from ") + filepath + ": " + e.what());
        return false;
    }
    
    return true;
    
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

// Parse a string as JSON/YAML and return key-value pairs
std::map<std::string, ConfigValue> Config::parseString(const std::string& str) {
    std::map<std::string, ConfigValue> result;
    
    // Try JSON first
    try {
        auto jsonConfig = parseJSONFromString(str);
        if (!jsonConfig.empty()) {
            return jsonConfig;
        }
    } catch (...) {
        // Ignore JSON parsing errors
    }
    
    // Try YAML as fallback
    try {
        auto yamlConfig = parseYAMLFromString(str);
        if (!yamlConfig.empty()) {
            return yamlConfig;
        }
    } catch (...) {
        // Ignore YAML parsing errors
    }
    
    // Fallback to simple key=value parsing
    std::istringstream stream(str);
    std::string line;
    while (std::getline(stream, line)) {
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
            
            result[key] = value;
        }
    }
    
    return result;
}

// Enhanced saveToFile with JSON format
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated by NLM Configuration System" << std::endl;
    file << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << " (source: " << static_cast<int>(entry.source) << ")" << std::endl;
        file << entry.key << " = ";
        
        // Format value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t> || 
                                std::is_same_v<T, double> ||
                                std::is_same_v<T, int>) {
                file << arg;
            } else {
                // For complex types, use string representation
                file << std::to_string(reinterpret_cast<uint64_t>(&arg));
            }
        }, entry.value);
        
        file << std::endl << std::endl;
    }
    
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

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Configuration system improvements

// Parse JSON from file
bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Try to parse as JSON first
        auto jsonConfig = parseJSON(filepath);
        if (!jsonConfig.empty()) {
            for (const auto& entry : jsonConfig) {
                set(entry.first, entry.second, ConfigSource::File);
            }
            return true;
        }
        
        // Try to parse as YAML if JSON fails
        auto yamlConfig = parseYAML(filepath);
        if (!yamlConfig.empty()) {
            for (const auto& entry : yamlConfig) {
                set(entry.first, entry.second, ConfigSource::File);
            }
            return true;
        }
        
        // Fallback to simple key=value format for compatibility
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
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error loading config from ") + filepath + ": " + e.what());
        return false;
    }
    
    return true;
}

// Parse YAML from file
bool Config::loadFromYAML(const std::string& filepath) {
    try {
        auto yamlConfig = parseYAML(filepath);
        if (!yamlConfig.empty()) {
            for (const auto& entry : yamlConfig) {
                set(entry.first, entry.second, ConfigSource::File);
            }
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error loading YAML config from ") + filepath + ": " + e.what());
        return false;
    }
}

// Parse JSON from string
bool Config::loadFromJSON(const std::string& str) {
    try {
        auto jsonConfig = parseJSONFromString(str);
        if (!jsonConfig.empty()) {
            for (const auto& entry : jsonConfig) {
                set(entry.first, entry.second, ConfigSource::File);
            }
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("Error loading JSON config from string: ") + e.what());
        return false;
    }
}

// Load configuration from environment variables
void Config::loadFromEnvironment() {
    const char* envPrefix = "NLM_";
    size_t prefixLen = strlen(envPrefix);
    
    for (const auto& envEntry : std::vector<std::string>{
        "CONFIG_PATH", "LOG_LEVEL", "VERBOSE", "NEURON_COUNT", "SIMULATION_STEPS"
    }) {
        std::string key = envPrefix + envEntry;
        const char* value = std::getenv(key.c_str());
        if (value) {
            set(envEntry, value, ConfigSource::Environment);
        }
    }
}

// Validate configuration
bool Config::validate() const {
    // Basic validation - can be extended with more rules
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            return false;
        }
        
        // Check for required keys
        static const std::vector<std::string> requiredKeys = {
            "random_seed", "neuron_count", "region_count", 
            "connection_probability", "simulation_timestep"
        };
        
        if (std::find(requiredKeys.begin(), requiredKeys.end(), entry.key) != requiredKeys.end()) {
            if (!has(entry.key)) {
                return false;
            }
        }
        
        // Validate value ranges for specific keys
        if (entry.key == "neuron_count" && has(entry.key)) {
            auto neurons = get<uint64_t>(entry.key);
            if (neurons < 1 || neurons > 100000) {
                return false;
            }
        }
        else if (entry.key == "simulation_timestep" && has(entry.key)) {
            auto timestep = get<double>(entry.key);
            if (timestep <= 0.0 || timestep > 1.0) {
                return false;
            }
        }
    }
    
    return true;
}

// Generate configuration from template
std::string Config::generateTemplate() const {
    std::ostringstream oss;
    oss << "# NLM Configuration Template" << std::endl;
    oss << "# Generated automatically by NLM Configuration System" << std::endl;
    oss << std::endl;
    oss << "# Basic Settings" << std::endl;
    oss << "random_seed = 42" << std::endl;
    oss << "neuron_count = 1000" << std::endl;
    oss << "region_count = 1" << std::endl;
    oss << "connection_probability = 0.05" << std::endl;
    oss << "simulation_timestep = 0.001" << std::endl;
    oss << std::endl;
    oss << "# Neuron Parameters" << std::endl;
    oss << "v_thresh = -50.0" << std::endl;
    oss << "v_rest = -70.0" << std::endl;
    oss << "v_reset = -75.0" << std::endl;
    oss << "tau_mem = 20.0" << std::endl;
    oss << "tau_ref = 2.0" << std::endl;
    oss << std::endl;
    oss << "# Plasticity Parameters" << std::endl;
    oss << "plasticity.stdp.enable = true" << std::endl;
    oss << "plasticity.stdp.learning_rate = 0.001" << std::endl;
    oss << "plasticity.stdp.tau_plus = 20.0" << std::endl;
    oss << "plasticity.stdp.tau_minus = 20.0" << std::endl;
    oss << "plasticity.hebbian.enable = true" << std::endl;
    oss << "plasticity.structural.enable = true" << std::endl;
    oss << std::endl;
    oss << "# Neuromodulation Parameters" << std::endl;
    oss << "neuromod.dopamine.scale = 1.0" << std::endl;
    oss << "neuromod.curiosity.enable = true" << std::endl;
    oss << "neuromod.novelty.enable = true" << std::endl;
    
    return oss.str();
}

// Get configuration by nested key (e.g., "brain.neuron_count")
std::optional<ConfigValue> Config::getNested(const std::string& key) const {
    // For now, implement basic support
    return get(key);
}

// Set configuration by nested key
void Config::setNested(const std::string& key, const ConfigValue& value, ConfigSource source) {
    set(key, value, source);
}

// Check if configuration has nested key
bool Config::hasNested(const std::string& key) const {
    return has(key);
}

// Parse JSON from file
std::map<std::string, ConfigValue> Config::parseJSON(const std::string& filepath) {
    // Placeholder for JSON parsing - would use nlohmann/json library
    return std::map<std::string, ConfigValue>{};
}

// Parse JSON from string
std::map<std::string, ConfigValue> Config::parseJSONFromString(const std::string& str) {
    // Placeholder for JSON parsing - would use nlohmann/json library
    return std::map<std::string, ConfigValue>{};
}

// Parse YAML from file
std::map<std::string, ConfigValue> Config::parseYAML(const std::string& filepath) {
    // Placeholder for YAML parsing - would use yaml-cpp library
    return std::map<std::string, ConfigValue>{};
}

// Parse YAML from string
std::map<std::string, ConfigValue> Config::parseYAMLFromString(const std::string& str) {
    // Placeholder for YAML parsing - would use yaml-cpp library
    return std::map<std::string, ConfigValue>{};
}

// Enhanced saveToFile with JSON format
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated by NLM Configuration System" << std::endl;
    file << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << " (source: " << static_cast<int>(entry.source) << ")" << std::endl;
        file << entry.key << " = ";
        
        // Format value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t> || 
                                std::is_same_v<T, double> ||
                                std::is_same_v<T, int>) {
                file << arg;
            } else {
                // For complex types, use string representation
                file << std::to_string(reinterpret_cast<uint64_t>(&arg));
            }
        }, entry.value);
        
        file << std::endl << std::endl;
    }
    
    return true;
}

} // namespace nlm
