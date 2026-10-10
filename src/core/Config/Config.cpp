#include "Config.hpp"
#include "../Exceptions/NLMExceptions.hpp"
#include "Validation/ConfigValidator.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <random>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), 
    changeLoggingEnabled(false), registeredParameters() {
    // Register default parameters
    registerParameter("random_seed", ParamValidation("random_seed", ParamType::Integer64, "Random seed for reproducible experiments"));
    registerParameter("simulation_timestep", ParamValidation("simulation_timestep", ParamType::Float, "Simulation timestep in seconds", 0.0, 1.0, 0.0001, 1.0));
    registerParameter("neuron_count", ParamValidation("neuron_count", ParamType::Integer64, "Number of neurons in the brain", 1, 1000000, 1));
    registerParameter("region_count", ParamValidation("region_count", ParamType::Integer64, "Number of brain regions", 1, 1000, 1));
    registerParameter("connection_probability", ParamValidation("connection_probability", ParamType::Float, "Connection probability for random connectivity", 0.0, 1.0, 0.0, 1.0));
    registerParameter("stdp_ltp_weight", ParamValidation("stdp_ltp_weight", ParamType::Float, "STDP LTP weight", 0.0, 0.1, 0.0, 0.1));
    registerParameter("stdp_ltd_weight", ParamValidation("stdp_ltd_weight", ParamType::Float, "STDP LTD weight", 0.0, 0.1, 0.0, 0.1));
    registerParameter("stdp_tau", ParamValidation("stdp_tau", ParamType::Float, "STDP time constant", 1.0, 100.0, 0.1, 100.0));
    registerParameter("synaptogenesis_rate", ParamValidation("synaptogenesis_rate", ParamType::Float, "Synaptogenesis rate", 0.0, 1.0, 0.0, 1.0));
    registerParameter("pruning_rate", ParamValidation("pruning_rate", ParamType::Float, "Pruning rate", 0.0, 1.0, 0.0, 1.0));
    registerParameter("environment_name", ParamValidation("environment_name", ParamType::String, "Environment name"));
    registerParameter("environment_width", ParamValidation("environment_width", ParamType::Integer64, "Environment width", 1, 1000, 1));
    registerParameter("environment_height", ParamValidation("environment_height", ParamType::Integer64, "Environment height", 1, 1000, 1));
    registerParameter("log_level", ParamValidation("log_level", ParamType::String, "Logging level"));
    registerParameter("log_to_file", ParamValidation("log_to_file", ParamType::Boolean, "Log to file"));
    registerParameter("max_simulation_steps", ParamValidation("max_simulation_steps", ParamType::Integer64, "Maximum simulation steps", 0, 1000000, 0));
    registerParameter("simulation_time_limit", ParamValidation("simulation_time_limit", ParamType::Float, "Simulation time limit in seconds", 0.0, 1000000.0, 0.0));
    registerParameter("visualization_enabled", ParamValidation("visualization_enabled", ParamType::Boolean, "Enable visualization"));
    registerParameter("visualization_update_rate", ParamValidation("visualization_update_rate", ParamType::Float, "Visualization update rate in Hz", 0.0, 1000.0, 0.0));
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    return loadFromFile(std::filesystem::path(filepath));
}

bool Config::loadFromFile(const std::filesystem::path& filepath) {
    // Add to config files list
    configFiles.push_back(filepath.string());
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    
    std::string format = detectFormat(filepath);
    
    bool success = false;
    if (format == "json") {
        success = parseJSON(content, ConfigSource::File);
    } else if (format == "yaml" || format == "yml") {
        success = parseYAML(content, ConfigSource::File);
    } else {
        success = parseLegacy(content, ConfigSource::File);
    }
    
    if (success && isChangeLoggingEnabled()) {
        logChange("Loaded configuration from: " + filepath.string());
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
    std::vector<std::string> args;
    for (int i = 1; i < argc; ++i) {
        args.push_back(argv[i]);
    }
    return loadFromArgs(args);
}

bool Config::loadFromArgs(const std::vector<std::string>& args) {
    for (const auto& arg : args) {
        if (arg.substr(0, 2) == "--") {
            if (arg.find('=') != std::string::npos) {
                std::string key = arg.substr(2, arg.find('=') - 2);
                std::string value = arg.substr(arg.find('=') + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
        else if (arg.substr(0, 1) == "-" && arg.length() > 1) {
            std::string key = arg.substr(1);
            // Find the value in the args vector
            auto it = std::find(args.begin(), args.end(), arg);
            if (it != args.end() && std::next(it) != args.end()) {
                set(key, *(std::next(it)), ConfigSource::CommandLine);
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool pretty) const {
    return saveToFile(std::filesystem::path(filepath), pretty);
}

bool Config::saveToFile(const std::filesystem::path& filepath, bool pretty) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    nlohmann::json json = exportToJSON();
    
    if (pretty) {
        file << std::setw(2) << json << std::endl;
    } else {
        file << json << std::endl;
    }
    
    if (isChangeLoggingEnabled()) {
        logChange("Saved configuration to: " + filepath.string());
    }
    
    return true;
}

bool Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto paramIt = registeredParameters.find(key);
    if (paramIt != registeredParameters.end()) {
        ConfigEntry entry(key, value, source);
        if (!isValueValid(entry)) {
            // Log validation failure and throw exception for critical parameters
            std::string errorMsg = "Invalid parameter '" + key + "'";
            if (isChangeLoggingEnabled()) {
                logChange("Validation failed: " + errorMsg);
            }
            
            // For critical parameters, throw an exception
            if (key == "neuron_count" || key == "region_count" || key == "simulation_timestep") {
                throw ConfigException(ErrorCode::CONFIG_PARAMETER_INVALID, 
                    errorMsg + " - " + getTypeErrorMessage(paramIt->second, value));
            }
            
            return false;
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        if (it->value != value || it->source != source) {
            it->value = value;
            it->source = source;
            it->timestamp = std::chrono::system_clock::now();
            
            if (isChangeLoggingEnabled()) {
                logChange("Updated configuration key: " + key + " = " + formatValue(value));
            }
        }
    } else {
        pImpl->entries.emplace_back(key, value, source);
        if (isChangeLoggingEnabled()) {
            logChange("Added configuration key: " + key + " = " + formatValue(value));
        }
    }
    
    return true;
}

bool Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, int value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, int64_t value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, double value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, bool value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
}

bool Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    return set(key, ConfigValue(value), source);
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
    if (isChangeLoggingEnabled()) {
        logChange("Cleared all configuration entries");
    }
    pImpl->entries.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary:" << std::endl;
    oss << "  Total entries: " << pImpl->entries.size() << std::endl;
    oss << "  Configuration files: " << configFiles.size() << std::endl;
    
    oss << std::endl << "  Parameter Sources:" << std::endl;
    oss << "    Default: " << std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.source == ConfigSource::Default; }) << std::endl;
    oss << "    File: " << std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.source == ConfigSource::File; }) << std::endl;
    oss << "    CommandLine: " << std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.source == ConfigSource::CommandLine; }) << std::endl;
    oss << "    Environment: " << std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.source == ConfigSource::Environment; }) << std::endl;
    oss << "    Runtime: " << std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.source == ConfigSource::Runtime; }) << std::endl;
    
    oss << std::endl << "  Registered Parameters: " << registeredParameters.size() << std::endl;
    
    return oss.str();
}

bool Config::addConfigFile(const std::string& filepath) {
    configFiles.push_back(filepath);
    return loadFromFile(filepath);
}

bool Config::removeConfigFile(const std::string& filepath) {
    auto it = std::find(configFiles.begin(), configFiles.end(), filepath);
    if (it != configFiles.end()) {
        configFiles.erase(it);
        return true;
    }
    return false;
}

void Config::clearConfigFiles() {
    configFiles.clear();
}

bool Config::validate() const {
    std::string errors = validateAndGetErrors();
    return errors.empty();
}

std::string Config::validateAndGetErrors() const {
    std::ostringstream errors;
    bool hasErrors = false;
    bool criticalErrors = false;
    
    // Check for critical parameter violations
    auto neuronCountIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.key == "neuron_count"; });
    auto regionCountIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.key == "region_count"; });
    
    if (neuronCountIt != pImpl->entries.end() && regionCountIt != pImpl->entries.end()) {
        try {
            int64_t neuronCount = std::get<int64_t>(neuronCountIt->value);
            int64_t regionCount = std::get<int64_t>(regionCountIt->value);
            if (neuronCount < regionCount * 10) {
                std::string errorMsg = "neuron_count (" + std::to_string(neuronCount) + ") "
                    "should be at least 10 times region_count (" + std::to_string(regionCount) + ") for proper connectivity";
                errors << "  Critical validation failed: " << errorMsg << std::endl;
                hasErrors = true;
                criticalErrors = true;
                errors << "  Throwing exception for critical validation error" << std::endl;
            }
        } catch (const std::bad_variant_access&) {}
    }
    
    // Perform detailed validation for each parameter
    for (const auto& entry : pImpl->entries) {
        auto paramIt = registeredParameters.find(entry.key);
        if (paramIt != registeredParameters.end()) {
            const auto& param = paramIt->second;
            if (!param.validate(entry.value)) {
                hasErrors = true;
                errors << "  Invalid parameter '" << entry.key << "': ";
                errors << getTypeErrorMessage(param, entry.value);
                errors << std::endl;
                
                // Mark as critical if it's a biologically important parameter
                if (entry.key == "neuron_count" || entry.key == "region_count" || 
                    entry.key == "connection_probability" || entry.key == "simulation_timestep") {
                    criticalErrors = true;
                }
            }
        }
    }
    
    if (hasErrors) {
        if (criticalErrors) {
            errors << "\nCRITICAL VALIDATION ERRORS DETECTED - Throwing exception!" << std::endl;
            throw ConfigValidationException("Configuration validation failed with critical errors", 
                std::map<std::string, std::string>{{"general", errors.str()}});
        } else {
            errors << "\nNon-critical validation errors (returned as string)." << std::endl;
        }
    } else {
        errors << "Configuration is valid." << std::endl;
    }
    
    return errors.str();
}

bool Config::validateCrossParameters() const {
    auto neuronCountIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.key == "neuron_count"; });
    auto regionCountIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [](const ConfigEntry& e) { return e.key == "region_count"; });
    
    if (neuronCountIt != pImpl->entries.end() && regionCountIt != pImpl->entries.end()) {
        try {
            int64_t neuronCount = std::get<int64_t>(neuronCountIt->value);
            int64_t regionCount = std::get<int64_t>(regionCountIt->value);
            if (neuronCount < regionCount * 10) {
                std::cerr << "Warning: neuron_count (" << neuronCount << ") should be at least 10 times region_count (" << regionCount << ") for proper connectivity." << std::endl;
                return false;
            }
        } catch (const std::bad_variant_access&) {}
    }
    
    return true;
}

bool Config::equals(const Config& other) const {
    return pImpl->entries == other.pImpl->entries;
}

std::string Config::diff(const Config& other) const {
    std::ostringstream diff;
    
    std::map<std::string, const ConfigEntry*> thisMap;
    for (const auto& entry : pImpl->entries) {
        thisMap[entry.key] = &entry;
    }
    
    std::map<std::string, const ConfigEntry*> otherMap;
    for (const auto& entry : other.pImpl->entries) {
        otherMap[entry.key] = &entry;
    }
    
    diff << "Configuration Differences:" << std::endl;
    
    for (const auto& [key, thisEntry] : thisMap) {
        auto otherIt = otherMap.find(key);
        if (otherIt == otherMap.end()) {
            diff << "  + Added: " << key << " = " << formatValue(thisEntry->value) << std::endl;
        } else if (!compareValues(thisEntry->value, otherIt->second->value)) {
            diff << "  ~ Changed: " << key << std::endl;
            diff << "    From: " << formatValue(otherIt->second->value) << std::endl;
            diff << "    To: " << formatValue(thisEntry->value) << std::endl;
        }
    }
    
    for (const auto& [key, otherEntry] : otherMap) {
        if (thisMap.find(key) == thisMap.end()) {
            diff << "  - Removed: " << key << " = " << formatValue(otherEntry->value) << std::endl;
        }
    }
    
    if (diff.str() == "Configuration Differences:\n") {
        diff << "No differences found." << std::endl;
    }
    
    return diff.str();
}

bool Config::backup(const std::string& backupPath) const {
    std::string actualBackupPath = backupPath;
    if (actualBackupPath.empty()) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
        gmtime_r(&time, &tm_buf);
        std::ostringstream oss;
        oss << "configs/backup_" << std::put_time(&tm_buf, "%Y%m%d_%H%M%S") << ".json";
        actualBackupPath = oss.str();
    }
    
    if (saveToFile(actualBackupPath, true)) {
        if (isChangeLoggingEnabled()) {
            logChange("Configuration backed up to: " + actualBackupPath);
        }
        return true;
    }
    return false;
}

bool Config::restoreFromBackup(const std::string& backupPath) const {
    std::string actualBackupPath = backupPath;
    if (actualBackupPath.empty()) {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::tm tm_buf;
        gmtime_r(&time, &tm_buf);
        std::ostringstream oss;
        oss << "configs/backup_" << std::put_time(&tm_buf, "%Y%m%d_%H%M%S") << ".json";
        actualBackupPath = oss.str();
    }
    
    if (std::filesystem::exists(actualBackupPath)) {
        Config backupConfig;
        if (backupConfig.loadFromFile(actualBackupPath)) {
            // This would require a merge operation; for simplicity, we'll just clear and reload
            clear();
            pImpl->entries = backupConfig.pImpl->entries;
            configFiles = backupConfig.configFiles;
            
            if (isChangeLoggingEnabled()) {
                logChange("Configuration restored from backup: " + actualBackupPath);
            }
            return true;
        }
    }
    return false;
}

Config Config::loadTemplate(const std::string& templateName) {
    Config config;
    std::string templatePath = "configs/templates/" + templateName + ".json";
    
    std::ifstream file(templatePath);
    if (file.is_open()) {
        nlohmann::json json;
        file >> json;
        
        config.importFromJSON(json, ConfigSource::File);
    }
    
    return config;
}

bool Config::saveTemplate(const std::string& templateName, const Config& config) {
    std::string templatePath = "configs/templates/" + templateName + ".json";
    
    nlohmann::json json = config.exportToJSON();
    
    std::ofstream file(templatePath);
    if (file.is_open()) {
        file << std::setw(2) << json << std::endl;
        return true;
    }
    
    return false;
}

std::vector<std::string> Config::getAvailableTemplates() {
    std::vector<std::string> templates;
    std::string templatesDir = "configs/templates/";
    
    if (std::filesystem::exists(templatesDir)) {
        for (const auto& entry : std::filesystem::directory_iterator(templatesDir)) {
            if (entry.is_regular_file()) {
                std::string filename = entry.path().filename().string();
                if (filename.size() > 5 && filename.substr(filename.size() - 5) == ".json") {
                    templates.push_back(filename.substr(0, filename.size() - 5));
                }
            }
        }
    }
    
    return templates;
}

bool Config::migrateFromOldFormat(const std::string& oldFormatFile) {
    if (loadFromFile(oldFormatFile)) {
        // Convert legacy entries to standard format
        for (auto& entry : pImpl->entries) {
            if (entry.source == ConfigSource::File) {
                // Legacy files need to be migrated to new format
                // For now, just log the migration
                if (isChangeLoggingEnabled()) {
                    logChange("Migrated configuration entry from old format: " + entry.key);
                }
            }
        }
        return true;
    }
    return false;
}

void Config::registerParameter(const std::string& key, const ParamValidation& param) {
    registeredParameters[key] = param;
}

std::optional<ParamValidation> Config::getParameterInfo(const std::string& key) const {
    auto it = registeredParameters.find(key);
    if (it != registeredParameters.end()) {
        return it->second;
    }
    return std::nullopt;
}

std::vector<std::string> Config::getRegisteredParameterKeys() const {
    std::vector<std::string> keys;
    keys.reserve(registeredParameters.size());
    for (const auto& pair : registeredParameters) {
        keys.push_back(pair.first);
    }
    return keys;
}

bool Config::isValueValid(const ConfigEntry& entry) const {
    auto paramIt = registeredParameters.find(entry.key);
    if (paramIt == registeredParameters.end()) {
        return true; // No validation required if not registered
    }
    
    return paramIt->second.validate(entry.value);
}

std::string Config::getTypeErrorMessage(const ParamValidation& param, const ConfigValue& value) const {
    std::ostringstream error;
    
    switch (param.type) {
        case ParamType::Integer:
            if (!std::holds_alternative<int>(value)) {
                error << "Expected integer, got " << valueTypeToString(value);
            }
            break;
        case ParamType::Integer64:
            if (!std::holds_alternative<int64_t>(value)) {
                error << "Expected integer64, got " << valueTypeToString(value);
            }
            break;
        case ParamType::Float:
            if (!std::holds_alternative<double>(value)) {
                error << "Expected float, got " << valueTypeToString(value);
            }
            break;
        case ParamType::Boolean:
            if (!std::holds_alternative<bool>(value)) {
                error << "Expected boolean, got " << valueTypeToString(value);
            }
            break;
        case ParamType::String:
            if (!std::holds_alternative<std::string>(value)) {
                error << "Expected string, got " << valueTypeToString(value);
            }
            break;
        case ParamType::StringList:
            if (!std::holds_alternative<std::vector<std::string>>(value)) {
                error << "Expected string list, got " << valueTypeToString(value);
            }
            break;
        case ParamType::IntList:
            if (!std::holds_alternative<std::vector<int>>(value)) {
                error << "Expected integer list, got " << valueTypeToString(value);
            }
            break;
        case ParamType::FloatList:
            if (!std::holds_alternative<std::vector<double>>(value)) {
                error << "Expected float list, got " << valueTypeToString(value);
            }
            break;
    }
    
    return error.str();
}

std::string Config::valueTypeToString(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64";
    if (std::holds_alternative<double>(value)) return "float";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "int[]";
    if (std::holds_alternative<std::vector<double>>(value)) return "float[]";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "string[]";
    return "unknown";
}

bool Config::compareValues(const ConfigValue& val1, const ConfigValue& val2) const {
    if (val1.index() != val2.index()) return false;
    
    switch (val1.index()) {
        case 0: return std::get<int>(val1) == std::get<int>(val2);
        case 1: return std::get<int64_t>(val1) == std::get<int64_t>(val2);
        case 2: return std::get<double>(val1) == std::get<double>(val2);
        case 3: return std::get<bool>(val1) == std::get<bool>(val2);
        case 4: return std::get<std::string>(val1) == std::get<std::string>(val2);
        case 5: return std::get<std::vector<int>>(val1) == std::get<std::vector<int>>(val2);
        case 6: return std::get<std::vector<double>>(val1) == std::get<std::vector<double>>(val2);
        case 7: return std::get<std::vector<std::string>>(val1) == std::get<std::vector<std::string>>(val2);
        default: return false;
    }
}

std::string Config::formatValue(const ConfigValue& value) const {
    std::ostringstream oss;
    if (std::holds_alternative<int>(value)) {
        oss << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        oss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        oss << std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        oss << (std::get<bool>(value) ? "true" : "false");
    } else if (std::holds_alternative<std::string>(value)) {
        oss << "\"" << std::get<std::string>(value) << "\"";
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        oss << "["; bool first = true;
        for (const auto& v : std::get<std::vector<int>>(value)) {
            if (!first) oss << ", ";
            oss << v;
            first = false;
        }
        oss << "]";
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        oss << "["; bool first = true;
        for (const auto& v : std::get<std::vector<double>>(value)) {
            if (!first) oss << ", ";
            oss << v;
            first = false;
        }
        oss << "]";
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        oss << "["; bool first = true;
        for (const auto& v : std::get<std::vector<std::string>>(value)) {
            if (!first) oss << ", ";
            oss << "\"" << v << "\"";
            first = false;
        }
        oss << "]";
    }
    return oss.str();
}

bool Config::parseJSON(const std::string& content, ConfigSource source) {
    try {
        nlohmann::json json = nlohmann::json::parse(content);
        importFromJSON(json, source);
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return false;
    }
}

bool Config::parseYAML(const std::string& content, ConfigSource source) {
    try {
        YAML::Node doc = YAML::Load(content);
        importFromYAML(doc, source);
        return true;
    } catch (const YAML::Exception& e) {
        std::cerr << "YAML parse error: " << e.what() << std::endl;
        return false;
    }
}

bool Config::parseLegacy(const std::string& content, ConfigSource source) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
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
            
            set(key, value, source);
        }
    }
    
    return true;
}

nlohmann::json Config::exportToJSON() const {
    nlohmann::json json;
    
    for (const auto& entry : pImpl->entries) {
        json[entry.key] = exportValueToJSON(entry.value);
    }
    
    return json;
}

void Config::importFromJSON(const nlohmann::json& data, ConfigSource source) {
    clear();
    
    for (auto it = data.begin(); it != data.end(); ++it) {
        std::string key = it.key();
        ConfigValue value = importValueFromJSON(*it);
        set(key, value, source);
    }
}

nlohmann::json Config::exportValueToJSON(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        return std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value);
    } else if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        nlohmann::json array = nlohmann::json::array();
        for (const auto& v : std::get<std::vector<int>>(value)) {
            array.push_back(v);
        }
        return array;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        nlohmann::json array = nlohmann::json::array();
        for (const auto& v : std::get<std::vector<double>>(value)) {
            array.push_back(v);
        }
        return array;
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        nlohmann::json array = nlohmann::json::array();
        for (const auto& v : std::get<std::vector<std::string>>(value)) {
            array.push_back(v);
        }
        return array;
    }
    
    return nullptr;
}

ConfigValue Config::importValueFromJSON(const nlohmann::json& json) const {
    if (json.is_number_integer()) {
        return json.get<int>();
    } else if (json.is_number()) {
        return json.get<double>();
    } else if (json.is_boolean()) {
        return json.get<bool>();
    } else if (json.is_string()) {
        return json.get<std::string>();
    } else if (json.is_array()) {
        nlohmann::json::array_t array = json.get<nlohmann::json::array_t>();
        if (!array.empty() && array[0].is_number_integer()) {
            std::vector<int> intArray;
            for (const auto& item : array) {
                intArray.push_back(item.get<int>());
            }
            return intArray;
        } else if (!array.empty() && array[0].is_number()) {
            std::vector<double> floatArray;
            for (const auto& item : array) {
                floatArray.push_back(item.get<double>());
            }
            return floatArray;
        } else {
            std::vector<std::string> stringArray;
            for (const auto& item : array) {
                stringArray.push_back(item.get<std::string>());
            }
            return stringArray;
        }
    }
    
    return std::string();
}

void Config::importFromYAML(const YAML::Node& data, ConfigSource source) {
    clear();
    
    for (const auto& pair : data) {
        std::string key = pair.first.as<std::string>();
        ConfigValue value = importValueFromYAML(pair.second);
        set(key, value, source);
    }
}

ConfigValue Config::importValueFromYAML(const YAML::Node& node) const {
    if (node.IsScalar()) {
        if (node.IsInteger()) {
            return node.as<int>();
        } else if (node.IsDouble()) {
            return node.as<double>();
        } else if (node.IsBool()) {
            return node.as<bool>();
        } else {
            return node.as<std::string>();
        }
    } else if (node.IsSequence()) {
        if (!node.empty()) {
            if (node[0].IsInteger()) {
                std::vector<int> intArray;
                for (const auto& item : node) {
                    intArray.push_back(item.as<int>());
                }
                return intArray;
            } else if (node[0].IsDouble()) {
                std::vector<double> floatArray;
                for (const auto& item : node) {
                    floatArray.push_back(item.as<double>());
                }
                return floatArray;
            } else {
                std::vector<std::string> stringArray;
                for (const auto& item : node) {
                    stringArray.push_back(item.as<std::string>());
                }
                return stringArray;
            }
        }
    }
    
    return std::string();
}

std::string Config::detectFormat(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::string extension = path.extension().string();
    
    if (extension == ".json") return "json";
    if (extension == ".yaml" || extension == ".yml") return "yaml";
    if (extension == ".cfg" || extension == ".ini") return "legacy";
    
    // Try to guess from content
    std::ifstream file(filepath);
    if (!file.is_open()) return "legacy";
    
    std::string firstLine;
    std::getline(file, firstLine);
    
    if (firstLine.find('{') != std::string::npos || firstLine.find('[') != std::string::npos) {
        return "json";
    }
    
    if (firstLine.find(":") != std::string::npos && (firstLine.find("#") != std::string::npos || firstLine.find("=") != std::string::npos)) {
        return "yaml";
    }
    
    return "legacy";
}

bool Config::convertToFormat(const std::string& fromPath, const std::string& toPath) {
    Config config;
    if (!config.loadFromFile(fromPath)) {
        return false;
    }
    
    std::filesystem::path toFilePath(toPath);
    std::string format = detectFormat(toPath);
    
    if (format == "json") {
        return config.saveToFile(toPath, true);
    } else if (format == "yaml") {
        std::ofstream file(toPath);
        if (!file.is_open()) return false;
        
        nlohmann::json json = config.exportToJSON();
        file << "# Configuration converted from " << fromPath << std::endl;
        file << "# Converted at: " << getTimestampString(std::chrono::system_clock::now()) << std::endl;
        file << std::endl;
        
        // Convert JSON to YAML-like format
        file << "---" << std::endl;
        for (auto it = json.begin(); it != json.end(); ++it) {
            file << it.key() << ": " << it.value() << std::endl;
        }
        
        return true;
    }
    
    return config.saveToFile(toPath, false);
}

void Config::enableChangeLogging(const std::string& logFile) {
    changeLoggingEnabled = true;
    changeLogFile = logFile;
    
    if (!changeLogFile.empty()) {
        std::ofstream file(changeLogFile, std::ios::app);
        if (file.is_open()) {
            file << "Configuration logging enabled at: " << getTimestampString(std::chrono::system_clock::now()) << std::endl;
        }
    }
}

void Config::disableChangeLogging() {
    changeLoggingEnabled = false;
    
    if (!changeLogFile.empty()) {
        std::ofstream file(changeLogFile, std::ios::app);
        if (file.is_open()) {
            file << "Configuration logging disabled at: " << getTimestampString(std::chrono::system_clock::now()) << std::endl;
        }
    }
}

bool Config::isChangeLoggingEnabled() const {
    return changeLoggingEnabled;
}

void Config::logChange(const std::string& message) {
    if (isChangeLoggingEnabled()) {
        std::ofstream file;
        if (changeLogFile.empty()) {
            file.open("configs/config_changes.log", std::ios::app);
        } else {
            file.open(changeLogFile, std::ios::app);
        }
        
        if (file.is_open()) {
            file << "[" << getTimestampString(std::chrono::system_clock::now()) << "] " 
                 << message << std::endl;
        }
    }
}

bool Config::loadFromEnvironment(const std::string& prefix) {
    bool success = false;
    
    for (auto& pair : registeredParameters) {
        const auto& key = pair.first;
        std::string envKey = prefix + key;
        
        const char* envValue = std::getenv(envKey.c_str());
        if (envValue != nullptr) {
            set(key, std::string(envValue), ConfigSource::Environment);
            success = true;
        }
    }
    
    return success;
}

nlohmann::json Config::exportToSchema() const {
    nlohmann::json schema;
    schema["type"] = "object";
    schema["properties"] = nlohmann::json::object();
    
    for (const auto& pair : registeredParameters) {
        const auto& param = pair.second;
        nlohmann::json prop;
        
        switch (param.type) {
            case ParamType::Integer:
                prop["type"] = "integer";
                break;
            case ParamType::Integer64:
                prop["type"] = "integer";
                break;
            case ParamType::Float:
                prop["type"] = "number";
                break;
            case ParamType::Boolean:
                prop["type"] = "boolean";
                break;
            case ParamType::String:
                prop["type"] = "string";
                break;
            case ParamType::StringList:
                prop["type"] = "array";
                prop["items"] = nlohmann::json::object({{"type", "string"}});
                break;
            case ParamType::IntList:
                prop["type"] = "array";
                prop["items"] = nlohmann::json::object({{"type", "integer"}});
                break;
            case ParamType::FloatList:
                prop["type"] = "array";
                prop["items"] = nlohmann::json::object({{"type", "number"}});
                break;
        }
        
        if (param.required) prop["required"] = true;
        if (param.min.has_value()) prop["minimum"] = param.min.value();
        if (param.max.has_value()) prop["maximum"] = param.max.value();
        if (param.minSize.has_value()) prop["minItems"] = param.minSize.value();
        if (param.maxSize.has_value()) prop["maxItems"] = param.maxSize.value();
        if (!param.description.empty()) prop["description"] = param.description;
        
        schema["properties"][key] = prop;
    }
    
    return schema;
}

bool Config::importFromSchema(const nlohmann::json& schema) {
    clear();
    registeredParameters.clear();
    
    if (!schema.contains("properties")) {
        return false;
    }
    
    for (const auto& item : schema["properties"].items()) {
        std::string key = item.key();
        const auto& prop = item.value();
        
        ParamValidation param(key, ParamType::String);
        
        if (prop.contains("description")) {
            param.description = prop["description"].get<std::string>();
        }
        
        if (prop.contains("type")) {
            std::string typeStr = prop["type"].get<std::string>();
            if (typeStr == "integer") {
                param.type = ParamType::Integer;
            } else if (typeStr == "number") {
                param.type = ParamType::Float;
            } else if (typeStr == "boolean") {
                param.type = ParamType::Boolean;
            } else if (typeStr == "string") {
                param.type = ParamType::String;
            } else if (typeStr == "array") {
                if (prop.contains("items") && prop["items"].contains("type")) {
                    std::string itemType = prop["items"]["type"].get<std::string>();
                    if (itemType == "integer") {
                        param.type = ParamType::IntList;
                    } else if (itemType == "number") {
                        param.type = ParamType::FloatList;
                    } else {
                        param.type = ParamType::StringList;
                    }
                }
            }
        }
        
        if (prop.contains("required") && prop["required"].get<bool>()) {
            param.required = true;
        }
        
        if (prop.contains("minimum")) {
            param.min = prop["minimum"].get<double>();
        }
        
        if (prop.contains("maximum")) {
            param.max = prop["maximum"].get<double>();
        }
        
        if (prop.contains("minItems")) {
            param.minSize = prop["minItems"].get<size_t>();
        }
        
        if (prop.contains("maxItems")) {
            param.maxSize = prop["maxItems"].get<size_t>();
        }
        
        registerParameter(key, param);
    }
    
    return true;
}

std::string Config::getDebugInfo() const {
    std::ostringstream oss;
    oss << "=== Configuration Debug Information ===" << std::endl;
    oss << "Source files: ";
    for (const auto& file : configFiles) {
        oss << file << " ";
    }
    oss << std::endl;
    oss << "Registered parameters: " << registeredParameters.size() << std::endl;
    oss << "Active entries: " << pImpl->entries.size() << std::endl;
    oss << "Change logging enabled: " << (changeLoggingEnabled ? "Yes" : "No") << std::endl;
    if (changeLoggingEnabled) {
        oss << "Change log file: " << changeLogFile << std::endl;
    }
    
    return oss.str();
}

std::string Config::getTimestampString(const std::chrono::system_clock::time_point& tp) {
    auto time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
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

} // namespace nlm
