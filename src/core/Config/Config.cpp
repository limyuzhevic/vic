#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <nlohmann/json.hpp>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format by extension
    std::string extension = "." + filepath.substr(filepath.find_last_of('.') + 1);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    bool success;
    if (extension == ".json") {
        success = loadFromJSON(content, ConfigSource::File);
    } else if (extension == ".yaml" || extension == ".yml") {
        success = loadFromYAML(content, ConfigSource::File);
    } else {
        // Default to simple key=value format for backward compatibility
        success = loadFromSimpleFormat(content, ConfigSource::File);
    }
    
    return success;
}

bool Config::loadFromJSON(const std::string& jsonString, ConfigSource source, const ParserOptions& options) {
    try {
        auto json = nlohmann::json::parse(jsonString);
        return flattenAndLoadJSON(json, source, options);
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& yamlString, ConfigSource source, const ParserOptions& options) {
    // YAML parsing is not yet implemented
    // TODO: Implement when libyaml-cpp is available
    return false;
}

bool Config::loadFromSimpleFormat(const std::string& content, ConfigSource source) {
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
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
            
            set(key, value, source);
        }
    }
    return true;
}

void Config::flattenAndLoadJSON(const nlohmann::json& json, ConfigSource source, const ParserOptions& options) {
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        const nlohmann::json& value = *it;
        setFromJSONValue(key, value, source);
    }
}

std::string Config::flattenKey(const std::string& key, const nlohmann::json& parent) const {
    // For now, just return the key. Could be enhanced for nested keys
    return key;
}

void Config::setFromJSONValue(const std::string& key, const nlohmann::json& value, ConfigSource source) {
    ConfigValue configValue;
    
    if (value.is_string()) {
        configValue = value.get<std::string>();
    } else if (value.is_number_integer()) {
        configValue = value.get<int>();
    } else if (value.is_number_unsigned()) {
        configValue = value.get<unsigned int>();
    } else if (value.is_number()) {
        configValue = value.get<double>();
    } else if (value.is_boolean()) {
        configValue = value.get<bool>();
    } else if (value.is_array()) {
        if (!value.empty()) {
            // Determine array type
            if (value[0].is_number_integer()) {
                std::vector<int> intArray;
                for (const auto& item : value) {
                    intArray.push_back(item.get<int>());
                }
                configValue = intArray;
            } else if (value[0].is_number()) {
                std::vector<double> doubleArray;
                for (const auto& item : value) {
                    doubleArray.push_back(item.get<double>());
                }
                configValue = doubleArray;
            } else if (value[0].is_string()) {
                std::vector<std::string> stringArray;
                for (const auto& item : value) {
                    stringArray.push_back(item.get<std::string>());
                }
                configValue = stringArray;
            } else {
                // Try to store as generic string array
                std::vector<std::string> stringArray;
                for (const auto& item : value) {
                    stringArray.push_back(item.dump());
                }
                configValue = stringArray;
            }
        } else {
            configValue = std::vector<int>();
        }
    } else if (value.is_object()) {
        // For objects, we'll try to flatten them into key-value pairs
        // For now, just store as string representation
        configValue = value.dump();
    } else {
        configValue = value.dump();
    }
    
    set(key, configValue, source);
}

nlohmann::json Config::buildJSONConfig() const {
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        std::string flattenedKey = entry.key;
        json[flattenedKey] = configValueToJSON(entry.value);
    }
    
    return json;
}

bool Config::validateJSON(const nlohmann::json& config, std::vector<std::string>& errors) {
    // Basic validation - check for required fields if needed
    // For now, just return true for valid JSON
    return true;
}

bool Config::jsonToConfigValue(const nlohmann::json& jsonValue, ConfigValue& configValue) {
    if (jsonValue.is_string()) {
        configValue = jsonValue.get<std::string>();
        return true;
    } else if (jsonValue.is_number_integer()) {
        configValue = jsonValue.get<int>();
        return true;
    } else if (jsonValue.is_number_unsigned()) {
        configValue = jsonValue.get<unsigned int>();
        return true;
    } else if (jsonValue.is_number()) {
        configValue = jsonValue.get<double>();
        return true;
    } else if (jsonValue.is_boolean()) {
        configValue = jsonValue.get<bool>();
        return true;
    } else if (jsonValue.is_array()) {
        if (!jsonValue.empty()) {
            if (jsonValue[0].is_number_integer()) {
                std::vector<int> intArray;
                for (const auto& item : jsonValue) {
                    intArray.push_back(item.get<int>());
                }
                configValue = intArray;
            } else if (jsonValue[0].is_number()) {
                std::vector<double> doubleArray;
                for (const auto& item : jsonValue) {
                    doubleArray.push_back(item.get<double>());
                }
                configValue = doubleArray;
            } else if (jsonValue[0].is_string()) {
                std::vector<std::string> stringArray;
                for (const auto& item : jsonValue) {
                    stringArray.push_back(item.get<std::string>());
                }
                configValue = stringArray;
            } else {
                return false;
            }
        } else {
            configValue = std::vector<int>();
        }
        return true;
    } else {
        return false;
    }
}

nlohmann::json Config::configValueToJSON(const ConfigValue& configValue) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || std::is_same_v<T, unsigned int>) {
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
            return "";
        }
    }, configValue);
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

// Save to file in JSON format
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Build JSON config
    auto json = buildJSONConfig();
    
    // Write JSON to file with indentation
    file << json.dump(4);
    
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
