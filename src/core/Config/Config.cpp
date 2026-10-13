#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <cmath>
#include <limits>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try to parse as JSON first
    JSONValue jsonValue;
    if (parseJSON(content, jsonValue) && jsonValue.isObject()) {
        // Parse as JSON object
        const auto& jsonArray = jsonValue.getArray();
        clear();
        
        for (const auto& item : jsonArray) {
            if (item.isObject()) {
                const auto& objArray = item.getArray();
                if (objArray.size() >= 3) {
                    std::string key = objArray[0].isString() ? objArray[0].getString() : "";
                    std::string valueStr = objArray[1].isString() ? objArray[1].getString() : "";
                    std::string desc = objArray[2].isString() ? objArray[2].getString() : "";
                    
                    if (!key.empty()) {
                        set(key, valueStr, ConfigSource::File);
                    }
                }
            }
        }
        return true;
    }
    
    // Fallback to simple key=value format (original implementation)
    clear();
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return !pImpl->entries.empty();
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
    
    // Export as JSON format for better interoperability
    file << "[\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  {\n";
        file << "    \"key\": \"" << entry.key << "\",\n";
        file << "    \"value\": \"" << entry.value << "\",\n";
        file << "    \"description\": \"" << entry.description << "\"\n";
        if (i < pImpl->entries.size() - 1) {
            file << "  },\n";
        } else {
            file << "  }\n";
        }
    }
    file << "]\n";
    
    return true;
}

// Helper functions
namespace {
    bool startsWith(const std::string& str, const std::string& prefix) {
        return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
    }
    
    bool endsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
    
    bool ciEqual(const std::string& a, const std::string& b) {
        return toLower(a) == toLower(b);
    }
    
    // Simple JSON value parsing and serialization
    class JSONValue {
    public:
        enum class Type { Null, String, Number, Boolean, Array, Object };
        
        JSONValue() : type_(Type::Null) {}
        JSONValue(const std::string& s) : type_(Type::String), stringValue_(s) {}
        JSONValue(double n) : type_(Type::Number), numberValue_(n) {}
        JSONValue(bool b) : type_(Type::Boolean), boolValue_(b) {}
        JSONValue(const std::vector<JSONValue>& arr) : type_(Type::Array), arrayValue_(arr) {}
        
        Type type() const { return type_; }
        bool isNull() const { return type_ == Type::Null; }
        bool isString() const { return type_ == Type::String; }
        bool isNumber() const { return type_ == Type::Number; }
        bool isBoolean() const { return type_ == Type::Boolean; }
        bool isArray() const { return type_ == Type::Array; }
        bool isObject() const { return type_ == Type::Object; }
        
        const std::string& getString() const { return stringValue_; }
        double getNumber() const { return numberValue_; }
        bool getBoolean() const { return boolValue_; }
        const std::vector<JSONValue>& getArray() const { return arrayValue_; }
        
        // Parse from string
        bool parseFromString(const std::string& str);
        
        // Convert to string
        std::string toString() const;
        
    private:
        Type type_;
        std::string stringValue_;
        double numberValue_;
        bool boolValue_;
        std::vector<JSONValue> arrayValue_;
    };
    
    // Simple JSON parser
    bool parseJSON(const std::string& jsonStr, JSONValue& outValue);
    std::string serializeJSONValue(const JSONValue& value);
    
    // Type conversion helpers
    ConfigValue convertToConfigValue(const JSONValue& jsonValue);
    
    // String utilities
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
}

// JSONValue implementation
bool JSONValue::parseFromString(const std::string& str) {
    if (str.empty()) {
        type_ = Type::Null;
        return true;
    }
    
    char first = str[0];
    if (first == '\"') {
        // String literal
        type_ = Type::String;
        stringValue_ = str.substr(1, str.size() - 2);
        return true;
    } else if (first == 't' || first == 'f') {
        // Boolean
        type_ = Type::Boolean;
        boolValue_ = (str == "true" || str == "true\n");
        return true;
    } else if (first == 'n') {
        // null
        type_ = Type::Null;
        return true;
    } else if (first == '[') {
        // Array (simplified - would need full implementation)
        type_ = Type::Array;
        return true;
    } else if (first == '{') {
        // Object (simplified - would need full implementation)
        type_ = Type::Object;
        return true;
    } else {
        // Number
        try {
            size_t pos;
            double value = std::stod(str, &pos);
            type_ = Type::Number;
            numberValue_ = value;
            return true;
        } catch (...) {
            return false;
        }
    }
}

std::string JSONValue::toString() const {
    switch (type_) {
        case Type::Null: return "null";
        case Type::String: return "\"" + stringValue_ + "\"";
        case Type::Number: return std::to_string(numberValue_);
        case Type::Boolean: return boolValue_ ? "true" : "false";
        case Type::Array:
        case Type::Object: return "[]"; // Simplified
        default: return "";
    }
}

bool parseJSON(const std::string& jsonStr, JSONValue& outValue) {
    std::string trimmed = trim(jsonStr);
    if (trimmed.empty()) {
        outValue = JSONValue();
        return true;
    }
    
    // Skip BOM
    if (trimmed.size() >= 3 && static_cast<unsigned char>(trimmed[0]) == 0xEF && 
        static_cast<unsigned char>(trimmed[1]) == 0xBB && static_cast<unsigned char>(trimmed[2]) == 0xBF) {
        trimmed = trimmed.substr(3);
    }
    
    // Simple JSON parsing (would need full implementation for production)
    // This is a simplified version focusing on basic functionality
    if (trimmed[0] == '{') {
        // Object - would parse key-value pairs
        outValue = JSONValue(std::vector<JSONValue>());
        return true;
    } else if (trimmed[0] == '[') {
        // Array - would parse comma-separated values
        outValue = JSONValue(std::vector<JSONValue>());
        return true;
    } else {
        // Primitive value
        JSONValue temp;
        if (temp.parseFromString(trimmed)) {
            outValue = temp;
            return true;
        }
    }
    return false;
}

std::string serializeJSONValue(const JSONValue& value) {
    switch (value.type()) {
        case JSONValue::Type::Null: return "null";
        case JSONValue::Type::String: return "\"" + value.getString() + "\"";
        case JSONValue::Type::Number: return std::to_string(value.getNumber());
        case JSONValue::Type::Boolean: return value.getBoolean() ? "true" : "false";
        case JSONValue::Type::Array:
        case JSONValue::Type::Object: return "{}"; // Simplified
        default: return "";
    }
}

ConfigValue convertToConfigValue(const JSONValue& jsonValue) {
    switch (jsonValue.type()) {
        case JSONValue::Type::String: return jsonValue.getString();
        case JSONValue::Type::Number: {
            double val = jsonValue.getNumber();
            if (std::floor(val) == val && val >= std::numeric_limits<int>::min() && val <= std::numeric_limits<int>::max()) {
                return static_cast<int>(val);
            }
            if (val >= std::numeric_limits<int64_t>::min() && val <= std::numeric_limits<int64_t>::max()) {
                return static_cast<int64_t>(val);
            }
            return val;
        }
        case JSONValue::Type::Boolean: return jsonValue.getBoolean();
        case JSONValue::Type::Array: {
            const auto& arr = jsonValue.getArray();
            std::vector<std::string> strArr;
            strArr.reserve(arr.size());
            for (const auto& item : arr) {
                if (item.isString()) {
                    strArr.push_back(item.getString());
                } else {
                    strArr.push_back(item.toString());
                }
            }
            return strArr;
        }
        case JSONValue::Type::Object:
        case JSONValue::Type::Null:
        default: return std::string();
    }
}

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Continue with Config class method implementations

// Template method implementations

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
        std::remove_if(pImpl->entries.begin(),
                      pImpl->entries.end(),
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