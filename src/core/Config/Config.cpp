#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstdint>
#include <unordered_map>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check file extension for format detection
    std::string extension = ".";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos);
    }
    
    // Try JSON format first (most common)
    if (extension == ".json") {
        return loadFromJSONFile(filepath);
    }
    // Try YAML format
    else if (extension == ".yaml" || extension == ".yml") {
        return loadFromYAMLFile(filepath);
    }
    // Default to key=value format (legacy support)
    else {
        return loadFromSimpleFile(filepath);
    }
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

bool Config::loadFromSimpleFile(const std::string& filepath) {
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
bool Config::loadFromSimpleFile(const std::string& filepath) {
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

// Load from JSON file format
bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple JSON parsing for configuration
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    
    clear(); // Clear existing configuration
    
    // Parse JSON objects
    parseJSON(content, "");
    
    return true;
}

// Load from YAML file format
bool Config::loadFromYAMLFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple YAML parsing for configuration
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    
    clear(); // Clear existing configuration
    
    // Parse YAML documents
    parseYAML(content, "");
    
    return true;
}

// Helper function to parse JSON content
gvoid Config::parseJSON(const std::string& json, const std::string& parentKey) {
    size_t pos = 0;
    while (pos < json.size()) {
        // Skip whitespace
        while (pos < json.size() && std::isspace(json[pos])) pos++;
        
        if (pos >= json.size()) break;
        
        // Parse key-value pairs
        if (json[pos] == '"') {
            // Find end of key
            size_t keyEnd = json.find('"', pos + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = json.substr(pos + 1, keyEnd - pos - 1);
            pos = keyEnd + 1;
            
            // Skip whitespace after key
            while (pos < json.size() && std::isspace(json[pos])) pos++;
            
            if (pos >= json.size() || json[pos] != ':') break;
            pos++; // Skip colon
            
            // Skip whitespace after colon
            while (pos < json.size() && std::isspace(json[pos])) pos++;
            
            // Parse value
            std::string valueStr;
            if (json[pos] == '"') {
                // String value
                size_t valueEnd = json.find('"', pos + 1);
                if (valueEnd == std::string::npos) break;
                valueStr = json.substr(pos + 1, valueEnd - pos - 1);
                pos = valueEnd + 1;
            } else if (json[pos] == '{' || json[pos] == '[') {
                // Object or array - skip for now
                // In a real implementation, you would parse nested structures
                pos = json.find_first_of(",}", pos);
                if (pos == std::string::npos) break;
                continue;
            } else {
                // Number, boolean, or null
                size_t valueEnd = json.find_first_of(",}\n\r", pos);
                if (valueEnd == std::string::npos) break;
                valueStr = json.substr(pos, valueEnd - pos);
                pos = valueEnd;
            }
            
            // Add to configuration
            if (parentKey.empty()) {
                set(key, valueStr, ConfigSource::File);
            } else {
                std::string fullKey = parentKey + "." + key;
                set(fullKey, valueStr, ConfigSource::File);
            }
        } else if (json[pos] == ',' || json[pos] == '}' || json[pos] == ']') {
            pos++;
        } else {
            pos++;
        }
    }
}

// Helper function to parse YAML content
gvoid Config::parseYAML(const std::string& yaml, const std::string& parentKey) {
    // Simple YAML parsing (YAML is complex, so this is simplified)
    size_t pos = 0;
    while (pos < yaml.size()) {
        // Skip whitespace
        while (pos < yaml.size() && std::isspace(yaml[pos])) pos++;
        
        if (pos >= yaml.size()) break;
        
        // Parse key-value pairs (simple YAML format)
        if (yaml[pos] == '"') {
            // Find end of key
            size_t keyEnd = yaml.find('"', pos + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = yaml.substr(pos + 1, keyEnd - pos - 1);
            pos = keyEnd + 1;
            
            // Skip whitespace after key
            while (pos < yaml.size() && std::isspace(yaml[pos])) pos++;
            
            if (pos >= yaml.size() || yaml[pos] != ':') break;
            pos++; // Skip colon
            
            // Skip whitespace after colon
            while (pos < yaml.size() && std::isspace(yaml[pos])) pos++;
            
            // Parse value
            std::string valueStr;
            if (yaml[pos] == '"') {
                // String value
                size_t valueEnd = yaml.find('"', pos + 1);
                if (valueEnd == std::string::npos) break;
                valueStr = yaml.substr(pos + 1, valueEnd - pos - 1);
                pos = valueEnd + 1;
            } else if (std::isdigit(yaml[pos]) || yaml[pos] == '-' || yaml[pos] == '+') {
                // Number
                size_t valueEnd = yaml.find_first_of("\n\r", pos);
                if (valueEnd == std::string::npos) break;
                valueStr = yaml.substr(pos, valueEnd - pos);
                pos = valueEnd;
            } else if (yaml[pos] == 't' || yaml[pos] == 'f') {
                // Boolean
                size_t valueEnd = yaml.find_first_of("\n\r", pos);
                if (valueEnd == std::string::npos) break;
                valueStr = yaml.substr(pos, valueEnd - pos);
                pos = valueEnd;
            } else {
                pos++;
            }
            
            // Add to configuration
            if (parentKey.empty()) {
                set(key, valueStr, ConfigSource::File);
            } else {
                std::string fullKey = parentKey + "." + key;
                set(fullKey, valueStr, ConfigSource::File);
            }
        } else if (yaml[pos] == '\n' || yaml[pos] == '\r') {
            pos++;
        } else {
            pos++;
        }
    }
}
