#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <system_error>
#include <optional>
#include <regex>
#include <ctime>

// Include nlohmann::json and YAML-cpp
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Implementation of value type conversion functions
ConfigValue stringToValue(const std::string& str);

// Helper to find entry by key
ConfigEntry* findEntry(const std::string& key);
const ConfigEntry* findEntry(const std::string& key) const;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Determine file format by extension
        std::string ext = "." + std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == ".yaml" || ext == ".yml") {
            return loadFromYAML(filepath);
        } else if (ext == ".json") {
            return loadFromJSON(filepath);
        } else {
            // Fallback to simple key=value format for backward compatibility
            return loadFromSimpleFormat(filepath);
        }
    } catch (const std::exception& e) {
        // TODO: Add proper error logging
        return false;
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        clear();
        
        // Recursively process JSON data
        processJSONData(jsonData, ConfigSource::File);
        
        return true;
    } catch (const std::exception& e) {
        // TODO: Add proper error logging
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        clear();
        
        // Recursively process YAML data
        processYAMLData(yamlData, ConfigSource::File);
        
        return true;
    } catch (const std::exception& e) {
        // TODO: Add proper error logging
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear();
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
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

void Config::processJSONData(const nlohmann::json& data, ConfigSource source) {
    if (data.is_object()) {
        for (const auto& [key, value] : data.items()) {
            std::string description = "";
            
            // Check for nested description
            auto it = data.find(key);
            if (it != data.items().end() && it->contains("_description")) {
                description = it->value("_description", "");
            }
            
            // Recursively process nested data
            processJSONData(value, source);
            
            // Set the value with its description
            set(key, valueToConfigValue(value), source);
            if (!description.empty()) {
                auto* entry = findEntry(key);
                if (entry) {
                    entry->description = description;
                }
            }
        }
    } else {
        // Handle standalone values or array of values
        // This is a simplification - might need to handle differently
    }
}

void Config::processYAMLData(const YAML::Node& data, ConfigSource source) {
    if (data.IsMap()) {
        for (const auto& pair : data) {
            std::string key = pair.first.Scalar();
            std::string description = "";
            
            // Check for nested description
            if (data[key].IsMap() && data[key].FindKey(YAML::NodeType::Scalar, "_description")) {
                description = data[key]["_description"].Scalar();
            }
            
            // Recursively process nested data
            processYAMLData(pair.second, source);
            
            // Set the value with its description
            set(key, valueToConfigValue(pair.second), source);
            if (!description.empty()) {
                auto* entry = findEntry(key);
                if (entry) {
                    entry->description = description;
                }
            }
        }
    } else if (data.IsSequence()) {
        // Handle arrays
        // This is a simplification - might need to handle differently
    }
}

ConfigValue Config::valueToConfigValue(const nlohmann::json& value) {
    if (value.is_string()) {
        return value.get<std::string>();
    } else if (value.is_number_integer()) {
        return value.get<int>();
    } else if (value.is_number_unsigned()) {
        return value.get<uint64_t>();
    } else if (value.is_number_float()) {
        return value.get<double>();
    } else if (value.is_boolean()) {
        return value.get<bool>();
    } else if (value.is_array()) {
        // Handle array type - determine element type
        if (!value.empty()) {
            if (value[0].is_number_integer()) {
                std::vector<int> intArray;
                for (const auto& item : value) {
                    intArray.push_back(item.get<int>());
                }
                return intArray;
            } else if (value[0].is_number_float()) {
                std::vector<double> doubleArray;
                for (const auto& item : value) {
                    doubleArray.push_back(item.get<double>());
                }
                return doubleArray;
            } else if (value[0].is_string()) {
                std::vector<std::string> stringArray;
                for (const auto& item : value) {
                    stringArray.push_back(item.get<std::string>());
                }
                return stringArray;
            }
        }
        // Default to empty vector
        return std::vector<int>{};
    }
    return "";
}

ConfigValue Config::valueToConfigValue(const YAML::Node& value) {
    if (value.IsScalar()) {
        std::string scalar = value.Scalar();
        
        // Try to determine type from scalar
        if (scalar == "true" || scalar == "false") {
            return scalar == "true";
        }
        
        // Check if it's an integer
        try {
            size_t pos;
            long long intVal = std::stoll(scalar, &pos);
            if (pos == scalar.size()) {
                return intVal;
            }
        } catch (...) {}
        
        // Check if it's a double
        try {
            size_t pos;
            double doubleVal = std::stod(scalar, &pos);
            if (pos == scalar.size()) {
                return doubleVal;
            }
        } catch (...) {}
        
        return scalar;
    } else if (value.IsSequence()) {
        // Handle array type - determine element type
        if (!value.empty()) {
            if (value[0].IsScalar() && isIntegerScalar(value[0].Scalar())) {
                std::vector<int> intArray;
                for (const auto& item : value) {
                    intArray.push_back(std::stoi(item.Scalar()));
                }
                return intArray;
            } else if (value[0].IsScalar() && isDoubleScalar(value[0].Scalar())) {
                std::vector<double> doubleArray;
                for (const auto& item : value) {
                    doubleArray.push_back(std::stod(item.Scalar()));
                }
                return doubleArray;
            } else {
                std::vector<std::string> stringArray;
                for (const auto& item : value) {
                    stringArray.push_back(item.Scalar());
                }
                return stringArray;
            }
        }
        return std::vector<int>{};
    }
    return "";
}

bool Config::isIntegerScalar(const std::string& str) {
    try {
        size_t pos;
        std::stoi(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

bool Config::isDoubleScalar(const std::string& str) {
    try {
        size_t pos;
        std::stod(str, &pos);
        return pos == str.size();
    } catch (...) {
        return false;
    }
}

ConfigEntry* Config::findEntry(const std::string& key);
const ConfigEntry* Config::findEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return (it != pImpl->entries.end()) ? &(*it) : nullptr;
}

ConfigEntry* Config::findEntry(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return (it != pImpl->entries.end()) ? &(*it) : nullptr;
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
    try {
        std::string ext = "." + std::filesystem::path(filepath).extension().string();
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        if (ext == ".yaml" || ext == ".yml") {
            file << saveToYAML();
        } else if (ext == ".json") {
            file << saveToJSON();
        } else {
            file << saveToSimpleFormat();
        }
        
        return true;
    } catch (const std::exception& e) {
        // TODO: Add proper error logging
        return false;
    }
}

std::string Config::saveToJSON() const {
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        // Add description as a special field
        jsonData[entry.key] = configValueToJSON(entry.value);
        
        if (!entry.description.empty()) {
            // Store description separately or as metadata
            // For simplicity, we'll store it as a comment
        }
    }
    
    return jsonData.dump(4);
}

std::string Config::saveToYAML() const {
    YAML::Emitter emitter;
    emitter.SetIndent(2);
    emitter.SetMapFormat(YAML::EMitterStyle::Flow);
    
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        emitter << YAML::Key << entry.key;
        emitter << YAML::Value << configValueToYAML(entry.value);
        
        if (!entry.description.empty()) {
            // Add description as a comment
            emitter << YAML::Comment(entry.description.c_str());
        }
    }
    
    emitter << YAML::EndMap;
    
    return emitter.c_str();
}

std::string Config::saveToSimpleFormat() const {
    std::ostringstream oss;
    
    for (const auto& entry : pImpl->entries) {
        if (!entry.description.empty()) {
            oss << "# " << entry.description << "\n";
        }
        oss << entry.key << " = " << valueToString(entry.value) << "\n";
    }
    
    return oss.str();
}

nlohmann::json Config::configValueToJSON(const ConfigValue& value) const {
    return std::visit([](const auto& val) -> nlohmann::json {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, int>) {
            return val;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return val;
        } else if constexpr (std::is_same_v<T, double>) {
            return val;
        } else if constexpr (std::is_same_v<T, bool>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::string>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : val) {
                array.push_back(item);
            }
            return array;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : val) {
                array.push_back(item);
            }
            return array;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            nlohmann::json array = nlohmann::json::array();
            for (const auto& item : val) {
                array.push_back(item);
            }
            return array;
        }
        return "";
    }, value);
}

YAML::Node Config::configValueToYAML(const ConfigValue& value) const {
    YAML::Node node;
    
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, int>) {
            node = val;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            node = static_cast<int>(val);
        } else if constexpr (std::is_same_v<T, double>) {
            node = val;
        } else if constexpr (std::is_same_v<T, bool>) {
            node = val;
        } else if constexpr (std::is_same_v<T, std::string>) {
            node = val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& item : val) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : val) {
                node.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : val) {
                node.push_back(item);
            }
        }
    }, value);
    
    return node;
}

std::string Config::valueToString(const ConfigValue& value) const {
    return std::visit([](const auto& val) -> std::string {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, int>) {
            return std::to_string(val);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::to_string(val);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(val);
        } else if constexpr (std::is_same_v<T, bool>) {
            return val ? "true" : "false";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << val[i];
            }
            oss << "]";
            return oss.str();
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << val[i];
            }
            oss << "]";
            return oss.str();
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << val[i] << "\"";
            }
            oss << "]";
            return oss.str();
        }
        return "";
    }, value);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Try to convert to more appropriate type
    ConfigValue convertedValue = stringToValue(value);
    
    // Check if value has changed
    auto* entry = findEntry(key);
    bool valueChanged = !entry || entry->value != convertedValue;
    
    if (entry) {
        entry->value = convertedValue;
        entry->source = source;
        // Preserve existing description
    } else {
        pImpl->entries.emplace_back(key, convertedValue, source);
    }
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
    return findEntry(key) != nullptr;
}

void Config::remove(const std::string& key) {
    auto it = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    pImpl->entries.erase(it, pImpl->entries.end());
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

std::string Config::getDescription(const std::string& key) const {
    const ConfigEntry* entry = findEntry(key);
    return entry ? entry->description : "";
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        oss << valueToString(entry.value);
        oss << "] (" << configSourceToString(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << "\n";
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