#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <limits>
#include <cmath>
#include <cctype>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ValidationError> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

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
    pImpl->validationErrors.clear();
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

// ConfigEntry validation methods
bool ConfigEntry::isValid() const {
    if (validationErrors.empty()) {
        return true;
    }
    return false;
}

ValidationError ConfigEntry::validate(const Config& config) const {
    ValidationError error;
    error.key = key;
    
    // Check type hint
    if (!typeHint.empty() && !matchesTypeHint()) {
        error.result = ValidationResult::TypeMismatch;
        error.message = "Type mismatch: expected " + typeHint + ", got " + getTypeName(value);
        error.suggestion = "Use correct type for parameter '" + key + "'";
        return error;
    }
    
    // Check range constraints
    if (!withinRange()) {
        error.result = ValidationResult::RangeViolation;
        error.message = "Value out of range for parameter '" + key + "'";
        error.suggestion = "Value must be between " + std::to_string(minValue) + " and " + std::to_string(maxValue);
        return error;
    }
    
    // Check allowed values
    if (!allowedValues.empty() && !matchesAllowedValues()) {
        error.result = ValidationResult::InvalidValue;
        error.message = "Invalid value for parameter '" + key + "'";
        std::ostringstream oss;
        oss << "Allowed values: ";
        for (size_t i = 0; i < allowedValues.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << allowedValues[i];
        }
        error.suggestion = oss.str();
        return error;
    }
    
    // Check validation rule
    if (!validationRule.empty() && !satisfiesValidationRule()) {
        error.result = ValidationResult::ConsistencyError;
        error.message = "Validation rule violated for parameter '" + key + "'";
        error.suggestion = "Update configuration to satisfy validation rule: " + validationRule;
        return error;
    }
    
    // Check required parameters
    if (required && !config.has(key)) {
        error.result = ValidationResult::MissingRequired;
        error.message = "Required parameter missing: '" + key + "'";
        error.suggestion = "Specify parameter '" + key + "' in configuration";
        return error;
    }
    
    error.result = ValidationResult::Success;
    error.message = "Parameter validation passed";
    return error;
}

bool ConfigEntry::matchesTypeHint() const {
    std::string typeName = getTypeName(value);
    
    if (typeHint == "int" || typeHint == "integer") {
        return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    } else if (typeHint == "double" || typeHint == "float" || typeHint == "number") {
        return std::holds_alternative<double>(value);
    } else if (typeHint == "bool" || typeHint == "boolean") {
        return std::holds_alternative<bool>(value);
    } else if (typeHint == "string") {
        return std::holds_alternative<std::string>(value);
    } else if (typeHint == "vector<int>") {
        return std::holds_alternative<std::vector<int>>(value);
    } else if (typeHint == "vector<double>") {
        return std::holds_alternative<std::vector<double>>(value);
    } else if (typeHint == "vector<string>") {
        return std::holds_alternative<std::vector<std::string>>(value);
    }
    
    return true; // Unknown type hint - assume valid
}

bool ConfigEntry::withinRange() const {
    double value = getNumericValue(value);
    return value >= minValue && value <= maxValue;
}

bool ConfigEntry::matchesAllowedValues() const {
    std::string strValue = getStringValue(value);
    return std::find(allowedValues.begin(), allowedValues.end(), strValue) != allowedValues.end();
}

bool ConfigEntry::satisfiesValidationRule() const {
    if (validationRule.empty()) {
        return true;
    }
    
    // Simple validation rule: "positive" or "non-negative"
    if (validationRule == "positive") {
        double val = getNumericValue(value);
        return val > 0.0;
    } else if (validationRule == "non-negative") {
        double val = getNumericValue(value);
        return val >= 0.0;
    } else if (validationRule == "between_0_and_1") {
        double val = getNumericValue(value);
        return val >= 0.0 && val <= 1.0;
    } else if (validationRule == "integer") {
        return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    }
    
    return true; // Unknown rule - assume valid
}

// Helper functions
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

// Get type name from variant
std::string getTypeName(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
    return "unknown";
}

// Get numeric value from variant
double getNumericValue(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) {
        return static_cast<double>(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        return static_cast<double>(std::get<int64_t>(value));
    } else if (std::holds_alternative<double>(value)) {
        return std::get<double>(value);
    }
    return 0.0; // Default for non-numeric types
}

// Get string value from variant
std::string getStringValue(const ConfigValue& value) {
    if (std::holds_alternative<std::string>(value)) {
        return std::get<std::string>(value);
    }
    return ""; // Default for non-string types
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
