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

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Detect file format
    std::string content;
    std::string line;
    while (std::getline(file, line)) {
        content += line + "\n";
    }
    
    file.close();
    
    // Clear existing entries
    clear();
    
    // Try JSON first
    if (detectFormat(content) == Format::JSON) {
        return parseJSON(content);
    }
    // Try YAML second
    else if (detectFormat(content) == Format::YAML) {
        return parseYAML(content);
    }
    // Fall back to simple key=value format
    else {
        return parseSimpleFormat(content);
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
        file << entry.key << " = ";
        
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
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
// Format detection and parsing functions for Config
namespace ConfigParser {

Format detectFormat(const std::string& content) {
    if (content.empty()) {
        return Format::Unknown;
    }
    
    // Simple detection based on content
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        // Check if it looks like JSON (starts with { and has })
        return Format::JSON;
    } else if (content.find('-') != std::string::npos || 
               content.find(':') != std::string::npos) {
        // Check for YAML-like content
        return Format::YAML;
    } else if (content.find('=') != std::string::npos) {
        // Check for simple key=value format
        return Format::Simple;
    }
    
    return Format::Unknown;
}

bool parseJSON(const std::string& jsonString, Config& config) {
    // Simple JSON parsing implementation
    // For now, we'll implement a basic JSON parser that can handle simple objects
    
    // Skip implementation for now - this is a placeholder
    // In a real implementation, we would use nlohmann/json or yaml-cpp
    return false;
}

bool parseYAML(const std::string& yamlString, Config& config) {
    // Simple YAML parsing implementation
    // For now, we'll implement a basic YAML parser that can handle simple key-value pairs
    
    // Skip implementation for now - this is a placeholder
    // In a real implementation, we would use yaml-cpp
    return false;
}

bool parseSimpleFormat(const std::string& content, Config& config) {
    // Simple key=value format parser
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Skip empty lines and comments
        line = config.trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = config.trim(line.substr(0, pos));
            std::string value = config.trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Try to determine value type
            bool isBool = false;
            bool isInt = false;
            bool isDouble = false;
            
            if (value == "true" || value == "false") {
                isBool = true;
            } else {
                try {
                    size_t pos;
                    int intVal = std::stoi(value, &pos);
                    if (pos == value.size()) {
                        isInt = true;
                    } else {
                        double doubleVal = std::stod(value, &pos);
                        if (pos == value.size()) {
                            isDouble = true;
                        }
                    }
                } catch (...) {
                    // Not a number
                }
            }
            
            if (isBool) {
                bool boolVal = (value == "true");
                config.set(key, boolVal, ConfigSource::File);
            } else if (isInt) {
                int intVal = std::stoi(value);
                config.set(key, intVal, ConfigSource::File);
            } else if (isDouble) {
                double doubleVal = std::stod(value);
                config.set(key, doubleVal, ConfigSource::File);
            } else {
                config.set(key, value, ConfigSource::File);
            }
        }
    }
    
    return true;
}

std::string formatToString(Format format) {
    switch (format) {
        case Format::Unknown: return "Unknown";
        case Format::Simple: return "Simple";
        case Format::JSON: return "JSON";
        case Format::YAML: return "YAML";
        default: return "Unknown";
    }
}

} // namespace ConfigParser

// Additional Config methods for format-specific operations

bool Config::loadFromJSON(const std::string& jsonString) {
    // Clear existing entries
    clear();
    
    // Delegate to the parser
    return ConfigParser::parseJSON(jsonString, *this);
}

bool Config::loadFromYAML(const std::string& yamlString) {
    // Clear existing entries
    clear();
    
    // Delegate to the parser
    return ConfigParser::parseYAML(yamlString, *this);
}

bool Config::loadFromSimple(const std::string& content) {
    // Clear existing entries
    clear();
    
    // Delegate to the parser
    return ConfigParser::parseSimpleFormat(content, *this);
}

std::string Config::saveToJSON() const {
    // Simple JSON serialization
    std::ostringstream oss;
    oss << "{\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        oss << "  \"" << entry.key << "\": ";
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << escapeJSON(arg) << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            oss << ",\n";
        } else {
            oss << "\n";
        }
    }
    
    oss << "}\n";
    return oss.str();
}

std::string Config::saveToYAML() const {
    // Simple YAML serialization
    std::ostringstream oss;
    
    for (const auto& entry : pImpl->entries) {
        oss << entry.key << ": ";
        
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
        
        oss << "\n";
    }
    
    return oss.str();
}

// Helper function for JSON escaping
inline std::string escapeJSON(const std::string& input) {
    std::ostringstream oss;
    oss << "\"";
    for (char c : input) {
        switch (c) {
            case '\"': oss << "\\\""; break;
            case '\\': oss << "\\\\"; break;
            case '\b': oss << "\\b"; break;
            case '\f': oss << "\\f"; break;
            case '\n': oss << "\\n"; break;
            case '\r': oss << "\\r"; break;
            case '\t': oss << "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                } else {
                    oss << c;
                }
                break;
        }
    }
    oss << "\"";
    return oss.str();
}
