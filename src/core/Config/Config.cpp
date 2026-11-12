#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <any>
#include <variant>
#include <vector>
#include <string>
#include <stdexcept>
#include <optional>
#include <cmath>

// Simple JSON parser implementation
namespace json {

    // JSON value types
    using JsonValue = std::variant<
        std::monostate,  // null
        bool,
        double,
        std::string,
        std::vector<JsonValue>,  // array
        std::unordered_map<std::string, JsonValue>  // object
    >;
    
    // JSON token types
    enum class TokenType {
        LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET,
        STRING, NUMBER, BOOL, NULL_VAL, COMMA, COLON,
        END_OF_FILE
    };
    
    struct Token {
        TokenType type;
        std::string value;
        double number;
        bool boolean;
    };
    
    class JsonParser {
    private:
        std::string::const_iterator current;
        std::string input;
        
        void skipWhitespace() {
            while (current != input.end() && std::isspace(*current)) {
                ++current;
            }
        }
        
        Token nextToken() {
            skipWhitespace();
            
            if (current == input.end()) {
                return {TokenType::END_OF_FILE, "", 0.0, false};
            }
            
            char ch = *current;
            
            if (ch == '{') {
                ++current;
                return {TokenType::LEFT_BRACE, "", 0.0, false};
            }
            if (ch == '}') {
                ++current;
                return {TokenType::RIGHT_BRACE, "", 0.0, false};
            }
            if (ch == '[') {
                ++current;
                return {TokenType::LEFT_BRACKET, "", 0.0, false};
            }
            if (ch == ']') {
                ++current;
                return {TokenType::RIGHT_BRACKET, "", 0.0, false};
            }
            if (ch == ',') {
                ++current;
                return {TokenType::COMMA, "", 0.0, false};
            }
            if (ch == ':') {
                ++current;
                return {TokenType::COLON, "", 0.0, false};
            }
            if (ch == '"') {
                return parseString();
            }
            if (ch == 't' || ch == 'f') {
                return parseBoolean();
            }
            if (ch == 'n') {
                return parseNull();
            }
            if (ch == '-' || std::isdigit(ch)) {
                return parseNumber();
            }
            
            throw std::runtime_error("Unexpected character: " + std::string(1, ch));
        }
        
        Token parseString() {
            std::string result;
            ++current; // Skip opening quote
            while (current != input.end() && *current != '"') {
                if (*current == '\\') {
                    ++current;
                    if (current == input.end()) {
                        throw std::runtime_error("Invalid escape sequence");
                    }
                    char escaped = *current;
                    switch (escaped) {
                        case '\\': result += '\\'; break;
                        case '\"': result += '"'; break;
                        case '/': result += '/'; break;
                        case 'b': result += '\b'; break;
                        case 'f': result += '\f'; break;
                        case 'n': result += '\n'; break;
                        case 'r': result += '\r'; break;
                        case 't': result += '\t'; break;
                        default: throw std::runtime_error("Invalid escape sequence");
                    }
                } else {
                    result += *current;
                }
                ++current;
            }
            
            if (current == input.end() || *current != '"') {
                throw std::runtime_error("Unterminated string literal");
            }
            
            ++current; // Skip closing quote
            return {TokenType::STRING, result, 0.0, false};
        }
        
        Token parseNumber() {
            std::string numStr;
            while (current != input.end() && (std::isdigit(*current) || *current == '-' || *current == '.' || *current == 'e' || *current == 'E' || *current == '+')) {
                numStr += *current;
                ++current;
            }
            
            try {
                double value = std::stod(numStr);
                return {TokenType::NUMBER, "", value, false};
            } catch (...) {
                throw std::runtime_error("Invalid number: " + numStr);
            }
        }
        
        Token parseBoolean() {
            std::string word;
            while (current != input.end() && std::isalpha(*current)) {
                word += *current;
                ++current;
            }
            
            if (word == "true") {
                return {TokenType::BOOL, "", 0.0, true};
            } else if (word == "false") {
                return {TokenType::BOOL, "", 0.0, false};
            } else {
                throw std::runtime_error("Invalid boolean value: " + word);
            }
        }
        
        Token parseNull() {
            std::string word;
            while (current != input.end() && std::isalpha(*current)) {
                word += *current;
                ++current;
            }
            
            if (word == "null") {
                return {TokenType::NULL_VAL, "", 0.0, false};
            } else {
                throw std::runtime_error("Invalid null value: " + word);
            }
        }
        
        JsonValue parseValue();
        JsonValue parseObject();
        JsonValue parseArray();
        
    public:
        explicit JsonParser(const std::string& input) : input(input), current(input.begin()) {}
        
        JsonValue parse() {
            auto token = nextToken();
            JsonValue result = parseValue();
            
            skipWhitespace();
            if (token.type != TokenType::END_OF_FILE) {
                throw std::runtime_error("Extra data after JSON");
            }
            
            return result;
        }
    
    private:
        JsonValue parseValue() {
            auto token = nextToken();
            skipWhitespace();
            
            switch (token.type) {
                case TokenType::LEFT_BRACE:
                    return parseObject();
                case TokenType::LEFT_BRACKET:
                    return parseArray();
                case TokenType::STRING:
                    return token.value;
                case TokenType::NUMBER:
                    return token.number;
                case TokenType::BOOL:
                    return token.boolean;
                case TokenType::NULL_VAL:
                    return std::monostate();
                default:
                    throw std::runtime_error("Unexpected token type in JSON value");
            }
        }
        
        JsonValue parseObject() {
            std::unordered_map<std::string, JsonValue> obj;
            
            auto token = nextToken();
            skipWhitespace();
            
            if (token.type == TokenType::RIGHT_BRACE) {
                return obj; // Empty object
            }
            
            while (true) {
                if (token.type != TokenType::STRING) {
                    throw std::runtime_error("Expected string key in object");
                }
                
                std::string key = token.value;
                
                token = nextToken();
                skipWhitespace();
                if (token.type != TokenType::COLON) {
                    throw std::runtime_error("Expected colon after key");
                }
                
                JsonValue value = parseValue();
                obj[key] = std::move(value);
                
                token = nextToken();
                skipWhitespace();
                
                if (token.type == TokenType::RIGHT_BRACE) {
                    break;
                } else if (token.type == TokenType::COMMA) {
                    token = nextToken();
                    skipWhitespace();
                } else {
                    throw std::runtime_error("Expected comma or closing brace");
                }
            }
            
            return obj;
        }
        
        JsonValue parseArray() {
            std::vector<JsonValue> arr;
            
            auto token = nextToken();
            skipWhitespace();
            
            if (token.type == TokenType::RIGHT_BRACKET) {
                return arr; // Empty array
            }
            
            while (true) {
                JsonValue value = parseValue();
                arr.push_back(std::move(value));
                
                token = nextToken();
                skipWhitespace();
                
                if (token.type == TokenType::RIGHT_BRACKET) {
                    break;
                } else if (token.type == TokenType::COMMA) {
                    token = nextToken();
                    skipWhitespace();
                } else {
                    throw std::runtime_error("Expected comma or closing bracket");
                }
            }
            
            return arr;
        }
    };
    
    JsonValue parseJson(const std::string& input) {
        JsonParser parser(input);
        return parser.parse();
    }
    
    // Helper functions to convert JSON to ConfigValue
    ConfigValue convertToConfigValue(const JsonValue& jsonValue);
    
    std::vector<ConfigValue> convertToConfigValueVector(const std::vector<JsonValue>& jsonArray);
    
    std::string jsonValueToString(const JsonValue& jsonValue);
    std::string jsonValueToString(const JsonValue& jsonValue);

// Helper functions to convert JSON to ConfigValue
ConfigValue convertToConfigValue(const JsonValue& jsonValue);

std::vector<ConfigValue> convertToConfigValueVector(const std::vector<JsonValue>& jsonArray);

std::string jsonValueToString(const JsonValue& jsonValue) {
    if (std::holds_alternative<std::monostate>(jsonValue)) {
        return "null";
    } else if (std::holds_alternative<bool>(jsonValue)) {
        return std::get<bool>(jsonValue) ? "true" : "false";
    } else if (std::holds_alternative<double>(jsonValue)) {
        return std::to_string(std::get<double>(jsonValue));
    } else if (std::holds_alternative<std::string>(jsonValue)) {
        return std::get<std::string>(jsonValue);
    } else if (std::holds_alternative<std::vector<JsonValue>>(jsonValue)) {
        return "array";
    } else if (std::holds_alternative<std::unordered_map<std::string, JsonValue>>(jsonValue)) {
        return "object";
    }
    return "unknown";
}

ConfigValue convertToConfigValue(const JsonValue& jsonValue) {
    if (std::holds_alternative<std::monostate>(jsonValue)) {
        return "null";
    } else if (std::holds_alternative<bool>(jsonValue)) {
        return std::get<bool>(jsonValue);
    } else if (std::holds_alternative<double>(jsonValue)) {
        double val = std::get<double>(jsonValue);
        // Try to convert to int if it's a whole number
        if (std::floor(val) == val) {
            return static_cast<int64_t>(val);
        }
        // Check if it fits in float
        if (val >= std::numeric_limits<float>::min() && val <= std::numeric_limits<float>::max()) {
            return static_cast<float>(val);
        }
        return val;
    } else if (std::holds_alternative<std::string>(jsonValue)) {
        return std::get<std::string>(jsonValue);
    } else if (std::holds_alternative<std::vector<JsonValue>>(jsonValue)) {
        const auto& jsonArray = std::get<std::vector<JsonValue>>(jsonValue);
        return convertToConfigValueVector(jsonArray);
    } else if (std::holds_alternative<std::unordered_map<std::string, JsonValue>>(jsonValue)) {
        // For now, treat objects as strings
        const auto& jsonObj = std::get<std::unordered_map<std::string, JsonValue>>(jsonValue);
        std::string result = "{";
        bool first = true;
        for (const auto& [key, val] : jsonObj) {
            if (!first) result += ", ";
            result += "\"" + key + "\": " + jsonValueToString(val);
            first = false;
        }
        result += "}";
        return result;
    }
    return "unknown";
}

std::vector<ConfigValue> convertToConfigValueVector(const std::vector<JsonValue>& jsonArray) {
    std::vector<ConfigValue> result;
    result.reserve(jsonArray.size());
    
    for (const auto& jsonVal : jsonArray) {
        result.push_back(convertToConfigValue(jsonVal));
    }
    
    return result;
}

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
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    try {
        auto jsonValue = json::parseJson(content);
        
        if (std::holds_alternative<std::unordered_map<std::string, json::JsonValue>>(jsonValue)) {
            const auto& jsonObj = std::get<std::unordered_map<std::string, json::JsonValue>>(jsonValue);
            
            for (const auto& [key, jsonVal] : jsonObj) {
                ConfigValue configVal = json::convertToConfigValue(jsonVal);
                set(key, configVal, ConfigSource::File);
            }
        }
    } catch (const std::exception& e) {
        // Fallback to simple key=value parser
        file.clear();
        file.seekg(0, std::ios::beg);
        
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
    
    file << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        file << "\"" << arg[j] << "\"";
                    } else {
                        file << arg[j];
                    }
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, int> ||
                                std::is_same_v<T, int64_t> ||
                                std::is_same_v<T, double> ||
                                std::is_same_v<T, bool>) {
                if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else {
                    file << arg;
                }
            } else {
                file << "null";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    
    file << "}\n";
    
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

} // namespace json

namespace nlm {

class ConfigValidator {
public:
    static bool validate(const Config& config, std::string& errorMsg) {
        const auto& entries = config.getKeys();
        
        for (const auto& key : entries) {
            if (!validateKey(key, errorMsg)) {
                return false;
            }
            
            // Try to get value as different types for validation
            bool valid = false;
            
            // Try string
            if (auto val = config.get<std::string>(key)) {
                valid = validateStringValue(key, *val, errorMsg);
            }
            // Try int
            else if (auto val = config.get<int64_t>(key)) {
                valid = validateIntValue(key, *val, errorMsg);
            }
            // Try double
            else if (auto val = config.get<double>(key)) {
                valid = validateDoubleValue(key, *val, errorMsg);
            }
            // Try bool
            else if (auto val = config.get<bool>(key)) {
                valid = validateBoolValue(key, *val, errorMsg);
            }
            
            if (!valid) {
                return false;
            }
        }
        
        return true;
    }
    
private:
    static bool validateKey(const std::string& key, std::string& errorMsg) {
        // Basic key validation
        if (key.empty()) {
            errorMsg = "Empty configuration key is not allowed";
            return false;
        }
        
        // Check for valid characters in key
        for (char c : key) {
            if (!(std::isalnum(c) || c == '.' || c == '_' || c == '-')) {
                errorMsg = "Invalid character in configuration key: '" + std::string(1, c) + "'. Keys must contain only alphanumeric characters, dots, underscores, or hyphens.";
                return false;
            }
        }
        
        return true;
    }
    
    static bool validateStringValue(const std::string& key, const std::string& value, std::string& errorMsg) {
        // Key-specific validation
        if (key == "brain.neuron_count" || key == "neuron_count") {
            try {
                int64_t neurons = std::stoll(value);
                if (neurons <= 0) {
                    errorMsg = "brain.neuron_count must be greater than 0, got: " + value;
                    return false;
                }
                if (neurons > 1000000) {
                    errorMsg = "brain.neuron_count is too large (max 1000000), got: " + value;
                    return false;
                }
            } catch (const std::exception&) {
                errorMsg = "brain.neuron_count must be a valid integer, got: " + value;
                return false;
            }
        }
        else if (key == "brain.region_count" || key == "region_count") {
            try {
                int64_t regions = std::stoll(value);
                if (regions <= 0) {
                    errorMsg = "region_count must be greater than 0, got: " + value;
                    return false;
                }
                if (regions > 100) {
                    errorMsg = "region_count is too large (max 100), got: " + value;
                    return false;
                }
            } catch (const std::exception&) {
                errorMsg = "region_count must be a valid integer, got: " + value;
                return false;
            }
        }
        else if (key == "brain.connection_probability" || key == "connection_probability") {
            try {
                double prob = std::stod(value);
                if (prob < 0.0 || prob > 1.0) {
                    errorMsg = "connection_probability must be between 0.0 and 1.0, got: " + value;
                    return false;
                }
            } catch (const std::exception&) {
                errorMsg = "connection_probability must be a valid number, got: " + value;
                return false;
            }
        }
        else if (key == "brain.initial_weight_mean" || key == "initial_weight_mean") {
            try {
                double mean = std::stod(value);
                if (mean < 0.0 || mean > 1.0) {
                    errorMsg = "initial_weight_mean must be between 0.0 and 1.0, got: " + value;
                    return false;
                }
            } catch (const std::exception&) {
                errorMsg = "initial_weight_mean must be a valid number, got: " + value;
                return false;
            }
        }
        else if (key == "simulation_timestep" || key == "brain.simulation_timestep") {
            try {
                double timestep = std::stod(value);
                if (timestep <= 0.0) {
                    errorMsg = "simulation_timestep must be greater than 0.0, got: " + value;
                    return false;
                }
                if (timestep > 0.1) {
                    errorMsg = "simulation_timestep is too large (max 0.1), got: " + value;
                    return false;
                }
            } catch (const std::exception&) {
                errorMsg = "simulation_timestep must be a valid number, got: " + value;
                return false;
            }
        }
        else if (key == "random_seed" || key == "brain.random_seed") {
            try {
                int64_t seed = std::stoll(value);
                if (seed < 0) {
                    errorMsg = "random_seed must be non-negative, got: " + value;
                    return false;
                }
            } catch (const std::exception&) {
                errorMsg = "random_seed must be a valid integer, got: " + value;
                return false;
            }
        }
        
        return true;
    }
    
    static bool validateIntValue(const std::string& key, int64_t value, std::string& errorMsg) {
        if (key == "brain.neuron_count" || key == "neuron_count") {
            if (value <= 0) {
                errorMsg = "neuron_count must be greater than 0, got: " + std::to_string(value);
                return false;
            }
            if (value > 1000000) {
                errorMsg = "neuron_count is too large (max 1000000), got: " + std::to_string(value);
                return false;
            }
        }
        return true;
    }
    
    static bool validateDoubleValue(const std::string& key, double value, std::string& errorMsg) {
        if (key == "brain.connection_probability" || key == "connection_probability") {
            if (value < 0.0 || value > 1.0) {
                errorMsg = "connection_probability must be between 0.0 and 1.0, got: " + std::to_string(value);
                return false;
            }
        }
        else if (key == "simulation_timestep" || key == "brain.simulation_timestep") {
            if (value <= 0.0) {
                errorMsg = "simulation_timestep must be greater than 0.0, got: " + std::to_string(value);
                return false;
            }
            if (value > 0.1) {
                errorMsg = "simulation_timestep is too large (max 0.1), got: " + std::to_string(value);
                return false;
            }
        }
        return true;
    }
    
    static bool validateBoolValue(const std::string& key, bool value, std::string& errorMsg) {
        // Boolean validation - just check key exists
        return true;
    }
};
