// Config.cpp - Implementation of the NLM Configuration System
// This file provides the implementation for the Config class with robust JSON/YAML parsing
// and file I/O capabilities.

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <cctype>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper functions for JSON parsing
namespace config_internal {

std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string stripQuotes(const std::string& str) {
    if (str.size() >= 2 && ((str.front() == '"' && str.back() == '"') || 
                          (str.front() == '\'' && str.back() == '\''))) {
        return str.substr(1, str.size() - 2);
    }
    return str;
}

// Parse a JSON value string into ConfigValue
ConfigValue parseJSONValue(const std::string& valueStr) {
    std::string trimmed = trim(valueStr);
    
    // Empty string
    if (trimmed.empty()) {
        return std::string("");
    }
    
    // Handle boolean values
    if (trimmed == "true") return true;
    if (trimmed == "false") return false;
    
    // Handle null
    if (trimmed == "null") return std::string("null");
    
    // Handle numbers
    try {
        if (trimmed.find('.') != std::string::npos || 
            trimmed.find('e') != std::string::npos || 
            trimmed.find('E') != std::string::npos) {
            // Floating point
            size_t pos;
            double val = std::stod(trimmed, &pos);
            if (pos == trimmed.size()) {
                return val;
            }
        } else {
            // Integer
            size_t pos;
            int64_t val = std::stoll(trimmed, &pos);
            if (pos == trimmed.size()) {
                return val;
            }
        }
    } catch (...) {
        // Not a number, continue to string check
    }
    
    // Handle arrays
    if (trimmed.front() == '[' && trimmed.back() == ']') {
        std::vector<double> array;
        std::string content = trimmed.substr(1, trimmed.size() - 2);
        
        if (!content.empty()) {
            size_t pos = 0;
            while (pos < content.size()) {
                size_t end = content.find(',', pos);
                if (end == std::string::npos) {
                    end = content.size();
                }
                
                std::string element = trim(content.substr(pos, end - pos));
                if (!element.empty()) {
                    try {
                        size_t elemPos;
                        double elemVal = std::stod(element, &elemPos);
                        if (elemPos == element.size()) {
                            array.push_back(elemVal);
                        }
                    } catch (...) {
                        // Skip invalid elements
                    }
                }
                
                pos = end + 1; // Skip comma
            }
        }
        
        return array;
    }
    
    // Handle strings (remove quotes)
    std::string result = stripQuotes(trimmed);
    return result;
}

} // namespace config_internal

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Try to parse as JSON first
    bool jsonSuccess = false;
    if (content.find('{') != std::string::npos) {
        try {
            // Simple JSON parsing
            size_t pos = 0;
            while (pos < content.size()) {
                pos = content.find('"', pos);
                if (pos == std::string::npos) break;
                
                size_t keyEnd = content.find('"', pos + 1);
                if (keyEnd == std::string::npos) break;
                
                std::string key = content.substr(pos + 1, keyEnd - pos - 1);
                pos = keyEnd + 1;
                
                // Skip whitespace
                while (pos < content.size() && std::isspace(content[pos])) pos++;
                
                if (pos >= content.size() || content[pos] != ':') break;
                pos++;
                
                // Skip whitespace
                while (pos < content.size() && std::isspace(content[pos])) pos++;
                
                // Find the value end
                if (content[pos] == '"') {
                    // String value
                    size_t valueEnd = content.find('"', pos + 1);
                    if (valueEnd == std::string::npos) break;
                    
                    std::string valueStr = content.substr(pos + 1, valueEnd - pos - 1);
                    set(key, config_internal::parseJSONValue(valueStr), ConfigSource::File);
                    pos = valueEnd + 1;
                } else if (content[pos] == '[') {
                    // Array value
                    int bracketCount = 1;
                    size_t valueEnd = pos + 1;
                    while (valueEnd < content.size() && bracketCount > 0) {
                        if (content[valueEnd] == '[') bracketCount++;
                        else if (content[valueEnd] == ']') bracketCount--;
                        valueEnd++;
                    }
                    
                    if (bracketCount == 0) {
                        std::string valueStr = content.substr(pos, valueEnd - pos);
                        set(key, config_internal::parseJSONValue(valueStr), ConfigSource::File);
                        pos = valueEnd;
                    }
                } else {
                    // Try to find end of value (simple approach)
                    size_t valueEnd = content.find_first_of(",}", pos);
                    if (valueEnd == std::string::npos) break;
                    
                    std::string valueStr = config_internal::trim(content.substr(pos, valueEnd - pos));
                    set(key, config_internal::parseJSONValue(valueStr), ConfigSource::File);
                    pos = valueEnd;
                }
                
                // Skip to next key or closing brace
                while (pos < content.size() && content[pos] != '"' && content[pos] != '}') {
                    pos++;
                }
                
                // Skip comma if present
                if (pos < content.size() && content[pos] == ',') {
                    pos++;
                }
            }
            
            jsonSuccess = true;
        } catch (...) {
            // JSON parsing failed, fall back to simple parser
        }
    }
    
    // Fall back to simple key=value parser if JSON parsing failed
    if (!jsonSuccess) {
        clear();
        std::istringstream stream(content);
        std::string line;
        
        while (std::getline(stream, line)) {
            // Skip empty lines and comments
            line = config_internal::trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/' || line[0] == '{') {
                continue;
            }
            
            // Parse key=value pairs
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = config_internal::trim(line.substr(0, pos));
                std::string value = config_internal::trim(line.substr(pos + 1));
                set(key, value, ConfigSource::File);
            }
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
        
        // Format value based on type
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, int64_t> || std::is_same_v<T, int>) {
                file << entry.key << " = " << arg << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << entry.key << " = [";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
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
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

// Public helper function implementations
std::string trim(const std::string& str) {
    return config_internal::trim(str);
}

std::string toLower(const std::string& str) {
    return config_internal::toLower(str);
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
