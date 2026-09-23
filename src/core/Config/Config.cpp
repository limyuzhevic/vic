#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstring>
#include <unordered_map>
#include <cctype>
#include <iterator>
#include <stack>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// JSON parsing implementation
namespace json {

// Skip whitespace (spaces, tabs, newlines)
static bool skipWhitespace(const std::string& str, size_t& pos) {
    while (pos < str.size() && std::isspace(str[pos])) {
        pos++;
    }
    return pos < str.size();
}

// Parse a JSON string value
static std::string parseString(const std::string& str, size_t& pos) {
    if (str[pos] != '\"') return "";
    
    std::string result;
    pos++; // Skip opening quote
    
    while (pos < str.size()) {
        if (str[pos] == '\"') {
            pos++; // Skip closing quote
            return result;
        }
        
        if (str[pos] == '\\\\' && pos + 1 < str.size()) {
            pos++; // Skip backslash
            if (pos < str.size()) {
                switch (str[pos]) {
                    case '\"': result += '\"'; break;
                    case '\\\\': result += '\\\\'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    default: result += str[pos]; break; // Keep unknown escapes
                }
            }
        } else {
            result += str[pos];
        }
        pos++;
    }
    
    return result; // Unterminated string, return what we have
}

// Parse a number value (int, float, scientific notation)
static double parseNumber(const std::string& str, size_t& pos) {
    size_t start = pos;
    
    // Optional minus sign
    if (pos < str.size() && str[pos] == '-') {
        pos++;
    }
    
    // Integer part
    while (pos < str.size() && std::isdigit(str[pos])) {
        pos++;
    }
    
    // Decimal part
    if (pos < str.size() && str[pos] == '.') {
        pos++;
        while (pos < str.size() && std::isdigit(str[pos])) {
            pos++;
        }
    }
    
    // Exponent part
    if (pos < str.size() && (str[pos] == 'e' || str[pos] == 'E')) {
        pos++;
        if (pos < str.size() && (str[pos] == '+' || str[pos] == '-')) {
            pos++;
        }
        while (pos < str.size() && std::isdigit(str[pos])) {
            pos++;
        }
    }
    
    return std::stod(str.substr(start, pos - start));
}

// Parse a boolean value
static bool parseBoolean(const std::string& str, size_t& pos) {
    if (pos + 4 <= str.size() && str.substr(pos, 4) == "true") {
        pos += 4;
        return true;
    } else if (pos + 5 <= str.size() && str.substr(pos, 5) == "false") {
        pos += 5;
        return false;
    }
    return false;
}

// Parse null value
static bool parseNull(const std::string& str, size_t& pos) {
    if (pos + 4 <= str.size() && str.substr(pos, 4) == "null") {
        pos += 4;
        return true;
    }
    return false;
}

// Parse a JSON value (object, array, string, number, boolean, null)
static bool parseValue(const std::string& str, size_t& pos, ConfigValue& outValue) {
    if (!skipWhitespace(str, pos)) return false;
    
    char current = str[pos];
    
    if (current == '\"') {
        // String
        std::string strValue = parseString(str, pos);
        skipWhitespace(str, pos);
        outValue = strValue;
        return true;
    } else if (current == '-' || std::isdigit(current)) {
        // Number
        double numValue = parseNumber(str, pos);
        // Determine if it's an integer
        if (std::floor(numValue) == numValue) {
            outValue = static_cast<int64_t>(numValue);
        } else {
            outValue = numValue;
        }
        return true;
    } else if (current == 't' || current == 'f') {
        // Boolean
        bool boolValue = parseBoolean(str, pos);
        outValue = boolValue;
        return true;
    } else if (current == 'n') {
        // Null
        bool nullResult = parseNull(str, pos);
        if (nullResult) {
            // For null, we store it as empty string as a placeholder
            outValue = std::string();
        }
        return nullResult;
    } else if (current == '{') {
        // Object - currently not supported in simple parsing
        // Skip object for now
        pos++;
        int braceCount = 1;
        while (pos < str.size() && braceCount > 0) {
            if (str[pos] == '{') braceCount++;
            else if (str[pos] == '}') braceCount--;
            pos++;
        }
        outValue = std::string(); // Placeholder for object
        return true;
    } else if (current == '[') {
        // Array - currently not supported in simple parsing
        // Skip array for now
        pos++;
        int bracketCount = 1;
        while (pos < str.size() && bracketCount > 0) {
            if (str[pos] == '[') bracketCount++;
            else if (str[pos] == ']') bracketCount--;
            pos++;
        }
        outValue = std::string(); // Placeholder for array
        return true;
    }
    
    return false;
}

// Parse key-value pairs for objects
static bool parseObject(const std::string& str, size_t& pos, std::unordered_map<std::string, std::string>& obj) {
    if (!skipWhitespace(str, pos) || str[pos] != '{') return false;
    
    pos++; // Skip opening brace
    obj.clear();
    
    while (skipWhitespace(str, pos) && str[pos] != '}') {
        std::string key;
        ConfigValue value;
        
        // Parse key (must be a string)
        if (str[pos] != '\"') return false;
        key = parseString(str, pos);
        
        skipWhitespace(str, pos);
        
        // Expect colon
        if (pos >= str.size() || str[pos] != ':') return false;
        pos++;
        
        // Parse value
        if (!parseValue(str, pos, value)) return false;
        
        // Store in map
        std::visit([&obj, &key](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                obj[key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, double> ||
                                 std::is_same_v<T, bool>) {
                std::ostringstream oss;
                oss << arg;
                obj[key] = oss.str();
            }
        }, value);
        
        // Skip trailing comma if present
        skipWhitespace(str, pos);
        if (pos < str.size() && str[pos] == ',') {
            pos++;
        }
    }
    
    if (pos < str.size() && str[pos] == '}') {
        pos++; // Skip closing brace
        return true;
    }
    
    return false;
}

} // namespace json

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Check if it's valid JSON
    size_t jsonPos = 0;
    skipWhitespace(content, jsonPos);
    
    if (jsonPos >= content.size() || content[jsonPos] != '{') {
        // Not JSON format, try simple key=value format for backward compatibility
        file.clear();
        file.seekg(0);
        
        std::string line;
        int lineNum = 0;
        while (std::getline(file, line)) {
            lineNum++;
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t equalsPos = line.find('=');
            if (equalsPos != std::string::npos) {
                std::string key = trim(line.substr(0, equalsPos));
                std::string value = trim(line.substr(equalsPos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '\"' && value.back() == '\"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            } else {
                NLM_LOG_WARNING("Skipping invalid config line " + std::to_string(lineNum) + ": " + line);
            }
        }
        
        NLM_LOG_INFO("Loaded config file " + filepath + " using simple key=value format (" + 
                    std::to_string(pImpl->entries.size()) + " entries)");
        return true;
    }
    
    // Try to parse as JSON
    std::unordered_map<std::string, std::string> jsonObj;
    if (json::parseObject(content, jsonPos, jsonObj)) {
        // Successfully parsed as JSON
        pImpl->entries.clear();
        
        for (const auto& pair : jsonObj) {
            // Try to infer type from string value
            const std::string& valueStr = pair.second;
            ConfigValue configValue;
            
            // Try to parse as boolean
            if (valueStr == "true" || valueStr == "false") {
                configValue = (valueStr == "true");
            }
            // Try to parse as integer
            else if (!valueStr.empty() && (std::all_of(valueStr.begin(), valueStr.end(), ::isdigit) || 
                                         (valueStr[0] == '-' && std::all_of(valueStr.begin() + 1, valueStr.end(), ::isdigit)))) {
                configValue = std::stoll(valueStr);
            }
            // Try to parse as floating point
            else {
                try {
                    size_t parsePos = 0;
                    double doubleVal = std::stod(valueStr, &parsePos);
                    if (parsePos == valueStr.size()) {
                        configValue = doubleVal;
                    } else {
                        configValue = valueStr; // Keep as string
                    }
                } catch (...) {
                    configValue = valueStr; // Keep as string
                }
            }
            
            set(pair.first, configValue, ConfigSource::File);
        }
        
        NLM_LOG_INFO("Loaded JSON config file " + filepath + " (" + 
                    std::to_string(pImpl->entries.size()) + " entries)");
        return true;
    }
    
    NLM_LOG_ERROR("Failed to parse config file: " + filepath);
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

// Enhanced error handling and JSON save functionality
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to create config file: " + filepath);
        return false;
    }
    
    file << "// NLM Configuration File\n";
    file << "// Generated automatically - DO NOT EDIT MANUALLY\n\n";
    
    // Write as JSON format for better compatibility
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) file << ",\n";
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Write with reasonable precision
                file.precision(6);
                file << std::fixed << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            }
        }, entry.value);
    }
    
    file << "\n}\n";
    
    NLM_LOG_INFO("Saved config to file " + filepath + " (" + 
                std::to_string(pImpl->entries.size()) + " entries)");
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
