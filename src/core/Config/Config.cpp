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

// Enhanced configuration file parsing with multiple format support
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try JSON format first
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0, std::ios::beg);
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.clear();
    
    if (isJsonFormat(content)) {
        return loadFromJson(content);
    } else {
        // Fall back to original key=value format
        file.clear();
        file.seekg(0, std::ios::beg);
        
        std::string line;
        while (std::getline(file, line)) {
            // Enhanced parsing with better error handling
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs
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
    }
    
    return true;
    
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
        // Handle boolean flags (--flag, -f)
        else if (arg[0] == '-') {
            std::string flag = arg.substr(1);
            set(flag, true, ConfigSource::CommandLine);
        }
        // Handle JSON-like arguments (--key:value)
        else if (arg.find(':') != std::string::npos && arg.substr(0, 2) == "--") {
            size_t pos = arg.find(':');
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
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

}

// Helper function to detect JSON format
bool Config::isJsonFormat(const std::string& content) {
    // Simple JSON detection: if it starts with { or [ and contains quotes and key-value pairs
    size_t braceCount = 0;
    size_t bracketCount = 0;
    bool hasQuotes = false;
    bool hasColon = false;
    
    for (size_t i = 0; i < content.length(); ++i) {
        char c = content[i];
        
        if (c == '{' || c == '[') {
            if (i == 0) {
                if (c == '{') braceCount++;
                else bracketCount++;
            }
        } else if (c == '}' || c == ']') {
            if (c == '}') braceCount++;
            else bracketCount++;
        } else if (c == '"') {
            hasQuotes = true;
        } else if (c == ':') {
            hasColon = true;
        }
    }
    
    // Simple heuristic: starts with { or [ and has quotes and colons
    return (content.find('{') == 0 || content.find('[') == 0) && 
           hasQuotes && hasColon && braceCount > 0 && bracketCount > 0;
}

// Simple JSON parser (basic implementation)
bool Config::loadFromJson(const std::string& jsonContent) {
    // Simple JSON parsing for basic key-value pairs
    // TODO: Implement full JSON parsing with nested objects and arrays
    // For now, handle simple {"key": "value", "key2": 123} format
    
    // Remove whitespace and newlines
    std::string content = jsonContent;
    content.erase(std::remove_if(content.begin(), content.end(), 
        [](char c) { return std::isspace(c); }), content.end());
    
    // Check if it's a valid object { ... }
    if (content.find('{') != 0 || content.back() != '}') {
        return false;
    }
    
    // Extract content between braces
    std::string inner = content.substr(1, content.length() - 2);
    
    // Parse key-value pairs
    size_t pos = 0;
    while (pos < inner.length()) {
        // Find next key (starts with ")
        size_t keyStart = inner.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = inner.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = inner.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Find colon after key
        size_t colonPos = inner.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        // Find value (skip whitespace)
        size_t valueStart = inner.find_first_not_of(" \t\n\r", colonPos + 1);
        if (valueStart == std::string::npos) break;
        
        // Find end of value (next comma or closing brace)
        size_t valueEnd = inner.find(',', valueStart);
        if (valueEnd == std::string::npos) {
            valueEnd = inner.find('}', valueStart);
        }
        if (valueEnd == std::string::npos) break;
        
        std::string value = inner.substr(valueStart, valueEnd - valueStart);
        
        // Clean up value (remove quotes)
        if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                  (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Try to parse value as different types
        parseAndSetValue(key, value);
        
        pos = valueEnd + 1;
        
        // Skip whitespace and comma
        while (pos < inner.length() && (inner[pos] == ' ' || inner[pos] == ',' || 
                                        inner[pos] == '\t' || inner[pos] == '\n')) {
            pos++;
        }
    }
    
    return true;
}

void Config::parseAndSetValue(const std::string& key, const std::string& valueStr) {
    // Try to parse value as different types
    // First try boolean
    if (valueStr == "true" || valueStr == "false") {
        bool val = (valueStr == "true");
        set(key, val, ConfigSource::File);
        return;
    }
    
    // Try integer
    try {
        size_t pos;
        int intVal = std::stoi(valueStr, &pos);
        if (pos == valueStr.length()) {
            set(key, intVal, ConfigSource::File);
            return;
        }
    } catch (...) {}
    
    // Try floating point
    try {
        size_t pos;
        double doubleVal = std::stod(valueStr, &pos);
        if (pos == valueStr.length()) {
            set(key, doubleVal, ConfigSource::File);
            return;
        }
    } catch (...) {}
    
    // Fall back to string
    set(key, valueStr, ConfigSource::File);
}
