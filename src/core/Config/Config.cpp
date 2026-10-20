#include "Config.hpp"
#include "Validation.hpp"
#include "ValidationUtils.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    ValidationMode validationMode;
    std::unordered_map<std::string, ConfigCategory> keyToCategory;
};

Config::Config() : pImpl(std::make_unique<Impl>()), currentValidationMode(ValidationMode::Strict) {
    pImpl->validationMode = ValidationMode::Strict;
    initializeKeyCategoryMapping();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeKeyCategoryMapping() {
    // Map configuration keys to categories
    pImpl->keyToCategory = {
        // Neuromodulation
        {"dopamine_baseline", ConfigCategory::Neuromodulation},
        {"reward_discount_factor", ConfigCategory::Neuromodulation},
        {"curiosity", ConfigCategory::Neuromodulation},
        {"novelty_sensitivity", ConfigCategory::Neuromodulation},
        {"prediction_error_baseline", ConfigCategory::Neuromodulation},
        
        // Simulation
        {"simulation_timestep", ConfigCategory::Simulation},
        {"max_simulation_steps", ConfigCategory::Simulation},
        {"simulation_time_limit", ConfigCategory::Simulation},
        {"random_seed", ConfigCategory::Simulation},
        
        // Development
        {"synaptogenesis_rate", ConfigCategory::Development},
        {"pruning_rate", ConfigCategory::Development},
        {"development_threshold", ConfigCategory::Development},
        {"critical_period_end", ConfigCategory::Development},
        
        // Memory
        {"working_memory_capacity", ConfigCategory::Memory},
        {"working_memory_decay_rate", ConfigCategory::Memory},
        {"episodic_memory_capacity", ConfigCategory::Memory},
        {"associative_memory_capacity", ConfigCategory::Memory},
        
        // Environment
        {"width", ConfigCategory::Environment},
        {"height", ConfigCategory::Environment},
        {"environment_name", ConfigCategory::Environment},
        {"wall_penalty", ConfigCategory::Environment},
        
        // Logging
        {"log_level", ConfigCategory::Logging},
        {"log_to_file", ConfigCategory::Logging},
        {"log_filename", ConfigCategory::Logging},
        
        // Visualization
        {"visualization_enabled", ConfigCategory::Visualization},
        {"visualization_update_rate", ConfigCategory::Visualization},
        {"visualization_size", ConfigCategory::Visualization},
        
        // Reward
        {"reward_discount_factor", ConfigCategory::Reward},
        {"reward_scaling", ConfigCategory::Reward},
    };
}

ConfigCategory Config::detectCategory(const std::string& key) {
    if (auto it = pImpl->keyToCategory.find(key);
        it != pImpl->keyToCategory.end()) {
        return it->second;
    }
    
    // Fallback: try to detect based on schema
    const auto& schemaManager = ConfigSchemaManager::getInstance();
    for (const auto& pair : schemaManager.getAllSchemas()) {
        const auto& schema = pair.second;
        if (schema.rules.find(key) != schema.rules.end()) {
            return schema.category;
        }
    }
    
    return ConfigCategory::Unknown;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Check file extension for format detection
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    // For now, use simple format only since we don't have JSON/YAML parsers
    // TODO: Implement JSON/YAML parsing in Phase 2
    return loadFromSimpleFormat(filepath);
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    clear();
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
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
            
            // Convert value based on type
            ConfigValue configValue;
            if (value.find('.') != std::string::npos || 
                (value[0] == '-' && value.find('.') != std::string::npos)) {
                // Try to parse as double
                try {
                    configValue = std::stod(value);
                } catch (...) {
                    configValue = value;
                }
            } else if (value == "true" || value == "false") {
                configValue = (value == "true");
            } else {
                // Try to parse as integer
                try {
                    configValue = std::stoi(value);
                } catch (...) {
                    // Try to parse as int64_t
                    try {
                        configValue = std::stoll(value);
                    } catch (...) {
                        configValue = value;
                    }
                }
            }
            
            set(key, configValue, ConfigSource::File);
        }
    }
    
    // Validate the loaded configuration
    validateAll();
    
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
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n";
    }
    
    return true;
}

std::optional<int64_t> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<int64_t>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

std::optional<double> Config::getDouble(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<double>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

std::optional<bool> Config::getBool(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<bool>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

std::optional<std::string> Config::getString(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<std::string>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

std::string Config::getOrString(const std::string& key, const std::string& defaultValue) const {
    auto val = getString(key);
    return val.has_value() ? *val : defaultValue;
}

int64_t Config::getOrInt64(const std::string& key, int64_t defaultValue) const {
    auto val = get(key);
    return val.has_value() ? *val : defaultValue;
}

double Config::getOrDouble(const std::string& key, double defaultValue) const {
    auto val = getDouble(key);
    return val.has_value() ? *val : defaultValue;
}

bool Config::getOrBool(const std::string& key, bool defaultValue) const {
    auto val = getBool(key);
    return val.has_value() ? *val : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
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

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
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

bool Config::validate(const std::string& category) const {
    std::vector<ValidationError> errors;
    return ValidationImplementation::validateConfig(*this, detectCategory(category), errors);
}

std::vector<ValidationError> Config::getValidationErrors(const std::string& category) const {
    std::vector<ValidationError> errors;
    ValidationImplementation::validateConfig(*this, detectCategory(category), errors);
    return errors;
}

void Config::setValidationMode(ValidationMode mode) {
    currentValidationMode = mode;
    pImpl->validationMode = mode;
}

ValidationMode Config::getValidationMode() const {
    return currentValidationMode;
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

void Config::validateAll() {
    // Get all entries as a map for validation
    std::unordered_map<std::string, ConfigValue> configData;
    for (const auto& entry : pImpl->entries) {
        configData[entry.key] = entry.value;
    }
    
    // Validate based on categories
    ConfigCategory currentCategory = ConfigCategory::Unknown;
    
    for (const auto& entry : pImpl->entries) {
        ConfigCategory entryCategory = detectCategory(entry.key);
        if (entryCategory != ConfigCategory::Unknown && entryCategory != currentCategory) {
            currentCategory = entryCategory;
            
            ConfigValidator validator;
            validator.setMode(pImpl->validationMode);
            
            auto errors = validator.validateAll(configData, currentCategory);
            if (!errors.empty() && currentValidationMode == ValidationMode::Strict) {
                // In strict mode, we could log errors here
                for (const auto& error : errors) {
                    // Log validation error
                }
            }
        }
    }
}

} // namespace nlm

// Implementation of ValidationImplementation
namespace nlm {

bool ValidationImplementation::validateConfig(const Config& config, ConfigCategory category, 
                                              std::vector<ValidationError>& errors) {
    const auto& validator = ConfigValidator::getInstance();
    validator.setMode(config.getValidationMode());
    
    // Get all entries as a map for validation
    std::unordered_map<std::string, ConfigValue> configData;
    for (const auto& entry : config.pImpl->entries) {
        configData[entry.key] = entry.value;
    }
    
    return validator.validateAll(configData, category);
}

ValidationMode ValidationImplementation::getValidationMode(const Config& config) {
    return config.getValidationMode();
}

ConfigCategory ValidationImplementation::detectCategory(const std::string& key) {
    return Config::detectCategory(key);
}

} // namespace nlm