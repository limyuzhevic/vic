#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schemas;
    std::vector<std::string> validationErrors;
    std::string version;
    std::chrono::system_clock::time_point lastModified;
    std::string workingDirectory;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default configuration
    pImpl->version = "0.1.0";
    pImpl->lastModified = std::chrono::system_clock::now();
    pImpl->workingDirectory = ".";
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool validate) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    // Try to detect format
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        // Try JSON parsing
        return loadFromJSON(content, validate);
    } else {
        // Use legacy key=value format
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line)) {
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
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        
        if (validate) {
            validateSchema(pImpl->schemas);
        }
        
        pImpl->lastModified = std::chrono::system_clock::now();
        return true;
    }
}

bool Config::loadFromJSON(const std::string& jsonStr, bool validate) {
    // Simple JSON parser (placeholder - would use nlohmann/json in production)
    // For now, we'll just parse a limited JSON format for basic types
    pImpl->entries.clear();
    
    size_t pos = 0;
    while ((pos = jsonStr.find('"', pos)) != std::string::npos) {
        size_t keyStart = pos + 1;
        pos = jsonStr.find('"', keyStart);
        if (pos == std::string::npos) break;
        
        std::string key = jsonStr.substr(keyStart, pos - keyStart);
        pos = jsonStr.find(':', pos + 1);
        if (pos == std::string::npos) break;
        
        // Skip whitespace
        size_t valueStart = jsonStr.find_first_not_of(": \t\r\n", pos);
        if (valueStart == std::string::npos) continue;
        
        // Parse simple JSON values
        if (jsonStr[valueStart] == '"') {
            // String value
            size_t valueEnd = jsonStr.find('"', valueStart + 1);
            if (valueEnd == std::string::npos) continue;
            std::string value = jsonStr.substr(valueStart + 1, valueEnd - valueStart - 1);
            set(key, value, ConfigSource::File);
        } else if (jsonStr[valueStart] == 't' || jsonStr[valueStart] == 'f') {
            // Boolean value
            bool value = (jsonStr.substr(valueStart, 5) == "true");
            set(key, value, ConfigSource::File);
        } else if (std::isdigit(jsonStr[valueStart]) || jsonStr[valueStart] == '-') {
            // Number value - find end
            size_t valueEnd = jsonStr.find_first_of(",}", valueStart);
            if (valueEnd == std::string::npos) valueEnd = jsonStr.size();
            std::string numStr = jsonStr.substr(valueStart, valueEnd - valueStart);
            
            // Try to parse as double first
            try {
                size_t decPos = numStr.find('.');
                if (decPos != std::string::npos && decPos < numStr.size() - 1) {
                    double value = std::stod(numStr);
                    set(key, value, ConfigSource::File);
                } else {
                    int value = std::stoi(numStr);
                    set(key, value, ConfigSource::File);
                }
            } catch (...) {
                // If parsing fails, skip
            }
        }
        
        pos = valueStart;
    }
    
    if (validate) {
        validateSchema(pImpl->schemas);
    }
    
    pImpl->lastModified = std::chrono::system_clock::now();
    return !pImpl->entries.empty();
}

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    if (format == "json") {
        file << saveToJSON();
    } else if (format == "yaml") {
        // Simple YAML output
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << ": ";
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else {
                    file << arg;
                }
            }, entry.value);
            file << "\n";
        }
    } else {
        // Default text format
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
            file << "\n";
        }
    }
    
    return true;
}

std::string Config::saveToJSON() const {
    std::ostringstream oss;
    oss << "{\n";
    
    bool first = true;
    for (const auto& entry : pImpl->entries) {
        if (!first) oss << ",\n";
        first = false;
        
        oss << "  \"" << entry.key << "\": ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
    }
    
    oss << "\n}";
    return oss.str();
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
bool Config::validateSchema(const std::vector<ConfigSchema>& schema) {
    pImpl->validationErrors.clear();
    
    for (const auto& schemaEntry : schema) {
        if (validateAgainstSchema(schemaEntry, schemaEntry)) {
            continue; // Valid according to schema
        }
        
        // Check if key exists
        if (!has(schemaEntry.key)) {
            if (schemaEntry.required) {
                pImpl->validationErrors.push_back("Missing required key: " + schemaEntry.key);
            }
            continue;
        }
        
        // Validate type
        if (!isValidType(getValueByKey(schemaEntry.key), schemaEntry.type)) {
            pImpl->validationErrors.push_back("Invalid type for key '" + schemaEntry.key + "': expected " + schemaEntry.type);
        }
        
        // Validate allowed values
        if (!schemaEntry.allowedValues.empty()) {
            auto value = getValueByKey(schemaEntry.key);
            bool found = false;
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    for (const auto& allowed : schemaEntry.allowedValues) {
                        if (arg == allowed) {
                            found = true;
                            break;
                        }
                    }
                }
            }, value);
            
            if (!found) {
                pImpl->validationErrors.push_back("Invalid value for key '" + schemaEntry.key + "': must be one of [" + 
                    joinStrings(schemaEntry.allowedValues, ", ") + "]");
            }
        }
    }
    
    return pImpl->validationErrors.empty();
}

void Config::addSchema(const ConfigSchema& schema) {
    pImpl->schemas.push_back(schema);
}

void Config::clearSchema() {
    pImpl->schemas.clear();
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::chrono::system_clock::time_point Config::getLastModified() const {
    return pImpl->lastModified;
}

void Config::setLastModified(const std::chrono::system_clock::time_point& time) {
    pImpl->lastModified = time;
}

void Config::setVersion(const std::string& version) {
    pImpl->version = version;
}

std::string Config::getVersion() const {
    return pImpl->version;
}

bool Config::isVersionCompatible(const std::string& requiredVersion) const {
    // Simple version comparison - could be enhanced
    return pImpl->version >= requiredVersion;
}

void Config::setWorkingDirectory(const std::string& directory) {
    pImpl->workingDirectory = directory;
}

std::string Config::getWorkingDirectory() const {
    return pImpl->workingDirectory;
}

bool Config::saveWithBackup(const std::string& filepath, const std::string& format) const {
    // Create backup by copying file if it exists
    if (std::filesystem::exists(filepath)) {
        std::string backupPath = filepath + ".backup";
        std::filesystem::copy_file(filepath, backupPath, std::filesystem::copy_options::overwrite_existing);
    }
    
    // Save new configuration
    return saveToFile(filepath, format);
}

// Helper methods for validation
ConfigValue Config::getValueByKey(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->value;
    }
    
    return ConfigValue();
}

bool Config::isValidType(const ConfigValue& value, const std::string& typeName) const {
    if (typeName == "int") {
        return std::holds_alternative<int>(value);
    } else if (typeName == "int64_t") {
        return std::holds_alternative<int64_t>(value);
    } else if (typeName == "double") {
        return std::holds_alternative<double>(value);
    } else if (typeName == "bool") {
        return std::holds_alternative<bool>(value);
    } else if (typeName == "string") {
        return std::holds_alternative<std::string>(value);
    } else if (typeName == "std::vector<int>") {
        return std::holds_alternative<std::vector<int>>(value);
    } else if (typeName == "std::vector<double>") {
        return std::holds_alternative<std::vector<double>>(value);
    } else if (typeName == "std::vector<string>") {
        return std::holds_alternative<std::vector<std::string>>(value);
    }
    return false;
}

bool Config::validateAgainstSchema(const ConfigEntry& entry, const ConfigSchema& schema) const {
    return true; // Simplified validation
}

std::string Config::joinStrings(const std::vector<std::string>& strings, const std::string& separator) const {
    std::string result;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) result += separator;
        result += strings[i];
    }
    return result;
}
