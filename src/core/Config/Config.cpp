#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Load configuration from JSON or YAML file
    // Supports both JSON (object syntax) and simple key=value (backward compatibility)
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file content
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    if (content.empty()) {
        return false;
    }
    
    try {
        // Try to parse as JSON first
        if (parseJSON(content)) {
            return true;
        }
        
        // If JSON parsing fails, try simple key=value format for backward compatibility
        return parseKeyValue(content);
    } catch (const std::exception& e) {
        NLM_LOG_WARNING("Failed to parse config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::parseJSON(const std::string& content) {
    // Simple JSON parser that supports basic objects, arrays, and values
    // This is a simplified implementation for configuration files
    
    size_t pos = 0;
    skipWhitespace(content, pos);
    
    if (content[pos] != '{') {
        return false; // Not a JSON object
    }
    
    pos++; // Skip {
    skipWhitespace(content, pos);
    
    while (pos < content.size()) {
        skipWhitespace(content, pos);
        
        if (content[pos] == '}') {
            pos++; // Skip }
            break;
        }
        
        // Parse key
        std::string key = parseString(content, pos);
        if (key.empty()) {
            return false;
        }
        
        skipWhitespace(content, pos);
        if (pos >= content.size() || content[pos] != ':') {
            return false;
        }
        pos++; // Skip :
        skipWhitespace(content, pos);
        
        // Parse value
        ConfigValue value = parseValue(content, pos);
        
        // Add to configuration
        set(key, value, ConfigSource::File);
        
        skipWhitespace(content, pos);
        if (pos < content.size() && content[pos] == ',') {
            pos++; // Skip ,
        }
    }
    
    return true;
}

bool Config::parseKeyValue(const std::string& content) {
    // Parse simple key=value format for backward compatibility
    // Each line is in format: key = value
    
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
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

ConfigValue Config::parseValue(const std::string& content, size_t& pos) {
    skipWhitespace(content, pos);
    
    if (pos >= content.size()) {
        throw std::runtime_error("Unexpected end of content while parsing value");
    }
    
    switch (content[pos]) {
        case '{': {
            // Parse object
            pos++; // Skip {
            std::map<std::string, ConfigValue> obj;
            skipWhitespace(content, pos);
            
            while (pos < content.size() && content[pos] != '}') {
                std::string key = parseString(content, pos);
                skipWhitespace(content, pos);
                if (pos >= content.size() || content[pos] != ':') {
                    throw std::runtime_error("Expected ':' after object key");
                }
                pos++; // Skip :
                skipWhitespace(content, pos);
                
                ConfigValue value = parseValue(content, pos);
                obj[key] = value;
                
                skipWhitespace(content, pos);
                if (pos < content.size() && content[pos] == ',') {
                    pos++; // Skip ,
                }
                skipWhitespace(content, pos);
            }
            
            if (pos >= content.size() || content[pos] != '}') {
                throw std::runtime_error("Expected '}' to close object");
            }
            pos++; // Skip }
            
            // Convert map to string representation for now
            // TODO: Implement proper object serialization
            std::ostringstream oss;
            oss << "{";
            bool first = true;
            for (const auto& pair : obj) {
                if (!first) oss << ", ";
                oss << "\"" << pair.first << "\": " << std::visit([](auto&& arg) {
                    if constexpr (std::is_same_v<decltype(arg), std::string>) {
                        return "\"" + arg + "\"";
                    } else {
                        return std::to_string(arg);
                    }
                }, pair.second);
                first = false;
            }
            oss << "}";
            return objToConfigValue(obj);
        }
        
        case '[': {
            // Parse array
            pos++; // Skip [
            std::vector<ConfigValue> arr;
            skipWhitespace(content, pos);
            
            while (pos < content.size() && content[pos] != ']') {
                ConfigValue value = parseValue(content, pos);
                arr.push_back(value);
                
                skipWhitespace(content, pos);
                if (pos < content.size() && content[pos] == ',') {
                    pos++; // Skip ,
                }
                skipWhitespace(content, pos);
            }
            
            if (pos >= content.size() || content[pos] != ']') {
                throw std::runtime_error("Expected ']' to close array");
            }
            pos++; // Skip ]
            
            // Convert array to appropriate ConfigValue type
            if (!arr.empty()) {
                // Determine type based on first element
                if (std::holds_alternative<int>(arr[0])) {
                    std::vector<int> intArr;
                    for (const auto& val : arr) {
                        if (std::holds_alternative<int>(val)) {
                            intArr.push_back(std::get<int>(val));
                        }
                    }
                    return intArr;
                } else if (std::holds_alternative<double>(val)) {
                    std::vector<double> doubleArr;
                    for (const auto& val : arr) {
                        if (std::holds_alternative<double>(val)) {
                            doubleArr.push_back(std::get<double>(val));
                        }
                    }
                    return doubleArr;
                } else if (std::holds_alternative<std::string>(val)) {
                    std::vector<std::string> stringArr;
                    for (const auto& val : arr) {
                        if (std::holds_alternative<std::string>(val)) {
                            stringArr.push_back(std::get<std::string>(val));
                        }
                    }
                    return stringArr;
                }
            }
            
            // Fallback to generic vector
            return arr;
        }
        
        case '"':
        case '\'':
            return parseString(content, pos);
            
        default:
            if (std::isdigit(content[pos]) || content[pos] == '-') {
                // Parse number
                size_t endPos;
                double num = parseNumber(content, pos, endPos);
                pos = endPos;
                return num;
            } else if (content.substr(pos, 4) == "true") {
                pos += 4;
                return true;
            } else if (content.substr(pos, 5) == "false") {
                pos += 5;
                return false;
            } else if (content.substr(pos, 4) == "null") {
                pos += 4;
                return std::string("null");
            } else {
                throw std::runtime_error("Unexpected character while parsing value: " + content.substr(pos, 1));
            }
    }
}

std::string Config::parseString(const std::string& content, size_t& pos) {
    skipWhitespace(content, pos);
    
    if (pos >= content.size()) {
        throw std::runtime_error("Unexpected end of content while parsing string");
    }
    
    char quote = content[pos];
    if (quote != '"' && quote != '\'') {
        throw std::runtime_error("Expected string quote");
    }
    
    pos++; // Skip opening quote
    std::string result;
    
    while (pos < content.size()) {
        if (content[pos] == '\\') {
            // Handle escape sequences
            pos++; // Skip backslash
            if (pos >= content.size()) {
                throw std::runtime_error("Unexpected end of content after escape character");
            }
            
            switch (content[pos]) {
                case '"': result += '"'; break;
                case '\'': result += '\''; break;
                case '\\': result += '\\'; break;
                case 'n': result += '\n'; break;
                case 't': result += '\t'; break;
                case 'r': result += '\r'; break;
                default: result += content[pos]; break;
            }
            pos++;
        } else if (content[pos] == quote) {
            pos++; // Skip closing quote
            return result;
        } else {
            result += content[pos];
            pos++;
        }
    }
    
    throw std::runtime_error("Unexpected end of content while parsing string");
}

double Config::parseNumber(const std::string& content, size_t& pos, size_t& endPos) {
    size_t startPos = pos;
    
    if (content[pos] == '-') {
        pos++; // Skip minus sign
    }
    
    while (pos < content.size() && std::isdigit(content[pos])) {
        pos++;
    }
    
    if (pos < content.size() && content[pos] == '.') {
        pos++; // Skip decimal point
        while (pos < content.size() && std::isdigit(content[pos])) {
            pos++;
        }
    }
    
    std::string numStr = content.substr(startPos, pos - startPos);
    endPos = pos;
    
    try {
        return std::stod(numStr);
    } catch (const std::exception&) {
        throw std::runtime_error("Failed to parse number: " + numStr);
    }
}

void Config::skipWhitespace(const std::string& content, size_t& pos) {
    while (pos < content.size() && std::isspace(content[pos])) {
        pos++;
    }
}

ConfigValue Config::objToConfigValue(const std::map<std::string, ConfigValue>& obj) {
    // Convert map to string representation
    std::ostringstream oss;
    oss << "{";
    bool first = true;
    for (const auto& pair : obj) {
        if (!first) oss << ", ";
        oss << "\"" << pair.first << "\": ";
        oss << std::visit([](auto&& arg) {
            if constexpr (std::is_same_v<decltype(arg), std::string>) {
                return "\"" + arg + "\"";
            } else if constexpr (std::is_same_v<decltype(arg), bool>) {
                return arg ? "true" : "false";
            } else {
                return std::to_string(arg);
            }
        }, pair.second);
        first = false;
    }
    oss << "}";
    return oss.str();
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
    
    // Write JSON configuration
    file << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Save the actual value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << arg[i];
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    file << "\"" << escapeString(arg[i]) << "\"";
                    if (i < arg.size() - 1) file << ", ";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else {
                file << arg;
            }
        }, entry.value);
    }
    
    file << "\n}\n";
    
    return true;
}

std::string Config::escapeString(const std::string& str) {
    std::ostringstream oss;
    for (char c : str) {
        switch (c) {
            case '\\': oss << "\\\\"; break;
            case '\"': oss << "\\\""; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default: oss << c; break;
        }
    }
    return oss.str();
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
