// Include basic nlohmann/json interface
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    time_t lastModified = 0;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->lastModified = std::time(nullptr);
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Try JSON first (preferred format)
    if (loadFromJSON(filepath)) {
        return true;
    }
    
    // Fall back to simple key=value format (backward compatibility)
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        // Validate JSON structure
        if (!j.is_object()) {
            return false;
        }
        
        // Clear existing entries before loading new ones
        clear();
        
        // Load entries from JSON
        for (auto it = j.begin(); it != j.end(); ++it) {
            std::string key = it->first;
            
            // Skip special keys
            if (key == "__descriptions__" || key == "__schema__") {
                continue;
            }
            
            try {
                ConfigValue value = jsonToConfigValue(it->second);
                set(key, value, ConfigSource::File);
            } catch (const std::exception&) {
                // Continue with other values
            }
        }
        
        pImpl->lastModified = std::time(nullptr);
        return true;
        
    } catch (const json::exception&) {
        return false;
    } catch (const std::exception&) {
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        // Clear existing entries before loading new ones
        clear();
        
        std::string line;
        size_t lineNumber = 0;
        
        while (std::getline(file, line)) {
            lineNumber++;
            
            // Skip comments and empty lines
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            // Parse key=value format
            size_t equalsPos = line.find('=');
            if (equalsPos == std::string::npos) {
                continue;
            }
            
            std::string key = trim(line.substr(0, equalsPos));
            std::string valueStr = trim(line.substr(equalsPos + 1));
            
            // Remove surrounding quotes if present
            if (valueStr.size() >= 2 && ((valueStr.front() == '"' && valueStr.back() == '"') ||
                                        (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            // Try to parse value as different types
            bool parsed = false;
            
            // Try boolean
            if (valueStr == "true" || valueStr == "false") {
                bool value = (valueStr == "true");
                set(key, value, ConfigSource::File);
                parsed = true;
            }
            // Try integer
            else if (std::all_of(valueStr.begin(), valueStr.end(), ::isdigit)) {
                int value = std::stoi(valueStr);
                set(key, value, ConfigSource::File);
                parsed = true;
            }
            // Try double
            else {
                try {
                    double value = std::stod(valueStr);
                    set(key, value, ConfigSource::File);
                    parsed = true;
                } catch (...) {
                    // Fall back to string
                    set(key, valueStr, ConfigSource::File);
                    parsed = true;
                }
            }
            
            if (!parsed) {
                // Continue with other values
            }
        }
        
        pImpl->lastModified = std::time(nullptr);
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
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
        // Simple string representation for non-vector types
        if (std::holds_alternative<std::string>(entry.value)) {
            oss << "\"" << std::get<std::string>(entry.value) << "\"";
        } else if (std::holds_alternative<int>(entry.value)) {
            oss << std::get<int>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            oss << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            oss << std::get<bool>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            oss << std::get<int64_t>(entry.value);
        } else {
            oss << "unsupported_type";
        }
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
namespace nlm {
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
}

// Template instantiations included at the end of the file
#include "Config.cpp.inl"

} // namespace nlm
#include "Config.cpp.inl"