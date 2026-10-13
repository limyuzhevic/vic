#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <numeric>
#include <iostream>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schemas;
    bool initialized;
    std::unordered_map<std::string, ValidationResult> validationCache;
};

Config::Config() : pImpl(std::make_unique<Impl>())
{
    pImpl->initialized = false;
    loadDefaults();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO: Implement proper JSON/YAML parser
    // Phase 1 uses simple key=value format, Phase 2 adds JSON support
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

bool Config::loadFromString(const std::string& content) {
    try {
        // Try JSON parsing first
        return parseJSON(content);
    } catch (...) {
        // Fall back to simple key=value format
        std::istringstream stream(content);
        std::string line;
        while (std::getline(stream, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }
            
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                set(key, value, ConfigSource::Runtime);
            }
        }
        return true;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated: " << std::endl;
    file << "\n[DEFAULT]\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << entry.key << " = \"" << arg << "\"\n";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << entry.key << " = " << (arg ? "true" : "false") << "\n";
            } else {
                file << entry.key << " = " << arg << "\n";
            }
        }, entry.value);
        file << "# Source: " << static_cast<int>(entry.source) << "\n\n";
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

bool Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, bool validate) {
    // Validate key format
    if (!isValidKey(key)) {
        reportError(key, "Invalid key format: " + key);
        return false;
    }
    
    // Check if key exists
    bool exists = has(key);
    
    // For new entries, check schema
    if (!exists && hasSchema(key)) {
        ConfigSchema schema = getSchema(key);
        
        // Check dependencies
        for (const auto& dep : schema.dependencies) {
            if (!has(dep)) {
                reportError(key, "Dependency not found: " + dep);
                return false;
            }
        }
    }
    
    // Create or update entry
    ConfigEntry entry;
    if (exists) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != pImpl->entries.end()) {
            entry = *it;
        }
    }
    
    entry.key = key;
    entry.value = value;
    entry.source = source;
    entry.type = getType(value);
    
    // Validate if requested
    if (validate) {
        ValidationResult validation = entry.validate();
        if (!validation.valid) {
            for (const auto& error : validation.errors) {
                reportError(key, error);
            }
            for (const auto& warning : validation.warnings) {
                // Just log warnings
                std::cout << "Warning: " << warning << std::endl;
            }
            return false;
        }
    }
    
    // Update or add entry
    if (exists) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (it != pImpl->entries.end()) {
            *it = entry;
        }
    } else {
        pImpl->entries.push_back(entry);
    }
    
    // Clear validation cache
    pImpl->validationCache.erase(key);
    
    return true;
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

namespace ConfigTemplates {
    ValidationResult validateNeurons(int neuronCount) {
        if (neuronCount <= 0) {
            return ValidationResult(false, {"Neuron count must be positive"});
        }
        if (neuronCount > 1000000) {
            return ValidationResult(false, {"Neuron count too large (max 1,000,000)"});
        }
        return ValidationResult(true);
    }
    
    ValidationResult validateSynapseProbability(double probability) {
        if (probability < 0.0 || probability > 1.0) {
            return ValidationResult(false, {"Synapse probability must be between 0.0 and 1.0"});
        }
        return ValidationResult(true);
    }
    
    ValidationResult validateTimeStep(double timestep) {
        if (timestep <= 0.0) {
            return ValidationResult(false, {"Time step must be positive"});
        }
        if (timestep > 1.0) {
            return ValidationResult(false, {"Time step too large (max 1.0)"});
        }
        return ValidationResult(true);
    }
    
    ValidationResult validateLearningRate(double rate) {
        if (rate < 0.0 || rate > 1.0) {
            return ValidationResult(false, {"Learning rate must be between 0.0 and 1.0"});
        }
        return ValidationResult(true);
    }
    
    ValidationResult validateRange01(double value) {
        if (value < 0.0 || value > 1.0) {
            return ValidationResult(false, {"Value must be between 0.0 and 1.0"});
        }
        return ValidationResult(true);
    }
    
    ValidationResult validatePositiveInt(int value) {
        if (value <= 0) {
            return ValidationResult(false, {"Value must be positive"});
        }
        return ValidationResult(true);
    }
    
    const std::vector<ConfigSchema> getDefaultSchemas() {
        return {
            ConfigSchema("brain.neuron_count", "Total number of neurons", ConfigValue(1000), ConfigSource::Default, "Brain"),
            ConfigSchema("brain.region_count", "Number of brain regions", ConfigValue(1), ConfigSource::Default, "Brain"),
            ConfigSchema("brain.connection_probability", "Synapse connectivity probability", ConfigValue(0.1), ConfigSource::Default, "Brain"),
            ConfigSchema("brain.v_thresh", "Neuron threshold potential (mV)", ConfigValue(-50.0), ConfigSource::Default, "Brain"),
            ConfigSchema("brain.v_rest", "Neuron resting potential (mV)", ConfigValue(-70.0), ConfigSource::Default, "Brain"),
            ConfigSchema("brain.v_reset", "Neuron reset potential (mV)", ConfigValue(-75.0), ConfigSource::Default, "Brain"),
            ConfigSchema("plasticity.stdp.enable", "Enable Spike-Timing-Dependent Plasticity", ConfigValue(true), ConfigSource::Default, "Plasticity"),
            ConfigSchema("plasticity.hebbian.enable", "Enable Hebbian learning", ConfigValue(true), ConfigSource::Default, "Plasticity"),
            ConfigSchema("plasticity.structural.enable", "Enable structural plasticity", ConfigValue(true), ConfigSource::Default, "Plasticity"),
            ConfigSchema("neuromod.dopamine.scale", "Dopamine modulation scale", ConfigValue(1.0), ConfigSource::Default, "Neuromodulation"),
            ConfigSchema("neuromod.curiosity.enable", "Enable curiosity-driven exploration", ConfigValue(true), ConfigSource::Default, "Neuromodulation"),
            ConfigSchema("neuromod.novelty.enable", "Enable novelty detection", ConfigValue(true), ConfigSource::Default, "Neuromodulation"),
        };
    }
}

bool Config::isValidKey(const std::string& key) {
    if (key.empty() || key.find(' ') != std::string::npos) {
        return false;
    }
    // Allow alphanumeric, underscores, dots, and hyphens
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '.' && c != '-') {
            return false;
        }
    }
    return true;
}

std::string Config::normalizeKey(const std::string& key) {
    std::string result = key;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::vector<std::string> Config::parseKeyPath(const std::string& keyPath) {
    std::vector<std::string> parts;
    std::string current;
    bool in_quotes = false;
    
    for (char c : keyPath) {
        if (c == '.' && !in_quotes) {
            if (!current.empty()) {
                parts.push_back(current);
                current.clear();
            }
        } else if (c == '\"' || c == '\'') {
            in_quotes = !in_quotes;
        } else {
            current += c;
        }
    }
    
    if (!current.empty()) {
        parts.push_back(current);
    }
    
    return parts;
}

bool Config::isValidType(const ConfigValue& value, const std::type_index& expectedType) {
    return value.index() == expectedType;
}

void Config::reportError(const std::string& key, const std::string& error) {
    std::cerr << "Config Error [key=" << key << "]: " << error << std::endl;
}

ValidationResult ConfigValidation::validateInt(const std::string& key, const ConfigValue& value,
                                              double min, double max) {
    if (!std::holds_alternative<int>(value)) {
        return ValidationResult(false, {"Expected int type for key: " + key});
    }
    
    int val = std::get<int>(value);
    std::vector<std::string> errors;
    
    if (val < static_cast<int>(min)) {
        errors.push_back("Value " + std::to_string(val) + " is less than minimum " + std::to_string(min));
    }
    if (val > static_cast<int>(max)) {
        errors.push_back("Value " + std::to_string(val) + " is greater than maximum " + std::to_string(max));
    }
    
    return ValidationResult(errors.empty(), errors);
}

ValidationResult ConfigValidation::validateDouble(const std::string& key, const ConfigValue& value,
                                                 double min, double max) {
    if (!std::holds_alternative<double>(value)) {
        return ValidationResult(false, {"Expected double type for key: " + key});
    }
    
    double val = std::get<double>(value);
    std::vector<std::string> errors;
    
    if (val < min) {
        errors.push_back("Value " + std::to_string(val) + " is less than minimum " + std::to_string(min));
    }
    if (val > max) {
        errors.push_back("Value " + std::to_string(val) + " is greater than maximum " + std::to_string(max));
    }
    
    return ValidationResult(errors.empty(), errors);
}

ValidationResult ConfigValidation::validateBool(const std::string& key, const ConfigValue& value) {
    if (!std::holds_alternative<bool>(value)) {
        return ValidationResult(false, {"Expected bool type for key: " + key});
    }
    return ValidationResult(true);
}

ValidationResult ConfigValidation::validateString(const std::string& key, const ConfigValue& value,
                                                 const std::vector<std::string>& allowed) {
    if (!std::holds_alternative<std::string>(value)) {
        return ValidationResult(false, {"Expected string type for key: " + key});
    }
    
    std::string val = std::get<std::string>(value);
    if (!allowed.empty()) {
        bool found = false;
        for (const auto& allowed_val : allowed) {
            if (val == allowed_val) {
                found = true;
                break;
            }
        }
        if (!found) {
            return ValidationResult(false, {"Value '" + val + "' not in allowed values"});
        }
    }
    
    return ValidationResult(true);
}

ValidationResult ConfigValidation::validateRange(const std::string& key, const ConfigValue& value,
                                                const std::vector<std::string>& categories) {
    if (!std::holds_alternative<int>(value)) {
        return ValidationResult(false, {"Expected int type for range validation"});
    }
    
    int val = std::get<int>(value);
    if (val < 0 || val >= static_cast<int>(categories.size())) {
        return ValidationResult(false, {"Value out of range for categories"});
    }
    
    return ValidationResult(true);
}

ValidationResult ConfigValidation::validateRequired(const std::string& key, const ConfigValue& value) {
    if (value.index() == std::type_index(typeid(std::monostate))) {
        return ValidationResult(false, {"Key is required"});
    }
    return ValidationResult(true);
}

ValidationResult ConfigValidation::validatePositive(const std::string& key, const ConfigValue& value) {
    bool valid = std::visit([](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
            return arg > 0;
        }
        return false;
    }, value);
    
    if (!valid) {
        return ValidationResult(false, {"Value must be positive"});
    }
    return ValidationResult(true);
}

ValidationResult ConfigValidation::validateNonNegative(const std::string& key, const ConfigValue& value) {
    bool valid = std::visit([](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
            return arg >= 0;
        }
        return false;
    }, value);
    
    if (!valid) {
        return ValidationResult(false, {"Value must be non-negative"});
    }
    return ValidationResult(true);
}

ValidationResult ConfigValidation::validatePercentage(const std::string& key, const ConfigValue& value) {
    bool valid = std::visit([](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_integral_v<T> || std::is_floating_point_v<T>) {
            return arg >= 0.0 && arg <= 100.0;
        }
        return false;
    }, value);
    
    if (!valid) {
        return ValidationResult(false, {"Percentage must be between 0.0 and 100.0"});
    }
    return ValidationResult(true);
}

ValidationResult ConfigValidation::validateAnd(const std::vector<ConfigValidator>& validators) {
    std::vector<std::string> errors;
    for (const auto& validator : validators) {
        ValidationResult result = validator("", ConfigValue());
        if (!result.valid) {
            errors.insert(errors.end(), result.errors.begin(), result.errors.end());
            errors.insert(errors.end(), result.warnings.begin(), result.warnings.end());
        }
    }
    return ValidationResult(errors.empty(), errors);
}

ValidationResult ConfigValidation::validateOr(const std::vector<ConfigValidator>& validators) {
    for (const auto& validator : validators) {
        ValidationResult result = validator("", ConfigValue());
        if (result.valid) {
            return result;
        }
    }
    std::vector<std::string> errors;
    for (const auto& validator : validators) {
        ValidationResult result = validator("", ConfigValue());
        errors.insert(errors.end(), result.errors.begin(), result.errors.end());
    }
    return ValidationResult(false, errors);
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

bool Config::isValid(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return it->validate();
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    pImpl->validationCache.erase(key);
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
    for (const auto& schema : pImpl->schemas) {
        if (schema.category == category) {
            keys.push_back(schema.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->validationCache.clear();
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
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.validate()) {
            oss << " [INVALID]";
        }
        oss << "\n";
    }
    return oss.str();
}

ValidationResult Config::validate(const std::string& key) const {
    // Check cache first
    auto cacheIt = pImpl->validationCache.find(key);
    if (cacheIt != pImpl->validationCache.end()) {
        return cacheIt->second;
    }
    
    // Find the entry
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ValidationResult(false, {"Key not found: " + key});
    }
    
    // Validate the entry
    ValidationResult result = it->validate();
    
    // Cache the result
    pImpl->validationCache[key] = result;
    
    return result;
}

ValidationResult Config::validateAll() const {
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    
    for (const auto& entry : pImpl->entries) {
        ValidationResult result = entry.validate();
        if (!result.valid) {
            errors.insert(errors.end(), result.errors.begin(), result.errors.end());
            warnings.insert(warnings.end(), result.warnings.begin(), result.warnings.end());
        }
    }
    
    return ValidationResult(errors.empty(), errors, warnings);
}

void Config::addSchema(const ConfigSchema& schema) {
    pImpl->schemas.push_back(schema);
    
    // Load default if not already set
    if (!has(schema.key)) {
        set(schema.key, schema.default_value, schema.default_source, false);
    }
}

void Config::loadDefaults() {
    if (pImpl->initialized) {
        return;
    }
    
    // Load all default schemas
    auto default_schemas = ConfigTemplates::getDefaultSchemas();
    
    for (const auto& schema : default_schemas) {
        addSchema(schema);
    }
    
    pImpl->initialized = true;
}

bool Config::hasSchema(const std::string& key) const {
    return std::any_of(pImpl->schemas.begin(), pImpl->schemas.end(),
        [&key](const ConfigSchema& s) { return s.key == key; });
}

ConfigSchema Config::getSchema(const std::string& key) const {
    auto it = std::find_if(pImpl->schemas.begin(), pImpl->schemas.end(),
        [&key](const ConfigSchema& s) { return s.key == key; });
    
    if (it != pImpl->schemas.end()) {
        return *it;
    }
    
    // Return empty schema if not found
    return ConfigSchema();
}

template<typename T>
bool Config::setWithValidation(const std::string& key, const T& value, ConfigSource source,
                              std::function<ValidationResult(T)> validator) {
    bool success = set(key, value, source, false);
    if (!success) {
        return false;
    }
    
    if (validator) {
        ValidationResult result = validator(value);
        if (!result.valid) {
            for (const auto& error : result.errors) {
                reportError(key, error);
            }
            return false;
        }
    }
    
    return true;
}

bool Config::parseJSON(const std::string& content) {
    // Simplified JSON parsing - in a real implementation, use a proper JSON library
    // This is a placeholder for future enhancement
    return false;
}

bool Config::parseYAML(const std::string& content) {
    // Simplified YAML parsing - in a real implementation, use a proper YAML library
    // This is a placeholder for future enhancement
    return false;
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
