#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <variant>
#include <string>
#include <optional>
#include <algorithm>

namespace nlm {

// Simple JSON parser implementation
class SimpleJsonParser {
public:
    struct JsonValue {
        std::variant<int, double, bool, std::string, std::unordered_map<std::string, JsonValue>, std::vector<JsonValue>> value;
        
        // Helper methods for type checking
        bool isNumber() const { return std::holds_alternative<int>(value) || std::holds_alternative<double>(value); }
        bool isString() const { return std::holds_alternative<std::string>(value); }
        bool isBool() const { return std::holds_alternative<bool>(value); }
        bool isArray() const { return std::holds_alternative<std::vector<JsonValue>>(value); }
        bool isObject() const { return std::holds_alternative<std::unordered_map<std::string, JsonValue>>(value); }
        
        // Get typed values
        int getInt() const { return std::get<int>(value); }
        double getDouble() const { return std::get<double>(value); }
        bool getBool() const { return std::get<bool>(value); }
        const std::string& getString() const { return std::get<std::string>(value); }
        const std::vector<JsonValue>& getArray() const { return std::get<std::vector<JsonValue>>(value); }
        const std::unordered_map<std::string, JsonValue>& getObject() const { return std::get<std::unordered_map<std::string, JsonValue>>(value); }
    };
    
    bool parse(const std::string& jsonStr, JsonValue& result) {
        try {
            // Remove whitespace
            std::string cleaned;
            for (char c : jsonStr) {
                if (!std::isspace(c)) cleaned += c;
            }
            
            return parseValue(cleaned, 0, result);
        } catch (const std::exception&) {
            return false;
        }
    }
    
private:
    bool parseValue(const std::string& json, size_t& pos, JsonValue& result) {
        skipWhitespace(json, pos);
        
        if (pos >= json.length()) return false;
        
        char c = json[pos];
        
        if (c == '{') {
            // Object
            return parseObject(json, pos, result);
        } else if (c == '[') {
            // Array
            return parseArray(json, pos, result);
        } else if (c == '"') {
            // String
            std::string str;
            if (parseString(json, pos, str)) {
                result.value = str;
                return true;
            }
            return false;
        } else if (std::isdigit(c) || c == '-') {
            // Number
            return parseNumber(json, pos, result);
        } else if (c == 't' || c == 'f') {
            // Boolean
            return parseBool(json, pos, result);
        } else if (c == 'n') {
            // Null (we don't support null, but could treat as empty string)
            std::string nullStr = "null";
            if (json.substr(pos, 4) == nullStr) {
                pos += 4;
                result.value = std::string();
                return true;
            }
            return false;
        }
        
        return false;
    }
    
    bool parseObject(const std::string& json, size_t& pos, JsonValue& result) {
        if (json[pos] != '{') return false;
        pos++;
        skipWhitespace(json, pos);
        
        if (json[pos] == '}') {
            pos++;
            result.value = std::unordered_map<std::string, JsonValue>();
            return true;
        }
        
        auto& obj = result.value = std::unordered_map<std::string, JsonValue>();
        
        while (true) {
            skipWhitespace(json, pos);
            
            // Parse key
            std::string key;
            if (!parseString(json, pos, key)) return false;
            skipWhitespace(json, pos);
            
            if (pos >= json.length() || json[pos] != ':') return false;
            pos++;
            skipWhitespace(json, pos);
            
            // Parse value
            JsonValue value;
            if (!parseValue(json, pos, value)) return false;
            obj[key] = std::move(value);
            
            skipWhitespace(json, pos);
            
            if (pos >= json.length()) return false;
            
            if (json[pos] == '}') {
                pos++;
                return true;
            } else if (json[pos] == ',') {
                pos++;
            } else {
                return false;
            }
        }
    }
    
    bool parseArray(const std::string& json, size_t& pos, JsonValue& result) {
        if (json[pos] != '[') return false;
        pos++;
        skipWhitespace(json, pos);
        
        if (json[pos] == ']') {
            pos++;
            result.value = std::vector<JsonValue>();
            return true;
        }
        
        auto& arr = result.value = std::vector<JsonValue>();
        
        while (true) {
            JsonValue element;
            if (!parseValue(json, pos, element)) return false;
            arr.push_back(std::move(element));
            
            skipWhitespace(json, pos);
            
            if (pos >= json.length()) return false;
            
            if (json[pos] == ']') {
                pos++;
                return true;
            } else if (json[pos] == ',') {
                pos++;
            } else {
                return false;
            }
        }
    }
    
    bool parseString(const std::string& json, size_t& pos, std::string& result) {
        if (json[pos] != '"') return false;
        pos++;
        
        std::string str;
        while (pos < json.length() && json[pos] != '"') {
            if (json[pos] == '\\') {
                // Handle escape sequences
                pos++;
                if (pos >= json.length()) return false;
                
                char escaped = json[pos];
                switch (escaped) {
                    case '\\': str += '\\'; break;
                    case '\"': str += '"'; break;
                    case '/': str += '/'; break;
                    case 'b': str += '\b'; break;
                    case 'f': str += '\f'; break;
                    case 'n': str += '\n'; break;
                    case 'r': str += '\r'; break;
                    case 't': str += '\t'; break;
                    default: str += escaped; break;
                }
            } else {
                str += json[pos];
            }
            pos++;
        }
        
        if (pos >= json.length() || json[pos] != '"') return false;
        pos++;
        
        result = str;
        return true;
    }
    
    bool parseNumber(const std::string& json, size_t& pos, JsonValue& result) {
        size_t start = pos;
        
        if (json[pos] == '-') {
            pos++;
            if (pos >= json.length() || !std::isdigit(json[pos])) return false;
        }
        
        while (pos < json.length() && std::isdigit(json[pos])) {
            pos++;
        }
        
        if (pos < json.length() && json[pos] == '.') {
            pos++;
            if (pos >= json.length() || !std::isdigit(json[pos])) return false;
            
            while (pos < json.length() && std::isdigit(json[pos])) {
                pos++;
            }
        }
        
        std::string numStr = json.substr(start, pos - start);
        
        try {
            if (numStr.find('.') != std::string::npos) {
                result.value = std::stod(numStr);
            } else {
                result.value = std::stoi(numStr);
            }
            return true;
        } catch (...) {
            return false;
        }
    }
    
    bool parseBool(const std::string& json, size_t& pos, JsonValue& result) {
        if (json.substr(pos, 4) == "true") {
            pos += 4;
            result.value = true;
            return true;
        } else if (json.substr(pos, 5) == "false") {
            pos += 5;
            result.value = false;
            return true;
        }
        return false;
    }
    
    void skipWhitespace(const std::string& json, size_t& pos) {
        while (pos < json.length() && std::isspace(json[pos])) {
            pos++;
        }
    }
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    SimpleJsonParser jsonParser;
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
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try JSON format first
    SimpleJsonParser::JsonValue jsonData;
    if (jsonParser.parse(content, jsonData)) {
        // Parse JSON object
        auto& jsonObject = jsonData.getObject();
        for (const auto& [key, value] : jsonObject) {
            ConfigValue configValue;
            if (value.isNumber()) {
                // Try to detect if it's int or double
                try {
                    int intVal = value.getInt();
                    configValue = intVal;
                } catch (...) {
                    configValue = value.getDouble();
                }
            } else if (value.isString()) {
                configValue = value.getString();
            } else if (value.isBool()) {
                configValue = value.getBool();
            } else if (value.isArray()) {
                // Convert array to vector based on element type
                const auto& arr = value.getArray();
                if (!arr.empty()) {
                    if (arr[0].isNumber()) {
                        std::vector<double> doubleArr;
                        for (const auto& element : arr) {
                            if (element.isNumber()) {
                                doubleArr.push_back(element.getDouble());
                            }
                        }
                        configValue = doubleArr;
                    } else if (arr[0].isString()) {
                        std::vector<std::string> stringArr;
                        for (const auto& element : arr) {
                            if (element.isString()) {
                                stringArr.push_back(element.getString());
                            }
                        }
                        configValue = stringArr;
                    }
                }
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    }
    
    // Fall back to simple key=value format
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
        } else {
            // Warn about malformed lines (but don't fail)
            std::cerr << "Warning: Invalid config line " << lineNumber << ": " << line << std::endl;
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
    
    file << "{" << std::endl;
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) file << "," << std::endl;
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format with 6 decimal places for consistency
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                const auto& vec = arg;
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << vec[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                const auto& vec = arg;
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::fixed << std::setprecision(6) << vec[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                const auto& vec = arg;
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << escapeString(vec[i]) << "\"";
                }
                file << "]";
            }
        }, entry.value);
        
        // Add description as comment
        if (!entry.description.empty()) {
            file << "  /* " << entry.description << " */";
        }
    }
    
    file << std::endl << "}" << std::endl;
    
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
    oss << "Configuration (" << pImpl->entries.size() << " entries):" << std::endl;
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                const auto& vec = arg;
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << vec[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                const auto& vec = arg;
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << std::fixed << std::setprecision(6) << vec[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                const auto& vec = arg;
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << vec[i] << "\"";
                }
                oss << "]";
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")" << std::endl;
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

std::string Config::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default: result += c; break;
        }
    }
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