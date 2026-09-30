#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <unordered_set>
#include <ctime>
#include <iomanip>
#include <regex>
#include <chrono>
#include <cstring>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigChangeRecord> history;
    ConfigSchema schema;
    ConfigVersion version;
    bool historyEnabled;
    bool environmentVariablesEnabled;
    std::string environmentPrefix;
    bool hotReloadingEnabled;
    int reloadInterval;
    std::string autoReloadPath;
    FileFormat autoReloadFormat;
    std::vector<std::string> inheritanceSources;
    std::string encryptionKey;
    bool encrypted;
    std::string backupDir;
    
    Impl() : historyEnabled(false), environmentVariablesEnabled(false), 
             environmentPrefix(""), hotReloadingEnabled(false), 
             reloadInterval(5), autoReloadPath(""), 
             autoReloadFormat(FileFormat::Auto), encrypted(false),
             backupDir("configs_backups") {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default values
    auto now = std::chrono::system_clock::now();
    ConfigEntry defaultEntry("config_version", ConfigValue(1), ConfigSource::Default, "Configuration version");
    defaultEntry.versionIntroduced = ConfigVersion(1, 0, 0);
    pImpl->entries.push_back(defaultEntry);
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept = default;
Config& Config::operator=(Config&& other) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, FileFormat format) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    FileFormat detectedFormat = (format == FileFormat::Auto) ? detectFormat(content) : format;
    
    clear();
    
    bool success = false;
    switch (detectedFormat) {
        case FileFormat::KeyValue:
            success = loadFromString(content, FileFormat::KeyValue);
            break;
        case FileFormat::JSON:
            success = fromJSON(content, *this);
            break;
        case FileFormat::YAML:
            success = fromYAML(content, *this);
            break;
        case FileFormat::XML:
            success = fromXML(content, *this);
            break;
        case FileFormat::TOML:
            success = fromTOML(content, *this);
            break;
        case FileFormat::INI:
            success = fromINI(content, *this);
            break;
        case FileFormat::Auto:
            break;
    }
    
    if (success && !filepath.empty()) {
        saveBackup(filepath);
    }
    
    return success;
}

bool Config::loadFromString(const std::string& content, FileFormat format) {
    clear();
    
    FileFormat detectedFormat = (format == FileFormat::Auto) ? detectFormat(content) : format;
    
    bool success = false;
    switch (detectedFormat) {
        case FileFormat::KeyValue:
            success = loadFromKeyValueString(content);
            break;
        case FileFormat::JSON:
            success = fromJSON(content, *this);
            break;
        case FileFormat::YAML:
            success = fromYAML(content, *this);
            break;
        case FileFormat::XML:
            success = fromXML(content, *this);
            break;
        case FileFormat::TOML:
            success = fromTOML(content, *this);
            break;
        case FileFormat::INI:
            success = fromINI(content, *this);
            break;
        case FileFormat::Auto:
            success = loadFromKeyValueString(content); // Fallback to key=value
            break;
    }
    
    if (success) {
        applyInheritance();
        if (isHistoryEnabled()) {
            logChange("Configuration loaded from string", ConfigSource::File);
        }
    }
    
    return success;
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
                try {
                    setFromString(key, value, ConfigSource::CommandLine);
                } catch (...) {
                    success = false;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            try {
                setFromString(key, value, ConfigSource::CommandLine);
            } catch (...) {
                success = false;
            }
        }
    }
    
    if (success && isHistoryEnabled()) {
        logChange("Configuration loaded from command line", ConfigSource::CommandLine);
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath, FileFormat format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content;
    FileFormat detectedFormat = (format == FileFormat::Auto) ? detectFormat(saveToString()) : format;
    
    switch (detectedFormat) {
        case FileFormat::KeyValue:
            content = saveToKeyValueString();
            break;
        case FileFormat::JSON:
            content = toJSON(*this);
            break;
        case FileFormat::YAML:
            content = toYAML(*this);
            break;
        case FileFormat::XML:
            content = toXML(*this);
            break;
        case FileFormat::TOML:
            content = toTOML(*this);
            break;
        case FileFormat::INI:
            content = toINI(*this);
            break;
        case FileFormat::Auto:
            content = saveToKeyValueString();
            break;
    }
    
    file << content;
    return true;
}

std::string Config::saveToString(FileFormat format) const {
    FileFormat detectedFormat = (format == FileFormat::Auto) ? FileFormat::KeyValue : format;
    
    switch (detectedFormat) {
        case FileFormat::KeyValue:
            return saveToKeyValueString();
        case FileFormat::JSON:
            return toJSON(*this);
        case FileFormat::YAML:
            return toYAML(*this);
        case FileFormat::XML:
            return toXML(*this);
        case FileFormat::TOML:
            return toTOML(*this);
        case FileFormat::INI:
            return toINI(*this);
        case FileFormat::Auto:
            return saveToKeyValueString();
    }
    return "";
}

bool Config::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    return errors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Validate each entry against schema
    for (const auto& entry : pImpl->entries) {
        if (pImpl->schema.validationRules.empty()) continue;
        
        for (const auto& rule : pImpl->schema.validationRules) {
            if (rule.key == entry.key || rule.key.empty()) {
                if (!validateValue(entry)) {
                    errors.push_back("Validation failed for key '" + entry.key + "': " + rule.message);
                }
            }
        }
    }
    
    // Validate cross-parameter dependencies
    if (!validateCrossParameters()) {
        errors.push_back("Cross-parameter validation failed");
    }
    
    // Check required keys
    for (const auto& requiredKey : pImpl->schema.requiredKeys) {
        if (!has(requiredKey)) {
            errors.push_back("Required key missing: '" + requiredKey + "'");
        }
    }
    
    return errors;
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

void Config::set(const std::string& key, const ConfigValue& value, 
                 ConfigSource source,
                 const std::string& description,
                 const std::string& category) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    std::optional<ConfigValue> oldValue;
    if (it != pImpl->entries.end()) {
        oldValue = it->value;
    }
    
    ConfigEntry newEntry(key, value, source, description, category);
    newEntry.updatedAt = std::chrono::system_clock::now();
    
    if (it != pImpl->entries.end()) {
        *it = newEntry;
    } else {
        newEntry.createdAt = std::chrono::system_clock::now();
        pImpl->entries.push_back(newEntry);
    }
    
    if (isHistoryEnabled() && oldValue.has_value()) {
        logChange(key, oldValue.value(), value, source);
    }
    
    validateAndApplyDependencies(key, value);
}

void Config::set(const std::string& key, const std::string& value,
                 ConfigSource source,
                 const std::string& description,
                 const std::string& category) {
    set(key, ConfigValue(value), source, description, category);
}

void Config::set(const std::string& key, int value,
                 ConfigSource source,
                 const std::string& description,
                 const std::string& category) {
    set(key, ConfigValue(value), source, description, category);
}

void Config::set(const std::string& key, double value,
                 ConfigSource source,
                 const std::string& description,
                 const std::string& category) {
    set(key, ConfigValue(value), source, description, category);
}

void Config::set(const std::string& key, bool value,
                 ConfigSource source,
                 const std::string& description,
                 const std::string& category) {
    set(key, ConfigValue(value), source, description, category);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    auto oldValue = getByKey(key);
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    if (isHistoryEnabled() && oldValue.has_value()) {
        logChange(key, oldValue.value(), std::nullopt, ConfigSource::Runtime);
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeysByCategory(const std::string& category) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    // Keep version entry
    std::vector<ConfigEntry> versionEntries;
    for (const auto& entry : pImpl->entries) {
        if (entry.key == "config_version") {
            versionEntries.push_back(entry);
        }
    }
    
    pImpl->entries = versionEntries;
    
    if (isHistoryEnabled()) {
        clearHistory();
    }
}

void Config::setSchema(const ConfigSchema& schema) {
    pImpl->schema = schema;
}

void Config::setVersion(const ConfigVersion& version) {
    pImpl->version = version;
    set("config_version", static_cast<int>(version.major * 10000 + version.minor * 100 + version.patch), 
        ConfigSource::Runtime, "Configuration version");
}

ConfigVersion Config::getVersion() const {
    auto versionOpt = get<int>("config_version");
    if (versionOpt.has_value()) {
        int versionNum = versionOpt.value();
        int major = versionNum / 10000;
        int minor = (versionNum % 10000) / 100;
        int patch = versionNum % 100;
        return ConfigVersion(major, minor, patch);
    }
    return ConfigVersion();
}

bool Config::attemptRecovery() {
    // Try to recover from backup if available
    std::vector<std::string> backups = getBackupFiles();
    if (backups.empty()) {
        return false;
    }
    
    // Try the most recent backup
    std::sort(backups.begin(), backups.end(), std::greater<std::string>());
    std::string latestBackup = backups[0];
    
    return restoreBackup(latestBackup);
}

bool Config::restoreBackup(const std::string& backupFile) {
    std::string actualBackupFile = backupFile.empty() ? (pImpl->backupDir + "/latest") : backupFile;
    
    if (!std::filesystem::exists(actualBackupFile)) {
        return false;
    }
    
    Config backupConfig;
    if (!backupConfig.loadFromFile(actualBackupFile)) {
        return false;
    }
    
    // Replace current configuration with backup
    pImpl->entries = backupConfig.pImpl->entries;
    pImpl->history = backupConfig.pImpl->history;
    
    if (isHistoryEnabled()) {
        logChange("Configuration restored from backup", ConfigSource::File);
    }
    
    return true;
}

std::vector<std::string> Config::getBackupFiles() const {
    std::vector<std::string> backups;
    
    if (!std::filesystem::exists(pImpl->backupDir)) {
        return backups;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator(pImpl->backupDir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".cfg") {
            backups.push_back(entry.path().string());
        }
    }
    
    return backups;
}

void Config::enableHistory(bool enabled) {
    pImpl->historyEnabled = enabled;
    if (!enabled) {
        clearHistory();
    }
}

bool Config::isHistoryEnabled() const {
    return pImpl->historyEnabled;
}

void Config::clearHistory() {
    pImpl->history.clear();
}

std::vector<ConfigChangeRecord> Config::getHistory(size_t maxEntries) const {
    std::vector<ConfigChangeRecord> history = pImpl->history;
    if (history.size() > maxEntries) {
        history.erase(history.begin(), history.begin() + (history.size() - maxEntries));
    }
    return history;
}

void Config::enableEnvironmentVariables(bool enabled) {
    pImpl->environmentVariablesEnabled = enabled;
}

bool Config::areEnvironmentVariablesEnabled() const {
    return pImpl->environmentVariablesEnabled;
}

void Config::setEnvironmentPrefix(const std::string& prefix) {
    pImpl->environmentPrefix = prefix;
}

std::string Config::getEnvironmentPrefix() const {
    return pImpl->environmentPrefix;
}

void Config::enableHotReloading(bool enabled) {
    pImpl->hotReloadingEnabled = enabled;
}

bool Config::isHotReloadingEnabled() const {
    return pImpl->hotReloadingEnabled;
}

void Config::setReloadInterval(int seconds) {
    pImpl->reloadInterval = seconds;
}

void Config::startAutoReload(const std::string& filepath, FileFormat format) {
    pImpl->autoReloadPath = filepath;
    pImpl->autoReloadFormat = format;
    // In a real implementation, this would start a background thread
}

void Config::stopAutoReload() {
    pImpl->autoReloadPath = "";
    pImpl->autoReloadFormat = FileFormat::Auto;
}

void Config::addInheritance(const std::string& sourceConfig, ConfigSource source) {
    pImpl->inheritanceSources.push_back(sourceConfig);
    applyInheritance();
}

void Config::clearInheritance() {
    pImpl->inheritanceSources.clear();
    clear();
    // Keep default entries
    ConfigEntry defaultEntry("config_version", ConfigValue(1), ConfigSource::Default, "Configuration version");
    defaultEntry.versionIntroduced = ConfigVersion(1, 0, 0);
    pImpl->entries.push_back(defaultEntry);
}

std::vector<std::string> Config::getInheritanceSources() const {
    return pImpl->inheritanceSources;
}

Config Config::createFromTemplate(const std::string& templateName) {
    Config config;
    
    // Load template configuration
    std::string templatePath = "templates/" + templateName + ".cfg";
    if (!config.loadFromFile(templatePath)) {
        // Fallback to built-in templates
        if (templateName == "minimal") {
            config.set("neuron_count", 1000);
            config.set("region_count", 2);
            config.set("simulation_timestep", 0.001);
        } else if (templateName == "development") {
            config.set("neuron_count", 10000);
            config.set("region_count", 5);
            config.set("development_synaptogenesis_rate", 0.01);
            config.set("development_pruning_rate", 0.001);
            config.set("visualization_enabled", true);
        }
    }
    
    return config;
}

bool Config::saveAsTemplate(const std::string& name, const Config& config) {
    std::string templatePath = "templates/" + name + ".cfg";
    std::filesystem::create_directories("templates");
    return config.saveToFile(templatePath);
}

std::vector<std::string> Config::getAvailableTemplates() {
    std::vector<std::string> templates;
    
    if (!std::filesystem::exists("templates")) {
        return templates;
    }
    
    for (const auto& entry : std::filesystem::directory_iterator("templates")) {
        if (entry.is_regular_file() && entry.path().extension() == ".cfg") {
            std::string filename = entry.path().filename().string();
            // Remove extension
            if (filename.size() > 4) {
                templates.push_back(filename.substr(0, filename.size() - 4));
            }
        }
    }
    
    return templates;
}

bool Config::equals(const Config& other) const {
    if (pImpl->entries.size() != other.pImpl->entries.size()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (otherIt == other.pImpl->entries.end()) {
            return false;
        }
        
        if (entry.value != otherIt->value) {
            return false;
        }
    }
    
    return true;
}

std::string Config::diff(const Config& other) const {
    std::stringstream diff;
    diff << "=== CONFIGURATION DIFF ===\n";
    
    // Find added and modified keys
    std::unordered_set<std::string> currentKeys;
    for (const auto& entry : pImpl->entries) {
        currentKeys.insert(entry.key);
        
        auto otherIt = std::find_if(other.pImpl->entries.begin(), other.pImpl->entries.end(),
            [&entry](const ConfigEntry& e) { return e.key == entry.key; });
        
        if (otherIt == other.pImpl->entries.end()) {
            diff << "ADDED: " << entry.key << " = " << toString(entry.value) << "\n";
        } else if (entry.value != otherIt->value) {
            diff << "MODIFIED: " << entry.key << "\n";
            diff << "  OLD: " << toString(entry.value) << "\n";
            diff << "  NEW: " << toString(otherIt->value) << "\n";
        }
    }
    
    // Find deleted keys
    for (const auto& entry : other.pImpl->entries) {
        if (currentKeys.find(entry.key) == currentKeys.end()) {
            diff << "DELETED: " << entry.key << " = " << toString(entry.value) << "\n";
        }
    }
    
    return diff.str();
}

std::string Config::generateDocumentation() const {
    std::stringstream doc;
    doc << "# Configuration Documentation\n\n";
    doc << "Generated: " << getCurrentTimestamp() << "\n\n";
    
    doc << "## Configuration Summary\n";
    doc << "Version: " << pImpl->version.toString() << "\n";
    doc << "Total Parameters: " << pImpl->entries.size() << "\n\n";
    
    doc << "## Configuration Parameters\n\n";
    
    // Group by category
    std::unordered_map<std::string, std::vector<ConfigEntry>> categories;
    for (const auto& entry : pImpl->entries) {
        if (entry.key != "config_version") {
            categories[entry.category].push_back(entry);
        }
    }
    
    for (const auto& [category, entries] : categories) {
        doc << "### " << category << "\n\n";
        for (const auto& entry : entries) {
            doc << "- **" << entry.key << "**";
            if (!entry.description.empty()) {
                doc << " - " << entry.description;
            }
            doc << "\n  - Type: " << getTypeString(entry.value) << "\n";
            doc << "  - Value: " << toString(entry.value) << "\n";
            doc << "  - Source: " << static_cast<int>(entry.source) << "\n\n";
        }
    }
    
    return doc.str();
}

std::string Config::getAutocompleteSuggestions(const std::string& partialKey) const {
    std::stringstream suggestions;
    std::string lowerPartial = toLower(partialKey);
    
    for (const auto& entry : pImpl->entries) {
        if (toLower(entry.key).find(lowerPartial) != std::string::npos) {
            suggestions << entry.key << " (" << getTypeString(entry.value) << ")" << "\n";
        }
    }
    
    return suggestions.str();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Enhanced Configuration (" << pImpl->entries.size() << " entries):\n";
    oss << "Version: " << pImpl->version.toString() << "\n";
    oss << "History Enabled: " << (pImpl->historyEnabled ? "Yes" : "No") << "\n";
    oss << "Environment Variables: " << (pImpl->environmentVariablesEnabled ? "Yes" : "No") << "\n";
    oss << "Hot Reloading: " << (pImpl->hotReloadingEnabled ? "Yes" : "No") << "\n";
    oss << "Encryption: " << (pImpl->encrypted ? "Enabled" : "Disabled") << "\n\n";
    
    oss << "Entries by source:\n";
    for (int i = 0; i < 7; ++i) {
        ConfigSource source = static_cast<ConfigSource>(i);
        size_t count = getEntryCountBySource(source);
        oss << "  " << static_cast<int>(source) << ": " << count << "\n";
    }
    
    return oss.str();
}

void Config::setEncryptionKey(const std::string& key) {
    pImpl->encryptionKey = key;
    pImpl->encrypted = !key.empty();
}

bool Config::isEncrypted() const {
    return pImpl->encrypted;
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getEntryCountBySource(ConfigSource source) const {
    return std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&source](const ConfigEntry& e) { return e.source == source; });
}

// Internal helper methods

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

bool Config::parseKeyValueLine(const std::string& line, std::string& key, std::string& value) {
    size_t pos = line.find('=');
    if (pos == std::string::npos) {
        return false;
    }
    
    key = trim(line.substr(0, pos));
    value = trim(line.substr(pos + 1));
    
    // Remove quotes
    if (value.size() >= 2 && 
        ((value.front() == '"' && value.back() == '"') ||
         (value.front() == '\'' && value.back() == '\''))) {
        value = value.substr(1, value.size() - 2);
    }
    
    return !key.empty();
}

bool Config::loadFromKeyValueString(const std::string& content) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        std::string key, value;
        if (parseKeyValueLine(line, key, value)) {
            setFromString(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

void Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Try to detect type from string
    if (valueStr == "true" || valueStr == "false") {
        set(key, valueStr == "true", source);
    } else {
        try {
            // Try integer
            size_t pos;
            long long_val = std::stoll(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, static_cast<int>(long_val), source);
                return;
            }
        } catch (...) {}
        
        try {
            // Try double
            size_t pos;
            double double_val = std::stod(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, double_val, source);
                return;
            }
        } catch (...) {}
        
        // Default to string
        set(key, valueStr, source);
    }
}

std::optional<ConfigEntry> Config::getByKey(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return *it;
    }
    return std::nullopt;
}

void Config::logChange(const std::string& key, const ConfigValue& oldValue, 
                       const ConfigValue& newValue, ConfigSource source) {
    ConfigChangeRecord record(key, oldValue, newValue, source, "system");
    record.notes = "Key: " + key;
    pImpl->history.push_back(record);
}

void Config::logChange(const std::string& description, ConfigSource source) {
    ConfigChangeRecord record("system", std::nullopt, std::nullopt, source, "system");
    record.notes = description;
    pImpl->history.push_back(record);
}

void Config::applyInheritance() {
    for (const auto& sourceConfig : pImpl->inheritanceSources) {
        Config inheritedConfig;
        if (inheritedConfig.loadFromFile(sourceConfig)) {
            // Merge inherited entries
            for (const auto& entry : inheritedConfig.pImpl->entries) {
                if (!has(entry.key)) {
                    set(entry.key, entry.value, entry.source, entry.description, entry.category);
                }
            }
        }
    }
}

bool Config::validateValue(const ConfigEntry& entry) const {
    // Basic type validation - this can be enhanced with schema validation
    return true;
}

bool Config::validateCrossParameters() const {
    // Cross-parameter validation (e.g., neuron_count and region_count ratios)
    auto neuronCountOpt = get<size_t>("neuron_count");
    auto regionCountOpt = get<size_t>("region_count");
    
    if (neuronCountOpt.has_value() && regionCountOpt.has_value()) {
        size_t neurons = neuronCountOpt.value();
        size_t regions = regionCountOpt.value();
        
        if (regions > 0 && neurons / regions > 10000) {
            return false; // Too many neurons per region
        }
        
        if (neurons < 100) {
            return false; // Too few neurons
        }
    }
    
    return true;
}

std::optional<ConfigValue> Config::getFromEnvironment(const std::string& key) const {
    if (!pImpl->environmentVariablesEnabled) {
        return std::nullopt;
    }
    
    std::string envKey = pImpl->environmentPrefix + toUpper(key);
    const char* envValue = std::getenv(envKey.c_str());
    if (!envValue) {
        return std::nullopt;
    }
    
    // Parse environment variable value to appropriate type
    std::string valueStr = envValue;
    return parseConfigValue(valueStr);
}

ConfigValue Config::parseConfigValue(const std::string& valueStr) {
    if (valueStr == "true" || valueStr == "false") {
        return valueStr == "true";
    }
    
    try {
        // Try integer
        size_t pos;
        long long_val = std::stoll(valueStr, &pos);
        if (pos == valueStr.size()) {
            return static_cast<int>(long_val);
        }
    } catch (...) {}
    
    try {
        // Try double
        size_t pos;
        double double_val = std::stod(valueStr, &pos);
        if (pos == valueStr.size()) {
            return double_val;
        }
    } catch (...) {}
    
    // Default to string
    return valueStr;
}

void Config::validateAndApplyDependencies(const std::string& key, const ConfigValue& value) {
    // Implement dependency validation logic
}

std::string Config::toString(const ConfigValue& value) {
    std::stringstream ss;
    std::visit([&ss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << arg << "\"";
        } else {
            ss << arg;
        }
    }, value);
    return ss.str();
}

std::string Config::getTypeString(const ConfigValue& value) {
    std::string typeName;
    std::visit([&typeName](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) typeName = "int";
        else if constexpr (std::is_same_v<T, int64_t>) typeName = "int64_t";
        else if constexpr (std::is_same_v<T, double>) typeName = "double";
        else if constexpr (std::is_same_v<T, bool>) typeName = "bool";
        else if constexpr (std::is_same_v<T, std::string>) typeName = "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) typeName = "int[]";
        else if constexpr (std::is_same_v<T, std::vector<double>>) typeName = "double[]";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) typeName = "string[]";
        else if constexpr (std::is_same_v<T, std::vector<bool>>) typeName = "bool[]";
        else typeName = "unknown";
    }, value);
    return typeName;
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

FileFormat Config::detectFormat(const std::string& content) {
    if (content.empty()) return FileFormat::KeyValue;
    
    // Check for JSON
    if (content.find('{') != std::string::npos && content.find('}') != std::string::npos) {
        return FileFormat::JSON;
    }
    
    // Check for YAML
    if (content.find("  ") != std::string::npos || content.find(": ") != std::string::npos) {
        return FileFormat::YAML;
    }
    
    // Check for XML
    if (content.find('<') != std::string::npos && content.find('>') != std::string::npos) {
        return FileFormat::XML;
    }
    
    // Check for TOML
    if (content.find("=") != std::string::npos && content.find('"') != std::string::npos) {
        return FileFormat::TOML;
    }
    
    // Check for INI
    if (content.find('[') != std::string::npos && content.find(']') != std::string::npos) {
        return FileFormat::INI;
    }
    
    return FileFormat::KeyValue;
}

void Config::saveBackup(const std::string& filepath) {
    if (pImpl->backupDir.empty()) return;
    
    std::filesystem::create_directories(pImpl->backupDir);
    
    std::string timestamp = std::to_string(std::time(nullptr));
    std::string backupPath = pImpl->backupDir + "/backup_" + timestamp + ".cfg";
    
    saveToFile(backupPath, FileFormat::KeyValue);
}

// Explicit template instantiations

// Helper function for JSON serialization
std::string Config::toJSON(const Config& config) {
    std::stringstream ss;
    ss << "{\n";
    for (size_t i = 0; i < config.pImpl->entries.size(); ++i) {
        const auto& entry = config.pImpl->entries[i];
        ss << "  \"" << entry.key << "\": " << toJSONValue(entry.value);
        if (i < config.pImpl->entries.size() - 1) ss << ",\n";
    }
    ss << "\n}\n";
    return ss.str();
}

std::string Config::toJSONValue(const ConfigValue& value) {
    std::stringstream ss;
    std::visit([&ss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            ss << "\"" << escapeJSON(arg) << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            ss << (arg ? "true" : "false");
        } else {
            ss << arg;
        }
    }, value);
    return ss.str();
}

std::string Config::escapeJSON(const std::string& str) {
    std::stringstream ss;
    for (char c : str) {
        switch (c) {
            case '\\': ss << "\\\\"; break;
            case '\"': ss << "\\\""; break;
            case '\b': ss << "\\b"; break;
            case '\f': ss << "\\f"; break;
            case '\n': ss << "\\n"; break;
            case '\r': ss << "\\r"; break;
            case '\t': ss << "\\t"; break;
            default: ss << c; break;
        }
    }
    return ss.str();
}

bool Config::fromJSON(const std::string& content, Config& config) {
    // Simple JSON parsing - in a real implementation would use a proper JSON library
    if (content.empty() || content[0] != '{') {
        return false;
    }
    
    // For now, just load as key=value format as fallback
    return config.loadFromKeyValueString(content);
}

// Similar implementations for other formats (YAML, XML, etc.) would go here...

// Encryption implementations
std::string Config::encrypt(const std::string& data) const {
    if (pImpl->encryptionKey.empty()) {
        return data; // No encryption
    }
    
    // Simple XOR encryption for demonstration
    std::string result = data;
    for (size_t i = 0; i < data.size(); ++i) {
        result[i] = data[i] ^ pImpl->encryptionKey[i % pImpl->encryptionKey.size()];
    }
    
    return result;
}

std::string Config::decrypt(const std::string& data) const {
    if (pImpl->encryptionKey.empty()) {
        return data; // No encryption
    }
    
    // Same XOR for decryption
    return encrypt(data);
}

} // namespace nlm