#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>
#include <map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

    bool loadFromFile(const std::string& filepath) {
        // TODO PHASE 2: Implement proper JSON/YAML parser
        // PLACEHOLDER - Phase 1 uses a simple key=value format
        
        // Check file extension for format detection
        std::string ext = "." + filepath.substr(filepath.find_last_of('.') + 1);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == ".json") {
            return loadFromJSON(filepath);
        } else if (ext == ".yaml" || ext == ".yml") {
            return loadFromYAML(filepath);
        } else {
            // Default to simple key=value format
            return loadFromSimpleFormat(filepath);
        }
    }
    
    bool loadFromSimpleFormat(const std::string& filepath) {
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
    
    bool loadFromJSON(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Read entire file
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        try {
            // Parse JSON content
            return parseJSON(content);
        } catch (const std::exception& e) {
            // If JSON parsing fails, fall back to simple format
            file.clear();
            file.seekg(0);
            return loadFromSimpleFormat(filepath);
        }
    }
    
    bool loadFromYAML(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // For now, treat YAML as equivalent to JSON for basic parsing
        // Full YAML support would require a more complex implementation
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string content = buffer.str();
        
        // Basic YAML to JSON conversion (very simple)
        content = convertYAMLToJSON(content);
        
        try {
            return parseJSON(content);
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    bool parseJSON(const std::string& json) {
        // Simple JSON parser for basic object structures
        size_t pos = 0;
        skipWhitespace(json, pos);
        
        if (json[pos] != '{') {
            return false; // Root must be an object
        }
        
        ++pos; // Skip {
        bool first = true;
        
        while (pos < json.size()) {
            skipWhitespace(json, pos);
            
            if (json[pos] == '}') {
                ++pos; // Skip }
                break;
            }
            
            if (!first) {
                if (json[pos] != ',') {
                    return false;
                }
                ++pos; // Skip ,
            }
            
            first = false;
            
            // Parse key
            std::string key = parseJSONString(json, pos);
            
            skipWhitespace(json, pos);
            if (pos >= json.size() || json[pos] != ':') {
                return false;
            }
            
            ++pos; // Skip :
            skipWhitespace(json, pos);
            
            // Parse value
            ConfigValue value = parseJSONValue(json, pos);
            set(key, value, ConfigSource::File);
            
            skipWhitespace(json, pos);
        }
        
        return true;
    }
    
    std::string parseJSONString(const std::string& json, size_t& pos) {
        skipWhitespace(json, pos);
        
        if (pos >= json.size() || json[pos] != '"') {
            throw std::runtime_error("Expected string start\"");
        }
        
        ++pos; // Skip "
        size_t start = pos;
        
        while (pos < json.size()) {
            if (json[pos] == '\\') {
                // Skip escaped character
                ++pos;
                if (pos >= json.size()) {
                    throw std::runtime_error("Unterminated escape sequence");
                }
            } else if (json[pos] == '"') {
                break;
            }
            ++pos;
        }
        
        if (pos >= json.size()) {
            throw std::runtime_error("Unterminated string");
        }
        
        std::string result = json.substr(start, pos - start);
        ++pos; // Skip ending "
        
        return result;
    }
    
    ConfigValue parseJSONValue(const std::string& json, size_t& pos) {
        skipWhitespace(json, pos);
        
        if (pos >= json.size()) {
            throw std::runtime_error("Unexpected end of JSON");
        }
        
        switch (json[pos]) {
            case '"': {
                // String
                std::string str = parseJSONString(json, pos);
                return str;
            }
            case '{': {
                // Object - for now, treat as unparsed (would need recursive parsing)
                size_t start = pos;
                int braceCount = 1;
                ++pos; // Skip {
                
                while (pos < json.size() && braceCount > 0) {
                    if (json[pos] == '{') {
                        ++braceCount;
                    } else if (json[pos] == '}') {
                        --braceCount;
                    }
                    ++pos;
                }
                
                if (braceCount != 0) {
                    throw std::runtime_error("Unmatched braces in JSON");
                }
                
                // For now, return empty object - could be expanded
                return std::string("{}:");
            }
            case '[': {
                // Array
                return parseJSONArray(json, pos);
            }
            case 't':
            case 'f': {
                // Boolean
                if (json.compare(pos, 4, "true") == 0) {
                    pos += 4;
                    return true;
                } else if (json.compare(pos, 5, "false") == 0) {
                    pos += 5;
                    return false;
                } else {
                    throw std::runtime_error("Invalid boolean value");
                }
            }
            case 'n': {
                // Null
                if (json.compare(pos, 4, "null") == 0) {
                    pos += 4;
                    return std::string("null");
                } else {
                    throw std::runtime_error("Invalid null value");
                }
            }
            default: {
                // Number
                return parseJSONNumber(json, pos);
            }
        }
    }
    
    std::vector<ConfigValue> parseJSONArray(const std::string& json, size_t& pos) {
        std::vector<ConfigValue> array;
        
        if (json[pos] != '[') {
            throw std::runtime_error("Expected array start[");
        }
        
        ++pos; // Skip [
        bool first = true;
        
        while (pos < json.size()) {
            skipWhitespace(json, pos);
            
            if (json[pos] == ']') {
                ++pos; // Skip ]
                break;
            }
            
            if (!first) {
                if (json[pos] != ',') {
                    throw std::runtime_error("Expected comma in array");
                }
                ++pos; // Skip ,
            }
            
            first = false;
            skipWhitespace(json, pos);
            
            array.push_back(parseJSONValue(json, pos));
            skipWhitespace(json, pos);
        }
        
        return array;
    }
    
    ConfigValue parseJSONNumber(const std::string& json, size_t& pos) {
        size_t start = pos;
        
        // Integer part
        while (pos < json.size() && std::isdigit(json[pos])) {
            ++pos;
        }
        
        // Decimal part
        if (pos < json.size() && json[pos] == '.') {
            ++pos; // Skip .
            while (pos < json.size() && std::isdigit(json[pos])) {
                ++pos;
            }
        }
        
        // Exponent part
        if (pos < json.size() && (json[pos] == 'e' || json[pos] == 'E')) {
            ++pos; // Skip e/E
            if (pos < json.size() && (json[pos] == '+' || json[pos] == '-')) {
                ++pos; // Skip sign
            }
            while (pos < json.size() && std::isdigit(json[pos])) {
                ++pos;
            }
        }
        
        std::string numStr = json.substr(start, pos - start);
        
        // Try to parse as integer first
        try {
            int intVal = std::stoi(numStr);
            return intVal;
        } catch (...) {
            // Try double
            try {
                double doubleVal = std::stod(numStr);
                return doubleVal;
            } catch (...) {
                throw std::runtime_error("Invalid number format: " + numStr);
            }
        }
    }
    
    void skipWhitespace(const std::string& json, size_t& pos) {
        while (pos < json.size() && std::isspace(json[pos])) {
            ++pos;
        }
    }
    
    std::string convertYAMLToJSON(const std::string& yaml) {
        // Very basic YAML to JSON conversion
        // This is a simplistic implementation that handles only basic cases
        std::string json = yaml;
        
        // Remove YAML comments
        size_t commentPos = 0;
        while ((commentPos = json.find("#", commentPos)) != std::string::npos) {
            size_t newlinePos = json.find("\n", commentPos);
            if (newlinePos != std::string::npos) {
                json.erase(commentPos, newlinePos - commentPos);
            } else {
                json.erase(commentPos);
                break;
            }
        }
        
        // Remove YAML directives (---)
        size_t directivePos = 0;
        while ((directivePos = json.find("---", directivePos)) != std::string::npos) {
            size_t newlinePos = json.find("\n", directivePos);
            if (newlinePos != std::string::npos) {
                json.erase(directivePos, newlinePos - directivePos + 1);
            } else {
                json.erase(directivePos, 3);
                break;
            }
        }
        
        // Basic conversion of YAML key-value pairs to JSON
        // This is a very simple implementation
        size_t colonPos;
        while ((colonPos = json.find(":", 0)) != std::string::npos) {
            size_t lineEnd = json.find("\n", 0);
            if (lineEnd == std::string::npos || lineEnd > colonPos) {
                break;
            }
            
            size_t keyEnd = colonPos;
            std::string key = json.substr(0, keyEnd);
            
            size_t valueStart = colonPos + 1;
            std::string value = json.substr(valueStart, lineEnd - valueStart);
            
            // Remove quotes from value if present
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            
            // Convert to JSON format
            json = "{\"" + key + "\":\"" + value + "\"}";
            break;
        }
        
        return json;
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

// Helper function to convert string to lowercase
static std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Helper function to trim whitespace from both ends
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
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

} // namespace nlm
