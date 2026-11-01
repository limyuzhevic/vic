#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <unordered_map>
#include <chrono>

// Simple JSON parser for configuration files
namespace {

// Simple JSON value type
struct JsonValue {
    enum Type { OBJECT, ARRAY, STRING, NUMBER, BOOL, NULL_VALUE };
    
    Type type;
    std::string stringValue;
    double numberValue;
    bool boolValue;
    std::unordered_map<std::string, JsonValue> objectValue;
    std::vector<JsonValue> arrayValue;
    
    JsonValue() : type(NULL_VALUE) {}
    JsonValue(const std::string& s) : type(STRING), stringValue(s) {}
    JsonValue(double n) : type(NUMBER), numberValue(n) {}
    JsonValue(bool b) : type(BOOL), boolValue(b) {}
    JsonValue(const std::unordered_map<std::string, JsonValue>& obj) : 
        type(OBJECT), objectValue(obj) {}
    JsonValue(const std::vector<JsonValue>& arr) : 
        type(ARRAY), arrayValue(arr) {}
};

// Simple JSON parser (minimal implementation for config files)
class SimpleJsonParser {
public:
    static bool parse(const std::string& jsonStr, JsonValue& outValue) {
        try {
            size_t pos = 0;
            return parseValue(jsonStr, pos, outValue);
        } catch (const std::exception&) {
            return false;
        }
    }
    
private:
    static bool parseValue(const std::string& jsonStr, size_t& pos, JsonValue& outValue) {
        skipWhitespace(jsonStr, pos);
        
        if (pos >= jsonStr.size()) {
            return false;
        }
        
        char ch = jsonStr[pos];
        
        if (ch == '{') {
            return parseObject(jsonStr, pos, outValue);
        } else if (ch == '[') {
            return parseArray(jsonStr, pos, outValue);
        } else if (ch == '"') {
            return parseString(jsonStr, pos, outValue);
        } else if (ch == 't' || ch == 'f') {
            return parseBoolean(jsonStr, pos, outValue);
        } else if (ch == 'n') {
            return parseNull(jsonStr, pos, outValue);
        } else {
            return parseNumber(jsonStr, pos, outValue);
        }
    }
    
    static bool parseObject(const std::string& jsonStr, size_t& pos, JsonValue& outValue) {
        if (jsonStr[pos] != '{') return false;
        pos++;
        
        std::unordered_map<std::string, JsonValue> obj;
        skipWhitespace(jsonStr, pos);
        
        if (jsonStr[pos] == '}') {
            pos++;
            outValue = JsonValue(obj);
            return true;
        }
        
        while (true) {
            skipWhitespace(jsonStr, pos);
            std::string key;
            if (!parseString(jsonStr, pos, key)) return false;
            
            skipWhitespace(jsonStr, pos);
            if (pos >= jsonStr.size() || jsonStr[pos] != ':') return false;
            pos++;
            
            skipWhitespace(jsonStr, pos);
            
            JsonValue value;
            if (!parseValue(jsonStr, pos, value)) return false;
            
            obj[key] = value;
            
            skipWhitespace(jsonStr, pos);
            if (pos >= jsonStr.size()) return false;
            
            if (jsonStr[pos] == '}') {
                pos++;
                break;
            } else if (jsonStr[pos] == ',') {
                pos++;
                continue;
            } else {
                return false;
            }
        }
        
        outValue = JsonValue(obj);
        return true;
    }
    
    static bool parseArray(const std::string& jsonStr, size_t& pos, JsonValue& outValue) {
        if (jsonStr[pos] != '[') return false;
        pos++;
        
        std::vector<JsonValue> arr;
        skipWhitespace(jsonStr, pos);
        
        if (jsonStr[pos] == ']') {
            pos++;
            outValue = JsonValue(arr);
            return true;
        }
        
        while (true) {
            JsonValue value;
            if (!parseValue(jsonStr, pos, value)) return false;
            
            arr.push_back(value);
            
            skipWhitespace(jsonStr, pos);
            if (pos >= jsonStr.size()) return false;
            
            if (jsonStr[pos] == ']') {
                pos++;
                break;
            } else if (jsonStr[pos] == ',') {
                pos++;
                continue;
            } else {
                return false;
            }
        }
        
        outValue = JsonValue(arr);
        return true;
    }
    
    static bool parseString(const std::string& jsonStr, size_t& pos, std::string& outValue) {
        if (jsonStr[pos] != '"') return false;
        pos++;
        
        size_t start = pos;
        while (pos < jsonStr.size() && jsonStr[pos] != '"') {
            if (jsonStr[pos] == '\\' && pos + 1 < jsonStr.size()) {
                pos++; // Skip escaped character
            }
            pos++;
        }
        
        if (pos >= jsonStr.size()) return false;
        
        outValue = jsonStr.substr(start, pos - start);
        pos++; // Skip closing quote
        return true;
    }
    
    static bool parseBoolean(const std::string& jsonStr, size_t& pos, JsonValue& outValue) {
        if (jsonStr.compare(pos, 4, "true") == 0) {
            outValue = JsonValue(true);
            pos += 4;
            return true;
        } else if (jsonStr.compare(pos, 5, "false") == 0) {
            outValue = JsonValue(false);
            pos += 5;
            return true;
        }
        return false;
    }
    
    static bool parseNull(const std::string& jsonStr, size_t& pos, JsonValue& outValue) {
        if (jsonStr.compare(pos, 4, "null") == 0) {
            outValue = JsonValue();
            pos += 4;
            return true;
        }
        return false;
    }
    
    static bool parseNumber(const std::string& jsonStr, size_t& pos, JsonValue& outValue) {
        size_t start = pos;
        bool hasDecimal = false;
        bool hasExp = false;
        
        if (jsonStr[pos] == '-') {
            pos++;
        }
        
        while (pos < jsonStr.size() && 
               (std::isdigit(jsonStr[pos]) || 
                (!hasDecimal && jsonStr[pos] == '.') ||
                (!hasExp && (jsonStr[pos] == 'e' || jsonStr[pos] == 'E')))) {
            if (jsonStr[pos] == '.') {
                hasDecimal = true;
            } else if (jsonStr[pos] == 'e' || jsonStr[pos] == 'E') {
                hasExp = true;
            }
            pos++;
        }
        
        if (pos == start || pos == start + 1) return false;
        
        try {
            double value = std::stod(jsonStr.substr(start, pos - start));
            outValue = JsonValue(value);
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    static void skipWhitespace(const std::string& jsonStr, size_t& pos) {
        while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
            pos++;
        }
    }
};

// Helper function to convert ConfigValue to JsonValue
JsonValue toJsonValue(const ConfigValue& cv) {
    return std::visit([](auto&& arg) -> JsonValue {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || 
                     std::is_same_v<T, float> || std::is_same_v<T, double>) {
            return JsonValue(static_cast<double>(arg));
        } else if constexpr (std::is_same_v<T, bool>) {
            return JsonValue(arg);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return JsonValue(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int> || 
                              std::is_same_v<T, std::vector<double> || 
                              std::is_same_v<T, std::vector<std::string>>) {
            // For arrays, return as is
            return std::visit([](auto&& arr) -> JsonValue {
                using ArrT = std::decay_t<decltype(arr)>;
                if constexpr (std::is_same_v<ArrT, std::vector<int>>) {
                    std::vector<JsonValue> jsonArr;
                    for (int val : arr) jsonArr.push_back(JsonValue(val));
                    return JsonValue(jsonArr);
                } else if constexpr (std::is_same_v<ArrT, std::vector<double>>) {
                    std::vector<JsonValue> jsonArr;
                    for (double val : arr) jsonArr.push_back(JsonValue(val));
                    return JsonValue(jsonArr);
                } else if constexpr (std::is_same_v<ArrT, std::vector<std::string>>) {
                    std::vector<JsonValue> jsonArr;
                    for (const std::string& val : arr) jsonArr.push_back(JsonValue(val));
                    return JsonValue(jsonArr);
                } else {
                    return JsonValue();
                }
            }, arg);
        } else {
            return JsonValue();
        }
    }, cv);
}

// Helper function to convert JsonValue to ConfigValue
ConfigValue toConfigValue(const JsonValue& jsonVal) {
    switch (jsonVal.type) {
        case JsonValue::STRING:
            return jsonVal.stringValue;
        case JsonValue::NUMBER:
            return jsonVal.numberValue;
        case JsonValue::BOOL:
            return jsonVal.boolValue;
        case JsonValue::OBJECT:
        case JsonValue::ARRAY:
        case JsonValue::NULL_VALUE:
        default:
            return std::string();
    }
}

} // namespace

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string lastLoadError;
    bool validateRequiredFields;
    
    Impl() : validateRequiredFields(true) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    pImpl->lastLoadError.clear();
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastLoadError = "Cannot open config file: " + filepath;
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try JSON/YAML parsing first
    JsonValue jsonRoot;
    bool parsedAsJson = false;
    
    // Try JSON parsing
    if (content.find('{') != std::string::npos || 
        content.find('[') != std::string::npos) {
        parsedAsJson = SimpleJsonParser::parse(content, jsonRoot);
    }
    
    if (parsedAsJson) {
        // Convert JSON to config entries
        pImpl->entries.clear();
        if (!convertJsonToConfig(jsonRoot)) {
            pImpl->lastLoadError = "Failed to parse JSON configuration";
            return false;
        }
        return true;
    }
    
    // Fall back to simple key=value format
    pImpl->entries.clear();
    std::string line;
    int lineNum = 0;
    
    while (std::getline(file, line)) {
        lineNum++;
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
            
            // Validate key format (alphanumeric and underscores)
            if (!isValidKey(key)) {
                pImpl->lastLoadError = "Invalid key at line " + std::to_string(lineNum) + ": " + key;
                return false;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else {
            // Line doesn't contain '=' - could be a comment or invalid format
            if (!line.empty() && line[0] != '#' && line[0] != '/') {
                pImpl->lastLoadError = "Invalid format at line " + std::to_string(lineNum) + ": " + line;
                return false;
            }
        }
    }
    
    // Validate required fields if enabled
    if (pImpl->validateRequiredFields) {
        std::vector<std::string> missingRequired = getMissingRequiredFields();
        if (!missingRequired.empty()) {
            pImpl->lastLoadError = "Missing required configuration fields: ";
            for (size_t i = 0; i < missingRequired.size(); ++i) {
                if (i > 0) pImpl->lastLoadError += ", ";
                pImpl->lastLoadError += missingRequired[i];
            }
            return false;
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
    
    // Save in JSON format for better readability
    file << "{" << std::endl;
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int> || 
                                std::is_same_v<T, int64_t> || 
                                std::is_same_v<T, float>) {
                file << static_cast<int>(arg);
            } else {
                file << "null";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << "," << std::endl;
        } else {
            file << std::endl;
        }
    }
    file << "}" << std::endl;
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key and value
    if (!isValidKey(key)) {
        throw std::invalid_argument("Invalid configuration key: " + key);
    }
    
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

// Helper methods
bool Config::isValidKey(const std::string& key) {
    if (key.empty()) return false;
    for (char ch : key) {
        if (!std::isalnum(ch) && ch != '_' && ch != '-') {
            return false;
        }
    }
    return true;
}

std::vector<std::string> Config::getMissingRequiredFields() const {
    std::vector<std::string> missing;
    
    // Check for essential fields
    std::vector<std::string> essentialFields = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count"
    };
    
    for (const auto& field : essentialFields) {
        if (!has(field)) {
            missing.push_back(field);
        }
    }
    
    return missing;
}

bool Config::convertJsonToConfig(const JsonValue& jsonVal) {
    if (jsonVal.type != JsonValue::OBJECT) {
        return false;
    }
    
    pImpl->entries.clear();
    
    for (const auto& pair : jsonVal.objectValue) {
        const std::string& key = pair.first;
        const JsonValue& jsonVal = pair.second;
        
        ConfigValue configVal = toConfigValue(jsonVal);
        pImpl->entries.emplace_back(key, configVal, ConfigSource::File);
    }
    
    return true;
}

std::string Config::getLastLoadError() const {
    return pImpl->lastLoadError;
}

void Config::setValidationRequired(bool required) {
    pImpl->validateRequiredFields = required;
}

bool Config::isValidationRequired() const {
    return pImpl->validateRequiredFields;
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
