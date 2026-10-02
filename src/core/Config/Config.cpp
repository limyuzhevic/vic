#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <variant>

// Modern configuration format support
namespace json {
    // Simple JSON parser for basic types
    class JsonValue {
    public:
        enum class Type { Null, Boolean, Number, String, Object, Array };
        
        JsonValue() : type_(Type::Null) {}
        JsonValue(bool b) : type_(Type::Boolean), boolValue_(b) {}
        JsonValue(double n) : type_(Type::Number), numberValue_(n) {}
        JsonValue(const std::string& s) : type_(Type::String), stringValue_(s) {}
        JsonValue(const std::unordered_map<std::string, JsonValue>& o) 
            : type_(Type::Object), objectValue_(o) {}
        JsonValue(const std::vector<JsonValue>& a) 
            : type_(Type::Array), arrayValue_(a) {}
        
        Type getType() const { return type_; }
        
        bool getBool() const { return boolValue_; }
        double getNumber() const { return numberValue_; }
        const std::string& getString() const { return stringValue_; }
        const std::unordered_map<std::string, JsonValue>& getObject() const { return objectValue_; }
        const std::vector<JsonValue>& getArray() const { return arrayValue_; }
        
        static JsonValue parse(const std::string& jsonStr);
        static std::string toString() const;
        
    private:
        Type type_;
        union {
            bool boolValue_;
            double numberValue_;
            std::string stringValue_;
            std::unordered_map<std::string, JsonValue> objectValue_;
            std::vector<JsonValue> arrayValue_;
        };
    };
}

namespace yaml {
    // Simple YAML parser for basic types
    class YamlValue {
    public:
        enum class Type { Null, Boolean, Number, String, Object, Array };
        
        YamlValue() : type_(Type::Null) {}
        YamlValue(bool b) : type_(Type::Boolean), boolValue_(b) {}
        YamlValue(double n) : type_(Type::Number), numberValue_(n) {}
        YamlValue(const std::string& s) : type_(Type::String), stringValue_(s) {}
        YamlValue(const std::unordered_map<std::string, YamlValue>& o) 
            : type_(Type::Object), objectValue_(o) {}
        YamlValue(const std::vector<YamlValue>& a) 
            : type_(Type::Array), arrayValue_(a) {}
        
        Type getType() const { return type_; }
        
        bool getBool() const { return boolValue_; }
        double getNumber() const { return numberValue_; }
        const std::string& getString() const { return stringValue_; }
        const std::unordered_map<std::string, YamlValue>& getObject() const { return objectValue_; }
        const std::vector<YamlValue>& getArray() const { return arrayValue_; }
        
        static YamlValue parse(const std::string& yamlStr);
        static std::string toString() const;
        
    private:
        Type type_;
        union {
            bool boolValue_;
            double numberValue_;
            std::string stringValue_;
            std::unordered_map<std::string, YamlValue> objectValue_;
            std::vector<YamlValue> arrayValue_;
        };
    };
}

// Utility function to convert variant values to strings
inline std::string variantToString(const ConfigValue& value) {
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::to_string(arg);
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg ? "true" : "false";
        }
    }, value);
}

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Enhanced file loading with JSON/YAML support
// Error handling and validation utilities
inline std::string getErrorMessage(const std::string& operation, const std::string& context) {
    return "Configuration error: " + operation + (context.empty() ? "" : " (" + context + ")");
}

inline bool validateKey(const std::string& key) {
    if (key.empty()) {
        NLM_LOG_ERROR("Configuration key cannot be empty");
        return false;
    }
    if (key.find('=') != std::string::npos) {
        NLM_LOG_ERROR("Configuration key cannot contain '=' character: " + key);
        return false;
    }
    if (key.find(' ') != std::string::npos) {
        NLM_LOG_ERROR("Configuration key cannot contain spaces: " + key);
        return false;
    }
    return true;
}

inline bool validateValue(const std::string& value, ConfigValueType type) {
    try {
        switch (type) {
            case ConfigValueType::String:
                // Any string is valid
                break;
            case ConfigValueType::Integer:
                std::stoll(value);
                break;
            case ConfigValueType::Float:
                std::stod(value);
                break;
            case ConfigValueType::Boolean:
                if (value != "true" && value != "false") {
                    NLM_LOG_ERROR("Invalid boolean value: " + value);
                    return false;
                }
                break;
            case ConfigValueType::Array:
                // JSON/YAML array validation would go here
                break;
            case ConfigValueType::Object:
                // JSON/YAML object validation would go here
                break;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Invalid value for type " + std::to_string(static_cast<int>(type)) + ": " + value);
        return false;
    }
    return true;
}

inline bool isSafePath(const std::string& path) {
    // Basic security check for path traversal
    if (path.find("..") != std::string::npos) {
        NLM_LOG_ERROR("Potentially unsafe path detected: " + path);
        return false;
    }
    if (path.empty()) {
        NLM_LOG_ERROR("Empty path not allowed");
        return false;
    }
    return true;
}

// Enhanced error handling in Config methods
bool Config::loadFromFile(const std::string& filepath) {
    if (!isSafePath(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open configuration file: " + filepath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Check file size to prevent loading extremely large files
    if (content.size() > 10 * 1024 * 1024) { // 10MB limit
        NLM_LOG_ERROR("Configuration file too large: " + filepath);
        return false;
    }
    
    // Detect format based on file extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    if (ext == ".json") {
        success = loadFromJson(content);
    } else if (ext == ".yaml" || ext == ".yml") {
        success = loadFromYaml(content);
    } else {
        // Default to simple key=value format for backward compatibility
        success = loadFromSimple(content);
    }
    
    if (success) {
        NLM_LOG_INFO("Loaded configuration from: " + filepath + " (format: " + 
                    (ext == ".json" ? "JSON" : (ext == ".yaml" || ext == ".yml" ? "YAML" : "simple")) + ")");
    } else {
        NLM_LOG_ERROR("Failed to load configuration from: " + filepath);
    }
    
    return success;
}

bool Config::loadFromJson(const std::string& jsonContent) {
    try {
        auto jsonValue = json::JsonValue::parse(jsonContent);
        if (jsonValue.getType() == json::JsonValue::Type::Object) {
            const auto& obj = jsonValue.getObject();
            for (const auto& pair : obj) {
                std::string key = pair.first;
                const json::JsonValue& value = pair.second;
                
                // Validate key
                if (!validateKey(key)) {
                    continue;
                }
                
                ConfigValue configValue;
                if (value.getType() == json::JsonValue::Type::Boolean) {
                    configValue = value.getBool();
                } else if (value.getType() == json::JsonValue::Type::Number) {
                    double num = value.getNumber();
                    // Determine if it's an integer or double
                    if (num == static_cast<int64_t>(num)) {
                        configValue = static_cast<int64_t>(num);
                    } else {
                        configValue = num;
                    }
                } else if (value.getType() == json::JsonValue::Type::String) {
                    configValue = value.getString();
                } else if (value.getType() == json::JsonValue::Type::Null) {
                    // Skip null values
                    continue;
                } else {
                    NLM_LOG_WARNING("Skipping unsupported JSON value type for key: " + key);
                    continue;
                }
                
                set(key, configValue, ConfigSource::File);
            }
            return true;
        } else {
            NLM_LOG_ERROR("JSON root element must be an object");
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
    }
    return false;
}

bool Config::loadFromYaml(const std::string& yamlContent) {
    try {
        auto yamlValue = yaml::YamlValue::parse(yamlContent);
        if (yamlValue.getType() == yaml::YamlValue::Type::Object) {
            const auto& obj = yamlValue.getObject();
            for (const auto& pair : obj) {
                std::string key = pair.first;
                const yaml::YamlValue& value = pair.second;
                
                // Validate key
                if (!validateKey(key)) {
                    continue;
                }
                
                ConfigValue configValue;
                if (value.getType() == yaml::YamlValue::Type::Boolean) {
                    configValue = value.getBool();
                } else if (value.getType() == yaml::YamlValue::Type::Number) {
                    double num = value.getNumber();
                    if (num == static_cast<int64_t>(num)) {
                        configValue = static_cast<int64_t>(num);
                    } else {
                        configValue = num;
                    }
                } else if (value.getType() == yaml::YamlValue::Type::String) {
                    configValue = value.getString();
                } else if (value.getType() == yaml::YamlValue::Type::Null) {
                    // Skip null values
                    continue;
                } else {
                    NLM_LOG_WARNING("Skipping unsupported YAML value type for key: " + key);
                    continue;
                }
                
                set(key, configValue, ConfigSource::File);
            }
            return true;
        } else {
            NLM_LOG_ERROR("YAML root element must be an object");
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("YAML parsing error: " + std::string(e.what()));
    }
    return false;
}

bool Config::loadFromSimple(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    bool success = false;
    
    int lineNumber = 0;
    while (std::getline(stream, line)) {
        lineNumber++;
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
            
            // Validate key
            if (!validateKey(key)) {
                NLM_LOG_ERROR("Invalid key at line " + std::to_string(lineNumber) + ": " + key);
                continue;
            }
            
            // Try to parse as different types
            bool successConverted = false;
            
            // Try bool
            if (value == "true" || value == "false") {
                set(key, value == "true", ConfigSource::File);
                successConverted = true;
            }
            // Try integer
            else {
                try {
                    size_t posEnd;
                    int64_t intVal = std::stoll(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(key, intVal, ConfigSource::File);
                        successConverted = true;
                    }
                } catch (...) {}
            }
            
            // Try double
            if (!successConverted) {
                try {
                    size_t posEnd;
                    double doubleVal = std::stod(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(key, doubleVal, ConfigSource::File);
                        successConverted = true;
                    }
                } catch (...) {}
            }
            
            // Fall back to string
            if (!successConverted) {
                set(key, value, ConfigSource::File);
            }
            
            success = true;
        } else {
            NLM_LOG_WARNING("Skipping invalid line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    if (!isSafePath(filepath)) {
        return false;
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot write configuration file: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated by NLM Configuration System\n";
    file << "# Format: JSON for structured data, simple key=value for compatibility\n\n";
    
    // Write as JSON for structured data
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    file << "}\n";
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

bool Config::has(const std::string& key) const {
    if (!validateKey(key)) {
        return false;
    }
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    if (!validateKey(key)) {
        return;
    }
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
    NLM_LOG_INFO("Configuration cleared");
    pImpl->entries.clear();
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
