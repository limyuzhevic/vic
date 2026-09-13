#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <unordered_map>
#include <stdexcept>
#include <variant>
#include <map>
#include <string>

namespace nlm {

// Simple JSON parser for config files
class SimpleJSONParser {
public:
    static bool parseFile(const std::string& filepath, std::map<std::string, std::string>& configMap) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        
        // Parse JSON-like content (simplified)
        // Supports: {"key": "value", "nested": {"subkey": 123}}
        configMap.clear();
        
        // Remove whitespace
        std::string cleanContent = removeWhitespace(content);
        
        if (cleanContent.empty() || cleanContent[0] != '{') {
            return false;
        }
        
        // Parse top-level object
        parseJSONObject(cleanContent, 0, configMap);
        return true;
    }
    
    static bool parseContent(const std::string& content, std::map<std::string, std::string>& configMap) {
        std::string cleanContent = removeWhitespace(content);
        
        if (cleanContent.empty() || cleanContent[0] != '{') {
            return false;
        }
        
        parseJSONObject(cleanContent, 0, configMap);
        return true;
    }
    
private:
    static std::string removeWhitespace(const std::string& str) {
        std::string result;
        result.reserve(str.size());
        for (char c : str) {
            if (!std::isspace(c)) {
                result.push_back(c);
            }
        }
        return result;
    }
    
    static void parseJSONObject(const std::string& content, size_t start, std::map<std::string, std::string>& configMap) {
        size_t pos = start + 1;  // Skip '{'
        while (pos < content.size() && content[pos] != '}') {
            // Parse key
            size_t keyStart = content.find('"', pos);
            if (keyStart == std::string::npos) break;
            size_t keyEnd = content.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Skip whitespace and colon
            pos = keyEnd + 1;
            while (pos < content.size() && (content[pos] == ' ' || content[pos] == '\t' || content[pos] == ':')) pos++;
            
            if (pos >= content.size()) break;
            
            // Parse value
            if (content[pos] == '"') {
                // String value
                size_t valueStart = pos;
                size_t valueEnd = content.find('"', valueStart + 1);
                if (valueEnd == std::string::npos) break;
                std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
                configMap[key] = value;
                pos = valueEnd + 1;
            } else if (std::isdigit(content[pos]) || content[pos] == '-' || content[pos] == '+') {
                // Number value
                size_t valueEnd = content.find_first_of(",}", pos);
                if (valueEnd == std::string::npos) break;
                std::string value = content.substr(pos, valueEnd - pos);
                configMap[key] = value;
                pos = valueEnd;
            } else if (content[pos] == '{') {
                // Nested object - flatten with prefix
                std::map<std::string, std::string> nestedMap;
                parseJSONObject(content, pos, nestedMap);
                // Flatten nested map with key prefix
                for (const auto& nested : nestedMap) {
                    std::string prefixedKey = key + "." + nested.first;
                    configMap[prefixedKey] = nested.second;
                }
                // Find closing brace
                pos = content.find('}', pos);
                if (pos == std::string::npos) break;
                pos++;
            }
            
            // Skip whitespace and comma
            while (pos < content.size() && (content[pos] == ' ' || content[pos] == ',')) {
                pos++;
            }
        }
    }
};

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::mutex configMutex;  // For thread safety
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON format first (Phase 2 implementation)
    std::map<std::string, std::string> jsonConfig;
    if (SimpleJSONParser::parseFile(filepath, jsonConfig)) {
        // Convert flattened JSON keys to proper format for set()
        for (const auto& pair : jsonConfig) {
            const std::string& key = pair.first;
            const std::string& value = pair.second;
            
            // Try to detect type and set appropriate value
            try {
                // Check if it's a boolean
                if (value == "true" || value == "false") {
                    bool boolValue = (value == "true");
                    set(key, boolValue, ConfigSource::File);
                } else if (value.find('.') != std::string::npos) {
                    // Check if it's a float
                    float floatValue = std::stof(value);
                    set(key, floatValue, ConfigSource::File);
                } else if (std::all_of(value.begin(), value.end(), ::isdigit) || 
                          (value[0] == '-' && std::all_of(value.begin() + 1, value.end(), ::isdigit))) {
                    // Integer
                    int intValue = std::stoi(value);
                    set(key, intValue, ConfigSource::File);
                } else {
                    // String
                    set(key, value, ConfigSource::File);
                }
            } catch (const std::exception&) {
                // If conversion fails, treat as string
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
    
    // Fall back to simple key=value format (Phase 1)
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
        // Use proper formatting based on type
        std::visit([&file](const auto& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Quote strings
                file << entry.key << " = \"" << value << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (value ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, double>) {
                file << entry.key << " = " << value << "\n";
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                file << entry.key << " = " << value << "\n";
            } else {
                // Fallback
                file << entry.key << " = " << value << "\n";
            }
        }, entry.value);
    }
    
    return true;
}

// Thread-safe versions of key methods

std::optional<std::string> Config::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<std::string>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

bool Config::has(const std::string& key) const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->configMutex);
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
