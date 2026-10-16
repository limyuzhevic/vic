#include "ConfigManager.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <filesystem>

namespace nlm {

ConfigManager::ConfigManager() : importFormat(ImportFormat::JSON), 
    autoBackup(true), backupRetentionDays(30), historyEnabled(true), maxHistorySize(100),
    isModified(false) {
    // Create default config directory if it doesn't exist
    configDir = "./configs";
    if (!std::filesystem::exists(configDir)) {
        std::filesystem::create_directories(configDir);
    }
    
    // Setup default sections
    setupBrainConfiguration();
    setupMemoryConfiguration();
    setupPlasticityConfiguration();
    setupDevelopmentConfiguration();
    setupNeuromodulationConfiguration();
    setupPerformanceConfiguration();
    setupOutputConfiguration();
    setupDebugConfiguration();
    setupLoggingConfiguration();
    setupExperimentConfiguration();
}

ConfigSection& ConfigManager::getSection(const std::string& name) {
    auto it = configSections.find(name);
    if (it == configSections.end()) {
        createSection(name);
        it = configSections.find(name);
    }
    markAsModified();
    return it->second;
}

const ConfigSection& ConfigManager::getSection(const std::string& name) const {
    static const ConfigSection emptySection("");
    auto it = configSections.find(name);
    if (it == configSections.end()) {
        return emptySection;
    }
    return it->second;
}

bool ConfigManager::hasSection(const std::string& name) const {
    return configSections.find(name) != configSections.end();
}

void ConfigManager::createSection(const std::string& name) {
    configSections[name] = ConfigSection(name);
    markAsModified();
}

void ConfigManager::removeSection(const std::string& name) {
    auto it = configSections.find(name);
    if (it != configSections.end()) {
        configSections.erase(it);
        markAsModified();
    }
}

std::vector<std::string> ConfigManager::getSectionNames() const {
    std::vector<std::string> names;
    for (const auto& pair : configSections) {
        names.push_back(pair.first);
    }
    return names;
}

template<typename T>
void ConfigManager::setValue(const std::string& section, const std::string& key, const T& value) {
    ConfigSection& sec = getSection(section);
    sec.values[key] = convertToConfigValue(value);
}

template<typename T>
T ConfigManager::getValue(const std::string& section, const std::string& key, const T& defaultValue) const {
    auto it = configSections.find(section);
    if (it == configSections.end()) {
        return defaultValue;
    }
    
    const auto& values = it->second.values;
    auto valueIt = values.find(key);
    if (valueIt == values.end()) {
        return defaultValue;
    }
    
    return std::any_cast<T>(valueIt->second);
}

ConfigValue ConfigManager::getValue(const std::string& section, const std::string& key) const {
    auto it = configSections.find(section);
    if (it == configSections.end()) {
        throw std::runtime_error("Section not found: " + section);
    }
    
    const auto& values = it->second.values;
    auto valueIt = values.find(key);
    if (valueIt == values.end()) {
        throw std::runtime_error("Key not found in section " + section + ": " + key);
    }
    
    return valueIt->second;
}

bool ConfigManager::hasValue(const std::string& section, const std::string& key) const {
    auto it = configSections.find(section);
    if (it == configSections.end()) {
        return false;
    }
    
    return it->second.values.find(key) != it->second.values.end();
}

void ConfigManager::removeValue(const std::string& section, const std::string& key) {
    auto it = configSections.find(section);
    if (it != configSections.end()) {
        auto valueIt = it->second.values.find(key);
        if (valueIt != it->second.values.end()) {
            it->second.values.erase(valueIt);
            markAsModified();
        }
    }
}

ConfigSection& ConfigManager::getSubsection(const std::string& parentSection, const std::string& subsectionName) {
    ConfigSection& parent = getSection(parentSection);
    auto it = parent.subsections.find(subsectionName);
    if (it == parent.subsections.end()) {
        parent.subsections[subsectionName] = ConfigSection(subsectionName);
    }
    markAsModified();
    return parent.subsections[subsectionName];
}

bool ConfigManager::hasSubsection(const std::string& parentSection, const std::string& subsectionName) const {
    auto it = configSections.find(parentSection);
    if (it == configSections.end()) {
        return false;
    }
    return it->second.subsections.find(subsectionName) != it->second.subsections.end();
}

void ConfigManager::createSubsection(const std::string& parentSection, const std::string& subsectionName) {
    ConfigSection& parent = getSection(parentSection);
    if (!hasSubsection(parentSection, subsectionName)) {
        parent.subsections[subsectionName] = ConfigSection(subsectionName);
        markAsModified();
    }
}

void ConfigManager::setDefaultValue(const std::string& section, const std::string& key, const ConfigValue& value) {
    auto it = defaultSections.find(section);
    if (it == defaultSections.end()) {
        createSection(section);
        it = defaultSections.find(section);
    }
    it->second.values[key] = value;
}

void ConfigManager::loadDefaults() {
    clear();
    
    for (const auto& [sectionName, defaultSection] : defaultSections) {
        configSections[sectionName] = defaultSection;
    }
}

bool ConfigManager::importFromFile(const std::string& filePath, ImportFormat format,
                                   MergeStrategy strategy) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        errors.push_back("Cannot open file for import: " + filePath);
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    return importFromString(buffer.str(), format, strategy);
}

bool ConfigManager::importFromString(const std::string& content, ImportFormat format,
                                     MergeStrategy strategy) {
    // For now, use JSON for all formats (would need YAML/XML parsers)
    if (format != ImportFormat::JSON) {
        errors.push_back("Unsupported import format: " + std::to_string(static_cast<int>(format)));
        return false;
    }
    
    try {
        auto jsonData = json::parse(content);
        
        // Clear current config if replacing
        if (strategy == MergeStrategy::REPLACE) {
            clear();
        }
        
        // Import JSON data
        importFromJson(jsonData, strategy);
        
        markAsModified();
        return true;
    } catch (const json::parse_error& e) {
        errors.push_back("JSON parse error: " + std::string(e.what()));
        return false;
    }
}

void ConfigManager::importFromJson(const json& jsonData, MergeStrategy strategy) {
    for (const auto& [sectionName, sectionData] : jsonData.items()) {
        if (sectionData.is_object()) {
            ConfigSection& configSection = getSection(sectionName);
            
            for (const auto& [key, value] : sectionData.items()) {
                if (strategy == MergeStrategy::INHERIT && hasValue(sectionName, key)) {
                    continue; // Don't inherit if already set
                }
                
                if (strategy == MergeStrategy::OVERRIDE || 
                    !hasValue(sectionName, key) || 
                    strategy == MergeStrategy::REPLACE) {
                    configSection.values[key] = jsonValueToConfigValue(value);
                }
            }
        }
    }
}

bool ConfigManager::exportToFile(const std::string& filePath, ImportFormat format) const {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        errors.push_back("Cannot write to file: " + filePath);
        return false;
    }
    
    std::string content = exportToString(format);
    file << content;
    return true;
}

std::string ConfigManager::exportToString(ImportFormat format) const {
    json jsonData;
    
    for (const auto& [sectionName, section] : configSections) {
        jsonData[sectionName] = configSectionToJson(section);
    }
    
    // For now, only support JSON output
    if (format != ImportFormat::JSON) {
        errors.push_back("Unsupported export format: " + std::to_string(static_cast<int>(format)));
        return "{}";
    }
    
    return jsonData.dump(2);
}

json ConfigManager::configSectionToJson(const ConfigSection& section) const {
    json jsonSection;
    
    for (const auto& [key, value] : section.values) {
        jsonSection[key] = configValueToJson(value);
    }
    
    for (const auto& [subSectionName, subSection] : section.subsections) {
        jsonSection[subSectionName] = configSectionToJson(subSection);
    }
    
    return jsonSection;
}

json ConfigManager::configValueToJson(const ConfigValue& value) const {
    return std::visit([&](const auto& val) -> json {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return val;
        } else if constexpr (std::is_same_v<T, int>) {
            return val;
        } else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
            return val;
        } else if constexpr (std::is_same_v<T, bool>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<float>>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return val;
        } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
            return val;
        }
        return "";
    }, value);
}

ConfigValue ConfigManager::jsonValueToConfigValue(const json& jsonValue) const {
    ConfigValue configValue;
    
    if (jsonValue.is_string()) {
        configValue = jsonValue.get<std::string>();
    } else if (jsonValue.is_number_integer()) {
        configValue = jsonValue.get<int>();
    } else if (jsonValue.is_number_float()) {
        configValue = jsonValue.get<float>();
    } else if (jsonValue.is_boolean()) {
        configValue = jsonValue.get<bool>();
    } else if (jsonValue.is_array()) {
        // Determine array type based on first element
        if (!jsonValue.empty()) {
            if (jsonValue[0].is_string()) {
                std::vector<std::string> vec = jsonValue.get<std::vector<std::string>>();
                configValue = vec;
            } else if (jsonValue[0].is_number_integer()) {
                std::vector<int> vec = jsonValue.get<std::vector<int>>();
                configValue = vec;
            } else if (jsonValue[0].is_number_float()) {
                std::vector<float> vec = jsonValue.get<std::vector<float>>();
                configValue = vec;
            } else if (jsonValue[0].is_boolean()) {
                std::vector<bool> vec = jsonValue.get<std::vector<bool>>();
                configValue = vec;
            } else {
                configValue = std::vector<std::string>();
            }
        } else {
            configValue = std::vector<std::string>();
        }
    }
    
    return configValue;
}

ConfigValue ConfigManager::convertToConfigValue(const std::any& value) const {
    if (value.type() == typeid(std::string)) {
        return std::any_cast<std::string>(value);
    } else if (value.type() == typeid(int)) {
        return std::any_cast<int>(value);
    } else if (value.type() == typeid(float)) {
        return std::any_cast<float>(value);
    } else if (value.type() == typeid(double)) {
        return std::any_cast<double>(value);
    } else if (value.type() == typeid(bool)) {
        return std::any_cast<bool>(value);
    } else if (value.type() == typeid(std::vector<std::string>)) {
        return std::any_cast<std::vector<std::string>>(value);
    } else if (value.type() == typeid(std::vector<int>)) {
        return std::any_cast<std::vector<int>>(value);
    } else if (value.type() == typeid(std::vector<float>)) {
        return std::any_cast<std::vector<float>>(value);
    } else if (value.type() == typeid(std::vector<double>)) {
        return std::any_cast<std::vector<double>>(value);
    } else if (value.type() == typeid(std::vector<bool>)) {
        return std::any_cast<std::vector<bool>>(value);
    }
    
    return std::string();
}

std::any ConfigManager::convertFromConfigValue(const ConfigValue& value) const {
    return std::visit([](const auto& val) -> std::any {
        return val;
    }, value);
}

void ConfigManager::addDefaultSection(const std::string& name, const ConfigSection& section) {
    defaultSections[name] = section;
}

std::string ConfigManager::configValueToString(const ConfigValue& value) const {
    return std::visit([](const auto& val) -> std::string {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return val;
        } else if constexpr (std::is_same_v<T, int>) {
            return std::to_string(val);
        } else if constexpr (std::is_same_v<T, float> || std::is_same_v<T, double>) {
            return std::to_string(val);
        } else if constexpr (std::is_same_v<T, bool>) {
            return val ? "true" : "false";
        } else {
            return "";
        }
    }, value);
}

bool ConfigManager::stringToConfigValue(const std::string& str, ConfigValue& outValue) const {
    // Try to parse as different types in order
    try {
        size_t pos;
        int intVal = std::stoi(str);
        outValue = intVal;
        return true;
    } catch (...) {}
    
    try {
        size_t pos;
        float floatVal = std::stof(str);
        outValue = floatVal;
        return true;
    } catch (...) {}
    
    if (str == "true" || str == "1") {
        outValue = true;
        return true;
    } else if (str == "false" || str == "0") {
        outValue = false;
        return true;
    } else if (str.front() == '"' && str.back() == '"') {
        outValue = str.substr(1, str.length() - 2);
        return true;
    }
    
    return false;
}

void ConfigManager::setInheritance(const std::string& section, const InheritanceInfo& inheritance) {
    inheritanceConfigs[section] = inheritance;
}

bool ConfigManager::applyInheritance(const std::string& section) {
    auto it = inheritanceConfigs.find(section);
    if (it == inheritanceConfigs.end()) {
        return true; // No inheritance configured
    }
    
    const auto& inheritance = it->second;
    
    // Apply inheritance logic
    // This would need to be implemented based on the specific requirements
    return true;
}

void ConfigManager::addValidator(std::function<bool(const ConfigSection&)> validator) {
    validators.push_back(validator);
}

void ConfigManager::addValueValidator(std::function<bool(const std::string&, const ConfigValue&)> validator) {
    valueValidators.push_back(validator);
}

ValidationResult ConfigManager::validate() const {
    ValidationResult result;
    
    for (const auto& validator : validators) {
        for (const auto& [sectionName, section] : configSections) {
            if (!validator(section)) {
                result.valid = false;
                result.errors.push_back("Validation failed for section: " + sectionName);
            }
        }
    }
    
    for (const auto& [sectionName, section] : configSections) {
        for (const auto& [key, value] : section.values) {
            for (const auto& validator : valueValidators) {
                if (!validator(sectionName + "." + key, value)) {
                    result.valid = false;
                    result.errors.push_back("Validation failed for key: " + sectionName + "." + key);
                }
            }
        }
    }
    
    return result;
}

ValidationResult ConfigManager::validateSection(const std::string& section) const {
    ValidationResult result;
    auto it = configSections.find(section);
    if (it == configSections.end()) {
        result.valid = false;
        result.errors.push_back("Section not found: " + section);
        return result;
    }
    
    const auto& sec = it->second;
    for (const auto& validator : validators) {
        if (!validator(sec)) {
            result.valid = false;
            result.errors.push_back("Validation failed for section: " + section);
        }
    }
    
    return result;
}

ValidationResult ConfigManager::validateValue(const std::string& section, const std::string& key) const {
    ValidationResult result;
    auto sectionIt = configSections.find(section);
    if (sectionIt == configSections.end()) {
        result.valid = false;
        result.errors.push_back("Section not found: " + section);
        return result;
    }
    
    const auto& sec = sectionIt->second;
    auto valueIt = sec.values.find(key);
    if (valueIt == sec.values.end()) {
        result.valid = false;
        result.errors.push_back("Key not found in section " + section + ": " + key);
        return result;
    }
    
    for (const auto& validator : valueValidators) {
        if (!validator(section + "." + key, valueIt->second)) {
            result.valid = false;
            result.errors.push_back("Validation failed for key: " + section + "." + key);
        }
    }
    
    return result;
}

bool ConfigManager::saveSession(const std::string& sessionPath) const {
    // Save current configuration as a session
    std::ofstream file(sessionPath);
    if (!file.is_open()) {
        errors.push_back("Cannot write session file: " + sessionPath);
        return false;
    }
    
    json sessionJson;
    sessionJson["config"] = exportToString(ImportFormat::JSON);
    sessionJson["timestamp"] = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    
    // Add metadata
    json metadata;
    metadata["sections"] = getSectionNames();
    metadata["valueCount"] = getValueCount();
    metadata["modified"] = isModified();
    metadata["lastChange"] = std::chrono::system_clock::to_time_t(getLastChange());
    
    sessionJson["metadata"] = metadata;
    
    file << sessionJson.dump(2);
    return true;
}

bool ConfigManager::loadSession(const std::string& sessionPath) {
    std::ifstream file(sessionPath);
    if (!file.is_open()) {
        errors.push_back("Cannot open session file: " + sessionPath);
        return false;
    }
    
    try {
        auto sessionJson = json::parse(file);
        
        // Load configuration from session
        if (sessionJson.contains("config")) {
            std::string configStr = sessionJson["config"];
            importFromString(configStr, ImportFormat::JSON, MergeStrategy::REPLACE);
        }
        
        // Load metadata (for info only)
        if (sessionJson.contains("metadata")) {
            // Metadata can be used for logging/info display
        }
        
        markAsModified();
        return true;
    } catch (const json::parse_error& e) {
        errors.push_back("Session parse error: " + std::string(e.what()));
        return false;
    }
}

bool ConfigManager::compareSessions(const std::string& sessionPath1, const std::string& sessionPath2) const {
    // Compare two session files and report differences
    auto session1 = loadSessionFromFile(sessionPath1);
    auto session2 = loadSessionFromFile(sessionPath2);
    
    // This would compare the JSON structures and report differences
    // For simplicity, just compare checksums
    std::string checksum1 = generateSessionChecksum(sessionPath1);
    std::string checksum2 = generateSessionChecksum(sessionPath2);
    
    if (checksum1 != checksum2) {
        warnings.push_back("Sessions differ: " + sessionPath1 + " vs " + sessionPath2);
    }
    
    return checksum1 == checksum2;
}

std::string ConfigManager::generateSessionChecksum(const std::string& sessionPath) const {
    // Generate a checksum for a session file
    std::ifstream file(sessionPath);
    if (!file.is_open()) {
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    // Simple checksum (in real implementation, use cryptographic hash)
    return std::to_string(std::hash<std::string>{}(buffer.str()));
}

void ConfigManager::saveToHistory() {
    if (!historyEnabled) return;
    
    if (configHistory.size() >= maxHistorySize) {
        configHistory.erase(configHistory.begin());
    }
    
    // Create a copy of current configuration
    ConfigSection historyCopy;
    for (const auto& [sectionName, section] : configSections) {
        historyCopy[sectionName] = section;
    }
    
    configHistory.push_back(historyCopy);
}

bool ConfigManager::restoreFromHistory(size_t index) {
    if (index >= configHistory.size()) {
        errors.push_back("Invalid history index: " + std::to_string(index));
        return false;
    }
    
    // Clear current config and restore from history
    clear();
    configSections = configHistory[index];
    markAsModified();
    
    return true;
}

void ConfigManager::clear() {
    configSections.clear();
    markAsModified();
}

void ConfigManager::resetToDefaults() {
    loadDefaults();
    markAsModified();
}

void ConfigManager::markAsModified() {
    isModified = true;
    lastChange = std::chrono::system_clock::now();
    
    if (historyEnabled) {
        saveToHistory();
    }
    
    if (autoBackup) {
        // Create backup
        std::time_t now = std::time(nullptr);
        std::string backupPath = backupDirectory + "/config_backup_" + 
                                std::to_string(now) + ".json";
        saveConfig(backupPath);
        
        // Cleanup old backups
        cleanupBackups();
    }
}

void ConfigManager::cleanupBackups() {
    if (!std::filesystem::exists(backupDirectory)) return;
    
    std::time_t now = std::time(nullptr);
    std::chrono::system_clock::time_point now_tp = std::chrono::system_clock::now();
    
    for (const auto& entry : std::filesystem::directory_iterator(backupDirectory)) {
        if (entry.path().extension() == ".json") {
            auto fileTime = std::filesystem::last_write_time(entry.path());
            auto age = now_tp - fileTime;
            auto ageDays = std::chrono::duration_cast<std::chrono::hours>(age).count() / 24;
            
            if (ageDays > backupRetentionDays) {
                std::filesystem::remove(entry.path());
            }
        }
    }
}

size_t ConfigManager::getValueCount() const {
    size_t count = 0;
    for (const auto& [sectionName, section] : configSections) {
        count += section.values.size();
        for (const auto& [subSectionName, subSection] : section.subsections) {
            count += getSubsectionValueCount(subSection);
        }
    }
    return count;
}

size_t ConfigManager::getSubsectionValueCount(const ConfigSection& section) const {
    size_t count = 0;
    for (const auto& [key, value] : section.values) {
        count++;
    }
    for (const auto& [subSectionName, subSection] : section.subsections) {
        count += getSubsectionValueCount(subSection);
    }
    return count;
}

bool ConfigManager::saveConfig(const std::string& path) const {
    return exportToFile(path, ImportFormat::JSON);
}

bool ConfigManager::loadConfig(const std::string& path) {
    return importFromFile(path, ImportFormat::JSON, MergeStrategy::MERGE);
}

bool ConfigManager::listConfigs() const {
    std::cout << "Configuration sections:" << std::endl;
    for (const auto& sectionName : getSectionNames()) {
        std::cout << "  " << sectionName << std::endl;
    }
    
    std::cout << std::endl << "Configuration values:" << std::endl;
    for (const auto& [sectionName, section] : configSections) {
        std::cout << "  [" << sectionName << "]" << std::endl;
        for (const auto& [key, value] : section.values) {
            std::cout << "    " << key << " = " << configValueToString(value) << std::endl;
        }
    }
    
    return true;
}

bool ConfigManager::validateConfig() const {
    ValidationResult result = validate();
    if (result.valid) {
        std::cout << "Configuration is valid." << std::endl;
        return true;
    } else {
        std::cout << "Configuration validation errors:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  ERROR: " << error << std::endl;
        }
        for (const auto& warning : result.warnings) {
            std::cout << "  WARNING: " << warning << std::endl;
        }
        return false;
    }
}

bool ConfigManager::mergeConfig(const std::string& sourcePath) {
    return importFromFile(sourcePath, ImportFormat::JSON, MergeStrategy::MERGE);
}

bool ConfigManager::diffConfig(const std::string& sourcePath) const {
    // Compare current config with source file
    auto originalConfig = configSections;
    bool loaded = importFromFile(sourcePath, ImportFormat::JSON, MergeStrategy::REPLACE);
    if (!loaded) return false;
    
    // Compare and show differences
    std::cout << "Configuration differences:" << std::endl;
    
    // This would show added, removed, and modified values
    // For simplicity, just indicate comparison was made
    
    // Restore original config
    clear();
    configSections = originalConfig;
    markAsModified();
    
    return true;
}

void ConfigManager::setupBrainOptions() {
    setupBrainConfiguration();
}

void ConfigManager::setupMemoryOptions() {
    setupMemoryConfiguration();
}

void ConfigManager::setupPlasticityOptions() {
    setupPlasticityConfiguration();
}

void ConfigManager::setupDevelopmentOptions() {
    setupDevelopmentConfiguration();
}

void ConfigManager::setupNeuromodulationOptions() {
    setupNeuromodulationConfiguration();
}

void ConfigManager::setupPerformanceOptions() {
    setupPerformanceConfiguration();
}

void ConfigManager::setupOutputOptions() {
    setupOutputConfiguration();
}

void ConfigManager::setupDebugOptions() {
    setupDebugConfiguration();
}

void ConfigManager::setupLoggingOptions() {
    setupLoggingConfiguration();
}

void ConfigManager::setupExperimentOptions() {
    setupExperimentConfiguration();
}

// Configuration group implementations
void ConfigManager::setupBrainConfiguration(const std::string& section) {
    ConfigSection& brainSection = getSection(section);
    
    brainSection.values["neuron_count"] = 1000;
    brainSection.values["region_count"] = 2;
    brainSection.values["connection_probability"] = 0.1f;
    brainSection.values["random_seed"] = 42u;
    brainSection.values["simulation_timestep"] = 0.001;
    brainSection.values["stdp_ltp_weight"] = 0.01f;
    brainSection.values["stdp_ltd_weight"] = 0.012f;
    brainSection.values["stdp_tau"] = 20.0f;
    
    createSubsection(section, "plasticity");
    auto& plasticitySection = getSubsection(section, "plasticity");
    plasticitySection.values["learning_rate"] = 0.01f;
    plasticitySection.values["hebbian_strength"] = 0.005f;
}

void ConfigManager::setupMemoryConfiguration(const std::string& section) {
    ConfigSection& memSection = getSection(section);
    
    memSection.values["working_memory_capacity"] = 100;
    memSection.values["episodic_memory_capacity"] = 1000;
    memSection.values["decay_rate"] = 0.01f;
    memSection.values["retrieval_threshold"] = 0.5f;
}

void ConfigManager::setupPlasticityConfiguration(const std::string& section) {
    ConfigSection& plastSection = getSection(section);
    
    plastSection.values["synaptogenesis_rate"] = 0.0001f;
    plastSection.values["pruning_rate"] = 0.00001f;
    plastSection.values["maturation_rate"] = 0.001f;
    plastSection.values["structural_plasticity"] = true;
}

void ConfigManager::setupDevelopmentConfiguration(const std::string& section) {
    ConfigSection& devSection = getSection(section);
    
    devSection.values["critical_period"] = true;
    devSection.values["maturation_threshold"] = 0.8f;
    devSection.values["pruning_threshold"] = 0.5f;
}

void ConfigManager::setupNeuromodulationConfiguration(const std::string& section) {
    ConfigSection& neuromodSection = getSection(section);
    
    neuromodSection.values["dopamine_baseline"] = 0.1f;
    neuromodSection.values["dopamine_gain"] = 1.0f;
    neuromodSection.values["curiosity_rate"] = 0.01f;
    neuromodSection.values["novelty_sensitivity"] = 0.5f;
}

void ConfigManager::setupPerformanceConfiguration(const std::string& section) {
    ConfigSection& perfSection = getSection(section);
    
    perfSection.values["optimization_level"] = 2;
    perfSection.values["thread_count"] = 4;
    perfSection.values["vectorization_enabled"] = true;
    perfSection.values["sparse_connectivity"] = true;
}

void ConfigManager::setupOutputConfiguration(const std::string& section) {
    ConfigSection& outputSection = getSection(section);
    
    outputSection.values["output_format"] = "text";
    outputSection.values["verbose"] = true;
    outputSection.values["quiet"] = false;
    outputSection.values["progress_interval"] = 100;
}

void ConfigManager::setupDebugConfiguration(const std::string& section) {
    ConfigSection& debugSection = getSection(section);
    
    debugSection.values["debug_mode"] = false;
    debugSection.values["trace_level"] = 0;
    debugSection.values["validate_every_step"] = false;
}

void ConfigManager::setupLoggingConfiguration(const std::string& section) {
    ConfigSection& logSection = getSection(section);
    
    logSection.values["log_level"] = "info";
    logSection.values["log_to_file"] = false;
    logSection.values["log_filename"] = "nlm.log";
    logSection.values["log_append"] = true;
}

void ConfigManager::setupExperimentConfiguration(const std::string& section) {
    ConfigSection& expSection = getSection(section);
    
    expSection.values["max_simulation_steps"] = 10000;
    expSection.values["simulation_time_limit"] = 0.0;
    expSection.values["random_seed"] = 42u;
    expSection.values["checkpoint_interval"] = 1000;
}

} // namespace nlm
