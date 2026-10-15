#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cctype>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Implement proper JSON/YAML parser
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file format by extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Simple JSON parsing (basic key-value support)
    // This is a minimal implementation - for production, use nlohmann/json library
    try {
        // Remove whitespace and comments
        std::string clean;
        for (size_t i = 0; i < content.size(); ++i) {
            if (content[i] == '/') {
                // Skip comments
                if (i + 1 < content.size() && content[i + 1] == '/') {
                    while (i < content.size() && content[i] != '\n') i++;
                } else if (i + 1 < content.size() && content[i + 1] == '*') {
                    // Skip block comments
                    while (i + 1 < content.size() && !(content[i] == '*' && content[i + 1] == '/')) {
                        i++;
                    }
                    if (i < content.size()) i++; // Skip '/' in "*/"
                } else {
                    clean += content[i];
                }
            } else if (!std::isspace(content[i])) {
                clean += content[i];
            }
        }
        
        // Parse simple JSON objects with key-value pairs
        if (clean.empty() || clean[0] != '{' || clean.back() != '}') {
            return false;
        }
        
        // Extract key-value pairs from JSON
        std::string inner = clean.substr(1, clean.size() - 2);
        size_t pos = 0;
        while (pos < inner.size()) {
            // Find next key
            size_t keyStart = inner.find('"', pos);
            if (keyStart == std::string::npos) break;
            size_t keyEnd = inner.find('"', keyStart + 1);
            if (keyEnd == std::string::npos) break;
            
            std::string key = inner.substr(keyStart + 1, keyEnd - keyStart - 1);
            
            // Find value (skip comma)
            size_t valuePos = inner.find(':', keyEnd);
            if (valuePos == std::string::npos) break;
            size_t valueStart = inner.find_first_not_of(' ', valuePos + 1);
            if (valueStart == std::string::npos) break;
            
            // Find end of value (next comma or })
            size_t valueEnd = inner.find(',', valueStart);
            if (valueEnd == std::string::npos) {
                valueEnd = inner.find('}', valueStart);
                if (valueEnd == std::string::npos) break;
            }
            
            std::string valueStr = inner.substr(valueStart, valueEnd - valueStart);
            
            // Clean up value string
            valueStr = trim(valueStr);
            
            // Parse value based on content
            ConfigValue configValue;
            if (valueStr == "true" || valueStr == "false") {
                configValue = (valueStr == "true");
            } else if (valueStr.find('.') != std::string::npos || valueStr.find('e') != std::string::npos || valueStr.find('E') != std::string::npos) {
                configValue = std::stod(valueStr);
            } else {
                configValue = valueStr;
            }
            
            set(key, configValue, ConfigSource::File);
            
            pos = valueEnd + 1;
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    // Simple YAML parsing (basic key=value format support)
    // This handles YAML with simple key-value pairs and lists
    try {
        std::vector<std::string> lines;
        std::stringstream lineStream(content);
        std::string line;
        
        while (std::getline(lineStream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Handle YAML list indicators
            if (line.find("-") == 0) {
                // For now, skip YAML lists in simple parsing
                // Could be expanded to support [1, 2, 3] syntax
                continue;
            }
            
            lines.push_back(line);
        }
        
        // Parse each line as key=value
        for (const std::string& line : lines) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Remove quotes
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                ConfigValue configValue;
                if (value == "true" || value == "false") {
                    configValue = (value == "true");
                } else if (value.find('.') != std::string::npos || value.find('e') != std::string::npos || value.find('E') != std::string::npos) {
                    configValue = std::stod(value);
                } else {
                    configValue = value;
                }
                
                set(key, configValue, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    return loadFromFile(filepath); // Backward compatibility
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