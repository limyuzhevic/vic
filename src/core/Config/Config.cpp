#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <unordered_map>
#include <variant>
#include <string>
#include <vector>
#include <memory>

namespace nlm {

// JSON value types
struct JsonValue;
using JsonObject = std::unordered_map<std::string, std::unique_ptr<JsonValue>>;
using JsonArray = std::vector<std::unique_ptr<JsonValue>>;

struct JsonValue {
    enum class Type { NULL_VAL, BOOLEAN, NUMBER, STRING, OBJECT, ARRAY };
    
    Type type;
    
    // Union for different JSON value types
    bool boolean;
    double number;
    std::string string;
    JsonObject object;
    JsonArray array;
    
    JsonValue() : type(Type::NULL_VAL) {}
    JsonValue(bool b) : type(Type::BOOLEAN), boolean(b) {}
    JsonValue(double n) : numeric n) {}
    JsonValue(const std::string& s) : type(Type::STRING), string(s) {}
    JsonValue(const JsonObject& o) : type(Type::OBJECT), object(o) {}
    JsonValue(const JsonArray& a) : type(Type::ARRAY), array(a) {}
    
    // Helper methods
    bool isNull() const { return type == Type::NULL_VAL; }
    bool isBool() const { return type == Type::BOOLEAN; }
    bool isNumber() const { return type == Type::NUMBER; }
    bool isString() const { return type == Type::STRING; }
    bool isObject() const { return type == Type::OBJECT; }
    bool isArray() const { return type == Type::ARRAY; }
    
    // Convert to ConfigValue
    ConfigValue toConfigValue() const {
        if (isBool()) {
            return boolean;
        } else if (isNumber()) {
            // Check if it's an integer
            if (std::floor(number) == number && number >= std::numeric_limits<int64_t>::min() && 
                number <= std::numeric_limits<int64_t>::max()) {
                return static_cast<int64_t>(number);
            } else if (number >= std::numeric_limits<double>::min() && 
                       number <= std::numeric_limits<double>::max()) {
                return number;
            }
        } else if (isString()) {
            return string;
        }
        // Fallback to string representation for complex types
        return string;
    }
};

class JsonParser {
public:
    static std::unique_ptr<JsonValue> parse(const std::string& json) {
        std::unique_ptr<JsonValue> root = parseValue(json, 0);
        if (!root || root->type == JsonValue::Type::NULL_VAL) {
            return nullptr;
        }
        return root;
    }
    
    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\n\r");
        size_t end = str.find_last_not_of(" \t\n\r");
        if (start == std::string::npos || end == std::string::npos) {
            return "";
        }
        return str.substr(start, end - start + 1);
    }
    
    static std::string unescapeString(const std::string& str) {
        std::string result;
        result.reserve(str.size());
        for (size_t i = 1; i < str.size() - 1; ++i) {
            if (str[i] == '\\' && i + 1 < str.size()) {
                switch (str[i + 1]) {
                    case '\"': result.push_back('\"'); break;
                    case '\\': result.push_back('\\'); break;
                    case '/':  result.push_back('/'); break;
                    case 'b':  result.push_back('\b'); break;
                    case 'f':  result.push_back('\f'); break;
                    case 'n':  result.push_back('\n'); break;
                    case 'r':  result.push_back('\r'); break;
                    case 't':  result.push_back('\t'); break;
                    default:   result.push_back(str[i + 1]); break;
                }
                i++;
            } else {
                result.push_back(str[i]);
            }
        }
        return result;
    }
    
private:
    static std::unique_ptr<JsonValue> parseValue(const std::string& json, size_t& pos) {
        skipWhitespace(json, pos);
        if (pos >= json.size()) return nullptr;
        
        switch (json[pos]) {
            case 'n': return parseNull(json, pos);
            case 't': return parseBoolean(json, pos);
            case 'f': return parseBoolean(json, pos);
            case '\"': return parseString(json, pos);
            case '[': return parseArray(json, pos);
            case '{': return parseObject(json, pos);
            default: return parseNumber(json, pos);
        }
    }
    
    static std::unique_ptr<JsonValue> parseNull(const std::string& json, size_t& pos) {
        if (pos + 3 < json.size() && json.substr(pos, 4) == "null") {
            pos += 4;
            return std::make_unique<JsonValue>();
        }
        return nullptr;
    }
    
    static std::unique_ptr<JsonValue> parseBoolean(const std::string& json, size_t& pos) {
        if (pos + 3 < json.size() && json.substr(pos, 4) == "true") {
            pos += 4;
            return std::make_unique<JsonValue>(true);
        } else if (pos + 4 < json.size() && json.substr(pos, 5) == "false") {
            pos += 5;
            return std::make_unique<JsonValue>(false);
        }
        return nullptr;
    }
    
    static std::unique_ptr<JsonValue> parseString(const std::string& json, size_t& pos) {
        if (json[pos] != '\"') return nullptr;
        
        size_t start = ++pos;
        while (pos < json.size()) {
            if (json[pos] == '\"') {
                std::string str = json.substr(start, pos - start);
                pos++; // Skip closing quote
                return std::make_unique<JsonValue>(unescapeString(str));
            } else if (json[pos] == '\\' && pos + 1 < json.size()) {
                pos += 2;
            } else {
                pos++;
            }
        }
        return nullptr;
    }
    
    static std::unique_ptr<JsonValue> parseNumber(const std::string& json, size_t& pos) {
        size_t start = pos;
        bool isNegative = false;
        
        if (json[pos] == '-') {
            isNegative = true;
            pos++;
        }
        
        while (pos < json.size() && std::isdigit(json[pos])) {
            pos++;
        }
        
        if (pos == start || (isNegative && pos == start + 1)) {
            return nullptr;
        }
        
        std::string numStr = json.substr(start, pos - start);
        double value = std::stod(numStr);
        if (isNegative) {
            value = -value;
        }
        
        // Check for decimal
        if (pos < json.size() && json[pos] == '.') {
            pos++;
            while (pos < json.size() && std::isdigit(json[pos])) {
                pos++;
            }
        }
        
        // Check for exponent
        if (pos < json.size() && (json[pos] == 'e' || json[pos] == 'E')) {
            pos++;
            if (pos < json.size() && (json[pos] == '+' || json[pos] == '-')) {
                pos++;
            }
            while (pos < json.size() && std::isdigit(json[pos])) {
                pos++;
            }
        }
        
        return std::make_unique<JsonValue>(value);
    }
    
    static std::unique_ptr<JsonValue> parseArray(const std::string& json, size_t& pos) {
        if (json[pos] != '[') return nullptr;
        pos++; // Skip '['
        
        JsonArray array;
        skipWhitespace(json, pos);
        
        if (json[pos] == ']') {
            pos++; // Skip ']'
            return std::make_unique<JsonValue>(array);
        }
        
        while (true) {
            auto value = parseValue(json, pos);
            if (!value) break;
            
            array.push_back(std::move(value));
            
            skipWhitespace(json, pos);
            if (pos >= json.size()) break;
            
            if (json[pos] == ']') {
                pos++;
                break;
            } else if (json[pos] == ',') {
                pos++;
                skipWhitespace(json, pos);
            } else {
                break; // Invalid format
            }
        }
        
        return std::make_unique<JsonValue>(array);
    }
    
    static std::unique_ptr<JsonValue> parseObject(const std::string& json, size_t& pos) {
        if (json[pos] != '{') return nullptr;
        pos++; // Skip '{'
        
        JsonObject object;
        skipWhitespace(json, pos);
        
        if (json[pos] == '}') {
            pos++; // Skip '}'
            return std::make_unique<JsonValue>(object);
        }
        
        while (true) {
            skipWhitespace(json, pos);
            
            if (json[pos] != '\"') break;
            
            // Parse key
            auto key = parseString(json, pos);
            if (!key || !key->isString()) break;
            
            skipWhitespace(json, pos);
            if (pos >= json.size() || json[pos] != ':') break;
            pos++; // Skip ':'
            
            skipWhitespace(json, pos);
            
            // Parse value
            auto value = parseValue(json, pos);
            if (!value) break;
            
            object[key->string] = std::move(value);
            
            skipWhitespace(json, pos);
            if (pos >= json.size()) break;
            
            if (json[pos] == '}') {
                pos++;
                break;
            } else if (json[pos] == ',') {
                pos++;
                skipWhitespace(json, pos);
            } else {
                break; // Invalid format
            }
        }
        
        return std::make_unique<JsonValue>(object);
    }
    
    static void skipWhitespace(const std::string& json, size_t& pos) {
        while (pos < json.size() && std::isspace(json[pos])) {
            pos++;
        }
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
    // Try JSON format first
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try to parse as JSON
    auto jsonRoot = JsonParser::parse(content);
    if (jsonRoot && jsonRoot->isObject()) {
        convertJsonToConfig(jsonRoot->object);
        return true;
    }
    
    // If not JSON, try the simple key=value format
    file.clear();
    file.seekg(0);
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = JsonParser::trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = JsonParser::trim(line.substr(0, pos));
            std::string value = JsonParser::trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
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
    
    // Simple key=value format for now
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "" << "\n";
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
    std::string result;
    result.reserve(str.size());
    std::transform(str.begin(), str.end(), std::back_inserter(result),
                   [](unsigned char c) { return std::tolower(c); });
    return result;
}

void Config::convertJsonToConfig(const JsonObject& json) {
    for (const auto& pair : json) {
        const std::string& key = pair.first;
        const auto& value = pair.second;
        
        if (value->isBool()) {
            set(key, value->boolean, ConfigSource::File);
        } else if (value->isNumber()) {
            // Check if it's an integer
            if (std::floor(value->number) == value->number && 
                value->number >= static_cast<double>(std::numeric_limits<int64_t>::min()) &&
                value->number <= static_cast<double>(std::numeric_limits<int64_t>::max())) {
                set(key, static_cast<int64_t>(value->number), ConfigSource::File);
            } else {
                set(key, value->number, ConfigSource::File);
            }
        } else if (value->isString()) {
            set(key, value->string, ConfigSource::File);
        } else if (value->isArray()) {
            // Convert array to string representation
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < value->array.size(); ++i) {
                if (i > 0) oss << ",";
                if (value->array[i]->isString()) {
                    oss << "\"" << value->array[i]->string << "\"";
                } else {
                    oss << value->array[i]->number;
                }
            }
            oss << "]";
            set(key, oss.str(), ConfigSource::File);
        }
    }
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