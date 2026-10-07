#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <stack>
#include <unordered_map>
#include <stdexcept>

// Simple JSON/YAML parser implementation
namespace nlm {
namespace json {

class JSONParser {
public:
    explicit JSONParser(const std::string& content) : content(content), pos(0) {}
    
    ConfigValue parseValue() {
        skipWhitespace();
        char c = peek();
        if (c == '{') {
            return parseObject();
        } else if (c == '[') {
            return parseArray();
        } else if (c == '"') {
            return parseString();
        } else if (c == '-' || std::isdigit(c)) {
            return parseNumber();
        } else if (c == 't' || c == 'f') {
            return parseBoolean();
        } else if (c == 'n') {
            return parseNull();
        }
        throw std::runtime_error("Unexpected character in JSON: " + std::string(1, c));
    }
    
    std::unordered_map<std::string, ConfigValue> parseObject() {
        std::unordered_map<std::string, ConfigValue> result;
        expect('{');
        skipWhitespace();
        
        while (peek() != '}') {
            std::string key = parseString();
            skipWhitespace();
            expect(':');
            ConfigValue value = parseValue();
            result[key] = value;
            skipWhitespace();
            if (peek() == ',') {
                advance();
                skipWhitespace();
            }
        }
        expect('}');
        return result;
    }
    
    std::vector<ConfigValue> parseArray() {
        std::vector<ConfigValue> result;
        expect('[');
        skipWhitespace();
        
        while (peek() != ']') {
            ConfigValue value = parseValue();
            result.push_back(value);
            skipWhitespace();
            if (peek() == ',') {
                advance();
                skipWhitespace();
            }
        }
        expect(']');
        return result;
    }
    
    std::string parseString() {
        expect('"');
        std::string result;
        while (peek() != '"') {
            if (pos >= content.size()) {
                throw std::runtime_error("Unterminated string literal");
            }
            if (peek() == '\\') {
                advance(); // skip backslash
                if (pos >= content.size()) {
                    throw std::runtime_error("Unexpected end of string after escape");
                }
                char escaped = content[pos];
                switch (escaped) {
                    case '\"': result += '\"'; break;
                    case '\\': result += '\\'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    case 'u': {
                        // Unicode escape - simplified
                        advance(); advance(); advance(); advance();
                        break;
                    }
                    default:
                        result += escaped;
                }
            } else {
                result += content[pos];
            }
            advance();
        }
        expect('"');
        return result;
    }
    
    ConfigValue parseNumber() {
        size_t start = pos;
        bool isDouble = false;
        bool isNegative = false;
        
        if (peek() == '-') {
            isNegative = true;
            advance();
        }
        
        while (pos < content.size() && (std::isdigit(content[pos]) || content[pos] == '.' || 
                                        content[pos] == 'e' || content[pos] == 'E')) {
            if (content[pos] == '.') {
                isDouble = true;
            }
            advance();
        }
        
        std::string numStr = content.substr(start, pos - start);
        
        if (isDouble || content.find('.', start) != std::string::npos) {
            double value = std::stod(numStr);
            return isNegative ? -value : value;
        } else {
            // Try int64 first, then int
            try {
                int64_t value = std::stoll(numStr);
                return isNegative ? -value : value;
            } catch (...) {
                int value = std::stoi(numStr);
                return isNegative ? -value : value;
            }
        }
    }
    
    ConfigValue parseBoolean() {
        if (content.substr(pos, 4) == "true") {
            advance(4);
            return true;
        } else if (content.substr(pos, 5) == "false") {
            advance(5);
            return false;
        }
        throw std::runtime_error("Invalid JSON boolean");
    }
    
    ConfigValue parseNull() {
        if (content.substr(pos, 4) == "null") {
            advance(4);
            return std::monostate{}; // Use std::monostate as null placeholder
        }
        throw std::runtime_error("Invalid JSON null");
    }
    
private:
    void skipWhitespace() {
        while (pos < content.size() && std::isspace(content[pos])) {
            advance();
        }
    }
    
    char peek() {
        if (pos < content.size()) {
            return content[pos];
        }
        return '\0';
    }
    
    void advance() {
        pos++;
    }
    
    void advance(int count) {
        pos += count;
    }
    
    void expect(char c) {
        if (peek() != c) {
            throw std::runtime_error(std::string("Expected character '") + c + "', found '" + peek() + "'");
        }
        advance();
    }
    
    std::string content;
    size_t pos;
};

} // namespace json
} // namespace nlm

// Helper function to convert JSON object to Config format
static void jsonObjectToConfig(const std::unordered_map<std::string, nlm::json::ConfigValue>& jsonObj, nlm::Config& cfg) {
    for (const auto& pair : jsonObj) {
        const std::string& key = pair.first;
        const nlm::json::ConfigValue& jsonValue = pair.second;
        
        // Convert JSON value to ConfigValue based on type
        if (std::holds_alternative<int>(jsonValue)) {
            cfg.set(key, std::get<int>(jsonValue));
        } else if (std::holds_alternative<int64_t>(jsonValue)) {
            cfg.set(key, std::get<int64_t>(jsonValue));
        } else if (std::holds_alternative<double>(jsonValue)) {
            cfg.set(key, std::get<double>(jsonValue));
        } else if (std::holds_alternative<bool>(jsonValue)) {
            cfg.set(key, std::get<bool>(jsonValue));
        } else if (std::holds_alternative<std::string>(jsonValue)) {
            cfg.set(key, std::get<std::string>(jsonValue));
        } else if (std::holds_alternative<std::vector<int>>(jsonValue)) {
            cfg.set(key, std::get<std::vector<int>>(jsonValue));
        } else if (std::holds_alternative<std::vector<double>>(jsonValue)) {
            cfg.set(key, std::get<std::vector<double>>(jsonValue));
        } else if (std::holds_alternative<std::vector<std::string>>(jsonValue)) {
            cfg.set(key, std::get<std::vector<std::string>>(jsonValue));
        } else if (std::holds_alternative<std::monostate>(jsonValue)) {
            // Skip null values
            continue;
        }
    }
}

// Helper function to parse simple key=value format (for backward compatibility)
static bool parseSimpleFormat(const std::string& line, std::string& key, std::string& value) {
    size_t pos = line.find('=');
    if (pos != std::string::npos) {
        key = nlm::Config::trim(line.substr(0, pos));
        value = nlm::Config::trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        return true;
    }
    return false;
}

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    
    try {
        json::JSONParser parser(content);
        nlm::json::ConfigValue jsonValue = parser.parseValue();
        
        if (std::holds_alternative<std::unordered_map<std::string, nlm::json::ConfigValue>>(jsonValue)) {
            auto jsonObj = std::get<std::unordered_map<std::string, nlm::json::ConfigValue>>(jsonValue);
            jsonObjectToConfig(jsonObj, *this);
        } else if (std::holds_alternative<std::monostate>(jsonValue)) {
            // Empty/null JSON, nothing to parse
            return true;
        } else {
            // JSON contains a single value at root level
            throw std::runtime_error("JSON file must contain an object at root level");
        }
    } catch (const std::runtime_error& e) {
        // Fallback to simple key=value format for backward compatibility
        file.clear();
        file.seekg(0, std::ios::beg);
        
        std::string line;
        while (std::getline(file, line)) {
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            std::string key, value;
            if (parseSimpleFormat(line, key, value)) {
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
                file << "\"" << escapeString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                if (!arg.empty()) {
                    if constexpr (std::is_same_v<T, std::vector<int>>) {
                        for (size_t j = 0; j < arg.size(); ++j) {
                            if (j > 0) file << ", ";
                            file << arg[j];
                        }
                    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                        for (size_t j = 0; j < arg.size(); ++j) {
                            if (j > 0) file << ", ";
                            file << arg[j];
                        }
                    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        for (size_t j = 0; j < arg.size(); ++j) {
                            if (j > 0) file << ", ";
                            file << "\"" << escapeString(arg[j]) << "\"";
                        }
                    }
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    file << "}\n";
    
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
// Escape special characters in JSON strings
    static std::string escapeString(const std::string& str) {
        std::string result;
        result.reserve(str.size());
        
        for (char c : str) {
            switch (c) {
                case '\"': result += "\\\""; break;
                case '\\': result += "\\\\"; break;
                case '\b': result += "\\b"; break;
                case '\f': result += "\\f"; break;
                case '\n': result += "\\n"; break;
                case '\r': result += "\\r"; break;
                case '\t': result += "\\t"; break;
                default:
                    if (c < 32 || c == 127) {
                        // Control characters - use escape
                        char buf[7];
                        snprintf(buf, sizeof(buf), "\\u%04x", static_cast<unsigned char>(c));
                        result += buf;
                    } else {
                        result += c;
                    }
                    break;
            }
        }
        
        return result;
    }
}
