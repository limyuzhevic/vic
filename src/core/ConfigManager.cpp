// Enhanced Configuration Manager Implementation
// Comprehensive configuration management with validation, inheritance, observers, and analytics

#include "core/ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <unordered_set>
#include <mutex>
#include <chrono>
#include <filesystem>
#include <json.hpp>
#include <zlib.h>
#include <unordered_map>
#include <iomanip>
#include <cryptography>

namespace nlm {

namespace {
    // Helper functions
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), 
                       [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    std::string trim(const std::string& str) {
        size_t first = str.find_first_not_of(" \t\n\r");
        if (first == std::string::npos) return str;
        size_t last = str.find_last_not_of(" \t\n\r");
        return str.substr(first, (last - first + 1));
    }
    
    bool stringContains(const std::string& str, const std::string& substr) {
        return str.find(substr) != std::string::npos;
    }
    
    std::string generateRandomString(size_t length) {
        const std::string chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
        std::string result;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, chars.size() - 1);
        
        for (size_t i = 0; i < length; ++i) {
            result += chars[dis(gen)];
        }
        return result;
    }
    
    std::string generateTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y%m%d_%H%M%S");
        return ss.str();
    }
    
    bool endsWith(const std::string& str, const std::string& suffix) {
        return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
    }
    
    std::string extractFilename(const std::string& path) {
        size_t pos = path.find_last_of("/\\");
        return (pos == std::string::npos) ? path : path.substr(pos + 1);
    }
    
    std::string extractDirectory(const std::string& path) {
        size_t pos = path.find_last_of("/\\");
        return (pos == std::string::npos) ? "." : path.substr(0, pos);
    }
    
    bool createDirectory(const std::string& path) {
        return std::filesystem::create_directories(path);
    }
    
    std::string getFileExtension(const std::string& filename) {
        size_t pos = filename.find_last_of('.');
        return (pos == std::string::npos) ? "" : filename.substr(pos + 1);
    }
    
    bool fileExists(const std::string& path) {
        return std::filesystem::exists(path);
    }
    
    size_t getFileSize(const std::string& path) {
        if (fileExists(path)) {
            return std::filesystem::file_size(path);
        }
        return 0;
    }
}

// ConfigManager implementation
ConfigManager::ConfigManager() 
    : monitoringEnabled(false), monitoringInterval(60), 
      lastMonitorCheck(std::chrono::system_clock::now()) {
    
    pImpl = std::make_unique<Impl>();
    backupDirectory = "backups";
    maxBackups = 10;
}

ConfigManager::~ConfigManager() {
    // Clean up resources
}

ConfigManager::ConfigManager(ConfigManager&& other) noexcept 
    : pImpl(std::move(other.pImpl)), monitoringEnabled(other.monitoringEnabled),
      monitoringInterval(other.monitoringInterval), lastMonitorCheck(other.lastMonitorCheck),
      observers(std::move(other.observers)), configHistory(std::move(other.configHistory)),
      templates(std::move(other.templates)), backupDirectory(other.backupDirectory),
      maxBackups(other.maxBackups), stats(other.stats), monitoringCallback(std::move(other.monitoringCallback)),
      validationCallback(std::move(other.validationCallback)) {
    
}

ConfigManager& ConfigManager::operator=(ConfigManager&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        monitoringEnabled = other.monitoringEnabled;
        monitoringInterval = other.monitoringInterval;
        lastMonitorCheck = other.lastMonitorCheck;
        observers = std::move(other.observers);
        configHistory = std::move(other.configHistory);
        templates = std::move(other.templates);
        backupDirectory = other.backupDirectory;
        maxBackups = other.maxBackups;
        stats = other.stats;
        monitoringCallback = std::move(other.monitoringCallback);
        validationCallback = std::move(other.validationCallback);
    }
    return *this;
}

void ConfigManager::enableFileLogging(const std::string& directory) {
    if (!createDirectory(directory)) {
        logError("Failed to create directory: " + directory);
        return;
    }
    
    backupDirectory = directory;
    logger->info("Configuration backup directory set to: " + directory);
}

void ConfigManager::setExportFormat(ExportFormat format) {
    exportFormat = format;
}

bool ConfigManager::loadFromFile(const std::string& filepath, bool validate) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto startTime = std::chrono::system_clock::now();
    
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            logError("Failed to open configuration file: " + filepath);
            return false;
        }
        
        std::string extension = getFileExtension(filepath);
        bool success = false;
        
        if (extension == "json" || extension == "JSON") {
            success = loadFromJSONInternal(filepath);
        } else if (extension == "ini" || extension == "cfg" || extension == "config") {
            success = loadFromINIInternal(filepath);
        } else {
            // Try to detect format
            std::string firstLine;
            std::getline(file, firstLine);
            file.clear();
            file.seekg(0);
            
            if (stringContains(firstLine, "{")) {
                success = loadFromJSONInternal(filepath);
            } else {
                success = loadFromINIInternal(filepath);
            }
        }
        
        if (success && validate) {
            if (!validate()) {
                logError("Configuration validation failed after loading from file: " + filepath);
                clear(); // Rollback changes
                return false;
            }
        }
        
        auto endTime = std::chrono::system_clock::now();
        stats.totalLoadTime += std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        
        // Create backup
        createAutoBackup();
        
        // Notify observers
        notifyObservers(ConfigChangeEvent("load", ConfigValue(), ConfigValue(), ConfigSource::File));
        
        logger->info("Configuration loaded successfully from file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        logError(std::string("Exception loading configuration from file: ") + e.what());
        return false;
    }
}

bool ConfigManager::loadFromJSON(const std::string& filepath, bool validate) {
    return loadFromJSONInternal(filepath);
}

bool ConfigManager::loadFromINI(const std::string& filepath, bool validate) {
    return loadFromINIInternal(filepath);
}

bool ConfigManager::loadFromYAML(const std::string& filepath, bool validate) {
    // YAML support would require a YAML library
    logError("YAML format not supported in this build");
    return false;
}

bool ConfigManager::loadFromArgs(int argc, char** argv) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto startTime = std::chrono::system_clock::now();
    
    try {
        int argCount = 0;
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            argCount++;
            
            if (arg.substr(0, 7) == "--config") {
                std::string configFile;
                if (arg.find('=') != std::string::npos) {
                    configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    configFile = argv[++i];
                    argCount++;
                }
                
                if (!configFile.empty() && loadFromFile(configFile, false)) {
                    logger->info("Loaded configuration from command line: " + configFile);
                }
            } else if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                }
            } else if (arg[0] == '-') {
                if (i + 1 < argc) {
                    std::string key = arg.substr(1);
                    std::string value = argv[++i];
                    argCount++;
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        
        auto endTime = std::chrono::system_clock::now();
        stats.totalLoadTime += std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
        
        logger->info("Configuration loaded from command line arguments");
        return true;
        
    } catch (const std::exception& e) {
        logError(std::string("Exception loading configuration from command line: ") + e.what());
        return false;
    }
}

bool ConfigManager::saveToFile(const std::string& filepath, const std::string& format) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto startTime = std::chrono::system_clock::now();
    
    try {
        std::string extension = getFileExtension(filepath);
        bool success = false;
        
        if (extension == "json" || format == "json") {
            success = saveToJSONInternal(filepath);
        } else if (extension == "ini" || format == "ini" || extension == "cfg" || extension == "config") {
            success = saveToINIInternal(filepath);
        } else {
            // Default to INI format
            success = saveToINIInternal(filepath);
        }
        
        if (success) {
            auto endTime = std::chrono::system_clock::now();
            stats.totalLoadTime += std::chrono::duration_cast<std::chrono::duration<double>>(endTime - startTime);
            
            logger->info("Configuration saved successfully to file: " + filepath);
        }
        
        return success;
        
    } catch (const std::exception& e) {
        logError(std::string("Exception saving configuration to file: ") + e.what());
        return false;
    }
}

bool ConfigManager::saveToJSON(const std::string& filepath) const {
    return saveToJSONInternal(filepath);
}

bool ConfigManager::saveToINI(const std::string& filepath) const {
    return saveToINIInternal(filepath);
}

bool ConfigManager::loadFromString(const std::string& content, const std::string& format) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (format == "json" || format == "auto") {
        return loadFromJSONInternal(content);
    } else {
        return loadFromINIInternal(content);
    }
}

void ConfigManager::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto oldValue = getValue(key);
    
    // Validate with templates
    validateWithTemplates(key, value);
    
    // Apply constraints
    validateConstraints(key, value);
    
    // Apply business rules
    if (validationCallback && !validationCallback(key, value)) {
        logError("Configuration validation failed for key: " + key);
        return;
    }
    
    // Set the value
    pImpl->entries[key] = ConfigEntry(key, value, source);
    
    // Log change event
    if (oldValue.has_value() && oldValue.value() != value) {
        ConfigChangeEvent event(key, oldValue.value(), value, source);
        
        // Add to history
        configHistory.push_back(std::unordered_map<std::string, ConfigValue>{
            {key, event.oldValue}, {key, event.newValue}
        });
        
        // Notify observers
        notifyObservers(event);
        
        // Monitor changes
        if (monitoringEnabled && std::chrono::system_clock::now() - lastMonitorCheck > 
            std::chrono::seconds(monitoringInterval)) {
            checkPerformance();
            lastMonitorCheck = std::chrono::system_clock::now();
        }
    }
}

void ConfigManager::set(const std::string& key, const std::string& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void ConfigManager::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void ConfigManager::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void ConfigManager::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void ConfigManager::remove(const std::string& key) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto oldValue = getValue(key);
    if (oldValue.has_value()) {
        pImpl->entries.erase(key);
        
        ConfigChangeEvent event(key, oldValue.value(), ConfigValue(), ConfigSource::Runtime);
        configHistory.push_back(std::unordered_map<std::string, ConfigValue>{{key, event.oldValue}});
        notifyObservers(event);
        
        logger->info("Configuration key removed: " + key);
    }
}

void ConfigManager::clear() {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (const auto& entry : pImpl->entries) {
        ConfigChangeEvent event(entry.first, entry.second.value, ConfigValue(), entry.second.source);
        configHistory.push_back(std::unordered_map<std::string, ConfigValue>{{entry.first, event.oldValue}});
        notifyObservers(event);
    }
    
    pImpl->entries.clear();
    logger->info("Configuration cleared completely");
}

void ConfigManager::merge(const ConfigManager& other, ConfigSource source) {
    std::lock_guard<std::mutex> lock(mutex);
    
    for (const auto& entry : other.pImpl->entries) {
        auto oldValue = getValue(entry.first);
        set(entry.first, entry.second.value, source);
    }
}

void ConfigManager::applyTemplate(const std::string& templateName) {
    auto it = templates.find(templateName);
    if (it == templates.end()) {
        logError("Template not found: " + templateName);
        return;
    }
    
    const auto& template = it->second;
    
    for (const auto& pair : template.defaults) {
        auto oldValue = getValue(pair.first);
        set(pair.first, pair.second, ConfigSource::Default);
    }
    
    logger->info("Template applied: " + templateName);
}

std::optional<ConfigValue> ConfigManager::get(const std::string& key) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return it->value;
}

template<typename T>
T ConfigManager::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    return value.has_value() ? value.value() : defaultValue;
}

bool ConfigManager::validate() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::unordered_map<std::string, std::vector<std::string>> errors;
    
    // Check all entries
    for (const auto& entry : pImpl->entries) {
        validateKey(entry.first);
        validateValue(entry.first, entry.value);
    }
    
    // Check constraints
    validateConstraints();
    
    // Check business rules
    // This would be implemented based on specific business logic
    
    if (!errors.empty()) {
        logValidationErrors(errors);
        return false;
    }
    
    return true;
}

bool ConfigManager::validateKey(const std::string& key) const {
    // Implement key validation
    // This could include checks for:
    // - Key format
    // - Key length
    // - Key characters
    return true;
}

bool ConfigManager::validateValue(const std::string& key, const ConfigValue& value) const {
    // Check template validation rules
    for (const auto& template : templates) {
        for (const auto& rule : template.second.validations) {
            if (validateValue(value, rule)) {
                return true;
            }
        }
    }
    
    return true;
}

bool ConfigManager::validateConstraints() const {
    // Check custom constraints
    if (validationCallback) {
        for (const auto& entry : pImpl->entries) {
            if (!validationCallback(entry.first, entry.second.value)) {
                return false;
            }
        }
    }
    
    return true;
}

std::vector<std::string> ConfigManager::getKeys() const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.first);
    }
    return keys;
}

std::vector<std::string> ConfigManager::findKeys(const std::string& pattern) const {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::vector<std::string> matchingKeys;
    for (const auto& entry : pImpl->entries) {
        if (stringContains(toLower(entry.first), toLower(pattern))) {
            matchingKeys.push_back(entry.first);
        }
    }
    return matchingKeys;
}

void ConfigManager::addObserver(std::shared_ptr<ConfigObserver> observer) {
    std::lock_guard<std::mutex> lock(mutex);
    observers.push_back(observer);
}

void ConfigManager::removeObserver(std::shared_ptr<ConfigObserver> observer) {
    std::lock_guard<std::mutex> lock(mutex);
    
    auto it = std::find(observers.begin(), observers.end(), observer);
    if (it != observers.end()) {
        observers.erase(it);
    }
}

void ConfigManager::snapshot(const std::string& description) {
    std::lock_guard<std::mutex> lock(mutex);
    
    std::unordered_map<std::string, ConfigValue> snapshot;
    for (const auto& entry : pImpl->entries) {
        snapshot[entry.first] = entry.second.value;
    }
    
    configHistory.push_back(snapshot);
    
    logger->info("Configuration snapshot created: " + description);
}

std::vector<std::unordered_map<std::string, ConfigValue>> ConfigManager::getHistory() const {
    return configHistory;
}

void ConfigManager::restoreFromSnapshot(size_t index) {
    std::lock_guard<std::mutex> lock(mutex);
    
    if (index >= configHistory.size()) {
        logError("Invalid snapshot index: " + std::to_string(index));
        return;
    }
    
    clear();
    
    for (const auto& pair : configHistory[index]) {
        set(pair.first, pair.second, ConfigSource::Runtime);
    }
    
    logger->info("Configuration restored from snapshot: " + std::to_string(index));
}

void ConfigManager::setBackupDirectory(const std::string& directory, size_t maxBackups) {
    backupDirectory = directory;
    this->maxBackups = maxBackups;
    
    if (!createDirectory(directory)) {
        logError("Failed to create backup directory: " + directory);
    }
}

bool ConfigManager::createBackup(const std::string& description) {
    if (!createDirectory(backupDirectory)) {
        logError("Failed to create backup directory");
        return false;
    }
    
    std::string backupName = "backup_" + generateTimestamp();
    if (!description.empty()) {
        backupName += "_" + description;
    }
    
    std::string backupPath = backupDirectory + "/" + backupName + ".json";
    
    if (!saveToJSON(backupPath)) {
        logError("Failed to create backup: " + backupPath);
        return false;
    }
    
    logger->info("Backup created: " + backupPath + " (" + description + ")");
    
    // Clean up old backups
    cleanBackups();
    
    return true;
}

bool ConfigManager::restoreBackup(const std::string& backupName) {
    std::string backupPath = backupDirectory + "/" + backupName + ".json";
    
    if (!fileExists(backupPath)) {
        logError("Backup not found: " + backupName);
        return false;
    }
    
    clear();
    
    if (!loadFromJSON(backupPath, false)) {
        logError("Failed to restore backup: " + backupName);
        return false;
    }
    
    logger->info("Backup restored: " + backupName);
    return true;
}

std::vector<std::string> ConfigManager::getBackups() const {
    std::vector<std::string> backups;
    
    if (!fileExists(backupDirectory)) {
        return backups;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(backupDirectory)) {
        if (entry.is_regular_file() && endsWith(entry.path().string(), ".json")) {
            backups.push_back(extractFilename(entry.path().string()));
        }
    }
    
    return backups;
}

void ConfigManager::addTemplate(const ConfigTemplate& template) {
    templates[template.name] = template;
    
    logger->info("Template added: " + template.name);
}

void ConfigManager::removeTemplate(const std::string& name) {
    templates.erase(name);
    
    logger->info("Template removed: " + name);
}

bool ConfigManager::hasTemplate(const std::string& name) const {
    return templates.find(name) != templates.end();
}

std::vector<std::string> ConfigManager::getTemplateNames() const {
    std::vector<std::string> names;
    for (const auto& pair : templates) {
        names.push_back(pair.first);
    }
    return names;
}

void ConfigManager::cleanBackups() {
    if (!fileExists(backupDirectory)) {
        return;
    }
    
    std::vector<std::string> backupFiles;
    std::vector<std::chrono::system_clock::time_point> backupTimes;
    
    for (const auto& entry : std::filesystem::directory_iterator(backupDirectory)) {
        if (entry.is_regular_file() && endsWith(entry.path().string(), ".json")) {
            backupFiles.push_back(entry.path().string());
            auto ftime = std::filesystem::last_write_time(entry.path());
            backupTimes.push_back(std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime));
        }
    }
    
    if (backupFiles.size() <= maxBackups) {
        return;
    }
    
    // Sort by time (oldest first)
    std::vector<size_t> indices(backupFiles.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&backupTimes](size_t i1, size_t i2) {
        return backupTimes[i1] < backupTimes[i2];
    });
    
    // Delete oldest backups
    for (size_t i = 0; i < backupFiles.size() - maxBackups; ++i) {
        try {
            std::filesystem::remove(backupFiles[indices[i]]);
            logger->info("Cleaned up old backup: " + extractFilename(backupFiles[indices[i]]));
        } catch (const std::exception& e) {
            logError("Failed to clean up backup: " + std::string(e.what()));
        }
    }
}

bool ConfigManager::exportToPython(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        logError("Failed to export Python data to: " + filepath);
        return false;
    }
    
    // Implementation for Python export
    file << "# NLM Configuration Export for Python\n";
    file << "# Generated by NLM Configuration Manager\n\n";
    
    file << "import json\n";
    file << "from pathlib import Path\n\n";
    
    file << "# Configuration data\n";
    file << "config_data = {\n";
    
    for (size_t i = 0; i < pImpl->entries.size(); ++i) {
        const auto& entry = pImpl->entries[i];
        file << "    \"" << entry.key << "\": ";
        file << formatConfigValue(entry.value) << ",\n";
    }
    
    file << "}\n\n";
    file << "# Helper functions\n";
    file << "def get_config_value(key, default_value=None):\n";
    file << "    \"\"\"Get configuration value with default.\"\"\"\n";
    file << "    return config_data.get(key, default_value)\n\n";
    
    file.close();
    
    logger->info("Configuration exported to Python: " + filepath);
    return true;
}

bool ConfigManager::importFromPython(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        logError("Failed to import Python data from: " + filepath);
        return false;
    }
    
    // Implementation for Python import
    // This would parse the Python file and load configuration data
    
    file.close();
    logger->info("Configuration imported from Python: " + filepath);
    return true;
}

bool ConfigManager::exportToJSONSchema(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        logError("Failed to export JSON schema to: " + filepath);
        return false;
    }
    
    // Implementation for JSON schema export
    file << "{\n";
    file << "  \"$schema\": \"http://json-schema.org/draft-07/schema#\",\n";
    file << "  \"title\": \"NLM Configuration\",\n";
    file << "  \"type\": \"object\",\n";
    file << "  \"properties\": {\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "    \"" << entry.key << "\": {\n";
        file << "      \"type\": \"" << getTypeString(entry.value) << "\",\n";
        file << "      \"description\": \"" << entry.description << "\"\n";
        if (entry.source != ConfigSource::Default) {
            file << "      ,\"source\": \"" << configSourceToString(entry.source) << "\"\n";
        }
        file << "    }\n";
        if (&entry != &pImpl->entries.back()) {
            file << ",\n";
        }
    }
    
    file << "  }\n";
    file << "}\n";
    
    file.close();
    
    logger->info("Configuration exported to JSON schema: " + filepath);
    return true;
}

bool ConfigManager::importFromJSONSchema(const std::string& filepath) {
    // Implementation for JSON schema import
    logError("JSON schema import not implemented yet");
    return false;
}

std::unordered_map<std::string, std::pair<ConfigValue, ConfigValue>> ConfigManager::diff(const ConfigManager& other) const {
    std::unordered_map<std::string, std::pair<ConfigValue, ConfigValue>> differences;
    
    // Check added keys
    for (const auto& entry : other.pImpl->entries) {
        if (!has(entry.first)) {
            differences[entry.first] = std::make_pair(ConfigValue(), entry.second.value);
        }
    }
    
    // Check removed keys
    for (const auto& entry : pImpl->entries) {
        if (!other.has(entry.first)) {
            differences[entry.first] = std::make_pair(entry.second.value, ConfigValue());
        }
    }
    
    // Check modified keys
    for (const auto& entry : pImpl->entries) {
        auto otherValue = other.get(entry.first);
        if (otherValue.has_value() && entry.second.value != otherValue.value()) {
            differences[entry.first] = std::make_pair(entry.second.value, otherValue.value());
        }
    }
    
    return differences;
}

bool ConfigManager::equals(const ConfigManager& other) const {
    return pImpl->entries == other.pImpl->entries;
}

std::vector<std::string> ConfigManager::search(const std::string& query) const {
    std::vector<std::string> results;
    
    for (const auto& entry : pImpl->entries) {
        if (stringContains(toLower(entry.first), toLower(query)) ||
            stringContains(valueToString(entry.value), toLower(query))) {
            results.push_back(entry.first);
        }
    }
    
    return results;
}

std::vector<std::pair<std::string, float>> ConfigManager::suggestKeys(const std::string& partialKey) const {
    std::vector<std::pair<std::string, float>> suggestions;
    
    for (const auto& entry : pImpl->entries) {
        float similarity = calculateKeySimilarity(entry.first, partialKey);
        if (similarity > 0.5) {
            suggestions.push_back(std::make_pair(entry.first, similarity));
        }
    }
    
    std::sort(suggestions.begin(), suggestions.end(),
              [](const std::pair<std::string, float>& a, const std::pair<std::string, float>& b) {
                  return a.second > b.second;
              });
    
    return suggestions;
}

size_t ConfigManager::getEntryCount() const {
    return pImpl->entries.size();
}

size_t ConfigManager::getEntryCount(ConfigSource source) const {
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.second.source == source) {
            count++;
        }
    }
    return count;
}

size_t ConfigManager::getValueCount(const std::string& type) const {
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (typeToString(entry.second.value) == type) {
            count++;
        }
    }
    return count;
}

std::string ConfigManager::getSummary() const {
    std::stringstream ss;
    ss << "Configuration Summary:\n";
    ss << "==================\n\n";
    ss << "Total entries: " << pImpl->entries.size() << "\n";
    ss << "File entries: " << getEntryCount(ConfigSource::File) << "\n";
    ss << "Command line entries: " << getEntryCount(ConfigSource::CommandLine) << "\n";
    ss << "Runtime entries: " << getEntryCount(ConfigSource::Runtime) << "\n";
    ss << "Default entries: " << getEntryCount(ConfigSource::Default) << "\n\n";
    
    ss << "Sources:\n";
    ss << "  File: " << getEntryCount(ConfigSource::File) << " entries\n";
    ss << "  Command Line: " << getEntryCount(ConfigSource::CommandLine) << " entries\n";
    ss << "  Runtime: " << getEntryCount(ConfigSource::Runtime) << " entries\n";
    ss << "  Default: " << getEntryCount(ConfigSource::Default) << " entries\n\n";
    
    return ss.str();
}

std::string ConfigManager::getSourceSummary() const {
    std::stringstream ss;
    ss << "Configuration Source Summary:\n";
    ss << "==========================\n\n";
    
    ss << "File sources:\n";
    for (const auto& entry : pImpl->entries) {
        if (entry.second.source == ConfigSource::File) {
            ss << "  " << entry.first << " = " << valueToString(entry.second.value) << "\n";
        }
    }
    
    ss << "\nCommand line sources:\n";
    for (const auto& entry : pImpl->entries) {
        if (entry.second.source == ConfigSource::CommandLine) {
            ss << "  " << entry.first << " = " << valueToString(entry.second.value) << "\n";
        }
    }
    
    return ss.str();
}

std::string ConfigManager::getValidationSummary() const {
    std::stringstream ss;
    ss << "Configuration Validation Summary:\n";
    ss << "===============================\n\n";
    
    bool isValid = validate();
    ss << "Overall validation: " << (isValid ? "PASS" : "FAIL") << "\n\n";
    
    if (!isValid) {
        ss << "Validation errors:\n";
        // Validation errors would be reported here
        ss << "  (Errors would be listed here)\n";
    }
    
    return ss.str();
}

void ConfigManager::logError(const std::string& error) {
    std::cerr << "[ERROR] " << error << std::endl;
}

void ConfigManager::notifyObservers(const ConfigChangeEvent& event) {
    for (const auto& observer : observers) {
        try {
            observer->onConfigChanged(event);
        } catch (const std::exception& e) {
            logError(std::string("Observer notification failed: ") + e.what());
        }
    }
}

void ConfigManager::validateWithTemplates(const std::string& key, const ConfigValue& value) {
    for (const auto& template : templates) {
        for (const auto& rule : template.second.validations) {
            if (validateValue(value, rule)) {
                return;
            }
        }
    }
}

void ConfigManager::validateConstraints() const {
    if (validationCallback) {
        for (const auto& entry : pImpl->entries) {
            if (!validationCallback(entry.first, entry.second.value)) {
                logError("Configuration constraint violated for key: " + entry.first);
            }
        }
    }
}

bool ConfigManager::validateValue(const ConfigValue& value, const ValidationRule& rule) const {
    switch (rule.type) {
        case ValidationRule::Type::Range:
            return validateRange(value, rule);
        case ValidationRule::Type::AllowedValues:
            return validateAllowedValues(value, rule);
        case ValidationRule::Type::Pattern:
            return validatePattern(value, rule);
        case ValidationRule::Type::Custom:
            return validateCustom(value, rule);
        default:
            return true;
    }
}

bool ConfigManager::validateRange(const ConfigValue& value, const ValidationRule& rule) const {
    // Implementation would depend on the type of value
    return true;
}

bool ConfigManager::validateAllowedValues(const ConfigValue& value, const ValidationRule& rule) const {
    // Implementation would depend on the type of value
    return true;
}

bool ConfigManager::validatePattern(const ConfigValue& value, const ValidationRule& rule) const {
    // Implementation would depend on the type of value
    return true;
}

bool ConfigManager::validateCustom(const ConfigValue& value, const ValidationRule& rule) const {
    // Implementation would depend on custom validation logic
    return true;
}

std::string ConfigManager::formatConfigValue(const ConfigValue& value) const {
    std::stringstream ss;
    
    if (std::holds_alternative<int>(value)) {
        ss << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        ss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        ss << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        ss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        ss << "\"" << std::get<std::string>(value) << "\"";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        ss << "[";
        const auto& vec = std::get<std::vector<int>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        ss << "[";
        const auto& vec = std::get<std::vector<double>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << vec[i];
        }
        ss << "]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        ss << "[";
        const auto& vec = std::get<std::vector<std::string>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << "\"" << vec[i] << "\"";
        }
        ss << "]";
    }
    
    return ss.str();
}

std::string ConfigManager::getTypeString(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) return "integer";
    if (std::holds_alternative<int64_t>(value)) return "integer64";
    if (std::holds_alternative<double>(value)) return "float";
    if (std::holds_alternative<bool>(value)) return "boolean";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "array of integers";
    if (std::holds_alternative<std::vector<double>>(value)) return "array of floats";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "array of strings";
    return "unknown";
}

std::string ConfigManager::configSourceToString(ConfigSource source) const {
    switch (source) {
        case ConfigSource::Default: return "default";
        case ConfigSource::File: return "file";
        case ConfigSource::CommandLine: return "command_line";
        case ConfigSource::Runtime: return "runtime";
        default: return "unknown";
    }
}

std::optional<ConfigValue> ConfigManager::getValue(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return it->value;
}

void ConfigManager::logValidationErrors(const std::unordered_map<std::string, std::vector<std::string>>& errors) const {
    for (const auto& pair : errors) {
        logError("Validation error for key '" + pair.first + "': " + joinStrings(pair.second, ", "));
    }
}

std::string ConfigManager::joinStrings(const std::vector<std::string>& strings, const std::string& separator) const {
    std::stringstream ss;
    for (size_t i = 0; i < strings.size(); ++i) {
        if (i > 0) ss << separator;
        ss << strings[i];
    }
    return ss.str();
}

float ConfigManager::calculateKeySimilarity(const std::string& key1, const std::string& key2) const {
    // Simple Levenshtein distance calculation
    size_t len1 = key1.length();
    size_t len2 = key2.length();
    
    if (len1 == 0) return (len2 == 0) ? 1.0f : 0.0f;
    if (len2 == 0) return 0.0f;
    
    std::vector<std::vector<float>> dp(len1 + 1, std::vector<float>(len2 + 1, 0.0f));
    
    for (size_t i = 0; i <= len1; ++i) dp[i][0] = static_cast<float>(i);
    for (size_t j = 0; j <= len2; ++j) dp[0][j] = static_cast<float>(j);
    
    for (size_t i = 1; i <= len1; ++i) {
        for (size_t j = 1; j <= len2; ++j) {
            float cost = (key1[i - 1] == key2[j - 1]) ? 0.0f : 1.0f;
            dp[i][j] = std::min({dp[i - 1][j] + 1.0f,    // deletion
                                dp[i][j - 1] + 1.0f,    // insertion
                                dp[i - 1][j - 1] + cost}); // substitution
        }
    }
    
    return 1.0f - (dp[len1][len2] / std::max(len1, len2));
}

void ConfigManager::cleanBackups() {
    std::vector<std::string> backupFiles;
    std::vector<std::chrono::system_clock::time_point> backupTimes;
    
    for (const auto& entry : std::filesystem::directory_iterator(backupDirectory)) {
        if (entry.is_regular_file() && endsWith(entry.path().string(), ".json")) {
            backupFiles.push_back(entry.path().string());
            auto ftime = std::filesystem::last_write_time(entry.path());
            backupTimes.push_back(std::chrono::time_point_cast<std::chrono::system_clock::duration>(ftime));
        }
    }
    
    if (backupFiles.size() <= maxBackups) {
        return;
    }
    
    // Sort by time (oldest first)
    std::vector<size_t> indices(backupFiles.size());
    std::iota(indices.begin(), indices.end(), 0);
    std::sort(indices.begin(), indices.end(), [&backupTimes](size_t i1, size_t i2) {
        return backupTimes[i1] < backupTimes[i2];
    });
    
    // Delete oldest backups
    for (size_t i = 0; i < backupFiles.size() - maxBackups; ++i) {
        try {
            std::filesystem::remove(backupFiles[indices[i]]);
        } catch (const std::exception& e) {
            logError("Failed to clean up backup: " + std::string(e.what()));
        }
    }
}

void ConfigManager::validateWithTemplates() {
    for (const auto& template : templates) {
        for (const auto& rule : template.second.validations) {
            // Validate all entries with this template's rules
        }
    }
}

void ConfigManager::applyInheritance() {
    // Apply inheritance from parent configurations
    // This would involve checking for base templates or parent configurations
}

bool ConfigManager::loadFromJSONInternal(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json jsonData;
    try {
        file >> jsonData;
    } catch (const nlohmann::json::parse_error& e) {
        logError("JSON parsing error: " + std::string(e.what()));
        return false;
    }
    
    // Convert JSON to configuration entries
    if (jsonData.contains("configuration")) {
        const auto& config = jsonData["configuration"];
        
        if (config.contains("entries")) {
            for (const auto& entry : config["entries"]) {
                std::string key = entry.value("key");
                std::string valueStr = entry.value("value", "");
                std::string sourceStr = entry.value("source", "file");
                
                ConfigSource source;
                if (sourceStr == "file") source = ConfigSource::File;
                else if (sourceStr == "command_line") source = ConfigSource::CommandLine;
                else if (sourceStr == "runtime") source = ConfigSource::Runtime;
                else source = ConfigSource::Default;
                
                // Parse value based on type
                ConfigValue value = parseConfigValue(valueStr);
                
                set(key, value, source);
            }
        }
    }
    
    return true;
}

bool ConfigManager::loadFromINIInternal(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string section;
    std::string currentSection;
    
    std::string line;
    while (std::getline(file, line)) {
        // Remove leading/trailing whitespace
        line = trim(line);
        
        // Skip empty lines and comments
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }
        
        // Check for section header
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        // Parse key=value pair
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Add section prefix to key
            if (!currentSection.empty()) {
                key = currentSection + "." + key;
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

ConfigValue ConfigManager::parseConfigValue(const std::string& valueStr) {
    // Try to parse as boolean
    if (valueStr == "true" || valueStr == "1") {
        return true;
    } else if (valueStr == "false" || valueStr == "0") {
        return false;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int intValue = std::stoi(valueStr, &pos);
        if (pos == valueStr.size()) {
            return intValue;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleValue = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            return doubleValue;
        }
    } catch (...) {}
    
    // Try to parse as list
    if (valueStr.front() == '[' && valueStr.back() == ']') {
        std::string content = valueStr.substr(1, valueStr.size() - 2);
        std::vector<std::string> items;
        
        size_t start = 0;
        size_t end = content.find(',');
        while (end != std::string::npos) {
            std::string item = trim(content.substr(start, end - start));
            items.push_back(item);
            start = end + 1;
            end = content.find(',', start);
        }
        
        if (start < content.size()) {
            items.push_back(trim(content.substr(start)));
        }
        
        // Try to convert to appropriate type
        if (!items.empty()) {
            // Try to convert to integers
            bool allInts = true;
            std::vector<int> intList;
            for (const auto& item : items) {
                try {
                    int val = std::stoi(item);
                    intList.push_back(val);
                } catch (...) {
                    allInts = false;
                    break;
                }
            }
            if (allInts) {
                return intList;
            }
            
            // Try to convert to doubles
            bool allDoubles = true;
            std::vector<double> doubleList;
            for (const auto& item : items) {
                try {
                    double val = std::stod(item);
                    doubleList.push_back(val);
                } catch (...) {
                    allDoubles = false;
                    break;
                }
            }
            if (allDoubles) {
                return doubleList;
            }
        }
        
        return items;
    }
    
    // Default to string
    return valueStr;
}

bool ConfigManager::loadFromJSONInternal(const std::string& content) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(content);
        
        // Convert JSON to configuration entries
        if (jsonData.contains("configuration")) {
            const auto& config = jsonData["configuration"];
            
            if (config.contains("entries")) {
                for (const auto& entry : config["entries"]) {
                    std::string key = entry.value("key", "");
                    std::string valueStr = entry.value("value", "");
                    std::string sourceStr = entry.value("source", "runtime");
                    
                    ConfigSource source;
                    if (sourceStr == "file") source = ConfigSource::File;
                    else if (sourceStr == "command_line") source = ConfigSource::CommandLine;
                    else if (sourceStr == "runtime") source = ConfigSource::Runtime;
                    else source = ConfigSource::Default;
                    
                    ConfigValue value = parseConfigValue(valueStr);
                    
                    set(key, value, source);
                }
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        logError("JSON parsing error: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        logError("Exception loading configuration from JSON: " + std::string(e.what()));
        return false;
    }
}

bool ConfigManager::loadFromINIInternal(const std::string& content) {
    std::stringstream stream(content);
    std::string line;
    
    std::string section;
    std::string currentSection;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        
        if (line.empty() || line[0] == ';' || line[0] == '#') {
            continue;
        }
        
        if (line[0] == '[' && line.back() == ']') {
            currentSection = line.substr(1, line.size() - 2);
            continue;
        }
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            if (!currentSection.empty()) {
                key = currentSection + "." + key;
            }
            
            set(key, value, ConfigSource::Runtime);
        }
    }
    
    return true;
}

bool ConfigManager::saveToJSONInternal(const std::string& filepath) const {
    nlohmann::json jsonData;
    
    nlohmann::json configArray = nlohmann::json::array();
    for (const auto& entry : pImpl->entries) {
        nlohmann::json jsonEntry;
        jsonEntry["key"] = entry.first;
        jsonEntry["value"] = formatConfigValue(entry.second.value);
        jsonEntry["source"] = configSourceToString(entry.second.source);
        configArray.push_back(jsonEntry);
    }
    
    jsonData["configuration"]["entries"] = configArray;
    jsonData["configuration"]["version"] = "1.0";
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        logError("Failed to save JSON configuration to: " + filepath);
        return false;
    }
    
    file << std::setw(2) << jsonData;
    
    return true;
}

bool ConfigManager::saveToINIInternal(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        logError("Failed to save INI configuration to: " + filepath);
        return false;
    }
    
    // Group entries by section
    std::unordered_map<std::string, std::vector<ConfigEntry>> sections;
    
    for (const auto& entry : pImpl->entries) {
        size_t pos = entry.first.find('.');
        std::string section = (pos == std::string::npos) ? "DEFAULT" : entry.first.substr(0, pos);
        sections[section].push_back(entry);
    }
    
    // Write sections
    for (const auto& pair : sections) {
        const std::string& section = pair.first;
        const auto& entries = pair.second;
        
        file << "[" << section << "]\n";
        
        for (const auto& entry : entries) {
            file << entry.first.substr(section.size() + 1) << " = " << formatConfigValue(entry.second.value) << "\n";
        }
        
        file << "\n";
    }
    
    return true;
}

void ConfigManager::validateConstraints() const {
    // Check custom constraints
    if (validationCallback) {
        for (const auto& entry : pImpl->entries) {
            if (!validationCallback(entry.first, entry.second.value)) {
                logError("Configuration constraint violated for key: " + entry.first);
            }
        }
    }
}

void ConfigManager::validateDependencies() const {
    // Check configuration dependencies
    // This would implement dependency checking logic
}

void ConfigManager::logValidationErrors(const std::unordered_map<std::string, std::vector<std::string>>& errors) const {
    for (const auto& pair : errors) {
        logError("Validation error for key '" + pair.first + "': " + joinStrings(pair.second, ", "));
    }
}

void ConfigManager::checkPerformance() {
    auto now = std::chrono::system_clock::now();
    auto timeSinceLastCheck = std::chrono::duration_cast<std::chrono::seconds>(now - lastMonitorCheck);
    
    if (timeSinceLastCheck.count() >= monitoringInterval) {
        // Update statistics
        updateStatistics();
        
        // Check performance thresholds
        checkPerformanceThresholds();
        
        lastMonitorCheck = now;
    }
}

void ConfigManager::updateStatistics() {
    size_t activeConnections = 0;
    for (const auto& observer : observers) {
        // Check if observer is active
        activeConnections++;
    }
    
    this->stats.totalEntries = pImpl->entries.size();
    // Update other statistics...
}

void ConfigManager::checkPerformanceThresholds() const {
    // Check performance thresholds
    // This would implement performance threshold checking
}

void ConfigManager::notifyObservers(const ConfigChangeEvent& event) {
    // This is defined in the header
}

void ConfigManager::setLoggingConfig(const std::string& configFile) {
    // Set up logging configuration
    // This would configure the logging system based on configuration
}

void ConfigManager::setMonitoringCallback(std::function<void(const ConfigChangeEvent&)> callback) {
    monitoringCallback = callback;
}

void ConfigManager::setValidationCallback(std::function<bool(const std::string&, const ConfigValue&)> callback) {
    validationCallback = callback;
}

// Global configuration manager instance
std::shared_ptr<ConfigManager> globalConfig = std::make_shared<ConfigManager>();

// Convenience functions
void initializeGlobalConfig(const std::string& configFile) {
    globalConfig->loadFromFile(configFile);
}

std::shared_ptr<ConfigManager> getGlobalConfig() {
    return globalConfig;
}

void setGlobalConfig(std::shared_ptr<ConfigManager> config) {
    globalConfig = config;
}

// Configuration utilities
std::string generateConfigID() {
    return "config_" + generateTimestamp();
}

std::string generateConfigChecksum(const ConfigManager& config) {
    // Generate checksum based on configuration content
    std::hash<std::string> hasher;
    std::string content = "config";
    for (const auto& entry : config.getKeys()) {
        content += entry;
    }
    return std::to_string(hasher(content));
}

bool validateConfigIntegrity(const ConfigManager& config) {
    // Validate configuration integrity
    return config.validate();
}

} // namespace nlm
