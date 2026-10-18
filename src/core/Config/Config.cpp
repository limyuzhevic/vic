#include "Config.hpp"
#include "Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <cassert>
#include <numeric>
#include <regex>
#include <iomanip>
#include <random>
#include <unordered_map>
#include <cmath>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Add includes and namespace
#include "Logger.hpp"
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schemas;
    std::vector<BackupInfo> backups;
    size_t maxBackups;
    bool autoBackupEnabled;
    size_t backupRetentionDays;
    bool strictValidation;
    std::string lastError;
    std::unordered_map<std::string, size_t> typeHashes;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with some default schema
    addDefaultSchemas();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool autoBackup) {
    if (!validateFilePath(filepath)) {
        throw ConfigFileException("Invalid file path: " + filepath);
    }
    
    std::string content;
    if (!safeRead(filepath, content)) {
        return false;
    }
    
    // Create backup if requested and enabled
    if (autoBackup && autoBackupEnabled) {
        if (!autoBackup(filepath)) {
            NLM_LOG_WARNING("Failed to create automatic backup for: " + filepath);
        }
    }
    
    // Clear existing config
    clear();
    
    // Try JSON parsing first
    if (content.find('{') != std::string::npos) {
        return loadFromJSON(content);
    }
    
    // Fall back to key=value format
    std::istringstream stream(content);
    std::string line;
    size_t lineNumber = 0;
    size_t validLines = 0;
    
    while (std::getline(stream, line)) {
        ++lineNumber;
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
            
            try {
                set(key, value, ConfigSource::File);
                ++validLines;
            } catch (const ConfigException& e) {
                NLM_LOG_WARNING("Config error at line " + std::to_string(lineNumber) + ": " + e.what());
                continue; // Skip invalid lines
            }
        }
    }
    
    // Log load statistics
    NLM_LOG_INFO("Loaded configuration from " + filepath + ": " + std::to_string(validLines) + " valid lines");
    
    return !pImpl->entries.empty();
}

bool Config::loadFromJSON(const std::string& jsonStr) {
    // TODO: Implement proper JSON parsing with a JSON library
    // For now, return false to indicate JSON parsing not implemented
    NLM_LOG_WARNING("JSON configuration parsing not implemented, falling back to key=value format");
    return false;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc < 1) {
        return false;
    }
    
    bool success = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                try {
                    set(key, value, ConfigSource::CommandLine);
                    success = true;
                } catch (const ConfigException& e) {
                    NLM_LOG_ERROR("Failed to set config from command line argument --" + key + "=" + value + ": " + e.what());
                }
            }
        }
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            try {
                set(key, value, ConfigSource::CommandLine);
                success = true;
            } catch (const ConfigException& e) {
                NLM_LOG_ERROR("Failed to set config from command line argument -" + key + ": " + e.what());
            }
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath, bool createBackup) const {
    if (!validateFilePath(filepath)) {
        throw ConfigFileException("Invalid file path: " + filepath);
    }
    
    if (createBackup && autoBackupEnabled) {
        auto backupPath = filepath + ".backup_" + currentTimestamp();
        if (!saveToFile(backupPath, false)) {
            NLM_LOG_ERROR("Failed to create backup before saving: " + filepath);
            return false;
        }
        pImpl->backups.push_back(BackupInfo(backupPath));
    }
    
    if (!safeWrite(filepath, summary())) {
        NLM_LOG_ERROR("Failed to save configuration to file: " + filepath);
        return false;
    }
    
    NLM_LOG_INFO("Configuration saved to: " + filepath);
    return true;
}

void Config::addDefaultSchemas() {
    // Add common NLM configuration schemas
    ConfigSchema neuronCountSchema;
    neuronCountSchema.key = "neuron_count";
    neuronCountSchema.type = "int";
    neuronCountSchema.description = "Total number of neurons in the brain";
    neuronCountSchema.defaultValue = "1000";
    neuronCountSchema.required = false;
    neuronCountSchema.minValue = 1.0;
    neuronCountSchema.maxValue = 100000.0;
    addSchema(neuronCountSchema);
    
    ConfigSchema regionCountSchema;
    regionCountSchema.key = "region_count";
    regionCountSchema.type = "int";
    regionCountSchema.description = "Number of neural regions";
    regionCountSchema.defaultValue = "1";
    regionCountSchema.required = false;
    regionCountSchema.minValue = 1.0;
    regionCountSchema.maxValue = 1000.0;
    addSchema(regionCountSchema);
    
    ConfigSchema connectionProbabilitySchema;
    connectionProbabilitySchema.key = "connection_probability";
    connectionProbabilitySchema.type = "double";
    connectionProbabilitySchema.description = "Probability of synaptic connection between neurons";
    connectionProbabilitySchema.defaultValue = "0.1";
    connectionProbabilitySchema.required = false;
    connectionProbabilitySchema.minValue = 0.0;
    connectionProbabilitySchema.maxValue = 1.0;
    addSchema(connectionProbabilitySchema);
    
    ConfigSchema randomSeedSchema;
    randomSeedSchema.key = "random_seed";
    randomSeedSchema.type = "int64";
    randomSeedSchema.description = "Random seed for reproducible simulations";
    randomSeedSchema.defaultValue = "42";
    randomSeedSchema.required = false;
    randomSeedSchema.minValue = 0.0;
    addSchema(randomSeedSchema);
}

bool Config::validateSchema() const {
    bool valid = true;
    
    for (const auto& schema : pImpl->schemas) {
        if (schema.required && !has(schema.key)) {
            NLM_LOG_ERROR("Missing required configuration key: " + schema.key);
            valid = false;
        }
    }
    
    return valid;
}

ValidationResult Config::validate() const {
    ValidationResult result;
    
    // Validate all entries
    for (const auto& entry : pImpl->entries) {
        if (!entry.valid) {
            result.addError("Invalid entry for key: " + entry.key);
            continue;
        }
        
        // Check schema validation if schema exists
        auto schemaIt = std::find_if(pImpl->schemas.begin(), pImpl->schemas.end(),
            [&entry](const ConfigSchema& s) { return s.key == entry.key; });
        
        if (schemaIt != pImpl->schemas.end()) {
            const auto& schema = *schemaIt;
            
            if (schema.required && !has(entry.key)) {
                result.addError("Missing required key: " + entry.key);
            }
            
            if (schema.validationRegex != "") {
                try {
                    std::regex pattern(schema.validationRegex);
                    if (!std::regex_match(entryToString(entry), pattern)) {
                        result.addError("Key " + entry.key + " does not match validation pattern");
                    }
                } catch (const std::regex_error& e) {
                    result.addError("Invalid regex pattern for key " + entry.key + ": " + e.what());
                }
            }
            
            try {
                double val = 0.0;
                if (std::holds_alternative<int>(entry.value)) {
                    val = static_cast<double>(std::get<int>(entry.value));
                } else if (std::holds_alternative<int64_t>(entry.value)) {
                    val = static_cast<double>(std::get<int64_t>(entry.value));
                } else if (std::holds_alternative<double>(entry.value)) {
                    val = std::get<double>(entry.value);
                } else if (std::holds_alternative<bool>(entry.value)) {
                    val = std::get<bool>(entry.value) ? 1.0 : 0.0;
                }
                
                if (val < schema.minValue || val > schema.maxValue) {
                    result.addError("Key " + entry.key + " value " + entryToString(entry) + 
                                  " is outside valid range [" + std::to_string(schema.minValue) + 
                                  ", " + std::to_string(schema.maxValue) + "]");
                }
            } catch (...) {
                result.addError("Failed to validate value for key: " + entry.key);
            }
        }
    }
    
    return result;
}

std::string Config::entryToString(const ConfigEntry& entry) const {
    std::ostringstream oss;
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
    return oss.str();
}

bool Config::safeWrite(const std::string& filepath, const std::string& content) {
    try {
        // Ensure directory exists
        std::filesystem::path path(filepath);
        if (!path.parent_path().empty()) {
            std::filesystem::create_directories(path.parent_path());
        }
        
        std::ofstream file(filepath, std::ios::trunc);
        if (!file.is_open()) {
            pImpl->lastError = "Failed to open file for writing: " + filepath;
            return false;
        }
        
        // Write content with error checking
        file.write(content.c_str(), static_cast<std::streamsize>(content.size()));
        if (!file.good()) {
            pImpl->lastError = "Failed to write to file: " + filepath;
            return false;
        }
        
        file.flush();
        
        // Try to sync if possible
        if (file.rdbuf()->pubsync() != 0) {
            pImpl->lastError = "Failed to sync file: " + filepath;
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        pImpl->lastError = "Exception writing to file " + filepath + ": " + e.what();
        NLM_LOG_ERROR(pImpl->lastError);
        return false;
    }
}

bool Config::safeRead(const std::string& filepath, std::string& content) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            pImpl->lastError = "Failed to open file for reading: " + filepath;
            return false;
        }
        
        // Get file size
        file.seekg(0, std::ios::end);
        size_t fileSize = static_cast<size_t>(file.tellg());
        file.seekg(0, std::ios::beg);
        
        // Limit file size (prevent reading huge files)
        const size_t maxFileSize = 10 * 1024 * 1024; // 10MB
        if (fileSize > maxFileSize) {
            pImpl->lastError = "File too large: " + filepath + " (" + std::to_string(fileSize) + " bytes)";
            return false;
        }
        
        content.reserve(fileSize + 1);
        content.assign((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        
        if (content.empty()) {
            NLM_LOG_WARNING("Empty configuration file: " + filepath);
        }
        
        return true;
    } catch (const std::exception& e) {
        pImpl->lastError = "Exception reading from file " + filepath + ": " + e.what();
        NLM_LOG_ERROR(pImpl->lastError);
        return false;
    }
}

bool Config::validateFilePath(const std::string& filepath) {
    if (filepath.empty()) {
        pImpl->lastError = "Empty file path";
        return false;
    }
    
    try {
        std::filesystem::path path(filepath);
        
        // Check for path traversal attacks
        std::string normalized = std::filesystem::weakly_normal(path).string();
        if (normalized.find("..") != std::string::npos) {
            pImpl->lastError = "Path traversal detected in: " + filepath;
            return false;
        }
        
        return true;
    } catch (const std::exception& e) {
        pImpl->lastError = "Invalid file path: " + filepath + ": " + e.what();
        return false;
    }
}

bool Config::autoBackup(const std::string& filepath) {
    try {
        std::filesystem::path path(filepath);
        if (path.parent_path().empty()) {
            return false;
        }
        
        std::string backupName = "backup_" + currentTimestamp() + 
                               path.extension().string();
        std::string backupPath = path.parent_path().string() + "/" + backupName;
        
        if (saveToFile(backupPath, false)) {
            pImpl->backups.push_back(BackupInfo(backupPath));
            
            // Clean old backups
            cleanBackups();
            return true;
        }
        
        return false;
    } catch (const std::exception& e) {
        pImpl->lastError = "Failed to create auto backup for " + filepath + ": " + e.what();
        NLM_LOG_ERROR(pImpl->lastError);
        return false;
    }
}

void Config::cleanBackups() {
    auto now = std::chrono::system_clock::now();
    auto cutoff = now - std::chrono::hours(backupRetentionDays * 24);
    
    pImpl->backups.erase(
        std::remove_if(pImpl->backups.begin(), pImpl->backups.end(),
            [cutoff](const BackupInfo& backup) {
                return backup.created < cutoff;
            }),
        pImpl->backups.end()
    );
    
    if (pImpl->backups.size() > static_cast<size_t>(maxBackups)) {
        std::sort(pImpl->backups.begin(), pImpl->backups.end(),
            [](const BackupInfo& a, const BackupInfo& b) { return a.created < b.created; });
        
        pImpl->backups.erase(
            pImpl->backups.begin(),
            pImpl->backups.begin() + (pImpl->backups.size() - maxBackups)
        );
    }
}

std::vector<BackupInfo> Config::listBackups(const std::string& pattern) {
    std::vector<BackupInfo> result;
    
    try {
        std::filesystem::path dir(".");
        for (const auto& entry : std::filesystem::directory_iterator(dir)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.find("backup_") == 0) {
                    BackupInfo backup(filename);
                    
                    // Calculate simple checksum
                    size_t checksum = calculateChecksum(filename);
                    backup.checksum = checksum;
                    result.push_back(backup);
                }
            }
        }
    } catch (...) {
        // Ignore filesystem errors
    }
    
    return result;
}

bool Config::restoreFromBackup(const std::string& backupPath) {
    if (!validateFilePath(backupPath)) {
        return false;
    }
    
    std::string content;
    if (!safeRead(backupPath, content)) {
        return false;
    }
    
    // Create a temporary config and load backup content
    Config tempConfig;
    
    // For now, just return false as backup restoration is complex
    // This would require proper JSON parsing and backup format support
    NLM_LOG_WARNING("Backup restoration not fully implemented");
    
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

std::string Config::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string Config::currentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S");
    return ss.str();
}

size_t Config::calculateChecksum(const std::string& content) const {
    size_t checksum = 0;
    for (char c : content) {
        checksum = (checksum << 5) + checksum + static_cast<size_t>(c);
    }
    return checksum;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    ConfigEntry entry(key, value, source);
    
    if (!validateEntry(entry)) {
        throw ConfigValidationException("Invalid configuration value for key: " + key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->timestamp = std::chrono::system_clock::now();
        it->valid = true;
    } else {
        pImpl->entries.push_back(std::move(entry));
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
        [&key](const ConfigEntry& e) { return e.key == key && e.valid; });
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
        if (entry.valid) {
            keys.push_back(entry.key);
        }
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
        if (!entry.valid) continue;
        
        oss << "  " << entry.key << " = [";
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
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

bool Config::validateEntry(const ConfigEntry& entry) const {
    if (entry.key.empty() || !entry.valid) {
        return false;
    }
    
    const auto& schemaIt = std::find_if(pImpl->schemas.begin(), pImpl->schemas.end(),
        [&entry](const ConfigSchema& s) { return s.key == entry.key; });
    
    if (schemaIt != pImpl->schemas.end()) {
        const auto& schema = *schemaIt;
        
        bool typeMatches = false;
        if (schema.type == "int" && std::holds_alternative<int>(entry.value)) typeMatches = true;
        else if (schema.type == "int64" && std::holds_alternative<int64_t>(entry.value)) typeMatches = true;
        else if (schema.type == "double" && std::holds_alternative<double>(entry.value)) typeMatches = true;
        else if (schema.type == "bool" && std::holds_alternative<bool>(entry.value)) typeMatches = true;
        else if (schema.type == "string" && std::holds_alternative<std::string>(entry.value)) typeMatches = true;
        else if (schema.type == "vec_int" && std::holds_alternative<std::vector<int>>(entry.value)) typeMatches = true;
        else if (schema.type == "vec_double" && std::holds_alternative<std::vector<double>>(entry.value)) typeMatches = true;
        else if (schema.type == "vec_string" && std::holds_alternative<std::vector<std::string>>(entry.value)) typeMatches = true;
        
        if (!typeMatches) {
            NLM_LOG_WARNING("Type mismatch for key " + entry.key + ": expected " + schema.type);
            return false;
        }
    }
    
    return true;
}

bool Config::validateValueType(const ConfigEntry& entry) const {
    return true;
}

std::string Config::getLastError() const {
    return pImpl->lastError;
}

bool Config::migrateFromLegacy(const std::string& legacyPath) {
    if (!validateFilePath(legacyPath)) {
        return false;
    }
    
    std::string content;
    if (!safeRead(legacyPath, content)) {
        return false;
    }
    
    // Parse legacy format and migrate to new format
    // This is a placeholder implementation
    NLM_LOG_WARNING("Legacy configuration migration not implemented");
    
    return false;
}

// Template method implementations
namespace nlm {
    template<typename T>
    std::optional<T> Config::get(const std::string& key) const {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end() || !it->valid) {
            return std::nullopt;
        }
        
        try {
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    template T Config::getOr<T>(const std::string& key, const T& defaultValue) const {
        auto val = get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    }
}

// Explicit template instantiations
namespace nlm {
    extern template std::optional<int> Config::get<int>(const std::string&) const;
    extern template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
    extern template std::optional<double> Config::get<double>(const std::string&) const;
    extern template std::optional<bool> Config::get<bool>(const std::string&) const;
    extern template std::optional<std::string> Config::get<std::string>(const std::string&) const;
    
    extern template int Config::getOr<int>(const std::string&, const int&) const;
    extern template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
    extern template double Config::getOr<double>(const std::string&, const double&) const;
    extern template bool Config::getOr<bool>(const std::string&, const bool&) const;
    extern template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
}

} // namespace nlm
