// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// This phase implements real spiking neural computation with:
// - Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - STDP and Hebbian plasticity
// - Structural plasticity (synaptogenesis/pruning)

#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// File loading with format detection
bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format based on extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    // Convert to lowercase
    for (char& c : ext) {
        c = std::tolower(c);
    }
    
    if (ext == "json") {
        return loadFromJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return loadFromYAML(filepath);
    } else {
        // Default to old key=value format
        return loadFromSimpleFormat(filepath);
    }
}

// Simple format loader (backward compatibility)
bool Config::loadFromSimpleFormat(const std::string& filepath) {
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

// JSON loader
bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData = nlohmann::json::parse(file);
        return loadFromJSON(jsonData.dump());
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    } catch (const std::exception& e) {
        return false;
    }
}

// JSON loader from string
bool Config::loadFromJSON(const std::string& jsonStr) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonStr);
        clear();
        
        // Convert JSON to config entries
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            const nlohmann::json& value = *it;
            
            ConfigValue configValue;
            if (parseJSONValue(value, configValue)) {
                set(key, configValue, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        return false;
    } catch (const std::exception& e) {
        return false;
    }
}

// YAML loader
bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        clear();
        
        // Convert YAML to config entries
        for (const auto& node : yamlData) {
            std::string key = node.first.as<std::string>();
            YAML::Node value = node.second;
            
            ConfigValue configValue;
            if (parseYAMLValue(value, configValue)) {
                set(key, configValue, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const YAML::Exception& e) {
        return false;
    } catch (const std::exception& e) {
        return false;
    }
}

// File saving with format detection
bool Config::saveToFile(const std::string& filepath) const {
    // Determine file format based on extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    // Convert to lowercase
    for (char& c : ext) {
        c = std::tolower(c);
    }
    
    if (ext == "json") {
        return saveToJSON(filepath);
    } else if (ext == "yaml" || ext == "yml") {
        return saveToYAML(filepath);
    } else {
        // Default to old key=value format
        return saveToSimpleFormat(filepath);
    }
}

// Simple format saver
bool Config::saveToSimpleFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated from config struct\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

// JSON saver
bool Config::saveToJSON(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        
        // Convert config entries to JSON
        for (const auto& entry : pImpl->entries) {
            jsonData[entry.key] = convertToJSON(entry.value);
        }
        
        // Write formatted JSON
        file << std::setw(2) << jsonData << std::endl;
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// YAML saver
bool Config::saveToYAML(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Emitter yamlData;
        yamlData << YAML::BeginMap;
        
        // Convert config entries to YAML
        for (const auto& entry : pImpl->entries) {
            yamlData << YAML::Key << entry.key;
            yamlData << YAML::Value << convertToYAML(entry.value);
        }
        
        yamlData << YAML::EndMap;
        
        file << yamlData.c_str();
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

// Command line argument loader
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

// Template method implementations
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
    oss << "Configuration (" << pImpl->entries.size() << " entries):\\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\\\"" << arg << "\\\"";
            } else if constexpr (std::is_same_v<T, int> || 
                                std::is_same_v<T, int64_t> || 
                                std::is_same_v<T, double> || 
                                std::is_same_v<T, bool>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << item;
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << item;
                    first = false;
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                bool first = true;
                for (const auto& item : arg) {
                    if (!first) oss << ", ";
                    oss << "\\\"" << item << "\\\"";
                    first = false;
                }
                oss << "]";
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\\n";
    }
    return oss.str();
}

// Helper functions
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

// JSON/YAML conversion helpers
bool Config::parseJSONValue(const nlohmann::json& jsonValue, ConfigValue& configValue) {
    try {
        if (jsonValue.is_null()) {
            configValue = std::string("");
            return true;
        } else if (jsonValue.is_boolean()) {
            configValue = jsonValue.get<bool>();
            return true;
        } else if (jsonValue.is_number_integer()) {
            int64_t intVal = jsonValue.get<int64_t>();
            if (intVal >= std::numeric_limits<int>::min() && intVal <= std::numeric_limits<int>::max()) {
                configValue = static_cast<int>(intVal);
            } else {
                configValue = intVal;
            }
            return true;
        } else if (jsonValue.is_number_float()) {
            configValue = jsonValue.get<double>();
            return true;
        } else if (jsonValue.is_string()) {
            configValue = jsonValue.get<std::string>();
            return true;
        } else if (jsonValue.is_array()) {
            std::vector<int> intArray;
            std::vector<double> doubleArray;
            std::vector<std::string> stringArray;
            
            bool allInts = true;
            bool allDoubles = true;
            bool allStrings = true;
            
            for (const auto& item : jsonValue) {
                if (item.is_number_integer()) {
                    if (allStrings) allStrings = false;
                    intArray.push_back(item.get<int>());
                } else if (item.is_number_float()) {
                    allInts = false;
                    if (allStrings) allStrings = false;
                    doubleArray.push_back(item.get<double>());
                } else if (item.is_string()) {
                    allInts = false;
                    allDoubles = false;
                    stringArray.push_back(item.get<std::string>());
                } else {
                    allInts = false;
                    allDoubles = false;
                    allStrings = false;
                    break;
                }
            }
            
            if (allInts) {
                configValue = intArray;
            } else if (allDoubles) {
                configValue = doubleArray;
            } else if (allStrings) {
                configValue = stringArray;
            } else {
                configValue = jsonValue.dump();
            }
            return true;
        } else {
            configValue = jsonValue.dump();
            return true;
        }
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::parseYAMLValue(const YAML::Node& yamlValue, ConfigValue& configValue) {
    try {
        if (!yamlValue.IsDefined()) {
            configValue = std::string("");
            return true;
        } else if (yamlValue.IsBoolean()) {
            configValue = yamlValue.as<bool>();
            return true;
        } else if (yamlValue.IsInt()) {
            configValue = yamlValue.as<int>();
            return true;
        } else if (yamlValue.IsDouble()) {
            configValue = yamlValue.as<double>();
            return true;
        } else if (yamlValue.IsScalar()) {
            std::string strValue = yamlValue.as<std::string>();
            
            if (strValue == "true" || strValue == "false") {
                configValue = (strValue == "true");
                return true;
            }
            
            try {
                size_t pos;
                int intVal = std::stoi(strValue, &pos);
                if (pos == strValue.size()) {
                    configValue = intVal;
                    return true;
                }
            } catch (...) {}
            
            try {
                size_t pos;
                double doubleVal = std::stod(strValue, &pos);
                if (pos == strValue.size()) {
                    configValue = doubleVal;
                    return true;
                }
            } catch (...) {}
            
            configValue = strValue;
            return true;
        } else if (yamlValue.IsSequence()) {
            std::vector<int> intArray;
            std::vector<double> doubleArray;
            std::vector<std::string> stringArray;
            
            bool allInts = true;
            bool allDoubles = true;
            bool allStrings = true;
            
            for (const auto& item : yamlValue) {
                if (item.IsInt()) {
                    if (allStrings) allStrings = false;
                    intArray.push_back(item.as<int>());
                } else if (item.IsDouble()) {
                    allInts = false;
                    if (allStrings) allStrings = false;
                    doubleArray.push_back(item.as<double>());
                } else if (item.IsScalar()) {
                    allInts = false;
                    allDoubles = false;
                    stringArray.push_back(item.as<std::string>());
                } else {
                    allInts = false;
                    allDoubles = false;
                    allStrings = false;
                    break;
                }
            }
            
            if (allInts) {
                configValue = intArray;
            } else if (allDoubles) {
                configValue = doubleArray;
            } else if (allStrings) {
                configValue = stringArray;
            } else {
                configValue = "[";
                bool first = true;
                for (const auto& item : yamlValue) {
                    if (!first) configValue += ", ";
                    first = false;
                    if (item.IsScalar()) {
                        configValue += "\"" + item.as<std::string>() + "\"";
                    } else {
                        configValue += std::to_string(item.as<double>());
                    }
                }
                configValue += "]";
            }
            return true;
        } else {
            configValue = "{";
            bool first = true;
            for (const auto& node : yamlValue) {
                if (!first) configValue += ", ";
                first = false;
                configValue += "\"" + node.first.as<std::string>() + "\": ";
                if (node.second.IsScalar()) {
                    configValue += "\"" + node.second.as<std::string>() + "\"";
                } else {
                    configValue += std::to_string(node.second.as<double>());
                }
            }
            configValue += "}";
            return true;
        }
    } catch (const std::exception& e) {
        return false;
    }
}

nlohmann::json Config::convertToJSON(const ConfigValue& configValue) const {
    nlohmann::json jsonValue;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            jsonValue = arg;
        } else {
            jsonValue = "UNKNOWN:" + std::to_string(static_cast<int>(arg));
        }
    }, configValue);
    
    return jsonValue;
}

YAML::Node Config::convertToYAML(const ConfigValue& configValue) const {
    YAML::Node yamlNode;
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            yamlNode = static_cast<int>(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            yamlNode = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            // For arrays, we need to create a YAML node that can hold the array
            YAML::Node arrayNode;
            for (const auto& item : arg) {
                arrayNode.push_back(item);
            }
            yamlNode = arrayNode;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            YAML::Node arrayNode;
            for (const auto& item : arg) {
                arrayNode.push_back(item);
            }
            yamlNode = arrayNode;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            YAML::Node arrayNode;
            for (const auto& item : arg) {
                arrayNode.push_back(item);
            }
            yamlNode = arrayNode;
        } else {
            yamlNode = "UNKNOWN:" + std::to_string(static_cast<int>(arg));
        }
    }, configValue);
    
    return yamlNode;
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
