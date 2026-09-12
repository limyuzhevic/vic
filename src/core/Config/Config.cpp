// Enhanced Configuration Management Implementation
// Extended Config.cpp with comparison, diffing, validation, templates, and inheritance

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <limits>
#include <stdexcept>
#include <sstream>
#include <map>
#include <set>
#include <unordered_map>
#include <chrono>

namespace nlm {

// Enhanced Config implementation with advanced features
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string name;
    std::string description;
    std::string version;
    std::chrono::system_clock::time_point createdAt;
    std::chrono::system_clock::time_point lastModified;
    std::string author;
    std::unordered_map<std::string, std::string> metadata;
    std::vector<std::string> tags;
    bool isTemplate;
    std::string templateId;
    std::string parentTemplateId;
    std::vector<std::string> inheritanceChain;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Load default configuration
    loadDefaults();
    
    // Set default metadata
    pImpl->name = "Default Configuration";
    pImpl->description = "Default NLM configuration";
    pImpl->version = "1.0.0";
    pImpl->createdAt = std::chrono::system_clock::now();
    pImpl->lastModified = pImpl->createdAt;
    pImpl->author = "NLM System";
    pImpl->isTemplate = false;
    
    // Add validation rules for key configuration parameters
    addValidationRule(ConfigValidationRule::Type::PositiveInteger, "neuron_count",
                     "Number of neurons in the brain", 
                     createPositiveValidator(),
                     "neuron_count must be a positive integer");
    
    addValidationRule(ConfigValidationRule::Type::PositiveInteger, "region_count",
                     "Number of brain regions",
                     createPositiveValidator(),
                     "region_count must be a positive integer");
    
    addValidationRule(ConfigValidationRule::Type::DoubleRange, "connection_probability",
                     "Connection probability for random connectivity",
                     createDoubleRangeValidator(0.0, 1.0),
                     "connection_probability must be between 0.0 and 1.0");
    
    addValidationRule(ConfigValidationRule::Type::DoubleRange, "simulation_timestep",
                     "Simulation timestep in seconds",
                     createDoubleRangeValidator(0.0, 1.0),
                     "simulation_timestep must be positive (recommended: < 0.001 for stability)");
    
    addValidationRule(ConfigValidationRule::Type::DoubleRange, "plasticity_learning_rate",
                     "Plasticity learning rate",
                     createDoubleRangeValidator(0.0, 1.0),
                     "plasticity_learning_rate must be between 0.0 and 1.0");
    
    addValidationRule(ConfigValidationRule::Type::DoubleRange, "stdp_ltp_weight",
                     "STDP long-term potentiation weight",
                     createDoubleRangeValidator(-1.0, 1.0),
                     "stdp_ltp_weight must be between -1.0 and 1.0");
    
    addValidationRule(ConfigValidationRule::Type::DoubleRange, "stdp_ltd_weight",
                     "STDP long-term depression weight",
                     createDoubleRangeValidator(-1.0, 1.0),
                     "stdp_ltd_weight must be between -1.0 and 1.0");
    
    addValidationRule(ConfigValidationRule::Type::DoubleRange, "stdp_tau",
                     "STDP time constant (tau)",
                     createPositiveValidator(),
                     "stdp_tau must be positive");
    
    addValidationRule(ConfigValidationRule::Type::DoubleRange, "synaptogenesis_rate",
                     "Synaptogenesis rate",
                     createDoubleRangeValidator(0.0, 1.0),
                     "synaptogenesis_rate must be between 0.0 and 1.0");
    
    addValidationRule(ConfigValidationRule::Type::DoubleRange, "pruning_rate",
                     "Pruning rate",
                     createDoubleRangeValidator(0.0, 1.0),
                     "pruning_rate must be between 0.0 and 1.0");
    
    addValidationRule(ConfigValidationRule::Type::PositiveInteger, "environment_width",
                     "Environment width",
                     createPositiveValidator(),
                     "environment_width must be a positive integer");
    
    addValidationRule(ConfigValidationRule::Type::PositiveInteger, "environment_height",
                     "Environment height",
                     createPositiveValidator(),
                     "environment_height must be a positive integer");
    
    addValidationRule(ConfigValidationRule::Type::DoubleRange, "reward_discount_factor",
                     "Reward discount factor",
                     createDoubleRangeValidator(0.0, 1.0),
                     "reward_discount_factor must be between 0.0 and 1.0");
    
    addValidationRule(ConfigValidationRule::Type::PositiveInteger, "log_level",
                     "Log level",
                     createPositiveValidator(),
                     "log_level must be a positive integer");
    
    addValidationRule(ConfigValidationRule::Type::PositiveInteger, "replay_interval",
                     "Replay interval (steps)",
                     createPositiveValidator(),
                     "replay_interval must be a positive integer");
    
    addValidationRule(ConfigValidationRule::Type::PositiveInteger, "consolidation_interval",
                     "Consolidation interval (steps)",
                     createPositiveValidator(),
                     "consolidation_interval must be a positive integer");
    
    addValidationRule(ConfigValidationRule::Type::StringValidator, "checkpoint_dir",
                     "Checkpoint directory",
                     createStringValidator(),
                     "checkpoint_dir must be a string representing a directory path");
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Enhanced helper functions
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

bool Config::parseValue(const std::string& value, ConfigValue& out) {
    // Try bool
    if (value == "true" || value == "1") {
        out = true;
        return true;
    }
    if (value == "false" || value == "0") {
        out = false;
        return true;
    }
    
    // Try int
    try {
        size_t pos;
        int intVal = std::stoi(value, &pos);
        if (pos == value.size()) {
            out = intVal;
            return true;
        }
    } catch (...) {}
    
    // Try int64_t
    try {
        size_t pos;
        int64_t int64Val = std::stoll(value, &pos);
        if (pos == value.size()) {
            out = int64Val;
            return true;
        }
    } catch (...) {}
    
    // Try double
    try {
        size_t pos;
        double doubleVal = std::stod(value, &pos);
        if (pos == value.size()) {
            out = doubleVal;
            return true;
        }
    } catch (...) {}
    
    // String (treat as string)
    out = value;
    return true;
}

std::string Config::configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError("Failed to open configuration file: " + filepath, ConfigErrorType::ParseError);
    }
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        ++lineNumber;
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
            
            ConfigValue configValue;
            if (!parseValue(value, configValue)) {
                throw ConfigError("Invalid value for key '" + key + "' on line " + 
                                std::to_string(lineNumber) + ": " + value, 
                                ConfigErrorType::ParseError);
            }
            
            set(key, configValue, ConfigSource::File);
        } else {
            throw ConfigError("Invalid configuration syntax on line " + 
                            std::to_string(lineNumber) + ": " + line,
                            ConfigErrorType::ParseError);
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
                ConfigValue configValue;
                if (parseValue(value, configValue)) {
                    set(key, configValue, ConfigSource::CommandLine);
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            ConfigValue configValue;
            if (parseValue(value, configValue)) {
                set(key, configValue, ConfigSource::CommandLine);
            }
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError("Failed to open configuration file for writing: " + filepath, 
                        ConfigErrorType::ParseError);
    }
    
    file << "# NLM Configuration File (saved from current configuration)\n";
    file << "# This file contains configuration values from runtime and other sources\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << " (source: " << static_cast<int>(entry.source) << ")\n";
        file << entry.key << " = " << configValueToString(entry.value) << "\n\n";
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
        // Check type safety
        if (!isTypeMatch(it->value, typeid(T))) {
            return std::nullopt;
        }
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

bool Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        // Find a description for the key
        std::string description = "Configuration parameter: " + key;
        pImpl->entries.emplace_back(key, value, source, description);
    }
    
    // Update timestamp
    pImpl->lastModified = std::chrono::system_clock::now();
    
    return true;
}

template<typename T>
bool Config::set(const std::string& key, const T& value, ConfigSource source) {
    ConfigValue configValue(value);
    set(key, configValue, source);
    return true;
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

template<typename T>
bool Config::has(const std::string& key) const {
    auto val = get<T>(key);
    return val.has_value();
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
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "=" << std::string(60, '=') << "\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "Key: " << std::left << std::setw(25) << entry.key;
        oss << "Source: " << std::setw(8) << static_cast<int>(entry.source);
        oss << "Value: [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "]" << std::endl;
    }
    oss << "=" << std::string(60, '=') << "\n";
    
    // Show validation status
    if (validationEnabled && !validationRules.empty()) {
        oss << "\nValidation Status:\n";
        bool valid = validate();
        oss << "Overall: " << (valid ? "VALID" : "INVALID") << "\n";
        if (!valid) {
            oss << "Errors:" << std::endl;
            std::vector<std::string> errors;
            collectValidationErrors(errors);
            for (const auto& error : errors) {
                oss << "  " << error << std::endl;
            }
        }
    }
    
    return oss.str();
}

void Config::loadDefaults() {
    auto defaults = getDefaults();
    for (const auto& [key, value] : defaults) {
        // Check if already set (from file, command line, or runtime)
        if (!has(key)) {
            set(key, value.first, ConfigSource::Default);
        }
    }
}

std::unordered_map<std::string, std::pair<ConfigValue, std::string>> Config::getDefaults() {
    static const std::unordered_map<std::string, std::pair<ConfigValue, std::string>> defaults = {
        {"random_seed", {int64_t(42), "Random seed for reproducible experiments"}},
        {"simulation_timestep", {0.001, "Simulation timestep in seconds"}},
        {"neuron_count", {int64_t(1000), "Number of neurons in the brain"}},
        {"region_count", {int64_t(2), "Number of brain regions"}},
        {"connection_probability", {0.1, "Connection probability for random connectivity"}},
        {"plasticity_learning_rate", {0.01, "Plasticity learning rate"}},
        {"stdp_ltp_weight", {0.01, "STDP long-term potentiation weight"}},
        {"stdp_ltd_weight", {0.012, "STDP long-term depression weight"}},
        {"stdp_time_constant", {20.0, "STDP time constant (tau)"}},
        {"stdp_tau", {20.0, "STDP time constant (tau)"}},
        {"development_synaptogenesis_rate", {0.001, "Development synaptogenesis rate"}},
        {"development_pruning_rate", {0.0001, "Development pruning rate"}},
        {"synaptogenesis_rate", {0.0001, "Synaptogenesis rate"}},
        {"pruning_rate", {0.00001, "Pruning rate"}},
        {"dopamine_baseline", {0.1, "Dopamine baseline level"}},
        {"reward_discount_factor", {0.99, "Reward discount factor"}},
        {"environment_name", {std::string("GridWorld"), "Environment name"}},
        {"environment_width", {int(10), "Environment width"}},
        {"environment_height", {int(10), "Environment height"}},
        {"vision_width", {int(16), "Vision width in pixels"}},
        {"vision_height", {int(16), "Vision height in pixels"}},
        {"log_level", {std::string("INFO"), "Log level"}},
        {"log_to_file", {false, "Log to file"}},
        {"log_filename", {std::string("nlm.log"), "Log filename"}},
        {"max_simulation_steps", {int64_t(10000), "Maximum simulation steps"}},
        {"simulation_time_limit", {0.0, "Simulation time limit (seconds)"}},
        {"visualization_enabled", {false, "Enable visualization"}},
        {"visualization_update_rate", {30.0, "Visualization update rate (Hz)"}},
        {"replay_interval", {size_t(100), "Replay interval (steps)"}},
        {"consolidation_interval", {size_t(1000), "Consolidation interval (steps)"}},
        {"checkpoint_dir", {std::string("./checkpoints"), "Checkpoint directory"}},
        {"working_memory_capacity", {size_t(100), "Working memory capacity"}},
        {"prediction_horizon", {size_t(3), "Prediction horizon"}},
        {"structural_plasticity_rate", {0.0001, "Structural plasticity rate"}}
    };
    
    return defaults;
}

void Config::addValidationRule(ConfigValidationRule::Type type, const std::string& key,
                              const std::string& description,
                              const std::function<bool(const ConfigValue&)>& validator,
                              const std::string& errorMessage) {
    validationRules.emplace_back(type, key, description, validator, errorMessage);
}

void Config::enableValidation(bool enable) {
    validationEnabled = enable;
}

bool Config::isValidationEnabled() const {
    return validationEnabled;
}

void Config::validateEntries() const {
    if (!validationEnabled || validationRules.empty()) {
        return;
    }
    
    for (const auto& rule : validationRules) {
        if (has(rule.key)) {
            auto valueOpt = get<std::remove_reference<decltype(rule.validator)>::argument_type>(rule.key);
            if (valueOpt) {
                if (!validateValue(rule, *valueOpt)) {
                    std::cerr << "Validation error for key '" << rule.key << "': " 
                              << rule.errorMessage << std::endl;
                }
            }
        }
    }
}

void Config::collectValidationErrors(std::vector<std::string>& errors) const {
    if (!validationEnabled || validationRules.empty()) {
        return;
    }
    
    for (const auto& rule : validationRules) {
        if (has(rule.key)) {
            auto valueOpt = get<std::remove_reference<decltype(rule.validator)>::argument_type>(rule.key);
            if (valueOpt) {
                if (!validateValue(rule, *valueOpt)) {
                    errors.push_back("Validation error for key '" + rule.key + "': " + 
                                    rule.errorMessage);
                }
            }
        }
    }
}

bool Config::validate() const {
    std::vector<std::string> errors;
    collectValidationErrors(errors);
    return errors.empty();
}

std::string Config::validationErrors() const {
    std::vector<std::string> errors;
    collectValidationErrors(errors);
    
    std::ostringstream oss;
    if (errors.empty()) {
        oss << "No validation errors. Configuration is valid.";
    } else {
        oss << errors.size() << " validation error(s):\n";
        for (const auto& error : errors) {
            oss << "  - " << error << "\n";
        }
    }
    return oss.str();
}

bool Config::isTypeMatch(const ConfigValue& value, const std::type_info& typeInfo) {
    if (typeInfo == typeid(int) || typeInfo == typeid(int64_t) || typeInfo == typeid(long)) {
        return std::holds_alternative<int>(value) || 
               std::holds_alternative<int64_t>(value);
    } else if (typeInfo == typeid(double) || typeInfo == typeid(float)) {
        return std::holds_alternative<double>(value);
    } else if (typeInfo == typeid(bool)) {
        return std::holds_alternative<bool>(value);
    } else if (typeInfo == typeid(std::string)) {
        return std::holds_alternative<std::string>(value);
    }
    return false;
}

bool Config::validateValue(const ConfigValidationRule& rule, const ConfigValue& value) {
    if (!rule.validator) {
        return false;
    }
    return rule.validator(value);
}

std::function<bool(const ConfigValue&)> Config::createIntRangeValidator(int min, int max) {
    return [min, max](const ConfigValue& value) {
        if (!std::holds_alternative<int>(value) && !std::holds_alternative<int64_t>(value)) {
            return false;
        }
        int64_t val = std::holds_alternative<int>(value) ? 
                      std::get<int>(value) : std::get<int64_t>(value);
        return val >= min && val <= max;
    };
}

std::function<bool(const ConfigValue&)> Config::createDoubleRangeValidator(double min, double max) {
    return [min, max](const ConfigValue& value) {
        if (!std::holds_alternative<double>(value)) {
            return false;
        }
        double val = std::get<double>(value);
        return val >= min && val <= max;
    };
}

std::function<bool(const ConfigValue&)> Config::createPositiveValidator() {
    return [](const ConfigValue& value) {
        if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
            int64_t val = std::holds_alternative<int>(value) ? 
                          std::get<int>(value) : std::get<int64_t>(value);
            return val > 0;
        } else if (std::holds_alternative<double>(value)) {
            double val = std::get<double>(value);
            return val >= 0.0;
        }
        return false;
    };
}

std::function<bool(const ConfigValue&)> Config::createStringValidator() {
    return [](const ConfigValue& value) {
        return std::holds_alternative<std::string>(value);
    };
}

// Enhanced template and inheritance methods
std::vector<ConfigDiff> Config::compareWith(const Config& other) const {
    std::vector<ConfigDiff> diffs;
    
    // Check for entries in this config not in other
    for (const auto& entry : pImpl->entries) {
        bool found = false;
        for (const auto& otherEntry : other.pImpl->entries) {
            if (entry.key == otherEntry.key) {
                found = true;
                if (entry.value != otherEntry.value || entry.source != otherEntry.source) {
                    ConfigDiff diff;
                    diff.key = entry.key;
                    diff.oldValue = otherEntry.value;
                    diff.newValue = entry.value;
                    diff.sourceOld = std::to_string(static_cast<int>(otherEntry.source));
                    diff.sourceNew = std::to_string(static_cast<int>(entry.source));
                    diff.changeType = "modified";
                    diff.descriptionOld = otherEntry.description;
                    diff.descriptionNew = entry.description;
                    diff.isBreakingChange = false;
                    diff.confidence = 1.0;
                    diffs.push_back(diff);
                }
                break;
            }
        }
        if (!found) {
            ConfigDiff diff;
            diff.key = entry.key;
            diff.oldValue = std::monostate{}; // null
            diff.newValue = entry.value;
            diff.sourceOld = "missing";
            diff.sourceNew = std::to_string(static_cast<int>(entry.source));
            diff.changeType = "added";
            diff.descriptionOld = "";
            diff.descriptionNew = entry.description;
            diff.isBreakingChange = false;
            diff.confidence = 1.0;
            diffs.push_back(diff);
        }
    }
    
    // Check for entries in other config not in this
    for (const auto& entry : other.pImpl->entries) {
        bool found = false;
        for (const auto& thisEntry : pImpl->entries) {
            if (entry.key == thisEntry.key) {
                found = true;
                break;
            }
        }
        if (!found) {
            ConfigDiff diff;
            diff.key = entry.key;
            diff.oldValue = entry.value;
            diff.newValue = std::monostate{}; // null
            diff.sourceOld = std::to_string(static_cast<int>(entry.source));
            diff.sourceNew = "missing";
            diff.changeType = "removed";
            diff.descriptionOld = entry.description;
            diff.descriptionNew = "";
            diff.isBreakingChange = false;
            diff.confidence = 1.0;
            diffs.push_back(diff);
        }
    }
    
    return diffs;
}

std::vector<ConfigDiff> Config::diffWith(const Config& original) const {
    return compareWith(original);
}

std::string Config::generateDiffReport(const std::vector<ConfigDiff>& diffs) const {
    std::ostringstream oss;
    oss << "Configuration Differences Report" << std::endl;
    oss << "=" << std::string(50, '=') << std::endl;
    
    if (diffs.empty()) {
        oss << "No differences found." << std::endl;
        return oss.str();
    }
    
    oss << "Total differences: " << diffs.size() << std::endl;
    oss << std::endl;
    
    for (const auto& diff : diffs) {
        oss << "Key: " << diff.key << std::endl;
        oss << "Change: " << diff.changeType << std::endl;
        
        if (diff.changeType == "modified") {
            oss << "  From: " << configValueToString(diff.oldValue) << " (" << diff.sourceOld << ")" << std::endl;
            oss << "  To:   " << configValueToString(diff.newValue) << " (" << diff.sourceNew << ")" << std::endl;
        } else if (diff.changeType == "added") {
            oss << "  Added: " << configValueToString(diff.newValue) << " (" << diff.sourceNew << ")" << std::endl;
        } else if (diff.changeType == "removed") {
            oss << "  Removed: " << configValueToString(diff.oldValue) << " (" << diff.sourceOld << ")" << std::endl;
        }
        
        oss << "  Description: " << diff.descriptionNew << std::endl;
        oss << std::endl;
    }
    
    return oss.str();
}

bool Config::detectBreakingChanges(const std::vector<ConfigDiff>& diffs, 
                                   std::vector<std::string>& breakingChanges) const {
    breakingChanges.clear();
    bool hasBreakingChanges = false;
    
    for (const auto& diff : diffs) {
        if (diff.isBreakingChange) {
            breakingChanges.push_back("Key '" + diff.key + "': " + diff.changeType + " change detected");
            hasBreakingChanges = true;
        }
    }
    
    return hasBreakingChanges;
}

ConfigValidationReport Config::generateValidationReport() const {
    ConfigValidationReport report;
    report.timestamp = std::chrono::system_clock::now();
    report.totalKeys = pImpl->entries.size();
    
    // Count valid and invalid keys
    report.validKeys = 0;
    report.errorCount = 0;
    
    for (const auto& entry : pImpl->entries) {
        // Simple validation - check if key exists
        bool isValid = true;
        std::string error;
        
        // Check validation rules
        for (const auto& rule : validationRules) {
            if (rule.key == entry.key) {
                if (!validateValue(rule, entry.value)) {
                    isValid = false;
                    error = "Validation error: " + rule.errorMessage;
                    break;
                }
            }
        }
        
        if (isValid) {
            report.validKeys++;
        } else {
            report.errorCount++;
        }
    }
    
    report.isValid = (report.errorCount == 0);
    report.validationScore = static_cast<double>(report.validKeys) / report.totalKeys;
    
    // Generate summary
    std::ostringstream oss;
    oss << "Validation Report for Configuration: " << pImpl->name << std::endl;
    oss << "Generated: " << std::chrono::system_clock::now() << std::endl;
    oss << "Overall Status: " << (report.isValid ? "VALID" : "INVALID") << std::endl;
    oss << "Validation Score: " << (report.validationScore * 100) << "%" << std::endl;
    oss << "Valid Keys: " << report.validKeys << "/" << report.totalKeys << std::endl;
    oss << "Errors: " << report.errorCount << std::endl;
    
    report.reportSummary = oss.str();
    
    return report;
}

double Config::calculateValidationScore() const {
    ConfigValidationReport report = generateValidationReport();
    return report.validationScore;
}

Config Config::createTemplate(const std::string& name, const std::string& description, 
                              const std::string& category) const {
    Config templateConfig;
    templateConfig.pImpl->name = name;
    templateConfig.pImpl->description = description;
    templateConfig.pImpl->isTemplate = true;
    templateConfig.pImpl->templateId = generateTemplateId(name);
    
    // Copy all entries
    templateConfig.pImpl->entries = pImpl->entries;
    
    return templateConfig;
}

std::vector<Config> Config::getAvailableTemplates() const {
    std::vector<Config> templates;
    // In a real implementation, this would query a template store
    // For now, return an empty vector
    return templates;
}

std::string Config::generateTemplateId(const std::string& name) const {
    // Simple hash function for template ID
    std::hash<std::string> hasher;
    return std::to_string(hasher(name)) + "_template";
}

Config Config::inheritFrom(const Config& baseConfig, const std::unordered_map<std::string, ConfigValue>& overrides,
                          const std::vector<std::string>& excludedKeys) const {
    Config inheritedConfig = baseConfig;
    
    // Merge this config into base
    for (const auto& entry : pImpl->entries) {
        if (std::find(excludedKeys.begin(), excludedKeys.end(), entry.key) == excludedKeys.end()) {
            inheritedConfig.set(entry.key, entry.value, entry.source);
        }
    }
    
    // Apply overrides
    for (const auto& [key, value] : overrides) {
        inheritedConfig.set(key, value, ConfigSource::Override);
    }
    
    return inheritedConfig;
}

Config Config::resolveInheritance(const ConfigInheritance& inheritance) const {
    Config result;
    
    if (inheritance.inheritanceStrategy == "override") {
        // Apply overrides directly
        for (const auto& [key, value] : inheritance.overrides) {
            result.set(key, value, ConfigSource::Inheritance);
        }
    } else if (inheritance.inheritanceStrategy == "merge") {
        // Merge with base config
        if (inheritance.baseConfigPath.empty()) {
            // Use default config
            result = getDefaults();
        } else {
            // Load base config (simplified - would actually load from file)
            result = getDefaults();
        }
        
        // Apply overrides
        for (const auto& [key, value] : inheritance.overrides) {
            result.set(key, value, ConfigSource::Inheritance);
        }
        
        // Apply excluded keys
        for (const auto& key : inheritance.excludedKeys) {
            result.remove(key);
        }
    }
    
    return result;
}

Config Config::getDefaults() {
    Config config;
    
    // Set default values
    config.set("random_seed", int64_t(42), ConfigSource::Default);
    config.set("simulation_timestep", 0.001, ConfigSource::Default);
    config.set("neuron_count", int64_t(1000), ConfigSource::Default);
    config.set("region_count", int64_t(2), ConfigSource::Default);
    config.set("connection_probability", 0.1, ConfigSource::Default);
    config.set("plasticity_learning_rate", 0.01, ConfigSource::Default);
    config.set("stdp_ltp_weight", 0.01, ConfigSource::Default);
    config.set("stdp_ltd_weight", 0.012, ConfigSource::Default);
    config.set("reward_discount_factor", 0.99, ConfigSource::Default);
    
    return config;
}

// Explicit template instantiations
namespace {
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
