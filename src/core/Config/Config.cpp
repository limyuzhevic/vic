#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <typeinfo>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file format based on extension
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    try {
        if (ext == ".json" || ext == ".json") {
            nlohmann::json j;
            file >> j;
            loadFromJSON(j);
        } else if (ext == ".yaml" || ext == ".yml") {
            YAML::Node yamlNode = YAML::Load(file);
            loadFromYAML(yamlNode);
        } else {
            // Default to simple key=value format for backward compatibility
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
                    
                    // Try to determine type and set appropriately
                    setFromString(key, value);
                }
            }
        }
        return true;
    } catch (const std::exception& e) {
        // Log error
        return false;
    }
}

// Helper method to load from nlohmann::json
void Config::loadFromJSON(const nlohmann::json& j) {
    if (!j.is_object()) {
        return;
    }
    
    for (auto it = j.begin(); it != j.end(); ++it) {
        std::string key = it.key();
        const nlohmann::json& value = it.value();
        setFromJSONValue(key, value);
    }
}

// Helper method to load from YAML::Node
void Config::loadFromYAML(const YAML::Node& yamlNode) {
    if (!yamlNode.IsMap()) {
        return;
    }
    
    for (auto it = yamlNode.begin(); it != yamlNode.end(); ++it) {
        std::string key = it->first.as<std::string>();
        setFromYAMLNode(key, *it);
    }
}

// Helper method to set value from JSON
void Config::setFromJSONValue(const std::string& key, const nlohmann::json& value) {
    if (value.is_null()) {
        return;
    }
    
    // Try different JSON types
    if (value.is_boolean()) {
        set(key, value.get<bool>(), ConfigSource::File);
    } else if (value.is_number_integer()) {
        set(key, value.get<int64_t>(), ConfigSource::File);
    } else if (value.is_number_unsigned()) {
        set(key, value.get<uint64_t>(), ConfigSource::File);
    } else if (value.is_number_float()) {
        set(key, value.get<double>(), ConfigSource::File);
    } else if (value.is_string()) {
        set(key, value.get<std::string>(), ConfigSource::File);
    } else if (value.is_array()) {
        // Handle arrays
        setFromJSONArray(key, value);
    } else if (value.is_object()) {
        // For nested objects, convert to JSON string for now
        set(key, value.dump(), ConfigSource::File);
    }
}

// Helper method to set value from YAML node
void Config::setFromYAMLNode(const std::string& key, const YAML::Node& node) {
    if (!node) {
        return;
    }
    
    // Try different YAML types
    if (node.IsScalar()) {
        // Try to determine scalar type
        if (node.IsBool()) {
            set(key, node.as<bool>(), ConfigSource::File);
        } else if (node.IsInt()) {
            set(key, node.as<int64_t>(), ConfigSource::File);
        } else if (node.IsFloat()) {
            set(key, node.as<double>(), ConfigSource::File);
        } else {
            // Default to string
            set(key, node.as<std::string>(), ConfigSource::File);
        }
    } else if (node.IsSequence()) {
        // Handle arrays/sequences
        setFromYAMLSequence(key, node);
    } else if (node.IsMap()) {
        // For nested maps, convert to YAML string for now
        YAML::Emitter emitter;
        emitter << node;
        set(key, emitter.str(), ConfigSource::File);
    }
}

// Helper method to set from JSON array
void Config::setFromJSONArray(const std::string& key, const nlohmann::json& array) {
    if (!array.empty()) {
        // Determine array type based on first element
        const nlohmann::json& first = array[0];
        
        if (first.is_number_integer()) {
            std::vector<int64_t> intArray;
            intArray.reserve(array.size());
            for (const auto& item : array) {
                intArray.push_back(item.get<int64_t>());
            }
            set(key, intArray, ConfigSource::File);
        } else if (first.is_number_float()) {
            std::vector<double> doubleArray;
            doubleArray.reserve(array.size());
            for (const auto& item : array) {
                doubleArray.push_back(item.get<double>());
            }
            set(key, doubleArray, ConfigSource::File);
        } else if (first.is_string()) {
            std::vector<std::string> stringArray;
            stringArray.reserve(array.size());
            for (const auto& item : array) {
                stringArray.push_back(item.get<std::string>());
            }
            set(key, stringArray, ConfigSource::File);
        }
    }
}

// Helper method to set from YAML sequence
void Config::setFromYAMLSequence(const std::string& key, const YAML::Node& sequence) {
    if (!sequence) {
        return;
    }
    
    std::vector<YAML::Node> items;
    for (const auto& item : sequence) {
        items.push_back(item);
    }
    
    if (!items.empty()) {
        // Determine sequence type based on first element
        if (items[0].IsInt()) {
            std::vector<int64_t> intArray;
            intArray.reserve(items.size());
            for (const auto& item : items) {
                intArray.push_back(item.as<int64_t>());
            }
            set(key, intArray, ConfigSource::File);
        } else if (items[0].IsFloat()) {
            std::vector<double> doubleArray;
            doubleArray.reserve(items.size());
            for (const auto& item : items) {
                doubleArray.push_back(item.as<double>());
            }
            set(key, doubleArray, ConfigSource::File);
        } else if (items[0].IsScalar()) {
            // Try to detect if it's a boolean or string
            std::vector<std::string> stringArray;
            stringArray.reserve(items.size());
            for (const auto& item : items) {
                if (item.IsBool()) {
                    stringArray.push_back(item.as<bool>() ? "true" : "false");
                } else {
                    stringArray.push_back(item.as<std::string>());
                }
            }
            set(key, stringArray, ConfigSource::File);
        }
    }
}

// Helper method to set value from string
void Config::setFromString(const std::string& key, const std::string& valueStr) {
    // Try to determine type from string content
    // Remove whitespace
    std::string trimmed = trim(valueStr);
    
    if (trimmed.empty()) {
        return;
    }
    
    // Check for boolean
    if (trimmed == "true" || trimmed == "false") {
        set(key, trimmed == "true", ConfigSource::File);
        return;
    }
    
    // Check for number
    try {
        size_t pos;
        int intVal = std::stoi(trimmed, &pos);
        if (pos == trimmed.size()) {
            set(key, intVal, ConfigSource::File);
            return;
        }
    } catch (...) {}
    
    try {
        size_t pos;
        double doubleVal = std::stod(trimmed, &pos);
        if (pos == trimmed.size()) {
            set(key, doubleVal, ConfigSource::File);
            return;
        }
    } catch (...) {}
    
    // Default to string
    set(key, trimmed, ConfigSource::File);
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
