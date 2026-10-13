#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <ctime>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ParameterSchema> parameterSchemas;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    initializeParameterSchemas();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;
Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeParameterSchemas() {
    // Define parameter schemas for NLM Phase 2 configuration
    pImpl->parameterSchemas = {
        // Neural system parameters
        ParameterSchema("neuron_count", "int", "Number of neurons in the network", true, ConfigValue(500), 1, 10000),
        ParameterSchema("region_count", "int", "Number of neural regions", true, ConfigValue(1), 1, 100),
        ParameterSchema("connection_probability", "float", "Probability of synaptic connections between neurons", true, ConfigValue(0.15f), 0.0, 1.0),
        
        // Simulation parameters
        ParameterSchema("simulation_timestep", "double", "Time step for simulation in seconds", true, ConfigValue(0.001), 1e-6, 0.1),
        ParameterSchema("simulation_duration", "double", "Total simulation duration in seconds", false, ConfigValue(10.0), 0.0, 3600.0),
        
        // STDP parameters
        ParameterSchema("stdp_ltp_weight", "float", "STDP long-term potentiation weight factor", true, ConfigValue(0.02f), 0.0, 1.0),
        ParameterSchema("stdp_ltd_weight", "float", "STDP long-term depression weight factor", true, ConfigValue(0.015f), 0.0, 1.0),
        ParameterSchema("stdp_tau", "float", "STDP time constant for eligibility traces", true, ConfigValue(20.0f), 0.0, 100.0),
        
        // Structural plasticity parameters
        ParameterSchema("synaptogenesis_rate", "float", "Rate of new synapse formation", true, ConfigValue(0.0001f), 0.0, 0.01),
        ParameterSchema("pruning_rate", "float", "Rate of synapse elimination", true, ConfigValue(0.00001f), 0.0, 0.01),
        
        // Randomness
        ParameterSchema("random_seed", "int64_t", "Seed for random number generator", true, ConfigValue(42), 0, 2147483647),
        
        // Memory systems
        ParameterSchema("working_memory_capacity", "int", "Capacity of working memory in items", false, ConfigValue(100), 1, 10000),
        ParameterSchema("episodic_memory_capacity", "int", "Capacity of episodic memory in episodes", false, ConfigValue(1000), 1, 100000),
        
        // Neuromodulation
        ParameterSchema("dopamine_strength", "float", "Dopamine system strength factor", false, ConfigValue(1.0f), 0.0, 5.0),
        ParameterSchema("curiosity_strength", "float", "Curiosity system strength factor", false, ConfigValue(1.0f), 0.0, 5.0),
        
        // Development
        ParameterSchema("plasticity_decay_rate", "float", "Rate at which plasticity decreases with development", false, ConfigValue(0.001f), 0.0, 0.1),
        
        // Output
        ParameterSchema("enable_console_output", "bool", "Enable console output logging", false, ConfigValue(true)),
        ParameterSchema("enable_file_logging", "bool", "Enable file logging to log file", false, ConfigValue(false)),
        ParameterSchema("log_file_path", "string", "Path to log file", false, ConfigValue("nlm_simulation.log")),
    };
}

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
    
    // Validate after loading from file
    validateAllParameters();
    
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
    
    // Validate after loading from command line
    validateAllParameters();
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << toString(entry.value) << "\n";
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
    // Set the value
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        // Reset validation error when value changes
        it->validationError = ConfigValidationError::None;
        it->validationMessage.clear();
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    // Validate the new value
    validateParameter(key);
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
    // Reset validation after clearing
    for (auto& entry : pImpl->entries) {
        entry.validationError = ConfigValidationError::None;
        entry.validationMessage.clear();
    }
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [" << toString(entry.value) << "] (" << static_cast<int>(entry.source) << ")\n";
        if (entry.validationError != ConfigValidationError::None) {
            oss << "    ⚠️ Validation Error: " << entry.validationMessage << "\n";
        }
    }
    return oss.str();
}

// Enhanced configuration management methods
bool Config::validate() const {
    for (const auto& entry : pImpl->entries) {
        if (!validateParameter(entry.key)) {
            return false;
        }
    }
    return true;
}

bool Config::validateParameter(const std::string& key) const {
    if (!parameterExists(key)) {
        return false;
    }
    
    const auto& entry = pImpl->entries[getParameterIndex(key)];
    bool isValid = true;
    
    // Check parameter exists in schema
    auto schemaIt = std::find_if(pImpl->parameterSchemas.begin(), pImpl->parameterSchemas.end(),
                                 [&key](const ParameterSchema& s) { return s.key == key; });
    
    if (schemaIt != pImpl->parameterSchemas.end()) {
        const auto& schema = *schemaIt;
        
        // Validate type
        if (!validateParameterType(key, entry.value, schema.type)) {
            entry.validationError = ConfigValidationError::TypeMismatch;
            entry.validationMessage = getParameterValidationError(key, entry.value, ConfigValidationError::TypeMismatch);
            isValid = false;
        }
        
        // Validate range if numeric
        if (isValid && isValueNumeric(entry.value) && (schema.minValue != 0.0 || schema.maxValue != 0.0)) {
            if (!validateParameterRange(key, entry.value)) {
                entry.validationError = ConfigValidationError::RangeViolation;
                entry.validationMessage = getParameterValidationError(key, entry.value, ConfigValidationError::RangeViolation);
                isValid = false;
            }
        }
        
        // Validate required
        if (!isValid && schema.required) {
            entry.validationError = ConfigValidationError::MissingRequired;
            entry.validationMessage = getParameterValidationError(key, entry.value, ConfigValidationError::MissingRequired);
            isValid = false;
        }
    }
    
    if (isValid) {
        // Reset validation error if valid
        const_cast<ConfigEntry&>(entry).validationError = ConfigValidationError::None;
        const_cast<ConfigEntry&>(entry).validationMessage.clear();
    }
    
    return isValid;
}

void Config::validateAllParameters() {
    for (auto& entry : pImpl->entries) {
        validateParameter(entry.key);
    }
}

ConfigValidationError Config::getParameterValidationError(const std::string& key) const {
    if (!parameterExists(key)) {
        return ConfigValidationError::None;
    }
    
    const auto& entry = pImpl->entries[getParameterIndex(key)];
    return entry.validationError;
}

std::string Config::getParameterValidationMessage(const std::string& key) const {
    if (!parameterExists(key)) {
        return "";
    }
    
    const auto& entry = pImpl->entries[getParameterIndex(key)];
    return entry.validationMessage;
}

bool Config::validateRange(const std::string& key) const {
    if (!parameterExists(key)) {
        return false;
    }
    
    const auto& entry = pImpl->entries[getParameterIndex(key)];
    auto schemaIt = std::find_if(pImpl->parameterSchemas.begin(), pImpl->parameterSchemas.end(),
                                 [&key](const ParameterSchema& s) { return s.key == key; });
    
    if (schemaIt == pImpl->parameterSchemas.end()) {
        return true; // No schema, no validation
    }
    
    const auto& schema = *schemaIt;
    return validateRange(entry.value, schema.minValue, schema.maxValue);
}

bool Config::validateType(const std::string& key) const {
    if (!parameterExists(key)) {
        return false;
    }
    
    const auto& entry = pImpl->entries[getParameterIndex(key)];
    auto schemaIt = std::find_if(pImpl->parameterSchemas.begin(), pImpl->parameterSchemas.end(),
                                 [&key](const ParameterSchema& s) { return s.key == key; });
    
    if (schemaIt == pImpl->parameterSchemas.end()) {
        return true; // No schema, no validation
    }
    
    const auto& schema = *schemaIt;
    return validateTypeMatch(entry.value, schema.type);
}

bool Config::validateRequired(const std::string& key) const {
    if (!parameterExists(key)) {
        return false;
    }
    
    auto schemaIt = std::find_if(pImpl->parameterSchemas.begin(), pImpl->parameterSchemas.end(),
                                 [&key](const ParameterSchema& s) { return s.key == key; });
    
    if (schemaIt == pImpl->parameterSchemas.end()) {
        return true; // No schema, no validation
    }
    
    const auto& schema = *schemaIt;
    return schema.required;
}

std::string Config::getParameterTypeInfo(const std::string& key) const {
    if (!parameterExists(key)) {
        return "Parameter not found";
    }
    
    auto schemaIt = std::find_if(pImpl->parameterSchemas.begin(), pImpl->parameterSchemas.end(),
                                 [&key](const ParameterSchema& s) { return s.key == key; });
    
    if (schemaIt == pImpl->parameterSchemas.end()) {
        return "No schema defined for this parameter";
    }
    
    const auto& schema = *schemaIt;
    return "Type: " + schema.type + ", Required: " + (schema.required ? "Yes" : "No") +
           ", Default: " + toString(schema.defaultValue);
}

void Config::printParameterInfo(const std::string& key) const {
    std::cout << "Parameter: " << key << std::endl;
    std::cout << "  Type: " << getParameterTypeInfo(key) << std::endl;
    
    if (parameterExists(key)) {
        const auto& entry = pImpl->entries[getParameterIndex(key)];
        std::cout << "  Current Value: " << toString(entry.value) << std::endl;
        std::cout << "  Source: " << static_cast<int>(entry.source) << std::endl;
        
        if (entry.validationError != ConfigValidationError::None) {
            std::cout << "  Status: INVALID - " << entry.validationMessage << std::endl;
        } else {
            std::cout << "  Status: VALID" << std::endl;
        }
    } else {
        std::cout << "  Status: NOT SET" << std::endl;
        
        auto schemaIt = std::find_if(pImpl->parameterSchemas.begin(), pImpl->parameterSchemas.end(),
                                     [&key](const ParameterSchema& s) { return s.key == key; });
        
        if (schemaIt != pImpl->parameterSchemas.end()) {
            const auto& schema = *schemaIt;
            std::cout << "  Default Value: " << toString(schema.defaultValue) << std::endl;
        }
    }
}

std::string Config::getParameterValidationError(const std::string& key, const ConfigValue& value, ConfigValidationError error) {
    switch (error) {
        case ConfigValidationError::TypeMismatch:
            return "Type mismatch: expected " + getTypeName(value) + " but got different type";
        case ConfigValidationError::RangeViolation:
            return "Value out of range: " + toString(value) + " is outside valid bounds";
        case ConfigValidationError::MissingRequired:
            return "Required parameter is missing or has invalid value";
        case ConfigValidationError::InvalidFormat:
            return "Invalid format: parameter value cannot be parsed correctly";
        case ConfigValidationError::ValueOutOfBounds:
            return "Value out of acceptable bounds: " + toString(value);
        default:
            return "Unknown validation error";
    }
}

// Parameter-specific validation helpers
bool Config::validateRange(const ConfigValue& value, double min, double max) {
    if (min == 0.0 && max == 0.0) return true; // No range specified
    
    double val = extractNumericValue(value);
    if (val == -1e9) return false; // Failed to extract numeric value
    
    return (val >= min && val <= max);
}

bool Config::validatePositive(const ConfigValue& value) {
    double val = extractNumericValue(value);
    return val >= 0.0;
}

bool Config::validateNonNegative(const ConfigValue& value) {
    double val = extractNumericValue(value);
    return val >= 0.0;
}

bool Config::validateStringLength(const ConfigValue& value, size_t minLen, size_t maxLen) {
    if (const std::string* str = std::get_if<std::string>(&value)) {
        size_t len = str->length();
        return (len >= minLen && len <= maxLen);
    }
    return false;
}

bool Config::validateTypeMatch(const ConfigValue& value, const std::string& expectedType) {
    std::string actualType = getTypeName(value);
    return actualType == expectedType;
}

bool Config::isValueNumeric(const ConfigValue& value) {
    return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value) ||
           std::holds_alternative<double>(value);
}

double Config::extractNumericValue(const ConfigValue& value) {
    if (const int* i = std::get_if<int>(&value)) {
        return static_cast<double>(*i);
    } else if (const int64_t* i64 = std::get_if<int64_t>(&value)) {
        return static_cast<double>(*i64);
    } else if (const double* d = std::get_if<double>(&value)) {
        return *d;
    }
    return -1e9; // Failed extraction
}

std::string Config::getTypeName(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "std::vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "std::vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "std::vector<std::string>";
    return "unknown";
}

// Configuration validation framework
std::vector<Config::ValidationResult> Config::validateAllParametersWithDetails() const {
    std::vector<ValidationResult> results;
    
    for (const auto& entry : pImpl->entries) {
        bool isValid = true;
        ConfigValidationError error = ConfigValidationError::None;
        std::string message;
        
        // Check parameter exists in schema
        auto schemaIt = std::find_if(pImpl->parameterSchemas.begin(), pImpl->parameterSchemas.end(),
                                     [&entry](const ParameterSchema& s) { return s.key == entry.key; });
        
        if (schemaIt != pImpl->parameterSchemas.end()) {
            const auto& schema = *schemaIt;
            
            // Validate type
            if (!validateParameterType(entry.key, entry.value, schema.type)) {
                isValid = false;
                error = ConfigValidationError::TypeMismatch;
                message = getParameterValidationError(entry.key, entry.value, error);
            }
            
            // Validate range if numeric
            if (isValid && isValueNumeric(entry.value) && (schema.minValue != 0.0 || schema.maxValue != 0.0)) {
                if (!validateRange(entry.value, schema.minValue, schema.maxValue)) {
                    isValid = false;
                    error = ConfigValidationError::RangeViolation;
                    message = getParameterValidationError(entry.key, entry.value, error);
                }
            }
        }
        
        results.emplace_back(entry.key, isValid, error, message);
    }
    
    return results;
}

// Batch operations for efficiency
void Config::setMultiple(const std::map<std::string, ConfigValue>& values) {
    for (const auto& pair : values) {
        set(pair.first, pair.second, ConfigSource::Runtime);
    }
}

void Config::applyConfiguration(const std::map<std::string, ConfigValue>& values) {
    for (const auto& pair : values) {
        // Only set if parameter already exists to avoid adding new keys unexpectedly
        if (has(pair.first)) {
            set(pair.first, pair.second, ConfigSource::Runtime);
        }
    }
}

// Configuration information and statistics
size_t Config::getParameterCount() const {
    return pImpl->entries.size();
}

size_t Config::getValidParameterCount() const {
    size_t count = 0;
    for (const auto& entry : pImpl->entries) {
        if (validateParameter(entry.key)) {
            count++;
        }
    }
    return count;
}

double Config::getValidationCoverage() const {
    if (pImpl->entries.empty()) return 0.0;
    return static_cast<double>(getValidParameterCount()) / pImpl->entries.size();
}

std::vector<std::string> Config::getInvalidParameters() const {
    std::vector<std::string> invalid;
    for (const auto& entry : pImpl->entries) {
        if (!validateParameter(entry.key)) {
            invalid.push_back(entry.key);
        }
    }
    return invalid;
}

// Source management
ConfigSource Config::getParameterSource(const std::string& key) const {
    if (!parameterExists(key)) {
        return ConfigSource::Default;
    }
    
    const auto& entry = pImpl->entries[getParameterIndex(key)];
    return entry.source;
}

std::vector<std::string> Config::getParametersBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

// Internal helper methods
bool Config::parameterExists(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

size_t Config::getParameterIndex(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return (it != pImpl->entries.end()) ? std::distance(pImpl->entries.begin(), it) : 0;
}

bool Config::validateParameterType(const std::string& key, const ConfigValue& value, const std::string& expectedType) const {
    return validateTypeMatch(value, expectedType);
}

bool Config::validateParameterRange(const std::string& key, const ConfigValue& value) const {
    auto schemaIt = std::find_if(pImpl->parameterSchemas.begin(), pImpl->parameterSchemas.end(),
                                 [&key](const ParameterSchema& s) { return s.key == key; });
    
    if (schemaIt == pImpl->parameterSchemas.end()) {
        return true; // No schema, assume valid
    }
    
    const auto& schema = *schemaIt;
    return validateRange(value, schema.minValue, schema.maxValue);
}

bool Config::validateParameterRequired(const std::string& key) const {
    auto schemaIt = std::find_if(pImpl->parameterSchemas.begin(), pImpl->parameterSchemas.end(),
                                 [&key](const ParameterSchema& s) { return s.key == key; });
    
    if (schemaIt == pImpl->parameterSchemas.end()) {
        return true; // No schema, assume not required
    }
    
    const auto& schema = *schemaIt;
    return schema.required;
}

std::vector<ParameterMetadata> Config::getParameterMetadata() {
    std::vector<ParameterMetadata> metadata;
    for (const auto& schema : getParameterSchemas()) {
        metadata.emplace_back(
            schema.key,
            schema.type,
            schema.description,
            schema.required,
            schema.defaultValue,
            schema.minValue,
            schema.maxValue,
            schema.validationRule
        );
    }
    return metadata;
}

ParameterMetadata Config::getParameterMetadata(const std::string& key) const {
    auto schemaIt = std::find_if(pImpl->parameterSchemas.begin(), pImpl->parameterSchemas.end(),
                                 [&key](const ParameterSchema& s) { return s.key == key; });
    
    if (schemaIt != pImpl->parameterSchemas.end()) {
        const auto& schema = *schemaIt;
        return ParameterMetadata(
            schema.key,
            schema.type,
            schema.description,
            schema.required,
            schema.defaultValue,
            schema.minValue,
            schema.maxValue,
            schema.validationRule
        );
    }
    
    return ParameterMetadata(key, "unknown", "No schema defined", false);
}

std::vector<std::string> Config::getParameterKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::string Config::toString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << arg[i];
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << arg[i];
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                oss << "\"" << arg[i] << "\"";
                if (i < arg.size() - 1) oss << ", ";
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

} // namespace nlm