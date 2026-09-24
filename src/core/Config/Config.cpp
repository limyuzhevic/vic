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

// JSON parser utilities
namespace {

    // Skip whitespace
    size_t skipWhitespace(const std::string& str, size_t pos) {
        while (pos < str.size() && std::isspace(str[pos])) {
            pos++;
        }
        return pos;
    }

    // Parse a JSON value and convert to ConfigValue
    ConfigValue parseJSONValue(const std::string& str, size_t& pos);

    // Parse a JSON object
    ConfigValue parseJSONObject(const std::string& str, size_t& pos);

    // Parse a JSON string (extract quotes)
    std::string parseJSONString(const std::string& str, size_t& pos);

    // Parse a JSON number
    ConfigValue parseJSONNumber(const std::string& str, size_t& pos);

    // Parse a JSON boolean
    ConfigValue parseJSONBoolean(const std::string& str, size_t& pos);

    // Parse a JSON array
    ConfigValue parseJSONArray(const std::string& str, size_t& pos);

    // Parse a JSON null (returns empty variant)
    ConfigValue parseJSONNull(const std::string& str, size_t& pos);

    // Parse any JSON value
    ConfigValue parseJSONValue(const std::string& str, size_t& pos) {
        pos = skipWhitespace(str, pos);
        
        if (pos >= str.size()) {
            return "";
        }
        
        switch (str[pos]) {
            case '{':
                return parseJSONObject(str, pos);
            case '"':
                return parseJSONString(str, pos);
            case '[':
                return parseJSONArray(str, pos);
            case '0': case '1': case '2': case '3': case '4':
            case '5': case '6': case '7': case '8': case '9':
            case '-':
                return parseJSONNumber(str, pos);
            case 't': case 'f':
                return parseJSONBoolean(str, pos);
            case 'n':
                return parseJSONNull(str, pos);
            default:
                return "";
        }
    }

    // Parse a JSON object
    ConfigValue parseJSONObject(const std::string& str, size_t& pos) {
        std::vector<ConfigEntry> entries;
        pos = skipWhitespace(str, pos + 1); // Skip {
        
        while (pos < str.size() && str[pos] != '}') {
            pos = skipWhitespace(str, pos);
            
            // Parse key (string)
            if (str[pos] != '"') {
                break;
            }
            
            size_t keyStart = ++pos; // Skip opening quote
            while (pos < str.size() && str[pos] != '"') {
                pos++;
            }
            std::string key = str.substr(keyStart, pos - keyStart);
            pos = skipWhitespace(str, ++pos); // Skip closing quote and whitespace
            
            // Expect colon
            if (pos >= str.size() || str[pos] != ':') {
                break;
            }
            pos = skipWhitespace(str, ++pos); // Skip colon and whitespace
            
            // Parse value
            ConfigValue value = parseJSONValue(str, pos);
            
            // Add to entries
            entries.emplace_back(key, value, ConfigSource::File);
            
            // Skip comma if present
            pos = skipWhitespace(str, pos);
            if (pos < str.size() && str[pos] == ',') {
                pos++;
            }
        }
        
        if (pos < str.size() && str[pos] == '}') {
            pos++; // Skip closing }
        }
        
        // Convert entries to ConfigValue (we'll just store as string representation for now)
        // In a real implementation, we might want to convert to appropriate variant type
        std::string jsonStr = "{";
        for (size_t i = 0; i < entries.size(); ++i) {
            if (i > 0) jsonStr += ",";
            jsonStr += "\"" + entries[i].key + "\":";
            // Convert value to string (simplified)
            jsonStr += "\"" + std::get<std::string>(entries[i].value) + "\"";
        }
        jsonStr += "}";
        
        return jsonStr;
    }

    // Parse a JSON string
    std::string parseJSONString(const std::string& str, size_t& pos) {
        size_t start = ++pos; // Skip opening quote
        while (pos < str.size() && str[pos] != '"') {
            if (str[pos] == '\\' && pos + 1 < str.size()) {
                pos += 2; // Skip escaped character
            } else {
                pos++;
            }
        }
        std::string result = str.substr(start, pos - start);
        
        // Handle escape sequences
        size_t outPos = 0;
        for (size_t inPos = 0; inPos < result.size(); ++inPos) {
            if (result[inPos] == '\\' && inPos + 1 < result.size()) {
                switch (result[++inPos]) {
                    case 'n':
                        result[outPos++] = '\n';
                        break;
                    case 't':
                        result[outPos++] = '\t';
                        break;
                    case 'r':
                        result[outPos++] = '\r';
                        break;
                    case '\\':
                        result[outPos++] = '\\';
                        break;
                    case '"':
                        result[outPos++] = '"';
                        break;
                    case 'u': {
                        // Unicode escape - simplified, just copy
                        if (inPos + 4 < result.size()) {
                            for (int i = 0; i < 4; ++i) {
                                result[outPos++] = result[++inPos];
                            }
                        }
                        break;
                    }
                    default:
                        result[outPos++] = result[inPos];
                }
            } else {
                result[outPos++] = result[inPos];
            }
        }
        result.resize(outPos);
        
        pos = skipWhitespace(str, ++pos); // Skip closing quote and whitespace
        return result;
    }

    // Parse a JSON number
    ConfigValue parseJSONNumber(const std::string& str, size_t& pos) {
        size_t start = pos;
        bool isNegative = false;
        
        if (str[pos] == '-') {
            isNegative = true;
            pos++;
        }
        
        // Integer part
        while (pos < str.size() && std::isdigit(str[pos])) {
            pos++;
        }
        
        // Decimal part
        if (pos < str.size() && str[pos] == '.') {
            pos++; // Skip decimal point
            while (pos < str.size() && std::isdigit(str[pos])) {
                pos++;
            }
        }
        
        // Exponent part
        if (pos < str.size() && (str[pos] == 'e' || str[pos] == 'E')) {
            pos++; // Skip 'e' or 'E'
            if (pos < str.size() && (str[pos] == '+' || str[pos] == '-')) {
                pos++;
            }
            while (pos < str.size() && std::isdigit(str[pos])) {
                pos++;
            }
        }
        
        std::string numStr = str.substr(start, pos - start);
        
        // Try to parse as integer first
        try {
            size_t decimalPos = numStr.find('.');
            if (decimalPos == std::string::npos) {
                // Integer
                int64_t intVal = std::stoll(numStr);
                return isNegative ? static_cast<int64_t>(-intVal) : intVal;
            } else {
                // Double
                double doubleVal = std::stod(numStr);
                return static_cast<double>(isNegative ? -doubleVal : doubleVal);
            }
        } catch (...) {
            // Fallback to string
            return numStr;
        }
    }

    // Parse a JSON boolean
    ConfigValue parseJSONBoolean(const std::string& str, size_t& pos) {
        size_t start = pos;
        while (pos < str.size() && (std::isalpha(str[pos]) || str[pos] == '_')) {
            pos++;
        }
        
        std::string boolStr = str.substr(start, pos - start);
        if (boolStr == "true") {
            return true;
        } else if (boolStr == "false") {
            return false;
        }
        
        return "";
    }

    // Parse a JSON array
    ConfigValue parseJSONArray(const std::string& str, size_t& pos) {
        pos = skipWhitespace(str, pos + 1); // Skip [
        std::vector<ConfigValue> arrayValues;
        
        while (pos < str.size() && str[pos] != ']') {
            pos = skipWhitespace(str, pos);
            
            // Parse value
            ConfigValue value = parseJSONValue(str, pos);
            arrayValues.push_back(value);
            
            // Skip comma if present
            pos = skipWhitespace(str, pos);
            if (pos < str.size() && str[pos] == ',') {
                pos++;
            }
        }
        
        if (pos < str.size() && str[pos] == ']') {
            pos++; // Skip closing ]
        }
        
        return arrayValues;
    }

    // Parse a JSON null
    ConfigValue parseJSONNull(const std::string& str, size_t& pos) {
        if (str.compare(pos, 4, "null") == 0) {
            pos += 4;
            return "";
        }
        return "";
    }

    // Helper function to unescape JSON string
    std::string unescapeJSON(const std::string& str) {
        std::string result;
        result.reserve(str.size());
        
        for (size_t i = 0; i < str.size(); ++i) {
            if (str[i] == '\\' && i + 1 < str.size()) {
                switch (str[i + 1]) {
                    case 'n':
                        result += '\n';
                        i++;
                        break;
                    case 't':
                        result += '\t';
                        i++;
                        break;
                    case 'r':
                        result += '\r';
                        i++;
                        break;
                    case '\\':
                        result += '\\';
                        i++;
                        break;
                    case '"':
                        result += '"';
                        i++;
                        break;
                    default:
                        result += str[i];
                }
            } else {
                result += str[i];
            }
        }
        
        return result;
    }

    // Helper function to convert ConfigValue to string for saving
    std::string configValueToString(const ConfigValue& value) {
        std::string result;
        
        std::visit([&result](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                result = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, int64_t>) {
                result = std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                // Format to avoid scientific notation
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(6) << arg;
                result = oss.str();
            } else if constexpr (std::is_same_v<T, bool>) {
                result = arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::string>) {
                result = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                result = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) result += ",";
                    result += std::to_string(arg[i]);
                }
                result += "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                result = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) result += ",";
                    std::ostringstream oss;
                    oss << std::fixed << std::setprecision(6) << arg[i];
                    result += oss.str();
                }
                result += "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                result = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) result += ",";
                    result += "\"" + arg[i] + "\"";
                }
                result += "]";
            } else {
                result = "";
            }
        }, value);
        
        return result;
    }

} // namespace

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Clear existing entries
    pImpl->entries.clear();
    
    // Parse JSON content
    size_t pos = 0;
    pos = skipWhitespace(content, pos);
    
    if (pos >= content.size()) {
        return true; // Empty file is valid
    }
    
    ConfigValue rootValue = parseJSONValue(content, pos);
    
    // If it's a JSON object, extract key-value pairs
    if (std::holds_alternative<std::string>(rootValue)) {
        std::string jsonStr = std::get<std::string>(rootValue);
        
        // Simple parsing for now - look for key=value patterns
        // This is a simplified approach for compatibility
        size_t start = 0;
        while (start < jsonStr.size()) {
            size_t equalsPos = jsonStr.find('=', start);
            if (equalsPos == std::string::npos) {
                break;
            }
            
            size_t endPos = jsonStr.find(',', equalsPos);
            if (endPos == std::string::npos) {
                endPos = jsonStr.size();
            }
            
            std::string key = jsonStr.substr(start, equalsPos - start);
            std::string value = jsonStr.substr(equalsPos + 1, endPos - equalsPos - 1);
            
            // Trim whitespace
            key = trim(key);
            value = trim(value);
            
            // Remove quotes from key and value if present
            if (key.size() >= 2 && key.front() == '"' && key.back() == '"') {
                key = key.substr(1, key.size() - 2);
            }
            if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
            
            start = endPos + 1;
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
        file << "  \"" << entry.key << "\": \"" << configValueToString(entry.value) << "\"";
        
        if (i < pImpl->entries.size() - 1) {
            file << ",";
        }
        file << "\n";
    }
    file << "}\n";
    
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
