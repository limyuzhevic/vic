#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <cctype>
#include <cstddef>
#include <cstring>
#include <typeinfo>
#include <unordered_map>
#include <unordered_set>

// JSON support
#ifdef __cplusplus
#include <nlohmann/json.hpp>
#endif

// YAML support
#ifdef __cplusplus
#include <yaml-cpp/yaml.h>
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    ConfigSchema schema;
};

Config::Config() : pImpl(std::make_unique<Impl>()), schema() {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Enhanced file loading with JSON/YAML support
bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::string format = detectFileFormat(filepath);
        
        if (format == "json") {
            nlohmann::json json_data;
            {
                std::ifstream file(filepath);
                if (!file.is_open()) {
                    throw ConfigFileError("Cannot open JSON file: " + filepath);
                }
                file >> json_data;
            }
            
            // Convert JSON to flattened map and load
            std::map<std::string, nlohmann::json> flat;
            flattenObject("", json_data, flat);
            
            for (const auto& pair : flat) {
                auto parsed = parseJsonValue(pair.second);
                if (parsed.has_value()) {
                    set(pair.first, *parsed, ConfigSource::File);
                }
            }
            
        } else if (format == "yaml" || format == "yml") {
            YAML::Node yaml_data;
            {
                try {
                    yaml_data = YAML::LoadFile(filepath);
                } catch (const YAML::Exception& e) {
                    throw ConfigFileError(std::string("YAML parse error in file ") + filepath + ": " + e.msg);
                }
            }
            
            // Convert YAML to flattened map and load
            std::map<std::string, YAML::Node> flat;
            flattenYamlObject("", yaml_data, flat);
            
            for (const auto& pair : flat) {
                auto parsed = parseYamlValue(pair.second);
                if (parsed.has_value()) {
                    set(pair.first, *parsed, ConfigSource::File);
                }
            }
            
        } else {
            // Fallback to simple key=value format
            std::ifstream file(filepath);
            if (!file.is_open()) {
                throw ConfigFileError("Cannot open config file: " + filepath);
            }
            
            std::string line;
            int lineNum = 0;
            while (std::getline(file, line)) {
                ++lineNum;
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                size_t pos = line.find('=');
                if (pos == std::string::npos) {
                    throw ConfigFileError(std::string("Syntax error at line ") + 
                                        std::to_string(lineNum) + ": missing '='");
                }
                
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                try {
                    set(key, value, ConfigSource::File);
                } catch (const ConfigException& e) {
                    throw ConfigFileError(std::string("Error parsing line ") + 
                                        std::to_string(lineNum) + ": " + e.what());
                }
            }
        }
        
        // Validate all loaded entries
        validate();
        return true;
        
    } catch (const ConfigFileError&) {
        throw;
    } catch (const ConfigValidationError&) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigFileError(std::string("Error loading config from file ") + filepath + ": " + e.what());
    }
}

// Helper functions
nlohmann::json Config::jsonFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileError("Cannot open JSON file: " + filepath);
    }
    
    try {
        nlohmann::json json;
        file >> json;
        return json;
    } catch (const nlohmann::json::parse_error& e) {
        throw ConfigFileError(std::string("JSON parse error in file ") + filepath + ": " + e.what());
    } catch (const std::exception& e) {
        throw ConfigFileError(std::string("Error reading JSON file ") + filepath + ": " + e.what());
    }
}

YAML::Node Config::yamlFromFile(const std::string& filepath) {
    try {
        return YAML::LoadFile(filepath);
    } catch (const YAML::Exception& e) {
        throw ConfigFileError(std::string("YAML parse error in file ") + filepath + ": " + e.msg);
    } catch (const std::exception& e) {
        throw ConfigFileError(std::string("Error reading YAML file ") + filepath + ": " + e.what());
    }
}

std::string Config::detectFileFormat(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return "json";
    } else if (ext == ".yaml" || ext == ".yml") {
        return "yaml";
    } else {
        return "unknown";
    }
}

void Config::flattenObject(const std::string& prefix, const nlohmann::json& obj, std::map<std::string, nlohmann::json>& flat) {
    if (obj.is_object()) {
        for (const auto& item : obj.items()) {
            std::string key = item.key();
            if (!prefix.empty()) {
                key = prefix + "." + key;
            }
            flattenObject(key, item.value(), flat);
        }
    } else {
        flat[prefix] = obj;
    }
}

void Config::flattenYamlObject(const std::string& prefix, const YAML::Node& obj, std::map<std::string, YAML::Node>& flat) {
    if (obj.IsMap()) {
        for (const auto& item : obj) {
            std::string key = item.first.as<std::string>();
            if (!prefix.empty()) {
                key = prefix + "." + key;
            }
            flattenYamlObject(key, item.second, flat);
        }
    } else {
        flat[prefix] = obj;
    }
}

std::optional<ConfigValue> Config::parseJsonValue(const nlohmann::json& json) {
    try {
        if (json.is_string()) {
            return ConfigValue(json.get<std::string>());
        } else if (json.is_number_integer()) {
            return ConfigValue(json.get<int64_t>());
        } else if (json.is_number_float()) {
            return ConfigValue(json.get<double>());
        } else if (json.is_boolean()) {
            return ConfigValue(json.get<bool>());
        } else if (json.is_array()) {
            std::vector<ConfigValue> result;
            for (const auto& item : json) {
                auto parsed = parseJsonValue(item);
                if (parsed.has_value()) {
                    result.push_back(*parsed);
                }
            }
            if (!result.empty()) {
                if (!result.empty() && std::holds_alternative<int64_t>(result[0])) {
                    std::vector<int64_t> int_vec;
                    for (const auto& val : result) {
                        if (const auto* p = std::get_if<int64_t>(&val)) {
                            int_vec.push_back(*p);
                        }
                    }
                    if (!int_vec.empty()) {
                        return ConfigValue(int_vec);
                    }
                } else if (!result.empty() && std::holds_alternative<double>(result[0])) {
                    std::vector<double> double_vec;
                    for (const auto& val : result) {
                        if (const auto* p = std::get_if<double>(&val)) {
                            double_vec.push_back(*p);
                        }
                    }
                    if (!double_vec.empty()) {
                        return ConfigValue(double_vec);
                    }
                } else if (!result.empty() && std::holds_alternative<std::string>(result[0])) {
                    std::vector<std::string> string_vec;
                    for (const auto& val : result) {
                        if (const auto* p = std::get_if<std::string>(&val)) {
                            string_vec.push_back(*p);
                        }
                    }
                    if (!string_vec.empty()) {
                        return ConfigValue(string_vec);
                    }
                } else if (!result.empty() && std::holds_alternative<bool>(result[0])) {
                    std::vector<bool> bool_vec;
                    for (const auto& val : result) {
                        if (const auto* p = std::get_if<bool>(&val)) {
                            bool_vec.push_back(*p);
                        }
                    }
                    if (!bool_vec.empty()) {
                        return ConfigValue(bool_vec);
                    }
                }
            }
            return ConfigValue(std::vector<int64_t>{});
        }
    } catch (...) {}
    return std::nullopt;
}

std::optional<ConfigValue> Config::parseYamlValue(const YAML::Node& yaml) {
    try {
        if (yaml.IsScalar()) {
            std::string value = yaml.as<std::string>();
            
            if (value == "true" || value == "false") {
                return ConfigValue(value == "true");
            }
            
            try {
                int64_t int_val = std::stoll(value);
                return ConfigValue(int_val);
            } catch (...) {}
            
            try {
                double double_val = std::stod(value);
                return ConfigValue(double_val);
            } catch (...) {}
            
            return ConfigValue(value);
        } else if (yaml.IsSequence()) {
            std::vector<ConfigValue> result;
            for (const auto& item : yaml) {
                auto parsed = parseYamlValue(item);
                if (parsed.has_value()) {
                    result.push_back(*parsed);
                }
            }
            if (!result.empty()) {
                if (!result.empty() && std::holds_alternative<int64_t>(result[0])) {
                    std::vector<int64_t> int_vec;
                    for (const auto& val : result) {
                        if (const auto* p = std::get_if<int64_t>(&val)) {
                            int_vec.push_back(*p);
                        }
                    }
                    if (!int_vec.empty()) {
                        return ConfigValue(int_vec);
                    }
                } else if (!result.empty() && std::holds_alternative<double>(result[0])) {
                    std::vector<double> double_vec;
                    for (const auto& val : result) {
                        if (const auto* p = std::get_if<double>(&val)) {
                            double_vec.push_back(*p);
                        }
                    }
                    if (!double_vec.empty()) {
                        return ConfigValue(double_vec);
                    }
                } else if (!result.empty() && std::holds_alternative<std::string>(result[0])) {
                    std::vector<std::string> string_vec;
                    for (const auto& val : result) {
                        if (const auto* p = std::get_if<std::string>(&val)) {
                            string_vec.push_back(*p);
                        }
                    }
                    if (!string_vec.empty()) {
                        return ConfigValue(string_vec);
                    }
                } else if (!result.empty() && std::holds_alternative<bool>(result[0])) {
                    std::vector<bool> bool_vec;
                    for (const auto& val : result) {
                        if (const auto* p = std::get_if<bool>(&val)) {
                            bool_vec.push_back(*p);
                        }
                    }
                    if (!bool_vec.empty()) {
                        return ConfigValue(bool_vec);
                    }
                }
            }
            return ConfigValue(std::vector<int64_t>{});
        }
    } catch (...) {}
    return std::nullopt;
}

// Basic set/get methods for support
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<bool>& value, ConfigSource source) {
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
        std::visit([&oss](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << (arg[i] ? "true" : "false");
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

ConfigValue Config::visit_config_value(const ConfigValue& value) {
    ConfigValue result;
    std::visit([&result](const auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            result = arg;
        }
    }, value);
    return result;
}

// Basic toJson method
std::string Config::toJson() const {
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        json[entry.key] = visit_config_value(entry.value);
    }
    
    return json.dump(2);
}

bool Config::fromJson(const std::string& json) {
    try {
        nlohmann::json json_data = nlohmann::json::parse(json);
        clear();
        
        for (const auto& item : json_data.items()) {
            auto parsed = parseJsonValue(item.value());
            if (parsed.has_value()) {
                set(item.key(), *parsed);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        throw ConfigFileError(std::string("JSON parse error: ") + e.what());
    } catch (const std::exception& e) {
        throw ConfigFileError(std::string("Error parsing JSON: ") + e.what());
    }
}

// Schema and validation methods
void Config::addSchema(const ConfigSchema& newSchema) {
    for (const auto& pair : newSchema) {
        schema[pair.first] = pair.second;
    }
}

bool Config::validate(const std::map<std::string, ConfigValue>& context) const {
    for (const auto& entry : pImpl->entries) {
        if (!validateValue(entry.value, entry.source)) {
            throw ConfigValidationError(std::string("Invalid value for key: ") + entry.key);
        }
        
        auto schemaIt = schema.find(entry.key);
        if (schemaIt != schema.end()) {
            if (!schemaIt->second.validate(entry.value, context)) {
                throw ConfigValidationError(std::string("Schema validation failed for key: ") + 
                                           entry.key + ": " + schemaIt->second.name);
            }
        }
    }
    return true;
}

// Configuration merging
void Config::merge(const Config& other) {
    for (const auto& entry : other.getEntries()) {
        set(entry.key, entry.value, entry.source);
    }
}

// Configuration diff
std::map<std::string, std::pair<ConfigValue, ConfigValue>> Config::diff(const Config& other) const {
    std::map<std::string, std::pair<ConfigValue, ConfigValue>> differences;
    
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                                   [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (otherIt == other.pImpl->entries.end()) {
            differences[entry.key] = std::make_pair(entry.value, ConfigValue());
        } else if (otherIt->value != entry.value) {
            differences[entry.key] = std::make_pair(entry.value, otherIt->value);
        }
    }
    
    for (const auto& entry : other.pImpl->entries) {
        auto thisIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                   [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (thisIt == pImpl->entries.end()) {
            differences[entry.key] = std::make_pair(ConfigValue(), entry.value);
        }
    }
    
    return differences;
}

std::map<std::string, ConfigValue> Config::toDict() const {
    std::map<std::string, ConfigValue> d;
    for (const auto& entry : pImpl->entries) {
        d[entry.key] = entry.value;
    }
    return d;
}

bool Config::fromDict(const std::map<std::string, ConfigValue>& d) {
    clear();
    for (const auto& item : d) {
        set(item.first, item.second);
    }
    return true;
}

Config::Stats Config::getStats() const {
    Stats stats;
    stats.totalEntries = pImpl->entries.size();
    stats.memoryUsage = sizeof(ConfigEntry) * stats.totalEntries;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&stats](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                stats.typeCounts["int"]++;
                stats.averageValues["int"] += static_cast<double>(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                stats.typeCounts["double"]++;
                stats.averageValues["double"] += arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                stats.typeCounts["bool"]++;
            } else if constexpr (std::is_same_v<T, std::string>) {
                stats.typeCounts["string"]++;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                stats.typeCounts["vector<int>"]++;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                stats.typeCounts["vector<double>"]++;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                stats.typeCounts["vector<string>"]++;
            } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                stats.typeCounts["vector<bool>"]++;
            }
        }, entry.value);
    }
    
    for (auto& pair : stats.averageValues) {
        size_t count = stats.typeCounts[pair.first];
        if (count > 0) {
            pair.second /= count;
        }
    }
    
    return stats;
}

// Basic validation
bool Config::validateValue(const ConfigValue& value, ConfigSource source) {
    if (source == ConfigSource::Default) {
        return true;
    }
    
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            if (arg.empty()) {
                throw ConfigValidationError("String values cannot be empty for non-default sources");
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (arg.size() > 1000) {
                throw ConfigValidationError("Vector size exceeds maximum limit (1000)");
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (arg.size() > 1000) {
                throw ConfigValidationError("Vector size exceeds maximum limit (1000)");
            }
            for (const auto& item : arg) {
                if (std::isnan(item) || std::isinf(item)) {
                    throw ConfigValidationError("Vector contains invalid floating-point values");
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (arg.size() > 1000) {
                throw ConfigValidationError("Vector size exceeds maximum limit (1000)");
            }
            for (const auto& item : arg) {
                if (item.empty()) {
                    throw ConfigValidationError("Vector contains empty string");
                }
            }
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            if (arg.size() > 1000) {
                throw ConfigValidationError("Vector size exceeds maximum limit (1000)");
            }
        }
    }, value);
    
    return true;
}

std::string Config::formatError(const std::string& key, const std::string& expectedType, 
    const std::string& actualType) {
    return std::string("Type mismatch for key '") + key + 
           std::string("': expected '") + expectedType + 
           std::string("', got '") + actualType + std::string("'") + 
           std::string(". Use get<CorrectType>('key') instead.");
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

// Basic save method
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigFileError("Cannot open file for writing: " + filepath);
    }
    
    try {
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            saveValueToStream(file, entry.value);
            file << "\n\n";
        }
        file.flush();
        
        if (!file.good()) {
            throw ConfigFileError("Failed to write complete configuration");
        }
        return true;
    } catch (const ConfigFileError&) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigFileError(std::string("Error saving config: ") + e.what());
    }
}

bool Config::saveValueToStream(std::ostream& out, const ConfigValue& value) {
    std::visit([&out](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            out << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            out << arg;
        } else if constexpr (std::is_same_v<T, double>) {
            out << arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            out << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            out << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) out << ", ";
                out << arg[i];
            }
            out << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            out << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) out << ", ";
                out << arg[i];
            }
            out << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            out << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) out << ", ";
                out << "\"" << arg[i] << "\"";
            }
            out << "]";
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            out << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) out << ", ";
                out << (arg[i] ? "true" : "false");
            }
            out << "]";
        }
    }, value);
    return true;
}

std::string Config::getKeyDocumentation(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                         [&key](const ConfigEntry& e) { return e.key == key; });
    if (it != pImpl->entries.end()) {
        std::string doc = "Key: " + it->key + "\n";
        doc += "Source: " + std::to_string(static_cast<int>(it->source)) + "\n";
        doc += "Description: " + it->description + "\n";
        
        auto schemaIt = schema.find(key);
        if (schemaIt != schema.end()) {
            doc += "Schema validation enabled: " + schemaIt->second.name + "\n";
            doc += "Validation rules: " + std::to_string(schemaIt->second.rules.size()) + "\n";
            doc += "Dependent keys: ";
            for (size_t i = 0; i < schemaIt->second.dependentKeys.size(); ++i) {
                if (i > 0) doc += ", ";
                doc += schemaIt->second.dependentKeys[i];
            }
            doc += "\n";
        }
        
        return doc;
    }
    return "Key not found: " + key;
}

std::vector<std::string> Config::getValidationRules() const {
    std::vector<std::string> rules;
    for (const auto& pair : schema) {
        rules.push_back(pair.first + ": " + pair.second.name + " - " + pair.second.description);
    }
    return rules;
}

std::string Config::help() const {
    std::ostringstream oss;
    oss << "Configuration Help:\n";
    oss << "===================\n\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n\n";
    oss << "Available keys:\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  - " << entry.key << ": " << entry.description << " (source: " 
            << static_cast<int>(entry.source) << ")\n";
    }
    oss << "\nType conversions:\n";
    oss << "  - asInt(key): Get value as int\n";
    oss << "  - asDouble(key): Get value as double\n";
    oss << "  - asString(key): Get value as string\n";
    oss << "  - asBool(key): Get value as bool\n";
    oss << "  - asIntVector(key): Get value as int vector\n";
    oss << "  - asDoubleVector(key): Get value as double vector\n";
    oss << "  - asStringVector(key): Get value as string vector\n";
    oss << "  - asBoolVector(key): Get value as bool vector\n\n";
    oss << "Validation:\n";
    oss << "  - validate(): Validate entire configuration\n";
    oss << "  - validateKey(key): Validate specific key\n";
    oss << "  - getErrors(): Get all validation errors\n";
    oss << "  - getSchema(): Get the validation schema\n\n";
    oss << "Operations:\n";
    oss << "  - merge(other): Merge with another configuration\n";
    oss << "  - diff(other): Get differences with another configuration\n";
    oss << "  - toDict(): Export to Python dictionary\n";
    oss << "  - fromDict(dict): Import from Python dictionary\n";
    oss << "  - toJson(): Export to JSON string\n";
    oss << "  - fromJson(json): Import from JSON string\n";
    oss << "  - batchSet(dict, source): Batch set multiple values\n";
    oss << "  - batchGet(keys): Batch get multiple values\n";
    return oss.str();
}

size_t Config::size() const {
    return pImpl->entries.size();
}

std::vector<Config::Item> Config::items() const {
    std::vector<Item> items;
    for (const auto& entry : pImpl->entries) {
        items.push_back({entry.key, entry.value});
    }
    return items;
}

std::vector<ConfigValue> Config::values() const {
    std::vector<ConfigValue> values;
    values.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        values.push_back(entry.value);
    }
    return values;
}

const std::vector<ConfigEntry>& Config::getEntries() const {
    return pImpl->entries;
}

const ConfigSchema& Config::getSchema() const {
    return schema;
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

// Basic template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// Explicit template instantiations for vector types
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;
template std::optional<std::vector<bool>> Config::get<std::vector<bool>>(const std::string&) const;

// Vector getOr instantiations
template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Additional vector getOr instantiations
template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;
template std::vector<bool> Config::getOr<std::vector<bool>>(const std::string&, const std::vector<bool>&) const;

std::map<std::string, std::string> Config::getAllDocumentation() const {
    std::map<std::string, std::string> docs;
    for (const auto& entry : pImpl->entries) {
        docs[entry.key] = getKeyDocumentation(entry.key);
    }
    return docs;
}

// Config::Stats implementation
Config::Stats Config::getStats() const {
    Stats stats;
    stats.totalEntries = pImpl->entries.size();
    stats.memoryUsage = sizeof(ConfigEntry) * stats.totalEntries;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&stats](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                stats.typeCounts["int"]++;
                stats.averageValues["int"] += static_cast<double>(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                stats.typeCounts["double"]++;
                stats.averageValues["double"] += arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                stats.typeCounts["bool"]++;
            } else if constexpr (std::is_same_v<T, std::string>) {
                stats.typeCounts["string"]++;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                stats.typeCounts["vector<int>"]++;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                stats.typeCounts["vector<double>"]++;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                stats.typeCounts["vector<string>"]++;
            } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                stats.typeCounts["vector<bool>"]++;
            }
        }, entry.value);
    }
    
    // Calculate averages
    for (auto& pair : stats.averageValues) {
        size_t count = stats.typeCounts[pair.first];
        if (count > 0) {
            pair.second /= count;
        }
    }
    
    return stats;
}

// Configuration diff implementation
std::map<std::string, std::pair<ConfigValue, ConfigValue>> Config::diff(const Config& other) const {
    std::map<std::string, std::pair<ConfigValue, ConfigValue>> differences;
    
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
                                   [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (otherIt == other.pImpl->entries.end()) {
            differences[entry.key] = std::make_pair(entry.value, ConfigValue());
        } else if (otherIt->value != entry.value) {
            differences[entry.key] = std::make_pair(entry.value, otherIt->value);
        }
    }
    
    for (const auto& entry : other.pImpl->entries) {
        auto thisIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                                   [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (thisIt == pImpl->entries.end()) {
            differences[entry.key] = std::make_pair(ConfigValue(), entry.value);
        }
    }
    
    return differences;
}

std::string Config::toJson() const {
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        json[entry.key] = visit_config_value(entry.value);
    }
    
    return json.dump(2);
}

bool Config::fromJson(const std::string& json) {
    try {
        nlohmann::json json_data = nlohmann::json::parse(json);
        clear();
        
        for (const auto& item : json_data.items()) {
            auto parsed = parseJsonValue(item.value());
            if (parsed.has_value()) {
                set(item.key(), *parsed);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        throw ConfigFileError(std::string("JSON parse error: ") + e.what());
    } catch (const std::exception& e) {
        throw ConfigFileError(std::string("Error parsing JSON: ") + e.what());
    }
}

Config::Impl::Impl() : entries(), schema() {}

Config::Impl::~Impl() = default;

// Clean up Config.cpp - remove the duplicate implementations that were causing issues

// Write a clean, focused implementation

// Template for Config.cpp - focusing on essential JSON/YAML support

// Main config.cpp file with clean implementation