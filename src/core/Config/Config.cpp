#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <cstring>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper function to trim whitespace
static std::string trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();
    
    while (start < end && std::isspace(str[start])) {
        start++;
    }
    while (end > start && std::isspace(str[end - 1])) {
        end--;
    }
    
    return str.substr(start, end - start);
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try to detect file format and load appropriately
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read first line to detect format
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0);
    
    // Detect JSON format (starts with { or [)
    firstLine = trim(firstLine);
    if ((firstLine.size() >= 1 && (firstLine[0] == '{' || firstLine[0] == '[')) ||
        (firstLine.size() >= 4 && (firstLine.substr(0, 4) == "{" || firstLine.substr(0, 4) == "[\"" || firstLine.substr(0, 4) == "{\""))) {
        // JSON format - delegate to JSON parser
        return loadFromJSON(filepath);
    }
    // Try YAML format (indented lines with colon)
    else if (firstLine.find(':') != std::string::npos || 
             (firstLine.size() >= 2 && firstLine.find("  ") != std::string::npos)) {
        // YAML format - delegate to YAML parser  
        return loadFromYAML(filepath);
    }
    // Assume legacy key=value format
    else {
        return loadFromLegacyFormat(filepath);
    }
}

// Legacy key=value format loader
bool Config::loadFromLegacyFormat(const std::string& filepath) {
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

// Simplified JSON parser (placeholder for full implementation)
bool Config::loadFromJSON(const std::string& filepath) {
    // Basic JSON parsing implementation
    // For now, use legacy format but try to handle JSON arrays/objects
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Clean up JSON formatting
    std::string cleaned;
    for (char c : content) {
        if (!std::isspace(c)) {
            cleaned += c;
        }
    }
    
    // Simple parsing for key:value pairs
    size_t pos = 0;
    while (pos < cleaned.size()) {
        // Find next "key":"value" pattern
        size_t keyStart = cleaned.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = cleaned.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = cleaned.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Skip to colon
        size_t colonPos = cleaned.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        // Skip whitespace and opening quote
        size_t valueStart = cleaned.find('"', colonPos + 1);
        if (valueStart == std::string::npos) {
            // Maybe it's a number or boolean
            size_t valueEnd = colonPos + 1;
            while (valueEnd < cleaned.size() && (std::isdigit(cleaned[valueEnd]) || cleaned[valueEnd] == '.' || cleaned[valueEnd] == '-' || cleaned[valueEnd] == 't' || cleaned[valueEnd] == 'f')) {
                valueEnd++;
            }
            std::string value = cleaned.substr(colonPos + 1, valueEnd - (colonPos + 1));
            set(key, value, ConfigSource::File);
            pos = valueEnd;
            continue;
        }
        
        // Find closing quote
        size_t valueEnd = cleaned.find('"', valueStart + 1);
        if (valueEnd == std::string::npos) break;
        
        std::string value = cleaned.substr(valueStart + 1, valueEnd - valueStart - 1);
        
        set(key, value, ConfigSource::File);
        pos = valueEnd + 1;
        
        // Skip comma
        if (pos < cleaned.size() && cleaned[pos] == ',') {
            pos++;
        }
    }
    
    return true;
}

// Simplified YAML parser (placeholder for full implementation)
bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse YAML key: value format
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Handle YAML scalar values (quoted strings, numbers, booleans)
            if (value.size() >= 2 && (value[0] == '\"' && value.back() == '\"') || 
                (value[0] == '\'' && value.back() == '\'')) {
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
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
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

bool Config::loadFromFile(const std::string& filepath) {
    // Try to detect file format and load appropriately
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Read first line to detect format
    std::string firstLine;
    std::getline(file, firstLine);
    file.clear();
    file.seekg(0);
    
    // Detect JSON format (starts with { or [)
    firstLine = trim(firstLine);
    if ((firstLine.size() >= 1 && (firstLine[0] == '{' || firstLine[0] == '[')) ||
        (firstLine.size() >= 4 && (firstLine.substr(0, 4) == "{" || firstLine.substr(0, 4) == "[\"" || firstLine.substr(0, 4) == "{\""))) {
        // JSON format - delegate to JSON parser
        return loadFromJSON(filepath);
    }
    // Try YAML format (indented lines with colon)
    else if (firstLine.find(':') != std::string::npos || 
             (firstLine.size() >= 2 && firstLine.find("  ") != std::string::npos)) {
        // YAML format - delegate to YAML parser  
        return loadFromYAML(filepath);
    }
    // Assume legacy key=value format
    else {
        return loadFromLegacyFormat(filepath);
    }
}

// Legacy key=value format loader
bool Config::loadFromLegacyFormat(const std::string& filepath) {
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            setNested(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

// Simplified JSON parser (placeholder for full implementation)
bool Config::loadFromJSON(const std::string& filepath) {
    // Basic JSON parsing implementation
    // For now, use legacy format but try to handle JSON arrays/objects
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();
    
    // Clean up JSON formatting
    std::string cleaned;
    for (char c : content) {
        if (!std::isspace(c)) {
            cleaned += c;
        }
    }
    
    // Simple parsing for key:value pairs
    size_t pos = 0;
    while (pos < cleaned.size()) {
        // Find next "key":"value" pattern
        size_t keyStart = cleaned.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = cleaned.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = cleaned.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Skip to colon
        size_t colonPos = cleaned.find(':', keyEnd);
        if (colonPos == std::string::npos) break;
        
        // Skip whitespace and opening quote
        size_t valueStart = cleaned.find('"', colonPos + 1);
        if (valueStart == std::string::npos) {
            // Maybe it's a number or boolean
            size_t valueEnd = colonPos + 1;
            while (valueEnd < cleaned.size() && (std::isdigit(cleaned[valueEnd]) || cleaned[valueEnd] == '.' || cleaned[valueEnd] == '-' || cleaned[valueEnd] == 't' || cleaned[valueEnd] == 'f')) {
                valueEnd++;
            }
            std::string value = cleaned.substr(colonPos + 1, valueEnd - (colonPos + 1));
            setNested(key, value, ConfigSource::File);
            pos = valueEnd;
            continue;
        }
        
        // Find closing quote
        size_t valueEnd = cleaned.find('"', valueStart + 1);
        if (valueEnd == std::string::npos) break;
        
        std::string value = cleaned.substr(valueStart + 1, valueEnd - valueStart - 1);
        
        setNested(key, value, ConfigSource::File);
        pos = valueEnd + 1;
        
        // Skip comma
        if (pos < cleaned.size() && cleaned[pos] == ',') {
            pos++;
        }
    }
    
    return true;
}

// Simplified YAML parser (placeholder for full implementation)
bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip comments and empty lines
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse YAML key: value format
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Handle YAML scalar values (quoted strings, numbers, booleans)
            if (value.size() >= 2 && (value[0] == '\"' && value.back() == '\"') || 
                (value[0] == '\'' && value.back() == '\'')) {
                value = value.substr(1, value.size() - 2);
            }
            
            setNested(key, value, ConfigSource::File);
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
                setNested(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            setNested(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Sort entries by key for consistent output
    std::vector<ConfigEntry> sortedEntries = pImpl->entries;
    std::sort(sortedEntries.begin(), sortedEntries.end(), 
              [](const ConfigEntry& a, const ConfigEntry& b) { return a.key < b.key; });
    
    for (const auto& entry : sortedEntries) {
        file << "# " << entry.description << "\n";
        
        // Handle nested keys
        if (entry.key.find('.') != std::string::npos) {
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
        } else {
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
        }
    }
    
    return true;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    setNested(key, value, source);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    setNested(key, value, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    setNested(key, value, source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    setNested(key, value, source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    setNested(key, value, source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    setNested(key, value, source);
}

void Config::setNested(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Normalize key for consistent handling
    std::string normalizedKey = normalizeKey(key);
    
    // Create nested structure if needed and set value at the deepest level
    if (normalizedKey.find('.') != std::string::npos) {
        // Get or create nested config
        std::shared_ptr<Config> nestedConfig = getNestedConfig(normalizedKey);
        if (nestedConfig) {
            // Split key into parts and navigate to the leaf
            size_t dotPos = normalizedKey.find('.');
            std::string leafKey = normalizedKey.substr(dotePos + 1);
            
            // Create ConfigEntry and add to nested config
            ConfigEntry entry(leafKey, value, source, "Dynamic nested key");
            nestedConfig->pImpl->entries.push_back(entry);
        }
    } else {
        // Direct key - use original logic
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(normalizedKey, value, source);
        }
    }
}

void Config::setNested(const std::string& key, const std::string& value, ConfigSource source) {
    setNested(key, ConfigValue(value), source);
}

void Config::setNested(const std::string& key, int value, ConfigSource source) {
    setNested(key, ConfigValue(value), source);
}

void Config::setNested(const std::string& key, int64_t value, ConfigSource source) {
    setNested(key, ConfigValue(value), source);
}

void Config::setNested(const std::string& key, double value, ConfigSource source) {
    setNested(key, ConfigValue(value), source);
}

void Config::setNested(const std::string& key, bool value, ConfigSource source) {
    setNested(key, ConfigValue(value), source);
}

bool Config::has(const std::string& key) const {
    return hasNested(key);
}

bool Config::hasNested(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    
    if (normalizedKey.find('.') == std::string::npos) {
        // Direct key lookup
        return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
            [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
    } else {
        // Nested key lookup - navigate to parent and check child
        std::shared_ptr<Config> nestedConfig = getNestedConfig(normalizedKey);
        if (nestedConfig) {
            size_t dotPos = normalizedKey.find('.');
            std::string leafKey = normalizedKey.substr(dotPos + 1);
            
            return std::any_of(nestedConfig->pImpl->entries.begin(), nestedConfig->pImpl->entries.end(),
                [&leafKey](const ConfigEntry& e) { return e.key == leafKey; });
        }
    }
    return false;
}

void Config::remove(const std::string& key) {
    removeNested(key);
}

void Config::removeNested(const std::string& key) {
    std::string normalizedKey = normalizeKey(key);
    
    if (normalizedKey.find('.') == std::string::npos) {
        // Direct key removal
        pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; }),
            pImpl->entries.end()
        );
    } else {
        // Nested key removal - remove from parent config
        std::shared_ptr<Config> nestedConfig = getNestedConfig(normalizedKey);
        if (nestedConfig) {
            size_t dotPos = normalizedKey.find('.');
            std::string leafKey = normalizedKey.substr(dotPos + 1);
            
            nestedConfig->pImpl->entries.erase(
                std::remove_if(nestedConfig->pImpl->entries.begin(), nestedConfig->pImpl->entries.end(),
                    [&leafKey](const ConfigEntry& e) { return e.key == leafKey; }),
                nestedConfig->pImpl->entries.end()
            );
        }
    }
}

std::vector<std::string> Config::getKeys() const {
    return getKeys("");
}

std::vector<std::string> Config::getKeys(const std::string& prefix) const {
    std::vector<std::string> keys;
    
    // Get direct keys
    for (const auto& entry : pImpl->entries) {
        if (prefix.empty() || entry.key.find(prefix) == 0) {
            keys.push_back(entry.key);
        }
    }
    
    // Get nested keys
    for (const auto& entry : pImpl->entries) {
        if (entry.key.find('.') != std::string::npos) {
            std::string baseKey = entry.key.substr(0, entry.key.find('.'));
            if (prefix.empty() || baseKey.find(prefix) == 0) {
                keys.push_back(entry.key);
            }
        }
    }
    
    // Remove duplicates and sort
    std::sort(keys.begin(), keys.end());
    keys.erase(std::unique(keys.begin(), keys.end()), keys.end());
    
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    // Get all keys including nested ones
    std::vector<std::string> allKeys = getKeys();
    oss << "Total keys (including nested): " << allKeys.size() << "\n";
    
    // Group by source
    int fileSources = 0, commandLineSources = 0, runtimeSources = 0, defaultSources = 0;
    
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
        
        // Count by source
        switch (entry.source) {
            case ConfigSource::File: fileSources++; break;
            case ConfigSource::CommandLine: commandLineSources++; break;
            case ConfigSource::Runtime: runtimeSources++; break;
            case ConfigSource::Default: defaultSources++; break;
        }
    }
    
    oss << "\nSource Summary:";
    oss << "  File: " << fileSources << ", CommandLine: " << commandLineSources << ", Runtime: " << runtimeSources << ", Default: " << defaultSources << "\n";
    
    return oss.str();
}

std::optional<ConfigValue> Config::getNested(const std::string& key) const {
    std::string normalizedKey = normalizeKey(key);
    
    if (normalizedKey.find('.') == std::string::npos) {
        // Direct key lookup
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&normalizedKey](const ConfigEntry& e) { return e.key == normalizedKey; });
        
        if (it == pImpl->entries.end()) {
            return std::nullopt;
        }
        
        return it->value;
    } else {
        // Nested key lookup
        std::shared_ptr<Config> nestedConfig = getNestedConfig(normalizedKey);
        if (!nestedConfig) {
            return std::nullopt;
        }
        
        size_t dotPos = normalizedKey.find('.');
        std::string leafKey = normalizedKey.substr(dotPos + 1);
        
        auto it = std::find_if(nestedConfig->pImpl->entries.begin(), nestedConfig->pImpl->entries.end(),
            [&leafKey](const ConfigEntry& e) { return e.key == leafKey; });
        
        if (it == nestedConfig->pImpl->entries.end()) {
            return std::nullopt;
        }
        
        return it->value;
    }
}

std::unordered_map<std::string, std::shared_ptr<Config>> Config::createNestedStructure(const std::string& key) const {
    std::unordered_map<std::string, std::shared_ptr<Config>> nestedStructure;
    
    size_t pos = 0;
    std::string currentKey;
    std::shared_ptr<Config> currentConfig = std::make_shared<Config>();
    nestedStructure[""] = currentConfig;  // Root config
    
    while (pos < key.size()) {
        size_t dotPos = key.find('.', pos);
        if (dotPos == std::string::npos) {
            currentKey = key.substr(pos);
            nestedStructure[currentKey] = currentConfig;
            break;
        }
        
        std::string part = key.substr(pos, dotPos - pos);
        pos = dotPos + 1;
        
        if (!part.empty()) {
            currentKey = part;
            nestedStructure[currentKey] = currentConfig;
        }
    }
    
    return nestedStructure;
}

std::shared_ptr<Config> Config::getNestedConfig(const std::string& key) const {
    size_t dotPos = key.find('.');
    if (dotPos == std::string::npos) {
        return std::const_cast<Config*>(this)->shared_from_this();
    }
    
    std::string parentKey = key.substr(0, dotPos);
    std::string childKey = key.substr(dotPos + 1);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&parentKey](const ConfigEntry& e) { return e.key == parentKey; });
    
    if (it == pImpl->entries.end()) {
        return nullptr;
    }
    
    // For now, return null - we need to implement nested Config storage
    // This is a simplified version
    return nullptr;
}

std::string Config::normalizeKey(const std::string& key) const {
    std::string normalized = key;
    
    // Convert to lowercase for case-insensitive comparison
    for (char& c : normalized) {
        c = std::tolower(c);
    }
    
    return normalized;
}

std::optional<ConfigValue> Config::get(const std::string& key) const {
    return getNested(key);
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

template<typename T>
std::optional<T> Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = getNested<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Get nested value with type conversion
template<typename T>
std::optional<T> Config::getNested(const std::string& key) const {
    auto val = getNested(key);
    if (!val.has_value()) {
        return std::nullopt;
    }
    
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, val.value());
}

// Explicit template instantiations for getNested
template std::optional<int> Config::getNested<int>(const std::string&) const;
template std::optional<int64_t> Config::getNested<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::getNested<uint64_t>(const std::string&) const;
template std::optional<float> Config::getNested<float>(const std::string&) const;
template std::optional<double> Config::getNested<double>(const std::string&) const;
template std::optional<bool> Config::getNested<bool>(const std::string&) const;
template std::optional<std::string> Config::getNested<std::string>(const std::string&) const;
