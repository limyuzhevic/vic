#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), lastErrorMessage() {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper functions for file format detection
inline bool endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

inline bool containsJSONBraces(const std::string& content) {
    return content.find('{') != std::string::npos && content.find('}') != std::string::npos;
}

inline bool containsYAMLDash(const std::string& content) {
    // Check for YAML-like indentation with dash
    std::istringstream stream(content);
    std::string line;
    while (std::getline(stream, line)) {
        // Trim whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        if (start != std::string::npos) {
            line = line.substr(start);
            if (line.size() >= 2 && line[0] == '-' && line[1] == ' ') {
                return true;
            }
        }
    }
    return false;
}

// Helper function for trimming whitespace
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Helper function for converting to lowercase
std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Helper function to parse simple key=value format
bool parseSimpleFormat(const std::string& content, std::vector<ConfigEntry>& entries) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = Config::trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = Config::trim(line.substr(0, pos));
            std::string value = Config::trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Convert to appropriate type
            ConfigEntry entry;
            entry.key = key;
            entry.source = ConfigSource::File;
            
            // Try to determine type
            try {
                if (value == "true" || value == "false") {
                    entry.value = (value == "true");
                } else {
                    // Try to parse as number
                    if (value.find('.') != std::string::npos || value.find('e') != std::string::npos || 
                        value.find('E') != std::string::npos) {
                        entry.value = std::stod(value);
                    } else {
                        entry.value = std::stoll(value);
                    }
                }
            } catch (...) {
                entry.value = value;
            }
            
            entries.push_back(entry);
        }
    }
    
    return !entries.empty();
}

// Simple JSON parser for basic objects
bool parseJSON(const std::string& content, std::vector<ConfigEntry>& entries) {
    // Very simple JSON object parser for basic key-value pairs
    // Skip leading whitespace
    auto trim = [](std::string& s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
            return !std::isspace(ch);
        }));
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
        }).base(), s.end());
    };
    
    // Find opening brace
    size_t pos = 0;
    while (pos < content.size() && content[pos] != '{') {
        pos++;
    }
    
    if (pos >= content.size()) {
        return false; // Not a JSON object
    }
    
    pos++; // Skip opening brace
    
    while (pos < content.size()) {
        // Find next key
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        trim(key);
        
        // Skip whitespace
        size_t colonPos = content.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        size_t valuePos = colonPos + 1;
        
        // Find value
        size_t valueEnd;
        if (content[valuePos] == '"') { // String value
            valueEnd = content.find('"', valuePos + 1);
            if (valueEnd == std::string::npos) break;
            std::string value = content.substr(valuePos + 1, valueEnd - valuePos - 1);
            ConfigEntry entry(key, value, ConfigSource::File);
            entries.push_back(entry);
        } else if (content[valuePos] == '{' || content[valuePos] == '[') { // Complex value
            // Skip to end of value for now
            valueEnd = content.find('}', valuePos);
            if (valueEnd == std::string::npos) {
                valueEnd = content.find(']', valuePos);
            }
            if (valueEnd == std::string::npos) break;
            // For now, just store as string representation
            std::string value = content.substr(valuePos, valueEnd - valuePos + 1);
            ConfigEntry entry(key, value, ConfigSource::File);
            entries.push_back(entry);
        } else { // Number or boolean
            // Find end of value (comma, brace, or end of string)
            size_t commaPos = content.find(',', valuePos);
            size_t bracePos = content.find('}', valuePos);
            size_t bracketPos = content.find(']', valuePos);
            
            valueEnd = std::min({commaPos, bracePos, bracketPos, content.size()});
            
            if (valueEnd == std::string::npos) break;
            
            std::string valueStr = content.substr(valuePos, valueEnd - valuePos);
            trim(valueStr);
            
            // Convert to appropriate type
            ConfigEntry entry;
            entry.key = key;
            entry.source = ConfigSource::File;
            
            // Try to parse as boolean
            if (valueStr == "true") {
                entry.value = true;
            } else if (valueStr == "false") {
                entry.value = false;
            } else {
                try {
                    if (valueStr.find('.') != std::string::npos || valueStr.find('e') != std::string::npos || 
                        valueStr.find('E') != std::string::npos) {
                        entry.value = std::stod(valueStr);
                    } else {
                        entry.value = std::stoll(valueStr);
                    }
                } catch (...) {
                    entry.value = valueStr;
                }
            }
            
            entries.push_back(entry);
        }
        
        // Skip to next entry
        pos = valueEnd + 1;
        
        // Skip comma or whitespace
        while (pos < content.size() && (content[pos] == ',' || std::isspace(content[pos]))) {
            pos++;
        }
        
        if (pos < content.size() && content[pos] == '}') {
            break;
        }
    }
    
    return !entries.empty();
}

// Simple YAML parser for key: value pairs
bool parseYAML(const std::string& content, std::vector<ConfigEntry>& entries) {
    // Very basic YAML parser that handles simple key: value pairs
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = Config::trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '%') {
            continue;
        }
        
        // Check for YAML key: value format
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos && colonPos > 0) {
            std::string key = Config::trim(line.substr(0, colonPos));
            std::string value = Config::trim(line.substr(colonPos + 1));
            
            // Remove quotes from value if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Convert to appropriate type
            ConfigEntry entry;
            entry.key = key;
            entry.source = ConfigSource::File;
            
            // Try to parse boolean
            if (value == "true") {
                entry.value = true;
            } else if (value == "false") {
                entry.value = false;
            } else {
                try {
                    if (value.find('.') != std::string::npos || value.find('e') != std::string::npos || 
                        value.find('E') != std::string::npos) {
                        entry.value = std::stod(value);
                    } else {
                        entry.value = std::stoll(value);
                    }
                } catch (...) {
                    entry.value = value;
                }
            }
            
            entries.push_back(entry);
        }
    }
    
    return !entries.empty();
}

// Set error message
void Config::setError(const std::string& message, ConfigErrorType type) {
    lastErrorMessage = message;
}

bool Config::loadFromFile(const std::string& filepath) {
    clearError();  // Clear any previous errors
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        setError("Cannot open configuration file: " + filepath, ConfigErrorType::FileNotFound);
        return false;
    }
    
    // Read the entire file
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    if (content.empty()) {
        setError("Configuration file is empty: " + filepath, ConfigErrorType::ParseError);
        return false;
    }
    
    // Try JSON first (if file ends with .json or contains { } )
    if (endsWith(filepath, ".json") || containsJSONBraces(content)) {
        return loadFromJSON(content, ConfigSource::File);
    }
    // Try YAML next (if file ends with .yaml or .yml, or contains YAML-like content)
    else if (endsWith(filepath, ".yaml") || endsWith(filepath, ".yml") || containsYAMLDash(content)) {
        return loadFromYAML(content, ConfigSource::File);
    }
    // Fall back to key=value format for backward compatibility
    else {
        return loadFromSimpleFormat(content, ConfigSource::File);
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
        setError("Error parsing command line arguments: " + std::string(e.what()), ConfigErrorType::ParseError);
        return false;
    } catch (...) {
        setError("Unknown error parsing command line arguments", ConfigErrorType::ParseError);
        return false;
    }
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
    
    return !file.fail();
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

bool Config::loadFromJSON(const std::string& content, ConfigSource source) {
    std::vector<ConfigEntry> entries;
    if (!parseJSON(content, entries)) {
        setError("Failed to parse JSON configuration", ConfigErrorType::ParseError);
        return false;
    }
    
    // Merge entries (overwriting existing keys)
    for (const auto& entry : entries) {
        set(entry.key, entry.value, source);
    }
    
    return true;
}

bool Config::loadFromYAML(const std::string& content, ConfigSource source) {
    std::vector<ConfigEntry> entries;
    if (!parseYAML(content, entries)) {
        setError("Failed to parse YAML configuration", ConfigErrorType::ParseError);
        return false;
    }
    
    // Merge entries (overwriting existing keys)
    for (const auto& entry : entries) {
        set(entry.key, entry.value, source);
    }
    
    return true;
}

bool Config::loadFromSimpleFormat(const std::string& content, ConfigSource source) {
    std::vector<ConfigEntry> entries;
    if (!parseSimpleFormat(content, entries)) {
        setError("Failed to parse simple configuration format", ConfigErrorType::ParseError);
        return false;
    }
    
    // Merge entries (overwriting existing keys)
    for (const auto& entry : entries) {
        set(entry.key, entry.value, source);
    }
    
    return true;
}

} // namespace nlm