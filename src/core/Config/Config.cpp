#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>
#include <optional>
#include <stdexcept>
#include <cctype>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Determine file format based on extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    if (ext == "json" || ext == "JSON") {
        return loadFromJSON(filepath);
    } else if (ext == "yaml" || ext == "yml" || ext == "YAML" || ext == "YML") {
        return loadFromYAML(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    // Use nlohmann::json library for JSON parsing
    // Since we don't have nlohmann::json directly included, we'll implement
    // a simple JSON parser or use an external library
    
    // For now, implement a simple JSON parser that handles basic types
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), 
                        std::istreambuf_iterator<char>());
    file.close();
    
    // Remove comments (simplified)
    size_t commentPos = content.find("//");
    while (commentPos != std::string::npos) {
        size_t newlinePos = content.find('\n', commentPos);
        if (newlinePos != std::string::npos) {
            content.erase(commentPos, newlinePos - commentPos);
        } else {
            content.erase(commentPos);
            break;
        }
        commentPos = content.find("//");
    }
    
    try {
        // Simple JSON parsing (limited functionality)
        parseJSON(content);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error: " + std::string(e.what()) + 
                     " in file: " + filepath);
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    // Use yaml-cpp library for YAML parsing
    // For now, implement a simple YAML parser or use an external library
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open YAML file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), 
                        std::istreambuf_iterator<char>());
    file.close();
    
    try {
        // Simple YAML parsing (limited functionality)
        parseYAML(content);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("YAML parsing error: " + std::string(e.what()) + 
                     " in file: " + filepath);
        return false;
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
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
        } else {
            NLM_LOG_WARNING("Skipping invalid line " + std::to_string(lineNumber) + 
                           " in config file: " + line);
        }
    }
    
    return true;
}

void Config::parseJSON(const std::string& jsonStr) {
    // Simple JSON parser implementation
    // This handles basic JSON objects with string values
    size_t pos = 0;
    while (pos < jsonStr.size()) {
        // Find next { or },
        size_t bracePos = jsonStr.find('{', pos);
        size_t commaPos = jsonStr.find(',', pos);
        
        if (bracePos == std::string::npos && commaPos == std::string::npos) {
            break;
        }
        
        size_t keyEnd = std::string::npos;
        size_t valueEnd = std::string::npos;
        
        if (bracePos != std::string::npos && (commaPos == std::string::npos || bracePos < commaPos)) {
            // Handle object { "key": "value" }
            size_t quote1 = jsonStr.find('"', bracePos + 1);
            if (quote1 != std::string::npos) {
                size_t quote2 = jsonStr.find('"', quote1 + 1);
                if (quote2 != std::string::npos) {
                    keyEnd = quote2;
                    size_t colon = jsonStr.find(':', quote2);
                    if (colon != std::string::npos) {
                        size_t valueQuote = jsonStr.find('"', colon + 1);
                        if (valueQuote != std::string::npos) {
                            size_t endQuote = jsonStr.find('"', valueQuote + 1);
                            if (endQuote != std::string::npos) {
                                valueEnd = endQuote;
                            }
                        }
                    }
                }
            }
        }
        
        if (keyEnd != std::string::npos && valueEnd != std::string::npos) {
            std::string key = jsonStr.substr(quote1 + 1, quote2 - quote1 - 1);
            std::string value = jsonStr.substr(valueQuote + 1, endQuote - valueQuote - 1);
            set(key, value, ConfigSource::File);
            pos = endQuote + 1;
        } else {
            break;
        }
    }
}

void Config::parseYAML(const std::string& yamlStr) {
    // Simple YAML parser implementation
    // This handles basic YAML key-value pairs
    size_t pos = 0;
    while (pos < yamlStr.size()) {
        // Find next line break
        size_t newline = yamlStr.find('\n', pos);
        if (newline == std::string::npos) {
            break;
        }
        
        std::string line = yamlStr.substr(pos, newline - pos);
        pos = newline + 1;
        
        // Trim whitespace
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        // Parse YAML key: value format
        size_t colon = line.find(':');
        if (colon != std::string::npos) {
            std::string key = trim(line.substr(0, colon));
            std::string value = trim(line.substr(colon + 1));
            
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
    // Determine file format based on extension
    std::string ext = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        ext = filepath.substr(dotPos + 1);
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    if (ext == "json" || ext == "JSON") {
        return saveToJSON(file);
    } else if (ext == "yaml" || ext == "yml" || ext == "YAML" || ext == "YML") {
        return saveToYAML(file);
    } else {
        // Default to simple key=value format for backward compatibility
        return saveToSimpleFile(file);
    }
}

bool Config::saveToJSON(std::ostream& out) const {
    out << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        out << "  \"" << entry.key << "\": \";
        
        std::visit([&out](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                // Escape quotes and backslashes
                std::string str = arg;
                size_t pos = 0;
                while ((pos = str.find('"', pos)) != std::string::npos) {
                    str.replace(pos, 1, "\\\"");
                    pos += 2;
                }
                pos = 0;
                while ((pos = str.find('\\', pos)) != std::string::npos) {
                    str.replace(pos, 1, "\\\\");
                    pos += 2;
                }
                out << str;
            } else {
                out << arg;
            }
        }, entry.value);
        
        out << \"\"\n\";;
        if (i < pImpl->entries.size() - 1) {
            out << ",";
        }
    }
    out << "}\n";
    return true;
}

bool Config::saveToYAML(std::ostream& out) const {
    out << "%YAML 1.2\n---\n";
    for (const auto& entry : pImpl->entries) {
        out << entry.key << ": ";
        
        std::visit([&out](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                out << \"\" << arg << \"\";
            } else if constexpr (std::is_same_v<T, bool>) {
                out << (arg ? "true" : "false");
            } else {
                out << arg;
            }
        }, entry.value);
        out << "\n";
    }
    return true;
}

bool Config::saveToSimpleFile(std::ostream& out) const {
    for (const auto& entry : pImpl->entries) {
        out << "# " << entry.description << "\n";
        
        std::visit([&out](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                out << entry.key << " = \"" << arg << "\"\n";
            } else {
                out << entry.key << " = " << arg << "\n";
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
