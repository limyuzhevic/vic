#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <map>
#include <set>
#include <variant>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigSchemaEntry> schema;
    std::vector<std::string> validationErrors;
    std::string includeDirectory;
    std::set<std::string> visitedFiles; // For cycle detection in includes
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // First try to detect file type
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".js") {
        return loadFromJsonFile(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYamlFile(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
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
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
}

bool Config::loadFromYamlFile(const std::string& filepath) {
    std::map<std::string, ConfigValue> data;
    if (parseYamlFile(filepath, data)) {
        clear();
        for (const auto& [key, value] : data) {
            set(key, value, ConfigSource::File);
        }
        return true;
    }
    return false;
}

bool Config::loadFromJsonFile(const std::string& filepath) {
    std::map<std::string, ConfigValue> data;
    if (parseJsonFile(filepath, data)) {
        clear();
        for (const auto& [key, value] : data) {
            set(key, value, ConfigSource::File);
        }
        return true;
    }
    return false;
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

bool Config::loadFromJsonString(const std::string& jsonString) {
    std::map<std::string, ConfigValue> data;
    // TODO: Implement JSON string parsing
    // For now, just return false
    return false;
}

bool Config::loadFromYamlString(const std::string& yamlString) {
    std::map<std::string, ConfigValue> data;
    // TODO: Implement YAML string parsing
    // For now, just return false
    return false;
}

bool Config::saveToFile(const std::string& filepath, bool prettyPrint) const {
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json" || ext == ".js") {
        return saveToJsonFile(filepath, prettyPrint);
    } else if (ext == ".yaml" || ext == ".yml") {
        return saveToYamlFile(filepath, prettyPrint);
    } else {
        // Default to simple key=value format
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
}

bool Config::saveToJsonFile(const std::string& filepath, bool prettyPrint) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "  \"" << entry.key << "\": ";
        
        std::visit([&file](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else {
                file << arg;
            }
        }, entry.value);
        
        if (i < pImpl->entries.size() - 1) {
            file << ",\n";
        } else {
            file << "\n";
        }
    }
    file << "}\n";
    return true;
}

bool Config::saveToYamlFile(const std::string& filepath, bool prettyPrint) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        std::visit([&file](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << ": \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << ": " << (arg ? "true" : "false") << "\n";
            } else if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
                file << entry.key << ": " << arg << "\n";
            } else if constexpr (std::is_floating_point_v<T>) {
                file << entry.key << ": " << arg << "\n";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << entry.key << ":\n";
                for (const auto& item : arg) {
                    file << "  - \"" << item << "\"\n";
                }
            } else {
                file << entry.key << ": " << arg << "\n";
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

std::optional<std::string> Config::getString(const std::string& key) const {
    return get<std::string>(key);
}

std::optional<int> Config::getInt(const std::string& key) const {
    return get<int>(key);
}

std::optional<double> Config::getDouble(const std::string& key) const {
    return get<double>(key);
}

std::optional<bool> Config::getBool(const std::string& key) const {
    return get<bool>(key);
}

std::optional<std::vector<std::string>> Config::getStringArray(const std::string& key) const {
    return get<std::vector<std::string>>(key);
}

std::optional<std::vector<int>> Config::getIntArray(const std::string& key) const {
    return get<std::vector<int>>(key);
}

std::optional<std::vector<double>> Config::getDoubleArray(const std::string& key) const {
    return get<std::vector<double>>(key);
}

bool Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source) {
    // Validate dependencies first
    auto entry = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                             [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entry != pImpl->entries.end() && !entry->dependsOn.empty()) {
        for (const auto& depKey : entry->dependsOn) {
            if (!has(depKey)) {
                pImpl->validationErrors.push_back(
                    "Key '" + key + "' depends on missing key '" + depKey + "'"
                );
                return false;
            }
        }
    }
    
    // Validate against schema if present
    auto schemaIt = pImpl->schema.find(key);
    if (schemaIt != pImpl->schema.end()) {
        if (!validateRange(value, schemaIt->second.range)) {
            pImpl->validationErrors.push_back(
                "Key '" + key + "' value violates range constraints"
            );
            return false;
        }
        if (!validateCustom(value, schemaIt->second.validator)) {
            pImpl->validationErrors.push_back(
                "Key '" + key + "': " + schemaIt->second.validationMessage
            );
            return false;
        }
    }
    
    // Set the value
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->validated = true;  // Will be validated later
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    return true;
}

bool Config::set(const std::string& key, const std::string& value, 
                 ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, int value, 
                 ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, double value, 
                 ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, bool value, 
                 ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::validate(const std::string& key) {
    auto entry = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                             [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entry == pImpl->entries.end()) {
        return false;
    }
    
    // Clear previous errors for this key
    pImpl->validationErrors.erase(
        std::remove_if(pImpl->validationErrors.begin(), pImpl->validationErrors.end(),
                      [&key](const std::string& err) { return err.find("'" + key + "'") != std::string::npos; }),
        pImpl->validationErrors.end()
    );
    
    // Check dependencies
    if (!entry->dependsOn.empty()) {
        for (const auto& depKey : entry->dependsOn) {
            if (!has(depKey)) {
                pImpl->validationErrors.push_back(
                    "Key '" + key + "' depends on missing key '" + depKey + "'"
                );
                entry->validated = false;
                return false;
            }
        }
    }
    
    // Validate against schema
    auto schemaIt = pImpl->schema.find(key);
    if (schemaIt != pImpl->schema.end()) {
        if (!validateRange(entry->value, schemaIt->second.range)) {
            pImpl->validationErrors.push_back(
                "Key '" + key + "' value violates range constraints"
            );
            entry->validated = false;
            return false;
        }
        if (!validateCustom(entry->value, schemaIt->second.validator)) {
            pImpl->validationErrors.push_back(
                "Key '" + key + "': " + schemaIt->second.validationMessage
            );
            entry->validated = false;
            return false;
        }
    }
    
    entry->validated = true;
    return true;
}

bool Config::validateAll() {
    pImpl->validationErrors.clear();
    bool allValid = true;
    
    for (auto& entry : pImpl->entries) {
        if (!validate(entry.key)) {
            allValid = false;
        }
    }
    
    return allValid;
}

void Config::addSchemaEntry(const std::string& key, const ConfigRange& range,
                           ValidationFunc validator, const std::string& message) {
    ConfigSchemaEntry schema;
    schema.key = key;
    schema.range = range;
    schema.validator = validator;
    schema.validationMessage = message;
    pImpl->schema[key] = schema;
}

void Config::removeSchemaEntry(const std::string& key) {
    pImpl->schema.erase(key);
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

std::vector<ConfigEntry> Config::getEntriesBySource(ConfigSource source) const {
    std::vector<ConfigEntry> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            result.push_back(entry);
        }
    }
    return result;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->schema.clear();
    pImpl->validationErrors.clear();
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
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.validated) {
            oss << " [UNVALIDATED]";
        }
        oss << "\n";
    }
    return oss.str();
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->validationErrors;
}

void Config::setIncludeDirectory(const std::string& directory) {
    pImpl->includeDirectory = directory;
}

bool Config::loadFromFileWithInheritance(const std::string& filepath) {
    std::map<std::string, ConfigValue> data;
    if (loadConfigFile(filepath, data)) {
        clear();
        for (const auto& [key, value] : data) {
            set(key, value, ConfigSource::File);
        }
        return true;
    }
    return false;
}

bool Config::loadFromYamlFileWithInheritance(const std::string& filepath) {
    std::map<std::string, ConfigValue> data;
    if (loadConfigFile(filepath, data)) {
        clear();
        for (const auto& [key, value] : data) {
            set(key, value, ConfigSource::File);
        }
        return true;
    }
    return false;
}

bool Config::loadFromJsonFileWithInheritance(const std::string& filepath) {
    std::map<std::string, ConfigValue> data;
    if (loadConfigFile(filepath, data)) {
        clear();
        for (const auto& [key, value] : data) {
            set(key, value, ConfigSource::File);
        }
        return true;
    }
    return false;
}

bool Config::loadFromFileWithIncludes(const std::string& filepath) {
    if (pImpl->visitedFiles.find(filepath) != pImpl->visitedFiles.end()) {
        return true; // Already processed, avoid cycles
    }
    pImpl->visitedFiles.insert(filepath);
    
    // Process the file itself
    bool success = loadFromFile(filepath);
    
    // Process included files
    if (success && !pImpl->includeDirectory.empty()) {
        std::string searchPath = pImpl->includeDirectory + "/" + std::filesystem::path(filepath).stem().string() + "_includes";
        if (std::filesystem::exists(searchPath)) {
            for (const auto& entry : std::filesystem::directory_iterator(searchPath)) {
                if (entry.is_regular_file() && 
                    (entry.path().extension() == ".json" || 
                     entry.path().extension() == ".yaml" ||
                     entry.path().extension() == ".yml")) {
                    loadFromFileWithIncludes(entry.path().string());
                }
            }
        }
    }
    
    pImpl->visitedFiles.erase(filepath);
    return success;
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        // Find if key already exists
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        if (it != pImpl->entries.end()) {
            // Keep existing entry if it has higher priority (CommandLine > File > Runtime > Default)
            if (it->source == ConfigSource::CommandLine || entry.source == ConfigSource::Default) {
                // Keep existing
            } else {
                *it = entry;
            }
        } else {
            pImpl->entries.push_back(entry);
        }
    }
}

bool Config::operator==(const Config& other) const {
    return pImpl->entries == other.pImpl->entries;
}

bool Config::operator!=(const Config& other) const {
    return !(*this == other);
}

Config::Stats Config::getStats() const {
    Stats stats;
    stats.totalEntries = pImpl->entries.size();
    
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::File: stats.fileEntries++; break;
            case ConfigSource::CommandLine: stats.commandLineEntries++; break;
            case ConfigSource::Runtime: stats.runtimeEntries++; break;
            case ConfigSource::Default: break;
        }
    }
    
    stats.validatedEntries = std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
                                        [](const ConfigEntry& e) { return e.validated; });
    
    return stats;
}

bool Config::validateRange(const ConfigValue& value, const ConfigRange& range) const {
    std::visit([&](const auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_arithmetic_v<T>) {
            double val = static_cast<double>(arg);
            switch (range.type) {
                case ConfigRange::Type::Min:
                    return val >= range.value;
                case ConfigRange::Type::Max:
                    return val <= range.value;
                case ConfigRange::Type::MinMax:
                    return val >= range.value && val <= range.value;
                case ConfigRange::Type::InclusiveMin:
                    return val >= range.value;
                case ConfigRange::Type::InclusiveMax:
                    return val <= range.value;
            }
        }
    }, value);
    return true; // Default to passing for non-arithmetic types
}

bool Config::validateCustom(const ConfigValue& value, const ValidationFunc& validator) const {
    return validator(value);
}

bool Config::loadConfigFile(const std::string& filepath, std::map<std::string, ConfigValue>& data,
                           const std::string& basePath) {
    std::string fullPath = basePath.empty() ? filepath : basePath + "/" + filepath;
    
    std::ifstream file(fullPath);
    if (!file.is_open()) {
        return false;
    }
    
    // Determine file type by extension
    std::string ext = "." + std::filesystem::path(fullPath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return parseJsonFile(fullPath, data);
    } else if (ext == ".yaml" || ext == ".yml") {
        return parseYamlFile(fullPath, data);
    } else {
        // Simple key=value format
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                data[key] = value;
            }
        }
        return true;
    }
}

bool Config::parseJsonFile(const std::string& filepath, std::map<std::string, ConfigValue>& data) {
    // TODO: Implement actual JSON parsing with nlohmann/json
    // For now, return false to use simple parser
    return false;
}

bool Config::parseYamlFile(const std::string& filepath, std::map<std::string, ConfigValue>& data) {
    // TODO: Implement actual YAML parsing with YAML-CPP
    // For now, return false to use simple parser
    return false;
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