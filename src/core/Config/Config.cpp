#include "Config.hpp"
#include "ConfigError.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <limits>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigValidationRule> validationRules;
    std::vector<std::unique_ptr<ConfigValidatorBase>> registeredValidators;
    ConfigValidationException::Level validationLevel;
    bool validationEnabled;
    
    Impl() : validationLevel(ConfigValidationException::Level::Full), validationEnabled(true) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    ConfigValidatorRegistry::getInstance().registerValidator(
        std::make_unique<BrainConfigValidator>());
    ConfigValidatorRegistry::getInstance().registerValidator(
        std::make_unique<PlasticityConfigValidator>());
    ConfigValidatorRegistry::getInstance().registerValidator(
        std::make_unique<NeuromodulationConfigValidator>());
    ConfigValidatorRegistry::getInstance().registerValidator(
        std::make_unique<MemoryConfigValidator>());
    ConfigValidatorRegistry::getInstance().registerValidator(
        std::make_unique<CognitionConfigValidator>());
    ConfigValidatorRegistry::getInstance().registerValidator(
        std::make_unique<DevelopmentConfigValidator>());
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
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
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Validate before setting (runtime validation)
        if (validationEnabled) {
            validateKeyType(key, value);
            validateKeyRange(key, value);
            applyCustomValidators(key, value);
        }
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

// Validation implementation
void Config::validate(ConfigValidationException::Level level) {
    if (!validationEnabled) return;
    
    std::vector<std::string> errors;
    
    // Validate key types
    for (const auto& entry : pImpl->entries) {
        try {
            validateKeyType(entry.key, entry.value);
            validateKeyRange(entry.key, entry.value);
            applyCustomValidators(entry.key, entry.value);
            validateDependencies(entry.key, *this);
            validateRequired(entry.key, *this);
        } catch (const ConfigValidationError& e) {
            errors.push_back("Key " + entry.key + ": " + e.what());
        }
    }
    
    // Apply category-specific validators
    try {
        validateBrainCategory(*this);
        validatePlasticityCategory(*this);
        validateNeuromodulationCategory(*this);
        validateMemoryCategory(*this);
        validateCognitionCategory(*this);
        validateDevelopmentCategory(*this);
    } catch (const ConfigValidationError& e) {
        errors.push_back(std::string("Category validation: ") + e.what());
    }
    
    if (!errors.empty()) {
        std::ostringstream oss;
        oss << "Configuration validation failed with " << errors.size() << " error(s):\n";
        for (size_t i = 0; i < errors.size(); ++i) {
            oss << "  " << (i + 1) << ". " << errors[i] << "\n";
        }
        
        if (level >= ConfigValidationException::Level::Full) {
            std::cout << "[Validation Error] " << oss.str() << std::endl;
        }
        
        throw ConfigValidationError(oss.str());
    }
}

std::vector<std::string> Config::validate() const {
    std::vector<std::string> errors;
    
    for (const auto& entry : pImpl->entries) {
        try {
            validateKeyType(entry.key, entry.value);
            validateKeyRange(entry.key, entry.value);
            applyCustomValidators(entry.key, entry.value);
            validateDependencies(entry.key, *this);
            validateRequired(entry.key, *this);
        } catch (const ConfigValidationError& e) {
            errors.push_back("Key " + entry.key + ": " + e.what());
        }
    }
    
    return errors;
}

bool Config::isValid() const {
    return validate().empty();
}

void Config::addValidationRule(const ConfigValidationRule& rule) {
    pImpl->validationRules.push_back(rule);
}

void Config::addValidationRule(const std::string& key, const std::string& name, 
                               ConfigValidationType type, bool required,
                               std::optional<double> minValue,
                               std::optional<double> maxValue) {
    ConfigValidationRule rule;
    rule.key = key;
    rule.name = name;
    rule.type = type;
    rule.required = required;
    rule.minValue = minValue;
    rule.maxValue = maxValue;
    addValidationRule(rule);
}

void Config::removeValidationRule(const std::string& key) {
    auto it = std::remove_if(pImpl->validationRules.begin(), pImpl->validationRules.end(),
        [&key](const ConfigValidationRule& rule) { return rule.key == key; });
    pImpl->validationRules.erase(it, pImpl->validationRules.end());
}

void Config::addValidator(std::unique_ptr<ConfigValidatorBase> validator) {
    pImpl->registeredValidators.push_back(std::move(validator));
    ConfigValidatorRegistry::getInstance().registerValidator(
        std::make_unique<BrainConfigValidator>());
    ConfigValidatorRegistry::getInstance().registerValidator(
        std::make_unique<PlasticityConfigValidator>());
    ConfigValidatorRegistry::getInstance().registerValidator(
        std::make_unique<NeuromodulationConfigValidator>());
    ConfigValidatorRegistry::getInstance().registerValidator(
        std::make_unique<MemoryConfigValidator>());
    ConfigValidatorRegistry::getInstance().registerValidator(
        std::make_unique<CognitionConfigValidator>());
    ConfigValidatorRegistry::getInstance().registerValidator(
        std::make_unique<DevelopmentConfigValidator>());
}

void Config::setValidationLevel(ConfigValidationException::Level level) {
    pImpl->validationLevel = level;
    validationEnabled = (level != ConfigValidationException::Level::Silent);
}

void Config::setValidationEnabled(bool enabled) {
    pImpl->validationEnabled = enabled;
}

bool Config::isValidationEnabled() const {
    return validationEnabled;
}

void Config::preValidate() const {
    if (!validationEnabled) return;
    
    validate(ConfigValidationException::Level::Full);
}

// Private validation methods
void Config::validateKeyType(const std::string& key, const ConfigValue& value) const {
    // Find validation rule for this key
    auto ruleIt = std::find_if(pImpl->validationRules.begin(), pImpl->validationRules.end(),
        [&key](const ConfigValidationRule& rule) { return rule.key == key; });
    
    if (ruleIt != pImpl->validationRules.end()) {
        const auto& rule = *ruleIt;
        bool typeMatch = false;
        
        switch (rule.type) {
            case ConfigValidationType::Integer:
                typeMatch = std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
                break;
            case ConfigValidationType::Float:
                typeMatch = std::holds_alternative<double>(value);
                break;
            case ConfigValidationType::Boolean:
                typeMatch = std::holds_alternative<bool>(value);
                break;
            case ConfigValidationType::String:
                typeMatch = std::holds_alternative<std::string>(value);
                break;
            case ConfigValidationType::VectorInt:
                typeMatch = std::holds_alternative<std::vector<int>>(value);
                break;
            case ConfigValidationType::VectorFloat:
                typeMatch = std::holds_alternative<std::vector<double>>(value);
                break;
            case ConfigValidationType::VectorString:
                typeMatch = std::holds_alternative<std::vector<std::string>>(value);
                break;
        }
        
        if (!typeMatch) {
            std::string actualType;
            if (std::holds_alternative<int>(value)) actualType = "integer";
            else if (std::holds_alternative<int64_t>(value)) actualType = "int64";
            else if (std::holds_alternative<double>(value)) actualType = "float";
            else if (std::holds_alternative<bool>(value)) actualType = "boolean";
            else if (std::holds_alternative<std::string>(value)) actualType = "string";
            else if (std::holds_alternative<std::vector<int>>(value)) actualType = "vector<int>";
            else if (std::holds_alternative<std::vector<double>>(value)) actualType = "vector<float>";
            else if (std::holds_alternative<std::vector<std::string>>(value)) actualType = "vector<string>";
            
            throw ConfigTypeError("Invalid type for configuration key '" + key + "'. Expected: " + 
                                 typeToString(rule.type) + ", Actual: " + actualType, key);
        }
    }
}

void Config::validateKeyRange(const std::string& key, const ConfigValue& value) const {
    // Find validation rule for this key
    auto ruleIt = std::find_if(pImpl->validationRules.begin(), pImpl->validationRules.end(),
        [&key](const ConfigValidationRule& rule) { return rule.key == key; });
    
    if (ruleIt != pImpl->validationRules.end()) {
        const auto& rule = *ruleIt;
        
        // Get numeric value if applicable
        double numValue = getNumericValue(value);
        
        // Check min/max constraints
        if (rule.minValue.has_value() && numValue < rule.minValue.value()) {
            throw ConfigRangeError("Value " + std::to_string(numValue) + " for key '" + key + 
                                 "' is below minimum " + std::to_string(rule.minValue.value()),
                                 key, numValue, rule.minValue.value(), std::numeric_limits<double>::max());
        }
        
        if (rule.maxValue.has_value() && numValue > rule.maxValue.value()) {
            throw ConfigRangeError("Value " + std::to_string(numValue) + " for key '" + key + 
                                 "' exceeds maximum " + std::to_string(rule.maxValue.value()),
                                 key, numValue, rule.minValue.value(), rule.maxValue.value());
        }
    }
}

void Config::validateDependencies(const std::string& key, const Config& config) const {
    // Find validation rule for this key
    auto ruleIt = std::find_if(pImpl->validationRules.begin(), pImpl->validationRules.end(),
        [&key](const ConfigValidationRule& rule) { return rule.key == key; });
    
    if (ruleIt != pImpl->validationRules.end()) {
        const auto& rule = *ruleIt;
        
        for (const auto& dependency : rule.dependencies) {
            if (!config.has(dependency.first) && !config.getOr<bool>(dependency.first, false)) {
                throw ConfigDependencyError("Required dependency '" + dependency.first + 
                                           "' for key '" + key + "' is not set or false",
                                           key, dependency.first);
            }
        }
    }
}

void Config::validateRequired(const std::string& key, const Config& config) const {
    // Find validation rule for this key
    auto ruleIt = std::find_if(pImpl->validationRules.begin(), pImpl->validationRules.end(),
        [&key](const ConfigValidationRule& rule) { return rule.key == key; });
    
    if (ruleIt != pImpl->validationRules.end()) {
        const auto& rule = *ruleIt;
        
        if (rule.required && !config.has(key)) {
            throw ConfigValidationError("Required configuration key '" + key + "' is missing", key);
        }
        
        // Check requiredIf conditions
        for (const auto& requiredIfKey : rule.requiredIf) {
            if (config.has(requiredIfKey) && config.getOr<bool>(requiredIfKey, false)) {
                if (!config.has(key)) {
                    throw ConfigValidationError("Configuration key '" + key + 
                                               "' is required when '" + requiredIfKey + 
                                               "' is true", key);
                }
            }
        }
    }
}

void Config::applyCustomValidators(const std::string& key, const ConfigValue& value) const {
    // Find validation rule for this key
    auto ruleIt = std::find_if(pImpl->validationRules.begin(), pImpl->validationRules.end(),
        [&key](const ConfigValidationRule& rule) { return rule.key == key; });
    
    if (ruleIt != pImpl->validationRules.end()) {
        const auto& rule = *ruleIt;
        
        if (rule.customValidator && !rule.customValidator(value)) {
            throw ConfigValidationError("Custom validation failed for key '" + key + "'", key);
        }
    }
}

bool Config::isKeyTypeValid(const std::string& key, const ConfigValue& value) const {
    try {
        validateKeyType(key, value);
        validateKeyRange(key, value);
        applyCustomValidators(key, value);
        validateDependencies(key, *this);
        validateRequired(key, *this);
        return true;
    } catch (...) {
        return false;
    }
}

double Config::getNumericValue(const ConfigValue& value) const {
    if (std::holds_alternative<int>(value)) {
        return static_cast<double>(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return static_cast<double>(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    } else if (std::holds_alternative<bool>(value)) {
        return std::get<bool>(value) ? 1.0 : 0.0;
    } else if (std::holds_alternative<std::string>(value)) {
        try {
            return std::stod(std::get<std::string>(value));
        } catch (...) {
            return 0.0;
        }
    }
    return 0.0;
}

// Category validation hooks
void Config::validateBrainCategory(const Config& config) const {
    // This is handled by BrainConfigValidator
}

void Config::validatePlasticityCategory(const Config& config) const {
    // This is handled by PlasticityConfigValidator
}

void Config::validateNeuromodulationCategory(const Config& config) const {
    // This is handled by NeuromodulationConfigValidator
}

void Config::validateMemoryCategory(const Config& config) const {
    // This is handled by MemoryConfigValidator
}

void Config::validateCognitionCategory(const Config& config) const {
    // This is handled by CognitionConfigValidator
}

void Config::validateDevelopmentCategory(const Config& config) const {
    // This is handled by DevelopmentConfigValidator
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