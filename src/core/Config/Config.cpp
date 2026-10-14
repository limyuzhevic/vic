#include "core/Config/Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <ctime>

namespace nlm {

// ConfigEntry implementation
std::string ConfigEntry::getTypeName(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
    return "unknown";
}

bool ConfigEntry::validate() const {
    return validateAll().empty();
}

std::vector<ValidationError> ConfigEntry::validateAll() const {
    std::vector<ValidationError> errors;
    
    // Check required constraint
    if (required && (value.index() == std::variant_npos || std::holds_alternative<std::monostate>(value))) {
        errors.emplace_back(ValidationResult::MissingRequired, 
                          "Required key has no value", key);
    }
    
    // Check type constraints (simplified - actual constraints are handled in Config class)
    // Type validation happens at Config level
    
    return errors;
}

void ConfigEntry::addConstraint(const ConfigConstraint& constraint) {
    // This is now handled at Config class level
    // ConfigEntry no longer owns constraint details
}

bool ConfigEntry::hasConstraint(const std::string& key) const {
    return false; // Simplified for now
}

ConfigConstraint ConfigEntry::getConstraint(const std::string& key) const {
    return ConfigConstraint(); // Simplified for now
}

bool ConfigEntry::isType(const std::string& expectedType) const {
    std::string actualType = getTypeName(value);
    return actualType == expectedType;
}

bool ConfigEntry::isNumeric() const {
    return isType("int") || isType("int64") || isType("double");
}

bool ConfigEntry::isBoolean() const {
    return isType("bool");
}

bool ConfigEntry::isString() const {
    return isType("string");
}

bool ConfigEntry::isVector() const {
    return isType("vector<int>") || isType("vector<double>") || isType("vector<string>");
}

template<typename T>
std::optional<T> ConfigEntry::getAs() const {
    if (std::holds_alternative<T>(value)) {
        return std::get<T>(value);
    }
    return std::nullopt;
}

template<typename T>
bool ConfigEntry::hasValueType() const {
    return std::holds_alternative<T>(value);
}

bool ConfigEntry::operator==(const ConfigEntry& other) const {
    return key == other.key && value == other.value && source == other.source;
}

bool ConfigEntry::operator!=(const ConfigEntry& other) const {
    return !(*this == other);
}

// Config implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigConstraint> constraints;
    std::map<std::string, std::vector<ValidationCallback>> validators;
    bool strictValidation;
    std::vector<std::string> appliedPresets;
    
    Impl() : strictValidation(false) {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Load default constraints
    auto defaultConstraints = getDefaultConstraints();
    for (const auto& constraint : defaultConstraints) {
        pImpl->constraints[constraint.key] = constraint;
    }
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, simple key=value parsing
    // In a real implementation, use JSON parsing
    std::string line;
    while (std::getline(file, line)) {
        size_t equalsPos = line.find('=');
        if (equalsPos != std::string::npos) {
            std::string key = trim(line.substr(0, equalsPos));
            std::string value = trim(line.substr(equalsPos + 1));
            
            // Parse value based on content
            ConfigValue configValue;
            if (value == "true" || value == "false") {
                configValue = (value == "true");
            } else if (value.find('.') != std::string::npos) {
                try {
                    configValue = std::stod(value);
                } catch (...) {
                    configValue = value;
                }
            } else {
                try {
                    configValue = std::stoll(value);
                } catch (...) {
                    configValue = value;
                }
            }
            
            set(key, configValue, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg.substr(0, 7) == "--config") {
            if (arg.find('=') != std::string::npos) {
                std::string key = trim(arg.substr(7, arg.find('=') - 7));
                std::string value = trim(arg.substr(arg.find('=') + 1));
                set(key, value, ConfigSource::CommandLine);
            } else if (i + 1 < argc) {
                std::string key = trim(arg.substr(7));
                std::string value = trim(argv[++i]);
                set(key, value, ConfigSource::CommandLine);
            }
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
        file << entry.key << " = " << formatValue(entry.value) << std::endl;
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key && std::holds_alternative<T>(entry.value)) {
            return std::get<T>(entry.value);
        }
    }
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto value = get<T>(key);
    if (value.has_value()) {
        return *value;
    }
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    bool found = false;
    for (auto& entry : pImpl->entries) {
        if (entry.key == key) {
            entry.value = value;
            entry.source = source;
            found = true;
            break;
        }
    }
    
    if (!found) {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Try to parse as boolean
    if (value == "true") {
        set(key, true, source);
        return;
    } else if (value == "false") {
        set(key, false, source);
        return;
    }
    
    // Try to parse as number
    try {
        if (value.find('.') != std::string::npos) {
            set(key, std::stod(value), source);
        } else {
            set(key, std::stoll(value), source);
        }
        return;
    } catch (...) {}
    
    // Fall back to string
    set(key, value, source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, static_cast<int64_t>(value), source);
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

bool Config::has(const std::string& key) const {
    for (const auto& entry : pImpl->entries) {
        if (entry.key == key) {
            return true;
        }
    }
    return false;
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& entry) { return entry.key == key; }),
        pImpl->entries.end()
    );
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getRequiredKeys() const {
    std::vector<std::string> requiredKeys;
    for (const auto& entry : pImpl->entries) {
        if (entry.required) {
            requiredKeys.push_back(entry.key);
        }
    }
    return requiredKeys;
}

std::vector<std::string> Config::getOptionalKeys() const {
    std::vector<std::string> optionalKeys;
    for (const auto& entry : pImpl->entries) {
        if (!entry.required) {
            optionalKeys.push_back(entry.key);
        }
    }
    return optionalKeys;
}

void Config::addConstraint(const ConfigConstraint& constraint) {
    pImpl->constraints[constraint.key] = constraint;
    
    // Update existing entries with this constraint
    for (auto& entry : pImpl->entries) {
        if (entry.key == constraint.key) {
            entry.required = constraint.required;
            entry.description = constraint.description;
            break;
        }
    }
}

void Config::addValidator(const std::string& key, const ValidationCallback& validator) {
    pImpl->validators[key].push_back(validator);
}

void Config::setRequired(const std::string& key, bool required) {
    auto it = pImpl->constraints.find(key);
    if (it != pImpl->constraints.end()) {
        it->second.required = required;
    }
    
    for (auto& entry : pImpl->entries) {
        if (entry.key == key) {
            entry.required = required;
            break;
        }
    }
}

std::vector<ValidationError> Config::validateAll() const {
    std::vector<ValidationError> allErrors;
    
    for (const auto& entry : pImpl->entries) {
        auto errors = validate(entry.key);
        allErrors.insert(allErrors.end(), errors.begin(), errors.end());
    }
    
    return allErrors;
}

std::vector<ValidationError> Config::validate(const std::string& key) const {
    std::vector<ValidationError> errors;
    
    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                               [&key](const ConfigEntry& e) { return e.key == key; });
    if (entryIt == pImpl->entries.end()) {
        return errors; // Key doesn't exist, no validation needed
    }
    
    const auto& entry = *entryIt;
    
    // Validate constraints
    auto it = pImpl->constraints.find(key);
    if (it != pImpl->constraints.end()) {
        const auto& constraint = it->second;
        
        // Check required
        if (constraint.required && (entry.value.index() == std::variant_npos || std::holds_alternative<std::monostate>(entry.value))) {
            errors.emplace_back(ValidationResult::MissingRequired,
                              "Required key has no value", key);
        }
        
        // Check type
        std::string actualType = ConfigEntry::getTypeName(entry.value);
        if (actualType != constraint.type && !constraint.type.empty()) {
            errors.emplace_back(ValidationResult::TypeMismatch,
                              "Value type mismatch", key, constraint.type, actualType);
        }
        
        // Check range for numeric types
        if (constraint.minValue.has_value() || constraint.maxValue.has_value()) {
            if (entry.isNumeric()) {
                double value = 0.0;
                if (entry.isType("int")) {
                    auto val = entry.getAs<int>();
                    if (val.has_value()) value = *val;
                } else if (entry.isType("int64")) {
                    auto val = entry.getAs<int64_t>();
                    if (val.has_value()) value = *val;
                } else if (entry.isType("double")) {
                    auto val = entry.getAs<double>();
                    if (val.has_value()) value = *val;
                }
                
                if (constraint.minValue.has_value() && value < constraint.minValue.value()) {
                    errors.emplace_back(ValidationResult::RangeViolation,
                                      "Value below minimum", key, 
                                      ">= " + std::to_string(constraint.minValue.value()),
                                      std::to_string(value));
                }
                
                if (constraint.maxValue.has_value() && value > constraint.maxValue.value()) {
                    errors.emplace_back(ValidationResult::RangeViolation,
                                      "Value above maximum", key,
                                      "<= " + std::to_string(constraint.maxValue.value()),
                                      std::to_string(value));
                }
            }
        }
        
        // Check allowed values
        if (!constraint.allowedValues.empty() && entry.isString()) {
            auto val = entry.getAs<std::string>();
            if (val.has_value()) {
                if (std::find(constraint.allowedValues.begin(), constraint.allowedValues.end(), *val) == constraint.allowedValues.end()) {
                    errors.emplace_back(ValidationResult::InvalidValue,
                                      "Value not in allowed list", key,
                                      "one of: " + std::string(1, constraint.allowedValues[0]),
                                      "'" + *val + "'");
                }
            }
        }
    }
    
    // Validate custom validators
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt != pImpl->validators.end()) {
        for (const auto& validator : validatorIt->second) {
            ValidationError error = validator(key, entry.value);
            if (error.result != ValidationResult::Success) {
                errors.push_back(error);
            }
        }
    }
    
    return errors;
}

bool Config::isValid(const std::string& key) const {
    return validate(key).empty();
}

bool Config::isValidAll() const {
    return validateAll().empty();
}

std::vector<ConfigConstraint> Config::getDefaultConstraints() {
    return {
        ConfigConstraint("neuron_count", "Number of neurons in the brain", "int64", true, "1000"),
        ConfigConstraint("region_count", "Number of brain regions", "int64", false, "1"),
        ConfigConstraint("connection_probability", "Probability of synapse formation", "double", false, "0.1"),
        ConfigConstraint("stdp_ltp_weight", "STDP long-term potentiation weight", "double", false, "0.01"),
        ConfigConstraint("stdp_ltd_weight", "STDP long-term depression weight", "double", false, "0.012"),
        ConfigConstraint("synaptogenesis_rate", "Structural plasticity synaptogenesis rate", "double", false, "0.0001"),
        ConfigConstraint("pruning_rate", "Structural plasticity pruning rate", "double", false, "0.00001"),
        ConfigConstraint("replay_interval", "Memory replay interval in steps", "int64", false, "100"),
        ConfigConstraint("consolidation_interval", "Memory consolidation interval in steps", "int64", false, "1000"),
        ConfigConstraint("simulation_timestep", "Simulation time step in seconds", "double", false, "0.001"),
        ConfigConstraint("checkpoint_dir", "Directory for checkpoint files", "string", false, "./checkpoints"),
        ConfigConstraint("random_seed", "Random seed for reproducibility", "int64", false, "42"),
    };
}

void Config::applyPreset(const std::string& presetName) {
    pImpl->appliedPresets.push_back(presetName);
    
    if (presetName == "minimal") {
        set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        set("connection_probability", 0.05f, ConfigSource::Default);
    } else if (presetName == "standard") {
        set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        set("connection_probability", 0.1f, ConfigSource::Default);
        set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
        set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    } else if (presetName == "high_performance") {
        set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
        set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
        set("connection_probability", 0.15f, ConfigSource::Default);
        set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        set("stdp_ltd_weight", 0.018f, ConfigSource::Default);
    } else if (presetName == "research") {
        set("neuron_count", static_cast<int64_t>(10000), ConfigSource::Default);
        set("region_count", static_cast<int64_t>(8), ConfigSource::Default);
        set("connection_probability", 0.2f, ConfigSource::Default);
        set("stdp_ltp_weight", 0.03f, ConfigSource::Default);
        set("stdp_ltd_weight", 0.025f, ConfigSource::Default);
        set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
        set("pruning_rate", 0.00002f, ConfigSource::Default);
    }
}

void Config::createPreset(const std::string& presetName, const std::vector<ConfigConstraint>& constraints) {
    // Store preset (simplified - would need persistent storage in real implementation)
    pImpl->appliedPresets.push_back(presetName);
}

std::vector<ConfigConstraint> Config::getConstraints(const std::string& key) const {
    std::vector<ConfigConstraint> constraints;
    auto it = pImpl->constraints.find(key);
    if (it != pImpl->constraints.end()) {
        constraints.push_back(it->second);
    }
    return constraints;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->appliedPresets.clear();
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "NLM Configuration Summary" << std::endl;
    ss << "=======================" << std::endl;
    ss << "Total parameters: " << pImpl->entries.size() << std::endl;
    ss << "Applied presets: " << pImpl->appliedPresets.size() << std::endl;
    
    if (!pImpl->entries.empty()) {
        ss << std::endl << "Configuration values:" << std::endl;
        for (const auto& entry : pImpl->entries) {
            ss << "  " << entry.key << " = " << formatValue(entry.value)
               << " (source: " << static_cast<int>(entry.source) << ")" << std::endl;
        }
    }
    
    if (!pImpl->appliedPresets.empty()) {
        ss << std::endl << "Applied presets:" << std::endl;
        for (const auto& preset : pImpl->appliedPresets) {
            ss << "  - " << preset << std::endl;
        }
    }
    
    return ss.str();
}

std::string Config::validationSummary() const {
    std::vector<ValidationError> errors = validateAll();
    
    std::stringstream ss;
    ss << "Configuration Validation Summary" << std::endl;
    ss << "===============================" << std::endl;
    ss << "Total parameters: " << pImpl->entries.size() << std::endl;
    ss << "Valid parameters: " << (pImpl->entries.size() - errors.size()) << std::endl;
    ss << "Invalid parameters: " << errors.size() << std::endl;
    
    if (!errors.empty()) {
        ss << std::endl << "Validation errors:" << std::endl;
        for (const auto& error : errors) {
            ss << "  - " << error.message << std::endl;
        }
    } else {
        ss << std::endl << "All parameters are valid!" << std::endl;
    }
    
    return ss.str();
}

bool Config::equals(const Config& other) const {
    return pImpl->entries == other.pImpl->entries;
}

std::vector<std::string> Config::diff(const Config& other) const {
    std::vector<std::string> differences;
    
    // Check for differences in current config
    for (const auto& entry : pImpl->entries) {
        bool found = false;
        for (const auto& otherEntry : other.pImpl->entries) {
            if (entry.key == otherEntry.key && entry.value != otherEntry.value) {
                differences.push_back("  " + entry.key + ": " + formatValue(entry.value) + " -> " + formatValue(otherEntry.value));
                found = true;
                break;
            }
        }
        if (!found) {
            differences.push_back("  " + entry.key + ": present -> missing");
        }
    }
    
    // Check for missing entries in other config
    for (const auto& otherEntry : other.pImpl->entries) {
        bool found = false;
        for (const auto& entry : pImpl->entries) {
            if (entry.key == otherEntry.key) {
                found = true;
                break;
            }
        }
        if (!found) {
            differences.push_back("  missing -> " + otherEntry.key + ": " + formatValue(otherEntry.value));
        }
    }
    
    return differences;
}

bool Config::isValidJsonKey(const std::string& key) {
    return !key.empty() && key.find('.') == std::string::npos;
}

std::string Config::sanitizeJsonKey(const std::string& key) {
    std::string result;
    for (char c : key) {
        if (std::isalnum(c) || c == '_' || c == '-') {
            result += c;
        } else if (c == ' ') {
            result += '_';
        }
    }
    return result;
}

std::string Config::formatValue(const ConfigValue& value) {
    std::stringstream ss;
    
    if (std::holds_alternative<int>(value)) {
        ss << std::get<int>(value);
    } else if (std::holds_alternative<int64_t>(value)) {
        ss << std::get<int64_t>(value);
    } else if (std::holds_alternative<double>(value)) {
        ss << std::fixed << std::setprecision(6) << std::get<double>(value);
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
            ss << std::fixed << std::setprecision(3) << vec[i];
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

std::string Config::trim(const std::string& str) {
    size_t start = 0;
    size_t end = str.size();
    
    while (start < end && std::isspace(str[start])) {
        start++;
    }
    
    while (end > start && std::isspace(str[end - 1])) {
        end--;
    }
    
    return str.substr(start, end - start);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(c);
    }
    return result;
}

bool Config::stringToBool(const std::string& str) {
    std::string lower = toLower(str);
    return (lower == "true" || lower == "yes" || lower == "1" || lower == "on");
}

std::string Config::boolToString(bool value) {
    return value ? "true" : "false";
}

std::string Config::generateConfigTemplate(const std::vector<ConfigConstraint>& constraints) {
    std::stringstream ss;
    ss << "# NLM Configuration Template" << std::endl;
    ss << "# Copy this file and modify values as needed" << std::endl;
    ss << "# Valid types: int64, double, bool, string, vector<type>" << std::endl;
    ss << std::endl;
    
    for (const auto& constraint : constraints) {
        ss << "# " << constraint.description << std::endl;
        ss << "# Type: " << constraint.type;
        if (constraint.minValue.has_value()) ss << ", Min: " << constraint.minValue.value();
        if (constraint.maxValue.has_value()) ss << ", Max: " << constraint.maxValue.value();
        if (!constraint.allowedValues.empty()) {
            ss << ", Allowed: ";
            for (size_t i = 0; i < constraint.allowedValues.size(); ++i) {
                if (i > 0) ss << ", ";
                ss << constraint.allowedValues[i];
            }
        }
        ss << ", Required: " << (constraint.required ? "Yes" : "No");
        ss << ", Default: " << constraint.defaultValue << std::endl;
        
        if (constraint.required) {
            ss << constraint.key << " = <value>" << std::endl;
        } else {
            ss << "# " << constraint.key << " = " << constraint.defaultValue << std::endl;
        }
        ss << std::endl;
    }
    
    return ss.str();
}

std::vector<ConfigConstraint> Config::parseConfigTemplate(const std::string& templateContent) {
    // Simplified parser for template files
    std::vector<ConfigConstraint> constraints;
    std::stringstream ss(templateContent);
    std::string line;
    
    ConfigConstraint current;
    bool inConstraint = false;
    
    while (std::getline(ss, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') {
            continue;
        }
        
        if (line.find("# Type:") == 0) {
            if (!current.key.empty()) {
                constraints.push_back(current);
                current = ConfigConstraint();
            }
            current.type = line.substr(7);
            trim(current.type);
        } else if (line.find("Type:") == 0) {
            if (!current.key.empty()) {
                constraints.push_back(current);
                current = ConfigConstraint();
            }
            current.type = line.substr(6);
            trim(current.type);
        } else if (line.find("Min:") != std::string::npos) {
            try {
                current.minValue = std::stod(line.substr(line.find("Min:") + 4));
            } catch (...) {}
        } else if (line.find("Max:") != std::string::npos) {
            try {
                current.maxValue = std::stod(line.substr(line.find("Max:") + 4));
            } catch (...) {}
        } else if (line.find("Allowed:") != std::string::npos) {
            size_t pos = line.find("Allowed:") + 8;
            // Extract allowed values (simplified)
            // Would need proper parsing for full implementation
        } else if (line.find("Required:") != std::string::npos) {
            current.required = (line.find("Yes") != std::string::npos);
        } else if (line.find("Default:") != std::string::npos) {
            size_t pos = line.find("Default:") + 8;
            current.defaultValue = line.substr(pos);
            trim(current.defaultValue);
        } else if (line.find('=') != std::string::npos) {
            if (!current.key.empty()) {
                constraints.push_back(current);
                current = ConfigConstraint();
            }
            
            size_t equalsPos = line.find('=');
            current.key = trim(line.substr(0, equalsPos));
            // Value would be parsed separately in real implementation
        }
    }
    
    if (!current.key.empty()) {
        constraints.push_back(current);
    }
    
    return constraints;
}

} // namespace nlm
