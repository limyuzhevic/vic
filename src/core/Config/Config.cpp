#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <optional>
#include <cctype>
#include <cstring>
#include <stdexcept>
#include <regex>

// JSON parsing helpers
namespace nlm {
namespace json {

    // Simple JSON value type
    using JSONValue = std::variant<
        std::monostate,  // null
        bool,
        int,
        double,
        std::string,
        std::vector<JSONValue>
    >;

    // Parse a JSON string into a map of key-value pairs
    std::unordered_map<std::string, JSONValue> parseJSON(const std::string& jsonStr);

    // Convert JSON value to ConfigValue
    ConfigValue jsonToConfigValue(const JSONValue& jsonValue);

    // Helper functions
    std::string trim(const std::string& str);
    bool isWhitespace(char c);
    std::string extractString(const std::string& jsonStr, size_t& pos);
    double parseNumber(const std::string& jsonStr, size_t& pos);
    bool parseBool(const std::string& jsonStr, size_t& pos);
    size_t findNextCommaOrBrace(const std::string& jsonStr, size_t pos);

}}} // namespace nlm::json

// JSON parsing implementation
namespace nlm {
namespace json {

    // Trim whitespace from both ends
    std::string trim(const std::string& str) {
        size_t start = 0;
        size_t end = str.size();
        
        while (start < end && isWhitespace(str[start])) start++;
        while (end > start && isWhitespace(str[end - 1])) end--;
        
        return str.substr(start, end - start);
    }

    // Check if character is whitespace
    bool isWhitespace(char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    // Extract a JSON string value
    std::string extractString(const std::string& jsonStr, size_t& pos) {
        if (jsonStr[pos] != '"') {
            throw std::runtime_error("Expected string starting with \"");
        }
        
        pos++;  // Skip opening quote
        std::string result;
        
        while (pos < jsonStr.size()) {
            if (jsonStr[pos] == '\\') {
                pos++;
                if (pos >= jsonStr.size()) {
                    throw std::runtime_error("Invalid escape sequence in string");
                }
                result += jsonStr[pos];
                pos++;
            } else if (jsonStr[pos] == '"') {
                pos++;  // Skip closing quote
                return result;
            } else {
                result += jsonStr[pos];
                pos++;
            }
        }
        
        throw std::runtime_error("Unterminated string literal");
    }

    // Parse a number (integer or float)
    double parseNumber(const std::string& jsonStr, size_t& pos) {
        size_t start = pos;
        bool hasDecimal = false;
        bool hasExponent = false;
        
        while (pos < jsonStr.size()) {
            char c = jsonStr[pos];
            if (c == '.' && !hasDecimal) {
                hasDecimal = true;
            } else if ((c == 'e' || c == 'E') && !hasExponent) {
                hasExponent = true;
            } else if (std::isdigit(static_cast<unsigned char>(c))) {
                // Valid digit
            } else if (c == '-' && pos == start) {
                // Valid sign
            } else {
                break;
            }
            pos++;
        }
        
        if (pos == start) {
            throw std::runtime_error("Expected number");
        }
        
        std::string numStr = jsonStr.substr(start, pos - start);
        return std::stod(numStr);
    }

    // Parse a boolean value
    bool parseBool(const std::string& jsonStr, size_t& pos) {
        if (pos + 4 <= jsonStr.size() && jsonStr.substr(pos, 4) == "true") {
            pos += 4;
            return true;
        } else if (pos + 5 <= jsonStr.size() && jsonStr.substr(pos, 5) == "false") {
            pos += 5;
            return false;
        }
        
        throw std::runtime_error("Expected boolean value (true or false)");
    }

    // Find the next comma or closing brace/bracket
    size_t findNextCommaOrBrace(const std::string& jsonStr, size_t pos) {
        size_t braceCount = 0;
        size_t bracketCount = 0;
        bool inString = false;
        char escapeChar = '\0';
        
        for (size_t i = pos; i < jsonStr.size(); i++) {
            char c = jsonStr[i];
            
            if (escapeChar) {
                escapeChar = '\0';
            } else if (c == '\\') {
                escapeChar = c;
            } else if (!inString) {
                if (c == '{') braceCount++;
                else if (c == '}') braceCount--;
                else if (c == '[') bracketCount++;
                else if (c == ']') bracketCount--;
                else if (c == '"') inString = true;
                else if (c == ',') {
                    if (braceCount == 0 && bracketCount == 0) {
                        return i;
                    }
                } else if (c == '}') {
                    if (braceCount == 1 && bracketCount == 0) {
                        return i;
                    }
                } else if (c == ']') {
                    if (bracketCount == 1) {
                        return i;
                    }
                }
            } else if (c == '"') {
                inString = false;
            }
        }
        
        return jsonStr.size();
    }

    // Parse JSON object
    std::unordered_map<std::string, JSONValue> parseJSONObject(const std::string& jsonStr, size_t& pos) {
        std::unordered_map<std::string, JSONValue> result;
        
        if (jsonStr[pos] != '{') {
            throw std::runtime_error("Expected object starting with {");
        }
        
        pos++;  // Skip opening brace
        size_t endPos = findNextCommaOrBrace(jsonStr, pos);
        
        while (pos < endPos - 1) {
            // Skip whitespace
            while (pos < endPos && isWhitespace(jsonStr[pos])) pos++;
            
            if (pos >= endPos - 1) break;
            
            // Parse key
            std::string key;
            if (jsonStr[pos] == '"') {
                key = extractString(jsonStr, pos);
            } else {
                throw std::runtime_error("Expected string key");
            }
            
            // Skip whitespace after key
            while (pos < endPos && isWhitespace(jsonStr[pos])) pos++;
            
            if (pos >= endPos || jsonStr[pos] != ':') {
                throw std::runtime_error("Expected colon after key");
            }
            
            pos++;  // Skip colon
            while (pos < endPos && isWhitespace(jsonStr[pos])) pos++;
            
            // Parse value
            JSONValue value;
            if (pos >= endPos) {
                throw std::runtime_error("Unexpected end of object");
            }
            
            char c = jsonStr[pos];
            if (c == 'n' && pos + 3 < endPos && jsonStr.substr(pos, 4) == "null") {
                pos += 4;
                value = std::monostate{};
            } else if (c == 't' || c == 'f') {
                value = parseBool(jsonStr, pos);
            } else if (std::isdigit(static_cast<unsigned char>(c)) || c == '-') {
                value = parseNumber(jsonStr, pos);
            } else if (c == '"') {
                value = extractString(jsonStr, pos);
            } else if (c == '[') {
                // Parse array
                pos++;  // Skip opening bracket
                size_t arrayEnd = findNextCommaOrBrace(jsonStr, pos);
                std::vector<JSONValue> array;
                
                size_t arrayPos = pos;
                while (arrayPos < arrayEnd) {
                    while (arrayPos < arrayEnd && isWhitespace(jsonStr[arrayPos])) arrayPos++;
                    if (arrayPos >= arrayEnd) break;
                    
                    if (jsonStr[arrayPos] == ']') {
                        arrayPos++;
                        break;
                    }
                    
                    // Parse element (simplified - doesn't handle nested objects in arrays)
                    if (jsonStr[arrayPos] == 'n' && arrayPos + 3 < arrayEnd && jsonStr.substr(arrayPos, 4) == "null") {
                        arrayPos += 4;
                        array.push_back(std::monostate{});
                    } else if (jsonStr[arrayPos] == 't' || jsonStr[arrayPos] == 'f') {
                        array.push_back(parseBool(jsonStr, arrayPos));
                    } else if (std::isdigit(static_cast<unsigned char>(jsonStr[arrayPos])) || jsonStr[arrayPos] == '-') {
                        array.push_back(parseNumber(jsonStr, arrayPos));
                    } else if (jsonStr[arrayPos] == '"') {
                        array.push_back(extractString(jsonStr, arrayPos));
                    } else if (jsonStr[arrayPos] == '[') {
                        // TODO: Handle nested arrays
                        arrayPos++;
                        array.push_back(std::monostate{});  // Placeholder
                    } else {
                        throw std::runtime_error("Invalid JSON value in array");
                    }
                    
                    while (arrayPos < arrayEnd && isWhitespace(jsonStr[arrayPos])) arrayPos++;
                    if (arrayPos < arrayEnd && jsonStr[arrayPos] == ',') {
                        arrayPos++;
                    }
                }
                
                value = array;
                pos = arrayPos;
            } else if (c == '{') {
                // Parse nested object
                pos++;  // Skip opening brace
                size_t objectEnd = findNextCommaOrBrace(jsonStr, pos);
                auto nestedObject = parseJSONObject(jsonStr, pos);
                value = nestedObject;
                pos = objectEnd + 1;  // Skip closing brace
            } else {
                throw std::runtime_error("Invalid JSON value");
            }
            
            result[key] = value;
            
            // Skip whitespace after value
            while (pos < endPos && isWhitespace(jsonStr[pos])) pos++;
            
            if (pos < endPos && jsonStr[pos] == ',') {
                pos++;  // Skip comma
            } else if (pos < endPos && jsonStr[pos] == '}') {
                break;  // End of object
            }
        }
        
        if (pos >= endPos || jsonStr[pos] != '}') {
            throw std::runtime_error("Expected closing brace }");
        }
        
        pos++;  // Skip closing brace
        return result;
    }

    // Parse JSON array
    std::vector<JSONValue> parseJSONArray(const std::string& jsonStr, size_t& pos) {
        std::vector<JSONValue> result;
        
        if (jsonStr[pos] != '[') {
            throw std::runtime_error("Expected array starting with [");
        }
        
        pos++;  // Skip opening bracket
        size_t endPos = findNextCommaOrBrace(jsonStr, pos);
        
        while (pos < endPos) {
            // Skip whitespace
            while (pos < endPos && isWhitespace(jsonStr[pos])) pos++;
            
            if (pos >= endPos) break;
            
            if (jsonStr[pos] == ']') {
                pos++;
                break;
            }
            
            // Parse element
            JSONValue value;
            if (jsonStr[pos] == 'n' && pos + 3 < endPos && jsonStr.substr(pos, 4) == "null") {
                pos += 4;
                value = std::monostate{};
            } else if (jsonStr[pos] == 't' || jsonStr[pos] == 'f') {
                value = parseBool(jsonStr, pos);
            } else if (std::isdigit(static_cast<unsigned char>(jsonStr[pos])) || jsonStr[pos] == '-') {
                value = parseNumber(jsonStr, pos);
            } else if (jsonStr[pos] == '"') {
                value = extractString(jsonStr, pos);
            } else if (jsonStr[pos] == '[') {
                // Parse nested array
                value = parseJSONArray(jsonStr, pos);
            } else if (jsonStr[pos] == '{') {
                // Parse nested object
                auto object = parseJSONObject(jsonStr, pos);
                value = object;
            } else {
                throw std::runtime_error("Invalid JSON value in array");
            }
            
            result.push_back(value);
            
            // Skip whitespace after element
            while (pos < endPos && isWhitespace(jsonStr[pos])) pos++;
            
            if (pos < endPos && jsonStr[pos] == ',') {
                pos++;  // Skip comma
            } else if (pos < endPos && jsonStr[pos] == ']') {
                break;  // End of array
            }
        }
        
        if (pos >= endPos || jsonStr[pos] != ']') {
            throw std::runtime_error("Expected closing bracket ]");
        }
        
        pos++;  // Skip closing bracket
        return result;
    }

    // Main JSON parsing function
    std::unordered_map<std::string, JSONValue> parseJSON(const std::string& jsonStr) {
        std::unordered_map<std::string, JSONValue> result;
        size_t pos = 0;
        
        // Skip whitespace
        while (pos < jsonStr.size() && isWhitespace(jsonStr[pos])) pos++;
        
        if (pos >= jsonStr.size()) {
            throw std::runtime_error("Empty JSON string");
        }
        
        // Parse JSON (object or array)
        if (jsonStr[pos] == '{') {
            result = parseJSONObject(jsonStr, pos);
        } else if (jsonStr[pos] == '[') {
            auto array = parseJSONArray(jsonStr, pos);
            result["array"] = array;  // Store array under "array" key
        } else {
            throw std::runtime_error("JSON must start with { or [");
        }
        
        // Skip remaining whitespace
        while (pos < jsonStr.size() && isWhitespace(jsonStr[pos])) pos++;
        
        if (pos != jsonStr.size()) {
            throw std::runtime_error("Extra characters after JSON");
        }
        
        return result;
    }

    // Convert JSON value to ConfigValue
    ConfigValue jsonToConfigValue(const JSONValue& jsonValue) {
        if (std::holds_alternative<std::monostate>(jsonValue)) {
            // null -> empty string
            return "";
        } else if (std::holds_alternative<bool>(jsonValue)) {
            return std::get<bool>(jsonValue);
        } else if (std::holds_alternative<int>(jsonValue)) {
            return std::get<int>(jsonValue);
        } else if (std::holds_alternative<double>(jsonValue)) {
            double val = std::get<double>(jsonValue);
            // Check if it's an integer
            if (std::abs(val - static_cast<int>(val)) < 1e-9) {
                return static_cast<int>(val);
            }
            return val;
        } else if (std::holds_alternative<std::string>(jsonValue)) {
            return std::get<std::string>(jsonValue);
        } else if (std::holds_alternative<std::vector<JSONValue>>(jsonValue)) {
            const auto& jsonArray = std::get<std::vector<JSONValue>>(jsonValue);
            
            // Try to convert to int array
            std::vector<int> intArray;
            bool allInt = true;
            for (const auto& elem : jsonArray) {
                if (std::holds_alternative<int>(elem)) {
                    intArray.push_back(std::get<int>(elem));
                } else if (std::holds_alternative<double>(elem)) {
                    double val = std::get<double>(elem);
                    if (std::abs(val - static_cast<int>(val)) < 1e-9) {
                        intArray.push_back(static_cast<int>(val));
                    } else {
                        allInt = false;
                        break;
                    }
                } else {
                    allInt = false;
                    break;
                }
            }
            
            if (allInt && !intArray.empty()) {
                return intArray;
            }
            
            // Try to convert to double array
            std::vector<double> doubleArray;
            bool allDouble = true;
            for (const auto& elem : jsonArray) {
                if (std::holds_alternative<double>(elem)) {
                    doubleArray.push_back(std::get<double>(elem));
                } else if (std::holds_alternative<int>(elem)) {
                    doubleArray.push_back(static_cast<double>(std::get<int>(elem)));
                } else {
                    allDouble = false;
                    break;
                }
            }
            
            if (allDouble && !doubleArray.empty()) {
                return doubleArray;
            }
            
            // Try to convert to string array
            std::vector<std::string> stringArray;
            bool allString = true;
            for (const auto& elem : jsonArray) {
                if (std::holds_alternative<std::string>(elem)) {
                    stringArray.push_back(std::get<std::string>(elem));
                } else {
                    allString = false;
                    break;
                }
            }
            
            if (allString && !stringArray.empty()) {
                return stringArray;
            }
            
            // If we can't determine type, return as string representation
            std::string arrayStr = "[";
            for (size_t i = 0; i < jsonArray.size(); i++) {
                if (i > 0) arrayStr += ", ";
                const auto& elem = jsonArray[i];
                if (std::holds_alternative<std::monostate>(elem)) {
                    arrayStr += "null";
                } else if (std::holds_alternative<bool>(elem)) {
                    arrayStr += (std::get<bool>(elem) ? "true" : "false");
                } else if (std::holds_alternative<int>(elem)) {
                    arrayStr += std::to_string(std::get<int>(elem));
                } else if (std::holds_alternative<double>(elem)) {
                    arrayStr += std::to_string(std::get<double>(elem));
                } else if (std::holds_alternative<std::string>(elem)) {
                    arrayStr += "\"" + std::get<std::string>(elem) + "\"";
                }
            }
            arrayStr += "]";
            return arrayStr;
        } else {
            throw std::runtime_error("Unsupported JSON value type");
        }
    }

}}} // namespace nlm::json

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
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    try {
        // Parse JSON content
        auto jsonData = nlm::json::parseJSON(content);
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Convert JSON data to config entries
        for (const auto& pair : jsonData) {
            const std::string& key = pair.first;
            const nlm::json::JSONValue& jsonValue = pair.second;
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue = nlm::json::jsonToConfigValue(jsonValue);
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        // Log error (using existing logging system if available)
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Basic validation
                if (key.empty()) {
                    success = false;
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            } else {
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Basic validation
            if (key.empty()) {
                success = false;
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
        } else {
            // Unknown argument format
            success = false;
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";  // Start JSON object
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        file << "    \"" << entry.key << "\": ";
        
        // Write value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\""";
            } else if constexpr (std::is_same_v<T, int> || 
                                std::is_same_v<T, int64_t> ||
                                std::is_same_v<T, double> ||
                                std::is_same_v<T, float>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<int>> ||
                                  std::is_same_v<T, std::vector<double>>) {
                        file << arg[j];
                    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << "\"" << arg[j] << "\"";
                    }
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
