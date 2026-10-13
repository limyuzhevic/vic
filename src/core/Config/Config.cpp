#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <variant>
#include <vector>
#include <string>
#include <cctype>
#include <stdexcept>
#include <ctime>

// Simple JSON parsing implementation
namespace nlm {
namespace json {

// JSON value types
using JSONValue = std::variant<
    std::nullptr_t,
    bool,
    int,
    int64_t,
    double,
    std::string,
    std::vector<JSONValue>,
    std::unordered_map<std::string, JSONValue>
>;

// JSON parsing utilities
class JSONParser {
public:
    static bool parseFile(const std::string& filepath, std::unordered_map<std::string, JSONValue>& out);
    static bool parseString(const std::string& jsonStr, std::unordered_map<std::string, JSONValue>& out);
    static std::string serializeToString(const std::unordered_map<std::string, JSONValue>& data);
    static bool isWhitespace(char c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r'; }
    static bool isDigit(char c) { return c >= '0' && c <= '9'; }
    static bool isAlpha(char c) { return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'; }
};

bool JSONParser::parseFile(const std::string& filepath, std::unordered_map<std::string, JSONValue>& out) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return parseString(buffer.str(), out);
}

bool JSONParser::parseString(const std::string& jsonStr, std::unordered_map<std::string, JSONValue>& out) {
    try {
        // Simple JSON parsing for configuration format
        size_t pos = 0;
        
        while (pos < jsonStr.size()) {
            // Skip whitespace
            while (pos < jsonStr.size() && isWhitespace(jsonStr[pos])) {
                pos++;
            }
            
            if (pos >= jsonStr.size()) break;
            
            // Check for end of object
            if (jsonStr[pos] == '}') {
                break;
            }
            
            // Parse key (must be quoted string)
            if (jsonStr[pos] != '"') {
                return false; // Expected quoted key
            }
            
            size_t keyStart = ++pos;
            while (pos < jsonStr.size() && jsonStr[pos] != '"') {
                pos++;
            }
            
            if (pos >= jsonStr.size()) {
                return false; // Unterminated string
            }
            
            std::string key = jsonStr.substr(keyStart, pos - keyStart);
            pos++; // Skip closing quote
            
            // Skip whitespace after key
            while (pos < jsonStr.size() && isWhitespace(jsonStr[pos])) {
                pos++;
            }
            
            if (pos >= jsonStr.size() || jsonStr[pos] != ':') {
                return false; // Expected colon after key
            }
            
            pos++; // Skip colon
            
            // Skip whitespace after colon
            while (pos < jsonStr.size() && isWhitespace(jsonStr[pos])) {
                pos++;
            }
            
            if (pos >= jsonStr.size()) {
                return false;
            }
            
            // Parse value based on first character
            JSONValue value;
            
            if (jsonStr[pos] == '"') {
                // String value
                size_t valueStart = ++pos;
                while (pos < jsonStr.size() && jsonStr[pos] != '"') {
                    if (jsonStr[pos] == '\\') {
                        pos += 2; // Skip escaped character
                    }
                    pos++;
                }
                if (pos >= jsonStr.size()) {
                    return false;
                }
                value = jsonStr.substr(valueStart, pos - valueStart);
                pos++; // Skip closing quote
            }
            else if (jsonStr[pos] == 't' || jsonStr[pos] == 'f') {
                // Boolean value
                if (pos + 4 < jsonStr.size() && jsonStr.substr(pos, 5) == "true") {
                    value = true;
                    pos += 5;
                } else if (pos + 5 < jsonStr.size() && jsonStr.substr(pos, 6) == "false") {
                    value = false;
                    pos += 6;
                } else {
                    return false;
                }
            }
            else if (jsonStr[pos] == 'n') {
                // Null value
                if (pos + 3 < jsonStr.size() && jsonStr.substr(pos, 4) == "null") {
                    value = std::nullptr_t{};
                    pos += 4;
                } else {
                    return false;
                }
            }
            else if (jsonStr[pos] == '-' || isDigit(jsonStr[pos])) {
                // Number value
                size_t numStart = pos;
                bool isFloat = false;
                
                while (pos < jsonStr.size() && (isDigit(jsonStr[pos]) || jsonStr[pos] == '.' || jsonStr[pos] == '-' || jsonStr[pos] == 'e' || jsonStr[pos] == 'E')) {
                    if (jsonStr[pos] == '.') isFloat = true;
                    pos++;
                }
                
                std::string numStr = jsonStr.substr(numStart, pos - numStart);
                
                if (isFloat) {
                    try {
                        size_t posAfter;
                        double val = std::stod(numStr, &posAfter);
                        if (posAfter == numStr.size()) {
                            value = val;
                        } else {
                            return false;
                        }
                    } catch (...) {
                        return false;
                    }
                } else {
                    try {
                        size_t posAfter;
                        int64_t val = std::stoll(numStr, &posAfter);
                        if (posAfter == numStr.size()) {
                            value = val;
                        } else {
                            return false;
                        }
                    } catch (...) {
                        return false;
                    }
                }
            }
            else if (jsonStr[pos] == '[') {
                // Array value
                std::vector<JSONValue> array;
                pos++; // Skip opening bracket
                
                while (pos < jsonStr.size() && jsonStr[pos] != ']') {
                    // Parse element
                    JSONValue element;
                    size_t elementStart = pos;
                    
                    // Recursively parse element (simplified)
                    if (jsonStr[pos] == '"') {
                        // String element
                        size_t strStart = ++pos;
                        while (pos < jsonStr.size() && jsonStr[pos] != '"') {
                            if (jsonStr[pos] == '\\') pos += 2;
                            pos++;
                        }
                        if (pos >= jsonStr.size()) return false;
                        element = jsonStr.substr(strStart, pos - strStart);
                        pos++;
                    }
                    else if (jsonStr[pos] == 't' || jsonStr[pos] == 'f') {
                        // Boolean element
                        if (pos + 4 < jsonStr.size() && jsonStr.substr(pos, 5) == "true") {
                            element = true;
                            pos += 5;
                        } else if (pos + 5 < jsonStr.size() && jsonStr.substr(pos, 6) == "false") {
                            element = false;
                            pos += 6;
                        } else {
                            return false;
                        }
                    }
                    else if (jsonStr[pos] == '-' || isDigit(jsonStr[pos])) {
                        // Number element
                        size_t numStart = pos;
                        bool isFloat = false;
                        
                        while (pos < jsonStr.size() && (isDigit(jsonStr[pos]) || jsonStr[pos] == '.' || jsonStr[pos] == '-' || jsonStr[pos] == 'e' || jsonStr[pos] == 'E')) {
                            if (jsonStr[pos] == '.') isFloat = true;
                            pos++;
                        }
                        
                        std::string numStr = jsonStr.substr(numStart, pos - numStart);
                        
                        if (isFloat) {
                            try {
                                size_t posAfter;
                                double val = std::stod(numStr, &posAfter);
                                if (posAfter == numStr.size()) {
                                    element = val;
                                } else {
                                    return false;
                                }
                            } catch (...) {
                                return false;
                            }
                        } else {
                            try {
                                size_t posAfter;
                                int64_t val = std::stoll(numStr, &posAfter);
                                if (posAfter == numStr.size()) {
                                    element = val;
                                } else {
                                    return false;
                                }
                            } catch (...) {
                                return false;
                            }
                        }
                    }
                    else {
                        return false; // Unsupported array element type
                    }
                    
                    array.push_back(element);
                    
                    // Skip whitespace
                    while (pos < jsonStr.size() && isWhitespace(jsonStr[pos])) {
                        pos++;
                    }
                    
                    if (pos < jsonStr.size() && jsonStr[pos] == ',') {
                        pos++; // Skip comma
                    } else if (pos < jsonStr.size() && jsonStr[pos] != ']') {
                        return false;
                    }
                }
                
                if (pos >= jsonStr.size() || jsonStr[pos] != ']') {
                    return false;
                }
                
                value = array;
                pos++; // Skip closing bracket
            }
            else {
                return false; // Unsupported value type
            }
            
            // Skip whitespace before next key or end
            while (pos < jsonStr.size() && isWhitespace(jsonStr[pos])) {
                pos++;
            }
            
            if (pos < jsonStr.size() && jsonStr[pos] == ',') {
                pos++; // Skip comma
            } else if (pos < jsonStr.size() && jsonStr[pos] != '}') {
                return false;
            }
            
            // Store the key-value pair
            out[key] = value;
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

std::string JSONParser::serializeToString(const std::unordered_map<std::string, JSONValue>& data) {
    std::ostringstream oss;
    oss << "{\n";
    
    size_t count = 0;
    for (const auto& pair : data) {
        oss << "  \"" << pair.first << "\": ";
        
        const JSONValue& value = pair.second;
        
        std::visit([&oss](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            
            if constexpr (std::is_same_v<T, std::nullptr_t>) {
                oss << "null";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                oss << val;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::setprecision(6) << val;
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<JSONValue>>) {
                oss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) oss << ", ";
                    // Simplified serialization - could be improved
                    if (std::holds_alternative<std::string>(val[i])) {
                        oss << "\"" << std::get<std::string>(val[i]) << "\"";
                    } else if (std::holds_alternative<bool>(val[i])) {
                        oss << (std::get<bool>(val[i]) ? "true" : "false");
                    } else if (std::holds_alternative<int>(val[i]) || std::holds_alternative<int64_t>(val[i])) {
                        oss << std::get<int64_t>(val[i]);
                    } else if (std::holds_alternative<double>(val[i])) {
                        oss << std::get<double>(val[i]);
                    }
                }
                oss << "]";
            }
        }, value);
        
        count++;
        if (count < data.size()) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    
    oss << "}\n";
    return oss.str();
}

} // namespace json

// Legacy simple parser for backward compatibility
bool parseSimpleFile(const std::string& filepath, std::unordered_map<std::string, ConfigValue>& out) {
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
            
            // Try to infer type
            if (value == "true" || value == "false") {
                out[key] = (value == "true");
            } else {
                try {
                    // Try integer
                    size_t posAfter;
                    long long intVal = std::stoll(value, &posAfter);
                    if (posAfter == value.size()) {
                        out[key] = intVal;
                    } else {
                        // Try double
                        double doubleVal = std::stod(value, &posAfter);
                        if (posAfter == value.size()) {
                            out[key] = doubleVal;
                        } else {
                            // String
                            out[key] = value;
                        }
                    }
                } catch (...) {
                    out[key] = value;
                }
            }
        }
    }
    
    return true;
}

} // namespace nlm

// Updated Config class with improved JSON/YAML support
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, nlm::json::JSONValue> jsonCache;
    std::string lastFilepath;
    bool jsonMode;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->jsonMode = true;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    pImpl->jsonMode = true;
    pImpl->lastFilepath = filepath;
    
    // Try to parse as JSON first
    if (nlm::json::JSONParser::parseFile(filepath, pImpl->jsonCache)) {
        // Successfully parsed as JSON
        // Convert JSON to Config entries
        pImpl->entries.clear();
        
        for (const auto& pair : pImpl->jsonCache) {
            ConfigValue configValue;
            const auto& jsonValue = pair.second;
            
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                
                if constexpr (std::is_same_v<T, std::nullptr_t>) {
                    configValue = std::string("null");
                } else if constexpr (std::is_same_v<T, bool>) {
                    configValue = val;
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    configValue = static_cast<int64_t>(val);
                } else if constexpr (std::is_same_v<T, double>) {
                    configValue = val;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    configValue = val;
                } else if constexpr (std::is_same_v<T, std::vector<nlm::json::JSONValue>>) {
                    // Convert array to string representation for now
                    configValue = "[]"; // TODO: Proper array support
                } else if constexpr (std::is_same_v<T, std::unordered_map<std::string, nlm::json::JSONValue>>) {
                    // Convert object to string representation for now
                    configValue = "{}"; // TODO: Proper object support
                }
            }, jsonValue);
            
            pImpl->entries.emplace_back(pair.first, configValue, ConfigSource::File, "");
        }
        
        return true;
    }
    
    // Fallback to simple key=value format
    pImpl->jsonMode = false;
    return nlm::parseSimpleFile(filepath, pImpl->jsonCache);
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
    
    if (pImpl->jsonMode && !pImpl->jsonCache.empty()) {
        // Save in JSON format
        std::string jsonContent = nlm::json::JSONParser::serializeToString(pImpl->jsonCache);
        file << jsonContent;
    } else {
        // Save in simple key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Convert ConfigValue to string
            std::visit([&](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << val << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (val ? "true" : "false");
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << val;
                } else if constexpr (std::is_same_v<T, double>) {
                    file << std::setprecision(6) << val;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t i = 0; i < val.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << val[i] << "\"";
                    }
                    file << "]";
                } else {
                    file << "UNKNOWN_TYPE";
                }
            }, entry.value);
            
            file << "\n\n";
        }
    }
    
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
    
    // Update JSON cache if in JSON mode
    if (pImpl->jsonMode) {
        // Convert ConfigValue to JSONValue
        auto& jsonValue = pImpl->jsonCache[key];
        
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                jsonValue = val;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonValue = val;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                jsonValue = static_cast<int64_t>(val);
            } else if constexpr (std::is_same_v<T, double>) {
                jsonValue = val;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                // Convert string vector to JSON array
                std::vector<nlm::json::JSONValue> array;
                for (const auto& str : val) {
                    array.push_back(str);
                }
                jsonValue = array;
            }
        }, value);
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
    
    // Also remove from JSON cache
    if (pImpl->jsonMode) {
        pImpl->jsonCache.erase(key);
    }
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
    pImpl->jsonCache.clear();
    pImpl->lastFilepath.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " << (pImpl->jsonMode ? "JSON" : "Simple") << "):\n";
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
    
    if (!pImpl->jsonCache.empty()) {
        oss << "\nJSON Cache:" << std::endl;
        for (const auto& pair : pImpl->jsonCache) {
            oss << "  \"" << pair.first << "\": ";
            // Simplified JSON serialization
            oss << "JSON_VALUE\n";
        }
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
