#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cctype>
#include <string>
#include <cctype>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string filepath;  // Store the filepath for saving
};

// Helper function to trim whitespace
std::string Config::trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size() - 1;
    
    while (start <= end && std::isspace(static_cast<unsigned char>(str[start]))) {
        start++;
    }
    while (end >= start && std::isspace(static_cast<unsigned char>(str[end]))) {
        end--;
    }
    
    return (start <= end) ? str.substr(start, end - start + 1) : "";
}

// Helper function to parse key=value format with enhanced validation
bool Config::parseKeyValueLine(const std::string& line, std::string& key, std::string& value) {
    size_t equals_pos = line.find('=');
    if (equals_pos == std::string::npos) {
        return false;
    }
    
    key = trim(line.substr(0, equals_pos));
    value = trim(line.substr(equals_pos + 1));
    
    // Remove surrounding quotes
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    // Validate key (should not be empty and should only contain valid characters)
    if (key.empty() || !std::isalpha(static_cast<unsigned char>(key[0])) || 
        key.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_") != std::string::npos) {
        return false;
    }
    
    return !key.empty();
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    pImpl->filepath = filepath; // Store for potential save operations
    
    // Try to parse as JSON first (modern format)
    if (loadFromJSON(filepath)) {
        return true;
    }
    
    // Try to parse as YAML (alternative modern format)
    if (loadFromYAML(filepath)) {
        return true;
    }
    
    // Fall back to simple key=value format for backward compatibility
    return loadFromKeyValueFile(filepath);
}

bool Config::loadFromJSON(const std::string& filepath) {
    // Simple JSON parsing - look for files with .json extension
    if (filepath.size() < 5 || filepath.substr(filepath.size() - 5) != ".json") {
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, try to parse as simple JSON-like format
    // This is a simplified implementation - real JSON parsing would need a proper parser
    nlohmann::json jsonData;
    try {
        file >> jsonData;
        
        // Convert JSON to config entries
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            const nlohmann::json& value = it.value();
            
            // Convert value to string representation based on type
            std::string valueStr;
            if (value.is_string()) {
                valueStr = "\"" + value.get<std::string>() + "\"";
            } else if (value.is_number_integer()) {
                valueStr = std::to_string(value.get<int64_t>());
            } else if (value.is_number_float()) {
                valueStr = std::to_string(value.get<double>());
            } else if (value.is_boolean()) {
                valueStr = value.get<bool>() ? "true" : "false";
            } else if (value.is_null()) {
                valueStr = "null";
            } else if (value.is_object() || value.is_array()) {
                // Skip complex types for now
                continue;
            }
            
            set(key, valueStr, ConfigSource::File);
        }
        
        pImpl->filepath = filepath;
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    // Simple YAML parsing - look for files with .yaml or .yml extension
    if (filepath.size() < 5) return false;
    std::string ext = filepath.substr(filepath.size() - 5);
    if (ext != ".yaml" && ext != ".yml") {
        return false;
    }
    
    // For now, delegate to key-value parser with YAML-style comment removal
    // A full YAML implementation would use a proper parser
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Remove YAML comments
        size_t commentPos = line.find('#');
        if (commentPos != std::string::npos) {
            line = line.substr(0, commentPos);
        }
        
        // Remove YAML directives
        if (line.find("%YAML") == 0 || line.find("%TAG") == 0 || line.find("%!")) {
            continue;
        }
        
        // Simple key: value format (YAML-like)
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string key = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            // Remove surrounding quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            if (!key.empty()) {
                set(key, value, ConfigSource::File);
            }
        }
    }
    
    return true;
}

bool Config::loadFromKeyValueFile(const std::string& filepath) {
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
        parseKeyValueLine(line, pImpl->currentKey, pImpl->currentValue);
        if (!pImpl->currentKey.empty()) {
            set(pImpl->currentKey, pImpl->currentValue, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save in JSON format for modern applications
    file << "{\n";
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) file << ",\n";
        first = false;
        
        file << "  \"" << entry.key << "\": ";
        std::visit([&file](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, double> || 
                                std::is_same_v<T, float> ||
                                std::is_same_v<T, int64_t> ||
                                std::is_same_v<T, int>) {
                file << arg;
            } else {
                file << "\"" << arg << "\"";
            }
        }, entry.value);
    }
    file << "\n}\n";
    
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