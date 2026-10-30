#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cmath>
#include <unordered_map>
#include <iomanip>

namespace nlm {

struct JsonValue;

class JsonParser {
public:
    static JsonValue parse(const std::string& json);
    static bool parseBool(const std::string& str);
    static int parseInt(const std::string& str);
    static int64_t parseInt64(const std::string& str);
    static double parseDouble(const std::string& str);
    static std::string parseString(const std::string& str);
    static bool isWhitespace(char c);
    static size_t skipWhitespace(const std::string& str, size_t pos);
    static size_t findNextToken(const std::string& str, size_t pos);
    static std::string extractToken(const std::string& str, size_t& pos);
};

struct JsonValue {
    enum class Type {
        Null,
        Bool,
        Number,
        String,
        Array,
        Object
    };
    
    Type type;
    
    union Value {
        bool boolVal;
        double numberVal;
        std::string stringVal;
        std::vector<JsonValue> arrayVal;
        std::vector<std::pair<std::string, JsonValue>> objectVal;
        
        Value() : boolVal(false) {}
        ~Value() {}
    };
    
    Value value;
    
    JsonValue() : type(Type::Null) {}
    JsonValue(bool val) : type(Type::Bool) { value.boolVal = val; }
    JsonValue(double val) : type(Type::Number) { value.numberVal = val; }
    JsonValue(int64_t val) : type(Type::Number) { value.numberVal = static_cast<double>(val); }
    JsonValue(const std::string& val) : type(Type::String) { value.stringVal = val; }
    JsonValue(const char* val) : type(Type::String) { value.stringVal = val; }
    
    ~JsonValue() {
        if (type == Type::String) {
            value.stringVal.~basic_string();
        } else if (type == Type::Array) {
            value.arrayVal.~vector();
        } else if (type == Type::Object) {
            value.objectVal.~vector();
        }
    }
    
    bool isBool() const { return type == Type::Bool; }
    bool isNumber() const { return type == Type::Number; }
    bool isString() const { return type == Type::String; }
    bool isArray() const { return type == Type::Array; }
    bool isObject() const { return type == Type::Object; }
    bool isNull() const { return type == Type::Null; }
    
    bool getBool() const { return value.boolVal; }
    double getNumber() const { return value.numberVal; }
    int64_t getInt64() const { return static_cast<int64_t>(value.numberVal); }
    int getInt() const { return static_cast<int>(value.numberVal); }
    const std::string& getString() const { return value.stringVal; }
    const std::vector<JsonValue>& getArray() const { return value.arrayVal; }
    const std::vector<std::pair<std::string, JsonValue>>& getObject() const { return value.objectVal; }
};

// Implement JsonParser methods
bool JsonParser::isWhitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

size_t JsonParser::skipWhitespace(const std::string& str, size_t pos) {
    while (pos < str.size() && isWhitespace(str[pos])) {
        pos++;
    }
    return pos;
}

size_t JsonParser::findNextToken(const std::string& str, size_t pos) {
    while (pos < str.size() && !isWhitespace(str[pos]) && str[pos] != '{' && 
           str[pos] != '}' && str[pos] != '[' && str[pos] != ']' && 
           str[pos] != ':' && str[pos] != ',' && str[pos] != '"') {
        pos++;
    }
    return pos;
}

std::string JsonParser::extractToken(const std::string& str, size_t& pos) {
    pos = skipWhitespace(str, pos);
    if (pos >= str.size()) {
        return "";
    }
    
    char c = str[pos];
    if (c == '"') {
        pos++;
        size_t start = pos;
        while (pos < str.size() && str[pos] != '"') {
            if (str[pos] == '\\' && pos + 1 < str.size()) {
                pos += 2;  // Skip escaped character
            } else {
                pos++;
            }
        }
        std::string result = str.substr(start, pos - start);
        pos++; // Skip closing quote
        return result;
    } else if (c == '-' || (c >= '0' && c <= '9')) {
        size_t start = pos;
        if (c == '-') pos++;
        while (pos < str.size() && (str[pos] >= '0' && str[pos] <= '9')) pos++;
        if (pos < str.size() && str[pos] == '.') {
            pos++;
            while (pos < str.size() && (str[pos] >= '0' && str[pos] <= '9')) pos++;
        }
        if (pos < str.size() && (str[pos] == 'e' || str[pos] == 'E')) {
            pos++;
            if (pos < str.size() && (str[pos] == '+' || str[pos] == '-')) pos++;
            while (pos < str.size() && (str[pos] >= '0' && str[pos] <= '9')) pos++;
        }
        return str.substr(start, pos - start);
    } else if (c == 't' || c == 'f') {
        if (str.substr(pos, 4) == "true") {
            pos += 4;
            return "true";
        } else if (str.substr(pos, 5) == "false") {
            pos += 5;
            return "false";
        }
    } else if (c == 'n') {
        if (str.substr(pos, 4) == "null") {
            pos += 4;
            return "null";
        }
    }
    
    return "";
}

bool JsonParser::parseBool(const std::string& str) {
    if (str == "true") return true;
    if (str == "false") return false;
    throw std::runtime_error("Invalid boolean value: " + str);
}

int JsonParser::parseInt(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (const std::exception& e) {
        throw std::runtime_error("Invalid integer value: " + str);
    }
}

int64_t JsonParser::parseInt64(const std::string& str) {
    try {
        return std::stoll(str);
    } catch (const std::exception& e) {
        throw std::runtime_error("Invalid integer64 value: " + str);
    }
}

double JsonParser::parseDouble(const std::string& str) {
    try {
        return std::stod(str);
    } catch (const std::exception& e) {
        throw std::runtime_error("Invalid double value: " + str);
    }
}

std::string JsonParser::parseString(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && i + 1 < str.size()) {
            switch (str[i + 1]) {
                case '\\': result += '\\'; break;
                case '\"': result += '"'; break;
                case '/': result += '/'; break;
                case 'b': result += '\b'; break;
                case 'f': result += '\f'; break;
                case 'n': result += '\n'; break;
                case 'r': result += '\r'; break;
                case 't': result += '\t'; break;
                default: result += str[++i]; break;
            }
            i++;
        } else {
            result += str[i];
        }
    }
    return result;
}

JsonValue JsonParser::parse(const std::string& json) {
    size_t pos = 0;
    JsonValue result;
    
    pos = skipWhitespace(json, pos);
    if (pos >= json.size()) {
        throw std::runtime_error("Empty JSON document");
    }
    
    char c = json[pos];
    if (c == '{') {
        result.type = JsonValue::Type::Object;
        result.value.objectVal = std::vector<std::pair<std::string, JsonValue>>();
        pos++;
        
        pos = skipWhitespace(json, pos);
        if (json[pos] == '}') {
            pos++;
            return result;
        }
        
        while (true) {
            std::string key = extractToken(json, pos);
            if (key.empty()) {
                throw std::runtime_error("Expected string key in JSON object");
            }
            
            pos = skipWhitespace(json, pos);
            if (pos >= json.size() || json[pos] != ':') {
                throw std::runtime_error("Expected ':' after key in JSON object");
            }
            pos++;
            
            JsonValue value = parse(json.substr(pos));
            result.value.objectVal.emplace_back(key, value);
            
            pos = skipWhitespace(json, pos);
            if (pos >= json.size()) {
                throw std::runtime_error("Unexpected end of JSON object");
            }
            
            if (json[pos] == '}') {
                pos++;
                break;
            } else if (json[pos] == ',') {
                pos++;
                continue;
            } else {
                throw std::runtime_error("Expected ',' or '}' in JSON object");
            }
        }
    } else if (c == '[') {
        result.type = JsonValue::Type::Array;
        result.value.arrayVal = std::vector<JsonValue>();
        pos++;
        
        pos = skipWhitespace(json, pos);
        if (json[pos] == ']') {
            pos++;
            return result;
        }
        
        while (true) {
            JsonValue value = parse(json.substr(pos));
            result.value.arrayVal.push_back(value);
            
            pos = skipWhitespace(json, pos);
            if (pos >= json.size()) {
                throw std::runtime_error("Unexpected end of JSON array");
            }
            
            if (json[pos] == ']') {
                pos++;
                break;
            } else if (json[pos] == ',') {
                pos++;
                continue;
            } else {
                throw std::runtime_error("Expected ',' or ']' in JSON array");
            }
        }
    } else if (c == '"') {
        pos++;
        size_t start = pos;
        while (pos < json.size() && json[pos] != '"') {
            if (json[pos] == '\\' && pos + 1 < json.size()) {
                pos += 2;
            } else {
                pos++;
            }
        }
        result.type = JsonValue::Type::String;
        result.value.stringVal = parseString(json.substr(start, pos - start));
        if (pos < json.size()) {
            pos++;
        }
    } else if (c == 't' || c == 'f') {
        std::string token = extractToken(json, pos);
        result.type = JsonValue::Type::Bool;
        result.value.boolVal = parseBool(token);
    } else if (c == 'n') {
        std::string token = extractToken(json, pos);
        result.type = JsonValue::Type::Null;
    } else if (c == '-' || (c >= '0' && c <= '9')) {
        std::string token = extractToken(json, pos);
        result.type = JsonValue::Type::Number;
        try {
            if (token.find('.') != std::string::npos || token.find('e') != std::string::npos || 
                token.find('E') != std::string::npos) {
                result.value.numberVal = parseDouble(token);
            } else {
                result.value.numberVal = parseInt64(token);
            }
        } catch (const std::exception& e) {
            throw std::runtime_error("Invalid number value: " + token);
        }
    } else {
        throw std::runtime_error("Unexpected character in JSON: " + std::string(1, c));
    }
    
    return result;
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, size_t> keyIndex;
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
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string jsonContent = buffer.str();
    
    try {
        JsonValue root = JsonParser::parse(jsonContent);
        
        if (!root.isObject()) {
            throw std::runtime_error("JSON document root must be an object");
        }
        
        const auto& obj = root.getObject();
        
        for (const auto& pair : obj) {
            const std::string& key = pair.first;
            const JsonValue& value = pair.second;
            
            ConfigValue configValue;
            if (value.isBool()) {
                configValue = value.getBool();
            } else if (value.isNumber()) {
                double num = value.getNumber();
                if (std::floor(num) == num && num >= std::numeric_limits<int>::min() && num <= std::numeric_limits<int>::max()) {
                    configValue = static_cast<int>(num);
                } else if (num >= std::numeric_limits<int64_t>::min() && num <= std::numeric_limits<int64_t>::max()) {
                    configValue = static_cast<int64_t>(num);
                } else {
                    configValue = num;
                }
            } else if (value.isString()) {
                configValue = value.getString();
            } else if (value.isNull()) {
                configValue = std::string();
            } else {
                throw std::runtime_error("Unsupported JSON value type for key: " + key);
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
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
    
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << std::fixed << std::setprecision(6) << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            }
        }, entry.value);
    }
    
    file << "\n}\n";
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = pImpl->keyIndex.find(key);
    if (it == pImpl->keyIndex.end()) {
        return std::nullopt;
    }
    
    size_t index = it->second;
    if (index >= pImpl->entries.size()) {
        return std::nullopt;
    }
    
    const ConfigEntry& entry = pImpl->entries[index];
    
    try {
        return std::get<T>(entry.value);
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
    // Validate key
    if (key.empty() || key.find_first_of("{}[]\": ,#+") != std::string::npos) {
        throw std::invalid_argument("Invalid key name: " + key);
    }
    
    // Validate value based on type
    if (std::holds_alternative<std::string>(value)) {
        const std::string& str = std::get<std::string>(value);
        if (str.size() > 10000) {
            throw std::invalid_argument("String value too long: " + key);
        }
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        const auto& vec = std::get<std::vector<std::string>>(value);
        if (vec.size() > 1000) {
            throw std::invalid_argument("String array too large: " + key);
        }
    }
    
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        pImpl->entries[it->second].value = value;
        pImpl->entries[it->second].source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
        pImpl->keyIndex[key] = pImpl->entries.size() - 1;
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
    return pImpl->keyIndex.find(key) != pImpl->keyIndex.end();
}

void Config::remove(const std::string& key) {
    auto it = pImpl->keyIndex.find(key);
    if (it != pImpl->keyIndex.end()) {
        size_t index = it->second;
        pImpl->entries.erase(pImpl->entries.begin() + index);
        
        // Update keyIndex for all entries after the removed one
        for (auto& kv : pImpl->keyIndex) {
            if (kv.second > index) {
                kv.second--;
            }
        }
        
        pImpl->keyIndex.erase(it);
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
    pImpl->keyIndex.clear();
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
