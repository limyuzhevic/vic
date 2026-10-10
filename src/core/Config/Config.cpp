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
    // Improved parser that handles both simple key=value format and JSON-like format
    // Supports: key=value, key="value with spaces", key=true/false, key=123, key=3.14
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Handle JSON-like format (simplified)
        if (line.find('{') == 0) {
            // Very basic JSON object support - extract key-value pairs
            // This is a simplified implementation for Phase 2
            try {
                // Find first "key": "value" pattern
                size_t keyPos = line.find('"');
                if (keyPos != std::string::npos) {
                    size_t keyEnd = line.find('"', keyPos + 1);
                    if (keyEnd != std::string::npos) {
                        std::string key = trim(line.substr(keyPos + 1, keyEnd - keyPos - 1));
                        
                        size_t valuePos = line.find('"', keyEnd + 1);
                        if (valuePos != std::string::npos) {
                            size_t valueEnd = line.find('"', valuePos + 1);
                            if (valueEnd != std::string::npos) {
                                std::string value = trim(line.substr(valuePos + 1, valueEnd - valuePos - 1));
                                set(key, value, ConfigSource::File);
                            }
                        }
                    }
                }
            } catch (const std::exception&) {
                // If JSON parsing fails, fall back to simple format
                parseSimpleLine(line);
            }
        } else {
            // Parse simple key=value pairs with enhanced support
            parseSimpleLine(line);
        }
    }
    
    return true;
}

void Config::parseSimpleLine(const std::string& line) {
    // Enhanced simple parser with better type detection
    size_t pos = line.find('=');
    if (pos != std::string::npos) {
        std::string key = trim(line.substr(0, pos));
        std::string valueStr = trim(line.substr(pos + 1));
        
        // Remove surrounding quotes
        if (valueStr.size() >= 2 && 
            ((valueStr.front() == '"' && valueStr.back() == '"') ||
             (valueStr.front() == '\'' && valueStr.back() == '\''))) {
            valueStr = valueStr.substr(1, valueStr.size() - 2);
        }
        
        // Detect value type and parse appropriately
        if (valueStr.empty()) {
            set(key, valueStr, ConfigSource::File);
        } else if (valueStr == "true" || valueStr == "false") {
            set(key, valueStr == "true", ConfigSource::File);
        } else {
            // Try to parse as number
            try {
                // Check if it's an integer
                size_t posEnd;
                int64_t intValue = std::stoll(valueStr, &posEnd);
                if (posEnd == valueStr.size()) {
                    set(key, intValue, ConfigSource::File);
                    return;
                }
                
                // Check if it's a floating point
                double floatValue = std::stod(valueStr, &posEnd);
                if (posEnd == valueStr.size()) {
                    set(key, floatValue, ConfigSource::File);
                    return;
                }
                
                // If both fail, treat as string
                set(key, valueStr, ConfigSource::File);
                
            } catch (const std::exception&) {
                // If parsing fails, treat as string
                set(key, valueStr, ConfigSource::File);
            }
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

void Config::parseSimpleLine(const std::string& line) {
    // Enhanced simple parser with better type detection
    size_t pos = line.find('=');
    if (pos != std::string::npos) {
        std::string key = trim(line.substr(0, pos));
        std::string valueStr = trim(line.substr(pos + 1));
        
        // Remove surrounding quotes
        if (valueStr.size() >= 2 && 
            ((valueStr.front() == '"' && valueStr.back() == '"') ||
             (valueStr.front() == '\'' && valueStr.back() == '\''))) {
            valueStr = valueStr.substr(1, valueStr.size() - 2);
        }
        
        // Detect value type and parse appropriately
        if (valueStr.empty()) {
            set(key, valueStr, ConfigSource::File);
        } else if (valueStr == "true" || valueStr == "false") {
            set(key, valueStr == "true", ConfigSource::File);
        } else {
            // Try to parse as number
            try {
                // Check if it's an integer
                size_t posEnd;
                int64_t intValue = std::stoll(valueStr, &posEnd);
                if (posEnd == valueStr.size()) {
                    set(key, intValue, ConfigSource::File);
                    return;
                }
                
                // Check if it's a floating point
                double floatValue = std::stod(valueStr, &posEnd);
                if (posEnd == valueStr.size()) {
                    set(key, floatValue, ConfigSource::File);
                    return;
                }
                
                // If both fail, treat as string
                set(key, valueStr, ConfigSource::File);
                
            } catch (const std::exception&) {
                // If parsing fails, treat as string
                set(key, valueStr, ConfigSource::File);
            }
        }
    }
}
