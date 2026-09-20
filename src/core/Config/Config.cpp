#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <cctype>
#include <iomanip>
#include <map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

struct JsonValue {
    enum Type { NULL_TYPE, BOOL, INT, DOUBLE, STRING, ARRAY, OBJECT };
    Type type;
    
    // Primitive types
    bool boolValue;
    int64_t intValue;
    double doubleValue;
    std::string stringValue;
    
    // Complex types
    std::vector<JsonValue> arrayValue;
    std::map<std::string, JsonValue> objectValue;
    
    JsonValue() : type(NULL_TYPE) {}
    JsonValue(bool v) : type(BOOL), boolValue(v) {}
    JsonValue(int64_t v) : type(INT), intValue(v) {}
    JsonValue(double v) : type(DOUBLE), doubleValue(v) {}
    JsonValue(const std::string& v) : type(STRING), stringValue(v) {}
};

// Simple JSON parser (supports basic JSON)
JsonValue parseJson(const std::string& json);
std::string jsonToString(const JsonValue& value);
void flattenJson(const JsonValue& value, const std::string& prefix, std::map<std::string, std::string>& flatConfig);

JsonValue parseJson(const std::string& json) {
    JsonValue result;
    const char* data = json.c_str();
    size_t pos = 0;
    
    while (pos < json.length() && std::isspace(data[pos])) pos++;
    
    if (parseJsonValue(data, pos, result)) {
        return result;
    }
    return result;
}

bool parseJsonValue(const char* data, size_t& pos, JsonValue& out);
bool parseJsonString(const char* data, size_t& pos, std::string& out);
bool parseJsonNumber(const char* data, size_t& pos, double& out);
bool parseJsonBoolean(const char* data, size_t& pos, bool& out);
bool parseJsonArray(const char* data, size_t& pos, JsonValue& out);
bool parseJsonObject(const char* data, size_t& pos, JsonValue& out);

bool parseJsonValue(const char* data, size_t& pos, JsonValue& out) {
    if (pos >= strlen(data)) return false;
    
    switch (data[pos]) {
        case 'n': // null
            if (strncmp(data + pos, "null", 4) == 0) {
                pos += 4;
                return true;
            }
            return false;
        case 't': // true
            if (strncmp(data + pos, "true", 4) == 0) {
                out = JsonValue(true);
                pos += 4;
                return true;
            }
            return false;
        case 'f': // false
            if (strncmp(data + pos, "false", 5) == 0) {
                out = JsonValue(false);
                pos += 5;
                return true;
            }
            return false;
        case '\"': // string
            return parseJsonString(data, pos, out.stringValue);
        case '{': // object
            return parseJsonObject(data, pos, out);
        case '[': // array
            return parseJsonArray(data, pos, out);
        default: // number or -something
            if (isdigit(data[pos]) || data[pos] == '-' || data[pos] == '+') {
                double num;
                if (parseJsonNumber(data, pos, num)) {
                    out = JsonValue(num);
                    return true;
                }
                return false;
            }
    }
    return false;
}

bool parseJsonString(const char* data, size_t& pos, std::string& out) {
    if (data[pos] != '\"') return false;
    pos++;
    out.clear();
    
    while (pos < strlen(data) && data[pos] != '\"') {
        if (data[pos] == '\\') {
            pos++;
            if (pos >= strlen(data)) return false;
            switch (data[pos]) {
                case '\"': out += '\"'; break;
                case '\\': out += '\\'; break;
                case '/': out += '/'; break;
                case 'b': out += '\b'; break;
                case 'f': out += '\f'; break;
                case 'n': out += '\n'; break;
                case 'r': out += '\r'; break;
                case 't': out += '\t'; break;
                default: out += data[pos]; break;
            }
        } else {
            out += data[pos];
        }
        pos++;
    }
    
    if (data[pos] != '\"') return false;
    pos++;
    return true;
}

bool parseJsonNumber(const char* data, size_t& pos, double& out) {
    char* end;
    out = std::strtod(data + pos, &end);
    pos = end - data;
    return true;
}

bool parseJsonBoolean(const char* data, size_t& pos, bool& out) {
    if (strncmp(data + pos, "true", 4) == 0) {
        out = true;
        pos += 4;
        return true;
    } else if (strncmp(data + pos, "false", 5) == 0) {
        out = false;
        pos += 5;
        return true;
    }
    return false;
}

bool parseJsonArray(const char* data, size_t& pos, JsonValue& out) {
    if (data[pos] != '[') return false;
    pos++;
    
    while (pos < strlen(data) && data[pos] != ']') {
        skipWhitespace(data, pos);
        
        JsonValue element;
        if (!parseJsonValue(data, pos, element)) {
            return false;
        }
        out.arrayValue.push_back(element);
        
        skipWhitespace(data, pos);
        if (data[pos] == ',') {
            pos++;
            continue;
        } else if (data[pos] != ']') {
            return false;
        }
    }
    
    if (data[pos] != ']') return false;
    pos++;
    return true;
}

bool parseJsonObject(const char* data, size_t& pos, JsonValue& out) {
    if (data[pos] != '{') return false;
    pos++;
    
    while (pos < strlen(data) && data[pos] != '}') {
        skipWhitespace(data, pos);
        
        // Parse key
        std::string key;
        if (!parseJsonString(data, pos, key)) return false;
        
        skipWhitespace(data, pos);
        if (data[pos] != ':') return false;
        pos++;
        
        skipWhitespace(data, pos);
        
        // Parse value
        JsonValue value;
        if (!parseJsonValue(data, pos, value)) return false;
        out.objectValue[key] = value;
        
        skipWhitespace(data, pos);
        if (data[pos] == ',') {
            pos++;
            continue;
        } else if (data[pos] != '}') {
            return false;
        }
    }
    
    if (data[pos] != '}') return false;
    pos++;
    return true;
}

void skipWhitespace(const char* data, size_t& pos) {
    while (pos < strlen(data) && std::isspace(data[pos])) {
        pos++;
    }
}

std::string jsonToString(const JsonValue& value) {
    switch (value.type) {
        case JsonValue::NULL_TYPE:
            return "null";
        case JsonValue::BOOL:
            return value.boolValue ? "true" : "false";
        case JsonValue::INT:
            return std::to_string(value.intValue);
        case JsonValue::DOUBLE:
            return std::to_string(value.doubleValue);
        case JsonValue::STRING:
            return '"' + value.stringValue + '"';
        case JsonValue::ARRAY:
            return "[" + arrayToString(value.arrayValue) + "]";
        case JsonValue::OBJECT:
            return objectToString(value.objectValue);
        default:
            return "";
    }
}

std::string arrayToString(const std::vector<JsonValue>& array) {
    std::string result;
    for (size_t i = 0; i < array.size(); ++i) {
        if (i > 0) result += ", ";
        result += jsonToString(array[i]);
    }
    return result;
}

std::string objectToString(const std::map<std::string, JsonValue>& object) {
    std::string result;
    for (auto it = object.begin(); it != object.end(); ++it) {
        if (it != object.begin()) result += ", ";
        result += '"' + it->first + '"' + ": " + jsonToString(it->second);
    }
    return "{" + result + "}";
}

void flattenJson(const JsonValue& value, const std::string& prefix, std::map<std::string, std::string>& flatConfig) {
    std::string key = prefix;
    if (!key.empty() && key.back() != '.') key += '.';
    
    switch (value.type) {
        case JsonValue::NULL_TYPE:
            flatConfig[key.substr(0, key.length() - 1)] = "null";
            break;
        case JsonValue::BOOL:
            flatConfig[key.substr(0, key.length() - 1)] = value.boolValue ? "true" : "false";
            break;
        case JsonValue::INT:
            flatConfig[key.substr(0, key.length() - 1)] = std::to_string(value.intValue);
            break;
        case JsonValue::DOUBLE:
            flatConfig[key.substr(0, key.length() - 1)] = std::to_string(value.doubleValue);
            break;
        case JsonValue::STRING:
            flatConfig[key.substr(0, key.length() - 1)] = value.stringValue;
            break;
        case JsonValue::ARRAY:
            for (size_t i = 0; i < value.arrayValue.size(); ++i) {
                flattenJson(value.arrayValue[i], key + std::to_string(i) + ".", flatConfig);
            }
            break;
        case JsonValue::OBJECT:
            for (const auto& pair : value.objectValue) {
                flattenJson(pair.second, key + pair.first + ".", flatConfig);
            }
            break;
    }
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try to parse as JSON
    JsonValue jsonValue = parseJson(content);
    
    if (jsonValue.type != JsonValue::OBJECT) {
        // If not JSON, fall back to legacy key=value format
        file.clear();
        file.seekg(0);
        
        std::string line;
        while (std::getline(file, line)) {
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
                
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
    
    // Convert JSON to flat key=value format
    std::map<std::string, std::string> flatConfig;
    flattenJson(jsonValue, "", flatConfig);
    
    for (const auto& pair : flatConfig) {
        set(pair.first, pair.second, ConfigSource::File);
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
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
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
