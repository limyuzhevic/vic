#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <regex>
#include <stdexcept>

// Include JSON/YAML libraries if available
#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

#ifdef HAVE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Parse helpers for JSON/YAML
    static std::unordered_map<std::string, std::string> parseJson(const std::string& jsonStr);
    static std::unordered_map<std::string, std::string> parseYaml(const std::string& yamlStr);
    static ConfigValue convertStringToConfigValue(const std::string& str, bool& success);
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Determine file format based on extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Remove comments and validate
    content = trim(content);
    
    std::unordered_map<std::string, std::string> parsedEntries;
    bool parseSuccess = false;
    
    // Precedence check for JSON/YAML libraries
    bool haveJson = false;
    bool haveYaml = false;
    
#if defined(HAVE_NLOHMANN_JSON)
    haveJson = true;
#endif
    
#if defined(HAVE_YAML_CPP)
    haveYaml = true;
#endif
    
    if (ext == ".json" && haveJson) {
        try {
            // Try to parse as JSON
            auto json = nlohmann::json::parse(content);
            parseJsonRecursive(json, "", parsedEntries);
            parseSuccess = true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to parse JSON config file " + filepath + ": " + std::string(e.what()));
            return false;
        }
    } else if (ext == ".yaml" || ext == ".yml" && haveYaml) {
        try {
            // Try to parse as YAML
            auto yaml = YAML::Load(content);
            parseYamlRecursive(yaml, "", parsedEntries);
            parseSuccess = true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to parse YAML config file " + filepath + ": " + std::string(e.what()));
            return false;
        }
    } else if (ext == ".cfg" || ext == ".txt" || ext.empty() || ext == ".unknown") {
        // Legacy support for simple key=value format files
        std::istringstream lineStream(content);
        std::string line;
        while (std::getline(lineStream, line)) {
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
                
                parsedEntries[key] = value;
            }
        }
        parseSuccess = true;
    } else {
        NLM_LOG_ERROR("Unsupported config file format: " + ext + " for file " + filepath);
        return false;
    }
    
    if (!parseSuccess) {
        NLM_LOG_ERROR("Failed to parse config file: " + filepath);
        return false;
    }
    
    // Clear existing entries
    pImpl->entries.clear();
    
    // Convert string values to appropriate ConfigValue types
    for (const auto& pair : parsedEntries) {
        bool success = false;
        ConfigValue configValue = Impl::convertStringToConfigValue(pair.second, success);
        if (success) {
            pImpl->entries.emplace_back(pair.first, configValue, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Could not parse config value for key \"" + pair.first + "\": \"" + pair.second + "\"");
            pImpl->entries.emplace_back(pair.first, ConfigValue(pair.second), ConfigSource::File);
        }
    }
    
    NLM_LOG_INFO("Successfully loaded configuration from: " + filepath);
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

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
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

#include <stdexcept>

// Implementation of JSON/YAML parsing helpers
void Config::Impl::parseJsonRecursive(const nlohmann::json& json, const std::string& prefix, std::unordered_map<std::string, std::string>& parsedEntries) {
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        std::string fullKey = prefix.empty() ? key : prefix + "." + key;
        
        if (it.is_object()) {
            parseJsonRecursive(*it, fullKey, parsedEntries);
        } else if (it.is_array()) {
            // Convert array to JSON string representation for compatibility
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < it.size(); ++i) {
                if (i > 0) oss << ",";
                oss << *it[i];
            }
            oss << "]";
            parsedEntries[fullKey] = oss.str();
        } else {
            // Convert JSON value to string
            parsedEntries[fullKey] = it->dump();
        }
    }
}

void Config::Impl::parseYamlRecursive(const YAML::Node& yaml, const std::string& prefix, std::unordered_map<std::string, std::string>& parsedEntries) {
    for (auto it = yaml.begin(); it != yaml.end(); ++it) {
        std::string key = it->first.as<std::string>();
        std::string fullKey = prefix.empty() ? key : prefix + "." + key;
        
        if (it->second.IsMap()) {
            parseYamlRecursive(it->second, fullKey, parsedEntries);
        } else if (it->second.IsSequence()) {
            // Convert YAML sequence to JSON string representation
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < it->second.size(); ++i) {
                if (i > 0) oss << ",";
                oss << it->second[i].to_string();
            }
            oss << "]";
            parsedEntries[fullKey] = oss.str();
        } else {
            // Convert YAML value to string
            parsedEntries[fullKey] = it->second.to_string();
        }
    }
}

ConfigValue Config::Impl::convertStringToConfigValue(const std::string& str, bool& success) {
    success = false;
    
    // Try integer conversion
    try {
        if (str.find('.') == std::string::npos && str.find('e') == std::string::npos) {
            size_t pos;
            long long_val = std::stol(str, &pos);
            if (pos == str.size()) {
                success = true;
                return ConfigValue(long_val);
            }
        }
    } catch (...) {}
    
    // Try double conversion
    try {
        double double_val = std::stod(str);
        success = true;
        return ConfigValue(double_val);
    } catch (...) {}
    
    // Try bool conversion
    if (str == "true" || str == "false") {
        bool bool_val = (str == "true");
        success = true;
        return ConfigValue(bool_val);
    }
    
    // Check if it's a JSON array
    if (str.size() >= 2 && str.front() == '[' && str.back() == ']') {
        // Parse as array - very basic implementation
        success = true;
        return ConfigValue(str); // Return as string for now
    }
    
    // Default to string
    success = true;
    return ConfigValue(str);
}

// Update the loadFromArgs method to support nested key=value format
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
