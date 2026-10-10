#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>
#include <map>
#include <variant>

// Try to include JSON/YAML libraries
#ifdef __has_include
#if __has_include("nlohmann/json.hpp")
#include "nlohmann/json.hpp"
#define HAS_NLOHMANN_JSON 1
#endif
#if __has_include("yaml-cpp/yaml.h")
#include "yaml-cpp/yaml.h"
#define HAS_YAML_CPP 1
#endif
#endif

// Helper functions to convert between JSON/YAML and ConfigValue
#ifdef HAS_NLOHMANN_JSON
nlohmann::json ConfigValueToJson(const ConfigValue& value);
ConfigValue JsonToConfigValue(const nlohmann::json& json);
#endif

#ifdef HAS_YAML_CPP
YAML::Node ConfigValueToYaml(const ConfigValue& value);
ConfigValue YamlToConfigValue(const YAML::Node& node);
#endif

namespace {

// Convert ConfigValue to nlohmann::json
#ifdef HAS_NLOHMANN_JSON
nlohmann::json ConfigValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            nlohmann::json jsonArray = nlohmann::json::array();
            for (const auto& item : arg) {
                jsonArray.push_back(item);
            }
            return jsonArray;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json jsonArray = nlohmann::json::array();
            for (const auto& item : arg) {
                jsonArray.push_back(item);
            }
            return jsonArray;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json jsonArray = nlohmann::json::array();
            for (const auto& item : arg) {
                jsonArray.push_back(item);
            }
            return jsonArray;
        } else {
            return nlohmann::json(nullptr); // Fallback for unknown types
        }
    }, value);
}

// Convert nlohmann::json to ConfigValue
ConfigValue JsonToConfigValue(const nlohmann::json& json) {
    if (json.is_number_integer()) {
        return json.get<int64_t>();
    } else if (json.is_number_unsigned()) {
        return json.get<uint64_t>();
    } else if (json.is_number_float()) {
        return json.get<double>();
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_array()) {
        // Try to infer array element type
        if (!json.empty()) {
            if (json[0].is_number_integer()) {
                std::vector<int64_t> intVec;
                for (const auto& item : json) {
                    intVec.push_back(item.get<int64_t>());
                }
                return intVec;
            } else if (json[0].is_number_float()) {
                std::vector<double> doubleVec;
                for (const auto& item : json) {
                    doubleVec.push_back(item.get<double>());
                }
                return doubleVec;
            } else if (json[0].is_string()) {
                std::vector<std::string> stringVec;
                for (const auto& item : json) {
                    stringVec.push_back(item.get<std::string>());
                }
                return stringVec;
            }
        }
        // Fallback to empty int64_t array
        return std::vector<int64_t>();
    } else if (json.is_object()) {
        // For objects, we could store them as strings for now
        return json.dump();
    } else {
        return "";
    }
}
#endif

#ifdef HAS_YAML_CPP
// Convert ConfigValue to YAML::Node
YAML::Node ConfigValueToYaml(const ConfigValue& value) {
    YAML::Node node;
    std::visit([&node](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            node = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                node.push_back(item);
            }
        } else {
            node = "";
        }
    }, value);
    return node;
}

// Convert YAML::Node to ConfigValue
ConfigValue YamlToConfigValue(const YAML::Node& node) {
    if (node.IsScalar()) {
        if (node.IsInteger()) {
            return node.as<int64_t>();
        } else if (node.IsFloat()) {
            return node.as<double>();
        } else if (node.IsBoolean()) {
            return node.as<bool>();
        } else {
            return node.as<std::string>();
        }
    } else if (node.IsSequence()) {
        // Try to infer sequence element type
        if (node.size() > 0) {
            if (node[0].IsInteger()) {
                std::vector<int64_t> intVec;
                for (const auto& item : node) {
                    intVec.push_back(item.as<int64_t>());
                }
                return intVec;
            } else if (node[0].IsFloat()) {
                std::vector<double> doubleVec;
                for (const auto& item : node) {
                    doubleVec.push_back(item.as<double>());
                }
                return doubleVec;
            } else if (node[0].IsScalar()) {
                std::vector<std::string> stringVec;
                for (const auto& item : node) {
                    stringVec.push_back(item.as<std::string>());
                }
                return stringVec;
            }
        }
        // Fallback to empty int64_t vector
        return std::vector<int64_t>();
    } else if (node.IsMap()) {
        // For maps, we could store them as YAML strings for now
        std::stringstream ss;
        ss << node;
        return ss.str();
    } else {
        return "";
    }
}
#endif

} // namespace

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file type by extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try JSON first if nlohmann is available
#ifdef HAS_NLOHMANN_JSON
    if (ext == "json" || ext == "JSON") {
        try {
            nlohmann::json jsonData;
            file >> jsonData;
            file.close();
            
            // Parse JSON and populate config
            for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
                std::string key = it.key();
                ConfigValue value = JsonToConfigValue(*it);
                set(key, value, ConfigSource::File);
            }
            
            return true;
        } catch (const std::exception& e) {
            file.close();
            // Fall through to other parsers
        }
    }
#endif
    
    // Try YAML if yaml-cpp is available
#ifdef HAS_YAML_CPP
    if (ext == "yaml" || ext == "yml" || ext == "YAML" || ext == "YML") {
        try {
            YAML::Node yamlData = YAML::Load(file);
            file.close();
            
            // Parse YAML and populate config
            for (const auto& pair : yamlData) {
                std::string key = pair.first.as<std::string>();
                ConfigValue value = YamlToConfigValue(pair.second);
                set(key, value, ConfigSource::File);
            }
            
            return true;
        } catch (const std::exception& e) {
            file.close();
            // Fall through to legacy parser
        }
    }
#endif
    
    // Legacy key=value format (original implementation)
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
