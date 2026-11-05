#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp> // JSON parser (Phase 6: proper config support)

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Simple JSON parser (header-only, no external dependencies)
namespace json {
    
    // JSON value types
    using JSONValue = std::variant<
        std::monostate, // null
        bool,
        int64_t,
        double,
        std::string,
        std::vector<JSONValue>
    >;
    
    // Parse JSON string into value
    JSONValue parseJSONValue(const std::string& str, size_t& pos);
    
    // Parse JSON object
    std::unordered_map<std::string, JSONValue> parseJSONObject(const std::string& str, size_t& pos);
    
    // Trim whitespace
    std::string trim(const std::string& str);
    
    // Check if character is whitespace
    bool isWhitespace(char c);
}

// Implement the JSON parser inline
namespace json {
    std::string json::trim(const std::string& str) {
        size_t start = 0, end = str.size();
        while (start < end && isWhitespace(str[start])) start++;
        while (end > start && isWhitespace(str[end - 1])) end--;
        return str.substr(start, end - start);
    }
    
    bool json::isWhitespace(char c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }
    
    JSONValue json::parseJSONValue(const std::string& str, size_t& pos) {
        std::string trimmed = trim(str.substr(pos));
        if (trimmed.empty()) {
            return std::monostate{}; // null
        }
        
        // Skip whitespace
        while (pos < str.size() && isWhitespace(str[pos])) pos++;
        
        if (str[pos] == '{') {
            // Parse object
            pos++;
            auto obj = parseJSONObject(str, pos);
            if (str[pos] == '}') pos++;
            return obj;
        } else if (str[pos] == '[') {
            // Parse array
            pos++;
            std::vector<JSONValue> arr;
            
            if (str[pos] == ']') {
                pos++;
                return arr;
            }
            
            while (pos < str.size() && str[pos] != ']') {
                arr.push_back(parseJSONValue(str, pos));
                
                // Skip whitespace and commas
                while (pos < str.size() && isWhitespace(str[pos])) pos++;
                if (pos < str.size() && str[pos] == ',') pos++;
            }
            
            if (pos < str.size() && str[pos] == ']') pos++;
            return arr;
        } else if (str[pos] == '"') {
            // Parse string
            pos++;
            std::string result;
            while (pos < str.size() && str[pos] != '"') {
                if (str[pos] == '\\' && pos + 1 < str.size()) {
                    pos++; // skip escape char
                    if (pos < str.size()) {
                        switch (str[pos]) {
                            case '\"': result += '\"'; break;
                            case '\\': result += '\\'; break;
                            case '/': result += '/'; break;
                            case 'b': result += '\b'; break;
                            case 'f': result += '\f'; break;
                            case 'n': result += '\n'; break;
                            case 'r': result += '\r'; break;
                            case 't': result += '\t'; break;
                            default: result += str[pos]; break;
                        }
                    }
                } else {
                    result += str[pos];
                }
                pos++;
            }
            
            if (pos < str.size() && str[pos] == '"') pos++;
            return result;
        } else if (std::isdigit(str[pos]) || str[pos] == '-') {
            // Parse number
            size_t start = pos;
            while (pos < str.size() && (std::isdigit(str[pos]) || str[pos] == '.' || str[pos] == 'e' || str[pos] == 'E' || str[pos] == '+' || str[pos] == '-')) {
                pos++;
            }
            
            std::string numStr = str.substr(start, pos - start);
            try {
                if (numStr.find('.') != std::string::npos || numStr.find('e') != std::string::npos || numStr.find('E') != std::string::npos) {
                    return std::stod(numStr);
                } else {
                    return std::stoll(numStr);
                }
            } catch (...) {
                return 0.0;
            }
        } else if (str.compare(pos, 4, "true") == 0) {
            pos += 4;
            return true;
        } else if (str.compare(pos, 5, "false") == 0) {
            pos += 5;
            return false;
        } else if (str.compare(pos, 4, "null") == 0) {
            pos += 4;
            return std::monostate{};
        }
        
        return std::monostate{}; // null for unknown
    }
    
    std::unordered_map<std::string, JSONValue> json::parseJSONObject(const std::string& str, size_t& pos) {
        std::unordered_map<std::string, JSONValue> obj;
        
        while (pos < str.size() && str[pos] != '}') {
            // Skip whitespace
            while (pos < str.size() && isWhitespace(str[pos])) pos++;
            
            if (pos >= str.size() || str[pos] == '}') break;
            
            // Parse key (must be string)
            if (str[pos] == '"') {
                pos++; // skip opening quote
                std::string key;
                while (pos < str.size() && str[pos] != '"') {
                    if (str[pos] == '\\' && pos + 1 < str.size()) {
                        pos++; // skip escape char
                        if (pos < str.size()) key += str[pos];
                    } else {
                        key += str[pos];
                    }
                    pos++;
                }
                
                if (pos < str.size() && str[pos] == '"') pos++; // skip closing quote
                
                // Skip whitespace
                while (pos < str.size() && isWhitespace(str[pos])) pos++;
                
                if (pos < str.size() && str[pos] == ':') pos++; // skip colon
                
                // Skip whitespace
                while (pos < str.size() && isWhitespace(str[pos])) pos++;
                
                // Parse value
                obj[key] = parseJSONValue(str, pos);
                
                // Skip whitespace and comma
                while (pos < str.size() && isWhitespace(str[pos])) pos++;
                if (pos < str.size() && str[pos] == ',') pos++;
            } else {
                // Skip to next key
                while (pos < str.size() && str[pos] != '"' && str[pos] != '}') pos++;
            }
        }
        
        return obj;
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    // Phase 6: Use proper JSON parser instead of simple key=value format
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Could not open config file: " + filepath);
            return false;
        }
        
        // Read entire file content
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Try to parse as JSON first
        size_t pos = 0;
        auto jsonData = json::parseJSONObject(content, pos);
        
        if (!jsonData.empty()) {
            // Parse JSON entries
            for (const auto& pair : jsonData) {
                std::string key = pair.first;
                
                // Convert JSON value to ConfigValue
                ConfigValue value;
                const auto& jsonVal = pair.second;
                
                if (std::holds_alternative<bool>(jsonVal)) {
                    value = std::get<bool>(jsonVal);
                } else if (std::holds_alternative<int64_t>(jsonVal)) {
                    value = std::get<int64_t>(jsonVal);
                } else if (std::holds_alternative<double>(jsonVal)) {
                    value = std::get<double>(jsonVal);
                } else if (std::holds_alternative<std::string>(jsonVal)) {
                    value = std::get<std::string>(jsonVal);
                } else if (std::holds_alternative<std::vector<json::JSONValue>>(jsonVal)) {
                    // Handle arrays
                    const auto& arr = std::get<std::vector<json::JSONValue>>(jsonVal);
                    if (arr.empty()) {
                        continue; // Skip empty arrays
                    }
                    
                    // Determine array element type by checking first element
                    if (std::holds_alternative<int64_t>(arr[0])) {
                        std::vector<int64_t> intArray;
                        for (const auto& elem : arr) {
                            if (std::holds_alternative<int64_t>(elem)) {
                                intArray.push_back(std::get<int64_t>(elem));
                            }
                        }
                        value = intArray;
                    } else if (std::holds_alternative<double>(arr[0])) {
                        std::vector<double> doubleArray;
                        for (const auto& elem : arr) {
                            if (std::holds_alternative<double>(elem)) {
                                doubleArray.push_back(std::get<double>(elem));
                            }
                        }
                        value = doubleArray;
                    } else if (std::holds_alternative<std::string>(arr[0])) {
                        std::vector<std::string> stringArray;
                        for (const auto& elem : arr) {
                            if (std::holds_alternative<std::string>(elem)) {
                                stringArray.push_back(std::get<std::string>(elem));
                            }
                        }
                        value = stringArray;
                    } else {
                        NLM_LOG_WARNING("Unsupported array type in config key: " + key);
                        continue;
                    }
                } else {
                    NLM_LOG_WARNING("Unsupported JSON type for config key: " + key);
                    continue;
                }
                
                // Set the value with File source
                set(key, value, ConfigSource::File);
            }
            
            NLM_LOG_INFO("Loaded JSON configuration from: " + filepath);
            return true;
        } else {
            // Fall back to simple key=value format
            return loadFromFileFallback(filepath);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse config file " + filepath + ": " + std::string(e.what()));
        // Fall back to simple parser if JSON parsing fails
        return loadFromFileFallback(filepath);
    }
}

bool Config::loadFromFileFallback(const std::string& filepath) {
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
    
    NLM_LOG_INFO("Loaded configuration from: " + filepath);
    return true;
}
    
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

bool Config::loadFromArgs(int argc, char** argv) {
    bool hasValidArgs = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format with error checking
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key (allow alphanumeric, underscore, dash)
                if (!isKeyValid(key)) {
                    NLM_LOG_WARNING("Invalid config key: " + key + ". Skipping.");
                    continue;
                }
                
                // Try to parse value and convert to appropriate type
                bool parsed = false;
                
                // Try boolean
                if (value == "true" || value == "false") {
                    set(key, value == "true", ConfigSource::CommandLine);
                    parsed = true;
                }
                // Try integer
                else if (isInteger(value)) {
                    try {
                        int64_t intVal = std::stoll(value);
                        set(key, intVal, ConfigSource::CommandLine);
                        parsed = true;
                    } catch (...) {
                        NLM_LOG_WARNING("Failed to parse integer value for key: " + key);
                        continue;
                    }
                }
                // Try floating point
                else if (isFloatingPoint(value)) {
                    try {
                        double floatVal = std::stod(value);
                        set(key, floatVal, ConfigSource::CommandLine);
                        parsed = true;
                    } catch (...) {
                        NLM_LOG_WARNING("Failed to parse float value for key: " + key);
                        continue;
                    }
                }
                // Try array format [val1,val2,val3]
                else if (value.size() >= 2 && value.front() == '[' && value.back() == ']') {
                    std::string arrayContent = value.substr(1, value.size() - 2);
                    set(key, parseArrayValue(arrayContent), ConfigSource::CommandLine);
                    parsed = true;
                }
                // Default to string
                else {
                    set(key, value, ConfigSource::CommandLine);
                    parsed = true;
                }
                
                if (!parsed) {
                    NLM_LOG_WARNING("Failed to parse value for key: " + key + ". Using string.");
                }
                
                hasValidArgs = true;
            } else {
                NLM_LOG_ERROR("Invalid command line argument: " + arg + ". Expected --key=value format.");
                hasValidArgs = false;
            }
        }
        // Handle -key value format with error checking
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key (allow alphanumeric, underscore, dash)
            if (!isKeyValid(key)) {
                NLM_LOG_WARNING("Invalid config key: " + key + ". Skipping.");
                continue;
            }
            
            // Try to parse value and convert to appropriate type (same as above)
            bool parsed = false;
            
            // Try boolean
            if (value == "true" || value == "false") {
                set(key, value == "true", ConfigSource::CommandLine);
                parsed = true;
            }
            // Try integer
            else if (isInteger(value)) {
                try {
                    int64_t intVal = std::stoll(value);
                    set(key, intVal, ConfigSource::CommandLine);
                    parsed = true;
                } catch (...) {
                    NLM_LOG_WARNING("Failed to parse integer value for key: " + key);
                    continue;
                }
            }
            // Try floating point
            else if (isFloatingPoint(value)) {
                try {
                    double floatVal = std::stod(value);
                    set(key, floatVal, ConfigSource::CommandLine);
                    parsed = true;
                } catch (...) {
                    NLM_LOG_WARNING("Failed to parse float value for key: " + key);
                    continue;
                }
            }
            // Try array format [val1,val2,val3]
            else if (value.size() >= 2 && value.front() == '[' && value.back() == ']') {
                std::string arrayContent = value.substr(1, value.size() - 2);
                set(key, parseArrayValue(arrayContent), ConfigSource::CommandLine);
                parsed = true;
            }
            // Default to string
            else {
                set(key, value, ConfigSource::CommandLine);
                parsed = true;
            }
            
            if (!parsed) {
                NLM_LOG_WARNING("Failed to parse value for key: " + key + ". Using string.");
            }
            
            hasValidArgs = true;
        }
        else {
            NLM_LOG_WARNING("Unrecognized command line argument: " + arg);
            hasValidArgs = false;
        }
    }
    
    if (hasValidArgs) {
        NLM_LOG_INFO("Loaded configuration from command line arguments");
    } else {
        NLM_LOG_INFO("No valid configuration arguments found");
    }
    
    return true;
}

// Helper functions for argument validation and parsing
bool Config::isKeyValid(const std::string& key) const {
    if (key.empty()) return false;
    
    for (char c : key) {
        if (!(std::isalnum(c) || c == '_' || c == '-' || c == '.')) {
            return false;
        }
    }
    
    return true;
}

bool Config::isInteger(const std::string& str) const {
    if (str.empty()) return false;
    
    size_t pos = 0;
    if (str[0] == '-' || str[0] == '+') pos++;
    
    for (; pos < str.size(); ++pos) {
        if (!std::isdigit(str[pos])) return false;
    }
    
    return pos > (str[0] == '-' || str[0] == '+' ? 1 : 0);
}

bool Config::isFloatingPoint(const std::string& str) const {
    if (str.empty()) return false;
    
    size_t dotCount = 0;
    size_t eCount = 0;
    size_t pos = 0;
    
    if (str[0] == '-' || str[0] == '+') pos++;
    
    for (; pos < str.size(); ++pos) {
        if (str[pos] == '.') {
            dotCount++;
            if (dotCount > 1) return false;
        } else if (str[pos] == 'e' || str[pos] == 'E') {
            eCount++;
            if (eCount > 1) return false;
            
            // After 'e', expect optional sign and digits
            if (pos + 1 >= str.size()) return false;
            pos++; // Skip 'e'
            if (pos < str.size() && (str[pos] == '-' || str[pos] == '+')) {
                pos++;
            }
            if (pos >= str.size() || !std::isdigit(str[pos])) return false;
        } else if (!std::isdigit(str[pos])) {
            return false;
        }
    }
    
    return dotCount <= 1 && eCount <= 1;
}

ConfigValue Config::parseArrayValue(const std::string& arrayStr) const {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(arrayStr);
    
    while (std::getline(tokenStream, token, ',')) {
        token = trim(token);
        if (!token.empty()) {
            tokens.push_back(token);
        }
    }
    
    if (tokens.empty()) {
        return std::vector<double>{}; // Empty double array
    }
    
    // Try to determine array type by checking first element
    bool allIntegers = true;
    bool allFloats = true;
    bool allStrings = true;
    
    for (const auto& t : tokens) {
        if (!isInteger(t)) allIntegers = false;
        if (!isFloatingPoint(t)) allFloats = false;
        if (t.front() != '"' || t.back() != '"') allStrings = false;
    }
    
    if (allIntegers) {
        std::vector<int64_t> intArray;
        for (const auto& t : tokens) {
            try {
                intArray.push_back(std::stoll(t));
            } catch (...) {
                // If parsing fails, treat as string (fallback)
                allIntegers = false;
                break;
            }
        }
        if (allIntegers) {
            return intArray;
        }
    }
    
    if (allFloats) {
        std::vector<double> floatArray;
        for (const auto& t : tokens) {
            try {
                floatArray.push_back(std::stod(t));
            } catch (...) {
                // If parsing fails, treat as string (fallback)
                allFloats = false;
                break;
            }
        }
        if (allFloats) {
            return floatArray;
        }
    }
    
    if (allStrings) {
        std::vector<std::string> stringArray;
        for (const auto& t : tokens) {
            std::string value = t;
            // Remove quotes from string values
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            stringArray.push_back(value);
        }
        return stringArray;
    }
    
    // Mixed type array - convert to string representation for now
    std::vector<std::string> mixedArray;
    for (const auto& t : tokens) {
        mixedArray.push_back(t);
    }
    return mixedArray;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Serialize the value to file format
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << '"' << val << '"';
            } else if constexpr (std::is_same_v<T, int64_t> || 
                                std::is_same_v<T, uint64_t> || 
                                std::is_same_v<T, double>) {
                file << val;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << val[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << val[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << '"' << val[i] << '"';
                }
                file << "]";
            } else {
                file << "UNKNOWN_TYPE";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Saved configuration to: " + filepath);
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
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

} // namespace nlm
