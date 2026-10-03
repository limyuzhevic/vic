#pragma once

#include "Config.hpp"
#include "ErrorHandling.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <regex>
#include <memory>
#include <limits>

namespace nlm {

// Helper to detect file format based on extension
std::string Config::detectFileFormat(const std::string& filepath) const {
    std::string ext = std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") return "JSON";
    if (ext == ".yaml" || ext == ".yml") return "YAML";
    if (ext == ".cfg" || ext == ".ini" || ext == ".properties") return "INI";
    return "UNKNOWN";
}

bool Config::loadFromFile(const std::string& filepath) {
    if (filepath.empty()) {
        throw ConfigError(ConfigError::Type::ValidationError, "File path cannot be empty");
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError(ConfigError::Type::FileNotFound, "Cannot open file: " + filepath);
    }
    
    // Check file size for memory allocation
    file.seekg(0, std::ios::end);
    size_t fileSize = static_cast<size_t>(file.tellg());
    file.seekg(0, std::ios::beg);
    
    if (fileSize > 100 * 1024 * 1024) { // 100MB limit
        throw ConfigError(ConfigError::Type::ValidationError, "File too large");
    }
    
    std::string format = detectFileFormat(filepath);
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    try {
        if (format == "JSON") {
            return loadFromJSON(content);
        } else if (format == "YAML") {
            return loadFromYAML(content);
        } else {
            // Fall back to simple key=value format for backward compatibility
            file.clear();
            file.seekg(0, std::ios::beg);
            
            std::string line;
            while (std::getline(file, line)) {
                line = trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = trim(line.substr(0, pos));
                    std::string value = trim(line.substr(pos + 1));
                    
                    if (value.size() >= 2 && 
                        ((value.front() == '"' && value.back() == '"') ||
                         (value.front() == '\'' && value.back() == '\''))) {
                        value = value.substr(1, value.size() - 2);
                    }
                    
                    set(key, value, ConfigSource::File);
                }
            }
        }
    } catch (const std::exception& e) {
        throw ConfigError(ConfigError::Type::ParseError, "Failed to parse file " + filepath + ": " + e.what());
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& jsonString) {
    if (jsonString.empty()) {
        throw ConfigError(ConfigError::Type::ValidationError, "JSON string cannot be empty");
    }
    
    // Try to parse as JSON string
    try {
        // Simple JSON parsing for key=value pairs
        // This is a basic implementation that can be extended with nlohmann/json library
        std::string content = jsonString;
        size_t pos = 0;
        while (pos < content.length()) {
            // Find next key (" or after { or after ,)
            size_t keyStart = content.find('"', pos);
            if (keyStart == std::string::npos) break;
            
            size_t keyEnd = content.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = content.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Find colon after key
            size_t colonPos = content.find(':', keyEnd);
            if (colonPos == std::string::npos) break;
            
            // Find value (could be string, number, boolean, null, or object)
            size_t valueStart = content.find_first_not_of(" \t,\n", colonPos + 1);
            if (valueStart == std::string::npos) break;
            
            if (content[valueStart] == '"') {
                // String value
                size_t valueEnd = content.find('"', valueStart + 1);
                if (valueEnd == std::string::npos) break;
                
                std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
                set(key, value, ConfigSource::File);
                
                pos = valueEnd + 1;
            } else if (content[valueStart] == '{') {
                // Object value - parse as nested map
                size_t braceCount = 1;
                size_t valueEnd = valueStart + 1;
                while (valueEnd < content.length() && braceCount > 0) {
                    if (content[valueEnd] == '{') braceCount++;
                    else if (content[valueEnd] == '}') braceCount--;
                    valueEnd++;
                }
                
                if (braceCount != 0) break;
                
                // For now, just set as string - could be improved
                std::string value = content.substr(valueStart, valueEnd - valueStart);
                set(key, value, ConfigSource::File);
                
                pos = valueEnd;
            } else {
                // Simple value (number, boolean, null)
                size_t valueEnd = content.find_first_of(",}\n", valueStart);
                if (valueEnd == std::string::npos) valueEnd = content.length();
                
                std::string value = content.substr(valueStart, valueEnd - valueStart);
                set(key, value, ConfigSource::File);
                
                pos = valueEnd;
            }
            
            // Skip whitespace and commas
            while (pos < content.length() && (content[pos] == ' ' || content[pos] == '\t' || 
                                              content[pos] == ',' || content[pos] == '\n')) {
                pos++;
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        throw ConfigError(ConfigError::Type::ParseError, "Failed to parse JSON: " + std::string(e.what()));
    }
}

bool Config::loadFromYAML(const std::string& yamlString) {
    if (yamlString.empty()) {
        throw ConfigError(ConfigError::Type::ValidationError, "YAML string cannot be empty");
    }
    
    // Try to parse as YAML string
    try {
        std::string content = yamlString;
        size_t pos = 0;
        
        while (pos < content.length()) {
            // Skip comments and whitespace
            while (pos < content.length() && (content[pos] == '#' || content[pos] == ' ' || 
                                              content[pos] == '\t' || content[pos] == '\n')) {
                pos++;
            }
            
            if (pos >= content.length()) break;
            
            // Find key (anything before colon)
            size_t colonPos = content.find(':', pos);
            if (colonPos == std::string::npos) break;
            
            // Extract key (trim whitespace)
            size_t keyStart = pos;
            size_t keyEnd = keyStart;
            while (keyEnd > 0 && (content[keyEnd - 1] == ' ' || content[keyEnd - 1] == '\t')) {
                keyEnd--;
            }
            std::string key = content.substr(keyStart, keyEnd - keyStart);
            
            // Find value (after colon)
            size_t valueStart = content.find_first_not_of(" \t", colonPos + 1);
            if (valueStart == std::string::npos) break;
            
            // Check if value is a block scalar (starts with | or >)
            if (valueStart < content.length() && (content[valueStart] == '|' || content[valueStart] == '>')) {
                // Multi-line scalar - simplified parsing
                size_t newlinePos = content.find('\n', valueStart);
                if (newlinePos == std::string::npos) break;
                
                size_t valueEnd = newlinePos;
                // Find the end of the block scalar (indented lines)
                size_t current = valueEnd + 1;
                while (current < content.length()) {
                    if (content[current] == '\n') {
                        // Check if this line is indented (part of block)
                        size_t nextNonSpace = content.find_first_not_of(" \t", current + 1);
                        if (nextNonSpace == std::string::npos || content[nextNonSpace] == '\n') {
                            break;
                        }
                        valueEnd = current;
                    }
                    current++;
                }
                
                std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);
                set(key, value, ConfigSource::File);
                pos = valueEnd + 1;
            } else {
                // Simple value
                size_t valueEnd = content.find_first_of("\n", valueStart);
                if (valueEnd == std::string::npos) valueEnd = content.length();
                
                std::string value = content.substr(valueStart, valueEnd - valueStart);
                
                // Trim whitespace from value
                size_t valStart = value.find_first_not_of(" \t");
                size_t valEnd = value.find_last_not_of(" \t");
                if (valStart != std::string::npos && valEnd != std::string::npos) {
                    value = value.substr(valStart, valEnd - valStart + 1);
                }
                
                set(key, value, ConfigSource::File);
                pos = valueEnd;
            }
            
            // Skip to next line
            while (pos < content.length() && content[pos] != '\n') pos++;
            pos++;
        }
        
        return true;
    } catch (const std::exception& e) {
        throw ConfigError(ConfigError::Type::ParseError, "Failed to parse YAML: " + std::string(e.what()));
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        return true; // No arguments to process
    }
    
    for (int i = 1; i < argc; ++i) {
        if (i + 1 < argc) {
            std::string key = argv[i];
            if (key.substr(0, 2) == "--") {
                size_t pos = key.find('=');
                if (pos != std::string::npos) {
                    std::string value = key.substr(pos + 1);
                    set(key.substr(2, pos - 2), value, ConfigSource::CommandLine);
                }
            } else if (key[0] == '-') {
                std::string value = argv[++i];
                set(key.substr(1), value, ConfigSource::CommandLine);
            }
        }
    }
    return true;
}

// Simple template instantiations for get() and getOr() methods
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

// Config::Impl definition
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    ConfigSchema schema;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Function implementations for get() and getOr()
template std::optional<int> Config::get<int>(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<int>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template std::optional<int64_t> Config::get<int64_t>(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<int64_t>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template std::optional<double> Config::get<double>(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<double>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template std::optional<bool> Config::get<bool>(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<bool>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template std::optional<std::string> Config::get<std::string>(const std::string& key) const {
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

template int Config::getOr<int>(const std::string& key, const int& defaultValue) const {
    auto val = get<int>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template int64_t Config::getOr<int64_t>(const std::string& key, const int64_t& defaultValue) const {
    auto val = get<int64_t>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template double Config::getOr<double>(const std::string& key, const double& defaultValue) const {
    auto val = get<double>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template bool Config::getOr<bool>(const std::string& key, const bool& defaultValue) const {
    auto val = get<bool>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template std::string Config::getOr<std::string>(const std::string& key, const std::string& defaultValue) const {
    auto val = get<std::string>(key);
    return val.has_value() ? val.value() : defaultValue;
}

} // namespace nlm
