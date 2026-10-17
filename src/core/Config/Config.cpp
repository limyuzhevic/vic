#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Phase 1: Try JSON/YAML parser first
    // Phase 2: Fall back to simple key=value format
    
    // Try JSON format first
    if (loadFromJSON(filepath)) {
        return true;
    }
    
    // Try YAML format second
    if (loadFromYAML(filepath)) {
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

bool Config::loadFromJSON(const std::string& filepath) {
    // Simple JSON parser for basic NLM configuration
    // Supports: { "key": value, "nested": { "key2": "value2" } }
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Basic JSON validation - just check if it looks like JSON
    if (content.find('{') == std::string::npos || content.find('}') == std::string::npos) {
        return false; // Not JSON format
    }
    
    // Simple JSON parsing for key-value pairs
    size_t pos = content.find('{');
    if (pos == std::string::npos) {
        return false;
    }
    
    pos++; // Skip '{'
    while (pos < content.length()) {
        // Find next key (quoted string)
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos) {
            break; // No more keys
        }
        
        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) {
            break; // Malformed JSON
        }
        
        std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        pos = content.find(':', keyEnd);
        if (pos == std::string::npos) {
            break; // Malformed JSON
        }
        
        pos++; // Skip ':'
        
        // Skip whitespace
        while (pos < content.length() && (content[pos] == ' ' || content[pos] == '\n' || 
                                          content[pos] == '\t' || content[pos] == '\r')) {
            pos++;
        }
        
        // Parse value based on its first character
        if (pos < content.length()) {
            if (content[pos] == '"') { // String value
                size_t valueEnd = content.find('"', pos + 1);
                if (valueEnd == std::string::npos) {
                    break;
                }
                std::string value = content.substr(pos + 1, valueEnd - pos - 1);
                set(key, value, ConfigSource::File);
                pos = valueEnd + 1;
            } else if (content[pos] == 't' || content[pos] == 'f') { // Boolean value
                // Simple boolean parsing
                if (content.substr(pos, 4) == "true") {
                    set(key, "true", ConfigSource::File);
                    pos += 4;
                } else if (content.substr(pos, 5) == "false") {
                    set(key, "false", ConfigSource::File);
                    pos += 5;
                } else {
                    break;
                }
            } else if (content[pos] >= '0' && content[pos] <= '9' || content[pos] == '-') { // Number
                // Parse number (int or float)
                size_t numEnd = pos;
                while (numEnd < content.length() && (content[numEnd] >= '0' && content[numEnd] <= '9' ||
                                                     content[numEnd] == '.' || content[numEnd] == '-' ||
                                                     content[numEnd] == 'e' || content[numEnd] == 'E')) {
                    numEnd++;
                }
                std::string value = content.substr(pos, numEnd - pos);
                set(key, value, ConfigSource::File);
                pos = numEnd;
            } else if (content[pos] == '{') { // Nested object (simplified - skip)
                // Find matching '}'
                int braceCount = 1;
                size_t nestedEnd = pos + 1;
                while (nestedEnd < content.length() && braceCount > 0) {
                    if (content[nestedEnd] == '{') {
                        braceCount++;
                    } else if (content[nestedEnd] == '}') {
                        braceCount--;
                    }
                    nestedEnd++;
                }
                // Skip nested object for now
                pos = nestedEnd;
            } else {
                // Unknown type, skip to next key
                while (pos < content.length() && content[pos] != ',') {
                    pos++;
                }
            }
        }
        
        // Skip to next key or closing brace
        while (pos < content.length() && content[pos] != '}' && content[pos] != ',') {
            pos++;
        }
        if (pos < content.length() && content[pos] == ',') {
            pos++; // Skip comma
        }
    }
    
    return true;
}

bool Config::loadFromYAML(const std::string& filepath) {
    // Simple YAML parser for basic NLM configuration
    // Supports: key: value, nested:
    //   key2: value2
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    std::string currentSection = "";
    
    while (std::getline(file, line)) {
        // Remove leading/trailing whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        size_t end = line.find_last_not_of(" \t\r\n");
        if (start == std::string::npos || end == std::string::npos) {
            continue; // Empty line
        }
        
        line = line.substr(start, end - start + 1);
        
        // Skip comments
        if (line.find("#") == 0) {
            continue;
        }
        
        // Check for section (key:)
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            // This is a key-value pair
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Handle YAML values with quotes
            if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
                                      (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
            currentSection = "";
        } else {
            // Continuation line (indented)
            if (!line.empty() && (line[0] == ' ' || line[0] == '\t')) {
                if (!currentSection.empty()) {
                    // This is a continuation of the previous key
                    set(currentSection, line, ConfigSource::File);
                }
            } else {
                currentSection = line;
            }
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

// Simple JSON value converter
std::string Config::valueToJSON(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        return "\"" + std::get<std::string>(value) + "\"";
    } else {
        return "\"unknown\"";
    }
}

std::string Config::valueToYAML(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) {
        return std::to_string(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::to_string(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::to_string(std::get<double>(value));
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? "true" : "false";
    } else if (std::holds_alternative<std::string>(value)) {
        // Quote strings that contain special characters or spaces
        const std::string& str = std::get<std::string>(value);
        if (str.find(' ') != std::string::npos || str.find(':') != std::string::npos ||
            str.find('#') != std::string::npos || str.find('{') != std::string::npos ||
            str.find('}') != std::string::npos) {
            return "\"" + str + "\"";
        } else {
            return str;
        }
    } else {
        return "\"unknown\"";
    }
}

// Helper to generate JSON file content
std::string Config::toJSON() const {
    std::string result = "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        result += "  \"" + entry.key + "\": " + valueToJSON(entry.value);
        if (i < pImpl->entries.size() - 1) {
            result += ",\n";
        } else {
            result += "\n";
        }
    }
    result += "}";
    return result;
}

// Helper to generate YAML file content
std::string Config::toYAML() const {
    std::string result = "";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        result += entry.key + ": " + valueToYAML(entry.value) + "\n";
    }
    return result;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine format based on file extension
    if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".json") {
        file << toJSON();
    } else if (filepath.size() >= 5 && filepath.substr(filepath.size() - 5) == ".yaml") {
        file << toYAML();
    } else if (filepath.size() >= 4 && filepath.substr(filepath.size() - 4) == ".yml") {
        file << toYAML();
    } else {
        // Default to simple key=value format
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
        }
    }
    
    return true;
}

} // namespace nlm
