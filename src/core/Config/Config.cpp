#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <variant>
#include <cstdint>
#include <type_traits>

namespace nlm {

// Helper function to check if variant has specific type
template<typename T, typename V>
bool holdsAlternative(const V& v) {
    return std::holds_alternative<T>(v);
}

// Helper function to check if variant has specific type
template<typename T, typename V>
bool holdsAlternative(const V& v) {
    return std::holds_alternative<T>(v);
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Implement proper JSON parser
    // Also support key=value format for backward compatibility
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Try to parse as JSON first
    if (parseJSON(content)) {
        return true;
    }
    
    // Fall back to key=value format for backward compatibility
    std::string line;
    std::istringstream stream(content);
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == '{') {
            continue;
        }
        
        // Parse key=value pairs
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

bool Config::parseJSON(const std::string& jsonStr) {
    // Simple JSON parser that supports basic objects
    // This is a minimal implementation focused on NLM configuration needs
    
    std::string::size_type pos = 0;
    skipWhitespace(jsonStr, pos);
    
    if (jsonStr[pos] == '{') {
        pos++; // Skip opening brace
        skipWhitespace(jsonStr, pos);
        
        // Parse key-value pairs
        while (pos < jsonStr.size() && jsonStr[pos] != '}') {
            std::string key = parseString(jsonStr, pos);
            skipWhitespace(jsonStr, pos);
            
            if (jsonStr[pos] != ':') {
                return false; // Expected colon
            }
            pos++; // Skip colon
            skipWhitespace(jsonStr, pos);
            
            // Parse value
            ConfigValue value = parseValue(jsonStr, pos);
            if (value.index() == std::variant_npos) {
                return false;
            }
            
            set(key, value, ConfigSource::File);
            
            skipWhitespace(jsonStr, pos);
            if (jsonStr[pos] == ',') {
                pos++; // Skip comma
                skipWhitespace(jsonStr, pos);
            }
        }
        
        if (pos >= jsonStr.size() || jsonStr[pos] != '}') {
            return false; // Missing closing brace
        }
        pos++; // Skip closing brace
        return true;
    }
    
    return false; // Not a JSON object
}

ConfigValue Config::parseValue(const std::string& jsonStr, std::string::size_type& pos) {
    skipWhitespace(jsonStr, pos);
    
    if (pos >= jsonStr.size()) {
        return ConfigValue(); // Empty
    }
    
    char ch = jsonStr[pos];
    
    if (ch == '"') {
        return parseString(jsonStr, pos);
    } else if (ch == '{') {
        // Object - not parsing nested objects for now
        return parseString(jsonStr, pos); // fallback to treating as string
    } else if (ch == '[') {
        // Array - not parsing arrays for now
        return parseString(jsonStr, pos); // fallback to treating as string
    } else if (ch == 't' || ch == 'f') {
        // Boolean
        if (jsonStr.compare(pos, 4, "true") == 0) {
            pos += 4;
            return true;
        } else if (jsonStr.compare(pos, 5, "false") == 0) {
            pos += 5;
            return false;
        }
    } else if (ch == 'n') {
        // Null
        if (jsonStr.compare(pos, 4, "null") == 0) {
            pos += 4;
            return std::string(); // Empty string for null
        }
    } else if (ch == '-' || (ch >= '0' && ch <= '9')) {
        // Number
        return parseNumber(jsonStr, pos);
    }
    
    return ConfigValue(); // Error
}

std::string Config::parseString(const std::string& jsonStr, std::string::size_type& pos) {
    if (pos >= jsonStr.size() || jsonStr[pos] != '"') {
        return ""; // Not a string
    }
    
    pos++; // Skip opening quote
    std::string result;
    
    while (pos < jsonStr.size() && jsonStr[pos] != '"') {
        if (jsonStr[pos] == '\\') {
            pos++; // Skip escape character
            if (pos < jsonStr.size()) {
                result += jsonStr[pos];
                pos++;
            }
        } else {
            result += jsonStr[pos];
            pos++;
        }
    }
    
    if (pos >= jsonStr.size() || jsonStr[pos] != '"') {
        return ""; // Unterminated string
    }
    
    pos++; // Skip closing quote
    return result;
}

ConfigValue Config::parseNumber(const std::string& jsonStr, std::string::size_type& pos) {
    std::string numberStr;
    bool isFloat = false;
    
    while (pos < jsonStr.size() && (jsonStr[pos] == '-' || jsonStr[pos] == '+' ||
                                   (jsonStr[pos] >= '0' && jsonStr[pos] <= '9') ||
                                   jsonStr[pos] == '.')) {
        if (jsonStr[pos] == '.') {
            isFloat = true;
        }
        numberStr += jsonStr[pos];
        pos++;
    }
    
    if (isFloat) {
        try {
            double value = std::stod(numberStr);
            return value;
        } catch (...) {
            return 0.0;
        }
    } else {
        try {
            // Try int first, then int64_t
            try {
                int value = std::stoi(numberStr);
                return value;
            } catch (...) {
                int64_t value = std::stoll(numberStr);
                return value;
            }
        } catch (...) {
            return 0;
        }
    }
}

void Config::skipWhitespace(const std::string& jsonStr, std::string::size_type& pos) {
    while (pos < jsonStr.size() && (jsonStr[pos] == ' ' || jsonStr[pos] == '\t' ||
                                   jsonStr[pos] == '\n' || jsonStr[pos] == '\r')) {
        pos++;
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
    
    // Write in JSON format for better interoperability
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Format the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeString(arg) << "\"";
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
                    file << "\"" << escapeString(arg[i]) << "\"";
                }
                file << "]";
            }
        }, entry.value);
        
        if (!entry.description.empty()) {
            file << " // " << entry.description;
        }
    }
    
    file << "\n}\n";
    
    return true;
}

std::string Config::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 32 || c > 126) {
                    // Non-printable ASCII
                    result += "\\u";
                    result += toHex(static_cast<unsigned int>(c));
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}

std::string Config::toHex(unsigned int value) {
    std::stringstream stream;
    stream << std::hex << std::setw(4) << std::setfill('0') << value;
    return stream.str();
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
// Apply settings from a key=value line
bool Config::loadFromLine(const std::string& line) {
    std::string lineCopy = line;
    std::string::size_type pos = 0;
    
    // Skip leading whitespace
    while (pos < lineCopy.size() && (lineCopy[pos] == ' ' || lineCopy[pos] == '\t')) {
        pos++;
    }
    
    if (pos >= lineCopy.size()) {
        return false; // Empty line
    }
    
    // Check for comment
    if (lineCopy[pos] == '#' || lineCopy[pos] == '/') {
        return false; // Comment line
    }
    
    // Find key
    std::string::size_type keyStart = pos;
    while (pos < lineCopy.size() && lineCopy[pos] != '=' && lineCopy[pos] != ' ' && lineCopy[pos] != '\t') {
        pos++;
    }
    
    if (pos >= lineCopy.size() || lineCopy[pos] != '=') {
        return false; // No equals sign
    }
    
    std::string key = lineCopy.substr(keyStart, pos - keyStart);
    
    // Skip equals sign and whitespace
    pos++; // Skip =
    while (pos < lineCopy.size() && (lineCopy[pos] == ' ' || lineCopy[pos] == '\t')) {
        pos++;
    }
    
    // Find end of value (end of line or comment)
    std::string::size_type valueStart = pos;
    while (pos < lineCopy.size() && lineCopy[pos] != ' ' && lineCopy[pos] != '\t' && 
           lineCopy[pos] != '\n' && lineCopy[pos] != '\r') {
        pos++;
    }
    
    std::string value = lineCopy.substr(valueStart, pos - valueStart);
    
    // Remove surrounding quotes
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    // Parse value and set
    ConfigValue parsedValue = parseValue(value);
    set(key, parsedValue, ConfigSource::File);
    
    return true;
}

// Parse a value from string (key=value format)
ConfigValue Config::parseValue(const std::string& str) {
    if (str.empty()) {
        return ConfigValue();
    }
    
    // Try boolean
    if (str == "true" || str == "false") {
        bool value = (str == "true");
        return value;
    }
    
    // Try integer
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            return intVal;
        }
    } catch (...) {}
    
    try {
        size_t pos;
        int64_t int64Val = std::stoll(str, &pos);
        if (pos == str.size()) {
            return int64Val;
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // String
    return str;
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
