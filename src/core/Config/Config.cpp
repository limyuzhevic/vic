#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <variant>
#include <any>
#include <stack>
#include <cctype>
#include <cstring>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// JSON/YAML Parser implementation
namespace {

class SimpleJsonParser {
public:
    struct JsonValue {
        std::variant<int, int64_t, double, bool, std::string, std::vector<JsonValue>, std::unordered_map<std::string, JsonValue>> value;
        
        // Helper methods to check type
        bool isInt() const { return std::holds_alternative<int>(value); }
        bool isInt64() const { return std::holds_alternative<int64_t>(value); }
        bool isDouble() const { return std::holds_alternative<double>(value); }
        bool isBool() const { return std::holds_alternative<bool>(value); }
        bool isString() const { return std::holds_alternative<std::string>(value); }
        bool isArray() const { return std::holds_alternative<std::vector<JsonValue>>(value); }
        bool isObject() const { return std::holds_alternative<std::unordered_map<std::string, JsonValue>>(value); }
        
        // Getters
        int getInt() const { return std::get<int>(value); }
        int64_t getInt64() const { return std::get<int64_t>(value); }
        double getDouble() const { return std::get<double>(value); }
        bool getBool() const { return std::get<bool>(value); }
        const std::string& getString() const { return std::get<std::string>(value); }
        const std::vector<JsonValue>& getArray() const { return std::get<std::vector<JsonValue>>(value); }
        const std::unordered_map<std::string, JsonValue>& getObject() const { return std::get<std::unordered_map<std::string, JsonValue>>(value); }
    };
    
    static bool parseJsonFile(const std::string& filepath, JsonValue& result) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        return parseJsonString(content, result);
    }
    
    static bool parseJsonString(const std::string& jsonStr, JsonValue& result) {
        // Skip whitespace
        size_t pos = 0;
        skipWhitespace(jsonStr, pos);
        
        if (pos >= jsonStr.length()) {
            return false;
        }
        
        char firstChar = jsonStr[pos];
        if (firstChar == '{') {
            return parseObject(jsonStr, pos, result);
        } else if (firstChar == '[') {
            return parseArray(jsonStr, pos, result);
        } else if (firstChar == '\"') {
            return parseString(jsonStr, pos, result);
        } else if (firstChar == 't' || firstChar == 'f') {
            return parseBool(jsonStr, pos, result);
        } else if (firstChar == '-' || (firstChar >= '0' && firstChar <= '9')) {
            return parseNumber(jsonStr, pos, result);
        } else if (firstChar == 'n' && jsonStr.substr(pos, 4) == "null") {
            pos += 4;
            result.value = std::string("null");
            return true;
        }
        
        return false;
    }
    
private:
    static void skipWhitespace(const std::string& str, size_t& pos) {
        while (pos < str.length() && std::isspace(str[pos])) {
            pos++;
        }
    }
    
    static bool parseObject(const std::string& str, size_t& pos, JsonValue& result) {
        if (pos >= str.length() || str[pos] != '{') {
            return false;
        }
        pos++;
        skipWhitespace(str, pos);
        
        auto obj = std::unordered_map<std::string, JsonValue>{};
        
        if (str[pos] == '}') {
            pos++;
            result.value = std::move(obj);
            return true;
        }
        
        while (true) {
            skipWhitespace(str, pos);
            
            // Parse key
            JsonValue keyValue;
            if (!parseString(str, pos, keyValue)) {
                return false;
            }
            
            if (!keyValue.isString()) {
                return false;
            }
            
            std::string key = keyValue.getString();
            skipWhitespace(str, pos);
            
            if (pos >= str.length() || str[pos] != ':') {
                return false;
            }
            pos++;
            skipWhitespace(str, pos);
            
            // Parse value
            JsonValue value;
            if (!parseJsonString(str, pos, value)) {
                return false;
            }
            
            obj[key] = std::move(value);
            skipWhitespace(str, pos);
            
            if (pos >= str.length()) {
                return false;
            }
            
            if (str[pos] == '}') {
                pos++;
                result.value = std::move(obj);
                return true;
            } else if (str[pos] != ',') {
                return false;
            }
            pos++;
        }
    }
    
    static bool parseArray(const std::string& str, size_t& pos, JsonValue& result) {
        if (pos >= str.length() || str[pos] != '[') {
            return false;
        }
        pos++;
        skipWhitespace(str, pos);
        
        auto arr = std::vector<JsonValue>{};
        
        if (str[pos] == ']') {
            pos++;
            result.value = std::move(arr);
            return true;
        }
        
        while (true) {
            JsonValue value;
            if (!parseJsonString(str, pos, value)) {
                return false;
            }
            
            arr.push_back(std::move(value));
            skipWhitespace(str, pos);
            
            if (pos >= str.length()) {
                return false;
            }
            
            if (str[pos] == ']') {
                pos++;
                result.value = std::move(arr);
                return true;
            } else if (str[pos] != ',') {
                return false;
            }
            pos++;
            skipWhitespace(str, pos);
        }
    }
    
    static bool parseString(const std::string& str, size_t& pos, JsonValue& result) {
        if (pos >= str.length() || str[pos] != '\"') {
            return false;
        }
        pos++;
        
        std::string parsed;
        while (pos < str.length() && str[pos] != '\"') {
            if (str[pos] == '\\' && pos + 1 < str.length()) {
                pos++; // Skip escape char
                if (str[pos] == 'n') {
                    parsed += '\n';
                } else if (str[pos] == 't') {
                    parsed += '\t';
                } else if (str[pos] == 'r') {
                    parsed += '\r';
                } else if (str[pos] == 'b') {
                    parsed += '\b';
                } else if (str[pos] == 'f') {
                    parsed += '\f';
                } else if (str[pos] == '\"' || str[pos] == '\\' || str[pos] == '/') {
                    parsed += str[pos];
                } else if (str[pos] == 'u') {
                    // Skip Unicode escape for simplicity
                    if (pos + 4 < str.length()) {
                        pos += 4;
                    }
                }
            } else {
                parsed += str[pos];
            }
            pos++;
        }
        
        if (pos >= str.length() || str[pos] != '\"') {
            return false;
        }
        pos++;
        
        result.value = std::move(parsed);
        return true;
    }
    
    static bool parseBool(const std::string& str, size_t& pos, JsonValue& result) {
        if (str.substr(pos, 4) == "true") {
            result.value = true;
            pos += 4;
            return true;
        } else if (str.substr(pos, 5) == "false") {
            result.value = false;
            pos += 5;
            return true;
        }
        return false;
    }
    
    static bool parseNumber(const std::string& str, size_t& pos, JsonValue& result) {
        size_t start = pos;
        if (str[pos] == '-') {
            pos++;
        }
        
        bool hasDecimal = false;
        while (pos < str.length() && (std::isdigit(str[pos]) || str[pos] == '.')) {
            if (str[pos] == '.') {
                if (hasDecimal) break; // Second decimal point not allowed
                hasDecimal = true;
            }
            pos++;
        }
        
        if (pos == start) {
            return false;
        }
        
        std::string numStr = str.substr(start, pos - start);
        
        // Check for scientific notation
        if (pos < str.length() && (str[pos] == 'e' || str[pos] == 'E')) {
            pos++;
            if (pos < str.length() && (str[pos] == '+' || str[pos] == '-')) {
                pos++;
            }
            while (pos < str.length() && std::isdigit(str[pos])) {
                pos++;
            }
        }
        
        // Parse number
        if (hasDecimal) {
            result.value = std::stod(numStr);
        } else {
            try {
                // Check if it's within int range
                long long_val = std::stoll(numStr);
                if (long_val <= std::numeric_limits<int>::max() && long_val >= std::numeric_limits<int>::min()) {
                    result.value = static_cast<int>(long_val);
                } else {
                    result.value = long_val;
                }
            } catch (...) {
                return false;
            }
        }
        
        return true;
    }
};

// YAML parser (simplified version)
namespace {

class SimpleYamlParser {
public:
    static bool parseYamlFile(const std::string& filepath, std::unordered_map<std::string, Config::ConfigValue>& result) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string line;
        std::vector<std::string> currentPath;
        std::stack<std::vector<std::string>> pathStack;
        
        while (std::getline(file, line)) {
            // Skip comments and empty lines
            line = Config::trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Handle indentation
            size_t indent = 0;
            while (indent < line.length() && (line[indent] == ' ' || line[indent] == '\t')) {
                indent++;
            }
            
            // Adjust current path based on indentation
            while (!pathStack.empty() && indent <= pathStack.top().size() * 2) {
                pathStack.pop();
                if (!currentPath.empty()) {
                    currentPath.pop_back();
                }
            }
            
            // Parse key-value pair
            size_t colonPos = line.find(':');
            if (colonPos != std::string::npos) {
                std::string key = Config::trim(line.substr(0, colonPos));
                std::string value = Config::trim(line.substr(colonPos + 1));
                
                // Remove quotes from string value
                if (value.size() >= 2 && ((value.front() == '\"' && value.back() == '\"') ||
                                         (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                // Convert value to appropriate type
                Config::ConfigValue configValue;
                if (!value.empty() && (value[0] == '-' || std::isdigit(value[0]))) {
                    // Try to parse as number
                    try {
                        if (value.find('.') != std::string::npos) {
                            configValue = std::stod(value);
                        } else {
                            configValue = std::stoll(value);
                        }
                    } catch (...) {
                        configValue = value;
                    }
                } else if (value == "true" || value == "false") {
                    configValue = (value == "true");
                } else {
                    configValue = value;
                }
                
                // Store with appropriate key path
                std::string fullKey = key;
                if (!currentPath.empty()) {
                    fullKey = currentPath.back() + "." + key;
                }
                
                result[fullKey] = configValue;
            } else {
                // It's a section header (simplified)
                currentPath.push_back(Config::trim(line));
                pathStack.push(currentPath);
            }
        }
        
        return true;
    }
};

} // namespace

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file type by extension
    std::string ext = filepath.substr(filepath.find_last_of('.') + 1);
    
    // First try JSON format
    if (ext == "json" || ext == "JSON") {
        SimpleJsonParser::JsonValue jsonData;
        if (SimpleJsonParser::parseJsonFile(filepath, jsonData)) {
            flattenJsonToConfig(jsonData, jsonData, "", ConfigSource::File);
            return true;
        }
    } 
    // Try YAML format
    else if (ext == "yaml" || ext == "yml" || ext == "YAML" || ext == "YML") {
        std::unordered_map<std::string, ConfigValue> yamlData;
        if (SimpleYamlParser::parseYamlFile(filepath, yamlData)) {
            pImpl->entries.clear();
            for (const auto& pair : yamlData) {
                set(pair.first, pair.second, ConfigSource::File);
            }
            return true;
        }
    }
    
    // Fallback to simple key=value format for .cfg files and other formats
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

void Config::flattenJsonToConfig(const SimpleJsonParser::JsonValue& jsonData, const SimpleJsonParser::JsonValue& current, 
                                 const std::string& prefix, ConfigSource source) {
    if (jsonData.isObject()) {
        const auto& obj = jsonData.getObject();
        for (const auto& pair : obj) {
            std::string newPrefix = prefix.empty() ? pair.first : prefix + "." + pair.first;
            flattenJsonToConfig(pair.second, pair.second, newPrefix, source);
        }
    } else if (jsonData.isArray()) {
        const auto& arr = jsonData.getArray();
        for (size_t i = 0; i < arr.size(); ++i) {
            std::string newPrefix = prefix + "[" + std::to_string(i) + "]";
            flattenJsonToConfig(arr[i], arr[i], newPrefix, source);
        }
    } else {
        // Convert JsonValue to ConfigValue
        ConfigValue configValue;
        if (jsonData.isInt()) {
            configValue = jsonData.getInt();
        } else if (jsonData.isInt64()) {
            configValue = jsonData.getInt64();
        } else if (jsonData.isDouble()) {
            configValue = jsonData.getDouble();
        } else if (jsonData.isBool()) {
            configValue = jsonData.getBool();
        } else if (jsonData.isString()) {
            configValue = jsonData.getString();
        }
        
        set(prefix, configValue, source);
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save in JSON format for structured data
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        // Convert ConfigValue to JSON string
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << val;
            } else if constexpr (std::is_same_v<T, double>) {
                file << val;
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < val.size(); ++j) {
                    if (j > 0) file << ", ";
                    // Simplified - just write raw values for now
                    file << "TODO";
                }
                file << "]";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    file << "}\n";
    
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
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
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
// (uint64_t and size_t are the same type on this platform, so only one is listed)
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
