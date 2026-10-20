#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_map>
#include <variant>
#include <string>
#include <vector>
#include <stdexcept>
#include <optional>

namespace nlm {

namespace {
    // Helper function to convert string to lowercase for case-insensitive comparison
    std::string toLowerCase(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    // Parse YAML value (simplified)
    ConfigValue parseYAMLValue(const std::string& valueStr) {
        std::string trimmed = trim(valueStr);
        if (trimmed.empty()) {
            return "";
        }
        
        // Handle quoted strings
        if ((trimmed.front() == '"' && trimmed.back() == '"') ||
            (trimmed.front() == '\'' && trimmed.back() == '\'')) {
            return trimmed.substr(1, trimmed.size() - 2);
        }
        
        // Handle boolean values
        if (trimmed == "true" || trimmed == "false") {
            return trimmed == "true";
        }
        
        // Handle numeric values
        try {
            size_t pos;
            int intVal = std::stoi(trimmed, &pos);
            if (pos == trimmed.size()) {
                return intVal;
            }
            
            int64_t int64Val = std::stoll(trimmed, &pos);
            if (pos == trimmed.size()) {
                return int64Val;
            }
            
            double doubleVal = std::stod(trimmed, &pos);
            if (pos == trimmed.size()) {
                return doubleVal;
            }
        } catch (const std::exception&) {
            // Not a number, treat as string
        }
        
        return trimmed;
    }
    
    // Parse simple key=value format (backward compatibility)
    bool parseSimpleFormat(const std::string& content, std::vector<ConfigEntry>& entries) {
        std::istringstream stream(content);
        std::string line;
        size_t lineNum = 0;
        
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
                
                // Parse the value
                ConfigValue parsedValue = parseYAMLValue(value);
                entries.emplace_back(key, parsedValue, ConfigSource::File, "");
            }
        }
        
        return !entries.empty();
    }
    
    // Parse JSON format
    bool parseJSONFormat(const std::string& content, std::vector<ConfigEntry>& entries) {
        // For now, implement a simple JSON parser
        // In a real implementation, we would use nlohmann/json or similar
        // This is a placeholder for the full JSON parsing implementation
        
        // Simple JSON parsing for key-value pairs
        // Note: This is a simplified implementation for demonstration
        size_t pos = 0;
        while (pos < content.size()) {
            // Find the next key
            size_t keyStart = content.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = content.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Find the colon
            size_t colonPos = content.find(':', keyEnd);
            if (colonPos == std::string::npos) break;
            
            // Find the value
            size_t valueStart = content.find_first_not_of(" \t\n\r", colonPos + 1);
            if (valueStart == std::string::npos) break;
            
            // Find end of value (either comma or brace)
            size_t valueEnd = content.find_first_of(",}", valueStart);
            if (valueEnd == std::string::npos) {
                valueEnd = content.size();
            }
            
            std::string valueStr = trim(content.substr(valueStart, valueEnd - valueStart));
            ConfigValue value = parseYAMLValue(valueStr);
            
            entries.emplace_back(key, value, ConfigSource::File, "");
            
            pos = valueEnd + 1;
        }
        
        return !entries.empty();
    }
    
    // Parse YAML format
    bool parseYAMLFormat(const std::string& content, std::vector<ConfigEntry>& entries) {
        // Simple YAML parsing for key: value format
        // This handles basic YAML key-value pairs
        size_t pos = 0;
        while (pos < content.size()) {
            // Find the next key (up to colon)
            size_t colonPos = content.find(':', pos);
            if (colonPos == std::string::npos) break;
            
            // Find key (look backward for non-whitespace)
            size_t keyEnd = colonPos;
            while (keyEnd > pos && std::isspace(content[keyEnd - 1])) {
                keyEnd--;
            }
            
            // Find start of key
            size_t keyStart = keyEnd;
            while (keyStart > pos && !std::isspace(content[keyStart - 1]) && 
                   content[keyStart - 1] != '\n') {
                keyStart--;
            }
            
            std::string key = content.substr(keyStart, keyEnd - keyStart);
            
            // Find value (look forward from colon)
            size_t valueStart = content.find_first_not_of(" \t\n\r", colonPos + 1);
            if (valueStart == std::string::npos) break;
            
            // Find end of value (either newline or next key)
            size_t lineEnd = content.find('\n', valueStart);
            if (lineEnd == std::string::npos) {
                lineEnd = content.size();
            }
            
            std::string valueStr = trim(content.substr(valueStart, lineEnd - valueStart));
            ConfigValue value = parseYAMLValue(valueStr);
            
            entries.emplace_back(key, value, ConfigSource::File, "");
            
            pos = lineEnd + 1;
        }
        
        return !entries.empty();
    }
    
    // Detect file format based on content
    std::string detectFormat(const std::string& content) {
        // Check if it looks like JSON
        size_t jsonStart = content.find('{');
        if (jsonStart != std::string::npos && content.find('}', jsonStart) != std::string::npos) {
            return "json";
        }
        
        // Check if it looks like YAML
        if (content.find(':') != std::string::npos) {
            // Simple heuristic for YAML-like structure
            return "yaml";
        }
        
        // Default to simple key=value format
        return "simple";
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchemaEntry> schema;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Configuration file loading methods
bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Clear existing entries
    pImpl->entries.clear();
    
    // Detect file format
    std::string format = detectFormat(content);
    bool success = false;
    
    // Try to parse based on detected format
    if (format == "json") {
        success = parseJSONFormat(content, pImpl->entries);
    } else if (format == "yaml") {
        success = parseYAMLFormat(content, pImpl->entries);
    } else {
        success = parseSimpleFormat(content, pImpl->entries);
    }
    
    if (!success) {
        // If parsing failed, keep empty config
        pImpl->entries.clear();
        return false;
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

bool Config::loadFromJSON(const std::string& jsonContent) {
    pImpl->entries.clear();
    return parseJSONFormat(jsonContent, pImpl->entries);
}

bool Config::loadFromYAML(const std::string& yamlContent) {
    pImpl->entries.clear();
    return parseYAMLFormat(yamlContent, pImpl->entries);
}

// Configuration file output methods
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save in JSON format for better readability and future compatibility
    file << "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Convert ConfigValue to JSON representation
        std::visit([&file](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeJSONString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::setprecision(6) << arg;
            } else {
                // For other types, just use string representation
                file << "\"" << arg << "\"";
            }
        }, entry.value);
    }
    
    file << "\n}\n";
    
    return true;
}

std::string Config::escapeJSONString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    // Control characters
                    result += "\\u";
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
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
    
    // Save in JSON format for better readability and future compatibility
    file << "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        
        // Convert ConfigValue to JSON representation
        std::visit([&file](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << escapeJSONString(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                file << std::setprecision(6) << arg;
            } else {
                // For other types, just use string representation
                file << "\"" << arg << "\"";
            }
        }, entry.value);
    }
    
    file << "\n}\n";
    
    return true;
}

std::string Config::escapeJSONString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    // Control characters
                    result += "\\u";
                    // In a real implementation, we would format the Unicode code point
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}

namespace nlm {

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return toLowerCase(e.key) == toLowerCase(key); });
    
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

// Add helper function for JSON escaping
static std::string escapeJSONString(const std::string& str) {
    std::string result;
    result.reserve(str.size());
    for (char c : str) {
        switch (c) {
            case '\\': result += "\\\\"; break;
            case '\"': result += "\\\""; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    // Control characters - simple escape
                    result += "\\u";
                    result += "00" + std::to_string(static_cast<int>(c));
                } else {
                    result += c;
                }
                break;
        }
    }
    return result;
}
