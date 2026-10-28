#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    nlohmann::json jsonConfig;  // For JSON parsing
    YAML::Node yamlConfig;      // For YAML parsing
};

// Helper function to convert ConfigValue to JSON
nlohmann::json configValueToJson(const ConfigValue& value) {
    nlohmann::json result;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = arg;
        }
    }, value);
    return result;
}

// Helper function to convert JSON to ConfigValue
ConfigValue jsonToConfigValue(const nlohmann::json& json) {
    ConfigValue result;
    if (json.is_number_integer()) {
        result = json.get<int>();
    } else if (json.is_number_unsigned()) {
        result = json.get<int64_t>();
    } else if (json.is_number_float()) {
        result = json.get<double>();
    } else if (json.is_boolean()) {
        result = json.get<bool>();
    } else if (json.is_string()) {
        result = json.get<std::string>();
    } else if (json.is_array()) {
        // Check array element type
        if (!json.empty()) {
            auto firstType = json[0].type();
            if (firstType == nlohmann::json::value_t::number_integer) {
                std::vector<int> vec = json.get<std::vector<int>>();
                result = vec;
            } else if (firstType == nlohmann::json::value_t::number_float) {
                std::vector<double> vec = json.get<std::vector<double>>();
                result = vec;
            } else if (firstType == nlohmann::json::value_t::string) {
                std::vector<std::string> vec = json.get<std::vector<std::string>>();
                result = vec;
            }
        }
    }
    return result;
}

// Helper function to convert YAML to ConfigValue
ConfigValue yamlToConfigValue(const YAML::Node& node) {
    ConfigValue result;
    if (node.IsScalar()) {
        if (node.IsInteger()) {
            result = node.as<int>();
        } else if (node.IsDouble()) {
            result = node.as<double>();
        } else if (node.IsBoolean()) {
            result = node.as<bool>();
        } else {
            result = node.as<std::string>();
        }
    } else if (node.IsSequence()) {
        if (!node.empty()) {
            auto firstNode = node[0];
            if (firstNode.IsInteger()) {
                std::vector<int> vec;
                for (const auto& elem : node) {
                    vec.push_back(elem.as<int>());
                }
                result = vec;
            } else if (firstNode.IsDouble()) {
                std::vector<double> vec;
                for (const auto& elem : node) {
                    vec.push_back(elem.as<double>());
                }
                result = vec;
            } else if (firstNode.IsScalar()) {
                std::vector<std::string> vec;
                for (const auto& elem : node) {
                    vec.push_back(elem.as<std::string>());
                }
                result = vec;
            }
        }
    }
    return result;
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    // Read first line to detect format
    if (!std::getline(file, line)) {
        return false;
    }
    
    file.clear();
    file.seekg(0);
    
    // Determine file format
    if (line.find('{') != std::string::npos && line.find('}') != std::string::npos) {
        // Try JSON parsing
        file.clear();
        file.seekg(0);
        try {
            pImpl->jsonConfig = nlohmann::json::parse(file);
            // Convert JSON to Config entries
            pImpl->entries.clear();
            convertJsonToConfig(pImpl->jsonConfig, ConfigSource::File);
            return true;
        } catch (const nlohmann::json::parse_error& e) {
            // Fall back to simple format
        }
    }
    
    if (line.find(": ") != std::string::npos || line.find("- ") != std::string::npos) {
        // Try YAML parsing
        file.clear();
        file.seekg(0);
        try {
            pImpl->yamlConfig = YAML::Load(file);
            // Convert YAML to Config entries
            pImpl->entries.clear();
            convertYamlToConfig(pImpl->yamlConfig, ConfigSource::File);
            return true;
        } catch (const YAML::ParserException& e) {
            // Fall back to simple format
        }
    }
    
    // Fall back to simple key=value format (original implementation)
    file.clear();
    file.seekg(0);
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

// Helper function to convert JSON to ConfigEntry
void Config::convertJsonToConfig(const nlohmann::json& json, ConfigSource source) {
    if (json.is_object()) {
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            ConfigValue value = jsonToConfigValue(it.value());
            std::string description = "";
            
            // Try to extract description if present
            if (it.value().is_object() && it.value().contains("_description")) {
                description = it.value()["_description"].get<std::string>();
            }
            
            pImpl->entries.emplace_back(key, value, source, description);
        }
    }
}

// Helper function to convert YAML to ConfigEntry
void Config::convertYamlToConfig(const YAML::Node& node, ConfigSource source) {
    if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            std::string key = it->first.as<std::string>();
            ConfigValue value = yamlToConfigValue(it->second);
            std::string description = "";
            
            // Try to extract description if present
            if (it->second.IsMap() && it->second["_description"]) {
                description = it->second["_description"].as<std::string>();
            }
            
            pImpl->entries.emplace_back(key, value, source, description);
        }
    }
// Helper function to convert JSON to Config entries (public version)
void Config::convertJsonToConfig(const nlohmann::json& json, ConfigSource source) {
    if (json.is_object()) {
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            ConfigValue value = jsonToConfigValue(it.value());
            std::string description = "";
            
            // Try to extract description if present
            if (it.value().is_object() && it.value().contains("_description")) {
                description = it.value()["_description"].get<std::string>();
            }
            
            pImpl->entries.emplace_back(key, value, source, description);
        }
    }
}

// Helper function to convert YAML to Config entries (public version)
void Config::convertYamlToConfig(const YAML::Node& node, ConfigSource source) {
    if (node.IsMap()) {
        for (auto it = node.begin(); it != node.end(); ++it) {
            std::string key = it->first.as<std::string>();
            ConfigValue value = yamlToConfigValue(it->second);
            std::string description = "";
            
            // Try to extract description if present
            if (it->second.IsMap() && it->second["_description"]) {
                description = it->second["_description"].as<std::string>();
            }
            
            pImpl->entries.emplace_back(key, value, source, description);
        }
    }
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
