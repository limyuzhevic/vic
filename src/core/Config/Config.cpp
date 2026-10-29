#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <json.hpp>  // nlohmann::json
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigSchema> schema;
    size_t accessCount;
    mutable size_t totalAccessTime;
    
    Impl() : accessCount(0), totalAccessTime(0) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize logger
    auto logger = spdlog::stdout_color_mt("Config");
    logger->set_level(spdlog::level::info);
    spdlog::set_default_logger(logger);
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    if (isJsonFile(filepath)) {
        return loadJsonFromFile(filepath);
    } else if (isYamlFile(filepath)) {
        return loadYamlFromFile(filepath);
    } else {
        return loadSimpleFromFile(filepath);
    }
}

bool Config::loadFromJson(const nlohmann::json& jsonData) {
    try {
        clear();
        
        for (const auto& [key, value] : jsonData.items()) {
            ConfigValue configValue;
            
            if (value.is_number_integer()) {
                configValue = value.get<int>();
            } else if (value.is_number_unsigned()) {
                configValue = value.get<uint64_t>();
            } else if (value.is_number_float()) {
                configValue = value.get<double>();
            } else if (value.is_boolean()) {
                configValue = value.get<bool>();
            } else if (value.is_string()) {
                configValue = value.get<std::string>();
            } else if (value.is_array()) {
                // Try to infer array type
                if (!value.empty()) {
                    if (std::all_of(value.begin(), value.end(), 
                                   [](const auto& v) { return v.is_number_integer(); })) {
                        configValue = value.get<std::vector<int>>();
                    } else if (std::all_of(value.begin(), value.end(), 
                                         [](const auto& v) { return v.is_number_float(); })) {
                        configValue = value.get<std::vector<double>>();
                    } else {
                        configValue = value.get<std::vector<std::string>>();
                    }
                } else {
                    configValue = std::vector<double>{};
                }
            } else {
                spdlog::warn("JSON value for key '{}' has unsupported type", key);
                continue;
            }
            
            set(key, configValue, ConfigSource::File, true);
        }
        
        spdlog::info("Loaded configuration from JSON with {} entries", pImpl->entries.size());
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to load JSON configuration: {}", e.what());
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                success &= setFromString(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            success &= setFromString(key, value, ConfigSource::CommandLine);
        }
    }
    
    return success;
}

bool Config::loadFromEnv(const std::string& prefix) {
    bool success = true;
    
    // Get all environment variables
    // Note: This is a placeholder - actual implementation would need platform-specific code
    // For now, we'll just demonstrate the concept
    
    spdlog::info("Environment variable loading is not yet implemented");
    return true;  // Return true for now to avoid breaking
}

bool Config::saveToFile(const std::string& filepath) const {
    if (isJsonFile(filepath)) {
        return saveJsonToFile(filepath);
    } else {
        return saveSimpleToFile(filepath);
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto startTime = std::chrono::high_resolution_clock::now();
    pImpl->accessCount++;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
    pImpl->totalAccessTime += duration.count();
    
    if (it == pImpl->entries.end()) {
        spdlog::debug("Config key '{}' not found", key);
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        spdlog::warn("Config key '{}' has wrong type for requested value type", key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        spdlog::debug("Config key '{}' not found, using default value", key);
    }
    return val.value_or(defaultValue);
}

template<typename T>
T Config::getRequired(const std::string& key) const {
    auto val = get<T>(key);
    if (!val.has_value()) {
        throw std::runtime_error("Required configuration key '" + key + "' is missing");
    }
    
    auto validation = validate(key);
    if (!validation.isValid) {
        throw std::runtime_error("Configuration key '" + key + "' validation failed: " + validation.errorMessage);
    }
    
    return val.value();
}

void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source, bool skipValidation) {
    // Apply validation if provided and not skipped
    auto schemaIt = pImpl->schema.find(key);
    if (schemaIt != pImpl->schema.end() && !skipValidation) {
        ValidationResult validation = schemaIt->second.validator ? 
            schemaIt->second.validator(value) : ValidationResult::success();
        
        if (!validation.isValid) {
            spdlog::warn("Configuration validation failed for key '{}': {}", key, validation.errorMessage);
            return;
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        spdlog::debug("Updated configuration key '{}' (source: {})", key, static_cast<int>(source));
    } else {
        ConfigEntry entry(key, value, source);
        
        // Apply schema description if available
        if (schemaIt != pImpl->schema.end()) {
            entry.description = schemaIt->second.description;
        }
        
        pImpl->entries.push_back(std::move(entry));
        spdlog::debug("Added new configuration key '{}' (source: {})", key, static_cast<int>(source));
    }
}

void Config::set(const std::string& key, const std::string& value, 
                 ConfigSource source, bool skipValidation) {
    set(key, ConfigValue(value), source, skipValidation);
}

void Config::set(const std::string& key, int value, 
                 ConfigSource source, bool skipValidation) {
    set(key, ConfigValue(value), source, skipValidation);
}

void Config::set(const std::string& key, double value, 
                 ConfigSource source, bool skipValidation) {
    set(key, ConfigValue(value), source, skipValidation);
}

void Config::set(const std::string& key, bool value, 
                 ConfigSource source, bool skipValidation) {
    set(key, ConfigValue(value), source, skipValidation);
}

ValidationResult Config::validate(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return ValidationResult::error("Configuration key not found");
    }
    
    auto schemaIt = pImpl->schema.find(key);
    if (schemaIt != pImpl->schema.end() && schemaIt->second.validator) {
        return schemaIt->second.validator(it->value);
    }
    
    return ValidationResult::success();
}

bool Config::isValid(const std::string& key) const {
    return validate(key).isValid;
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

template<typename T>
bool Config::hasType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return std::holds_alternative<T>(it->value);
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    spdlog::debug("Removed configuration key '{}'", key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeys(const std::string& category) const {
    std::vector<std::string> keys;
    std::string lowerCategory = toLower(category);
    
    for (const auto& entry : pImpl->entries) {
        if (extractCategory(entry.key) == lowerCategory) {
            keys.push_back(entry.key);
        }
    }
    
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    spdlog::info("Cleared all configuration entries");
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary:";
    oss << "  Total entries: " << pImpl->entries.size();
    oss << "  Access count: " << pImpl->accessCount;
    
    if (pImpl->totalAccessTime > 0) {
        double avgAccessTime = static_cast<double>(pImpl->totalAccessTime) / pImpl->accessCount / 1000000.0;
        oss << "  Average access time: " << avgAccessTime << " ms";
    }
    
    oss << "\n\nEntries:";
    for (const auto& entry : pImpl->entries) {
        oss << "\n  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (source: " << static_cast<int>(entry.source) << ")";
        if (!entry.description.empty()) {
            oss << " - " << entry.description;
        }
    }
    
    return oss.str();
}

std::string Config::stats() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:";
    oss << "  Total entries: " << pImpl->entries.size();
    oss << "  Total access count: " << pImpl->accessCount;
    
    if (pImpl->totalAccessTime > 0 && pImpl->accessCount > 0) {
        double avgAccessTime = static_cast<double>(pImpl->totalAccessTime) / pImpl->accessCount / 1000000.0;
        oss << "  Average access time: " << avgAccessTime << " ms";
        oss << "  Min access time: " << (pImpl->totalAccessTime / pImpl->accessCount) / 1000000.0 << " ms";
    }
    
    return oss.str();
}

void Config::initializeWithSchema(const std::vector<ConfigSchema>& schema) {
    pImpl->schema.clear();
    
    for (const auto& schemaEntry : schema) {
        pImpl->schema[schemaEntry.key] = schemaEntry;
        
        // Check if this key already has a value
        if (!has(schemaEntry.key)) {
            // Set default value if not already set
            set(schemaEntry.key, schemaEntry.defaultValue, 
                schemaEntry.defaultSource, true);
        }
    }
    
    spdlog::info("Initialized configuration with {} schema entries", schema.size());
}

void Config::merge(const Config& other) {
    for (const auto& entry : other.pImpl->entries) {
        if (!has(entry.key) || entry.source == ConfigSource::Runtime) {
            set(entry.key, entry.value, entry.source, true);
        }
    }
    
    spdlog::info("Merged configuration with {} entries from other config", other.pImpl->entries.size());
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ConfigSource::Default;
    }
    
    return it->source;
}

std::vector<std::pair<std::string, ConfigSource>> Config::getSourceMap() const {
    std::vector<std::pair<std::string, ConfigSource>> result;
    result.reserve(pImpl->entries.size());
    
    for (const auto& entry : pImpl->entries) {
        result.emplace_back(entry.key, entry.source);
    }
    
    return result;
}

std::string Config::toEnvVarName(const std::string& key) {
    std::string result = key;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    // Replace dots and dashes with underscores
    std::replace(result.begin(), result.end(), '.', '_');
    std::replace(result.begin(), result.end(), '-', '_');
    return "NLM_" + result;
}

bool Config::fromEnvVar(const std::string& envVar, ConfigValue& value) {
    // This is a placeholder implementation
    // In a real implementation, this would read from the actual environment
    spdlog::info("Environment variable loading is not yet implemented for '{}'", envVar);
    return false;
}

bool Config::isJsonFile(const std::string& filepath) {
    return filepath.size() >= 5 && 
           (filepath.substr(filepath.size() - 5) == ".json" ||
            filepath.substr(filepath.size() - 4) == ".JSON");
}

bool Config::isYamlFile(const std::string& filepath) {
    return filepath.size() >= 4 && 
           (filepath.substr(filepath.size() - 4) == ".yaml" ||
            filepath.substr(filepath.size() - 5) == ".yml" ||
            filepath.substr(filepath.size() - 4) == ".YAML" ||
            filepath.substr(filepath.size() - 5) == ".YML");
}

bool Config::isConfigFile(const std::string& filepath) {
    return isJsonFile(filepath) || isYamlFile(filepath);
}

bool Config::loadJsonFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            spdlog::error("Cannot open JSON configuration file: {}", filepath);
            return false;
        }
        
        nlohmann::json jsonData;
        file >> jsonData;
        
        return loadFromJson(jsonData);
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to load JSON configuration file '{}': {}", filepath, e.what());
        return false;
    }
}

bool Config::loadYamlFromFile(const std::string& filepath) {
    // Placeholder for YAML loading
    spdlog::warn("YAML file loading is not yet implemented, treating as simple config file");
    return loadSimpleFromFile(filepath);
}

bool Config::loadSimpleFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        spdlog::error("Cannot open configuration file: {}", filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    int validLines = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
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
            
            if (setFromString(key, value, ConfigSource::File)) {
                validLines++;
            } else {
                spdlog::warn("Failed to parse configuration line {} in {}", lineNumber, filepath);
            }
        } else {
            spdlog::warn("Invalid configuration line {} in {}: {}", lineNumber, filepath, line);
        }
    }
    
    spdlog::info("Loaded {} valid configuration lines from {}", validLines, filepath);
    return true;
}

bool Config::saveJsonToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            spdlog::error("Cannot open JSON configuration file for writing: {}", filepath);
            return false;
        }
        
        nlohmann::json jsonData;
        
        for (const auto& entry : pImpl->entries) {
            std::visit([&jsonData, &entry](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, double>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    jsonData[entry.key] = arg;
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    jsonData[entry.key] = arg;
                }
            }, entry.value);
        }
        
        file << jsonData.dump(2);
        spdlog::info("Saved {} configuration entries to JSON file {}", pImpl->entries.size(), filepath);
        return true;
        
    } catch (const std::exception& e) {
        spdlog::error("Failed to save JSON configuration file '{}': {}", filepath, e.what());
        return false;
    }
}

bool Config::saveSimpleToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        spdlog::error("Cannot open configuration file for writing: {}", filepath);
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
    
    spdlog::info("Saved {} configuration entries to simple file {}", pImpl->entries.size(), filepath);
    return true;
}

bool Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Try to infer type from string
    bool isNumber = true;
    bool isFloat = false;
    for (char c : valueStr) {
        if (!(c >= '0' && c <= '9') && c != '-' && c != '.') {
            isNumber = false;
            break;
        }
        if (c == '.') isFloat = true;
    }
    
    if (isNumber) {
        if (isFloat) {
            try {
                double val = std::stod(valueStr);
                set(key, val, source, true);
                return true;
            } catch (...) {}
        } else {
            try {
                int val = std::stoi(valueStr);
                set(key, val, source, true);
                return true;
            } catch (...) {}
        }
    }
    
    // Check for boolean
    if (valueStr == "true" || valueStr == "false") {
        bool val = (valueStr == "true");
        set(key, val, source, true);
        return true;
    }
    
    // Default to string
    set(key, valueStr, source, true);
    return true;
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

std::string Config::extractCategory(const std::string& key) {
    size_t dotPos = key.find('.');
    if (dotPos != std::string::npos) {
        return toLower(key.substr(0, dotPos));
    }
    return "general";
}

ValidationResult Config::validateValue(const ConfigValue& value, 
                                       std::function<ValidationResult(const ConfigValue&)> validator) const {
    if (validator) {
        return validator(value);
    }
    return ValidationResult::success();
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
