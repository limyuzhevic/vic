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
// PHASE 2: JSON/YAML configuration parser
    // Supports JSON format with nested object notation (e.g., "brain.neuron_count")

    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Parse JSON content
    return parseJSON(content, ConfigSource::File);
}

bool Config::parseJSON(const std::string& jsonStr, ConfigSource source) {
    // Simple JSON parser implementation for NLM configuration
    // Supports: { "key1": value1, "key2": { "subkey": value } }
    
    size_t pos = 0;
    skipWhitespace(jsonStr, pos);
    
    if (pos >= jsonStr.size() || jsonStr[pos] != '{') {
        return false; // Not a valid JSON object
    }
    
    ++pos; // Skip opening brace
    skipWhitespace(jsonStr, pos);
    
    while (pos < jsonStr.size() && jsonStr[pos] != '}') {
        // Parse key
        skipWhitespace(jsonStr, pos);
        if (pos >= jsonStr.size() || jsonStr[pos] != '"') {
            return false; // Expected string key
        }
        
        size_t keyStart = ++pos;
        while (pos < jsonStr.size() && jsonStr[pos] != '"') {
            // Handle escaped quotes
            if (jsonStr[pos] == '\\' && pos + 1 < jsonStr.size()) {
                pos++; // Skip escape character
            }
            pos++;
        }
        
        if (pos >= jsonStr.size()) {
            return false; // Unterminated string
        }
        
        std::string key = jsonStr.substr(keyStart, pos - keyStart);
        pos++; // Skip closing quote
        skipWhitespace(jsonStr, pos);
        
        if (pos >= jsonStr.size() || jsonStr[pos] != ':') {
            return false; // Expected colon
        }
        
        pos++; // Skip colon
        skipWhitespace(jsonStr, pos);
        
        // Parse value based on type
        ConfigValue value;
        if (parseJSONValue(jsonStr, pos, value)) {
            // Handle nested keys with dot notation
            std::string fullKey = key;
            size_t nestedPos = pos;
            skipWhitespace(jsonStr, nestedPos);
            
            if (nestedPos < jsonStr.size() && jsonStr[nestedPos] == '{') {
                // Nested object - flatten with dot notation
                size_t braceCount = 1;
                size_t endPos = nestedPos + 1;
                
                while (endPos < jsonStr.size() && braceCount > 0) {
                    if (jsonStr[endPos] == '{') braceCount++;
                    else if (jsonStr[endPos] == '}') braceCount--;
                    endPos++;
                }
                
                // Parse nested object and flatten keys
                std::string nestedJson = jsonStr.substr(nestedPos, endPos - nestedPos);
                std::vector<std::string> flattened = flattenJSON(nestedJson, key + ".");
                for (const auto& flatKey : flattened) {
                    // Extract value from nested structure
                    ConfigValue nestedValue;
                    size_t valPos = 0;
                    if (parseJSONValue(jsonStr, nestedPos, nestedValue)) {
                        set(flatKey, nestedValue, source);
                    }
                }
                pos = endPos;
            } else {
                // Simple value
                set(key, value, source);
            }
        }
        
        skipWhitespace(jsonStr, pos);
        
        // Skip comma if present
        if (pos < jsonStr.size() && jsonStr[pos] == ',') {
            pos++;
        }
        skipWhitespace(jsonStr, pos);
    }
    
    return true;
}

bool Config::parseJSONValue(const std::string& jsonStr, size_t& pos, ConfigValue& value) {
    skipWhitespace(jsonStr, pos);
    
    if (pos >= jsonStr.size()) {
        return false;
    }
    
    char c = jsonStr[pos];
    
    switch (c) {
        case '\"': { // String
            size_t start = ++pos;
            while (pos < jsonStr.size()) {
                if (jsonStr[pos] == '\\' && pos + 1 < jsonStr.size()) {
                    pos += 2; // Skip escaped character
                } else if (jsonStr[pos] == '\"') {
                    break;
                }
                pos++;
            }
            
            if (pos >= jsonStr.size()) {
                return false; // Unterminated string
            }
            
            std::string strValue = jsonStr.substr(start, pos - start);
            value = strValue;
            pos++; // Skip closing quote
            return true;
        }
        case 't': { // true
            if (pos + 4 <= jsonStr.size() && jsonStr.substr(pos, 4) == "true") {
                value = true;
                pos += 4;
                return true;
            }
            return false;
        }
        case 'f': { // false
            if (pos + 5 <= jsonStr.size() && jsonStr.substr(pos, 5) == "false") {
                value = false;
                pos += 5;
                return true;
            }
            return false;
        }
        case 'n': { // null
            if (pos + 4 <= jsonStr.size() && jsonStr.substr(pos, 4) == "null") {
                pos += 4;
                return false; // Don't store null values
            }
            return false;
        }
        case '{': { // Object (should be handled by parseJSON)
            return false;
        }
        case '[': { // Array
            size_t start = pos++;
            std::vector<double> arrayValues;
            
            while (pos < jsonStr.size() && jsonStr[pos] != ']') {
                double arrayValue = 0.0;
                size_t valuePos = pos;
                if (parseNumber(jsonStr, valuePos, arrayValue)) {
                    arrayValues.push_back(arrayValue);
                    pos = valuePos;
                }
                skipWhitespace(jsonStr, pos);
                
                if (pos < jsonStr.size() && jsonStr[pos] == ',') {
                    pos++;
                }
                skipWhitespace(jsonStr, pos);
            }
            
            if (pos >= jsonStr.size() || jsonStr[pos] != ']') {
                return false;
            }
            
            value = arrayValues;
            pos++; // Skip closing bracket
            return true;
        }
        default: { // Number or boolean
            double numValue;
            size_t valuePos = parseNumber(jsonStr, pos, numValue);
            if (valuePos > pos) {
                // Check if it's an integer
                int intValue = static_cast<int>(numValue);
                if (static_cast<double>(intValue) == numValue) {
                    value = intValue;
                } else {
                    value = numValue;
                }
                pos = valuePos;
                return true;
            }
            return false;
        }
    }
}

bool Config::parseNumber(const std::string& jsonStr, size_t& pos, double& value) {
    size_t start = pos;
    
    // Handle optional sign
    if (pos < jsonStr.size() && (jsonStr[pos] == '+' || jsonStr[pos] == '-')) {
        pos++;
    }
    
    // Parse integer part
    while (pos < jsonStr.size() && std::isdigit(jsonStr[pos])) {
        pos++;
    }
    
    // Parse fractional part
    if (pos < jsonStr.size() && jsonStr[pos] == '.') {
        pos++; // Skip decimal point
        while (pos < jsonStr.size() && std::isdigit(jsonStr[pos])) {
            pos++;
        }
    }
    
    // Parse exponent
    if (pos < jsonStr.size() && (jsonStr[pos] == 'e' || jsonStr[pos] == 'E')) {
        pos++; // Skip exponent marker
        
        // Handle optional sign in exponent
        if (pos < jsonStr.size() && (jsonStr[pos] == '+' || jsonStr[pos] == '-')) {
            pos++;
        }
        
        // Parse exponent digits
        while (pos < jsonStr.size() && std::isdigit(jsonStr[pos])) {
            pos++;
        }
    }
    
    if (start == pos) {
        return false; // No digits found
    }
    
    try {
        value = std::stod(jsonStr.substr(start, pos - start));
        return true;
    } catch (...) {
        return false;
    }
}

void Config::skipWhitespace(const std::string& jsonStr, size_t& pos) {
    while (pos < jsonStr.size() && std::isspace(jsonStr[pos])) {
        pos++;
    }
}

std::vector<std::string> Config::flattenJSON(const std::string& jsonStr, const std::string& prefix) {
    std::vector<std::string> keys;
    
    size_t pos = 0;
    skipWhitespace(jsonStr, pos);
    
    if (pos >= jsonStr.size() || jsonStr[pos] != '{') {
        return keys;
    }
    
    ++pos; // Skip opening brace
    skipWhitespace(jsonStr, pos);
    
    while (pos < jsonStr.size() && jsonStr[pos] != '}') {
        // Parse key
        skipWhitespace(jsonStr, pos);
        if (pos >= jsonStr.size() || jsonStr[pos] != '"') {
            break;
        }
        
        size_t keyStart = ++pos;
        while (pos < jsonStr.size() && jsonStr[pos] != '"') {
            if (jsonStr[pos] == '\\' && pos + 1 < jsonStr.size()) {
                pos++;
            }
            pos++;
        }
        
        if (pos >= jsonStr.size()) {
            break;
        }
        
        std::string key = jsonStr.substr(keyStart, pos - keyStart);
        pos++; // Skip closing quote
        skipWhitespace(jsonStr, pos);
        
        if (pos >= jsonStr.size() || jsonStr[pos] != ':') {
            break;
        }
        
        pos++; // Skip colon
        skipWhitespace(jsonStr, pos);
        
        std::string fullKey = prefix + "." + key;
        keys.push_back(fullKey);
        
        // Skip comma if present
        skipWhitespace(jsonStr, pos);
        if (pos < jsonStr.size() && jsonStr[pos] == ',') {
            pos++;
        }
        skipWhitespace(jsonStr, pos);
    }
    
    return keys;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{" << std::endl;
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format to avoid scientific notation
                file << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeString(arg) << "\"";
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
                    file << std::fixed << std::setprecision(6) << arg[j];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t j = 0; j < arg.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << escapeString(arg[j]) << "\"";
                }
                file << "]";
            } else {
                file << "null";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << "," << std::endl;
        } else {
            file << std::endl;
        }
    }
    
    file << "}" << std::endl;
    
    return true;
}

std::string Config::escapeString(const std::string& str) {
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
                if (static_cast<unsigned char>(c) < 32) {
                    // Control character, use Unicode escape
                    result += "\\u";
                    char hex[5];
                    snprintf(hex, sizeof(hex), "%04x", c);
                    result += hex;
                } else {
                    result += c;
                }
        }
    }
    
    return result;
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
