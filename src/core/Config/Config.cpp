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
    // Check file extension to determine parser
    std::string ext = "." + std::string(
        std::filesystem::path(filepath).extension().string()
    );
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".yaml" || ext == ".yml") {
        return loadFromJSON(filepath);
    } else {
        // Default to simple key=value format
        return loadFromSimpleFormat(filepath);
    }
}

// Helper to parse simple key=value format
bool Config::loadFromSimpleFormat(const std::string& filepath) {
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

// Helper to parse JSON format
bool Config::loadFromJSON(const std::string& filepath) {
    // Simple JSON parser that handles basic types
    // In a real implementation, you would use a proper JSON library like nlohmann/json
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Very basic JSON parsing - for demonstration only
    // Real implementation should use a proper JSON parser
    
    // Simple parsing for key-value pairs in { "key": "value", "num": 123 }
    size_t braceStart = content.find('{');
    size_t braceEnd = content.rfind('}');
    
    if (braceStart == std::string::npos || braceEnd == std::string::npos) {
        return false;
    }
    
    std::string jsonContent = content.substr(braceStart + 1, braceEnd - braceStart - 1);
    
    // Parse key-value pairs (simplified - doesn't handle nested objects)
    size_t pos = 0;
    while (pos < jsonContent.length()) {
        // Find next key
        size_t keyStart = jsonContent.find('"', pos);
        if (keyStart == std::string::npos) break;
        
        size_t keyEnd = jsonContent.find('"', keyStart + 1);
        if (keyEnd == std::string::npos) break;
        
        std::string key = jsonContent.substr(keyStart + 1, keyEnd - keyStart - 1);
        
        // Skip whitespace and colon
        size_t afterKey = jsonContent.find(':', keyEnd);
        if (afterKey == std::string::npos) break;
        
        size_t valueStart = jsonContent.find_first_not_of(" ", afterKey + 1);
        if (valueStart == std::string::npos) break;
        
        // Find value end (next comma or closing brace)
        size_t commaPos = jsonContent.find(',', valueStart);
        size_t bracePos = jsonContent.find('}', valueStart);
        
        size_t valueEnd = std::min(
            (commaPos == std::string::npos) ? jsonContent.length() : commaPos,
            (bracePos == std::string::npos) ? jsonContent.length() : bracePos
        );
        
        std::string valueStr = jsonContent.substr(valueStart, valueEnd - valueStart);
        
        // Trim whitespace
        valueStr = trim(valueStr);
        
        // Try to infer type and set value
        if (valueStr.front() == '"' && valueStr.back() == '"') {
            // String value - remove quotes
            set(key, valueStr.substr(1, valueStr.length() - 2), ConfigSource::File);
        } else if (valueStr == "true" || valueStr == "false") {
            // Boolean value
            set(key, valueStr == "true", ConfigSource::File);
        } else {
            // Try numeric value
            try {
                if (valueStr.find('.') != std::string::npos) {
                    double doubleVal = std::stod(valueStr);
                    set(key, doubleVal, ConfigSource::File);
                } else {
                    int64_t intVal = std::stoll(valueStr);
                    set(key, intVal, ConfigSource::File);
                }
            } catch (...) {
                // If conversion fails, treat as string
                set(key, valueStr, ConfigSource::File);
            }
        }
        
        // Move to next position
        pos = valueEnd;
        if (pos < jsonContent.length() && jsonContent[pos] == ',') {
            pos++; // Skip comma
        }
    }
    
    return true;
}

// Default constructor implementation
Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

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
