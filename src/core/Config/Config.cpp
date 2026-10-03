#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <ctime>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool validateOnSet;
    bool strictJsonParsing;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->validateOnSet = true;
    pImpl->strictJsonParsing = true;
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)),
                           std::istreambuf_iterator<char>());
        
        // Try JSON first
        if (pImpl->strictJsonParsing) {
            return loadFromJson(content);
        }
        
        // Fall back to simple key=value format for backward compatibility
        return loadFromSimpleFormat(content);
    } catch (const std::exception& e) {
        std::cerr << "Error loading config file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    std::cerr << "Warning: Invalid argument format: " << arg << std::endl;
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
    } catch (const std::exception& e) {
        std::cerr << "Error loading config from arguments: " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromJson(const std::string& jsonStr) {
    try {
        // Simple JSON parsing implementation
        // In a real implementation, you would use nlohmann/json or a similar library
        
        // Parse JSON-like format: {"key": value, "key2": "value2", "key3": 123}
        clear();
        
        size_t pos = 0;
        while (pos < jsonStr.size()) {
            // Find key
            pos = jsonStr.find('"', pos);
            if (pos == std::string::npos) break;
            
            size_t keyEnd = jsonStr.find('"', pos + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = jsonStr.substr(pos + 1, keyEnd - pos - 1);
            
            // Find value separator
            size_t colonPos = jsonStr.find(':', keyEnd);
            if (colonPos == std::string::npos) break;
            
            // Find next key or end
            size_t nextKey = jsonStr.find('"', colonPos);
            if (nextKey == std::string::npos) {
                nextKey = jsonStr.size();
            }
            
            std::string valueStr = jsonStr.substr(colonPos + 1, nextKey - colonPos - 1);
            
            // Trim whitespace
            key = trim(key);
            valueStr = trim(valueStr);
            
            // Try to determine value type
            ConfigValue value;
            bool parsed = false;
            
            // Try boolean
            if (valueStr == "true" || valueStr == "false") {
                value = (valueStr == "true");
                parsed = true;
            }
            // Try integer
            else if (isInteger(valueStr)) {
                value = std::stoll(valueStr);
                parsed = true;
            }
            // Try floating point
            else if (isDouble(valueStr)) {
                value = std::stod(valueStr);
                parsed = true;
            }
            // Try quoted string
            else if ((valueStr.front() == '"' && valueStr.back() == '"') ||
                    (valueStr.front() == '\'' && valueStr.back() == '\'')) {
                value = valueStr.substr(1, valueStr.size() - 2);
                parsed = true;
            }
            // Try JSON array
            else if (valueStr.front() == '[' && valueStr.back() == ']') {
                value = parseJsonArray(valueStr);
                parsed = true;
            }
            
            if (!parsed) {
                std::cerr << "Warning: Could not parse JSON value for key \"" << key 
                          << "\": \"" << valueStr << "\" - storing as string" << std::endl;
                value = valueStr;
            }
            
            set(key, value, ConfigSource::File);
            
            // Update position for next iteration
            pos = nextKey + 1;
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& content) {
    try {
        clear();
        
        std::istringstream stream(content);
        std::string line;
        int lineNum = 0;
        
        while (std::getline(stream, line)) {
            lineNum++;
            
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
            } else {
                std::cerr << "Warning: Line " << lineNum << " doesn't contain '=': " << line << std::endl;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Simple format parsing error: " << e.what() << std::endl;
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        // Write JSON header and opening brace
        file << "{\n";
        
        // Write configuration entries
        for (size_t i = 0; i < pImpl->entries.size(); ++i) {
            const auto& entry = pImpl->entries[i];
            
            file << "  \"" << entry.key << "\": ";
            
            // Serialize based on value type
            bool first = true;
            std::visit([&file, &first](const auto& val) {
                using T = std::decay_t<decltype(val)>;
                
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    file << val;
                } else if constexpr (std::is_same_v<T, double>) {
                    file << std::fixed << std::setprecision(6) << val;
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (val ? "true" : "false");
                } else if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << escapeJsonString(val) << "\"";
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    file << "[";
                    for (size_t j = 0; j < val.size(); ++j) {
                        if (!first) file << ", ";
                        file << val[j];
                        first = false;
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    file << "[";
                    for (size_t j = 0; j < val.size(); ++j) {
                        if (!first) file << ", ";
                        file << std::fixed << std::setprecision(6) << val[j];
                        first = false;
                    }
                    file << "]";
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    file << "[";
                    for (size_t j = 0; j < val.size(); ++j) {
                        if (!first) file << ", ";
                        file << "\"" << escapeJsonString(val[j]) << "\"";
                        first = false;
                    }
                    file << "]";
                }
            }, entry.value);
            
            // Add comment if present
            if (!entry.description.empty()) {
                file << " // " << entry.description;
            }
            
            if (i < pImpl->entries.size() - 1) {
                file << ",\n";
            } else {
                file << "\n";
            }
        }
        
        // Write closing brace
        file << "}\n";
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error saving config file " << filepath << ": " << e.what() << std::endl;
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::any_cast<T>(it->value);
    } catch (const std::bad_any_cast&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!validateOnSet || validateKeyValue(key, value)) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
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
        oss << anyToString(entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << "\n";
    }
    return oss.str();
}

bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateKeyValue(entry.key, entry.value)) {
            return false;
        }
    }
    return true;
}

std::string Config::getDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "";
    }
    
    return it->description;
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

std::string Config::configValueToJson(const ConfigValue& value) {
    std::ostringstream oss;
    bool first = true;
    
    std::visit([&oss, &first](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << val;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::fixed << std::setprecision(6) << val;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << escapeJsonString(val) << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (!first) oss << ", ";
                oss << val[i];
                first = false;
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (!first) oss << ", ";
                oss << std::fixed << std::setprecision(6) << val[i];
                first = false;
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (!first) oss << ", ";
                oss << "\"" << escapeJsonString(val[i]) << "\"";
                first = false;
            }
            oss << "]";
        }
    }, value);
    
    return oss.str();
}

ConfigValue Config::jsonToConfigValue(const std::string& jsonStr) {
    // Simple implementation - in practice, use nlohmann/json
    if (jsonStr == "true" || jsonStr == "false") {
        return jsonStr == "true";
    } else if (isInteger(jsonStr)) {
        return std::stoll(jsonStr);
    } else if (isDouble(jsonStr)) {
        return std::stod(jsonStr);
    } else if ((jsonStr.front() == '"' && jsonStr.back() == '"') ||
              (jsonStr.front() == '\'' && jsonStr.back() == '\'')) {
        return jsonStr.substr(1, jsonStr.size() - 2);
    } else {
        return jsonStr; // Default to string
    }
}

bool Config::parseJsonValue(const std::string& jsonStr, ConfigValue& outValue) {
    // Implementation for parsing JSON values
    // This is a simplified version
    try {
        outValue = jsonToConfigValue(jsonStr);
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<ConfigValue> Config::parseJsonArray(const std::string& jsonArray) {
    std::vector<ConfigValue> result;
    // Simplified array parsing - in practice use a proper JSON parser
    return result;
}

bool Config::isInteger(const std::string& str) {
    if (str.empty()) return false;
    size_t i = 0;
    if (str[0] == '-' || str[0] == '+') i++;
    for (; i < str.size(); ++i) {
        if (!std::isdigit(str[i])) return false;
    }
    return i > 0 || std::isdigit(str[0]);
}

bool Config::isDouble(const std::string& str) {
    try {
        std::stod(str);
        return true;
    } catch (...) {
        return false;
    }
}

std::string Config::escapeJsonString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '"': result += "\\\""; break;
            case '/': result += "\\/"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 0x20) {
                    result += "\\u" + std::to_string(static_cast<int>(c));
                } else {
                    result += c;
                }
                break;
        }
    }
    
    return result;
}

std::string Config::anyToString(const ConfigValue& value) {
    std::ostringstream oss;
    
    std::visit([&oss](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
            oss << val;
        } else if constexpr (std::is_same_v<T, double>) {
            oss << std::fixed << std::setprecision(6) << val;
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (val ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << val[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << std::fixed << std::setprecision(6) << val[i];
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < val.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << val[i] << "\"";
            }
            oss << "]";
        } else {
            oss << "<unknown type>";
        }
    }, value);
    
    return oss.str();
}

bool Config::validateKeyValue(const std::string& key, const ConfigValue& value) {
    // Basic validation - in practice, you'd have more sophisticated rules
    if (key.empty()) {
        std::cerr << "Error: Empty key is not allowed." << std::endl;
        return false;
    }
    
    // Check for invalid characters in key
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '.' && c != '-') {
            std::cerr << "Error: Key \"" << key << "\" contains invalid characters." << std::endl;
            return false;
        }
    }
    
    return true;
}

// Explicit template instantiations
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

} // namespace nlm
