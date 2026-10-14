#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <unordered_map>
#include <stdexcept>
#include <cctype>
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>

namespace nlm {

// Helper functions
namespace {

// Type conversion utilities
template<typename T>
T convertTo(const std::string& str) {
    std::istringstream iss(str);
    T value;
    if (!(iss >> value)) {
        throw ConfigError("Failed to convert string to value: " + str);
    }
    return value;
}

bool isBoolean(const std::string& str) {
    return str == "true" || str == "false" || str == "1" || str == "0";
}

ConfigValue stringToConfigValue(const std::string& str) {
    if (isBoolean(str)) {
        if (str == "true" || str == "1") return true;
        if (str == "false" || str == "0") return false;
    }
    
    // Try int64_t
    try {
        size_t pos = 0;
        int64_t intVal = std::stoll(str, &pos);
        if (pos == str.size()) {
            return intVal;
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos = 0;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // Return as string
    return str;
}

// Trim whitespace
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Detect file format
detectFileFormat(const std::string& filepath) {
    if (filepath.size() < 3) return "unknown";
    
    std::string ext = filepath.substr(filepath.size() - 3);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "son" || ext == "son") return "json";
    if (ext == "aml" || ext == "aml") return "yaml";
    if (ext == "fg" || ext == "fg") return "keyvalue";
    
    return "keyvalue"; // default fallback
}

// Environment variable substitution
std::string substituteEnvVars(const std::string& str) {
    std::string result = str;
    size_t pos = 0;
    while ((pos = result.find("${", pos)) != std::string::npos) {
        size_t endPos = result.find('}', pos);
        if (endPos == std::string::npos) break;
        
        std::string varName = result.substr(pos + 2, endPos - pos - 2);
        const char* value = std::getenv(varName.c_str());
        if (value) {
            result.replace(pos, endPos - pos + 1, value);
            pos += strlen(value);
        } else {
            result.replace(pos, endPos - pos + 1, "");
            pos += 1;
        }
    }
    return result;
}

// Recursively convert JSON value to ConfigValue
ConfigValue jsonValueToConfigValue(const nlohmann::json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int64_t>();
    } else if (j.is_number_float()) {
        return j.get<double>();
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        std::vector<ConfigValue> result;
        for (const auto& item : j) {
            result.push_back(jsonValueToConfigValue(item));
        }
        return result;
    } else if (j.is_object()) {
        std::unordered_map<std::string, ConfigValue> result;
        for (const auto& item : j.items()) {
            result[item.key()] = jsonValueToConfigValue(item.value());
        }
        return result;
    } else {
        return j.get<std::string>();
    }
}

// Recursively convert ConfigValue to JSON value
nlohmann::json configValueToJsonValue(const ConfigValue& value) {
    return std::visit(
        [](const auto& val) -> nlohmann::json {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return val;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return val;
            } else if constexpr (std::is_same_v<T, double>) {
                return val;
            } else if constexpr (std::is_same_v<T, bool>) {
                return val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                nlohmann::json result = nlohmann::json::array();
                for (const auto& item : val) {
                    result.push_back(item);
                }
                return result;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                nlohmann::json result = nlohmann::json::array();
                for (const auto& item : val) {
                    result.push_back(item);
                }
                return result;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                nlohmann::json result = nlohmann::json::array();
                for (const auto& item : val) {
                    result.push_back(item);
                }
                return result;
            } else if constexpr (std::is_same_v<T, std::vector<ConfigValue>>) {
                nlohmann::json result = nlohmann::json::array();
                for (const auto& item : val) {
                    result.push_back(configValueToJsonValue(item));
                }
                return result;
            } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, ConfigValue>>) {
                nlohmann::json result = nlohmann::json::object();
                for (const auto& item : val) {
                    result[item.first] = configValueToJsonValue(item.second);
                }
                return result;
            } else if constexpr (std::is_same_v<T, std::vector<std::unordered_map<std::string, ConfigValue>>>) {
                nlohmann::json result = nlohmann::json::array();
                for (const auto& item : val) {
                    nlohmann::json obj = nlohmann::json::object();
                    for (const auto& pair : item) {
                        obj[pair.first] = configValueToJsonValue(pair.second);
                    }
                    result.push_back(obj);
                }
                return result;
            } else {
                return nlohmann::json();
            }
        }, value
    );
}

// Recursively convert YAML value to ConfigValue
ConfigValue yamlValueToConfigValue(const YAML::Node& node) {
    if (node.IsScalar()) {
        std::string value = node.Scalar();
        if (value == "true" || value == "1") return true;
        if (value == "false" || value == "0") return false;
        
        // Try numeric conversion
        try {
            size_t pos = 0;
            int64_t intVal = std::stoll(value, &pos);
            if (pos == value.size()) {
                return intVal;
            }
        } catch (...) {}
        
        try {
            size_t pos = 0;
            double doubleVal = std::stod(value, &pos);
            if (pos == value.size()) {
                return doubleVal;
            }
        } catch (...) {}
        
        return value;
    } else if (node.IsSequence()) {
        std::vector<ConfigValue> result;
        for (const auto& item : node) {
            result.push_back(yamlValueToConfigValue(item));
        }
        return result;
    } else if (node.IsMap()) {
        std::unordered_map<std::string, ConfigValue> result;
        for (const auto& item : node) {
            result[item.first.as<std::string>()] = yamlValueToConfigValue(item.second);
        }
        return result;
    } else {
        return "";
    }
}

} // namespace

// Key-value parser implementation
KeyValueConfigParser::KeyValueConfigParser() {}

std::unordered_map<std::string, ConfigValue> KeyValueConfigParser::parse(const std::string& content) {
    std::unordered_map<std::string, ConfigValue> result;
    std::istringstream iss(content);
    std::string line;
    
    while (std::getline(iss, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
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
            
            // Substitute environment variables
            value = substituteEnvVars(value);
            
            result[key] = stringToConfigValue(value);
        }
    }
    
    return result;
}

std::string KeyValueConfigParser::serialize(const std::unordered_map<std::string, ConfigValue>& config) {
    std::ostringstream oss;
    
    for (const auto& pair : config) {
        oss << "# Configuration entry\n";
        oss << pair.first << " = ";
        
        std::visit([&oss](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (val ? "true" : "false");
            } else {
                oss << val;
            }
        }, pair.second);
        oss << "\n\n";
    }
    
    return oss.str();
}

bool KeyValueConfigParser::canParse(const std::string& filename) const {
    return detectFileFormat(filename) == "keyvalue";
}

// JSON parser implementation
std::unordered_map<std::string, ConfigValue> JsonConfigParser::parse(const std::string& content) {
    try {
        auto j = nlohmann::json::parse(content);
        std::unordered_map<std::string, ConfigValue> result;
        
        if (j.is_object()) {
            for (const auto& item : j.items()) {
                result[item.key()] = jsonValueToConfigValue(item.value());
            }
        }
        
        return result;
    } catch (const nlohmann::json::parse_error& e) {
        throw ConfigError("JSON parsing error: " + std::string(e.what()));
    }
}

std::string JsonConfigParser::serialize(const std::unordered_map<std::string, ConfigValue>& config) {
    nlohmann::json j = nlohmann::json::object();
    
    for (const auto& pair : config) {
        j[pair.first] = configValueToJsonValue(pair.second);
    }
    
    return j.dump(4); // Pretty print with indentation
}

bool JsonConfigParser::canParse(const std::string& filename) const {
    std::string ext = filename.substr(filename.size() - 3);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "son" || ext == "son" || ext == "jsn") return true;
    return false;
}

// YAML parser implementation
std::unordered_map<std::string, ConfigValue> YamlConfigParser::parse(const std::string& content) {
    try {
        auto node = YAML::Load(content);
        std::unordered_map<std::string, ConfigValue> result;
        
        if (node.IsMap()) {
            for (const auto& item : node) {
                result[item.first.as<std::string>()] = yamlValueToConfigValue(item.second);
            }
        }
        
        return result;
    } catch (const YAML::ParserException& e) {
        throw ConfigError("YAML parsing error: " + std::string(e.what()));
    }
}

std::string YamlConfigParser::serialize(const std::unordered_map<std::string, ConfigValue>& config) {
    YAML::Emitter emitter;
    emitter << YAML::BeginMap;
    
    for (const auto& pair : config) {
        emitter << YAML::Key << pair.first;
        emitter << YAML::Value;
        
        std::visit([&emitter](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                emitter << val;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                emitter << static_cast<int>(val);
            } else if constexpr (std::is_same_v<T, double>) {
                emitter << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                emitter << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                emitter << YAML::BeginSeq;
                for (const auto& item : val) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                emitter << YAML::BeginSeq;
                for (const auto& item : val) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                emitter << YAML::BeginSeq;
                for (const auto& item : val) {
                    emitter << item;
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::vector<ConfigValue>>) {
                emitter << YAML::BeginSeq;
                for (const auto& item : val) {
                    emitter << configValueToString(item);
                }
                emitter << YAML::EndSeq;
            } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, ConfigValue>>) {
                emitter << YAML::BeginMap;
                for (const auto& item : val) {
                    emitter << YAML::Key << item.first;
                    emitter << YAML::Value;
                    emitter << configValueToString(item.second);
                }
                emitter << YAML::EndMap;
            } else if constexpr (std::is_same_v<T, std::vector<std::unordered_map<std::string, ConfigValue>>>) {
                emitter << YAML::BeginSeq;
                for (const auto& item : val) {
                    emitter << YAML::BeginMap;
                    for (const auto& pair : item) {
                        emitter << YAML::Key << pair.first;
                        emitter << YAML::Value;
                        emitter << configValueToString(pair.second);
                    }
                    emitter << YAML::EndMap;
                }
                emitter << YAML::EndSeq;
            } else {
                emitter << "null";
            }
        }, pair.second);
    }
    
    emitter << YAML::EndMap;
    
    std::stringstream ss;
    ss << emitter;
    return ss.str();
}

bool YamlConfigParser::canParse(const std::string& filename) const {
    std::string ext = filename.substr(filename.size() - 3);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "aml" || ext == "aml" || ext == "yml") return true;
    return false;
}

// Recursively convert ConfigValue to string for YAML
std::string configValueToString(const ConfigValue& value) {
    return std::visit(
        [](const auto& val) -> std::string {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return "\"" + val + "\"";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return std::to_string(val);
            } else if constexpr (std::is_same_v<T, double>) {
                return std::to_string(val);
            } else if constexpr (std::is_same_v<T, bool>) {
                return val ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                std::string result = "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) result += ", ";
                    result += std::to_string(val[i]);
                }
                result += "]";
                return result;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                std::string result = "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) result += ", ";
                    result += std::to_string(val[i]);
                }
                result += "]";
                return result;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                std::string result = "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) result += ", ";
                    result += "\"" + val[i] + "\"";
                }
                result += "]";
                return result;
            } else if constexpr (std::is_same_v<T, std::vector<ConfigValue>>) {
                std::string result = "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) result += ", ";
                    result += configValueToString(val[i]);
                }
                result += "]";
                return result;
            } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, ConfigValue>>) {
                std::string result = "{";
                for (auto it = val.begin(); it != val.end(); ++it) {
                    if (it != val.begin()) result += ", ";
                    result += "\"" + it->first + "\": " + configValueToString(it->second);
                }
                result += "}";
                return result;
            } else if constexpr (std::is_same_v<T, std::vector<std::unordered_map<std::string, ConfigValue>>>) {
                std::string result = "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) result += ", ";
                    std::string obj = "{";
                    for (auto it = val[i].begin(); it != val[i].end(); ++it) {
                        if (it != val[i].begin()) obj += ", ";
                        obj += "\"" + it->first + "\": " + configValueToString(it->second);
                    }
                    obj += "}";
                    result += obj;
                }
                result += "]";
                return result;
            } else {
                return "null";
            }
        }, value
    );
}

// ConfigSchema implementation
void ConfigSchema::addEntry(const ConfigEntry& entry) {
    entries[entry.key] = entry;
}

bool ConfigSchema::validate(const std::string& key, const ConfigValue& value) const {
    auto it = entries.find(key);
    if (it == entries.end()) {
        return true; // Unknown key, allow it
    }
    
    const ConfigEntry& entry = it->second;
    
    // Check required
    if (entry.required && entry.defaultValue.has_value()) {
        return false;
    }
    
    // Validate type (basic check)
    if (entry.validator && !entry.validator(value)) {
        return false;
    }
    
    return true;
}

bool ConfigSchema::validate(const std::unordered_map<std::string, ConfigValue>& config) const {
    for (const auto& pair : config) {
        if (!validate(pair.first, pair.second)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> ConfigSchema::getMissingRequired() const {
    std::vector<std::string> missing;
    for (const auto& pair : entries) {
        if (pair.second.required && !has(pair.first)) {
            missing.push_back(pair.first);
        }
    }
    return missing;
}

// Config implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<std::unique_ptr<ConfigParser>> parsers;
    std::unique_ptr<ConfigSchema> schema;
    
    Impl() {
        // Register default parsers
        parsers.push_back(std::make_unique<KeyValueConfigParser>());
        parsers.push_back(std::make_unique<JsonConfigParser>());
        parsers.push_back(std::make_unique<YamlConfigParser>());
        
        schema = std::make_unique<ConfigSchema>();
    }
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
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Find appropriate parser
    for (const auto& parser : pImpl->parsers) {
        if (parser->canParse(filepath)) {
            auto parsed = parser->parse(content);
            
            // Apply parsed values
            for (const auto& pair : parsed) {
                set(pair.first, pair.second, ConfigSource::File);
            }
            
            return true;
        }
    }
    
    return false;
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
    
    // Find appropriate serializer
    for (const auto& parser : pImpl->parsers) {
        if (parser->canParse(filepath)) {
            std::unordered_map<std::string, ConfigValue> config;
            for (const auto& entry : pImpl->entries) {
                config[entry.key] = entry.value;
            }
            
            std::string content = parser->serialize(config);
            file << content;
            return true;
        }
    }
    
    return false;
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
        std::remove_if(pImpl->entries.begin(),
                      pImpl->entries.end(),
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
        std::visit([&oss](const auto& arg) {
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

// detectFileFormat function implementation
detectFileFormat(const std::string& filepath) {
    if (filepath.size() < 3) return "unknown";
    
    std::string ext = filepath.substr(filepath.size() - 3);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "son" || ext == "son") return "json";
    if (ext == "aml" || ext == "aml") return "yaml";
    if (ext == "fg" || ext == "fg") return "keyvalue";
    
    return "keyvalue"; // default fallback
}

} // namespace nlm
