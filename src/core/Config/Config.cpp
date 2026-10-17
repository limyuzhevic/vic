#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), format_(ConfigFormat::Unknown) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    format_ = detectFormat(filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError(ConfigError::Type::FileNotFound, "Could not open file: " + filepath);
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    if (content.empty()) {
        return true; // Empty file is valid
    }
    
    try {
        return loadFromString(content, format_);
    } catch (const ConfigError& e) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigError(ConfigError::Type::ParseError, "Failed to parse file: " + filepath + ", error: " + std::string(e.what()));
    }
}

bool Config::loadFromString(const std::string& content, ConfigFormat format) {
    clear();
    format_ = format;
    
    switch (format_) {
        case ConfigFormat::JSON:
            return fromJsonString(content);
        case ConfigFormat::YAML:
            return fromYamlString(content);
        case ConfigFormat::SimpleKeyValue:
            return fromKeyValueString(content);
        case ConfigFormat::Unknown:
        default:
            throw ConfigError(ConfigError::Type::FormatError, "Unknown configuration format");
    }
}

bool Config::fromJsonString(const std::string& jsonStr) {
    json::JsonValue json = json::parse(jsonStr);
    fromJson(json);
    return true;
}

bool Config::fromYamlString(const std::string& yamlStr) {
    yaml::YamlNode yaml = yaml::parse(yamlStr);
    fromYaml(yaml);
    return true;
}

bool Config::fromKeyValueString(const std::string& content) {
    auto kvp = keyvalue::parse(content);
    for (const auto& pair : kvp) {
        // Try to infer type from string value
        setFromString(pair.first, pair.second, ConfigSource::File);
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
                setFromString(key, value, ConfigSource::CommandLine);
            } else {
                setFromString(arg.substr(2), "true", ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            setFromString(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath, ConfigFormat format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError(ConfigError::Type::IoError, "Could not open file for writing: " + filepath);
    }
    
    switch (format) {
        case ConfigFormat::JSON:
            file << toJsonString();
            break;
        case ConfigFormat::YAML:
            file << toYamlString();
            break;
        case ConfigFormat::SimpleKeyValue:
            file << keyvalue::toString(toKeyValueMap());
            break;
        default:
            file << toJsonString(); // Default to JSON
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

void Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Try to parse as different types in order of specificity
    if (valueStr == "true" || valueStr == "false") {
        bool val = (valueStr == "true");
        set(key, val, source);
    } else {
        // Try to parse as number
        try {
            // Try int64_t
            size_t pos;
            int64_t intVal = std::stoll(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, intVal, source);
                return;
            }
        } catch (...) {}
        
        try {
            // Try double
            size_t pos;
            double doubleVal = std::stod(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, doubleVal, source);
                return;
            }
        } catch (...) {}
        
        // Default to string
        set(key, valueStr, source);
    }
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
    format_ = ConfigFormat::Unknown;
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, format: ";
    
    switch (format_) {
        case ConfigFormat::JSON: oss << "JSON"; break;
        case ConfigFormat::YAML: oss << "YAML"; break;
        case ConfigFormat::SimpleKeyValue: oss << "Simple Key-Value"; break;
        default: oss << "Unknown"; break;
    }
    oss << "):\n";
    
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

void Config::fromJson(const json::JsonValue& json) {
    if (json.isObject()) {
        for (const auto& pair : json.getObject()) {
            setFromJsonValue(pair.first, pair.second);
        }
    } else if (json.isArray()) {
        // For arrays, use index as key
        for (size_t i = 0; i < json.getArray().size(); ++i) {
            setFromJsonValue(std::to_string(i), json.getArray()[i]);
        }
    }
}

void Config::fromYaml(const yaml::YamlNode& yaml) {
    if (yaml.isMapping()) {
        for (const auto& pair : yaml.getMapping()) {
            setFromYamlNode(pair.first, pair.second);
        }
    } else if (yaml.isSequence()) {
        // For sequences, use index as key
        for (size_t i = 0; i < yaml.getSequence().size(); ++i) {
            setFromYamlNode(std::to_string(i), yaml.getSequence()[i]);
        }
    }
}

void Config::setFromJsonValue(const std::string& key, const json::JsonValue& json) {
    if (json.isBoolean()) {
        set(key, json.getBool(), ConfigSource::File);
    } else if (json.isNumber()) {
        double val = json.getNumber();
        // Check if it's actually an integer
        if (val == static_cast<int64_t>(val)) {
            set(key, static_cast<int64_t>(val), ConfigSource::File);
        } else {
            set(key, val, ConfigSource::File);
        }
    } else if (json.isString()) {
        set(key, json.getString(), ConfigSource::File);
    }
}

void Config::setFromYamlNode(const std::string& key, const yaml::YamlNode& yaml) {
    if (yaml.isScalar()) {
        const std::string& value = yaml.getScalar();
        setFromString(key, value, ConfigSource::File);
    }
}

json::JsonValue Config::toJson() const {
    json::JsonObject jsonObj;
    
    for (const auto& entry : pImpl->entries) {
        jsonObj[entry.key] = toJsonValue(entry.value);
    }
    
    return json::JsonValue(jsonObj);
}

yaml::YamlNode Config::toYaml() const {
    yaml::YamlMapping yamlMap;
    
    for (const auto& entry : pImpl->entries) {
        yamlMap[entry.key] = toYamlValue(entry.value);
    }
    
    return yaml::YamlNode(yamlMap);
}

json::JsonValue Config::toJsonValue(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> json::JsonValue {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, bool>) {
            return json::JsonValue(arg);
        } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            return json::JsonValue(static_cast<double>(arg));
        } else if constexpr (std::is_same_v<T, double>) {
            return json::JsonValue(arg);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return json::JsonValue(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            json::JsonArray arr;
            for (int val : arg) {
                arr.push_back(json::JsonValue(static_cast<double>(val)));
            }
            return json::JsonValue(arr);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            json::JsonArray arr;
            for (double val : arg) {
                arr.push_back(json::JsonValue(val));
            }
            return json::JsonValue(arr);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            json::JsonArray arr;
            for (const std::string& val : arg) {
                arr.push_back(json::JsonValue(val));
            }
            return json::JsonValue(arr);
        }
        return json::JsonValue();
    }, value);
}

yaml::YamlNode Config::toYamlValue(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> yaml::YamlNode {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, bool>) {
            return yaml::YamlNode(arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
            return yaml::YamlNode(std::to_string(arg));
        } else if constexpr (std::is_same_v<T, double>) {
            return yaml::YamlNode(std::to_string(arg));
        } else if constexpr (std::is_same_v<T, std::string>) {
            return yaml::YamlNode(arg);
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            yaml::YamlSequence seq;
            for (int val : arg) {
                seq.push_back(yaml::YamlNode(std::to_string(val)));
            }
            return yaml::YamlNode(seq);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            yaml::YamlSequence seq;
            for (double val : arg) {
                seq.push_back(yaml::YamlNode(std::to_string(val)));
            }
            return yaml::YamlNode(seq);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            yaml::YamlSequence seq;
            for (const std::string& val : arg) {
                seq.push_back(yaml::YamlNode(val));
            }
            return yaml::YamlNode(seq);
        }
        return yaml::YamlNode("null");
    }, value);
}

void Config::setCriticalDefaults() {
    // Set default values for critical parameters if they don't exist
    if (!has("simulation_timestep")) {
        set("simulation_timestep", 0.001, ConfigSource::Default);
    }
    if (!has("neuron_count")) {
        set("neuron_count", 1000LL, ConfigSource::Default);
    }
    if (!has("region_count")) {
        set("region_count", 2LL, ConfigSource::Default);
    }
}

bool Config::validate() const {
    // Basic validation: check for required fields
    if (has("simulation_timestep")) {
        auto ts = get<double>("simulation_timestep");
        if (ts.has_value() && (ts.value() <= 0.0 || ts.value() > 1.0)) {
            return false;
        }
    }
    
    if (has("neuron_count")) {
        auto nc = get<int64_t>("neuron_count");
        if (nc.has_value() && (nc.value() <= 0 || nc.value() > 100000)) {
            return false;
        }
    }
    
    if (has("region_count")) {
        auto rc = get<int64_t>("region_count");
        if (rc.has_value() && (rc.value() <= 0 || rc.value() > 100)) {
            return false;
        }
    }
    
    return true;
}

std::string Config::toJsonString() const {
    return json::toString(toJson());
}

std::string Config::toYamlString() const {
    return yaml::toString(toYaml());
}

std::map<std::string, std::string> Config::toKeyValueMap() const {
    std::map<std::string, std::string> kvp;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&kvp, &entry](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, bool>) {
                kvp[entry.key] = arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                kvp[entry.key] = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                kvp[entry.key] = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, std::string>) {
                kvp[entry.key] = arg;
            }
        }, entry.value);
    }
    
    return kvp;
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

ConfigFormat Config::detectFormat(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string ext = path.extension().string();
    
    if (ext.empty()) {
        return ConfigFormat::SimpleKeyValue; // Default for files without extension
    }
    
    ext = toLower(ext);
    
    if (ext == ".json" || ext == ".cfg") {
        return ConfigFormat::JSON;
    } else if (ext == ".yaml" || ext == ".yml") {
        return ConfigFormat::YAML;
    }
    
    // Try to guess from file content if extension is unknown
    // For now, default to SimpleKeyValue for backward compatibility
    return ConfigFormat::SimpleKeyValue;
}

bool Config::isTextFile(const std::string& filepath) {
    // Simple heuristic: if file can be opened and doesn't contain null bytes
    std::ifstream file(filepath, std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    
    constexpr size_t CHUNK_SIZE = 4096;
    char buffer[CHUNK_SIZE];
    while (file.read(buffer, CHUNK_SIZE)) {
        for (size_t i = 0; i < CHUNK_SIZE; ++i) {
            if (buffer[i] == '\0') {
                return false;
            }
        }
    }
    
    // Check remainder
    if (file.gcount() > 0) {
        for (size_t i = 0; i < static_cast<size_t>(file.gcount()); ++i) {
            if (buffer[i] == '\0') {
                return false;
            }
        }
    }
    
    return true;
}

// JSON implementation
namespace json {
    bool isValidJSON(const std::string& str) {
        // Very simple JSON validation
        const std::string validChars = "\"{}:,.[]\\t\\n\\r \";0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_-+eE.";
        for (char c : str) {
            if (validChars.find(c) == std::string::npos) {
                return false;
            }
        }
        return true;
    }
    
    std::string escapeString(const std::string& str) {
        std::string result;
        for (char c : str) {
            switch (c) {
                case '\"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
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
    
    JsonValue parse(const std::string& jsonStr) {
        // Very simple JSON parser - only handles basic objects
        if (!isValidJSON(jsonStr)) {
            throw ConfigError(ConfigError::Type::ParseError, "Invalid JSON format");
        }
        
        JsonValue result;
        
        // Find first non-whitespace character
        size_t pos = 0;
        while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
            pos++;
        }
        
        if (pos >= jsonStr.size()) {
            return result; // Empty
        }
        
        if (jsonStr[pos] == '{') {
            // Parse object
            JsonObject obj;
            ++pos; // Skip '{'
            
            while (pos < jsonStr.size() && jsonStr[pos] != '}') {
                // Skip whitespace
                while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
                    pos++;
                }
                
                if (pos >= jsonStr.size()) {
                    throw ConfigError(ConfigError::Type::ParseError, "Unexpected end of JSON object");
                }
                
                if (jsonStr[pos] == '}') {
                    break;
                }
                
                // Parse key
                std::string key;
                while (pos < jsonStr.size() && jsonStr[pos] != '"') {
                    if (jsonStr[pos] == '\\') {
                        pos++; // Skip escape
                        if (pos < jsonStr.size()) {
                            key += jsonStr[pos];
                        }
                    } else {
                        key += jsonStr[pos];
                    }
                    pos++;
                }
                
                if (pos >= jsonStr.size() || jsonStr[pos] != '"') {
                    throw ConfigError(ConfigError::Type::ParseError, "Expected closing quote for key");
                }
                
                pos++; // Skip '"'
                
                // Skip whitespace
                while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
                    pos++;
                }
                
                if (pos >= jsonStr.size() || jsonStr[pos] != ':') {
                    throw ConfigError(ConfigError::Type::ParseError, "Expected ':' after key");
                }
                
                pos++; // Skip ':'
                
                // Skip whitespace
                while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
                    pos++;
                }
                
                // Parse value
                result = parseValue(jsonStr, pos);
                obj[key] = result;
                
                // Skip whitespace
                while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
                    pos++;
                }
                
                if (pos < jsonStr.size() && jsonStr[pos] == ',') {
                    pos++; // Skip ','
                }
            }
            
            if (pos >= jsonStr.size()) {
                throw ConfigError(ConfigError::Type::ParseError, "Unexpected end of JSON object");
            }
            
            return JsonValue(obj);
        } else {
            // Parse simple value
            return parseValue(jsonStr, pos);
        }
    }
    
    JsonValue parseValue(const std::string& jsonStr, size_t& pos) {
        // Skip whitespace
        while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
            pos++;
        }
        
        if (pos >= jsonStr.size()) {
            throw ConfigError(ConfigError::Type::ParseError, "Unexpected end while parsing JSON value");
        }
        
        char c = jsonStr[pos];
        
        if (c == 'n' || c == 'N') { // null
            if (pos + 3 < jsonStr.size() && jsonStr.substr(pos, 4) == "null") {
                pos += 4;
                return JsonValue();
            }
        } else if (c == 't' || c == 'T') { // true
            if (pos + 3 < jsonStr.size() && jsonStr.substr(pos, 4) == "true") {
                pos += 4;
                return JsonValue(true);
            }
        } else if (c == 'f' || c == 'F') { // false
            if (pos + 4 < jsonStr.size() && jsonStr.substr(pos, 5) == "false") {
                pos += 5;
                return JsonValue(false);
            }
        } else if (c == '"') { // string
            pos++; // Skip '"'
            std::string value;
            while (pos < jsonStr.size() && jsonStr[pos] != '"') {
                if (jsonStr[pos] == '\\') {
                    pos++; // Skip escape
                    if (pos < jsonStr.size()) {
                        value += jsonStr[pos];
                    }
                } else {
                    value += jsonStr[pos];
                }
                pos++;
            }
            if (pos >= jsonStr.size()) {
                throw ConfigError(ConfigError::Type::ParseError, "Unterminated string in JSON");
            }
            pos++; // Skip '"'
            return JsonValue(value);
        } else if (c == '[') { // array
            pos++; // Skip '['
            JsonArray array;
            
            while (pos < jsonStr.size() && jsonStr[pos] != ']') {
                // Skip whitespace
                while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
                    pos++;
                }
                
                if (pos >= jsonStr.size()) {
                    throw ConfigError(ConfigError::Type::ParseError, "Unexpected end of JSON array");
                }
                
                JsonValue value = parseValue(jsonStr, pos);
                array.push_back(value);
                
                // Skip whitespace
                while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
                    pos++;
                }
                
                if (pos < jsonStr.size() && jsonStr[pos] == ',') {
                    pos++; // Skip ','
                }
            }
            
            if (pos >= jsonStr.size()) {
                throw ConfigError(ConfigError::Type::ParseError, "Unexpected end of JSON array");
            }
            
            pos++; // Skip ']'
            return JsonValue(array);
        } else { // number
            size_t start = pos;
            bool isDouble = false;
            
            while (pos < jsonStr.size()) {
                char c = jsonStr[pos];
                if (std::isdigit(c) || c == '.' || c == '-' || c == '+' || c == 'e' || c == 'E') {
                    if (c == '.' || c == 'e' || c == 'E') {
                        isDouble = true;
                    }
                    pos++;
                } else {
                    break;
                }
            }
            
            if (start == pos) {
                throw ConfigError(ConfigError::Type::ParseError, "Expected number in JSON");
            }
            
            std::string numStr = jsonStr.substr(start, pos - start);
            
            if (isDouble) {
                try {
                    double val = std::stod(numStr);
                    return JsonValue(val);
                } catch (...) {
                    throw ConfigError(ConfigError::Type::ParseError, "Invalid number: " + numStr);
                }
            } else {
                try {
                    int64_t val = std::stoll(numStr);
                    return JsonValue(static_cast<double>(val)); // Store as double for consistency
                } catch (...) {
                    throw ConfigError(ConfigError::Type::ParseError, "Invalid number: " + numStr);
                }
            }
        }
    }
    
    std::string toString(const JsonValue& value, int indent) {
        std::string indentStr(indent, ' ');
        
        switch (value.type()) {
            case JsonValue::Type::Null:
                return "null";
            case JsonValue::Type::Boolean:
                return value.getBool() ? "true" : "false";
            case JsonValue::Type::Number:
                return std::to_string(value.getNumber());
            case JsonValue::Type::String:
                return "\"" + escapeString(value.getString()) + "\"";
            case JsonValue::Type::Object:
                return toString(value.getObject(), indent);
            case JsonValue::Type::Array:
                return toString(value.getArray(), indent);
        }
    }
    
    std::string toString(const JsonObject& obj, int indent) {
        std::string result = "{\n";
        indent += 2;
        std::string indentStr(indent, ' ');
        
        for (auto it = obj.begin(); it != obj.end(); ++it) {
            if (it != obj.begin()) {
                result += ",\n";
            }
            result += indentStr + "\"" + escapeString(it->first) + "\": " + toString(it->second, indent);
        }
        
        result += "\n" + std::string(indent - 2, ' ') + "}";
        return result;
    }
    
    std::string toString(const JsonArray& arr, int indent) {
        std::string result = "[\n";
        indent += 2;
        std::string indentStr(indent, ' ');
        
        for (size_t i = 0; i < arr.size(); ++i) {
            if (i > 0) {
                result += ",\n";
            }
            result += indentStr + toString(arr[i], indent);
        }
        
        result += "\n" + std::string(indent - 2, ' ') + "]";
        return result;
    }
}

// YAML implementation
namespace yaml {
    bool isValidYAML(const std::string& str) {
        // Very simple YAML validation
        const std::string validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-:. \t\n\r:[]{}#?";
        for (char c : str) {
            if (validChars.find(c) == std::string::npos) {
                return false;
            }
        }
        return true;
    }
    
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    YamlNode parse(const std::string& yamlStr) {
        if (!isValidYAML(yamlStr)) {
            throw ConfigError(ConfigError::Type::ParseError, "Invalid YAML format");
        }
        
        // Simple YAML parsing for key-value pairs
        YamlMapping mapping;
        
        std::istringstream stream(yamlStr);
        std::string line;
        
        while (std::getline(stream, line)) {
            line = trim(line);
            
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Parse key: value
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes from value
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == "'"))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                mapping[key] = YamlNode(value);
            }
        }
        
        return YamlNode(mapping);
    }
    
    std::string toString(const YamlNode& node, int indent) {
        std::string indentStr(indent, ' ');
        
        switch (node.type()) {
            case YamlNode::Type::Null:
                return "null";
            case YamlNode::Type::Scalar:
                return node.getScalar();
            case YamlNode::Type::Mapping:
                return toString(node.getMapping(), indent);
            case YamlNode::Type::Sequence:
                return toString(node.getSequence(), indent);
        }
    }
    
    std::string toString(const YamlMapping& mapping, int indent) {
        std::string result;
        
        for (const auto& pair : mapping) {
            result += indentStr + pair.first + ": " + toString(pair.second, indent + 2) + "\n";
        }
        
        return result;
    }
    
    std::string toString(const YamlSequence& sequence, int indent) {
        std::string result = "- ";
        
        for (const auto& item : sequence) {
            result += toString(item, indent + 2) + "\n";
        }
        
        return result;
    }
}

// Key-value parser implementation
namespace keyvalue {
    std::map<std::string, std::string> parse(const std::string& content) {
        std::map<std::string, std::string> kvp;
        
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            line = trim(line);
            
            // Skip comments and empty lines
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
                     (value.front() == '\'' && value.back() == "'"))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                kvp[key] = value;
            }
        }
        
        return kvp;
    }
    
    std::string toString(const std::map<std::string, std::string>& kvp) {
        std::string result;
        
        for (const auto& pair : kvp) {
            result += pair.first + " = " + pair.second + "\n";
        }
        
        return result;
    }
}
}} // namespace nlm

// Explicit template instantiations
template std::optional<int> nlm::Config::get<int>(const std::string&) const;
template std::optional<int64_t> nlm::Config::get<int64_t>(const std::string&) const;
template std::optional<double> nlm::Config::get<double>(const std::string&) const;
template std::optional<bool> nlm::Config::get<bool>(const std::string&) const;
template std::optional<std::string> nlm::Config::get<std::string>(const std::string&) const;

template int nlm::Config::getOr<int>(const std::string&, const int&) const;
template int64_t nlm::Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double nlm::Config::getOr<double>(const std::string&, const double&) const;
template bool nlm::Config::getOr<bool>(const std::string&, const bool&) const;
template std::string nlm::Config::getOr<std::string>(const std::string&, const std::string&) const;
