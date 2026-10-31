#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <variant>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <vector>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unique_ptr<ConfigSchema> schema;
    std::unique_ptr<ConfigValidator> validator;
    std::vector<ValidationResult> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default validator
    pImpl->validator = std::make_unique<DefaultConfigValidator>();
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate against schema if present
    ValidationResult validationResult;
    if (pImpl->schema) {
        validationResult = pImpl->schema->validate(key, value);
    }
    
    if (pImpl->validator) {
        ValidationResult validatorResult = pImpl->validator->validate(key, value);
        if (!validatorResult.valid) {
            validationResult = validatorResult;
        }
    }
    
    if (!validationResult.valid) {
        pImpl->validationErrors.push_back(validationResult);
        // Still set the value but log the error
        NLM_LOG_WARNING("Config validation error for key '" + key + "': " + validationResult.errorMessage);
    }
    
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
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

// ConfigSchema implementation
void ConfigSchema::addElement(const ConfigSchemaElement& element) {
    elements_[element.key] = element;
}

ValidationResult ConfigSchema::validate(const std::string& key, const ConfigValue& value) const {
    auto it = elements_.find(key);
    if (it == elements_.end()) {
        return ValidationResult(true);  // No validation for unknown keys
    }
    
    const auto& element = it->second;
    
    // Check type
    ConfigDataType actualType = getTypeFromValue(value);
    if (actualType != element.type && element.type != ConfigDataType::UNKNOWN) {
        return ValidationResult(false, 
            "Type mismatch for key '" + key + "': expected " + 
            configDataTypeToString(element.type) + ", got " + 
            configDataTypeToString(actualType), key);
    }
    
    // Check range for numeric types
    if ((element.type == ConfigDataType::INTEGER || element.type == ConfigDataType::INT64 ||
         element.type == ConfigDataType::UINT64 || element.type == ConfigDataType::FLOAT ||
         element.type == ConfigDataType::DOUBLE) && element.range.first != element.range.second) {
        
        double val = 0.0;
        if (std::holds_alternative<int>(value)) val = static_cast<double>(std::get<int>(value));
        else if (std::holds_alternative<int64_t>(value)) val = static_cast<double>(std::get<int64_t>(value));
        else if (std::holds_alternative<uint64_t>(value)) val = static_cast<double>(std::get<uint64_t>(value));
        else if (std::holds_alternative<float>(value)) val = static_cast<double>(std::get<float>(value));
        else if (std::holds_alternative<double>(value)) val = std::get<double>(value);
        
        if (val < element.range.first || val > element.range.second) {
            return ValidationResult(false,
                "Value out of range for key '" + key + "': " + std::to_string(val) +
                " not in [" + std::to_string(element.range.first) + ", " +
                std::to_string(element.range.second) + "]", key);
        }
    }
    
    // Check allowed values
    if (!element.allowedValues.empty()) {
        std::string strValue;
        if (std::holds_alternative<std::string>(value)) {
            strValue = std::get<std::string>(value);
        } else {
            strValue = configValueToString(value);
        }
        
        if (std::find(element.allowedValues.begin(), element.allowedValues.end(), strValue) == 
            element.allowedValues.end()) {
            return ValidationResult(false,
                "Invalid value for key '" + key + "': '" + strValue + "' not in allowed values", key);
        }
    }
    
    return ValidationResult(true);
}

bool ConfigSchema::hasElement(const std::string& key) const {
    return elements_.find(key) != elements_.end();
}

ConfigDataType ConfigSchema::getType(const std::string& key) const {
    auto it = elements_.find(key);
    if (it == elements_.end()) return ConfigDataType::UNKNOWN;
    return it->second.type;
}

const ConfigSchemaElement& ConfigSchema::getElement(const std::string& key) const {
    return elements_.at(key);
}

std::vector<std::string> ConfigSchema::getAllKeys() const {
    std::vector<std::string> keys;
    keys.reserve(elements_.size());
    for (const auto& pair : elements_) {
        keys.push_back(pair.first);
    }
    return keys;
}

void ConfigSchema::clear() {
    elements_.clear();
}

// DefaultConfigValidator implementation
ValidationResult DefaultConfigValidator::validate(const std::string& key, const ConfigValue& value) const {
    // Basic validation: ensure key is not empty and value is not null
    if (key.empty()) {
        return ValidationResult(false, "Config key cannot be empty", key);
    }
    
    // Check for null values in variant
    if (std::holds_alternative<std::monostate>(value)) {
        return ValidationResult(false, "Config value cannot be null for key '" + key + "'", key);
    }
    
    return ValidationResult(true);
}

bool DefaultConfigValidator::isValidType(ConfigDataType expected, ConfigDataType actual) const {
    if (expected == ConfigDataType::UNKNOWN) return true;
    return expected == actual;
}

// Config implementation

void Config::setSchema(const ConfigSchema& schema) {
    pImpl->schema = std::make_unique<ConfigSchema>(schema);
    pImpl->validationErrors.clear();
}

ValidationResult Config::validate(const std::string& key, const ConfigValue& value) const {
    ValidationResult result = ValidationResult(true);
    
    if (pImpl->schema) {
        result = pImpl->schema->validate(key, value);
    }
    
    if (pImpl->validator) {
        ValidationResult validatorResult = pImpl->validator->validate(key, value);
        if (!validatorResult.valid) {
            result = validatorResult;
        }
    }
    
    return result;
}

bool Config::isValid() const {
    pImpl->validationErrors.clear();
    for (const auto& entry : pImpl->entries) {
        ValidationResult result = validate(entry.key, entry.value);
        if (!result.valid) {
            pImpl->validationErrors.push_back(result);
        }
    }
    return pImpl->validationErrors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    for (const auto& error : pImpl->validationErrors) {
        if (!error.valid) {
            errors.push_back(error.errorMessage);
        }
    }
    return errors;
}

Config::ConfigStats Config::getStats() const {
    ConfigStats stats;
    stats.totalEntries = pImpl->entries.size();
    stats.validEntries = 0;
    stats.invalidEntries = 0;
    
    for (size_t i = 0; i <= static_cast<size_t>(ConfigSource::Environment); ++i) {
        stats.entriesBySource[i] = 0;
    }
    
    for (const auto& entry : pImpl->entries) {
        // Check if entry is valid
        ValidationResult validation = validate(entry.key, entry.value);
        if (validation.valid) {
            stats.validEntries++;
        } else {
            stats.invalidEntries++;
        }
        
        if (static_cast<size_t>(entry.source) <= static_cast<size_t>(ConfigSource::Environment)) {
            stats.entriesBySource[static_cast<size_t>(entry.source)]++;
        }
    }
    
    return stats;
}

ConfigDataType Config::getTypeFromValue(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return ConfigDataType::INTEGER;
    if (std::holds_alternative<int64_t>(value)) return ConfigDataType::INT64;
    if (std::holds_alternative<uint64_t>(value)) return ConfigDataType::UINT64;
    if (std::holds_alternative<float>(value)) return ConfigDataType::FLOAT;
    if (std::holds_alternative<double>(value)) return ConfigDataType::DOUBLE;
    if (std::holds_alternative<bool>(value)) return ConfigDataType::BOOLEAN;
    if (std::holds_alternative<std::string>(value)) return ConfigDataType::STRING;
    
    // Check arrays
    if (std::holds_alternative<std::vector<int>>(value)) return ConfigDataType::ARRAY_INT;
    if (std::holds_alternative<std::vector<int64_t>>(value)) return ConfigDataType::ARRAY_INT64;
    if (std::holds_alternative<std::vector<uint64_t>>(value)) return ConfigDataType::ARRAY_UINT64;
    if (std::holds_alternative<std::vector<float>>(value)) return ConfigDataType::ARRAY_FLOAT;
    if (std::holds_alternative<std::vector<double>>(value)) return ConfigDataType::ARRAY_DOUBLE;
    if (std::holds_alternative<std::vector<bool>>(value)) return ConfigDataType::ARRAY_BOOL;
    if (std::holds_alternative<std::vector<std::string>>(value)) return ConfigDataType::ARRAY_STRING;
    
    // Check array of arrays
    if (std::holds_alternative<std::vector<std::vector<double>>>(value)) return ConfigDataType::ARRAY_ARRAY_DOUBLE;
    
    return ConfigDataType::UNKNOWN;
}

std::string Config::configDataTypeToString(ConfigDataType type) {
    switch (type) {
        case ConfigDataType::INTEGER: return "int";
        case ConfigDataType::INT64: return "int64_t";
        case ConfigDataType::UINT64: return "uint64_t";
        case ConfigDataType::FLOAT: return "float";
        case ConfigDataType::DOUBLE: return "double";
        case ConfigDataType::BOOLEAN: return "bool";
        case ConfigDataType::STRING: return "string";
        case ConfigDataType::ARRAY_INT: return "array<int>";
        case ConfigDataType::ARRAY_INT64: return "array<int64_t>";
        case ConfigDataType::ARRAY_UINT64: return "array<uint64_t>";
        case ConfigDataType::ARRAY_FLOAT: return "array<float>";
        case ConfigDataType::ARRAY_DOUBLE: return "array<double>";
        case ConfigDataType::ARRAY_BOOL: return "array<bool>";
        case ConfigDataType::ARRAY_STRING: return "array<string>";
        case ConfigDataType::ARRAY_ARRAY_DOUBLE: return "array<array<double>>";
        case ConfigDataType::UNKNOWN: return "unknown";
        default: return "unknown";
    }
}

std::string Config::configValueToString(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return std::to_string(std::get<int>(value));
    if (std::holds_alternative<int64_t>(value)) return std::to_string(std::get<int64_t>(value));
    if (std::holds_alternative<uint64_t>(value)) return std::to_string(std::get<uint64_t>(value));
    if (std::holds_alternative<float>(value)) return std::to_string(std::get<float>(value));
    if (std::holds_alternative<double>(value)) return std::to_string(std::get<double>(value));
    if (std::holds_alternative<bool>(value)) return std::get<bool>(value) ? "true" : "false";
    if (std::holds_alternative<std::string>(value)) return std::get<std::string>(value);
    
    // For arrays, provide a simple representation
    if (std::holds_alternative<std::vector<int>>(value)) {
        std::ostringstream oss;
        oss << "[";
        const auto& arr = std::get<std::vector<int>>(value);
        for (size_t i = 0; i < arr.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << arr[i];
        }
        oss << "]";
        return oss.str();
    }
    
    return "<unknown>";
}

// Template instantiations for enhanced types
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Summary method implementations
std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [" << configValueToString(entry.value) << "] (" 
            << static_cast<int>(entry.source) << ")\n";
    }
    if (!pImpl->validationErrors.empty()) {
        oss << "\nValidation Errors (" << pImpl->validationErrors.size() << "):\n";
        for (const auto& error : pImpl->validationErrors) {
            oss << "  " << error.errorMessage << "\n";
        }
    }
    return oss.str();
}

std::string Config::summaryWithValidation() const {
    std::ostringstream oss;
    oss << "=== Configuration Summary ===\n";
    oss << "Total entries: " << pImpl->entries.size() << "\n";
    oss << "Valid entries: " << std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [this](const ConfigEntry& entry) { return validate(entry.key, entry.value).valid; }) << "\n";
    oss << "Invalid entries: " << std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
        [this](const ConfigEntry& entry) { return !validate(entry.key, entry.value).valid; }) << "\n";
    oss << "Sources:" << std::endl;
    
    for (size_t i = 0; i <= static_cast<size_t>(ConfigSource::Environment); ++i) {
        ConfigSource source = static_cast<ConfigSource>(i);
        size_t count = std::count_if(pImpl->entries.begin(), pImpl->entries.end(),
            [source](const ConfigEntry& entry) { return entry.source == source; });
        oss << "  " << static_cast<int>(source) << ": " << count << "\n";
    }
    
    if (!pImpl->validationErrors.empty()) {
        oss << "\nValidation Errors:\n";
        for (const auto& error : pImpl->validationErrors) {
            oss << "  " << error.errorMessage << "\n";
        }
    }
    
    return oss.str();
}

// Comparison operators
bool Config::operator==(const Config& other) const {
    return pImpl->entries == other.pImpl->entries;
}

bool Config::operator!=(const Config& other) const {
    return !(*this == other);
}

} // namespace nlm
