// NLM - Enhanced Configuration System Implementation

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <ctime>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::function<bool(const ConfigValue&)>> validators;
    std::vector<ConfigurationTemplate> templates;
    std::map<std::string, std::vector<std::string>> keyToTags;
    std::map<std::string, std::string> keyToDescription;
    std::map<std::string, std::string> keyToHelpText;
    std::map<std::string, std::vector<std::string>> keyToTags;
    std::map<std::string, std::string> keyToHelpText;
    bool autoSave;
    std::string backupDirectory;
    size_t maxBackupFiles;
    std::map<std::string, std::chrono::system_clock::time_point> keyModificationTimes;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->autoSave = false;
    pImpl->backupDirectory = "backups";
    pImpl->maxBackupFiles = 10;
    
    // Create backup directory if it doesn't exist
    if (!std::filesystem::exists(pImpl->backupDirectory)) {
        std::filesystem::create_directories(pImpl->backupDirectory);
    }
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::string extension = extractExtension(filepath);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "json") {
        return loadFromJSON(filepath);
    } else if (extension == "yaml" || extension == "yml") {
        // TODO: Implement YAML support
        return false;
    } else if (extension == "toml") {
        // TODO: Implement TOML support
        return false;
    } else {
        // Simple key=value format
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
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
                
                set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::loadFromJSON(const std::string& jsonStr) {
    try {
        auto json = nlohmann::json::parse(jsonStr);
        return parseJSON(jsonStr, pImpl->entries);
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    bool loaded = false;
    
    for (const auto& env : std::filesystem::environment()) {
        std::string key = env.first;
        std::string value = env.second;
        
        if (key.rfind(prefix, 0) == 0) {
            std::string configKey = key.substr(prefix.length());
            set(configKey, value, ConfigSource::Environment, "From environment variable " + key);
            loaded = true;
        }
    }
    
    return loaded;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::string extension = extractExtension(filepath);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == "json") {
        return writeJSON(pImpl->entries, filepath);
    } else {
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
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
    auto value = get<T>(key);
    if (value) {
        return *value;
    }
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source,
                 const std::string& description, bool validate) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        if (!description.empty()) {
            it->description = description;
        }
        it->lastModified = std::chrono::system_clock::now();
    } else {
        ConfigEntry entry(key, value, source, description);
        entry.lastModified = std::chrono::system_clock::now();
        pImpl->entries.push_back(entry);
    }
    
    if (validate) {
        auto validation = validateKey(key);
        if (!validation.isValid) {
            for (const auto& error : validation.errors) {
                // Log error
            }
        }
    }
    
    if (pImpl->autoSave) {
        createBackup("".append(std::to_string(std::time(nullptr))).append(".backup"));
    }
}

void Config::set(const std::string& key, int value, ConfigSource source,
                 const std::string& description, bool validate) {
    set(key, ConfigValue(value), source, description, validate);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source,
                 const std::string& description, bool validate) {
    set(key, ConfigValue(value), source, description, validate);
}

void Config::set(const std::string& key, double value, ConfigSource source,
                 const std::string& description, bool validate) {
    set(key, ConfigValue(value), source, description, validate);
}

void Config::set(const std::string& key, bool value, ConfigSource source,
                 const std::string& description, bool validate) {
    set(key, ConfigValue(value), source, description, validate);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source,
                 const std::string& description, bool validate) {
    set(key, ConfigValue(value), source, description, validate);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source,
                 const std::string& description, bool validate) {
    set(key, ConfigValue(value), source, description, validate);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

bool Config::hasType(const std::string& key, ConfigValue::variant_type type) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return it->value.index() == type;
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end());
}

std::map<ConfigCategory, std::vector<std::string>> Config::getKeysByCategory() const {
    std::map<ConfigCategory, std::vector<std::string>> result;
    
    for (const auto& entry : pImpl->entries) {
        result[entry.category].push_back(entry.key);
    }
    
    return result;
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> result;
    for (const auto& entry : pImpl->entries) {
        result.push_back(entry.key);
    }
    return result;
}

std::vector<std::string> Config::getKeys(ConfigCategory category) const {
    std::vector<std::string> result;
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            result.push_back(entry.key);
        }
    }
    return result;
}

bool Config::addTemplate(const ConfigurationTemplate& template) {
    pImpl->templates.push_back(template);
    return true;
}

bool Config::removeTemplate(const std::string& name) {
    auto it = std::find_if(pImpl->templates.begin(), pImpl->templates.end(),
        [&name](const ConfigurationTemplate& t) { return t.name == name; });n    
    if (it == pImpl->templates.end()) {
        return false;
    }
    
    pImpl->templates.erase(it);
    return true;
}

std::optional<ConfigurationTemplate> Config::getTemplate(const std::string& name) const {
    auto it = std::find_if(pImpl->templates.begin(), pImpl->templates.end(),
        [&name](const ConfigurationTemplate& t) { return t.name == name; });n    
    if (it == pImpl->templates.end()) {
        return std::nullopt;
    }
    
    return *it;
}

std::vector<ConfigurationTemplate> Config::getTemplates(ConfigCategory category) const {
    std::vector<ConfigurationTemplate> result;
    for (const auto& template : pImpl->templates) {
        if (template.category == category) {
            result.push_back(template);
        }
    }
    return result;
}

bool Config::applyTemplate(const std::string& templateName, ConfigSource source) {
    auto templateOpt = getTemplate(templateName);
    if (!templateOpt) {
        return false;
    }
    
    const auto& template = *templateOpt;
    bool success = true;
    
    for (const auto& pair : template.values) {
        set(pair.first, pair.second, source);
        if (!validateKey(pair.first).isValid) {
            success = false;
        }
    }
    
    return success;
}

ValidationResult Config::validate(const std::string& key) const {
    ValidationResult result;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        result.addError("Key not found");
        return result;
    }
    
    // Apply custom validator if exists
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt != pImpl->validators.end()) {
        if (!validatorIt->second(it->value)) {
            result.addError("Custom validation failed for key: " + key);
        }
    }
    
    return result;
}

ValidationResult Config::validateAll() const {
    ValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult entryValidation = validate(entry.key);
        if (!entryValidation.isValid) {
            for (const auto& error : entryValidation.errors) {
                result.addError("Key '" + entry.key + "': " + error);
            }
        }
    }
    
    return result;
}

void Config::setValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator) {
    pImpl->validators[key] = validator;
}

ValidationResult Config::getValidationResult(const std::string& key) const {
    return validate(key);
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->validators.clear();
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "Configuration Summary (" << pImpl->entries.size() << " entries)" << std::endl;
    ss << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        ss << "  " << entry.key << " ("
           << entry.getCategoryName() << ", "
           << entry.getSourceName() << ")"
           << std::endl;
        ss << "    Description: " << entry.description << std::endl;
        ss << "    Value: " << "PLACEHOLDER_VALUE" << std::endl;
        ss << std::endl;
    }
    
    return ss.str();
}

std::optional<ConfigEntry> Config::getEntry(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return *it;
}

std::vector<ConfigurationTemplate> Config::getCommonPresets() {
    std::vector<ConfigurationTemplate> presets;
    
    // Minimal preset
    std::vector<std::pair<std::string, ConfigValue>> minimalValues = {
        {"brain.neuron_count", 100},
        {"brain.connection_probability", 0.05},
        {"plasticity.stdp.enable", true},
        {"plasticity.hebbian.enable", false}
    };
    presets.push_back(ConfigurationTemplate("Minimal", "Minimal configuration for quick testing",
        minimalValues, ConfigCategory::General, false));
    
    // Balanced preset
    std::vector<std::pair<std::string, ConfigValue>> balancedValues = {
        {"brain.neuron_count", 1000},
        {"brain.connection_probability", 0.1},
        {"plasticity.stdp.enable", true},
        {"plasticity.hebbian.enable", true},
        {"plasticity.structural.enable", true},
        {"memory.enable", true},
        {"neuromodulation.enable", true}
    };
    presets.push_back(ConfigurationTemplate("Balanced", "Balanced configuration for general use",
        balancedValues, ConfigCategory::General, false));
    
    // High performance preset
    std::vector<std::pair<std::string, ConfigValue>> performanceValues = {
        {"brain.neuron_count", 5000},
        {"brain.connection_probability", 0.02},
        {"plasticity.stdp.enable", true},
        {"plasticity.hebbian.enable", false},
        {"performance.optimization", "aggressive"},
        {"memory.enable", false},  // Disable memory for performance
        {"checkpoint_count", 5},
        {"simulation_timestep", 0.0001}
    };
    presets.push_back(ConfigurationTemplate("HighPerformance", "High performance configuration for simulations",
        performanceValues, ConfigCategory::Performance, false));
    
    // Research preset
    std::vector<std::pair<std::string, ConfigValue>> researchValues = {
        {"brain.neuron_count", 10000},
        {"brain.connection_probability", 0.15},
        {"plasticity.stdp.enable", true},
        {"plasticity.hebbian.enable", true},
        {"plasticity.structural.enable", true},
        {"memory.enable", true},
        {"memory.episodic_max_episodes", 10000},
        {"neuromodulation.enable", true},
        {"neuromodulation.curiosity.enable", true},
        {"neuromodulation.novelty.enable", true},
        {"development.enable", true},
        {"checkpoint_count", 20},
        {"enable_replay", true}
    };
    presets.push_back(ConfigurationTemplate("Research", "Research-oriented configuration for studying learning",
        researchValues, ConfigCategory::General, true));
    
    return presets;
}

ConfigurationTemplate Config::getMinimalPreset() {
    auto presets = getCommonPresets();
    auto it = std::find_if(presets.begin(), presets.end(),
        [](const ConfigurationTemplate& t) { return t.name == "Minimal"; });n    
    if (it != presets.end()) {
        return *it;
    }
    
    return ConfigurationTemplate("Minimal", "Minimal configuration for quick testing", {}, ConfigCategory::General);
}

ConfigurationTemplate Config::getBalancedPreset() {
    auto presets = getCommonPresets();
    auto it = std::find_if(presets.begin(), presets.end(),
        [](const ConfigurationTemplate& t) { return t.name == "Balanced"; });n    
    if (it != presets.end()) {
        return *it;
    }
    
    return ConfigurationTemplate("Balanced", "Balanced configuration for general use", {}, ConfigCategory::General);
}

ConfigurationTemplate Config::getHighPerformancePreset() {
    auto presets = getCommonPresets();
    auto it = std::find_if(presets.begin(), presets.end(),
        [](const ConfigurationTemplate& t) { return t.name == "HighPerformance"; });n    
    if (it != presets.end()) {
        return *it;
    }
    
    return ConfigurationTemplate("HighPerformance", "High performance configuration for simulations", {}, ConfigCategory::Performance);
}

ConfigurationTemplate Config::getResearchPreset() {
    auto presets = getCommonPresets();
    auto it = std::find_if(presets.begin(), presets.end(),
        [](const ConfigurationTemplate& t) { return t.name == "Research"; });n    
    if (it != presets.end()) {
        return *it;
    }
    
    return ConfigurationTemplate("Research", "Research-oriented configuration for studying learning", {}, ConfigCategory::General);
}

std::string Config::getConfigHelp(const std::string& key) {
    if (key.empty()) {
        std::string help = "NLM Configuration Options:\n\n";
        help += "Common Categories:\n";
        help += "  brain - Brain configuration options\n";
        help += "  memory - Memory system configuration\n";
        help += "  neuromodulation - Neuromodulation system configuration\n";
        help += "  prediction - Prediction system configuration\n";
        help += "  cognition - Cognition system configuration\n";
        help += "  development - Development system configuration\n";
        help += "  plasticity - Plasticity rules configuration\n";
        help += "  performance - Performance optimization options\n";
        help += "  debugging - Debugging and testing options\n";
        help += "  environment - Environment configuration\n\n";
        help += "For detailed help on specific keys, specify the key name.\n";
        return help;
    }
    
    // TODO: Return help for specific key
    return "Help for key '" + key + "': Not yet implemented.";
}

std::vector<std::string> Config::getConfigCategories() {
    std::vector<std::string> categories;
    categories.push_back("brain");
    categories.push_back("memory");
    categories.push_back("neuromodulation");
    categories.push_back("prediction");
    categories.push_back("cognition");
    categories.push_back("development");
    categories.push_back("plasticity");
    categories.push_back("performance");
    categories.push_back("debugging");
    categories.push_back("environment");
    return categories;
}

std::vector<std::string> Config::getConfigKeys(ConfigCategory category) {
    std::vector<std::string> keys;
    // TODO: Implement based on category
    return keys;
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getEntryCount(ConfigSource source) const {
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            count++;
        }
    }
    return count;
}

size_t Config::getEntryCount(ConfigCategory category) const {
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (entry.category == category) {
            count++;
        }
    }
    return count;
}

bool Config::hasSameValues(const Config& other, const std::vector<std::string>& keys) const {
    if (keys.empty()) {
        return pImpl->entries.size() == other.pImpl->entries.size();
    }
    
    for (const auto& key : keys) {
        auto thisEntry = getEntry(key);
        auto otherEntry = other.getEntry(key);
        
        if (!thisEntry || !otherEntry) {
            return false;
        }
        
        if (thisEntry->value != otherEntry->value) {
            return false;
        }
    }
    
    return true;
}

void Config::merge(const Config& other, ConfigSource source) {
    for (const auto& entry : other.pImpl->entries) {
        set(entry.key, entry.value, source);
        
        // Copy additional metadata
        if (entry.validator) {
            setValidator(entry.key, entry.validator);
        }
    }
}

bool Config::exportToJSON(const std::string& filepath, const std::vector<std::string>& keys) const {
    try {
        nlohmann::json json;
        
        if (keys.empty()) {
            // Export all entries
            for (const auto& entry : pImpl->entries) {
                json[entry.key] = "PLACEHOLDER_VALUE";  // TODO: Convert actual value
            }
        } else {
            // Export only specified keys
            for (const auto& key : keys) {
                auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                    [&key](const ConfigEntry& e) { return e.key == key; });n                
                if (it != pImpl->entries.end()) {
                    json[key] = "PLACEHOLDER_VALUE";
                }
            }
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << json.dump(2);
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::importFromJSON(const std::string& filepath, ConfigSource source) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        nlohmann::json json = nlohmann::json::parse(file);
        
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            // TODO: Convert JSON value to ConfigValue
            set(key, ConfigValue(std::string("PLACEHOLDER")), source);
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::createBackup(const std::string& filepath) const {
    try {
        std::filesystem::path backupPath(pImpl->backupDirectory);
        backupPath /= filepath;
        
        std::ofstream file(backupPath);
        if (!file.is_open()) {
            return false;
        }
        
        // Export as JSON
        return exportToJSON(backupPath.string());
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::restoreFromBackup(const std::string& filepath) {
    try {
        std::filesystem::path backupPath(pImpl->backupDirectory);
        backupPath /= filepath;
        
        if (!std::filesystem::exists(backupPath)) {
            return false;
        }
        
        return importFromJSON(backupPath.string(), ConfigSource::Imported);
        
    } catch (const std::exception& e) {
        return false;
    }
}

std::chrono::system_clock::time_point Config::getLastModified(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        return std::chrono::system_clock::time_point();
    }
    
    return it->lastModified;
}

std::vector<std::string> Config::getModifiedKeys(const std::chrono::system_clock::time_point& since) const {
    std::vector<std::string> result;
    
    for (const auto& entry : pImpl->entries) {
        if (entry.lastModified > since) {
            result.push_back(entry.key);
        }
    }
    
    return result;
}

double Config::getMemoryUsage() const {
    double memoryUsage = sizeof(Config::Impl);
    
    for (const auto& entry : pImpl->entries) {
        memoryUsage += sizeof(ConfigEntry);
        memoryUsage += entry.key.size();
        // TODO: Add actual value size
    }
    
    for (const auto& validator : pImpl->validators) {
        memoryUsage += sizeof(std::function<bool(const ConfigValue&)>);
    }
    
    for (const auto& template : pImpl->templates) {
        memoryUsage += sizeof(ConfigurationTemplate);
    }
    
    return memoryUsage;
}

// Validation helper
ValidationResult Config::validateKey(const std::string& key) const {
    ValidationResult result;
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        result.addError("Key not found: " + key);
        return result;
    }
    
    // Basic validation based on key patterns
    if (key.find("brain.") == 0) {
        // Brain configuration validation
        if (key == "brain.neuron_count") {
            auto valueOpt = get<int64_t>(key);
            if (valueOpt && (*valueOpt < 1 || *valueOpt > 100000)) {
                result.addError("brain.neuron_count must be between 1 and 100000");
            }
        } else if (key == "brain.connection_probability") {
            auto valueOpt = get<double>(key);
            if (valueOpt && (*valueOpt < 0.0 || *valueOpt > 1.0)) {
                result.addError("brain.connection_probability must be between 0.0 and 1.0");
            }
        }
    } else if (key.find("plasticity.") == 0) {
        // Plasticity configuration validation
        // ...
    }
    
    return result;
}

// Static helper methods
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");
    
    if (start == std::string::npos || end == std::string::npos) {
        return "";
    }
    
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

bool Config::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

std::string Config::extractExtension(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    return filename.substr(pos + 1);
}

std::string Config::getFileContent(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool Config::parseJSON(const std::string& jsonStr, std::map<std::string, ConfigValue>& outValues) {
    try {
        auto json = nlohmann::json::parse(jsonStr);
        
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            ConfigValue value;
            
            if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_unsigned()) {
                value = it->get<uint64_t>();
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_array()) {
                std::vector<double> array;
                for (const auto& item : *it) {
                    array.push_back(item.get<double>());
                }
                value = array;
            }
            
            outValues[key] = value;
        }
        
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::writeJSON(const std::map<std::string, ConfigEntry>& entries, const std::string& filepath) {
    try {
        nlohmann::json json;
        
        for (const auto& pair : entries) {
            json[pair.first] = "PLACEHOLDER_VALUE";  // TODO: Convert actual value
        }
        
        std::ofstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        file << json.dump(2);
        return true;
        
    } catch (const std::exception& e) {
        return false;
    }
}

nlohmann::json Config::configValueToJSON(const ConfigValue& value) {
    return "PLACEHOLDER_JSON";  // TODO: Implement proper conversion
}

bool Config::jsonToConfigValue(const nlohmann::json& json, ConfigValue& value) {
    // TODO: Implement proper conversion
    return false;
}

} // namespace nlm
