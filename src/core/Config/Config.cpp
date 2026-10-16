#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <regex>
#include <stdexcept>
#include <ctime>
#include <numeric>
#include "constants.h"

using json = nlohmann::json;

namespace nlm {

// ==================== CONFIG IMPLEMENTATION ====================

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string fileFormat;
    std::string lastLoadedFile;
    bool autoSaveEnabled;
    std::string backupDirectory;
    size_t autoSaveInterval;
    
    Impl() : fileFormat(ConfigConstants::DEFAULT_FILE_FORMAT), 
             lastLoadedFile(""), autoSaveEnabled(false), backupDirectory(""),
             autoSaveInterval(ConfigSystemConstants::AUTO_SAVE_INTERVAL) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default values for common configuration keys
    pImpl->fileFormat = ConfigConstants::DEFAULT_FILE_FORMAT;
    pImpl->autoSaveEnabled = false;
    pImpl->backupDirectory = ConfigSystemConstants::AUTO_SAVE_DIR;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFile(filepath, pImpl->fileFormat);
}

bool Config::loadFromFile(const std::string& filepath, const std::string& format) {
    try {
        // Validate file path
        if (filepath.empty()) {
            NLM_LOG_ERROR("Config file path cannot be empty");
            return false;
        }
        
        std::filesystem::path filePath(filepath);
        if (!std::filesystem::exists(filePath)) {
            NLM_LOG_WARN("Config file not found: " + filepath);
            return false;
        }
        
        if (!std::filesystem::is_regular_file(filePath)) {
            NLM_LOG_ERROR("Config path is not a file: " + filepath);
            return false;
        }
        
        // Check file size
        auto fileSize = std::filesystem::file_size(filePath);
        if (fileSize > ConfigSystemConstants::MAX_CONFIG_SIZE) {
            NLM_LOG_ERROR("Config file too large: " + std::to_string(fileSize) + " bytes");
            return false;
        }
        
        pImpl->lastLoadedFile = filepath;
        
        // Parse based on format
        bool success = false;
        if (format == ConfigConstants::JSON_FILE_FORMAT) {
            success = loadJsonFromFile(filepath);
        } else if (format == ConfigConstants::YAML_FILE_FORMAT) {
            success = loadYamlFromFile(filepath);
        } else {
            // Default to key=value format
            success = loadKeyValueFromFile(filepath);
        }
        
        if (success) {
            pImpl->fileFormat = format;
            NLM_LOG_INFO("Successfully loaded configuration from: " + filepath);
        } else {
            NLM_LOG_ERROR("Failed to load configuration from: " + filepath);
        }
        
        return success;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading config file " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (...) {
        NLM_LOG_ERROR("Unknown exception loading config file: " + filepath);
        return false;
    }
}

bool Config::loadJsonFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open JSON config file: " + filepath);
            return false;
        }
        
        json configJson;
        file >> configJson;
        
        // Clear existing entries
        clear();
        
        // Process JSON object
        if (!configJson.is_object()) {
            NLM_LOG_ERROR("JSON config must be an object");
            return false;
        }
        
        for (auto it = configJson.begin(); it != configJson.end(); ++it) {
            std::string key = it.key();
            const json& value = it.value();
            
            // Convert JSON value to ConfigValue
            ConfigValue configValue;
            std::string error = jsonToConfigValue(value, configValue);
            
            if (!error.empty()) {
                NLM_LOG_WARN("Skipping invalid config entry '" + key + "': " + error);
                continue;
            }
            
            // Add to config with file source
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
        
    } catch (const json::parse_error& e) {
        NLM_LOG_ERROR("JSON parse error in " + filepath + ": " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading JSON config " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadYamlFromFile(const std::string& filepath) {
    try {
        // YAML parsing would require a YAML library
        // For now, treat YAML files as key=value format with simple parsing
        return loadKeyValueFromFile(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading YAML config " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadKeyValueFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    size_t validEntries = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Check line length
        if (line.length() > ConfigSystemConstants::MAX_LINE_LENGTH) {
            NLM_LOG_WARN("Line " + std::to_string(lineNumber) + " too long, skipping");
            continue;
        }
        
        // Parse key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_WARN("Line " + std::to_string(lineNumber) + ": Empty key, skipping");
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Parse value based on content
            set(key, value, ConfigSource::File);
            ++validEntries;
        } else {
            NLM_LOG_WARN("Line " + std::to_string(lineNumber) + ": Invalid format '=" + line + "', skipping");
        }
    }
    
    NLM_LOG_INFO("Loaded " + std::to_string(validEntries) + " configuration entries from " + 
                filepath + " (" + std::to_string(lineNumber) + " lines)");
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    return saveToFile(filepath, pImpl->fileFormat);
}

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    try {
        std::filesystem::path filePath(filepath);
        std::filesystem::path dirPath = filePath.parent_path();
        
        // Create directory if it doesn't exist
        if (!dirPath.empty() && !std::filesystem::exists(dirPath)) {
            if (!std::filesystem::create_directories(dirPath)) {
                NLM_LOG_ERROR("Cannot create config directory: " + dirPath.string());
                return false;
            }
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open config file for writing: " + filepath);
            return false;
        }
        
        // Write based on format
        if (format == ConfigConstants::JSON_FILE_FORMAT) {
            saveAsJson(file);
        } else if (format == ConfigConstants::YAML_FILE_FORMAT) {
            saveAsYaml(file);  // Basic YAML support
        } else {
            // Default to key=value format
            saveAsKeyValue(file);
        }
        
        file.flush();
        if (!file.good()) {
            NLM_LOG_ERROR("Error writing to config file: " + filepath);
            return false;
        }
        
        NLM_LOG_INFO("Configuration saved to: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception saving config file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

void Config::saveAsJson(std::ostream& output) const {
    json configJson;
    
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        configJson[key] = configValueToJson(entry.value);
    }
    
    output << configJson.dump(2);  // Pretty print with 2-space indentation
}

void Config::saveAsYaml(std::ostream& output) const {
    json configJson;
    
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        configJson[key] = configValueToJson(entry.value);
    }
    
    // Basic YAML output (simplified)
    output << "# NLM Configuration" << std::endl;
    output << "# Format: YAML" << std::endl;
    output << "# Generated: " << getCurrentTimestamp() << std::endl;
    output << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        output << "# " << entry.key << std::endl;
        
        std::visit([&output](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                output << "  \"" << arg << "\"" << std::endl;
            } else {
                output << "  " << arg << std::endl;
            }
        }, entry.value);
        output << std::endl;
    }
}

void Config::saveAsKeyValue(std::ostream& output) const {
    for (const auto& entry : pImpl->entries) {
        output << "# " << entry.description << std::endl;
        output << entry.key << " = ";
        
        std::visit([&output](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                output << "\"" << arg << "\"";
            } else {
                output << arg;
            }
        }, entry.value);
        output << std::endl << std::endl;
    }
}

std::string Config::getCurrentTimestamp() const {
    std::time_t now = std::time(nullptr);
    char buf[100];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buf);
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
        NLM_LOG_WARN("Type mismatch for config key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        NLM_LOG_ERROR("Cannot set empty config key");
        return;
    }
    
    // Validate value based on key type (basic validation)
    std::string error = validateConfigValue(key, value);
    if (!error.empty()) {
        NLM_LOG_WARN("Invalid value for key '" + key + "': " + error);
        return;
    }
    
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setWithConstantFallback(const std::string& key, const std::string& constantName, const std::string& defaultValue) {
    if (has(key)) {
        // Use existing value
        return;
    }
    
    // Check if it's a known constant
    if (isBuiltInConstant(constantName)) {
        set(key, defaultValue, ConfigSource::Default);
        NLM_LOG_INFO("Using constant fallback for key: " + key + " (" + constantName + ")");
    }
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
    oss << "Configuration (" << pImpl->entries.size() << " entries, " << pImpl->fileFormat << "):" << std::endl;
    oss << "Last loaded: " << pImpl->lastLoadedFile << std::endl;
    oss << "Auto-save: " << (pImpl->autoSaveEnabled ? "enabled" : "disabled") << std::endl;
    oss << "Backup directory: " << pImpl->backupDirectory << std::endl;
    oss << std::endl;
    
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
        oss << "] (" << static_cast<int>(entry.source) << ") " << entry.description << std::endl;
    }
    return oss.str();
}

std::string Config::getMetadata() const {
    std::ostringstream oss;
    oss << "=== Configuration Metadata ===" << std::endl;
    oss << "File format: " << pImpl->fileFormat << std::endl;
    oss << "Last loaded: " << pImpl->lastLoadedFile << std::endl;
    oss << "Auto-save enabled: " << (pImpl->autoSaveEnabled ? "yes" : "no") << std::endl;
    oss << "Auto-save interval: " << pImpl->autoSaveInterval << " seconds" << std::endl;
    oss << "Backup directory: " << pImpl->backupDirectory << std::endl;
    oss << std::endl;
    oss << "=== Entry Statistics ===" << std::endl;
    oss << "Total entries: " << pImpl->entries.size() << std::endl;
    
    int defaultCount = 0, fileCount = 0, commandLineCount = 0, runtimeCount = 0;
    for (const auto& entry : pImpl->entries) {
        switch (entry.source) {
            case ConfigSource::Default: defaultCount++; break;
            case ConfigSource::File: fileCount++; break;
            case ConfigSource::CommandLine: commandLineCount++; break;
            case ConfigSource::Runtime: runtimeCount++; break;
        }
    }
    
    oss << "Default: " << defaultCount << std::endl;
    oss << "File: " << fileCount << std::endl;
    oss << "Command Line: " << commandLineCount << std::endl;
    oss << "Runtime: " << runtimeCount << std::endl;
    
    return oss.str();
}

void Config::setFileFormat(FileFormat format) {
    switch (format) {
        case FileFormat::KeyValue:
            pImpl->fileFormat = ConfigConstants::DEFAULT_FILE_FORMAT;
            break;
        case FileFormat::JSON:
            pImpl->fileFormat = ConfigConstants::JSON_FILE_FORMAT;
            break;
        case FileFormat::YAML:
            pImpl->fileFormat = ConfigConstants::YAML_FILE_FORMAT;
            break;
    }
}

Config::FileFormat Config::getFileFormat() const {
    if (pImpl->fileFormat == ConfigConstants::JSON_FILE_FORMAT) {
        return FileFormat::JSON;
    } else if (pImpl->fileFormat == ConfigConstants::YAML_FILE_FORMAT) {
        return FileFormat::YAML;
    } else {
        return FileFormat::KeyValue;
    }
}

bool Config::createBackup(const std::string& originalFile, const std::string& backupFile) const {
    std::string actualBackupFile = backupFile.empty() ? 
        originalFile + ConfigSystemConstants::DEFAULT_BACKUP_EXTENSION : backupFile;
    
    try {
        std::filesystem::path origPath(originalFile);
        std::filesystem::path backupPath(actualBackupFile);
        
        if (!std::filesystem::exists(origPath)) {
            NLM_LOG_WARN("Original file does not exist: " + originalFile);
            return false;
        }
        
        if (std::filesystem::exists(backupPath)) {
            // Remove old backup if it exists
            std::filesystem::remove(backupPath);
        }
        
        std::filesystem::copy_file(origPath, backupPath, std::filesystem::copy_options::overwrite_existing);
        NLM_LOG_INFO("Created backup: " + actualBackupFile);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error creating backup: " + std::string(e.what()));
        return false;
    }
}

bool Config::restoreFromBackup(const std::string& backupFile, const std::string& targetFile) const {
    std::string actualTargetFile = targetFile.empty() ? 
        backupFile.substr(0, backupFile.find_last_of('.')) : targetFile;
    
    try {
        std::filesystem::path backupPath(backupFile);
        std::filesystem::path targetPath(actualTargetFile);
        
        if (!std::filesystem::exists(backupPath)) {
            NLM_LOG_ERROR("Backup file does not exist: " + backupFile);
            return false;
        }
        
        if (std::filesystem::exists(targetPath)) {
            std::filesystem::remove(targetPath);
        }
        
        std::filesystem::copy_file(backupPath, targetPath, std::filesystem::copy_options::overwrite_existing);
        NLM_LOG_INFO("Restored from backup: " + backupFile + " -> " + actualTargetFile);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error restoring from backup: " + std::string(e.what()));
        return false;
    }
}

void Config::enableAutoSave(size_t intervalSeconds) {
    pImpl->autoSaveEnabled = true;
    pImpl->autoSaveInterval = intervalSeconds;
    NLM_LOG_INFO("Auto-save enabled with interval: " + std::to_string(intervalSeconds) + " seconds");
}

void Config::disableAutoSave() {
    pImpl->autoSaveEnabled = false;
    NLM_LOG_INFO("Auto-save disabled");
}

bool Config::isAutoSaveEnabled() const {
    return pImpl->autoSaveEnabled;
}

std::string Config::getAutoSavePath() const {
    if (pImpl->autoSaveEnabled) {
        return ConfigSystemConstants::AUTO_SAVE_DIR;
    }
    return "";
}

bool Config::validateConfiguration() const {
    std::vector<std::string> errors = getValidationErrors();
    if (!errors.empty()) {
        NLM_LOG_ERROR("Configuration validation failed:");
        for (const auto& error : errors) {
            NLM_LOG_ERROR("  " + error);
        }
        return false;
    }
    
    NLM_LOG_INFO("Configuration validation passed");
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        if (!validateKey(entry.key)) {
            errors.push_back("Invalid key format: " + entry.key);
        }
        
        if (!validateValue(entry.value)) {
            errors.push_back("Invalid value for key: " + entry.key);
        }
    }
    
    // Check for required keys
    std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count"
    };
    
    for (const auto& requiredKey : requiredKeys) {
        if (!has(requiredKey)) {
            errors.push_back("Missing required configuration key: " + requiredKey);
        }
    }
    
    return errors;
}
bool Config::exportToFile(const std::string& filepath, const std::string& format) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot open export file: " + filepath);
            return false;
        }
        
        return exportToStream(file, format);
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception exporting configuration: " + std::string(e.what()));
        return false;
    }
}

bool Config::exportToStream(std::ostream& output, const std::string& format) const {
    if (format == ConfigConstants::JSON_FILE_FORMAT) {
        saveAsJson(output);
    } else if (format == ConfigConstants::YAML_FILE_FORMAT) {
        saveAsYaml(output);
    } else {
        saveAsKeyValue(output);
    }
    
    return true;
}

bool Config::mergeWith(const Config& otherConfig, ConfigSource source) {
    bool changed = false;
    
    for (const auto& otherEntry : otherConfig.pImpl->entries) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&otherEntry](const ConfigEntry& e) { return e.key == otherEntry.key; });
        
        if (it != pImpl->entries.end()) {
            // Key exists, update if different source or value differs
            if (it->source == ConfigSource::Default || source == ConfigSource::Default) {
                // Prefer non-default sources
                if (source != ConfigSource::Default) {
                    it->value = otherEntry.value;
                    it->source = source;
                    changed = true;
                }
            } else if (it->value != otherEntry.value) {
                it->value = otherEntry.value;
                it->source = source;
                changed = true;
            }
        } else {
            // New key, add it
            pImpl->entries.push_back(otherEntry);
            pImpl->entries.back().source = source;
            changed = true;
        }
    }
    
    if (changed) {
        NLM_LOG_INFO("Configuration merge completed");
    }
    
    return changed;
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getEntryCountBySource(ConfigSource source) const {
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            count++;
        }
    }
    return count;
}

std::string Config::getStatistics() const {
    std::ostringstream oss;
    oss << "=== Configuration Statistics ===" << std::endl;
    oss << "Total entries: " << pImpl->entries.size() << std::endl;
    oss << "Default entries: " << getEntryCountBySource(ConfigSource::Default) << std::endl;
    oss << "File entries: " << getEntryCountBySource(ConfigSource::File) << std::endl;
    oss << "Command line entries: " << getEntryCountBySource(ConfigSource::CommandLine) << std::endl;
    oss << "Runtime entries: " << getEntryCountBySource(ConfigSource::Runtime) << std::endl;
    oss << std::endl;
    
    // Value type distribution
    std::map<std::string, size_t> typeCounts;
    for (const auto& entry : pImpl->entries) {
        std::string typeName;
        std::visit([&typeName](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) typeName = "int";
            else if constexpr (std::is_same_v<T, int64_t>) typeName = "int64_t";
            else if constexpr (std::is_same_v<T, double>) typeName = "double";
            else if constexpr (std::is_same_v<T, bool>) typeName = "bool";
            else if constexpr (std::is_same_v<T, std::string>) typeName = "string";
            else if constexpr (std::is_same_v<T, std::vector<int>>) typeName = "std::vector<int>";
            else if constexpr (std::is_same_v<T, std::vector<double>>) typeName = "std::vector<double>";
            else if constexpr (std::is_same_v<T, std::vector<std::string>>) typeName = "std::vector<std::string>";
            else typeName = "unknown";
        }, entry.value);
        typeCounts[typeName]++;
    }
    
    oss << "Value type distribution:" << std::endl;
    for (const auto& pair : typeCounts) {
        oss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    
    return oss.str();
}

std::string Config::getConstantName(const std::string& key) {
    // Map common keys to constant names
    std::map<std::string, std::string> constantMap = {
        {"random_seed", "RANDOM_SEED"},
        {"simulation_timestep", "SIMULATION_TIMESTEP"},
        {"neuron_count", "NEURON_COUNT"},
        {"region_count", "REGION_COUNT"},
        {"connection_probability", "CONNECTION_PROBABILITY"},
        {"stdp_ltp_weight", "STDP_LTP_WEIGHT"},
        {"stdp_ltd_weight", "STDP_LTD_WEIGHT"},
        {"stdp_tau", "STDP_TAU"},
        {"synaptogenesis_rate", "SYNAPTAGENESIS_RATE"},
        {"pruning_rate", "PRUNING_RATE"},
        {"checkpoint_dir", "CHECKPOINT_DIR"},
        {"replay_interval", "REPLAY_INTERVAL"},
        {"consolidation_interval", "CONSOLIDATION_INTERVAL"}
    };
    
    auto it = constantMap.find(key);
    if (it != constantMap.end()) {
        return it->second;
    }
    
    return "";
}

bool Config::isConstantKey(const std::string& key) const {
    return getConstantName(key) != "";
}

std::string Config::jsonToConfigValue(const json& jsonValue, ConfigValue& configValue) {
    try {
        if (jsonValue.is_string()) {
            configValue = jsonValue.get<std::string>();
        } else if (jsonValue.is_number_integer()) {
            configValue = jsonValue.get<int64_t>();
        } else if (jsonValue.is_number_unsigned()) {
            configValue = jsonValue.get<uint64_t>();
        } else if (jsonValue.is_number_float()) {
            configValue = jsonValue.get<double>();
        } else if (jsonValue.is_boolean()) {
            configValue = jsonValue.get<bool>();
        } else if (jsonValue.is_null()) {
            configValue = std::string("");
        } else {
            return "Unsupported JSON value type";
        }
        return "";
    } catch (const std::exception& e) {
        return std::string("JSON conversion error: ") + e.what();
    }
}

json Config::configValueToJson(const ConfigValue& configValue) const {
    return std::visit([](auto&& arg) -> json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else {
            return json(nullptr);
        }
    }, configValue);
}

std::string Config::validateConfigValue(const std::string& key, const ConfigValue& value) const {
    // Basic validation - can be extended based on specific key patterns
    if (key.empty()) {
        return "Key cannot be empty";
    }
    
    // Check for obviously invalid values
    if (std::holds_alternative<std::string>(value)) {
        const std::string& strVal = std::get<std::string>(value);
        if (strVal.find('\0') != std::string::npos) {
            return "String contains null character";
        }
    }
    
    // Numeric bounds checking could be added here
    if (std::holds_alternative<double>(value)) {
        double numVal = std::get<double>(value);
        if (std::isnan(numVal) || std::isinf(numVal)) {
            return "Invalid numeric value";
        }
    }
    
    // Range validation based on key name
    if (key == "neuron_count") {
        if (std::holds_alternative<int64_t>(value)) {
            int64_t count = std::get<int64_t>(value);
            if (count <= 0 || count > BrainConstants::MAX_NEURONS_PER_REGION) {
                return "Neuron count must be between 1 and " + std::to_string(BrainConstants::MAX_NEURONS_PER_REGION);
            }
        }
    }
    
    return "";
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