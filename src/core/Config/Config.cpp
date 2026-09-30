#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>

namespace nlm {

    // JSON serialization/deserialization helpers
    void to_json(nlohmann::json& j, const ConfigEntry& entry) {
        j = nlohmann::json{{{"key", entry.key}, {"value", entry.value}, {"source", static_cast<int>(entry.source)}, {"description", entry.description}}};
    }

    void from_json(const nlohmann::json& j, ConfigEntry& entry) {
        entry.key = j.value("key", "");
        entry.value = j.value("value", ConfigValue());
        entry.source = static_cast<ConfigSource>(j.value("source", static_cast<int>(ConfigSource::Default)));
        entry.description = j.value("description", "");
    }

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string lastError;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    clear(); // Clear existing configuration
    
    // Try to parse as JSON first
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->lastError = "Failed to open configuration file: " + filepath;
        return false;
    }
    
    try {
        nlohmann::json j;
        file >> j;
        
        // Check if it's a JSON object
        if (!j.is_object()) {
            pImpl->lastError = "Configuration file is not a valid JSON object: " + filepath;
            return false;
        }
        
        // Parse JSON configuration
        for (const auto& [key, value] : j.items()) {
            ConfigSource source = ConfigSource::File;
            std::string description;
            
            // Try to get description if available
            if (j.contains(key) && j[key].is_object() && j[key].contains("description")) {
                description = j[key]["description"].get<std::string>();
            }
            
            set(key, value, source, description);
        }
        
        return true;
    } catch (const std::exception& e) {
        // Fallback to simple key=value format for backward compatibility
        file.clear();
        file.seekg(0, std::ios::beg);
        
        std::string line;
        int lineNum = 0;
        bool hasErrors = false;
        
        while (std::getline(file, line)) {
            ++lineNum;
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
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
                
                try {
                    set(key, value, ConfigSource::File);
                } catch (const std::exception& e) {
                    pImpl->lastError = "Error parsing line " + std::to_string(lineNum) + " in " + filepath + ": " + e.what();
                    hasErrors = true;
                }
            } else {
                // Comment or malformed line
                pImpl->lastError = "Malformed line " + std::to_string(lineNum) + " in " + filepath + ": " + line;
                hasErrors = true;
            }
        }
        
        if (hasErrors) {
            return false;
        }
        
        return true;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Clear command line arguments
    clear();
    
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
        pImpl->lastError = "Failed to open configuration file for writing: " + filepath;
        return false;
    }
    
    try {
        nlohmann::json j = nlohmann::json::array();
        
        for (const auto& entry : pImpl->entries) {
            nlohmann::json entryJson;
            entryJson["key"] = entry.key;
            
            // Convert ConfigValue to JSON
            std::visit([&entryJson](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    entryJson["value"] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    entryJson["value"] = arg;
                } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    entryJson["value"] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    entryJson["value"] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    entryJson["value"] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    entryJson["value"] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    entryJson["value"] = arg;
                } else {
                    entryJson["value"] = "UNKNOWN_TYPE";
                }
            }, entry.value);
            
            entryJson["source"] = static_cast<int>(entry.source);
            entryJson["description"] = entry.description;
            
            j.push_back(entryJson);
        }
        
        // Write JSON with indentation for readability
        file << j.dump(2);
        return true;
    } catch (const std::exception& e) {
        pImpl->lastError = "Error saving configuration to " + filepath + ": " + e.what();
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
    set(key, value, source, "");
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, int value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, double value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, bool value, ConfigSource source, const std::string& description) {
    set(key, ConfigValue(value), source, description);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->description = description; // Update description if key exists
    } else {
        pImpl->entries.emplace_back(key, value, source, description);
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
    pImpl->lastError.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    
    // Source name mapping
    const std::vector<std::string> sourceNames = {
        "Default", "File", "CommandLine", "Runtime"
    };
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                auto it = arg.begin();
                if (it != arg.end()) {
                    oss << *it;
                    for (++it; it != arg.end(); ++it) {
                        oss << ", " << *it;
                    }
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        
        oss << "] (";
        if (static_cast<size_t>(entry.source) < sourceNames.size()) {
            oss << sourceNames[static_cast<size_t>(entry.source)];
        } else {
            oss << "Unknown(" << static_cast<int>(entry.source) << ")";
        }
        oss << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
        oss << "\n";
    }
    
    if (!pImpl->lastError.empty()) {
        oss << "\nLast error: " << pImpl->lastError << "\n";
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
    std::string result;
    result.reserve(str.size());
    std::transform(str.begin(), str.end(), std::back_inserter(result), ::tolower);
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

// ConfigRegistry implementation

void ConfigRegistry::registerConfig(const std::string& key, const std::string& description, 
                                   ConfigCategory category, const std::string& exampleValue) {
    registry[key] = ConfigEntry(key, ConfigValue(exampleValue), ConfigSource::Default, description, category);
    keyToDescription[key] = description;
    keyToCategory[key] = category;
}

std::string ConfigRegistry::getDescription(const std::string& key) const {
    auto it = keyToDescription.find(key);
    return it != keyToDescription.end() ? it->second : "";
}

ConfigRegistry::ConfigCategory ConfigRegistry::getCategory(const std::string& key) const {
    auto it = keyToCategory.find(key);
    return it != keyToCategory.end() ? it->second : ConfigCategory::General;
}

std::vector<std::string> ConfigRegistry::getKeysByCategory(ConfigCategory category) const {
    std::vector<std::string> keys;
    for (const auto& [key, entry] : registry) {
        if (entry.category == category) {
            keys.push_back(key);
        }
    }
    return keys;
}

std::map<ConfigRegistry::ConfigCategory, std::vector<std::string>> ConfigRegistry::getAllCategories() const {
    std::map<ConfigCategory, std::vector<std::string>> categoryMap;
    for (const auto& [key, entry] : registry) {
        categoryMap[entry.category].push_back(key);
    }
    return categoryMap;
}

void ConfigRegistry::clear() {
    registry.clear();
    keyToDescription.clear();
    keyToCategory.clear();
}

// Configuration helper functions
namespace ConfigHelpers {
    std::string sourceToString(ConfigSource source) {
        switch (source) {
            case ConfigSource::Default: return "Default";
            case ConfigSource::File: return "File";
            case ConfigSource::CommandLine: return "CommandLine";
            case ConfigSource::Runtime: return "Runtime";
            default: return "Unknown";
        }
    }

    std::string categoryToString(ConfigCategory category) {
        if (static_cast<size_t>(category) < ConfigCategoryNames.size()) {
            return ConfigCategoryNames[static_cast<size_t>(category)];
        }
        return "Unknown";
    }

    ConfigType valueTypeToString(ConfigType type) {
        return type;
    }

    std::string valueToString(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                auto it = arg.begin();
                if (it != arg.end()) {
                    oss << *it;
                    for (++it; it != arg.end(); ++it) {
                        oss << ", " << *it;
                    }
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, value);
        return oss.str();
    }

    bool stringToValue(const std::string& str, ConfigValue& value) {
        // Try to determine type
        if (str == "true" || str == "false") {
            value = str == "true";
            return true;
        }
        
        // Try integer parsing
        try {
            size_t pos;
            int64_t intVal = std::stoll(str, &pos);
            if (pos == str.size()) {
                value = intVal;
                return true;
            }
        } catch (...) {}
        
        // Try double parsing
        try {
            size_t pos;
            double doubleVal = std::stod(str, &pos);
            if (pos == str.size()) {
                value = doubleVal;
                return true;
            }
        } catch (...) {}
        
        // Default to string
        value = str;
        return true;
    }

    ConfigType getValueType(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) return ConfigType::Integer;
        if (std::holds_alternative<int64_t>(value)) return ConfigType::Integer;
        if (std::holds_alternative<double>(value)) return ConfigType::Float;
        if (std::holds_alternative<bool>(value)) return ConfigType::Boolean;
        if (std::holds_alternative<std::string>(value)) return ConfigType::String;
        if (std::holds_alternative<std::vector<int>>(value)) return ConfigType::VectorInt;
        if (std::holds_alternative<std::vector<double>>(value)) return ConfigType::VectorFloat;
        if (std::holds_alternative<std::vector<std::string>>(value)) return ConfigType::VectorString;
        return ConfigType::Unknown;
    }

    bool isValidInteger(const std::string& str, int& result, int min, int max) {
        try {
            size_t pos;
            long long val = std::stoll(str, &pos);
            if (pos != str.size()) return false;
            if (val < min || val > max) return false;
            result = static_cast<int>(val);
            return true;
        } catch (...) {
            return false;
        }
    }

    bool isValidDouble(const std::string& str, double& result, double min, double max) {
        try {
            size_t pos;
            double val = std::stod(str, &pos);
            if (pos != str.size()) return false;
            if (val < min || val > max) return false;
            result = val;
            return true;
        } catch (...) {
            return false;
        }
    }

    bool isValidBool(const std::string& str, bool& result) {
        std::string lower = str;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (lower == "true" || lower == "t" || lower == "1") {
            result = true;
            return true;
        } else if (lower == "false" || lower == "f" || lower == "0") {
            result = false;
            return true;
        }
        return false;
    }
}

// Initialize global registry
ConfigRegistry Config::globalRegistry;

// ConfigRegistry implementation

void ConfigRegistry::registerConfig(const std::string& key, const std::string& description, 
                                   ConfigCategory category, const std::string& exampleValue) {
    registry[key] = ConfigEntry(key, ConfigValue(exampleValue), ConfigSource::Default, description, category);
    keyToDescription[key] = description;
    keyToCategory[key] = category;
}

std::string ConfigRegistry::getDescription(const std::string& key) const {
    auto it = keyToDescription.find(key);
    return it != keyToDescription.end() ? it->second : "";
}

ConfigRegistry::ConfigCategory ConfigRegistry::getCategory(const std::string& key) const {
    auto it = keyToCategory.find(key);
    return it != keyToCategory.end() ? it->second : ConfigCategory::General;
}

std::vector<std::string> ConfigRegistry::getKeysByCategory(ConfigCategory category) const {
    std::vector<std::string> keys;
    for (const auto& [key, entry] : registry) {
        if (entry.category == category) {
            keys.push_back(key);
        }
    }
    return keys;
}

std::map<ConfigRegistry::ConfigCategory, std::vector<std::string>> ConfigRegistry::getAllCategories() const {
    std::map<ConfigCategory, std::vector<std::string>> categoryMap;
    for (const auto& [key, entry] : registry) {
        categoryMap[entry.category].push_back(key);
    }
    return categoryMap;
}

void ConfigRegistry::clear() {
    registry.clear();
    keyToDescription.clear();
    keyToCategory.clear();
}
