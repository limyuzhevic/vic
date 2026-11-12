#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstdint>
#include <regex>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <numeric>
#include <stdexcept>

namespace nlm {

// JSON parsing helpers
namespace json {

struct JsonNode {
    enum class Type { NULL_T, BOOLEAN, NUMBER, STRING, ARRAY, OBJECT };
    
    Type type;
    
    // For boolean
    bool boolValue;
    
    // For number
    double numberValue;
    
    // For string
    std::string stringValue;
    
    // For array
    std::vector<JsonNode> arrayValue;
    
    // For object
    std::map<std::string, JsonNode> objectValue;
    
    JsonNode() : type(Type::NULL_T) {}
    JsonNode(bool b) : type(Type::BOOLEAN), boolValue(b) {}
    JsonNode(double n) : type(Type::NUMBER), numberValue(n) {}
    JsonNode(const std::string& s) : type(Type::STRING), stringValue(s) {}
    JsonNode(const char* s) : type(Type::STRING), stringValue(s) {}
    JsonNode(std::vector<JsonNode> arr) : type(Type::ARRAY), arrayValue(std::move(arr)) {}
    JsonNode(std::map<std::string, JsonNode> obj) : type(Type::OBJECT), objectValue(std::move(obj)) {}
    
    bool isNull() const { return type == Type::NULL_T; }
    bool isBool() const { return type == Type::BOOLEAN; }
    bool isNumber() const { return type == Type::NUMBER; }
    bool isString() const { return type == Type::STRING; }
    bool isArray() const { return type == Type::ARRAY; }
    bool isObject() const { return type == Type::OBJECT; }
    
    const std::string& asString() const { return stringValue; }
    double asNumber() const { return numberValue; }
    bool asBool() const { return boolValue; }
    const std::vector<JsonNode>& asArray() const { return arrayValue; }
    const std::map<std::string, JsonNode>& asObject() const { return objectValue; }
};

class JsonParser {
public:
    static JsonNode parse(const std::string& jsonStr);
    static JsonNode parseFile(const std::string& filepath);
    static std::string toString(const JsonNode& node);
    
private:
    static std::string trim(const std::string& str);
    static bool isWhitespace(char c);
    static char nextNonWhitespace(const std::string& str, size_t& pos);
    static JsonNode parseValue(const std::string& str, size_t& pos);
    static JsonNode parseObject(const std::string& str, size_t& pos);
    static JsonNode parseArray(const std::string& str, size_t& pos);
    static std::string parseString(const std::string& str, size_t& pos);
    static double parseNumber(const std::string& str, size_t& pos);
    static bool parseBool(const std::string& str, size_t& pos);
    static bool parseNull(const std::string& str, size_t& pos);
    static void skipWhitespace(const std::string& str, size_t& pos);
    static char getChar(const std::string& str, size_t pos);
};

// YAML parsing helpers
namespace yaml {

struct YamlNode {
    enum class Type { NULL_T, SCALAR, SEQUENCE, MAPPING };
    
    Type type;
    std::string scalar;
    std::vector<YamlNode> sequence;
    std::vector<std::pair<std::string, YamlNode>> mapping;
    
    YamlNode() : type(Type::NULL_T) {}
    YamlNode(const std::string& s) : type(Type::SCALAR), scalar(s) {}
    YamlNode(const char* s) : type(Type::SCALAR), scalar(s) {}
    YamlNode(std::vector<YamlNode> seq) : type(Type::SEQUENCE), sequence(std::move(seq)) {}
    YamlNode(std::vector<std::pair<std::string, YamlNode>> map) : type(Type::MAPPING), mapping(std::move(map)) {}
};

class YamlParser {
public:
    static YamlNode parse(const std::string& yamlStr);
    static YamlNode parseFile(const std::string& filepath);
    static std::string toString(const YamlNode& node);
    
private:
    static std::string trim(const std::string& str);
    static std::string ltrim(const std::string& str);
    static std::string rtrim(const std::string& str);
    static std::vector<std::string> split(const std::string& str, char delimiter);
    static bool isWhitespace(char c);
    static bool isDigit(char c);
    static YamlNode parseDocument(const std::string& str);
    static YamlNode parseNode(const std::string& str, size_t& pos);
    static std::string parseScalar(const std::string& str, size_t& pos);
    static YamlNode parseSequence(const std::string& str, size_t& pos);
    static YamlNode parseMapping(const std::string& str, size_t& pos);
    static std::string unescapeString(const std::string& str);
    static std::string escapeString(const std::string& str);
};

// Forward declarations for config conversion
JsonNode configValueToJson(const ConfigValue& value);
ConfigValue jsonToConfigValue(const JsonNode& node);

}} // namespace nlm

// Begin Config class implementation

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Load configuration from a file (JSON format)
bool Config::loadFromFile(const std::string& filepath) {
    NLM_LOG_INFO("Attempting to load configuration from: " + filepath);
    
    // Try to parse as JSON first
    bool jsonSuccess = false;
    try {
        NLM_LOG_INFO("Attempting JSON parsing...");
        auto jsonRoot = json::parseFile(filepath);
        NLM_LOG_INFO("JSON parsing successful");
        // Convert JSON to config entries
        convertJsonToConfig(jsonRoot, ConfigSource::File);
        NLM_LOG_INFO("JSON configuration loaded successfully");
        jsonSuccess = true;
    } catch (const std::exception& e) {
        NLM_LOG_WARNING("JSON parsing failed: " + std::string(e.what()) + ", falling back to simple format");
        // If JSON fails, fall back to simple key=value format
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
            return false;
        }

        std::string line;
        size_t lineNumber = 0;
        size_t validEntries = 0;
        while (std::getline(file, line)) {
            lineNumber++;
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }

            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));

                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }

                // Validate key format
                if (key.empty() || key.find(' ') != std::string::npos) {
                    NLM_LOG_WARNING("Invalid configuration key at line " + std::to_string(lineNumber) + ": '" + key + "'");
                    continue;
                }

                set(key, value, ConfigSource::File);
                validEntries++;
                NLM_LOG_DEBUG("Loaded configuration: " + key + " = " + value);
            } else {
                NLM_LOG_WARNING("Invalid configuration format at line " + std::to_string(lineNumber) + ": " + line);
            }
        }
        NLM_LOG_INFO("Configuration file parsing complete: " + std::to_string(validEntries) + " valid entries loaded");
        jsonSuccess = true;
    }
    
    NLM_LOG_INFO("Configuration loaded successfully");
    return jsonSuccess;
}

// Load configuration from command line arguments
bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_INFO("Loading configuration from command line arguments");
    
    bool success = true;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                NLM_LOG_INFO("Command line override: " + key + " = " + value);
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Invalid command line argument format: " + arg);
                success = false;
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            NLM_LOG_INFO("Command line override: " + key + " = " + value);
            set(key, value, ConfigSource::CommandLine);
        } else {
            NLM_LOG_WARNING("Ignoring unrecognized command line argument: " + arg);
            // Continue processing other arguments
        }
    }
    
    if (success) {
        NLM_LOG_INFO("Command line configuration loaded successfully");
    } else {
        NLM_LOG_WARNING("Command line configuration loaded with warnings");
    }
    return success;
}

// Save configuration to a file (JSON format)
bool Config::saveToFile(const std::string& filepath) const {
    NLM_LOG_INFO("Attempting to save configuration to: " + filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    // Convert config entries to JSON and save
    json::JsonNode jsonRoot;
    jsonRoot.objectValue = std::map<std::string, json::JsonNode>();
    
    for (const auto& entry : pImpl->entries) {
        jsonRoot.objectValue[entry.key] = configValueToJson(entry.value);
    }
    
    std::string jsonStr = json::JsonParser::toString(jsonRoot);
    file << jsonStr;
    
    if (file.good()) {
        NLM_LOG_INFO("Configuration saved successfully to: " + filepath);
        return true;
    } else {
        NLM_LOG_ERROR("Failed to write configuration to file: " + filepath);
        return false;
    }
}

// Convert ConfigValue to JSON Node
json::JsonNode configValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& v) -> json::JsonNode {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, int> || std::is_same_v<V, int64_t>) {
            return json::JsonNode(static_cast<double>(v));
        } else if constexpr (std::is_same_v<V, double> || std::is_same_v<V, float>) {
            return json::JsonNode(v);
        } else if constexpr (std::is_same_v<V, bool>) {
            return json::JsonNode(v);
        } else if constexpr (std::is_same_v<V, std::string>) {
            return json::JsonNode(v);
        } else if constexpr (std::is_same_v<V, std::vector<int>> ||
                             std::is_same_v<V, std::vector<double>> ||
                             std::is_same_v<V, std::vector<std::string>>) {
            if constexpr (std::is_same_v<V, std::vector<int>> ||
                          std::is_same_v<V, std::vector<double>> ||
                          std::is_same_v<V, std::vector<std::string>>) {
                std::vector<json::JsonNode> arr;
                for (const auto& item : v) {
                    arr.push_back(configValueToJson(item));
                }
                return json::JsonNode(std::move(arr));
            }
        }
        return json::JsonNode(); // Default for unsupported types
    }, value);
}

// Convert JSON Node to ConfigValue
ConfigValue jsonToConfigValue(const json::JsonNode& node) {
    if (node.isBool()) {
        return node.asBool();
    } else if (node.isNumber()) {
        double val = node.asNumber();
        if (std::floor(val) == val && val >= std::numeric_limits<int>::min() && val <= std::numeric_limits<int>::max()) {
            return static_cast<int>(val);
        }
        if (std::floor(val) == val && val >= std::numeric_limits<int64_t>::min() && val <= std::numeric_limits<int64_t>::max()) {
            return static_cast<int64_t>(val);
        }
        return val;
    } else if (node.isString()) {
        return node.asString();
    } else if (node.isArray()) {
        const auto& arr = node.asArray();
        if (!arr.empty()) {
            // Determine array type from first element
            if (arr[0].isNumber()) {
                std::vector<double> vec;
                vec.reserve(arr.size());
                for (const auto& item : arr) {
                    vec.push_back(item.asNumber());
                }
                return vec;
            } else if (arr[0].isString()) {
                std::vector<std::string> vec;
                vec.reserve(arr.size());
                for (const auto& item : arr) {
                    vec.push_back(item.asString());
                }
                return vec;
            }
        }
        return std::vector<int>{}; // Default empty array
    }
    return std::string(); // Default for unsupported types
}

// Helper function to convert JSON to config entries
void Config::convertJsonToConfig(const json::JsonNode& node, ConfigSource source) {
    if (node.isObject()) {
        for (const auto& pair : node.asObject()) {
            set(pair.first, jsonToConfigValue(pair.second), source);
        }
    }
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

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm