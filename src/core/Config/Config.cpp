#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstring>
#include <iostream>
#include <unordered_map>

namespace nlm {

namespace {
    // Simple JSON parser for NLM configuration
    // Supports basic types: numbers, strings, booleans, objects, arrays
    
    // JSON value types
    enum class JSONValueType { Null, Boolean, Number, String, Object, Array };
    
    struct JSONValue;
    struct JSONObject;
    struct JSONArray;
    
    struct JSONValue {
        JSONValueType type;
        union {
            bool boolean;
            double number;
            std::string* string;
            JSONObject* object;
            JSONArray* array;
        };
        
        JSONValue() : type(JSONValueType::Null) {}
        ~JSONValue() {
            switch (type) {
                case JSONValueType::String: delete string; break;
                case JSONValueType::Object: delete object; break;
                case JSONValueType::Array: delete array; break;
                default: break;
            }
        }
    };
    
    struct JSONObject {
        std::unordered_map<std::string, JSONValue> members;
    };
    
    struct JSONArray {
        std::vector<JSONValue> elements;
    };
    
    // Simple tokenization
    class JSONTokenizer {
    public:
        JSONTokenizer(const std::string& input) : text(input), pos(0) {}
        
        enum TokenType { TT_EOF, TT_STRING, TT_NUMBER, TT_BOOLEAN, TT_NULL, TT_COMMA, TT_COLON, TT_BRACE_OPEN, TT_BRACE_CLOSE, TT_BRACKET_OPEN, TT_BRACKET_CLOSE };
        
        struct Token {
            TokenType type;
            std::string value;
            size_t line, column;
        };
        
        Token nextToken() {
            skipWhitespace();
            if (pos >= text.length()) return {TT_EOF, "", lineCount, columnCount};
            
            char c = text[pos];
            lineCount = lineCountAtPos;
            columnCount = pos - lastNewline - lineStart;
            
            if (c == '{') { pos++; return {TT_BRACE_OPEN, "{", lineCount, columnCount}; }
            if (c == '}') { pos++; return {TT_BRACE_CLOSE, "}", lineCount, columnCount}; }
            if (c == '[') { pos++; return {TT_BRACKET_OPEN, "[", lineCount, columnCount}; }
            if (c == ']') { pos++; return {TT_BRACKET_CLOSE, "]", lineCount, columnCount}; }
            if (c == ':') { pos++; return {TT_COLON, ":", lineCount, columnCount}; }
            if (c == ',') { pos++; return {TT_COMMA, ",", lineCount, columnCount}; }
            
            if (c == '"') return readString();
            if (c == '-' || (c >= '0' && c <= '9')) return readNumber();
            if (c == 't' || c == 'f') return readBoolean();
            if (c == 'n') return readNull();
            
            throw std::runtime_error("Unexpected character in JSON: " + std::string(1, c));
        }
        
    private:
        std::string text;
        size_t pos;
        size_t lastNewline;
        size_t lineStart;
        size_t lineCount;
        
        void skipWhitespace() {
            while (pos < text.length()) {
                char c = text[pos];
                if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
                    if (c == '\n') {
                        lastNewline = pos;
                        lineStart = pos + 1;
                        lineCount++;
                    }
                    pos++;
                } else {
                    break;
                }
            }
        }
        
        Token readString() {
            size_t start = ++pos; // Skip opening quote
            std::string result;
            while (pos < text.length() && text[pos] != '"') {
                if (text[pos] == '\\' && pos + 1 < text.length()) {
                    pos++; // Skip escape char
                    if (pos < text.length()) {
                        result += text[pos];
                        pos++;
                    }
                } else {
                    result += text[pos];
                    pos++;
                }
            }
            if (pos >= text.length()) throw std::runtime_error("Unterminated string in JSON");
            pos++; // Skip closing quote
            return {TT_STRING, result, lineCount, columnCount};
        }
        
        Token readNumber() {
            size_t start = pos;
            bool isFloat = false;
            while (pos < text.length()) {
                char c = text[pos];
                if (c == '-' || c == '+' || (c >= '0' && c <= '9') || c == '.' || c == 'e' || c == 'E') {
                    if (c == '.' || c == 'e' || c == 'E') isFloat = true;
                    pos++;
                } else {
                    break;
                }
            }
            std::string numStr = text.substr(start, pos - start);
            return {TT_NUMBER, numStr, lineCount, columnCount};
        }
        
        Token readBoolean() {
            size_t start = pos;
            if (text.substr(pos, 4) == "true") {
                pos += 4;
                return {TT_BOOLEAN, "true", lineCount, columnCount};
            } else if (text.substr(pos, 5) == "false") {
                pos += 5;
                return {TT_BOOLEAN, "false", lineCount, columnCount};
            }
            throw std::runtime_error("Invalid boolean value in JSON");
        }
        
        Token readNull() {
            size_t start = pos;
            if (text.substr(pos, 4) == "null") {
                pos += 4;
                return {TT_NULL, "null", lineCount, columnCount};
            }
            throw std::runtime_error("Invalid null value in JSON");
        }
        
        size_t lineCountAtPos() {
            size_t count = 1;
            for (size_t i = 0; i < pos && i < text.length(); i++) {
                if (text[i] == '\n') count++;
            }
            return count;
        }
        
        size_t columnCount = 0;
    };
    
    // Parse JSON into our internal representation
    JSONValue parseJSON(const std::string& jsonStr) {
        JSONTokenizer tokenizer(jsonStr);
        JSONValue result;
        
        JSONTokenizer::Token token = tokenizer.nextToken();
        if (token.type == JSONTokenizer::TT_EOF) {
            result.type = JSONValueType::Null;
            return result;
        }
        
        result = parseValue(tokenizer, token);
        
        token = tokenizer.nextToken();
        if (token.type != JSONTokenizer::TT_EOF) {
            throw std::runtime_error("Extra content after JSON");
        }
        
        return result;
    }
    
    JSONValue parseValue(JSONTokenizer& tokenizer, JSONTokenizer::Token token) {
        switch (token.type) {
            case JSONTokenizer::TT_BOOLEAN:
                if (token.value == "true") {
                    JSONValue result;
                    result.type = JSONValueType::Boolean;
                    result.boolean = true;
                    return result;
                } else {
                    JSONValue result;
                    result.type = JSONValueType::Boolean;
                    result.boolean = false;
                    return result;
                }
            case JSONTokenizer::TT_NUMBER:
                JSONValue result;
                result.type = JSONValueType::Number;
                result.number = std::stod(token.value);
                return result;
            case JSONTokenizer::TT_STRING:
                JSONValue result;
                result.type = JSONValueType::String;
                result.string = new std::string(token.value);
                return result;
            case JSONTokenizer::TT_BRACE_OPEN:
                return parseObject(tokenizer);
            case JSONTokenizer::TT_BRACKET_OPEN:
                return parseArray(tokenizer);
            case JSONTokenizer::TT_NULL:
                JSONValue result;
                result.type = JSONValueType::Null;
                return result;
            default:
                throw std::runtime_error("Unexpected token in JSON value");
        }
    }
    
    JSONValue parseObject(JSONTokenizer& tokenizer) {
        JSONValue result;
        result.type = JSONValueType::Object;
        result.object = new JSONObject();
        
        JSONTokenizer::Token token = tokenizer.nextToken();
        if (token.type == JSONTokenizer::TT_BRACE_CLOSE) {
            return result; // Empty object
        }
        
        while (true) {
            if (token.type != JSONTokenizer::TT_STRING) {
                throw std::runtime_error("Expected string key in JSON object");
            }
            
            std::string key = *token.string;
            
            token = tokenizer.nextToken();
            if (token.type != JSONTokenizer::TT_COLON) {
                throw std::runtime_error("Expected ':' after key in JSON object");
            }
            
            token = tokenizer.nextToken();
            JSONValue value = parseValue(tokenizer, token);
            
            (*result.object).members[key] = std::move(value);
            
            token = tokenizer.nextToken();
            if (token.type == JSONTokenizer::TT_BRACE_CLOSE) {
                break;
            } else if (token.type == JSONTokenizer::TT_COMMA) {
                token = tokenizer.nextToken();
                continue;
            } else {
                throw std::runtime_error("Expected ',' or '}' in JSON object");
            }
        }
        
        return result;
    }
    
    JSONValue parseArray(JSONTokenizer& tokenizer) {
        JSONValue result;
        result.type = JSONValueType::Array;
        result.array = new JSONArray();
        
        JSONTokenizer::Token token = tokenizer.nextToken();
        if (token.type == JSONTokenizer::TT_BRACKET_CLOSE) {
            return result; // Empty array
        }
        
        while (true) {
            JSONValue value = parseValue(tokenizer, token);
            (*result.array).elements.push_back(std::move(value));
            
            token = tokenizer.nextToken();
            if (token.type == JSONTokenizer::TT_BRACKET_CLOSE) {
                break;
            } else if (token.type == JSONTokenizer::TT_COMMA) {
                token = tokenizer.nextToken();
                continue;
            } else {
                throw std::runtime_error("Expected ',' or ']' in JSON array");
            }
        }
        
        return result;
    }
    
    // Convert JSON value to ConfigValue
    ConfigValue jsonToConfigValue(const JSONValue& jsonValue) {
        switch (jsonValue.type) {
            case JSONValueType::Boolean:
                return jsonValue.boolean;
            case JSONValueType::Number: {
                // Try to determine if it's an int or double
                double val = jsonValue.number;
                int intVal = static_cast<int>(val);
                if (val == static_cast<double>(intVal)) {
                    return intVal;
                }
                // Check if it could be int64_t
                int64_t int64Val = static_cast<int64_t>(val);
                if (val == static_cast<double>(int64Val)) {
                    return int64Val;
                }
                return val;
            }
            case JSONValueType::String:
                return *jsonValue.string;
            case JSONValueType::Null:
                return std::string(""); // Default string for null
            case JSONValueType::Object:
            case JSONValueType::Array:
                // For now, store JSON as string, could be enhanced later
                std::ostringstream oss;
                oss << "{JSON:" << jsonValue.object << "}";
                return oss.str();
        }
        throw std::runtime_error("Unknown JSON value type");
    }
    
    // Simple JSON string parsing (fallback for file-based config)
    ConfigValue parseSimpleValue(const std::string& str) {
        // Trim whitespace
        std::string trimmed = str;
        size_t start = trimmed.find_first_not_of(" \t\r\n");
        size_t end = trimmed.find_last_not_of(" \t\r\n");
        if (start == std::string::npos) return std::string("");
        trimmed = trimmed.substr(start, end - start + 1);
        
        // Check for boolean
        if (trimmed == "true") return true;
        if (trimmed == "false") return false;
        
        // Check for number
        if (trimmed.find_first_not_of("0123456789.-+eE") == std::string::npos) {
            try {
                // Try int first
                size_t pos;
                int intVal = std::stoi(trimmed, &pos);
                if (pos == trimmed.length()) return intVal;
                
                // Try double
                double doubleVal = std::stod(trimmed, &pos);
                if (pos == trimmed.length()) return doubleVal;
                
                // Try int64_t
                size_t pos64;
                int64_t int64Val = std::stoll(trimmed, &pos64);
                if (pos64 == trimmed.length()) return int64Val;
                
                return doubleVal;
            } catch (...) {
                // Fall through to string
            }
        }
        
        // Default to string
        return trimmed;
    }
    
} // namespace

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
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Read entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    try {
        // Try to parse as JSON first
        JSONValue jsonRoot = parseJSON(content);
        
        if (jsonRoot.type == JSONValueType::Object) {
            // Parse JSON object into config entries
            for (const auto& member : (*jsonRoot.object).members) {
                ConfigValue configValue = jsonToConfigValue(member.second);
                set(member.first, configValue, ConfigSource::File);
            }
            NLM_LOG_INFO("Loaded configuration from JSON file: " + filepath + " (" + 
                        std::to_string((*jsonRoot.object).members.size()) + " entries)");
            return true;
        } else if (jsonRoot.type == JSONValueType::Array) {
            NLM_LOG_WARNING("JSON file contains array, not object. Parsing as array-based config.");
            // For array format, we could process each element
            // For now, treat as single array entry
            ConfigValue configValue = jsonToConfigValue(jsonRoot);
            set("config_array", configValue, ConfigSource::File);
            NLM_LOG_INFO("Loaded configuration from JSON array file: " + filepath);
            return true;
        } else {
            NLM_LOG_WARNING("JSON file does not contain an object. Trying simple format.");
            // Fall back to simple format
        }
    } catch (const std::exception& e) {
        NLM_LOG_WARNING("Failed to parse JSON from " + filepath + ": " + std::string(e.what()) + ". Trying simple format.");
        // Fall back to simple format
    }
    
    // Fallback: Parse as simple key=value format (Phase 1 legacy)
    clear();
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
        } else {
            NLM_LOG_WARNING("Invalid config line in " + filepath + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded configuration from simple file: " + filepath + " (" + 
                std::to_string(pImpl->entries.size()) + " entries)");
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_INFO("Loading configuration from command line arguments");
    
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Parse and validate the value
                try {
                    ConfigValue configValue = parseSimpleValue(value);
                    set(key, configValue, ConfigSource::CommandLine);
                    NLM_LOG_DEBUG("Config from args: " + key + " = " + value);
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Failed to parse config argument " + key + " = " + value + ": " + std::string(e.what()));
                    success = false;
                }
            } else {
                NLM_LOG_WARNING("Invalid config argument format: " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Parse and validate the value
            try {
                ConfigValue configValue = parseSimpleValue(value);
                set(key, configValue, ConfigSource::CommandLine);
                NLM_LOG_DEBUG("Config from args: " + key + " = " + value);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to parse config argument " + key + " = " + value + ": " + std::string(e.what()));
                success = false;
            }
        } else {
            NLM_LOG_WARNING("Invalid config argument format: " + arg);
            success = false;
        }
    }
    
    if (success) {
        NLM_LOG_INFO("Loaded " + std::to_string(pImpl->entries.size()) + " configuration entries from command line");
    } else {
        NLM_LOG_ERROR("Failed to load all configuration entries from command line");
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Generated on " << __DATE__ << " at " << __TIME__ << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Format value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n\n";
            } else if constexpr (std::is_same_v<T, int> || 
                                std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << arg << "\n\n";
            } else if constexpr (std::is_same_v<T, double>) {
                // Format double to avoid scientific notation
                std::ostringstream oss;
                oss.precision(10);
                oss << std::fixed << arg;
                file << entry.key << " = " << oss.str() << "\n\n";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    std::ostringstream oss;
                    oss.precision(10);
                    oss << std::fixed << arg[i];
                    file << oss.str();
                }
                file << "]\n\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]\n\n";
            } else {
                file << entry.key << " = \"UNKNOWN_TYPE\"\n\n";
            }
        }, entry.value);
    }
    
    file << "\n# End of configuration file\n";
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
