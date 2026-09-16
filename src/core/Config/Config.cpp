#include <sstream>
#include <unordered_map>
#include <cctype>
#include <cstring>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// JSON parsing implementation for Config.cpp

namespace {
    // Internal exception type for JSON parsing errors
    class JSONParseError : public std::runtime_error {
    public:
        JSONParseError(const std::string& msg, const std::string& context = "", size_t pos = 0)
            : std::runtime_error(msg), context_(context), position_(pos) {}
        
        const std::string& context() const { return context_; }
        size_t position() const { return position_; }
    
    private:
        std::string context_;
        size_t position_;
    };
    
    // Skip whitespace and comments
    void skipWhitespace(const std::string& str, size_t& pos) {
        while (pos < str.size()) {
            if (std::isspace(str[pos])) {
                pos++;
            } else if (str[pos] == '/' && pos + 1 < str.size() && str[pos + 1] == '/') {
                // Line comment
                pos += 2;
                while (pos < str.size() && str[pos] != '\n') {
                    pos++;
                }
            } else if (str[pos] == '/' && pos + 1 < str.size() && str[pos + 1] == '*') {
                // Block comment
                pos += 2;
                while (pos < str.size() - 1) {
                    if (str[pos] == '*' && str[pos + 1] == '/') {
                        pos += 2;
                        break;
                    }
                    pos++;
                }
            } else {
                break;
            }
        }
    }
    
    // Parse JSON string, handling escape sequences
    std::string parseString(const std::string& str, size_t& pos) {
        if (pos >= str.size() || str[pos] != '"') {
            throw JSONParseError("Expected '\"' at start of string", str, pos);
        }
        
        pos++;
        std::string result;
        
        while (pos < str.size()) {
            if (str[pos] == '"') {
                pos++;
                return result;
            } else if (str[pos] == '\\') {
                pos++;
                if (pos >= str.size()) {
                    throw JSONParseError("Invalid escape sequence", str, pos - 1);
                }
                
                switch (str[pos]) {
                    case '\\': result += '\\'; break;
                    case '"': result += '"'; break;
                    case '/': result += '/'; break;
                    case 'b': result += '\b'; break;
                    case 'f': result += '\f'; break;
                    case 'n': result += '\n'; break;
                    case 'r': result += '\r'; break;
                    case 't': result += '\t'; break;
                    case 'u':
                        // Unicode escape sequence - simplified for basic support
                        if (pos + 4 >= str.size()) {
                            throw JSONParseError("Invalid Unicode escape sequence", str, pos - 1);
                        }
                        // For now, just add the characters as-is
                        // TODO: Implement proper Unicode decoding
                        for (int i = 0; i < 4; i++) {
                            result += str[pos + i];
                        }
                        pos += 4;
                        break;
                    default:
                        result += str[pos];
                        break;
                }
            } else {
                result += str[pos];
            }
            pos++;
        }
        
        throw JSONParseError("Unterminated string literal", str, pos);
    }
    
    // Parse JSON number (int or double)
    ConfigValue parseNumber(const std::string& str, size_t& pos) {
        size_t start = pos;
        
        if (str[pos] == '-') {
            pos++;
        }
        
        if (pos < str.size() && str[pos] == '0') {
            pos++;
            if (pos < str.size() && std::isdigit(str[pos])) {
                throw JSONParseError("Invalid number format - leading zero", str, start);
            }
        } else {
            while (pos < str.size() && std::isdigit(str[pos])) {
                pos++;
            }
        }
        
        bool hasDecimal = false;
        if (pos < str.size() && str[pos] == '.') {
            hasDecimal = true;
            pos++;
            while (pos < str.size() && std::isdigit(str[pos])) {
                pos++;
            }
        }
        
        bool hasExponent = false;
        if (pos < str.size() && (str[pos] == 'e' || str[pos] == 'E')) {
            hasExponent = true;
            pos++;
            if (pos < str.size() && (str[pos] == '+' || str[pos] == '-')) {
                pos++;
            }
            if (pos >= str.size() || !std::isdigit(str[pos])) {
                throw JSONParseError("Invalid exponent in number", str, pos - 1);
            }
            while (pos < str.size() && std::isdigit(str[pos])) {
                pos++;
            }
        }
        
        if (hasDecimal || hasExponent) {
            // Parse as double
            size_t end = pos;
            double value = std::stod(str.substr(start, end - start));
            return ConfigValue(value);
        } else {
            // Try to parse as int first, fallback to int64_t
            size_t end = pos;
            std::string numStr = str.substr(start, end - start);
            
            // Check for overflow
            if (numStr.size() > 19 || (numStr.size() == 19 && numStr > "9223372036854775807")) {
                // Too large for int64_t, treat as double
                try {
                    double value = std::stod(numStr);
                    return ConfigValue(value);
                } catch (...) {
                    throw JSONParseError("Number out of range for double", str, start);
                }
            }
            
            try {
                int64_t value = std::stoll(numStr);
                return ConfigValue(value);
            } catch (...) {
                // Fallback to double
                double value = std::stod(numStr);
                return ConfigValue(value);
            }
        }
    }
    
    // Parse JSON boolean value
    ConfigValue parseBoolean(const std::string& str, size_t& pos) {
        if (str.substr(pos, 4) == "true") {
            pos += 4;
            return ConfigValue(true);
        } else if (str.substr(pos, 5) == "false") {
            pos += 5;
            return ConfigValue(false);
        } else {
            throw JSONParseError("Expected 'true' or 'false' for boolean", str, pos);
        }
    }
    
    // Parse JSON null value
    ConfigValue parseNull(const std::string& str, size_t& pos) {
        if (str.substr(pos, 4) == "null") {
            pos += 4;
            // Return empty variant (null)
            return ConfigValue();
        } else {
            throw JSONParseError("Expected 'null' for null value", str, pos);
        }
    }
    
    // Parse JSON array
    std::vector<ConfigValue> parseArray(const std::string& str, size_t& pos) {
        if (pos >= str.size() || str[pos] != '[') {
            throw JSONParseError("Expected '[' for array start", str, pos);
        }
        
        pos++;
        std::vector<ConfigValue> result;
        
        skipWhitespace(str, pos);
        
        if (str[pos] == ']') {
            pos++;
            return result;
        }
        
        while (true) {
            skipWhitespace(str, pos);
            
            ConfigValue value = parseValue(str, pos);
            result.push_back(std::move(value));
            
            skipWhitespace(str, pos);
            
            if (pos >= str.size()) {
                throw JSONParseError("Unexpected end of input in array", str, pos);
            }
            
            if (str[pos] == ']') {
                pos++;
                break;
            } else if (str[pos] == ',') {
                pos++;
            } else {
                throw JSONParseError("Expected ',' or ']' after array element", str, pos);
            }
        }
        
        return result;
    }
    
    // Parse JSON object
    std::unordered_map<std::string, ConfigValue> parseObject(const std::string& str, size_t& pos) {
        if (pos >= str.size() || str[pos] != '{') {
            throw JSONParseError("Expected '{' for object start", str, pos);
        }
        
        pos++;
        std::unordered_map<std::string, ConfigValue> result;
        
        skipWhitespace(str, pos);
        
        if (str[pos] == '}') {
            pos++;
            return result;
        }
        
        while (true) {
            skipWhitespace(str, pos);
            
            if (str[pos] != '"') {
                throw JSONParseError("Expected string key at object start", str, pos);
            }
            
            std::string key = parseString(str, pos);
            
            skipWhitespace(str, pos);
            
            if (pos >= str.size() || str[pos] != ':') {
                throw JSONParseError("Expected ':' after object key", str, pos);
            }
            
            pos++;
            skipWhitespace(str, pos);
            
            ConfigValue value = parseValue(str, pos);
            result[key] = std::move(value);
            
            skipWhitespace(str, pos);
            
            if (pos >= str.size()) {
                throw JSONParseError("Unexpected end of input in object", str, pos);
            }
            
            if (str[pos] == '}') {
                pos++;
                break;
            } else if (str[pos] == ',') {
                pos++;
            } else {
                throw JSONParseError("Expected ',' or '}' after object member", str, pos);
            }
        }
        
        return result;
    }
    
    // Parse any JSON value
    ConfigValue parseValue(const std::string& str, size_t& pos) {
        skipWhitespace(str, pos);
        
        if (pos >= str.size()) {
            throw JSONParseError("Unexpected end of input", str, pos);
        }
        
        switch (str[pos]) {
            case '"':
                return parseString(str, pos);
            case '{':
                return parseObject(str, pos);
            case '[':
                return parseArray(str, pos);
            case '-':
            case '0': case '1': case '2': case '3': case '4': case '5':
            case '6': case '7': case '8': case '9':
                return parseNumber(str, pos);
            case 't': case 'f':
                return parseBoolean(str, pos);
            case 'n':
                return parseNull(str, pos);
            default:
                throw JSONParseError("Unexpected character in JSON value", str, pos);
        }
    }
    
    // Handle simple key=value format for backward compatibility
    bool parseSimpleFormat(const std::string& content) {
        // Parse lines as key=value pairs (legacy format)
        std::istringstream stream(content);
        std::string line;
        bool result = true;
        
        while (std::getline(stream, line)) {
            // Skip empty lines and comments
            line = Config::trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse simple key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = Config::trim(line.substr(0, pos));
                std::string value = Config::trim(line.substr(pos + 1));
                
                // Try to interpret value as different types
                // Try boolean first (case-insensitive)
                std::string lowerValue = Config::toLower(value);
                if (lowerValue == "true") {
                    set(key, true, ConfigSource::File);
                } else if (lowerValue == "false") {
                    set(key, false, ConfigSource::File);
                } else {
                    // Try integer
                    try {
                        size_t endpos;
                        int64_t intValue = std::stoll(value, &endpos);
                        if (endpos == value.size()) {
                            set(key, intValue, ConfigSource::File);
                            continue;
                        }
                    } catch (...) {}
                    
                    // Try double
                    try {
                        size_t endpos;
                        double doubleValue = std::stod(value, &endpos);
                        if (endpos == value.size()) {
                            set(key, doubleValue, ConfigSource::File);
                            continue;
                        }
                    } catch (...) {}
                    
                    // Try string
                    if (value.size() >= 2 && 
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    set(key, value, ConfigSource::File);
                }
            }
        }
        
        return result;
    }
    
    // Process parsed object into Config
    void processParsedObject(const std::unordered_map<std::string, ConfigValue>& obj, ConfigSource source) {
        for (const auto& pair : obj) {
            // Recursively process nested objects
            if (std::holds_alternative<std::vector<ConfigValue>>(pair.second) ||
                std::holds_alternative<std::vector<int>>(pair.second) ||
                std::holds_alternative<std::vector<double>>(pair.second) ||
                std::holds_alternative<std::vector<std::string>>(pair.second)) {
                // Handle arrays - for now, just store as-is
                // TODO: Flatten array values or store as JSON strings
            }
            
            // For nested objects, we could prefix keys
            set(pair.first, pair.second, source);
        }
    }
} // anonymous namespace

bool Config::parseJSON(const std::string& content) {
    try {
        size_t pos = 0;
        
        // Try to parse as JSON first
        skipWhitespace(content, pos);
        
        if (pos >= content.size()) {
            // Empty file
            return true;
        }
        
        ConfigValue value = parseValue(content, pos);
        
        skipWhitespace(content, pos);
        
        if (pos != content.size()) {
            // There are extra characters after the JSON value
            // Try simple format instead
            return parseSimpleFormat(content);
        }
        
        // Successfully parsed JSON
        // Now process the value
        if (std::holds_alternative<std::unordered_map<std::string, ConfigValue>>(value)) {
            processParsedObject(
                std::get<std::unordered_map<std::string, ConfigValue>>(std::move(value)),
                ConfigSource::File
            );
        } else if (std::holds_alternative<std::vector<ConfigValue>>(value)) {
            // Top-level array - for now, parse as simple format
            return parseSimpleFormat(content);
        } else {
            // Simple value - parse as simple format
            return parseSimpleFormat(content);
        }
        
        return true;
        
    } catch (const JSONParseError& e) {
        // Parse error - fall back to simple format
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        if (!e.context().empty()) {
            std::cerr << " at position " << e.position() << ": " << e.context() << std::endl;
        }
        return parseSimpleFormat(content);
    } catch (const std::exception& e) {
        // Other error - fall back to simple format
        std::cerr << "Error parsing JSON: " << e.what() << std::endl;
        return parseSimpleFormat(content);
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
