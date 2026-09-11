#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <any>
#include <vector>
#include <memory>
#include <string>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// Implement trim for config values (inside namespace nlm)
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Implement toLower for config keys
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Simple file loader that supports both JSON and YAML-like formats
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line.find('#') == 0 || line.find('/') == 0) {
            continue;
        }
        
        // Try to parse as simple key=value format first
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
            continue;
        }
        
        // Try to parse as JSON-like format (simple handling)
        if (line.find('{') != std::string::npos && line.find('}') != std::string::npos) {
            // Very simple JSON parsing - extract key-value pairs between braces
            size_t bracePos = line.find('{');
            size_t closePos = line.rfind('}');
            if (bracePos < closePos) {
                std::string content = line.substr(bracePos + 1, closePos - bracePos - 1);
                // Split by commas
                size_t start = 0;
                while (start < content.size()) {
                    size_t commaPos = content.find(',', start);
                    std::string pair;
                    if (commaPos == std::string::npos) {
                        pair = content.substr(start);
                        start = content.size();
                    } else {
                        pair = content.substr(start, commaPos - start);
                        start = commaPos + 1;
                    }
                    
                    size_t colonPos = pair.find(':');
                    if (colonPos != std::string::npos) {
                        std::string key = trim(pair.substr(0, colonPos));
                        std::string value = trim(pair.substr(colonPos + 1));
                        
                        // Remove quotes from value
                        if (value.size() >= 2 && 
                            ((value.front() == '"' && value.back() == '"') ||
                             (value.front() == '\'' && value.back() == '\''))) {
                            value = value.substr(1, value.size() - 2);
                        }
                        
                        set(key, value, ConfigSource::File);
                    }
                }
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
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Output based on value type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int> || std::is_same_v<T, std::vector<int64_t> || std::is_same_v<T, std::vector<double> || std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                const auto& vec = arg;
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (std::is_same_v<T, std::vector<std::string>) {
                        file << "\"" << vec[i] << "\"";
                    } else {
                        file << vec[i];
                    }
                    if (i < vec.size() - 1) file << ", ";
                }
                file << "]";
            } else {
                file << "PLACEHOLDER_VALUE";
            }
        }, entry.value);
        
        file << "\n\n";
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

// Template method for setFromDict
// Template method for setFromDict
template<typename T>
void Config::setFromDict(const std::string& key, const T& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Type-specific getters for convenience
std::optional<int> Config::getInt(const std::string& key) const {
    return get<int>(key);
}

std::optional<double> Config::getDouble(const std::string& key) const {
    return get<double>(key);
}

std::optional<bool> Config::getBool(const std::string& key) const {
    return get<bool>(key);
}

std::optional<std::string> Config::getString(const std::string& key) const {
    return get<std::string>(key);
}

// Type-safe set/get with validation
template<typename T>
bool Config::setAndValidate(const std::string& key, const T& value, ConfigSource source) {
    if (validate({})) {
        set(key, ConfigValue(value), source);
        return true;
    }
    return false;
}

// Batch operations
void Config::setBatch(const std::map<std::string, ConfigValue>& values, ConfigSource source) {
    for (const auto& pair : values) {
        set(pair.first, pair.second, source);
    }
}

std::map<std::string, ConfigValue> Config::getBatch(const std::vector<std::string>& keys) const {
    std::map<std::string, ConfigValue> result;
    for (const auto& key : keys) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != pImpl->entries.end()) {
            result[key] = it->value;
        }
    }
    return result;
}

// Configuration validation
bool Config::validate(std::vector<std::string>& errors) const {
    bool isValid = true;
    
    for (const auto& entry : pImpl->entries) {
        const std::string& key = entry.key;
        const ConfigValue& value = entry.value;
        
        // Basic validation based on key patterns
        if (key.empty()) {
            errors.push_back("Empty configuration key");
            isValid = false;
            continue;
        }
        
        // Type-specific validation
        std::visit([&](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            
            if constexpr (std::is_same_v<T, int>) {
                if (val < -2147483648 || val > 2147483647) {
                    errors.push_back("Integer " + key + " out of range [-2147483648, 2147483647]");
                    isValid = false;
                }
            } else if constexpr (std::is_same_v<T, int64_t>) {
                if (val < -9223372036854775808LL || val > 9223372036854775807LL) {
                    errors.push_back("Integer64 " + key + " out of range [-9223372036854775808, 9223372036854775807]");
                    isValid = false;
                }
            } else if constexpr (std::is_same_v<T, double>) {
                if (std::isnan(val) || std::isinf(val)) {
                    errors.push_back("Double " + key + " is NaN or infinite");
                    isValid = false;
                }
            } else if constexpr (std::is_same_v<T, bool>) {
                // Boolean values are always valid
            } else if constexpr (std::is_same_v<T, std::string>) {
                if (val.empty()) {
                    errors.push_back("String " + key + " is empty (if not allowed)");
                }
            } else if constexpr (std::is_same_v<T, std::vector<int> || std::is_same_v<T, std::vector<int64_t> || std::is_same_v<T, std::vector<double> || std::is_same_v<T, std::vector<std::string>>) {
                if (val.size() > 10000) {
                    errors.push_back("Vector " + key + " is too large (>10000 elements)");
                    isValid = false;
                }
            }
        }, value);
    }
    
    return isValid;
}

bool Config::isValid() const {
    std::vector<std::string> errors;
    return validate(errors);
}

// Convenience methods for common patterns
void Config::loadFromDict(const std::map<std::string, std::variant<int, double, bool, std::string, std::vector<int>, std::vector<double>, std::vector<std::string>>>& configDict) {
    for (const auto& pair : configDict) {
        set(pair.first, ConfigValue(pair.second), ConfigSource::Runtime);
    }
}

// Export/import helpers
std::string Config::toJson() const {
    std::ostringstream oss;
    oss << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": ";
        
        std::visit([&oss](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            
            if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                oss << std::to_string(val);
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::to_string(val);
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int> || std::is_same_v<T, std::vector<int64_t> || std::is_same_v<T, std::vector<double> || std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                const auto& vec = val;
                for (size_t j = 0; j < vec.size(); ++j) {
                    if constexpr (std::is_same_v<T, std::vector<std::string>) {
                        oss << "\"" << vec[j] << "\"";
                    } else {
                        oss << vec[j];
                    }
                    if (j < vec.size() - 1) oss << ", ";
                }
                oss << "]";
            } else {
                oss << "null";
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) oss << ",\n";
    }
    oss << "\n}\n";
    return oss.str();
}

bool Config::fromJson(const std::string& jsonStr) {
    clear();
    
    // Very simple JSON parsing for demonstration
    // This could be enhanced with a proper JSON parser
    size_t pos = 0;
    
    // Skip opening brace
    if (pos >= jsonStr.size() || jsonStr[pos] != '{') return false;
    pos++;
    
    while (pos < jsonStr.size()) {
        // Skip whitespace
        while (pos < jsonStr.size() && (jsonStr[pos] == ' ' || jsonStr[pos] == '\n' || jsonStr[pos] == '\t')) pos++;
        
        // Check for closing brace
        if (pos >= jsonStr.size() || jsonStr[pos] == '}') break;
        
        // Parse key (quoted string)
        if (pos >= jsonStr.size() || jsonStr[pos] != '\"') return false;
        pos++;
        size_t keyStart = pos;
        while (pos < jsonStr.size() && jsonStr[pos] != '\"') pos++;
        if (pos >= jsonStr.size()) return false;
        std::string key = jsonStr.substr(keyStart, pos - keyStart);
        pos++; // Skip closing quote
        
        // Skip whitespace
        while (pos < jsonStr.size() && (jsonStr[pos] == ' ' || jsonStr[pos] == '\n' || jsonStr[pos] == '\t')) pos++;
        
        // Check for colon
        if (pos >= jsonStr.size() || jsonStr[pos] != ':') return false;
        pos++;
        
        // Skip whitespace
        while (pos < jsonStr.size() && (jsonStr[pos] == ' ' || jsonStr[pos] == '\n' || jsonStr[pos] == '\t')) pos++;
        
        // Parse value based on first character
        if (pos >= jsonStr.size()) return false;
        
        if (jsonStr[pos] == '\"') {
            // String value
            pos++;
            size_t valueStart = pos;
            while (pos < jsonStr.size() && jsonStr[pos] != '\"') pos++;
            if (pos >= jsonStr.size()) return false;
            std::string value = jsonStr.substr(valueStart, pos - valueStart);
            pos++; // Skip closing quote
            set(key, value, ConfigSource::File);
        } else if (jsonStr[pos] == 't' || jsonStr[pos] == 'f') {
            // Boolean value
            bool value;
            if (jsonStr.substr(pos, 4) == "true") {
                value = true;
                pos += 4;
            } else if (jsonStr.substr(pos, 5) == "false") {
                value = false;
                pos += 5;
            } else {
                return false;
            }
            set(key, value, ConfigSource::File);
        } else if (jsonStr[pos] == '-' || (jsonStr[pos] >= '0' && jsonStr[pos] <= '9')) {
            // Number value
            size_t valueStart = pos;
            while (pos < jsonStr.size() && (jsonStr[pos] == '-' || jsonStr[pos] == '.' || 
                (jsonStr[pos] >= '0' && jsonStr[pos] <= '9') || jsonStr[pos] == 'e' || jsonStr[pos] == 'E')) pos++;
            std::string numStr = jsonStr.substr(valueStart, pos - valueStart);
            try {
                if (numStr.find('.') != std::string::npos || numStr.find('e') != std::string::npos || numStr.find('E') != std::string::npos) {
                    // Float
                    double value = std::stod(numStr);
                    set(key, value, ConfigSource::File);
                } else {
                    // Integer
                    long long value = std::stoll(numStr);
                    set(key, value, ConfigSource::File);
                }
            } catch (...) {
                return false;
            }
        } else if (jsonStr[pos] == '[') {
            // Array value - for now skip complex array parsing
            // This is a simplified implementation
            pos++;
            set(key, std::vector<double>{1.0, 2.0, 3.0}, ConfigSource::File); // Placeholder
        } else {
            return false;
        }
        
        // Skip whitespace
        while (pos < jsonStr.size() && (jsonStr[pos] == ' ' || jsonStr[pos] == '\n' || jsonStr[pos] == '\t')) pos++;
        
        // Check for comma or closing brace
        if (pos >= jsonStr.size()) return false;
        if (jsonStr[pos] == ',') {
            pos++;
            continue;
        } else if (jsonStr[pos] == '}') {
            pos++; // Skip closing brace
            break;
        } else {
            return false;
        }
    }
    
    return true;
}

} // namespace nlm