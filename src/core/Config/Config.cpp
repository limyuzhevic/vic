#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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

// JSON parser implementation
namespace {

class JsonParser {
public:
    struct JsonValue {
        enum class Type { Null, String, Number, Boolean, Array, Object };
        
        Type type;
        std::string stringValue;
        double numberValue;
        bool boolValue;
        std::vector<JsonValue> arrayValue;
        std::unordered_map<std::string, JsonValue> objectValue;
        
        JsonValue() : type(Type::Null), numberValue(0.0), boolValue(false) {}
        
        JsonValue(const std::string& str) : type(Type::String), stringValue(str) {}
        
        JsonValue(double num) : type(Type::Number), numberValue(num) {}
        
        JsonValue(bool val) : type(Type::Boolean), boolValue(val) {}
        
        JsonValue(const std::vector<JsonValue>& arr) : type(Type::Array), arrayValue(arr) {}
        
        JsonValue(const std::unordered_map<std::string, JsonValue>& obj) : type(Type::Object), objectValue(obj) {}
    };
    
    static bool parseJsonFile(const std::string& filepath, std::unordered_map<std::string, JsonValue>& outData) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        
        try {
            auto jsonValue = parseJsonValue(buffer.str());
            if (jsonValue.type != JsonValue::Type::Object) {
                return false; // Root must be an object
            }
            
            outData = std::move(jsonValue.objectValue);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    static bool parseJsonString(const std::string& jsonStr, std::unordered_map<std::string, JsonValue>& outData) {
        try {
            auto jsonValue = parseJsonValue(jsonStr);
            if (jsonValue.type != JsonValue::Type::Object) {
                return false; // Root must be an object
            }
            
            outData = std::move(jsonValue.objectValue);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    static bool jsonValueToConfigValue(const JsonValue& jsonVal, ConfigValue& outValue) {
        switch (jsonVal.type) {
            case JsonValue::Type::String:
                outValue = jsonVal.stringValue;
                return true;
            case JsonValue::Type::Number:
                // Check if it's an integer or float
                if (std::floor(jsonVal.numberValue) == jsonVal.numberValue) {
                    // Try to fit in available types
                    if (jsonVal.numberValue >= std::numeric_limits<int>::min() && 
                        jsonVal.numberValue <= std::numeric_limits<int>::max()) {
                        outValue = static_cast<int>(jsonVal.numberValue);
                    } else if (jsonVal.numberValue >= std::numeric_limits<int64_t>::min() && 
                               jsonVal.numberValue <= std::numeric_limits<int64_t>::max()) {
                        outValue = static_cast<int64_t>(jsonVal.numberValue);
                    } else {
                        outValue = jsonVal.numberValue;
                    }
                } else {
                    outValue = jsonVal.numberValue;
                }
                return true;
            case JsonValue::Type::Boolean:
                outValue = jsonVal.boolValue;
                return true;
            case JsonValue::Type::Array: {
                // Determine array element type
                if (jsonVal.arrayValue.empty()) {
                    // Default to int array
                    outValue = std::vector<int>{};
                    return true;
                }
                
                // Try to infer type from first element
                auto firstVal = jsonValToConfigValue(jsonVal.arrayValue[0]);
                if (!firstVal.has_value()) {
                    return false;
                }
                
                using FirstType = std::decay_t<decltype(firstVal.value())>;
                
                if constexpr (std::is_same_v<FirstType, int>) {
                    std::vector<int> intArray;
                    for (const auto& jsonItem : jsonVal.arrayValue) {
                        auto val = jsonValueToConfigValue(jsonItem);
                        if (!val.has_value()) return false;
                        if constexpr (std::is_same_v<decltype(val.value()), int>) {
                            intArray.push_back(val.value());
                        } else {
                            return false;
                        }
                    }
                    outValue = intArray;
                } else if constexpr (std::is_same_v<FirstType, double>) {
                    std::vector<double> doubleArray;
                    for (const auto& jsonItem : jsonVal.arrayValue) {
                        auto val = jsonValueToConfigValue(jsonItem);
                        if (!val.has_value()) return false;
                        if constexpr (std::is_same_v<decltype(val.value()), double>) {
                            doubleArray.push_back(val.value());
                        } else {
                            return false;
                        }
                    }
                    outValue = doubleArray;
                } else if constexpr (std::is_same_v<FirstType, std::string>) {
                    std::vector<std::string> stringArray;
                    for (const auto& jsonItem : jsonVal.arrayValue) {
                        auto val = jsonValueToConfigValue(jsonItem);
                        if (!val.has_value()) return false;
                        if constexpr (std::is_same_v<decltype(val.value()), std::string>) {
                            stringArray.push_back(val.value());
                        } else {
                            return false;
                        }
                    }
                    outValue = stringArray;
                } else {
                    return false;
                }
                return true;
            }
            case JsonValue::Type::Object:
                // Objects are not supported in ConfigValue, skip them
                return false;
            case JsonValue::Type::Null:
                return false;
        }
        return false;
    }
    
    static std::optional<ConfigValue> jsonValueToConfigValue(const JsonValue& jsonVal) {
        ConfigValue configVal;
        if (jsonValueToConfigValue(jsonVal, configVal)) {
            return configVal;
        }
        return std::nullopt;
    }
    
private:
    static JsonValue parseJsonValue(const std::string& jsonStr) {
        size_t pos = 0;
        skipWhitespace(jsonStr, pos);
        
        if (pos >= jsonStr.size()) {
            throw std::runtime_error("Unexpected end of JSON");
        }
        
        char ch = jsonStr[pos];
        
        if (ch == '{') {
            return parseJsonObject(jsonStr, pos);
        } else if (ch == '[') {
            return parseJsonArray(jsonStr, pos);
        } else if (ch == '"') {
            return parseJsonStringLiteral(jsonStr, pos);
        } else if (ch == 't' || ch == 'f') {
            return parseJsonBoolean(jsonStr, pos);
        } else if (ch == 'n') {
            return parseJsonNull(jsonStr, pos);
        } else {
            return parseJsonNumber(jsonStr, pos);
        }
    }
    
    static JsonValue parseJsonObject(const std::string& jsonStr, size_t& pos) {
        if (jsonStr[pos] != '{') {
            throw std::runtime_error("Expected '{' for object");
        }
        ++pos;
        skipWhitespace(jsonStr, pos);
        
        std::unordered_map<std::string, JsonValue> obj;
        
        if (jsonStr[pos] == '}') {
            ++pos;
            return JsonValue(obj);
        }
        
        while (true) {
            skipWhitespace(jsonStr, pos);
            
            // Parse key (must be string)
            if (jsonStr[pos] != '"') {
                throw std::runtime_error("Expected string key in object");
            }
            
            JsonValue key = parseJsonStringLiteral(jsonStr, pos);
            if (key.type != JsonValue::Type::String) {
                throw std::runtime_error("Object key must be a string");
            }
            
            skipWhitespace(jsonStr, pos);
            if (pos >= jsonStr.size() || jsonStr[pos] != ':') {
                throw std::runtime_error("Expected ':' after object key");
            }
            ++pos;
            skipWhitespace(jsonStr, pos);
            
            // Parse value
            JsonValue value = parseJsonValue(jsonStr, pos);
            obj[key.stringValue] = value;
            
            skipWhitespace(jsonStr, pos);
            
            if (jsonStr[pos] == '}') {
                ++pos;
                break;
            } else if (jsonStr[pos] == ',') {
                ++pos;
                continue;
            } else {
                throw std::runtime_error("Expected ',' or '}' in object");
            }
        }
        
        return JsonValue(obj);
    }
    
    static JsonValue parseJsonArray(const std::string& jsonStr, size_t& pos) {
        if (jsonStr[pos] != '[') {
            throw std::runtime_error("Expected '[' for array");
        }
        ++pos;
        skipWhitespace(jsonStr, pos);
        
        std::vector<JsonValue> arr;
        
        if (jsonStr[pos] == ']') {
            ++pos;
            return JsonValue(arr);
        }
        
        while (true) {
            JsonValue value = parseJsonValue(jsonStr, pos);
            arr.push_back(value);
            
            skipWhitespace(jsonStr, pos);
            
            if (jsonStr[pos] == ']') {
                ++pos;
                break;
            } else if (jsonStr[pos] == ',') {
                ++pos;
                continue;
            } else {
                throw std::runtime_error("Expected ',' or ']' in array");
            }
        }
        
        return JsonValue(arr);
    }
    
    static JsonValue parseJsonStringLiteral(const std::string& jsonStr, size_t& pos) {
        if (jsonStr[pos] != '"') {
            throw std::runtime_error("Expected '\"' for string literal");
        }
        
        ++pos;
        std::string result;
        
        while (pos < jsonStr.size() && jsonStr[pos] != '"') {
            if (jsonStr[pos] == '\\') {
                ++pos;
                if (pos >= jsonStr.size()) {
                    throw std::runtime_error("Unterminated escape sequence");
                }
                
                char escaped = jsonStr[pos];
                switch (escaped) {
                    case '"': result += '"'; break;
                    case '\\\\': result += '\\\\'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    default:
                        if (escaped >= '0' && escaped <= '7') {
                            // Octal escape
                            int octal = escaped - '0';
                            if (pos + 1 < jsonStr.size() && jsonStr[pos + 1] >= '0' && jsonStr[pos + 1] <= '7') {
                                octal = octal * 8 + (jsonStr[pos + 1] - '0');
                                ++pos;
                            }
                            if (pos + 1 < jsonStr.size() && jsonStr[pos + 1] >= '0' && jsonStr[pos + 1] <= '7') {
                                octal = octal * 8 + (jsonStr[pos + 1] - '0');
                                ++pos;
                            }
                            result += static_cast<char>(octal);
                        } else {
                            result += escaped;
                        }
                }
            } else {
                result += jsonStr[pos];
            }
            ++pos;
        }
        
        if (pos >= jsonStr.size() || jsonStr[pos] != '"') {
            throw std::runtime_error("Unterminated string literal");
        }
        
        ++pos;
        return JsonValue(result);
    }
    
    static JsonValue parseJsonBoolean(const std::string& jsonStr, size_t& pos) {
        if (pos + 4 <= jsonStr.size() && jsonStr.substr(pos, 4) == "true") {
            pos += 4;
            return JsonValue(true);
        } else if (pos + 5 <= jsonStr.size() && jsonStr.substr(pos, 5) == "false") {
            pos += 5;
            return JsonValue(false);
        } else {
            throw std::runtime_error("Invalid boolean literal");
        }
    }
    
    static JsonValue parseJsonNull(const std::string& jsonStr, size_t& pos) {
        if (pos + 4 <= jsonStr.size() && jsonStr.substr(pos, 4) == "null") {
            pos += 4;
            return JsonValue();
        } else {
            throw std::runtime_error("Invalid null literal");
        }
    }
    
    static JsonValue parseJsonNumber(const std::string& jsonStr, size_t& pos) {
        size_t startPos = pos;
        
        // Parse sign
        if (jsonStr[pos] == '+' || jsonStr[pos] == '-') {
            ++pos;
        }
        
        // Parse digits before decimal point
        while (pos < jsonStr.size() && std::isdigit(jsonStr[pos])) {
            ++pos;
        }
        
        // Parse decimal point and fractional part
        if (pos < jsonStr.size() && jsonStr[pos] == '.') {
            ++pos;
            while (pos < jsonStr.size() && std::isdigit(jsonStr[pos])) {
                ++pos;
            }
        }
        
        // Parse exponent
        if (pos < jsonStr.size() && (jsonStr[pos] == 'e' || jsonStr[pos] == 'E')) {
            ++pos;
            if (pos < jsonStr.size() && (jsonStr[pos] == '+' || jsonStr[pos] == '-')) {
                ++pos;
            }
            while (pos < jsonStr.size() && std::isdigit(jsonStr[pos])) {
                ++pos;
            }
        }
        
        std::string numStr = jsonStr.substr(startPos, pos - startPos);
        
        // Check if it's an integer or float
        bool isInteger = true;
        bool isNegative = false;
        
        if (!numStr.empty() && numStr[0] == '-') {
            isNegative = true;
            numStr = numStr.substr(1);
        }
        
        for (char ch : numStr) {
            if (ch == '.') {
                isInteger = false;
                break;
            }
            if (!std::isdigit(ch)) {
                throw std::runtime_error("Invalid number format");
            }
        }
        
        double value = std::stod(numStr);
        
        if (isInteger) {
            // Try to parse as integer
            try {
                if (isNegative) {
                    return JsonValue(static_cast<int64_t>(-value));
                } else {
                    return JsonValue(static_cast<int64_t>(value));
                }
            } catch (...) {
                // Fall back to double
                return JsonValue(value);
            }
        } else {
            return JsonValue(value);
        }
    }
    
    static void skipWhitespace(const std::string& jsonStr, size_t& pos) {
        while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
            ++pos;
        }
    }
};

} // anonymous namespace

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON/YAML first
    std::unordered_map<std::string, JsonParser::JsonValue> jsonData;
    
    // First try as JSON
    if (JsonParser::parseJsonFile(filepath, jsonData)) {
        for (const auto& pair : jsonData) {
            auto configVal = JsonParser::jsonValueToConfigValue(pair.second);
            if (configVal.has_value()) {
                set(pair.first, configVal.value(), ConfigSource::File);
            }
        }
        return true;
    }
    
    // Fall back to simple key=value format
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
        file << entry.key << " = ";
        
        // Convert ConfigValue to string
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, int> || 
                                std::is_same_v<T, int64_t> || 
                                std::is_same_v<T, float> || 
                                std::is_same_v<T, double>) {
                file << value;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (value ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                file << "["; 
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << "\"" << value[i] << "\"";
                    } else {
                        file << value[i];
                    }
                }
                file << "]";
            }
        }, entry.value);
        
        file << "\n\n";
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

// ===== ADVANCED CONFIGURATION IMPLEMENTATION =====

// Configuration type checking
bool Config::isNumericType(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) return false;
    
    using T = std::decay_t<decltype(it->value)>;
    return std::is_arithmetic_v<T> || std::is_same_v<T, int64_t>;
}

bool Config::isStringType(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) return false;
    
    using T = std::decay_t<decltype(it->value)>;
    return std::is_same_v<T, std::string>;
}

bool Config::isVectorType(const std::string& key) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) return false;
    
    using T = std::decay_t<decltype(it->value)>;
    return std::is_same_v<T, std::vector<int>> ||
           std::is_same_v<T, std::vector<double>> ||
           std::is_same_v<T, std::vector<std::string>>;
}

size_t Config::getVectorSize(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) return 0;
    
    using T = std::decay_t<decltype(it->value)>;
    if constexpr (std::is_same_v<T, std::vector<int>> ||
                  std::is_same_v<T, std::vector<double>> ||
                  std::is_same_v<T, std::vector<std::string>>) {
        return it->value.size();
    }
    return 0;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, source);
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        if (!validateEntry(ConfigEntry{key, value, source, ""})) {
            return; // Reject invalid entry
        }
        it->value = value;
        it->source = source;
    } else {
        if (!validateEntry(ConfigEntry{key, value, source, ""})) {
            return; // Reject invalid entry
        }
        pImpl->entries.emplace_back(key, value, source);
    }
}

bool Config::validateEntry(const ConfigEntry& entry) const {
    // Basic validation: key is not empty
    if (entry.key.empty()) return false;
    
    // Value validation based on type
    using T = std::decay_t<decltype(entry.value)>;
    
    if constexpr (std::is_same_v<T, std::string>) {
        // String must not be too long
        if (entry.value.size() > 10000) return false;
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        // String vectors: each string not too long
        for (const auto& str : entry.value) {
            if (str.size() > 1000) return false;
        }
    }
    
    return true;
}

std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (val ? "true" : "false");
        } else {
            oss << val;
        }
    }, value);
    return oss.str();
}

Config::ConfigStats Config::getStats() const {
    ConfigStats stats{};
    stats.totalEntries = pImpl->entries.size();
    
    for (const auto& entry : pImpl->entries) {
        stats.entriesBySource[static_cast<size_t>(entry.source)]++;
        
        using T = std::decay_t<decltype(entry.value)>;
        if constexpr (std::is_arithmetic_v<T> || std::is_same_v<T, int64_t>) {
            stats.numericEntries++;
        } else if constexpr (std::is_same_v<T, std::string>) {
            stats.stringEntries++;
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                           std::is_same_v<T, std::vector<double>> ||
                           std::is_same_v<T, std::vector<std::string>>) {
            stats.vectorEntries++;
        }
    }
    
    return stats;
}

// ===== END ADVANCED IMPLEMENTATION =====

// ===== TEMPLATE IMPLEMENTATIONS =====

template<typename T>
bool Config::hasType(const std::string& key) const {
    if (!has(key)) return false;
    auto val = get<T>(key);
    return val.has_value();
}

// ===== TEMPLATE INSTANTIATIONS =====
// (Already present in Config.cpp)

// Forward declaration helpers
static bool Config::hasType(const std::string& key) const;

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
