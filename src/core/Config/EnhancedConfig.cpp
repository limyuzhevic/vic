#include "EnhancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <unordered_map>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <json/json.h> // Assuming JSON library is available

namespace nlm {

struct EnhancedConfig::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schemas;
    std::string envVarPrefix;
    std::unordered_map<std::string, std::any> cache;
    bool validateOnSet = true;
    
    Impl() : envVarPrefix("NLM_"), validateOnSet(true) {}
};

EnhancedConfig::EnhancedConfig() : pImpl(std::make_unique<Impl>()) {}

EnhancedConfig::~EnhancedConfig() = default;

EnhancedConfig::EnhancedConfig(EnhancedConfig&&) noexcept = default;

EnhancedConfig& EnhancedConfig::operator=(EnhancedConfig&&) noexcept = default;

bool EnhancedConfig::loadFromFile(const std::string& filepath, bool validate) {
    // Try to detect file format
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    if (ext == ".json" || ext == ".jsonl" || ext == ".jsonl.gz") {
        success = loadFromJsonFile(filepath);
    } else {
        // Try simple key=value format for backward compatibility
        success = loadFromSimpleFile(filepath);
    }
    
    if (success && validate) {
        auto result = validate();
        if (!result.isValid) {
            clear();
            return false;
        }
    }
    
    return success;
}

bool EnhancedConfig::loadFromEnvironment(const std::string& prefix) {
    setEnvVarPrefix(prefix);
    return loadEnvVars(prefix);
}

bool EnhancedConfig::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            // --key=value format
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        } else if (arg.substr(0, 2) == "-") {
            // -key value format
            std::string key = arg.substr(1);
            if (i + 1 < argc) {
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    return true;
}

bool EnhancedConfig::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on: " << getCurrentTimestamp() << "\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        if (!entry.envVar.empty()) {
            file << "# Environment variable: " << entry.envVar << "\n";
        }
        file << entry.key << " = ";
        
        // Print value based on type
        std::visit([&file](auto&& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else if constexpr (std::is_arithmetic_v<T>) {
                file << val;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << val[i];
                }
                file << "]";
            } else {
                file << "" << val << "";
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    return true;
}

template<typename T>
void EnhancedConfig::set(const std::string& key, const T& value, ConfigSource source) {
    if (pImpl->validateOnSet) {
        auto schema = getSchema(key);
        if (schema) {
            ValidationResult validation = validateSingleSchema(*schema, std::any(value));
            if (!validation.isValid) {
                throw std::runtime_error("Configuration validation failed for key '" + key + "': " + validation.errorMessage);
            }
            for (const auto& warning : validation.warnings) {
                // Log warning but don't fail
                std::cerr << "Warning: " << warning << std::endl;
            }
        }
    }
    
    ConfigEntry entry(key, std::any(value), source);
    
    // Try to get description from schema
    auto schema = getSchema(key);
    if (schema) {
        entry.description = schema->description;
        entry.envVar = toEnvVarName(key);
        entry.required = schema->required;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = std::any(value);
        it->source = source;
    } else {
        pImpl->entries.push_back(entry);
    }
    
    // Clear cache
    pImpl->cache.erase(key);
}

template<typename T>
T EnhancedConfig::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        throw std::runtime_error("Configuration key not found: '" + key + "'");
    }
    
    if (pImpl->cache.count(key)) {
        try {
            return std::any_cast<T>(pImpl->cache.at(key));
        } catch (const std::bad_any_cast&) {
            // Fall through to conversion
        }
    }
    
    try {
        T result = std::any_cast<T>(it->value);
        pImpl->cache[key] = result;
        return result;
    } catch (const std::bad_any_cast&) {
        // Try type conversion
        T result = convertToType<T>(it->value);
        pImpl->cache[key] = result;
        return result;
    }
}

template<typename T>
T EnhancedConfig::getOr(const std::string& key, const T& defaultValue) const {
    if (has(key)) {
        return get<T>(key);
    }
    return defaultValue;
}

template<typename T>
std::optional<T> EnhancedConfig::tryGet(const std::string& key) const {
    if (!has(key)) {
        return std::nullopt;
    }
    
    try {
        return get<T>(key);
    } catch (const std::exception&) {
        return std::nullopt;
    }
}

bool EnhancedConfig::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& e) { return e.key == key; });
}

void EnhancedConfig::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                      [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    pImpl->cache.erase(key);
}

std::vector<std::string> EnhancedConfig::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void EnhancedConfig::clear() {
    pImpl->entries.clear();
    pImpl->cache.clear();
}

std::string EnhancedConfig::summary() const {
    std::ostringstream oss;
    oss << "Enhanced Configuration Summary:\n";
    oss << "===============================\n";
    oss << "Loaded entries: " << pImpl->entries.size() << "\n";
    oss << "Environment prefix: " << pImpl->envVarPrefix << "\n";
    oss << "Validation on set: " << (pImpl->validateOnSet ? "enabled" : "disabled") << "\n\n";
    
    if (!pImpl->entries.empty()) {
        oss << "Entries:\n";
        for (const auto& entry : pImpl->entries) {
            oss << "  " << entry.key << " = ";
            std::visit([&oss](auto&& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    oss << "\"" << val << "\"";
                } else if constexpr (std::is_same_v<T, bool>) {
                    oss << (val ? "true" : "false");
                } else {
                    oss << val;
                }
            }, entry.value);
            oss << " (" << static_cast<int>(entry.source) << ")";
            if (!entry.description.empty()) {
                oss << " - " << entry.description;
            }
            if (!entry.envVar.empty()) {
                oss << " [env: " << entry.envVar << "]";
            }
            oss << "\n";
        }
    }
    
    return oss.str();
}

void EnhancedConfig::setSchema(const ConfigSchema& schema) {
    // Remove existing schema for this key
    pImpl->schemas.erase(
        std::remove_if(pImpl->schemas.begin(), pImpl->schemas.end(),
                      [&schema](const ConfigSchema& s) { return s.key == schema.key; }),
        pImpl->schemas.end()
    );
    
    pImpl->schemas.push_back(schema);
}

void EnhancedConfig::addSchema(const ConfigSchema& schema) {
    pImpl->schemas.push_back(schema);
}

bool EnhancedConfig::hasSchema(const std::string& key) const {
    return getSchema(key) != nullptr;
}

const ConfigSchema* EnhancedConfig::getSchema(const std::string& key) const {
    auto it = std::find_if(pImpl->schemas.begin(), pImpl->schemas.end(),
                          [&key](const ConfigSchema& s) { return s.key == key; });
    return (it != pImpl->schemas.end()) ? &(*it) : nullptr;
}

void EnhancedConfig::merge(const EnhancedConfig& other, bool override) {
    for (const auto& entry : other.pImpl->entries) {
        if (override || !has(entry.key)) {
            set(entry.key, entry.value, entry.source);
        }
    }
}

void EnhancedConfig::setEnvVarPrefix(const std::string& prefix) {
    pImpl->envVarPrefix = prefix;
}

std::string EnhancedConfig::getEnvVarPrefix() const {
    return pImpl->envVarPrefix;
}

ValidationResult EnhancedConfig::validate() const {
    ValidationResult overallResult;
    
    // Validate against schemas
    ValidationResult schemaResult = validateAgainstSchema();
    if (!schemaResult.isValid) {
        overallResult.isValid = false;
        overallResult.errorMessage += schemaResult.errorMessage + "\n";
    }
    overallResult.warnings.insert(overallResult.warnings.end(), 
                                  schemaResult.warnings.begin(), schemaResult.warnings.end());
    
    // Check required fields
    for (const auto& schema : pImpl->schemas) {
        if (schema.required && !has(schema.key)) {
            overallResult.isValid = false;
            overallResult.errorMessage += "Missing required configuration key: '" + schema.key + "'\n";
        }
    }
    
    return overallResult;
}

ValidationResult EnhancedConfig::validate(const std::string& key) const {
    ValidationResult result;
    
    if (!has(key)) {
        result.isValid = false;
        result.errorMessage = "Key not found: '" + key + "'";
        return result;
    }
    
    // Get current value
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });n    
    if (it == pImpl->entries.end()) {
        result.isValid = false;
        result.errorMessage = "Key not found: '" + key + "'";
        return result;
    }
    
    // Validate against schema
    auto schema = getSchema(key);
    if (schema) {
        ValidationResult validation = validateSingleSchema(*schema, it->value);
        if (!validation.isValid) {
            return validation;
        }
        result.warnings.insert(result.warnings.end(), 
                              validation.warnings.begin(), validation.warnings.end());
    }
    
    return result;
}

size_t EnhancedConfig::getEntryCount() const {
    return pImpl->entries.size();
}

ConfigSource EnhancedConfig::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                          [&key](const ConfigEntry& e) { return e.key == key; });n    
    return (it != pImpl->entries.end()) ? it->source : ConfigSource::Default;
}

void EnhancedConfig::configureBrain(const std::string& configFile) {
    if (!configFile.empty()) {
        loadFromFile(configFile);
    }
    
    // Set default brain configuration if not already set
    if (!has("brain.neuron_count")) {
        set("brain.neuron_count", 1000);
    }
    if (!has("brain.synapse_density")) {
        set("brain.synapse_density", 0.1f);
    }
    if (!has("brain.v_thresh")) {
        set("brain.v_thresh", -50.0);
    }
    if (!has("brain.v_rest")) {
        set("brain.v_rest", -70.0);
    }
    if (!has("brain.v_reset")) {
        set("brain.v_reset", -75.0);
    }
    if (!has("brain.tau_mem")) {
        set("brain.tau_mem", 20.0);
    }
    if (!has("brain.tau_ref")) {
        set("brain.tau_ref", 2.0);
    }
}

void EnhancedConfig::configureWorld(const std::string& configFile) {
    if (!configFile.empty()) {
        loadFromFile(configFile);
    }
    
    // Set default world configuration
    if (!has("world.width")) {
        set("world.width", 20);
    }
    if (!has("world.height")) {
        set("world.height", 20);
    }
    if (!has("world.vision.width")) {
        set("world.vision.width", 8);
    }
    if (!has("world.vision.height")) {
        set("world.vision.height", 8);
    }
    if (!has("world.max_energy")) {
        set("world.max_energy", 100.0f);
    }
}

void EnhancedConfig::configureNeuromodulation(const std::string& configFile) {
    if (!configFile.empty()) {
        loadFromFile(configFile);
    }
    
    // Set default neuromodulation configuration
    if (!has("neuromod.dopamine.scale")) {
        set("neuromod.dopamine.scale", 1.0f);
    }
    if (!has("neuromod.curiosity.enable")) {
        set("neuromod.curiosity.enable", true);
    }
    if (!has("neuromod.novelty.enable")) {
        set("neuromod.novelty.enable", true);
    }
}

// Private implementation methods
ValidationResult EnhancedConfig::validateValue(const std::string& key, const std::any& value) const {
    ValidationResult result;
    
    // Get schema for this key
    auto schema = getSchema(key);
    if (!schema) {
        // No schema, basic validation
        return result;
    }
    
    return validateSingleSchema(*schema, value);
}

ValidationResult EnhancedConfig::validateType(const std::string& key, const std::any& value) const {
    ValidationResult result;
    
    // Implementation for type validation
    return result;
}

ValidationResult EnhancedConfig::validateConstraints(const std::string& key, const std::any& value) const {
    ValidationResult result;
    
    // Implementation for constraint validation
    return result;
}

bool EnhancedConfig::loadEnvVars(const std::string& prefix) {
    bool success = true;
    
    // This is a simplified implementation
    // In a real implementation, you would scan all environment variables
    // and match them to configuration keys
    
    return success;
}

bool EnhancedConfig::parseEnvLine(const std::string& line, const std::string& prefix) {
    // Implementation for parsing environment variable lines
    return true;
}

bool EnhancedConfig::loadFromSimpleFile(const std::string& filepath) {
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
            
            // Convert value to appropriate type based on key
            set(key, convertToValue(key, value), ConfigSource::File);
        }
    }
    
    return true;
}

bool EnhancedConfig::loadFromJsonFile(const std::string& filepath) {
    // This is a simplified JSON loader
    // In a real implementation, you would use a proper JSON parser
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // For now, just fall back to simple file parsing
    return loadFromSimpleFile(filepath);
}

ValidationResult EnhancedConfig::validateAgainstSchema() const {
    ValidationResult result;
    
    for (const auto& schema : pImpl->schemas) {
        ValidationResult validation = validateSingleSchema(schema, std::any());
        if (!validation.isValid) {
            result.isValid = false;
            result.errorMessage += validation.errorMessage + "\n";
        }
        result.warnings.insert(result.warnings.end(), 
                              validation.warnings.begin(), validation.warnings.end());
    }
    
    return result;
}

ValidationResult EnhancedConfig::validateSingleSchema(const ConfigSchema& schema, const std::any& value) const {
    ValidationResult result;
    
    // Check if value is provided for required field
    if (schema.required && value.has_value()) {
        // Required field is present, validate its value
    } else if (schema.required && !value.has_value()) {
        result.isValid = false;
        result.errorMessage = "Required configuration key is missing: '" + schema.key + "'";
        return result;
    }
    
    if (!value.has_value()) {
        // Skip validation for missing optional fields
        return result;
    }
    
    // Validate type
    if (!value.has_value()) {
        return result;
    }
    
    // Check type against schema
    switch (schema.type) {
        case ConfigType::INT:
            if (!isTypeMatch<int>(value)) {
                result.isValid = false;
                result.errorMessage = "Configuration key '" + schema.key + "' must be an integer";
            }
            break;
        case ConfigType::FLOAT:
            if (!isTypeMatch<float>(value)) {
                result.isValid = false;
                result.errorMessage = "Configuration key '" + schema.key + "' must be a float";
            }
            break;
        case ConfigType::STRING:
            if (!isTypeMatch<std::string>(value)) {
                result.isValid = false;
                result.errorMessage = "Configuration key '" + schema.key + "' must be a string";
            }
            break;
        case ConfigType::BOOL:
            if (!isTypeMatch<bool>(value)) {
                result.isValid = false;
                result.errorMessage = "Configuration key '" + schema.key + "' must be a boolean";
            }
            break;
        // Add other type cases as needed
    }
    
    // If we passed type validation, check constraints
    if (result.isValid) {
        ValidationResult constraintResult = validateConstraints(schema, value);
        if (!constraintResult.isValid) {
            result.isValid = false;
            result.errorMessage = constraintResult.errorMessage;
        }
        result.warnings.insert(result.warnings.end(), 
                              constraintResult.warnings.begin(), constraintResult.warnings.end());
    }
    
    return result;
}

std::string EnhancedConfig::getCurrentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm tm_buf;
    gmtime_r(&time, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// Template type conversion utilities
template<typename T>
T EnhancedConfig::convertToType(const std::any& value) {
    try {
        return std::any_cast<T>(value);
    } catch (const std::bad_any_cast&) {
        // Try to convert from different type
        if constexpr (std::is_same_v<T, int>) {
            if (value.type() == typeid(float)) {
                return static_cast<int>(std::any_cast<float>(value));
            }
        }
        // Add more conversions as needed
        throw;
    }
}

template<typename T>
bool EnhancedConfig::isTypeMatch(const std::any& value) {
    return value.type() == typeid(T);
}

// Helper function to convert string to appropriate type based on key
std::any EnhancedConfig::convertToValue(const std::string& key, const std::string& strValue) {
    // Try to determine type based on key or content
    if (strValue == "true" || strValue == "false") {
        return strValue == "true";
    }
    
    try {
        // Try integer
        size_t pos;
        int intVal = std::stoi(strValue, &pos);
        if (pos == strValue.size()) {
            return intVal;
        }
    } catch (...) {}
    
    try {
        // Try float
        size_t pos;
        float floatVal = std::stof(strValue, &pos);
        if (pos == strValue.size()) {
            return floatVal;
        }
    } catch (...) {}
    
    // Default to string
    return strValue;
}

// Explicit template instantiations
template void EnhancedConfig::set<int>(const std::string&, const int&, ConfigSource);
template void EnhancedConfig::set<float>(const std::string&, const float&, ConfigSource);
template void EnhancedConfig::set<double>(const std::string&, const double&, ConfigSource);
template void EnhancedConfig::set<bool>(const std::string&, const bool&, ConfigSource);
template void EnhancedConfig::set<std::string>(const std::string&, const std::string&, ConfigSource);

template int EnhancedConfig::get<int>(const std::string&) const;
template float EnhancedConfig::get<float>(const std::string&) const;
template double EnhancedConfig::get<double>(const std::string&) const;
template bool EnhancedConfig::get<bool>(const std::string&) const;
template std::string EnhancedConfig::get<std::string>(const std::string&) const;
template std::vector<int> EnhancedConfig::get<std::vector<int>>(const std::string&) const;
template std::vector<double> EnhancedConfig::get<std::vector<double>>(const std::string&) const;
template std::vector<std::string> EnhancedConfig::get<std::vector<std::string>>(const std::string&) const;

// Convenience function implementations
EnhancedConfig createDefaultConfig() {
    EnhancedConfig config;
    config.configureBrain();
    config.configureWorld();
    config.configureNeuromodulation();
    return config;
}

EnhancedConfig createBrainConfig() {
    EnhancedConfig config;
    config.configureBrain();
    return config;
}

EnhancedConfig createWorldConfig() {
    EnhancedConfig config;
    config.configureWorld();
    return config;
}

EnhancedConfig createNeuromodulationConfig() {
    EnhancedConfig config;
    config.configureNeuromodulation();
    return config;
}

} // namespace nlm
