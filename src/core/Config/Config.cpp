#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <string>
#include <vector>
#include <map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // JSON/YAML parsing helpers
    std::map<std::string, std::string> parseSimpleJSON(const std::string& json) const;
    std::map<std::string, std::string> parseYAML(const std::string& yaml) const;
    bool isJSON(const std::string& content) const;
    bool isYAML(const std::string& content) const;
};

Config::Config() : pImpl(new Impl()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Determine file format based on extension and content
    std::string ext = std::filesystem::path(filepath).extension().string();
    bool isJsonFormat = isJSON(content) || (ext == ".json" || ext == ".JSON");
    bool isYamlFormat = isYAML(content) || (ext == ".yaml" || ext == ".yml" || ext == ".YAML" || ext == ".YML");
    
    // Parse based on format
    std::map<std::string, std::string> parsed;
    if (isJsonFormat) {
        parsed = pImpl->parseSimpleJSON(content);
    } else if (isYamlFormat) {
        parsed = pImpl->parseYAML(content);
    } else {
        // Fallback to simple key=value format for backward compatibility
        return loadFromSimpleFormat(content);
    }
    
    // Apply parsed values
    for (const auto& pair : parsed) {
        set(pair.first, pair.second, ConfigSource::File);
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
    
    // Save in simple key=value format for backward compatibility
    // This format is both human-readable and machine-parseable
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert variant value to string
        std::string valueStr;
        std::visit([&valueStr](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStr = "\"" + arg + "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                valueStr = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    valueStr += "\"" + arg[i] + "\"";
                    if (i < arg.size() - 1) valueStr += ", ";
                }
                valueStr += "]";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                valueStr = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    valueStr += std::to_string(arg[i]);
                    if (i < arg.size() - 1) valueStr += ", ";
                }
                valueStr += "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                valueStr = "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    valueStr += std::to_string(arg[i]);
                    if (i < arg.size() - 1) valueStr += ", ";
                }
                valueStr += "]";
            } else {
                valueStr = std::to_string(arg);
            }
        }, entry.value);
        
        file << entry.key << " = " << valueStr << "\n\n";
    }
    
    return true;
}

bool Config::loadFromSimpleFormat(const std::string& content) {
    // Parse simple key=value format (original implementation)
    // Keep this for backward compatibility with existing config files
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
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

std::map<std::string, std::string> Config::Impl::parseSimpleJSON(const std::string& json) const {
    std::map<std::string, std::string> result;
    
    // Simple JSON parsing for key-value pairs
    // Supports basic JSON objects: {"key": "value", "key2": 123, "key3": true}
    
    size_t pos = 0;
    while (pos < json.size()) {
        // Skip whitespace
        while (pos < json.size() && std::isspace(json[pos])) pos++;
        if (pos >= json.size()) break;
        
        // Skip opening brace
        if (json[pos] == '{') {
            pos++;
            continue;
        }
        
        // Skip closing brace
        if (json[pos] == '}') {
            pos++;
            continue;
        }
        
        // Find key name (string in quotes)
        if (json[pos] == '"') {
            pos++;
            size_t keyStart = pos;
            while (pos < json.size() && json[pos] != '"') {
                pos++;
            }
            std::string key = json.substr(keyStart, pos - keyStart);
            pos++; // Skip closing quote
            
            // Skip whitespace
            while (pos < json.size() && std::isspace(json[pos])) pos++;
            if (pos >= json.size() || json[pos] != ':') break;
            pos++; // Skip colon
            
            // Skip whitespace
            while (pos < json.size() && std::isspace(json[pos])) pos++;
            
            // Parse value based on type
            if (pos < json.size() && json[pos] == '"') {
                // String value
                pos++;
                size_t valueStart = pos;
                while (pos < json.size() && json[pos] != '"') {
                    pos++;
                }
                std::string value = json.substr(valueStart, pos - valueStart);
                result[key] = value;
                pos++; // Skip closing quote
            } else if (pos < json.size() && json[pos] == 't' || json[pos] == 'f') {
                // Boolean value
                size_t valueStart = pos;
                while (pos < json.size() && (std::isalnum(json[pos]) || json[pos] == '.' || json[pos] == '-')) {
                    pos++;
                }
                std::string value = json.substr(valueStart, pos - valueStart);
                result[key] = value;
            } else if (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '-')) {
                // Numeric value
                size_t valueStart = pos;
                while (pos < json.size() && (std::isdigit(json[pos]) || json[pos] == '.' || json[pos] == '-' || json[pos] == 'e' || json[pos] == 'E')) {
                    pos++;
                }
                std::string value = json.substr(valueStart, pos - valueStart);
                result[key] = value;
            } else {
                // Skip to next comma or closing brace
                while (pos < json.size() && json[pos] != ',' && json[pos] != '}') {
                    pos++;
                }
            }
            
            // Skip comma
            while (pos < json.size() && std::isspace(json[pos])) pos++;
            if (pos < json.size() && json[pos] == ',') {
                pos++;
                while (pos < json.size() && std::isspace(json[pos])) pos++;
            }
        }
    }
    
    return result;
}

std::map<std::string, std::string> Config::Impl::parseYAML(const std::string& yaml) const {
    std::map<std::string, std::string> result;
    
    // Simple YAML parsing for key-value pairs
    // Supports basic YAML format: key: value
    std::istringstream stream(yaml);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '%') {
            continue;
        }
        
        // Find key-value pair separated by colon
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Remove quotes from value if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            result[key] = value;
        }
    }
    
    return result;
}

bool Config::Impl::isJSON(const std::string& content) const {
    size_t pos = content.find_first_not_of(" \t\n\r");
    if (pos >= content.size() || content[pos] != '{') {
        return false;
    }
    
    // Simple heuristic: count braces to ensure they're balanced
    int braceCount = 0;
    for (char c : content) {
        if (c == '{') braceCount++;
        else if (c == '}') braceCount--;
        if (braceCount < 0) return false;
    }
    
    return braceCount == 0;
}

bool Config::Impl::isYAML(const std::string& content) const {
    // Simple heuristic: check for YAML-like patterns
    // YAML typically has key: value pairs
    for (size_t i = 0; i < content.size(); ++i) {
        if (content[i] != ' ' && content[i] != '\t' && content[i] != '\n' && content[i] != '\r') {
            // Found non-whitespace character
            // Look ahead for colon separator (YAML key-value)
            size_t colonPos = content.find(':', i);
            if (colonPos != std::string::npos) {
                size_t nextNonWhitespace = colonPos + 1;
                while (nextNonWhitespace < content.size() && 
                       (content[nextNonWhitespace] == ' ' || content[nextNonWhitespace] == '\t')) {
                    nextNonWhitespace++;
                }
                // Valid YAML pattern if there's content after colon
                if (nextNonWhitespace < content.size() && content[nextNonWhitespace] != '\n' && content[nextNonWhitespace] != '\r') {
                    return true;
                }
            }
            break;
        }
    }
    return false;
}

// Rest of the Config methods remain unchanged...

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
// (Same as before...)

} // namespace nlm
