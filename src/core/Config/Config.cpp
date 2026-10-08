#include <sstream>
#include <iomanip>
#include <chrono>
#include <ctime>

namespace nlm {

std::string Config::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Simple JSON parser for configuration files
class JSONParser {
public:
    struct JSONValue {
        enum class Type { OBJECT, ARRAY, STRING, NUMBER, BOOLEAN, NULL_VALUE };
        
        Type type;
        std::string stringValue;
        double numberValue;
        bool boolValue;
        
        std::vector<std::pair<std::string, JSONValue>> objectValue;
        std::vector<JSONValue> arrayValue;
        
        JSONValue() : type(Type::NULL_VALUE) {}
        JSONValue(const std::string& s) : type(Type::STRING), stringValue(s) {}
        JSONValue(double n) : type(Type::NUMBER), numberValue(n) {}
        JSONValue(bool b) : type(Type::BOOLEAN), boolValue(b) {}
        JSONValue(const std::vector<std::pair<std::string, JSONValue>>& obj) 
            : type(Type::OBJECT), objectValue(obj) {}
        JSONValue(const std::vector<JSONValue>& arr) 
            : type(Type::ARRAY), arrayValue(arr) {}
    };
    
    static bool parseFile(const std::string& filepath, JSONValue& root) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        return parseString(buffer.str(), root);
    }
    
    static bool parseString(const std::string& json, JSONValue& root) {
        // Skip whitespace
        size_t pos = 0;
        skipWhitespace(json, pos);
        
        if (pos >= json.length()) {
            return false; // Empty JSON
        }
        
        if (json[pos] == '{') {
            return parseObject(json, pos, root);
        } else if (json[pos] == '[') {
            return parseArray(json, pos, root);
        } else {
            // Simple value (string, number, boolean, null)
            return parseValue(json, pos, root);
        }
    }
    
private:
    static void skipWhitespace(const std::string& json, size_t& pos) {
        while (pos < json.length() && std::isspace(json[pos])) {
            pos++;
        }
    }
    
    static bool parseObject(const std::string& json, size_t& pos, JSONValue& root) {
        std::vector<std::pair<std::string, JSONValue>> members;
        
        pos++; // Skip {
        skipWhitespace(json, pos);
        
        if (json[pos] == '}') {
            pos++; // Skip }
            root = JSONValue(members);
            return true;
        }
        
        while (pos < json.length()) {
            skipWhitespace(json, pos);
            
            // Parse key
            if (json[pos] != '"') {
                return false;
            }
            
            size_t keyStart = ++pos; // Skip opening quote
            while (pos < json.length() && json[pos] != '"') {
                if (json[pos] == '\\') {
                    pos++; // Skip escape character
                }
                pos++;
            }
            
            if (pos >= json.length()) {
                return false;
            }
            
            std::string key = json.substr(keyStart, pos - keyStart);
            pos++; // Skip closing quote
            skipWhitespace(json, pos);
            
            if (json[pos] != ':') {
                return false;
            }
            pos++; // Skip colon
            skipWhitespace(json, pos);
            
            // Parse value
            JSONValue value;
            if (!parseValue(json, pos, value)) {
                return false;
            }
            
            members.emplace_back(key, value);
            skipWhitespace(json, pos);
            
            if (json[pos] == '}') {
                pos++; // Skip }
                root = JSONValue(members);
                return true;
            } else if (json[pos] == ',') {
                pos++; // Skip comma
                continue;
            } else {
                return false;
            }
        }
        
        return false;
    }
    
    static bool parseArray(const std::string& json, size_t& pos, JSONValue& root) {
        std::vector<JSONValue> elements;
        
        pos++; // Skip [
        skipWhitespace(json, pos);
        
        if (json[pos] == ']') {
            pos++; // Skip ]
            root = JSONValue(elements);
            return true;
        }
        
        while (pos < json.length()) {
            JSONValue element;
            if (!parseValue(json, pos, element)) {
                return false;
            }
            
            elements.push_back(element);
            skipWhitespace(json, pos);
            
            if (json[pos] == ']') {
                pos++; // Skip ]
                root = JSONValue(elements);
                return true;
            } else if (json[pos] == ',') {
                pos++; // Skip comma
                skipWhitespace(json, pos);
                continue;
            } else {
                return false;
            }
        }
        
        return false;
    }
    
    static bool parseValue(const std::string& json, size_t& pos, JSONValue& root) {
        skipWhitespace(json, pos);
        
        if (pos >= json.length()) {
            return false;
        }
        
        switch (json[pos]) {
            case '"':
                return parseStringValue(json, pos, root);
            case 'n': // null
                if (json.substr(pos, 4) == "null") {
                    pos += 4;
                    root = JSONValue();
                    return true;
                }
                return false;
            case 't': // true
                if (json.substr(pos, 4) == "true") {
                    pos += 4;
                    root = JSONValue(true);
                    return true;
                }
                return false;
            case 'f': // false
                if (json.substr(pos, 5) == "false") {
                    pos += 5;
                    root = JSONValue(false);
                    return true;
                }
                return false;
            default:
                return parseNumberValue(json, pos, root);
        }
    }
    
    static bool parseStringValue(const std::string& json, size_t& pos, JSONValue& root) {
        size_t start = ++pos; // Skip opening quote
        while (pos < json.length() && json[pos] != '"') {
            if (json[pos] == '\\') {
                pos++; // Skip escape character
            }
            pos++;
        }
        
        if (pos >= json.length()) {
            return false;
        }
        
        std::string value = json.substr(start, pos - start);
        pos++; // Skip closing quote
        root = JSONValue(value);
        return true;
    }
    
    static bool parseNumberValue(const std::string& json, size_t& pos, JSONValue& root) {
        size_t start = pos;
        bool hasDecimal = false;
        bool hasExponent = false;
        
        while (pos < json.length()) {
            char c = json[pos];
            if (c >= '0' && c <= '9') {
                pos++;
            } else if (c == '.' && !hasDecimal) {
                hasDecimal = true;
                pos++;
            } else if ((c == 'e' || c == 'E') && !hasExponent) {
                hasExponent = true;
                pos++;
                if (pos < json.length() && (json[pos] == '+' || json[pos] == '-')) {
                    pos++;
                }
            } else {
                break;
            }
        }
        
        if (pos == start) {
            return false;
        }
        
        try {
            double value = std::stod(json.substr(start, pos - start));
            root = JSONValue(value);
            return true;
        } catch (...) {
            return false;
        }
    }
    
    // Helper function to convert JSONValue to ConfigValue
    static ConfigValue jsonValueToConfigValue(const JSONValue& jsonValue) {
        switch (jsonValue.type) {
            case JSONValue::Type::STRING:
                return ConfigValue(jsonValue.stringValue);
            case JSONValue::Type::NUMBER:
                // Try to preserve integer type if it's a whole number
                if (std::abs(jsonValue.numberValue - std::round(jsonValue.numberValue)) < 1e-10) {
                    if (jsonValue.numberValue >= std::numeric_limits<int64_t>::min() && 
                        jsonValue.numberValue <= std::numeric_limits<int64_t>::max()) {
                        return ConfigValue(static_cast<int64_t>(jsonValue.numberValue));
                    }
                }
                return ConfigValue(jsonValue.numberValue);
            case JSONValue::Type::BOOLEAN:
                return ConfigValue(jsonValue.boolValue);
            case JSONValue::Type::NULL_VALUE:
                // For null, use default empty string
                return ConfigValue(std::string());
            case JSONValue::Type::OBJECT:
            case JSONValue::Type::ARRAY:
                // For complex types, convert to string representation
                std::stringstream ss;
                if (jsonValue.type == JSONValue::Type::OBJECT) {
                    ss << "{";
                    for (size_t i = 0; i < jsonValue.objectValue.size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << "\"" << jsonValue.objectValue[i].first << "\": ";
                        // Recursively convert nested values
                        ss << jsonValueToConfigValue(jsonValue.objectValue[i].second);
                    }
                    ss << "}";
                } else {
                    ss << "[";
                    for (size_t i = 0; i < jsonValue.arrayValue.size(); ++i) {
                        if (i > 0) ss << ", ";
                        ss << jsonValueToConfigValue(jsonValue.arrayValue[i]);
                    }
                    ss << "]";
                }
                return ConfigValue(ss.str());
        }
        return ConfigValue(std::string());
    }
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // PHASE 2: Proper JSON/YAML parser implementation
    // Load configuration from JSON format with comprehensive error handling
    
    // Try JSON parser first
    JSONParser::JSONValue jsonRoot;
    try {
        if (!JSONParser::parseFile(filepath, jsonRoot)) {
            // If JSON parsing fails, fall back to simple key=value format
            NLM_LOG_WARNING("Failed to parse JSON configuration file, falling back to simple format: " + filepath);
            
            // Fallback to simple key=value format
            std::ifstream file(filepath, std::ios::in);
            if (!file.is_open()) {
                NLM_LOG_ERROR("Cannot open configuration file: " + filepath);
                return false;
            }
            
            std::string line;
            int lineNumber = 0;
            while (std::getline(file, line)) {
                lineNumber++;
                // Skip empty lines and comments
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                // Parse simple key=value pairs
                size_t pos = line.find('=');
                if (pos == std::string::npos) {
                    NLM_LOG_WARNING("Invalid configuration line in " + filepath + ": line " + std::to_string(lineNumber) + ": " + line);
                    continue; // Skip malformed lines
                }
                
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
            
            return true;
        }
        
        // Validate JSON structure
        if (jsonRoot.type != JSONParser::JSONValue::Type::OBJECT) {
            NLM_LOG_ERROR("Invalid JSON configuration file: root element must be an object: " + filepath);
            return false;
        }
        
        // Recursively parse JSON object into Config entries
        for (const auto& pair : jsonRoot.objectValue) {
            const std::string& key = pair.first;
            const JSONParser::JSONValue& jsonValue = pair.second;
            
            try {
                // Convert JSON value to ConfigValue
                ConfigValue configValue = JSONParser::jsonValueToConfigValue(jsonValue);
                
                // Extract description from key if possible (e.g., "key = description" format)
                std::string description = "";
                size_t equalsPos = key.find('=');
                if (equalsPos != std::string::npos && equalsPos > 0) {
                    description = key.substr(equalsPos + 1);
                    // Remove leading/trailing whitespace
                    description = trim(description);
                    // Use the part before = as the actual key
                    std::string actualKey = trim(key.substr(0, equalsPos));
                    set(actualKey, configValue, ConfigSource::File);
                    
                    // Store description if available
                    if (!description.empty()) {
                        // Find the entry and update its description
                        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                            [&actualKey](const ConfigEntry& e) { return e.key == actualKey; });
                        if (it != pImpl->entries.end()) {
                            it->description = description;
                        }
                    }
                } else {
                    set(key, configValue, ConfigSource::File);
                }
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Failed to parse JSON value for key '" + key + "': " + std::string(e.what()));
                // Skip invalid entries
                continue;
            }
        }
        
        NLM_LOG_INFO("Successfully loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const std::bad_alloc& e) {
        NLM_LOG_ERROR("Memory allocation error while loading configuration: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        for (int i = 1; i < argc; ++i) {
            if (i >= argc) break; // Safety check
            
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    NLM_LOG_WARNING("Command line argument --" + arg.substr(2) + " has no value");
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            } else {
                NLM_LOG_WARNING("Unknown command line argument: " + arg);
            }
        }
        return true;
    } catch (const std::bad_alloc& e) {
        NLM_LOG_ERROR("Memory allocation error while parsing command line arguments: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing command line arguments: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open configuration file for writing: " + filepath);
        return false;
    }
    
    try {
        file << "# NLM Configuration File\n";
        file << "# Saved at: " << getCurrentTime() << "\n\n";
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            
            // Serialize the actual value based on its type
            std::visit([&file, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << entry.key << " = \"" << arg << "\"\n";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << entry.key << " = " << (arg ? "true" : "false") << "\n";
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << entry.key << " = " << arg << "\n";
                } else if constexpr (std::is_same_v<T, double>) {
                    // Format double with appropriate precision
                    std::ostringstream oss;
                    oss.precision(6);
                    oss << std::fixed << arg;
                    file << entry.key << " = " << oss.str() << "\n";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << arg[i];
                    }
                    file << "]\n";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        std::ostringstream oss;
                        oss.precision(6);
                        oss << std::fixed << arg[i];
                        file << oss.str();
                    }
                    file << "]\n";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << entry.key << " = [";
                    for (size_t i = 0; i < arg.size(); ++i) {
                        if (i > 0) file << ", ";
                        file << "\"" << arg[i] << "\"";
                    }
                    file << "]\n";
                } else {
                    // Fallback for unsupported types
                    file << entry.key << " = \"" << "PLACEHOLDER_VALUE" << "\"\n";
                }
            }, entry.value);
            file << "\n";
        }
        
        NLM_LOG_INFO("Successfully saved configuration to: " + filepath);
        return true;
        
    } catch (const std::bad_alloc& e) {
        NLM_LOG_ERROR("Memory allocation error while saving configuration: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to file " + filepath + ": " + std::string(e.what()));
        return false;
    }
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
