#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <unordered_map>
#include <vector>
#include <memory>
#include <string>

namespace nlm {

// JSON parsing helper functions
namespace json_parser {

    // Helper function to trim whitespace from a string
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }

    // Helper function to check if a character is whitespace
    bool isWhitespace(char c) {
        return std::isspace(static_cast<unsigned char>(c));
    }

    // Helper function to parse a JSON value
    ConfigValue parseJsonValue(const std::string& valueStr) {
        std::string trimmed = trim(valueStr);
        
        if (trimmed.empty()) {
            return "";
        }
        
        if (trimmed.front() == '"' && trimmed.back() == '"') {
            // String value
            return trimmed.substr(1, trimmed.size() - 2);
        }
        
        if (trimmed == "true") {
            return true;
        }
        
        if (trimmed == "false") {
            return false;
        }
        
        // Try to parse as number
        try {
            // Check if it's an integer
            size_t pos;
            long long intVal = std::stoll(trimmed, &pos);
            if (pos == trimmed.size()) {
                return intVal;
            }
            
            // Try double
            double doubleVal = std::stod(trimmed, &pos);
            if (pos == trimmed.size()) {
                return doubleVal;
            }
            
            return trimmed;
        } catch (...) {
            return trimmed;
        }
    }

    // Helper function to parse a JSON array
    std::vector<ConfigValue> parseJsonArray(const std::string& arrayStr) {
        std::vector<ConfigValue> result;
        size_t pos = 1; // Skip opening bracket
        size_t start = pos;
        
        while (pos < arrayStr.size()) {
            char c = arrayStr[pos];
            
            if (c == '[') {
                // Nested array, skip for now (not supported)
                size_t bracketCount = 1;
                while (bracketCount > 0 && ++pos < arrayStr.size()) {
                    if (arrayStr[pos] == '[') bracketCount++;
                    else if (arrayStr[pos] == ']') bracketCount--;
                }
                start = pos + 1;
            } else if (c == ']') {
                // End of array
                if (start < pos) {
                    std::string elementStr = arrayStr.substr(start, pos - start);
                    result.push_back(parseJsonValue(elementStr));
                }
                break;
            } else if (c == ',') {
                // End of element
                if (start < pos) {
                    std::string elementStr = arrayStr.substr(start, pos - start);
                    result.push_back(parseJsonValue(elementStr));
                }
                start = pos + 1;
            }
            
            pos++;
        }
        
        return result;
    }

    // Parse JSON string into ConfigValue
    ConfigValue parseJsonString(const std::string& jsonStr) {
        std::string str = trim(jsonStr);
        
        if (str.empty()) {
            return "";
        }
        
        if (str.front() == '{') {
            // Object (key-value pairs) - not directly supported, return as string
            return str;
        } else if (str.front() == '[') {
            // Array
            return parseJsonArray(str);
        } else {
            // Simple value
            return parseJsonValue(str);
        }
    }

    // Extract key-value pairs from a JSON object
    std::unordered_map<std::string, std::string> parseJsonObject(const std::string& jsonStr) {
        std::unordered_map<std::string, std::string> result;
        
        size_t pos = 1; // Skip opening brace
        size_t start = pos;
        
        while (pos < jsonStr.size() && jsonStr.front() == '{') {
            char c = jsonStr[pos];
            
            if (c == '"') {
                // Key found
                size_t keyEnd = jsonStr.find('"', pos + 1);
                if (keyEnd == std::string::npos) break;
                
                std::string key = jsonStr.substr(pos + 1, keyEnd - pos - 1);
                
                // Find colon after key
                size_t colonPos = jsonStr.find(':', keyEnd);
                if (colonPos == std::string::npos) break;
                
                // Find value (skip whitespace)
                size_t valuePos = jsonStr.find_first_not_of(" \t\r\n", colonPos + 1);
                if (valuePos == std::string::npos) break;
                
                if (jsonStr[valuePos] == '"') {
                    // String value
                    size_t valueEnd = jsonStr.find('"', valuePos + 1);
                    if (valueEnd == std::string::npos) break;
                    
                    std::string value = jsonStr.substr(valuePos + 1, valueEnd - valuePos - 1);
                    result[key] = value;
                    
                    pos = valueEnd + 1;
                } else {
                    // Numeric or boolean value
                    size_t valueEnd = jsonStr.find_first_of(",}", valuePos);
                    if (valueEnd == std::string::npos) valueEnd = jsonStr.size();
                    
                    std::string value = jsonStr.substr(valuePos, valueEnd - valuePos);
                    result[key] = value;
                    
                    pos = valueEnd;
                }
                
                start = pos + 1;
            } else if (c == '}') {
                // End of object
                break;
            } else if (c == ',') {
                // Skip comma
                pos++;
                start = pos + 1;
            }
            
            pos++;
        }
        
        return result;
    }

} // namespace json_parser

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file format based on extension and content
    std::string extension = ".";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos && dotPos < filepath.size() - 1) {
        extension = filepath.substr(dotPos + 1);
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();
    
    // Remove comments (lines starting with # or //)
    std::string processedContent;
    size_t pos = 0;
    while (pos < content.size()) {
        size_t lineEnd = content.find('\n', pos);
        if (lineEnd == std::string::npos) {
            lineEnd = content.size();
        }
        
        std::string line = content.substr(pos, lineEnd - pos);
        
        // Remove comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        commentPos = line.find("//");
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        // Add to processed content
        if (!line.empty()) {
            if (!processedContent.empty()) {
                processedContent += '\n';
            }
            processedContent += line;
        }
        
        pos = lineEnd + 1;
    }
    
    // Detect if it's JSON or YAML or simple key=value format
    std::string normalizedContent = json_parser::trim(processedContent);
    bool isJson = false;
    bool isYaml = false;
    
    if (normalizedContent.empty()) {
        return true; // Empty file
    }
    
    // Check for JSON indicators
    if ((normalizedContent.front() == '{' && normalizedContent.back() == '}') ||
        (normalizedContent.front() == '[' && normalizedContent.back() == ']')) {
        isJson = true;
    }
    
    // Check for YAML indicators (has key-value pairs with colons)
    if (normalizedContent.find(':') != std::string::npos &&
        (normalizedContent.find(':') < 100 || extension == "yaml" || extension == "yml")) {
        // Could be YAML, check for proper indentation
        size_t colonPos = normalizedContent.find(':');
        if (colonPos > 0 && colonPos < normalizedContent.size() - 1) {
            // Check if there's text before and after colon
            std::string before = json_parser::trim(normalizedContent.substr(0, colonPos));
            std::string after = json_parser::trim(normalizedContent.substr(colonPos + 1));
            if (!before.empty() && !after.empty()) {
                isYaml = true;
            }
        }
    }
    
    // Parse based on format
    if (isJson) {
        // Parse as JSON
        auto keyValuePairs = json_parser::parseJsonObject(normalizedContent);
        for (const auto& pair : keyValuePairs) {
            ConfigValue configValue = json_parser::parseJsonString(pair.second);
            set(pair.first, configValue, ConfigSource::File);
        }
    } else if (isYaml) {
        // Parse as YAML (basic implementation)
        auto keyValuePairs = json_parser::parseJsonObject(normalizedContent);
        for (const auto& pair : keyValuePairs) {
            ConfigValue configValue = json_parser::parseJsonString(pair.second);
            set(pair.first, configValue, ConfigSource::File);
        }
    } else {
        // Parse as simple key=value format (backward compatibility)
        std::istringstream stream(processedContent);
        std::string line;
        while (std::getline(stream, line)) {
            line = json_parser::trim(line);
            if (line.empty()) {
                continue;
            }
            
            // Parse simple key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = json_parser::trim(line.substr(0, pos));
                std::string value = json_parser::trim(line.substr(pos + 1));
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
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
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert ConfigValue to string representation
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
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
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
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
