#include "Config.hpp"
#include <map>
#include <any>
#include <vector>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <unordered_map>
#include <shared_mutex>

namespace nlm {

// Forward declarations
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unique_ptr<ConfigSchema> schema;
    std::unique_ptr<ConfigMigration> migration;
    std::unique_ptr<ConfigWatcher> watcher;
    std::map<std::string, std::any> configMap;
    std::map<std::string, std::any> parentConfig;
    bool schemaEnabled = false;
    bool validationEnabled = false;
    int migrationVersion = 1;
    std::shared_mutex mutex;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default schema
    pImpl->schema = std::make_unique<ConfigSchema>();
    pImpl->migration = std::make_unique<ConfigMigration>();
    pImpl->watcher = std::make_unique<ConfigWatcher>();
    
    // Initialize built-in presets
    pImpl->schema->addParameter(ParameterInfo("mode", "Operating mode", ParameterType::Choice, std::nullopt, 
        ChoiceSpec({"quick", "balanced", "research"}), std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"performance"}, "quick", 0, ""));
    
    pImpl->schema->addParameter(ParameterInfo("performance", "Performance level (0-100)", ParameterType::Range, 
        RangeSpec("0", "100"), std::nullopt, std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"speed", "quality"}, "50", 1, "%"));
    
    pImpl->schema->addParameter(ParameterInfo("memory", "Memory allocation in MB", ParameterType::Int, 
        RangeSpec("256", "16384"), std::nullopt, std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"resources", "performance"}, "2048", 2, "MB"));
    
    pImpl->schema->addParameter(ParameterInfo("threads", "Number of threads to use", ParameterType::Int, 
        RangeSpec("1", "32"), std::nullopt, std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"performance", "parallelism"}, "4", 3, ""));
    
    pImpl->schema->addParameter(ParameterInfo("enable_cuda", "Enable CUDA acceleration", ParameterType::Bool, 
        std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::vector<std::string>{"hardware"},
        "false", 4, ""));
    
    pImpl->schema->addParameter(ParameterInfo("verbose", "Enable verbose logging", ParameterType::Bool, 
        std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::vector<std::string>{"debug"},
        "false", 5, ""));
    
    pImpl->schema->addParameter(ParameterInfo("cache_size", "Cache size in MB", ParameterType::Int, 
        RangeSpec("64", "4096"), std::nullopt, std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"performance", "memory"}, "256", 6, "MB"));
    
    pImpl->schema->addParameter(ParameterInfo("timeout", "Operation timeout in seconds", ParameterType::Int, 
        RangeSpec("30", "3600"), std::nullopt, std::nullopt, std::nullopt, std::nullopt,
        std::vector<std::string>{"performance", "safety"}, "300", 7, "s"));
    
    pImpl->schemaEnabled = true;
    pImpl->validationEnabled = true;
    pImpl->migration->setCurrentVersion(1);
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Helper methods
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

// Configuration management methods
void Config::setSchemaEnabled(bool enabled) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    pImpl->schemaEnabled = enabled;
}

void Config::setValidationEnabled(bool enabled) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    pImpl->validationEnabled = enabled;
}

void Config::setParentConfig(const Config& parent) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    pImpl->parentConfig = convertConfigToMap(parent);
}

void Config::addChangeListener(std::shared_ptr<ConfigChangeListener> listener) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    pImpl->watcher->addListener(listener);
}

void Config::removeChangeListener(std::shared_ptr<ConfigChangeListener> listener) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    pImpl->watcher->removeListener(listener);
}

void Config::setWatchInterval(std::chrono::milliseconds interval) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    pImpl->watcher->setWatchInterval(interval);
}

// Presets loading
void Config::loadQuickPreset() {
    loadPreset(ConfigPresets::getQuickPreset());
}

void Config::loadBalancedPreset() {
    loadPreset(ConfigPresets::getBalancedPreset());
}

void Config::loadResearchPreset() {
    loadPreset(ConfigPresets::getResearchPreset());
}

void Config::loadMinimalPreset() {
    loadPreset(ConfigPresets::getMinimalPreset());
}

void Config::loadMaximumPreset() {
    loadPreset(ConfigPresets::getMaximumPreset());
}

// Validation
std::vector<ConfigError> Config::validate() const {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    if (!pImpl->validationEnabled || !pImpl->schemaEnabled) {
        return {};
    }
    
    return validateConfig();
}

// Export/Import
bool Config::exportToYAML(const std::string& filepath) const {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    return exportToYAMLFile(filepath, pImpl->configMap);
}

bool Config::importFromYAML(const std::string& filepath) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    auto config = importFromYAMLFile(filepath);
    if (config.empty()) {
        return false;
    }
    
    clear();
    for (const auto& pair : config) {
        setEntryFromAny(pair.first, pair.second);
    }
    
    return true;
}

bool Config::exportToJSON(const std::string& filepath) const {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    return exportToJSONFile(filepath, pImpl->configMap);
}

bool Config::importFromJSON(const std::string& filepath) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    auto config = importFromJSONFile(filepath);
    if (config.empty()) {
        return false;
    }
    
    clear();
    for (const auto& pair : config) {
        setEntryFromAny(pair.first, pair.second);
    }
    
    return true;
}

// Schema management
void Config::setSchema(const ConfigSchema& schema) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    pImpl->schema.reset(new ConfigSchema(schema));
}

const ConfigSchema* Config::getSchema() const {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    return pImpl->schema.get();
}

// Migration management
void Config::addMigrationStep(int fromVersion, int toVersion, const std::string& description,
                             const std::function<void(std::map<std::string, std::any>&)>& migrationLogic) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    pImpl->migration->addMigrationStep(fromVersion, toVersion, description, migrationLogic);
}

bool Config::applyMigrations() {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    std::vector<ConfigError> errors = pImpl->migration->applyMigrations(pImpl->configMap);
    
    if (!errors.empty()) {
        ConfigErrorReporter reporter;
        std::cout << reporter.reportValidationErrors(errors, pImpl->schema.get()) << std::endl;
    }
    
    return errors.empty();
}

int Config::getMigrationVersion() const {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    return pImpl->migration->getCurrentVersion();
}

void Config::setMigrationVersion(int version) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    pImpl->migration->setCurrentVersion(version);
}

// Documentation and help
std::string Config::getDocumentation() const {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    if (!pImpl->schema) {
        return "No schema defined.";
    }
    
    ConfigDocumentation doc;
    return doc.generateDocumentation(*pImpl->schema);
}

std::string Config::getHelp(const std::string& topic) const {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    if (topic == "presets") {
        ConfigHelpSystem help;
        return help.getSchemaHelp(*pImpl->schema);
    } else if (topic == "validate") {
        return "Use config::validate() to check for configuration errors.";
    } else if (topic == "migration") {
        return "Use config::applyMigrations() to apply pending migrations.";
    } else {
        return "Unknown help topic: " + topic + "\nAvailable topics: presets, validate, migration";
    }
}

// Configuration analysis
std::vector<std::string> Config::getConflictingParameters() const {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    if (!pImpl->schema) {
        return {};
    }
    
    ConfigValidator validator;
    return validator.getConflictingParameters(*pImpl->schema);
}

std::vector<std::string> Config::getDependencyCycles() const {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    if (!pImpl->schema) {
        return {};
    }
    
    ConfigDependencyAnalyzer analyzer;
    auto graph = analyzer.buildDependencyGraph(*pImpl->schema);
    return analyzer.findCycles(graph);
}

// Error reporting
std::string Config::reportValidationErrors() const {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    auto errors = validateConfig();
    if (errors.empty()) {
        return "Configuration is valid.";
    }
    
    ConfigErrorReporter reporter;
    return reporter.reportValidationErrors(errors, pImpl->schema.get());
}

// Original Config methods
bool Config::loadFromFile(const std::string& filepath) {
    // Try YAML first
    if (importFromYAML(filepath)) {
        return true;
    }
    
    // Try JSON
    if (importFromJSON(filepath)) {
        return true;
    }
    
    // Try simple key=value format
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "" << "\n";
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    std::any oldValue;
    if (it != pImpl->entries.end()) {
        oldValue = getAnyFromConfigValue(it->value);
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
        oldValue = std::any();
    }
    
    // Update configMap
    pImpl->configMap[key] = getAnyFromConfigValue(value);
    
    // Validate if enabled
    if (pImpl->validationEnabled && pImpl->schemaEnabled && pImpl->schema) {
        validateParameter(key, value);
    }
    
    // Notify watchers
    notifyConfigChange(key, oldValue, pImpl->configMap[key]);
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
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        std::any oldValue = getAnyFromConfigValue(it->value);
        
        pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
            pImpl->entries.end()
        );
        
        pImpl->configMap.erase(key);
        
        // Notify watchers
        notifyConfigChange(key, oldValue, std::any());
    }
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
    pImpl->entries.clear();
    pImpl->configMap.clear();
}

std::string Config::summary() const {
    std::lock_guard<std::shared_mutex> lock(pImpl->mutex);
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

std::any Config::getAnyFromConfigValue(const ConfigValue& value) const {
    std::any result;
    std::visit([&result](auto&& arg) {
        result = arg;
    }, value);
    return result;
}

void Config::validateSchema() const {
    if (!pImpl->schemaEnabled || !pImpl->schema) {
        return;
    }
    
    auto errors = validateConfig();
    for (const auto& error : errors) {
        // Create a minimal error for validation failure
        ConfigError validationError(ConfigErrorType::SchemaValidation, error.key, error.expected, error.actual, 
                                   "Schema validation failed: " + error.message);
        
        // Notify validation failure
        notifyValidationFailed(validationError);
    }
}

void Config::validateParameters() const {
    if (!pImpl->validationEnabled || !pImpl->schema) {
        return;
    }
    
    for (const auto& pair : pImpl->configMap) {
        const std::string& key = pair.first;
        const std::any& value = pair.second;
        
        // Find parameter in schema
        const ParameterInfo* param = pImpl->schema->getParameter(key);
        if (param) {
            validateParameter(key, value);
        }
    }
}

bool Config::isValueValid(const std::any& value, const ParameterInfo& param) const {
    ConfigValidationUtils utils;
    return utils.isValidValue(value, param);
}

void Config::validateParameter(const std::string& key, const std::any& value) {
    if (!pImpl->schema) {
        return;
    }
    
    const ParameterInfo* param = pImpl->schema->getParameter(key);
    if (!param) {
        return;
    }
    
    ConfigValidationUtils utils;
    if (!utils.isValidValue(value, *param)) {
        ConfigError error(ConfigErrorType::TypeMismatch, key, getExpectedTypeString(param->type), 
                         utils.anyToString(value), "Parameter '" + key + "' has invalid value");
        
        notifyValidationFailed(error);
    }
}

void Config::notifyConfigChange(const std::string& key, const std::any& oldValue, const std::any& newValue) {
    if (newValue.has_value()) {
        pImpl->watcher->notifyChange(key, oldValue, newValue);
    } else {
        pImpl->watcher->notifyRemove(key);
    }
}

void Config::notifyValidationFailed(const ConfigError& error) {
    ConfigChangeListener listener;
    pImpl->watcher->notifyValidationFailed(error);
}

std::vector<ConfigError> Config::validateConfig() const {
    std::vector<ConfigError> errors;
    
    if (!pImpl->schema) {
        return errors;
    }
    
    ConfigValidator validator;
    errors = validator.validate(pImpl->configMap, *pImpl->schema);
    
    return errors;
}

void Config::setEntryFromAny(const std::string& key, const std::any& value) {
    ConfigSource source = ConfigSource::File;
    std::string description;
    
    if (pImpl->schemaEnabled && pImpl->schema) {
        const ParameterInfo* param = pImpl->schema->getParameter(key);
        if (param) {
            description = param->description;
        }
    }
    
    // Convert any to ConfigValue
    ConfigValue configValue;
    if (value.type() == typeid(int)) {
        configValue = std::any_cast<int>(value);
    } else if (value.type() == typeid(int64_t)) {
        configValue = std::any_cast<int64_t>(value);
    } else if (value.type() == typeid(double)) {
        configValue = std::any_cast<double>(value);
    } else if (value.type() == typeid(bool)) {
        configValue = std::any_cast<bool>(value);
    } else if (value.type() == typeid(std::string)) {
        configValue = std::any_cast<std::string>(value);
    } else if (value.type() == typeid(std::vector<int>)) {
        configValue = std::any_cast<std::vector<int>>(value);
    } else if (value.type() == typeid(std::vector<double>)) {
        configValue = std::any_cast<std::vector<double>>(value);
    } else if (value.type() == typeid(std::vector<std::string>)) {
        configValue = std::any_cast<std::vector<std::string>>(value);
    }
    
    set(key, configValue, source);
}

std::map<std::string, std::any> Config::convertConfigToMap(const Config& config) const {
    std::map<std::string, std::any> result;
    
    auto keys = config.getKeys();
    for (const auto& key : keys) {
        std::any value;
        if (config.has<int>(key)) {
            value = config.get<int>(key).value();
        } else if (config.has<int64_t>(key)) {
            value = config.get<int64_t>(key).value();
        } else if (config.has<double>(key)) {
            value = config.get<double>(key).value();
        } else if (config.has<bool>(key)) {
            value = config.get<bool>(key).value();
        } else if (config.has<std::string>(key)) {
            value = config.get<std::string>(key).value();
        } else if (config.has<std::vector<int>>(key)) {
            value = config.get<std::vector<int>>(key).value();
        } else if (config.has<std::vector<double>>(key)) {
            value = config.get<std::vector<double>>(key).value();
        } else if (config.has<std::vector<std::string>>(key)) {
            value = config.get<std::vector<std::string>>(key).value();
        }
        
        if (value.has_value()) {
            result[key] = value;
        }
    }
    
    return result;
}

void Config::loadPreset(const std::vector<ConfigPresets::Preset>& presets) {
    clear();
    
    for (const auto& preset : presets) {
        for (const auto& pair : preset.values) {
            setEntryFromAny(pair.first, pair.second);
        }
    }
}

bool Config::exportToYAMLFile(const std::string& filepath, const std::map<std::string, std::any>& config) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration Export\n";
    file << "# Format: key: value\n\n";
    
    for (const auto& pair : config) {
        file << pair.first << ": ";
        exportAnyValue(file, pair.second);
        file << "\n";
    }
    
    return true;
}

bool Config::importFromYAMLFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::map<std::string, std::any> config;
    std::string line;
    std::string currentKey;
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            currentKey = trim(line.substr(0, colonPos));
            std::string value = trim(line.substr(colonPos + 1));
            
            if (currentKey.empty()) {
                continue;
            }
            
            config[currentKey] = parseAnyValue(value);
        }
    }
    
    pImpl->configMap = config;
    return true;
}

bool Config::exportToJSONFile(const std::string& filepath, const std::map<std::string, std::any>& config) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "{\n";
    bool first = true;
    for (const auto& pair : config) {
        if (!first) {
            file << ",\n";
        }
        first = false;
        
        file << "  \"" << pair.first << "\": ";
        exportAnyValue(file, pair.second);
    }
    file << "\n}\n";
    
    return true;
}

bool Config::importFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Simple JSON parsing - could be enhanced with a proper JSON library
    std::map<std::string, std::any> config;
    std::string line;
    std::string currentKey;
    bool inObject = false;
    
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) {
            continue;
        }
        
        // Skip object opening and closing
        if (line.find("{") != std::string::npos) {
            inObject = true;
            continue;
        }
        if (line.find("}") != std::string::npos) {
            inObject = false;
            continue;
        }
        
        if (!inObject) {
            continue;
        }
        
        // Parse key: value pairs
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            currentKey = trim(line.substr(0, colonPos));
            // Remove quotes from key
            if (currentKey.size() >= 2 && currentKey.front() == '\"' && currentKey.back() == '\"') {
                currentKey = currentKey.substr(1, currentKey.size() - 2);
            }
            
            std::string value = trim(line.substr(colonPos + 1));
            // Remove trailing comma
            if (!value.empty() && value.back() == ',') {
                value = value.substr(0, value.size() - 1);
            }
            
            if (currentKey.empty()) {
                continue;
            }
            
            config[currentKey] = parseAnyValue(value);
        }
    }
    
    pImpl->configMap = config;
    return true;
}

void Config::exportAnyValue(std::ostream& os, const std::any& value) const {
    if (value.type() == typeid(int)) {
        os << std::any_cast<int>(value);
    } else if (value.type() == typeid(int64_t)) {
        os << std::any_cast<int64_t>(value);
    } else if (value.type() == typeid(double)) {
        os << std::any_cast<double>(value);
    } else if (value.type() == typeid(float)) {
        os << std::any_cast<float>(value);
    } else if (value.type() == typeid(bool)) {
        os << (std::any_cast<bool>(value) ? "true" : "false");
    } else if (value.type() == typeid(std::string)) {
        os << "\"" << std::any_cast<std::string>(value) << "\"";
    } else if (value.type() == typeid(std::vector<int>)) {
        os << "[";
        const auto& vec = std::any_cast<std::vector<int>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) os << ", ";
            os << vec[i];
        }
        os << "]";
    } else if (value.type() == typeid(std::vector<double>)) {
        os << "[";
        const auto& vec = std::any_cast<std::vector<double>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) os << ", ";
            os << vec[i];
        }
        os << "]";
    } else if (value.type() == typeid(std::vector<std::string>)) {
        os << "[";
        const auto& vec = std::any_cast<std::vector<std::string>>(value);
        for (size_t i = 0; i < vec.size(); ++i) {
            if (i > 0) os << ", ";
            os << "\"" << vec[i] << "\"";
        }
        os << "]";
    } else {
        os << "null";
    }
}

std::any Config::parseAnyValue(const std::string& str) const {
    // Simple parsing - could be enhanced
    if (str == "true") {
        return true;
    } else if (str == "false") {
        return false;
    } else if (str.find('.') != std::string::npos) {
        try {
            return std::stod(str);
        } catch (...) {
            return str;
        }
    } else {
        try {
            return std::stoi(str);
        } catch (...) {
            return str;
        }
    }
}

std::string Config::getExpectedTypeString(ParameterType type) const {
    switch (type) {
        case ParameterType::Int: return "int";
        case ParameterType::Int64: return "int64_t";
        case ParameterType::Float: return "float";
        case ParameterType::Double: return "double";
        case ParameterType::Bool: return "bool";
        case ParameterType::String: return "string";
        case ParameterType::StringList: return "string[]";
        case ParameterType::IntList: return "int[]";
        case ParameterType::FloatList: return "float[]";
        case ParameterType::Choice: return "choice";
        case ParameterType::Range: return "range";
        case ParameterType::Vector2D: return "vector2d";
        case ParameterType::Vector3D: return "vector3d";
        case ParameterType::Matrix: return "matrix";
        case ParameterType::Custom: return "custom";
        default: return "unknown";
    }
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