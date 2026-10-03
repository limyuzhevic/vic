#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

// ConfigFileFormat enum moved from Config.hpp

// Format detection helpers
static ConfigFileFormat detectFormatByExtension(const std::string& filepath);
static ConfigFileFormat detectFormatByContent(const std::string& content);
static bool isJSON(const std::string& content);
static bool isYAML(const std::string& content);

// Parser helpers
static std::string trimYamlIndent(const std::string& line);
static bool startsWith(const std::string& str, const std::string& prefix);

// JSON parsing
static bool parseJSON(const std::string& content, std::vector<ConfigEntry>& entries);

// YAML parsing
static bool parseYAML(const std::string& content, std::vector<ConfigEntry>& entries);

// Format conversion helpers
static std::vector<ConfigEntry> flattenNestedJSON(const std::vector<std::pair<std::string, ConfigValue>>& nested);
static std::vector<ConfigEntry> flattenNestedYAML(const std::vector<std::pair<std::string, ConfigValue>>& nested);
static ConfigValue stringToValue(const std::string& str);
static bool stringToBool(const std::string& str);
static int stringToInt(const std::string& str);
static double stringToDouble(const std::string& str);
static std::vector<std::string> parseYAMLArray(const std::string& str);
static ConfigValue parseYAMLValue(const std::string& str);

// Simple key=value format parser
static bool parseKeyValue(const std::string& content, std::vector<ConfigEntry>& entries);

// Main implementation starts below

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

// Original loadFromFile method (now enhanced)
bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFile(filepath, ConfigFileFormat::AutoDetect);
}

bool Config::loadFromFile(const std::string& filepath, ConfigFileFormat format) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read entire file for format detection
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Determine format
    ConfigFileFormat detectedFormat = format;
    if (detectedFormat == ConfigFileFormat::AutoDetect) {
        detectedFormat = detectFormatByContent(content);
    }
    
    // Clear existing entries
    clear();
    
    // Parse based on detected format
    std::vector<ConfigEntry> entries;
    bool success = false;
    
    switch (detectedFormat) {
        case ConfigFileFormat::KeyValue:
            success = parseKeyValue(content, entries);
            break;
        case ConfigFileFormat::JSON:
            success = parseJSON(content, entries);
            break;
        case ConfigFileFormat::YAML:
            success = parseYAML(content, entries);
            break;
        default:
            success = parseKeyValue(content, entries);
            break;
    }
    
    // Add all entries to config
    if (success) {
        for (const auto& entry : entries) {
            set(entry.key, entry.value, ConfigSource::File);
        }
    }
    
    return success;
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

} // namespace nlm

// ConfigFileFormat enum implementation
namespace nlm {

// Format detection implementation
ConfigFileFormat detectFormatByExtension(const std::string& filepath) {
    // Check file extension
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json" || extension == ".cfg") {
        return ConfigFileFormat::JSON;
    } else if (extension == ".yaml" || extension == ".yml" || extension == ".ini") {
        return ConfigFileFormat::YAML;
    } else if (extension == ".cfg" || extension == ".ini") {
        return ConfigFileFormat::KeyValue;
    } else if (extension == ".txt") {
        return ConfigFileFormat::KeyValue;
    } else {
        return ConfigFileFormat::AutoDetect;
    }
}

ConfigFileFormat detectFormatByContent(const std::string& content) {
    if (isJSON(content)) {
        return ConfigFileFormat::JSON;
    } else if (isYAML(content)) {
        return ConfigFileFormat::YAML;
    } else {
        return ConfigFileFormat::KeyValue;
    }
}

bool isJSON(const std::string& content) {
    if (content.empty()) return false;
    
    std::string trimmed = trim(content);
    
    // Skip whitespace and comments
    size_t pos = 0;
    while (pos < trimmed.size()) {
        if (trimmed[pos] == '/' && pos + 1 < trimmed.size() && trimmed[pos + 1] == '/') {
            // Skip line comment
            size_t newline = trimmed.find('\n', pos);
            if (newline == std::string::npos) break;
            pos = newline + 1;
            continue;
        } else if (trimmed[pos] == '/' && pos + 1 < trimmed.size() && trimmed[pos + 1] == '*') {
            // Skip block comment
            size_t end = trimmed.find("*/", pos + 2);
            if (end == std::string::npos) break;
            pos = end + 2;
            continue;
        } else if (trimmed[pos] == ' ' || trimmed[pos] == '\t' || trimmed[pos] == '\n' || trimmed[pos] == '\r') {
            pos++;
            continue;
        } else {
            break;
        }
    }
    
    if (pos >= trimmed.size()) return false;
    
    return trimmed[pos] == '{' || trimmed[pos] == '[';
}

bool isYAML(const std::string& content) {
    if (content.empty()) return false;
    
    std::string trimmed = trim(content);
    
    // Skip whitespace and comments
    size_t pos = 0;
    while (pos < trimmed.size()) {
        if (trimmed[pos] == '/' && pos + 1 < trimmed.size() && trimmed[pos + 1] == '/') {
            // Skip line comment
            size_t newline = trimmed.find('\n', pos);
            if (newline == std::string::npos) break;
            pos = newline + 1;
            continue;
        } else if (trimmed[pos] == '/' && pos + 1 < trimmed.size() && trimmed[pos + 1] == '*') {
            // Skip block comment
            size_t end = trimmed.find("*/", pos + 2);
            if (end == std::string::npos) break;
            pos = end + 2;
            continue;
        } else if (trimmed[pos] == ' ' || trimmed[pos] == '\t' || trimmed[pos] == '\n' || trimmed[pos] == '\r') {
            pos++;
            continue;
        } else {
            break;
        }
    }
    
    if (pos >= trimmed.size()) return false;
    
    // Check for YAML patterns: key: value or - array item
    size_t newline = trimmed.find('\n', pos);
    if (newline == std::string::npos) newline = trimmed.size();
    
    std::string line = trimmed.substr(pos, newline - pos);
    if (line.empty()) return false;
    
    // Skip leading whitespace
    size_t firstNonSpace = line.find_first_not_of(" \t");
    if (firstNonSpace == std::string::npos) return false;
    
    // Check if line contains a colon (YAML key:value pattern)
    size_t colonPos = line.find(':', firstNonSpace);
    if (colonPos != std::string::npos) {
        return true;
    }
    
    // Check for array start
    if (line[firstNonSpace] == '-') {
        return true;
    }
    
    return false;
}

// Key=value parser implementation
bool parseKeyValue(const std::string& content, std::vector<ConfigEntry>& entries) {
    std::stringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Convert value to appropriate type
            ConfigValue configValue = stringToValue(value);
            
            entries.emplace_back(key, configValue, ConfigSource::File);
        }
    }
    
    return true;
}

// JSON parsing implementation
bool parseJSON(const std::string& content, std::vector<ConfigEntry>& entries) {
    // Simplified JSON parsing - looks for key-value patterns
    // This is a basic implementation without external dependencies
    
    std::string stripped = trim(content);
    
    // Remove surrounding braces/brackets
    if (stripped.size() >= 2 && ((stripped[0] == '{' && stripped.back() == '}') || 
                                 (stripped[0] == '[' && stripped.back() == ']'))) {
        stripped = stripped.substr(1, stripped.size() - 2);
    }
    
    // Parse key:value pairs (very basic)
    size_t pos = 0;
    while (pos < stripped.size()) {
        // Find next quote
        size_t quoteStart = stripped.find('\"', pos);
        if (quoteStart == std::string::npos) break;
        
        // Find end quote
        size_t quoteEnd = stripped.find('\"', quoteStart + 1);
        if (quoteEnd == std::string::npos) break;
        
        std::string key = stripped.substr(quoteStart + 1, quoteEnd - quoteStart - 1);
        
        // Find colon after the key
        size_t colonPos = stripped.find(':', quoteEnd);
        if (colonPos == std::string::npos) break;
        
        // Find next non-whitespace after colon
        size_t valueStart = colonPos;
        while (valueStart < stripped.size() && 
               (stripped[valueStart] == ' ' || stripped[valueStart] == '\t' || stripped[valueStart] == '\n')) {
            valueStart++;
        }
        
        if (valueStart >= stripped.size()) break;
        
        // Find end of value (comma or closing brace)
        size_t valueEnd = stripped.find(',', valueStart);
        if (valueEnd == std::string::npos) {
            valueEnd = stripped.find('}', valueStart);
            if (valueEnd == std::string::npos) valueEnd = stripped.size();
        }
        
        std::string value = stripped.substr(valueStart, valueEnd - valueStart);
        
        // Trim value
        value = trim(value);
        
        // Remove surrounding quotes if present
        if (value.size() >= 2 && ((value.front() == '\"' && value.back() == '\"') ||
                                   (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Convert value to appropriate type
        ConfigValue configValue = stringToValue(value);
        
        entries.emplace_back(key, configValue, ConfigSource::File);
        
        // Move to next pair
        pos = valueEnd + 1;
        
        // Skip comma
        while (pos < stripped.size() && (stripped[pos] == ',' || stripped[pos] == ' ' || 
                                          stripped[pos] == '\t' || stripped[pos] == '\n')) {
            pos++;
        }
    }
    
    return !entries.empty();
}

// YAML parsing implementation
bool parseYAML(const std::string& content, std::vector<ConfigEntry>& entries) {
    std::stringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Handle arrays (lines starting with -)
        if (line.find_first_not_of(" \t") == 0 && line.find('-') == 0) {
            // Array item - convert to string representation
            std::string value = line.substr(1);
            value = trim(value);
            
            // Parse as YAML value
            ConfigValue configValue = parseYAMLValue(value);
            
            // For now, skip arrays in main parsing (simple approach)
            continue;
        }
        
        // Parse key: value pairs
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Parse value
            ConfigValue configValue = parseYAMLValue(value);
            
            entries.emplace_back(key, configValue, ConfigSource::File);
        }
    }
    
    return !entries.empty();
}

// Helper function implementations

std::string trimYamlIndent(const std::string& line) {
    size_t firstNonSpace = line.find_first_not_of(" \t");
    if (firstNonSpace == std::string::npos) return "";
    return line.substr(firstNonSpace);
}

bool startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

ConfigValue stringToValue(const std::string& str) {
    if (str.empty()) return std::string();
    
    // Try boolean
    std::string lower = toLower(str);
    if (lower == "true") return true;
    if (lower == "false") return false;
    
    // Try integer
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) return intVal;
    } catch (...) {}
    
    // Try int64_t
    try {
        size_t pos;
        int64_t int64Val = std::stoll(str, &pos);
        if (pos == str.size()) return int64Val;
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) return doubleVal;
    } catch (...) {}
    
    // Default to string
    return str;
}

bool stringToBool(const std::string& str) {
    std::string lower = toLower(str);
    return lower == "true";
}

int stringToInt(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (...) {
        return 0;
    }
}

double stringToDouble(const std::string& str) {
    try {
        return std::stod(str);
    } catch (...) {
        return 0.0;
    }
}

std::vector<std::string> parseYAMLArray(const std::string& str) {
    std::vector<std::string> result;
    std::stringstream stream(str);
    std::string item;
    
    while (std::getline(stream, item, ',')) {
        item = trim(item);
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    
    return result;
}

ConfigValue parseYAMLValue(const std::string& str) {
    if (str.empty()) return std::string();
    
    // Check for quoted strings
    if ((str.front() == '\"' && str.back() == '\"') || 
        (str.front() == '\'' && str.back() == '\'')) {
        return str.substr(1, str.size() - 2);
    }
    
    // Check for array [item1, item2, ...]
    if (str.front() == '[' && str.back() == ']') {
        std::string inner = str.substr(1, str.size() - 2);
        std::vector<std::string> arrayItems = parseYAMLArray(inner);
        if (!arrayItems.empty()) {
            // Convert to string array
            std::vector<std::string> stringVector = arrayItems;
            return stringVector;
        }
    }
    
    // Try to convert to other types
    return stringToValue(str);
}

// Simple key-value format parser implementation
bool parseKeyValue(const std::string& content, std::vector<ConfigEntry>& entries) {
    std::stringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Convert value to appropriate type
            ConfigValue configValue = stringToValue(value);
            
            entries.emplace_back(key, configValue, ConfigSource::File);
        }
    }
    
    return true;
}

} // namespace nlm
