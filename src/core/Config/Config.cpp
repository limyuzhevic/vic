#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <stdexcept>
#include <regex>

// Include fallback implementations if real ones are not available
#if defined(NLOHMANN_JSON_FALLBACK)
#include "nlohmann/json.hpp"
#endif

#if defined(YAML_CPP_FALLBACK)
#include "yaml-cpp/yaml.h"
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigSchema> schema;
    bool schemaValidationEnabled = true;
    
    // JSON/YAML parsing helpers
    static nlohmann::json configValueToJSON(const ConfigValue& value);
    static ConfigValue JSONToConfigValue(const nlohmann::json& jsonValue);
    static YAML::Node configValueToYAML(const ConfigValue& value);
    static ConfigValue YAMLToConfigValue(const YAML::Node& yamlValue);
};

// Config implementation
inline Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Register default schema entries
    defineSchema("random_seed", ConfigType::Integer, "Random seed for reproducible simulations", 
                 [](const ConfigValue& v) { return validateInt(v, 0, std::numeric_limits<int>::max()); }, 
                 false, "42");
    
    defineSchema("simulation_timestep", ConfigType::Float, "Simulation timestep in seconds",
                 [](const ConfigValue& v) { return validateDouble(v, 0.0, 1.0); },
                 false, "0.001");
    
    defineSchema("neuron_count", ConfigType::Integer, "Number of neurons in the network",
                 [](const ConfigValue& v) { return validateInt(v, 1, 1000000); },
                 false, "1000");
    
    defineSchema("region_count", ConfigType::Integer, "Number of neural regions",
                 [](const ConfigValue& v) { return validateInt(v, 1, 100); },
                 false, "1");
    
    defineSchema("connection_probability", ConfigType::Float, "Probability of synapse formation",
                 [](const ConfigValue& v) { return validateDouble(v, 0.0, 1.0); },
                 false, "0.1");
}

inline Config::~Config() = default;

inline Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

inline Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

inline bool Config::loadFromFile(const std::string& filepath) {
    // TODO: Implement proper JSON/YAML parser
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
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

inline bool Config::loadFromArgs(int argc, char** argv) {
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

inline bool Config::saveToFile(const std::string& filepath) const {
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

inline template<typename T>
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

inline template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

inline void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

inline void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

inline void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

inline void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

inline void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

inline bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

inline void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

inline std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

inline void Config::clear() {
    pImpl->entries.clear();
}

inline std::string Config::summary() const {
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

inline std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

inline std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Explicit template instantiations
inline template std::optional<int> Config::get<int>(const std::string&) const;
inline template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
inline template std::optional<double> Config::get<double>(const std::string&) const;
inline template std::optional<bool> Config::get<bool>(const std::string&) const;
inline template std::optional<std::string> Config::get<std::string>(const std::string&) const;

inline template int Config::getOr<int>(const std::string&, const int&) const;
inline template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
inline template double Config::getOr<double>(const std::string&, const double&) const;
inline template bool Config::getOr<bool>(const std::string&, const bool&) const;
inline template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm