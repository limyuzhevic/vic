#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <cctype>
#include <unordered_map>
#include <memory>
#include <stack>

namespace nlm {

// JSON Parser helper functions
namespace JsonParser {

class JsonTokenizer {
public:
    enum TokenType {
        TT_EOF,
        TT_STRING,
        TT_NUMBER,
        TT_BOOLEAN,
        TT_NULL,
        TT_OBJECT_START,
        TT_OBJECT_END,
        TT_ARRAY_START,
        TT_ARRAY_END,
        TT_COMMA,
        TT_COLON,
        TT_ERROR
    };
    
    struct Token {
        TokenType type;
        std::string value;
        size_t line;
        size_t column;
    };
    
    explicit JsonTokenizer(const std::string& source) : source(source), pos(0), line(1), column(1) {}
    
    Token nextToken() {
        skipWhitespaceAndComments();
        if (pos >= source.length()) {
            return {TT_EOF, "", line, column};
        }
        
        char ch = source[pos];
        
        if (ch == '{') {
            ++pos;
            ++column;
            return {TT_OBJECT_START, "{", line, column - 1};
        } else if (ch == '}') {
            ++pos;
            ++column;
            return {TT_OBJECT_END, "}", line, column - 1};
        } else if (ch == '[') {
            ++pos;
            ++column;
            return {TT_ARRAY_START, "[", line, column - 1};
        } else if (ch == ']') {
            ++pos;
            ++column;
            return {TT_ARRAY_END, "]", line, column - 1};
        } else if (ch == ',') {
            ++pos;
            ++column;
            return {TT_COMMA, ",", line, column - 1};
        } else if (ch == ':') {
            ++pos;
            ++column;
            return {TT_COLON, ":", line, column - 1};
        } else if (ch == '"') {
            return readString();
        } else if (ch == '-' || std::isdigit(ch)) {
            return readNumber();
        } else if (ch == 't' || ch == 'f') {
            return readBoolean();
        } else if (ch == 'n') {
            return readNull();
        }
        
        // Error
        return {TT_ERROR, std::string(1, ch), line, column};
    }
    
private:
    std::string source;
    size_t pos;
    size_t line;
    size_t column;
    
    void skipWhitespaceAndComments() {
        while (pos < source.length()) {
            char ch = source[pos];
            if (std::isspace(ch)) {
                if (ch == '\n') {
                    ++line;
                    column = 1;
                } else {
                    ++column;
                }
                ++pos;
            } else if (ch == '/' && pos + 1 < source.length() && source[pos + 1] == '/') {
                // Line comment
                while (pos < source.length() && source[pos] != '\n') {
                    ++pos;
                    ++column;
                }
            } else if (ch == '/' && pos + 1 < source.length() && source[pos + 1] == '*') {
                // Block comment
                pos += 2;
                column += 2;
                while (pos + 1 < source.length() && !(source[pos] == '*' && source[pos + 1] == '/')) {
                    if (source[pos] == '\n') {
                        ++line;
                        column = 1;
                    } else {
                        ++column;
                    }
                    ++pos;
                }
                if (pos + 1 < source.length()) {
                    pos += 2;
                    column += 2;
                }
            } else {
                break;
            }
        }
    }
    
    Token readString() {
        size_t start = ++pos; // Skip opening quote
        column++; // Skip opening quote
        
        while (pos < source.length()) {
            char ch = source[pos];
            if (ch == '\\') {
                ++pos;
                ++column;
                if (pos < source.length()) {
                    ++pos;
                    ++column;
                }
            } else if (ch == '"') {
                std::string value = source.substr(start, pos - start);
                ++pos;
                ++column;
                return {TT_STRING, value, line, column - 1};
            } else if (ch == '\n') {
                ++line;
                column = 1;
            } else {
                ++pos;
                ++column;
            }
        }
        
        return {TT_ERROR, "Unterminated string", line, column};
    }
    
    Token readNumber() {
        size_t start = pos;
        bool isFloat = false;
        bool isNegative = false;
        
        if (source[pos] == '-') {
            isNegative = true;
            ++pos;
            ++column;
            ++start;
        }
        
        while (pos < source.length() && std::isdigit(source[pos])) {
            ++pos;
            ++column;
        }
        
        if (pos < source.length() && source[pos] == '.') {
            isFloat = true;
            ++pos;
            ++column;
            while (pos < source.length() && std::isdigit(source[pos])) {
                ++pos;
                ++column;
            }
        }
        
        if (pos < source.length() && (source[pos] == 'e' || source[pos] == 'E')) {
            isFloat = true;
            ++pos;
            ++column;
            if (pos < source.length() && (source[pos] == '+' || source[pos] == '-')) {
                ++pos;
                ++column;
            }
            while (pos < source.length() && std::isdigit(source[pos])) {
                ++pos;
                ++column;
            }
        }
        
        std::string numStr = source.substr(start, pos - start);
        return {isFloat ? TT_NUMBER : TT_NUMBER, numStr, line, column - 1};
    }
    
    Token readBoolean() {
        if (source.substr(pos, 4) == "true") {
            pos += 4;
            column += 4;
            return {TT_BOOLEAN, "true", line, column - 4};
        } else if (source.substr(pos, 5) == "false") {
            pos += 5;
            column += 5;
            return {TT_BOOLEAN, "false", line, column - 5};
        }
        return {TT_ERROR, "Invalid boolean", line, column};
    }
    
    Token readNull() {
        if (source.substr(pos, 4) == "null") {
            pos += 4;
            column += 4;
            return {TT_NULL, "null", line, column - 4};
        }
        return {TT_ERROR, "Invalid null", line, column};
    }
};

class JsonParser {
public:
    struct JsonValue {
        enum Type { NULL_TYPE, BOOL, NUMBER, STRING, ARRAY, OBJECT } type;
        bool boolVal;
        double numberVal;
        std::string stringVal;
        std::vector<JsonValue> arrayVal;
        std::unordered_map<std::string, JsonValue> objectVal;
        
        JsonValue() : type(NULL_TYPE) {}
        JsonValue(bool v) : type(BOOL), boolVal(v) {}
        JsonValue(double v) : type(NUMBER), numberVal(v) {}
        JsonValue(const std::string& v) : type(STRING), stringVal(v) {}
        JsonValue(const std::vector<JsonValue>& v) : type(ARRAY), arrayVal(v) {}
        JsonValue(const std::unordered_map<std::string, JsonValue>& v) : type(OBJECT), objectVal(v) {}
    };
    
    static std::unordered_map<std::string, ConfigValue> parseJsonToConfig(const std::string& jsonText) {
        std::unordered_map<std::string, ConfigValue> config;
        
        JsonTokenizer tokenizer(jsonText);
        JsonValue root = parseValue(tokenizer);
        
        if (root.type != JsonValue::OBJECT) {
            // If root is not an object, treat it as a single key-value
            std::string key = "value";
            config[key] = valueToConfigValue(root);
        } else {
            for (const auto& pair : root.objectVal) {
                std::string key = pair.first;
                ConfigValue value = valueToConfigValue(pair.second);
                config[key] = value;
            }
        }
        
        return config;
    }
    
private:
    static JsonValue parseValue(JsonTokenizer& tokenizer) {
        JsonTokenizer::Token token = tokenizer.nextToken();
        
        switch (token.type) {
            case JsonTokenizer::TT_NULL:
                return JsonValue();
            case JsonTokenizer::TT_BOOLEAN:
                return JsonValue(token.value == "true");
            case JsonTokenizer::TT_NUMBER: {
                double num = std::stod(token.value);
                return JsonValue(num);
            }
            case JsonTokenizer::TT_STRING:
                return JsonValue(token.value);
            case JsonTokenizer::TT_OBJECT_START: {
                std::unordered_map<std::string, JsonValue> obj;
                
                while (true) {
                    JsonTokenizer::Token next = tokenizer.nextToken();
                    if (next.type == JsonTokenizer::TT_OBJECT_END) {
                        break;
                    }
                    
                    if (next.type != JsonTokenizer::TT_STRING) {
                        throw std::runtime_error("Expected string key in object");
                    }
                    
                    std::string key = next.value;
                    
                    JsonTokenizer::Token colon = tokenizer.nextToken();
                    if (colon.type != JsonTokenizer::TT_COLON) {
                        throw std::runtime_error("Expected colon after object key");
                    }
                    
                    JsonValue value = parseValue(tokenizer);
                    obj[key] = std::move(value);
                    
                    JsonTokenizer::Token commaOrEnd = tokenizer.nextToken();
                    if (commaOrEnd.type == JsonTokenizer::TT_COMMA) {
                        continue;
                    } else if (commaOrEnd.type == JsonTokenizer::TT_OBJECT_END) {
                        break;
                    } else {
                        throw std::runtime_error("Expected comma or end of object");
                    }
                }
                
                return JsonValue(obj);
            }
            case JsonTokenizer::TT_ARRAY_START: {
                std::vector<JsonValue> arr;
                
                while (true) {
                    JsonValue value = parseValue(tokenizer);
                    arr.push_back(std::move(value));
                    
                    JsonTokenizer::Token commaOrEnd = tokenizer.nextToken();
                    if (commaOrEnd.type == JsonTokenizer::TT_COMMA) {
                        continue;
                    } else if (commaOrEnd.type == JsonTokenizer::TT_ARRAY_END) {
                        break;
                    } else {
                        throw std::runtime_error("Expected comma or end of array");
                    }
                }
                
                return JsonValue(arr);
            }
            default:
                throw std::runtime_error("Unexpected token: " + token.value);
        }
    }
    
    static ConfigValue valueToConfigValue(const JsonValue& json) {
        switch (json.type) {
            case JsonValue::NULL_TYPE:
                return std::string("null");
            case JsonValue::BOOL:
                return json.boolVal;
            case JsonValue::NUMBER:
                return json.numberVal;
            case JsonValue::STRING:
                return json.stringVal;
            case JsonValue::ARRAY: {
                std::vector<double> vec;
                for (const auto& item : json.arrayVal) {
                    if (item.type == JsonValue::NUMBER) {
                        vec.push_back(item.numberVal);
                    } else {
                        vec.push_back(0.0);  // Default for non-numbers
                    }
                }
                return vec;
            }
            case JsonValue::OBJECT:
                // For objects, convert to string representation
                return "{}"; // Simple placeholder
        }
        return std::string("");
    }
};

} // namespace JsonParser

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

std::string Config::normalizeJsonKey(const std::string& key) {
    std::string normalized = key;
    
    // Convert camelCase to snake_case for consistency
    std::string result;
    for (char ch : normalized) {
        if (std::isupper(ch)) {
            if (!result.empty() && result.back() != '_') {
                result += '_';
            }
            result += std::tolower(ch);
        } else {
            result += ch;
        }
    }
    
    return result;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Check if file appears to be JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file to detect format and parse
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Trim whitespace
    content = trim(content);
    
    // Check if it looks like JSON (starts with { or [)
    bool isJson = !content.empty() && (content.front() == '{' || content.front() == '[');
    
    if (isJson) {
        try {
            // Use JSON parser
            auto jsonConfig = JsonParser::parseJsonToConfig(content);
            
            // Clear existing entries
            clear();
            
            // Add entries from JSON
            for (const auto& pair : jsonConfig) {
                set(pair.first, pair.second, ConfigSource::File);
            }
            
            NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath);
            return true;
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()));
            return false;
        }
    }
    
    // Otherwise, treat as simple key=value format
    clear();
    
    std::stringstream fileStream(content);
    std::string line;
    while (std::getline(fileStream, line)) {
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
    
    NLM_LOG_INFO("Loaded configuration from simple file: " + filepath);
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format (JSON-like)
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Try to parse as JSON-like value
                if (value == "true") {
                    set(key, true, ConfigSource::CommandLine);
                } else if (value == "false") {
                    set(key, false, ConfigSource::CommandLine);
                } else if (value == "null") {
                    set(key, std::string("null"), ConfigSource::CommandLine);
                } else {
                    // Try to parse as number
                    try {
                        size_t posEnd;
                        double num = std::stod(value, &posEnd);
                        if (posEnd == value.length()) {
                            set(key, num, ConfigSource::CommandLine);
                            continue;
                        }
                    } catch (...) {}
                    
                    // Try to parse as integer
                    try {
                        size_t posEnd;
                        long long intVal = std::stoll(value, &posEnd);
                        if (posEnd == value.length()) {
                            set(key, intVal, ConfigSource::CommandLine);
                            continue;
                        }
                    } catch (...) {}
                    
                    // Treat as string
                    set(key, value, ConfigSource::CommandLine);
                }
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
    
    // Save as JSON format
    file << "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else {
                file << "\"" << arg << "\"";
            }
        }, entry.value);
        
        file << "\n";
        first = false;
    }
    
    file << "}\n";
    
    NLM_LOG_INFO("Configuration saved to JSON file: " + filepath);
    return true;
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
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
