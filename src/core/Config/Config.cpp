#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iostream>
#include <cctype>

namespace nlm {

// Simple JSON value structure
struct JsonValue {
    enum class Type { Null, String, Number, Boolean, Array, Object };
    
    Type type;
    std::string stringValue;
    double numberValue;
    bool boolValue;
    std::vector<JsonValue> arrayValue;
    std::unordered_map<std::string, JsonValue> objectValue;
    
    JsonValue() : type(Type::Null) {}
    JsonValue(const std::string& val) : type(Type::String), stringValue(val) {}
    JsonValue(double val) : type(Type::Number), numberValue(val) {}
    JsonValue(bool val) : type(Type::Boolean), boolValue(val) {}
};

// Simple JSON parser
class JsonParser {
public:
    static JsonValue parse(const std::string& jsonStr) {
        JsonValue result;
        size_t pos = 0;
        skipWhitespace(jsonStr, pos);
        parseValue(jsonStr, pos, result);
        return result;
    }
    
private:
    static void skipWhitespace(const std::string& str, size_t& pos) {
        while (pos < str.size() && std::isspace(str[pos])) {
            pos++;
        }
    }
    
    static void parseValue(const std::string& str, size_t& pos, JsonValue& result) {
        skipWhitespace(str, pos);
        
        if (pos >= str.size()) {
            return;
        }
        
        char ch = str[pos];
        
        if (ch == '{') {
            parseObject(str, pos, result);
        } else if (ch == '[') {
            parseArray(str, pos, result);
        } else if (ch == '"') {
            parseString(str, pos, result);
        } else if (ch == 't' || ch == 'f') {
            parseBoolean(str, pos, result);
        } else if (ch == 'n') {
            parseNull(str, pos, result);
        } else {
            parseNumber(str, pos, result);
        }
    }
    
    static void parseString(const std::string& str, size_t& pos, JsonValue& result) {
        size_t start = ++pos; // Skip opening quote
        while (pos < str.size() && str[pos] != '"') {
            if (str[pos] == '\\' && pos + 1 < str.size()) {
                pos++; // Skip escaped character
            }
            pos++;
        }
        if (pos < str.size()) {
            result.type = JsonValue::Type::String;
            result.stringValue = str.substr(start, pos - start);
            pos++; // Skip closing quote
        }
    }
    
    static void parseNumber(const std::string& str, size_t& pos, JsonValue& result) {
        size_t start = pos;
        bool isDecimal = false;
        while (pos < str.size() && (std::isdigit(str[pos]) || 
                                   str[pos] == '.' || 
                                   str[pos] == '-' || 
                                   str[pos] == 'e' || 
                                   str[pos] == 'E')) {
            if (str[pos] == '.') {
                if (isDecimal) break; // Multiple decimal points
                isDecimal = true;
            }
            pos++;
        }
        result.type = JsonValue::Type::Number;
        result.numberValue = std::stod(str.substr(start, pos - start));
    }
    
    static void parseBoolean(const std::string& str, size_t& pos, JsonValue& result) {
        if (str.substr(pos, 4) == "true") {
            result.type = JsonValue::Type::Boolean;
            result.boolValue = true;
            pos += 4;
        } else if (str.substr(pos, 5) == "false") {
            result.type = JsonValue::Type::Boolean;
            result.boolValue = false;
            pos += 5;
        }
    }
    
    static void parseNull(const std::string& str, size_t& pos, JsonValue& result) {
        if (str.substr(pos, 4) == "null") {
            result.type = JsonValue::Type::Null;
            pos += 4;
        }
    }
    
    static void parseArray(const std::string& str, size_t& pos, JsonValue& result) {
        pos++; // Skip '['
        result.type = JsonValue::Type::Array;
        
        skipWhitespace(str, pos);
        if (pos < str.size() && str[pos] == ']') {
            pos++; // Skip ']'
            return;
        }
        
        while (pos < str.size() && str[pos] != ']') {
            JsonValue element;
            parseValue(str, pos, element);
            result.arrayValue.push_back(element);
            
            skipWhitespace(str, pos);
            if (pos < str.size() && str[pos] == ',') {
                pos++; // Skip ','
                skipWhitespace(str, pos);
            }
        }
        
        if (pos < str.size() && str[pos] == ']') {
            pos++; // Skip ']'
        }
    }
    
    static void parseObject(const std::string& str, size_t& pos, JsonValue& result) {
        pos++; // Skip '{'
        result.type = JsonValue::Type::Object;
        
        skipWhitespace(str, pos);
        if (pos < str.size() && str[pos] == '}') {
            pos++; // Skip '}'
            return;
        }
        
        while (pos < str.size() && str[pos] != '}') {
            skipWhitespace(str, pos);
            
            // Parse key
            if (str[pos] != '"') {
                break; // Invalid JSON
            }
            std::string key;
            size_t keyStart = ++pos; // Skip opening quote
            while (pos < str.size() && str[pos] != '"') {
                key += str[pos++];
            }
            if (pos < str.size()) {
                pos++; // Skip closing quote
                
                skipWhitespace(str, pos);
                if (pos < str.size() && str[pos] == ':') {
                    pos++; // Skip ':'
                    skipWhitespace(str, pos);
                    
                    // Parse value
                    JsonValue value;
                    parseValue(str, pos, value);
                    result.objectValue[key] = value;
                    
                    skipWhitespace(str, pos);
                    if (pos < str.size() && str[pos] == ',') {
                        pos++; // Skip ','
                        skipWhitespace(str, pos);
                    }
                }
            }
        }
        
        if (pos < str.size() && str[pos] == '}') {
            pos++; // Skip '}'
        }
    }
};

// Configuration schema validation
class ConfigValidator {
public:
    static bool validate(const JsonValue& json, const std::string& configFile) {
        // Basic validation - can be extended for specific schema
        return true;
    }
    
    static bool validateValue(const std::string& key, const JsonValue& value) {
        // Validate individual configuration values
        // This can be extended based on configuration key
        return true;
    }
};

// Configuration documentation
static const std::unordered_map<std::string, std::string> configDocumentation = {
    {"neuron_count", "Number of neurons in the brain"},
    {"region_count", "Number of brain regions"},
    {"connection_probability", "Probability of connections between neurons (0.0-1.0)"},
    {"simulation_timestep", "Time step for simulation in seconds"},
    {"random_seed", "Random seed for reproducibility"},
    {"stdp_ltp_weight", "STDP long-term potentiation weight"},
    {"stdp_ltd_weight", "STDP long-term depression weight"},
    {"stdp_tau", "STDP time constant"},
    {"synaptogenesis_rate", "Rate of new synapse formation"},
    {"pruning_rate", "Rate of synapse elimination"},
    {"enable_checkpointing", "Enable checkpoint save/load functionality"},
    {"enable_replay", "Enable memory replay functionality"},
    {"enable_development", "Enable developmental plasticity"},
    {"checkpoint_path", "Path for checkpoint files"},
    {"log_level", "Log level: debug, info, warning, error, critical"},
    {"log_file", "File path for log output"},
    {"enable_colors", "Enable colored output"},
};

// Get configuration description
static std::string getConfigDescription(const std::string& key) {
    auto it = configDocumentation.find(key);
    if (it != configDocumentation.end()) {
        return it->second;
    }
    return "No documentation available for key: " + key;
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Updated loadFromFile with JSON support
    bool Config::loadFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
            return false;
        }
        
        // Read entire file
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Try to parse as JSON first
        bool isJson = false;
        if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
            try {
                JsonValue json = JsonParser::parse(content);
                if (json.type == JsonValue::Type::Object) {
                    // Parse JSON object
                    for (const auto& pair : json.objectValue) {
                        std::string key = pair.first;
                        const JsonValue& value = pair.second;
                        
                        // Convert JSON value to string
                        std::string strValue;
                        switch (value.type) {
                            case JsonValue::Type::String:
                                strValue = value.stringValue;
                                break;
                            case JsonValue::Type::Number:
                                strValue = std::to_string(value.numberValue);
                                break;
                            case JsonValue::Type::Boolean:
                                strValue = value.boolValue ? "true" : "false";
                                break;
                            case JsonValue::Type::Null:
                                strValue = "";
                                break;
                            default:
                                NLM_LOG_WARNING("Skipping unsupported JSON type for key: " + key);
                                continue;
                        }
                        
                        set(key, strValue, ConfigSource::File);
                    }
                    
                    isJson = true;
                    NLM_LOG_INFO("Loaded JSON configuration from: " + filepath);
                }
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to parse JSON configuration: " + std::string(e.what()));
                // Fall back to simple parsing
            }
        }
        
        // If not JSON or JSON parsing failed, try simple key=value format
        if (!isJson) {
            std::ifstream file2(filepath);
            if (!file2.is_open()) {
                return false;
            }
            
            std::string line;
            while (std::getline(file2, line)) {
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
                    
                    // Validate key (alphanumeric and underscores)
                    if (!std::regex_match(key, std::regex("^[a-zA-Z_][a-zA-Z0-9_]*$"))) {
                        NLM_LOG_WARNING("Invalid configuration key format: " + key);
                        continue;
                    }
                    
                    // Validate value format based on key
                    if (!validateConfigValue(key, value)) {
                        NLM_LOG_WARNING("Invalid configuration value for key " + key + ": " + value);
                        continue;
                    }
                    
                    set(key, value, ConfigSource::File);
                    
                    // Log configuration with documentation
                    std::string doc = getConfigDescription(key);
                    NLM_LOG_DEBUG("Config[" + key + "] = \"" + value + "\" - " + doc);
                }
            }
            
            NLM_LOG_INFO("Loaded configuration from (simple format): " + filepath);
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
