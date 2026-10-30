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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Try to parse as JSON first (more robust)
    if (parseJSON(content)) {
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
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

bool Config::parseJSON(const std::string& json) {
    // Simple but robust JSON parser for configuration files
    // Supports:
    // - Key-value pairs: {"key": "value", "number": 42, "boolean": true, "array": [1, 2, 3]}
    // - No nested objects (for simplicity)
    // - Handles basic escaping
    
    try {
        size_t pos = 0;
        
        // Skip whitespace and opening brace
        while (pos < json.size() && std::isspace(json[pos])) pos++;
        if (pos >= json.size() || json[pos] != '{') {
            return false; // Not JSON
        }
        pos++;
        
        while (pos < json.size()) {
            // Skip whitespace
            while (pos < json.size() && std::isspace(json[pos])) pos++;
            
            // Check for end of object
            if (pos >= json.size() || json[pos] == '}') {
                break;
            }
            
            // Parse key (must be a string)
            while (pos < json.size() && std::isspace(json[pos])) pos++;
            if (pos >= json.size() || json[pos] != '"') {
                return false; // Invalid key format
            }
            pos++; // Skip opening quote
            
            size_t keyStart = pos;
            while (pos < json.size() && json[pos] != '"') {
                if (json[pos] == '\\' && pos + 1 < json.size()) {
                    pos += 2; // Skip escaped character
                } else {
                    pos++;
                }
            }
            
            if (pos >= json.size()) {
                return false; // Unterminated string
            }
            
            std::string key = json.substr(keyStart, pos - keyStart);
            pos++; // Skip closing quote
            
            // Skip whitespace
            while (pos < json.size() && std::isspace(json[pos])) pos++;
            
            // Check for colon
            if (pos >= json.size() || json[pos] != ':') {
                return false;
            }
            pos++;
            
            // Skip whitespace
            while (pos < json.size() && std::isspace(json[pos])) pos++;
            
            // Parse value
            auto value = parseJSONValue(json, pos);
            if (!value.has_value()) {
                return false; // Failed to parse value
            }
            
            // Set the key-value pair
            set(key, value.value(), ConfigSource::File);
            
            // Skip whitespace
            while (pos < json.size() && std::isspace(json[pos])) pos++;
            
            // Check for comma or end
            if (pos < json.size() && json[pos] == ',') {
                pos++;
                continue;
            } else if (pos < json.size() && json[pos] == '}') {
                break;
            } else {
                return false; // Invalid format
            }
        }
        
        return true;
    } catch (...) {
        return false; // Parsing failed
    }
}

std::optional<ConfigValue> Config::parseJSONValue(const std::string& json, size_t& pos) {
    if (pos >= json.size()) {
        return std::nullopt;
    }
    
    // Skip whitespace
    while (pos < json.size() && std::isspace(json[pos])) pos++;
    
    if (pos >= json.size()) {
        return std::nullopt;
    }
    
    switch (json[pos]) {
        case '"': {
            // String value
            pos++; // Skip opening quote
            size_t start = pos;
            
            while (pos < json.size() && json[pos] != '"') {
                if (json[pos] == '\\' && pos + 1 < json.size()) {
                    pos += 2; // Skip escaped character
                } else {
                    pos++;
                }
            }
            
            if (pos >= json.size()) {
                return std::nullopt; // Unterminated string
            }
            
            std::string value = json.substr(start, pos - start);
            pos++; // Skip closing quote
            return value;
        }
        
        case 't': case 'f': {
            // Boolean value (true/false)
            if (json.substr(pos, 4) == "true") {
                pos += 4;
                return true;
            } else if (json.substr(pos, 5) == "false") {
                pos += 5;
                return false;
            } else {
                return std::nullopt;
            }
        }
        
        case '-': case '0': case '1': case '2': case '3': case '4': 
        case '5': case '6': case '7': case '8': case '9': {
            // Number (integer or float)
            size_t start = pos;
            bool isFloat = false;
            
            while (pos < json.size()) {
                char c = json[pos];
                if (c == '.' || c == 'e' || c == 'E') {
                    isFloat = true;
                } else if (c == '-' && pos > start) {
                    // Negative sign already at start
                    return std::nullopt;
                } else if (!(c >= '0' && c <= '9') && c != '+' && c != '-') {
                    break;
                }
                pos++;
            }
            
            std::string numStr = json.substr(start, pos - start);
            
            // Try to parse as double first (handles both int and float)
            try {
                size_t endPos;
                double doubleVal = std::stod(numStr, &endPos);
                if (endPos == numStr.size()) {
                    // Check if it's actually an integer (no decimal point or exponent)
                    if (!isFloat && doubleVal == static_cast<int64_t>(doubleVal)) {
                        return static_cast<int64_t>(doubleVal);
                    }
                    return doubleVal;
                }
            } catch (...) {
            }
            
            // Fallback to int
            try {
                size_t endPos;
                int64_t intVal = std::stoll(numStr, &endPos);
                if (endPos == numStr.size()) {
                    return intVal;
                }
            } catch (...) {
            }
            
            return std::nullopt;
        }
        
        case '[': {
            // Array value
            pos++; // Skip opening bracket
            std::vector<ConfigValue> arrayValues;
            
            while (pos < json.size() && json[pos] != ']') {
                // Skip whitespace
                while (pos < json.size() && std::isspace(json[pos])) pos++;
                
                if (pos >= json.size()) {
                    return std::nullopt;
                }
                
                auto value = parseJSONValue(json, pos);
                if (!value.has_value()) {
                    return std::nullopt;
                }
                
                arrayValues.push_back(value.value());
                
                // Skip whitespace
                while (pos < json.size() && std::isspace(json[pos])) pos++;
                
                if (pos < json.size() && json[pos] == ',') {
                    pos++; // Skip comma
                    continue;
                } else if (pos < json.size() && json[pos] == ']') {
                    break;
                } else {
                    return std::nullopt;
                }
            }
            
            if (pos >= json.size() || json[pos] != ']') {
                return std::nullopt;
            }
            
            pos++; // Skip closing bracket
            
            // Convert to appropriate vector type
            if (arrayValues.empty()) {
                return std::vector<std::string>{}; // Default to string vector
            }
            
            // Check if all elements are the same type
            auto firstType = arrayValues[0].index();
            bool sameType = std::all_of(arrayValues.begin(), arrayValues.end(),
                [firstType](const ConfigValue& val) { return val.index() == firstType; });
            
            if (sameType) {
                if (firstType == 0) { // int
                    std::vector<int> result;
                    for (const auto& val : arrayValues) {
                        result.push_back(std::get<int>(val));
                    }
                    return result;
                } else if (firstType == 1) { // int64_t
                    std::vector<int64_t> result;
                    for (const auto& val : arrayValues) {
                        result.push_back(std::get<int64_t>(val));
                    }
                    return result;
                } else if (firstType == 2) { // double
                    std::vector<double> result;
                    for (const auto& val : arrayValues) {
                        result.push_back(std::get<double>(val));
                    }
                    return result;
                } else if (firstType == 4) { // std::string
                    std::vector<std::string> result;
                    for (const auto& val : arrayValues) {
                        result.push_back(std::get<std::string>(val));
                    }
                    return result;
                }
            }
            
            // Mixed types - keep as generic vector (will use string vector)
            return std::vector<std::string>{};
        }
        
        default:
            return std::nullopt;
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
    
    // Write as JSON for better compatibility and readability
    file << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        // Serialize the value based on its type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeJSONString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || 
                                 std::is_same_v<T, int64_t> ||
                                 std::is_same_v<T, double>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << escapeJSONString(arg[j]) << "\"";
                }
                file << "]";
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

std::string Config::escapeJSONString(const std::string& str) {
    std::string result;
    result.reserve(str.size() * 2); // Reserve extra space for escaping
    
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (c < 32 || c == 127) {
                    // Control character, use Unicode escape
                    result += "\\u";
                    char hex[5];
                    snprintf(hex, sizeof(hex), "%04x", static_cast<unsigned char>(c));
                    result += hex;
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
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
