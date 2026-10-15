#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <regex>
#include <map>
#include <vector>
#include <stdexcept>

// Include nlohmann/json if available
#ifdef NLM_USE_JSON
#include "nlohmann/json.hpp"
#endif

// Include YAML-cpp if available  
#ifdef NLM_USE_YAML
#include "yaml-cpp/yaml.h"
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON format first (if enabled)
#ifdef NLM_USE_JSON
    // Try to parse as JSON
    std::ifstream jsonFile(filepath);
    if (jsonFile.good()) {
        nlohmann::json jsonData;
        try {
            jsonFile >> jsonData;
            return loadFromJson(jsonData);
        } catch (const nlohmann::json::parse_error& e) {
            NLM_LOG_WARN("JSON parse error: " + std::string(e.what()) + ". Trying other formats.");
        }
    }
#endif

    // Try YAML format second (if enabled)
#ifdef NLM_USE_YAML
    // Try to parse as YAML
    std::ifstream yamlFile(filepath);
    if (yamlFile.good()) {
        try {
            YAML::Node yamlData = YAML::LoadFile(filepath);
            return loadFromYaml(yamlData);
        } catch (const YAML::Exception& e) {
            NLM_LOG_WARN("YAML parse error: " + std::string(e.what()) + ". Trying other formats.");
        }
    }
#endif

    // Fall back to the basic key=value format
    return loadFromKeyValue(filepath);
}

// Load from key=value format (original implementation)
bool Config::loadFromKeyValue(const std::string& filepath) {
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
        } else {
            // Invalid line format
            NLM_LOG_WARN("Invalid config line (no '=' found): " + line);
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
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

// Load configuration from JSON data
#ifdef NLM_USE_JSON
bool Config::loadFromJson(const nlohmann::json& jsonData) {
    try {
        for (const auto& [key, value] : jsonData.items()) {
            set(key, value, ConfigSource::File);
        }
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON config: " + std::string(e.what()));
        return false;
    }
}

// Load configuration from YAML data  
#ifdef NLM_USE_YAML
bool Config::loadFromYaml(const YAML::Node& yamlData) {
    try {
        // Convert YAML to our internal format
        std::function<void(const YAML::Node&, const std::string&)> convertNode = 
            [&](const YAML::Node& node, const std::string& prefix) {
                if (node.IsScalar()) {
                    set(prefix, node.as<std::string>(), ConfigSource::File);
                } else if (node.IsSequence()) {
                    // Convert to vector representation
                    std::vector<std::string> values;
                    for (const auto& item : node) {
                        if (item.IsScalar()) {
                            values.push_back(item.as<std::string>());
                        }
                    }
                    set(prefix, values, ConfigSource::File);
                } else if (node.IsMap()) {
                    // Handle nested objects - convert to flattened key with dot notation
                    for (const auto& kv : node) {
                        std::string newPrefix = prefix.empty() ? kv.first.as<std::string>() : 
                                               prefix + "." + kv.first.as<std::string>();
                        convertNode(kv.second, newPrefix);
                    }
                }
            };
        
        convertNode(yamlData, "");
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing YAML config: " + std::string(e.what()));
        return false;
    }
}
#endif

// Save configuration to JSON file (for backward compatibility)
bool Config::saveToJson(const std::string& filepath) const {
#ifdef NLM_USE_JSON
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json jsonData;
    for (const auto& entry : pImpl->entries) {
        std::visit([&jsonData, &entry](auto&& arg) {
            jsonData[entry.key] = arg;
        }, entry.value);
    }
    
    file << jsonData.dump(2); // Pretty print with 2-space indentation
    return true;
#else
    NLM_LOG_ERROR("JSON support not compiled in");
    return false;
#endif
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
