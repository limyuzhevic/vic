#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <cctype>
#include <cstring>
#include <iomanip>

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper functions
namespace {

// Skip whitespace
static void skipWhitespace(const std::string& str, size_t& pos) {
    while (pos < str.length() && std::isspace(str[pos])) {
        pos++;
    }
}

// Parse JSON string (with basic escape handling)
static std::string parseJSONString(const std::string& str, size_t& pos) {
    skipWhitespace(str, pos);
    if (str[pos] != '\"') {
        throw std::runtime_error("Expected quote for string value");
    }
    pos++;
    std::string result;
    while (pos < str.length() && str[pos] != '\"') {
        if (str[pos] == '\\' && pos + 1 < str.length()) {
            pos++; // Skip escape character
            if (pos < str.length()) {
                result += str[pos];
                pos++;
            }
        } else {
            result += str[pos];
            pos++;
        }
    }
    if (pos >= str.length() || str[pos] != '\"') {
        throw std::runtime_error("Unterminated string in JSON");
    }
    pos++; // Skip closing quote
    return result;
}

// Parse JSON value
static ConfigValue parseJSONValue(const std::string& str, size_t& pos) {
    skipWhitespace(str, pos);
    if (str[pos] == '\"') {
        std::string result = parseJSONString(str, pos);
        return result;
    } else if (str[pos] == 't' || str[pos] == 'f') {
        // Parse boolean
        size_t start = pos;
        while (pos < str.length() && std::isalnum(str[pos])) pos++;
        std::string boolStr = str.substr(start, pos - start);
        if (boolStr == "true") {
            return true;
        } else if (boolStr == "false") {
            return false;
        } else {
            throw std::runtime_error("Invalid boolean value in JSON: " + boolStr);
        }
    } else if (str[pos] == 'n') {
        // Parse null
        size_t start = pos;
        while (pos < str.length() && std::isalnum(str[pos])) pos++;
        std::string nullStr = str.substr(start, pos - start);
        if (nullStr == "null") {
            return "null"; // Use string representation for null
        } else {
            throw std::runtime_error("Invalid null value in JSON: " + nullStr);
        }
    } else if (str[pos] == '{') {
        // Parse object (key-value pairs)
        pos++; // Skip {
        std::map<std::string, ConfigValue> object;
        skipWhitespace(str, pos);
        if (str[pos] == '}') {
            pos++; // Skip }
            return object;
        }
        
        while (true) {
            // Parse key
            std::string key = parseJSONString(str, pos);
            skipWhitespace(str, pos);
            if (str[pos] != ':') {
                throw std::runtime_error("Expected colon after key in JSON object");
            }
            pos++; // Skip :
            
            // Parse value
            ConfigValue value = parseJSONValue(str, pos);
            object[key] = value;
            
            skipWhitespace(str, pos);
            if (str[pos] == '}') {
                pos++; // Skip }
                break;
            } else if (str[pos] == ',') {
                pos++; // Skip ,
                skipWhitespace(str, pos);
            } else {
                throw std::runtime_error("Expected comma or closing brace in JSON object");
            }
        }
        // For now, convert map to vector of ConfigEntry
        // TODO: Add proper vector storage for objects
        return object;
    } else if (str[pos] == '[') {
        // Parse array
        pos++; // Skip [
        std::vector<ConfigValue> array;
        skipWhitespace(str, pos);
        if (str[pos] == ']') {
            pos++; // Skip ]
            return array;
        }
        
        while (true) {
            ConfigValue value = parseJSONValue(str, pos);
            array.push_back(value);
            
            skipWhitespace(str, pos);
            if (str[pos] == ']') {
                pos++; // Skip ]
                break;
            } else if (str[pos] == ',') {
                pos++; // Skip ,
                skipWhitespace(str, pos);
            } else {
                throw std::runtime_error("Expected comma or closing bracket in JSON array");
            }
        }
        return array;
    } else if (str[pos] == '-' || std::isdigit(str[pos])) {
        // Parse number
        size_t start = pos;
        if (str[pos] == '-') pos++;
        while (pos < str.length() && std::isdigit(str[pos])) pos++;
        if (pos < str.length() && str[pos] == '.') {
            pos++; // Skip decimal point
            while (pos < str.length() && std::isdigit(str[pos])) pos++;
        }
        if (pos < str.length() && (str[pos] == 'e' || str[pos] == 'E')) {
            pos++; // Skip exponent indicator
            if (pos < str.length() && (str[pos] == '+' || str[pos] == '-')) pos++;
            while (pos < str.length() && std::isdigit(str[pos])) pos++;
        }
        std::string numStr = str.substr(start, pos - start);
        try {
            // Try to parse as integer first
            size_t pos2 = 0;
            long long intVal = std::stoll(numStr, &pos2);
            if (pos2 == numStr.length()) {
                return intVal;
            }
            // If not complete, try as double
            double doubleVal = std::stod(numStr);
            return doubleVal;
        } catch (...) {
            throw std::runtime_error("Invalid number in JSON: " + numStr);
        }
    } else {
        throw std::runtime_error("Unexpected character in JSON value: " + std::string(1, str[pos]));
    }
}

// Simple JSON parser for config files
static bool parseSimpleJSON(const std::string& content, std::map<std::string, ConfigValue>& result) {
    try {
        size_t pos = 0;
        if (content.empty()) {
            return true;
        }
        
        if (content[pos] != '{') {
            throw std::runtime_error("Expected { at start of JSON object");
        }
        pos++;
        
        skipWhitespace(content, pos);
        
        if (content[pos] == '}') {
            return true; // Empty object
        }
        
        while (true) {
            // Parse key
            size_t keyStart = pos;
            while (pos < content.length() && content[pos] != '\"' && 
                   !std::isspace(content[pos]) && content[pos] != '}') {
                pos++;
            }
            if (pos >= content.length() || content[pos] != '\"') {
                throw std::runtime_error("Expected quote for key");
            }
            pos++; // Skip quote
            keyStart = pos;
            while (pos < content.length() && content[pos] != '\"') {
                if (content[pos] == '\\' && pos + 1 < content.length()) {
                    pos++;
                }
                pos++;
            }
            if (pos >= content.length() || content[pos] != '\"') {
                throw std::runtime_error("Unterminated key");
            }
            pos++; // Skip closing quote
            std::string key = content.substr(keyStart, pos - keyStart);
            
            skipWhitespace(content, pos);
            if (pos < content.length() && content[pos] != ':') {
                throw std::runtime_error("Expected colon after key");
            }
            pos++; // Skip :
            
            skipWhitespace(content, pos);
            
            // Parse value
            ConfigValue value;
            if (content[pos] == '\"') {
                // String value
                size_t valueStart = pos;
                pos++; // Skip quote
                while (pos < content.length() && content[pos] != '\"' && content[pos] != ',') {
                    if (content[pos] == '\\' && pos + 1 < content.length()) {
                        pos++;
                    }
                    pos++;
                }
                if (pos >= content.length() || content[pos] != '\"') {
                    throw std::runtime_error("Unterminated string value");
                }
                value = content.substr(valueStart, pos - valueStart);
                pos++; // Skip closing quote
            } else if (content[pos] == 't' || content[pos] == 'f') {
                // Boolean value
                size_t valueStart = pos;
                while (pos < content.length() && (content[pos] == 't' || content[pos] == 'f' || content[pos] == ' ')) {
                    pos++;
                }
                std::string boolStr = content.substr(valueStart, pos - valueStart);
                value = (boolStr == "true");
            } else if (content[pos] == 'n') {
                // null
                size_t valueStart = pos;
                while (pos < content.length() && content[pos] == ' ') {
                    pos++;
                }
                std::string nullStr = content.substr(valueStart, pos - valueStart);
                value = "null";
            } else if (content[pos] == '-' || std::isdigit(content[pos])) {
                // Number
                size_t valueStart = pos;
                while (pos < content.length() && (std::isdigit(content[pos]) || content[pos] == '.' || content[pos] == '-')) {
                    pos++;
                }
                std::string numStr = content.substr(valueStart, pos - valueStart);
                try {
                    size_t pos2 = 0;
                    long long intVal = std::stoll(numStr, &pos2);
                    if (pos2 == numStr.length()) {
                        value = intVal;
                    } else {
                        double doubleVal = std::stod(numStr);
                        value = doubleVal;
                    }
                } catch (...) {
                    throw std::runtime_error("Invalid number: " + numStr);
                }
            } else if (content[pos] == '[') {
                // Array - simplified parsing
                size_t valueStart = pos;
                pos++; // Skip [
                while (pos < content.length() && content[pos] != ']') {
                    if (content[pos] == '\"') {
                        pos++; // Skip opening quote
                        while (pos < content.length() && content[pos] != '\"') {
                            if (content[pos] == '\\') pos++;
                            pos++;
                        }
                        if (pos < content.length()) pos++; // Skip closing quote
                    } else if (content[pos] == '{') {
                        // Nested object - skip
                        int depth = 1;
                        pos++;
                        while (pos < content.length() && depth > 0) {
                            if (content[pos] == '{') depth++;
                            else if (content[pos] == '}') depth--;
                            pos++;
                        }
                    } else {
                        pos++;
                    }
                    if (pos < content.length() && content[pos] == ',') {
                        pos++;
                    }
                }
                if (pos < content.length() && content[pos] == ']') {
                    pos++; // Skip closing bracket
                    value = std::vector<ConfigValue>{}; // Return empty array for now
                } else {
                    throw std::runtime_error("Unterminated array");
                }
            }
            
            result[key] = value;
            
            skipWhitespace(content, pos);
            if (pos < content.length() && content[pos] == '}') {
                break;
            } else if (pos < content.length() && content[pos] == ',') {
                pos++; // Skip comma
                skipWhitespace(content, pos);
            } else {
                throw std::runtime_error("Expected } or , after JSON entry");
            }
        }
    } catch (const std::exception& e) {
        throw std::runtime_error("JSON parsing error: " + std::string(e.what()));
    }
    return true;
}

} // namespace

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to detect file format
    std::string content((std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>()));
    
    clear(); // Clear existing entries
    
    try {
        // Try JSON format first (starts with {)
        if (!content.empty() && content[0] == '{') {
            std::map<std::string, ConfigValue> jsonMap;
            parseSimpleJSON(content, jsonMap);
            
            // Convert map to entries
            for (const auto& pair : jsonMap) {
                // Convert map to string for storage
                std::string valueStr;
                if (std::holds_alternative<int>(pair.second)) {
                    valueStr = std::to_string(std::get<int>(pair.second));
                } else if (std::holds_alternative<int64_t>(pair.second)) {
                    valueStr = std::to_string(std::get<int64_t>(pair.second));
                } else if (std::holds_alternative<double>(pair.second)) {
                    valueStr = std::to_string(std::get<double>(pair.second));
                } else if (std::holds_alternative<bool>(pair.second)) {
                    valueStr = std::get<bool>(pair.second) ? "true" : "false";
                } else {
                    valueStr = std::get<std::string>(pair.second);
                }
                set(pair.first, valueStr, ConfigSource::File);
            }
            return true;
        }
        
        // Fall back to simple key=value format
        std::string line;
        std::istringstream iss(content);
        while (std::getline(iss, line)) {
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs
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
    } catch (const std::exception& e) {
        throw std::runtime_error("Configuration file parsing error: " + std::string(e.what()));
    }
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
    
    // Save in JSON format for better compatibility
    file << "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        } else {
            first = false;
        }
        file << "  \"" << entry.key << "\": ";
        
        // Convert value to JSON representation
        std::visit([&file](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << val;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::setprecision(6) << std::fixed << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else {
                file << "???";
            }
        }, entry.value);
        
        file << "\n";
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
