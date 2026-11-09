#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <limits>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigConstraint> constraints;
    std::vector<std::string> validationErrors;
    std::vector<std::string> validationWarnings;
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        it->wasValidated = false;
        it->validationError.clear();
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
    pImpl->validationWarnings.clear();
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")";
        if (!entry.validationError.empty()) {
            oss << " [VALIDATION ERROR: " << entry.validationError << "]";
        }
        oss << "\n";
    }
    return oss.str();
}

std::string Config::getValidationErrors() const {
    std::ostringstream oss;
    for (const auto& error : pImpl->validationErrors) {
        oss << "ERROR: " << error << "\n";
    }
    return oss.str();
}

std::string Config::getValidationWarnings() const {
    std::ostringstream oss;
    for (const auto& warning : pImpl->validationWarnings) {
        oss << "WARNING: " << warning << "\n";
    }
    return oss.str();
}

bool Config::hasValidationErrors() const {
    return !pImpl->validationErrors.empty();
}

bool Config::hasValidationWarnings() const {
    return !pImpl->validationWarnings.empty();
}

void Config::addConstraint(const std::string& key, double min, double max, const std::string& desc, 
                           bool minInclusive, bool maxInclusive) {
    pImpl->constraints.emplace_back(key, min, max, desc, minInclusive, maxInclusive);
}

ValidationResult Config::validateKey(const std::string& key) const {
    // Find the entry
    auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (entryIt == pImpl->entries.end()) {
        return ValidationResult(false, "Key not found: " + key);
    }
    
    ValidationResult result;
    
    // Find constraints for this key
    std::vector<ConfigConstraint> keyConstraints;
    for (const auto& constraint : pImpl->constraints) {
        if (constraint.key == key) {
            keyConstraints.push_back(constraint);
        }
    }
    
    // Validate against constraints
    if (!keyConstraints.empty()) {
        std::visit([&](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            
            if constexpr (std::is_arithmetic_v<T>) {
                for (const auto& constraint : keyConstraints) {
                    ValidationResult constraintResult = validateRange(value, constraint);
                    if (!constraintResult.valid) {
                        result.valid = false;
                        result.errorMessage += constraintResult.errorMessage + "; ";
                    } else if (!constraintResult.warningMessage.empty()) {
                        result.warningMessage += constraintResult.warningMessage + "; ";
                    }
                }
            }
        }, entryIt->value);
    }
    
    return result;
}

ValidationResult Config::validateAll() const {
    ValidationResult overallResult;
    
    // Clear previous errors and warnings
    pImpl->validationErrors.clear();
    pImpl->validationWarnings.clear();
    
    // Validate each entry
    for (const auto& entry : pImpl->entries) {
        ValidationResult keyResult = validateKey(entry.key);
        
        if (!keyResult.valid) {
            overallResult.valid = false;
            overallResult.errorMessage += "Key \"" + entry.key + "\": " + keyResult.errorMessage + " ";
            pImpl->validationErrors.push_back("Key \"" + entry.key + "\": " + keyResult.errorMessage);
        }
        
        if (!keyResult.warningMessage.empty()) {
            overallResult.warningMessage += "Key \"" + entry.key + "\": " + keyResult.warningMessage + " ";
            pImpl->validationWarnings.push_back("Key \"" + entry.key + "\": " + keyResult.warningMessage);
        }
    }
    
    return overallResult;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });n    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    // Convert value to desired type with validation
    return getValidated<T>(it->value, key, pImpl->constraints);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Template specializations for numeric types

template<typename T>
T getValidated(const ConfigValue& value, const std::string& key, const std::vector<ConfigConstraint>& constraints) {
    using V = std::decay_t<decltype(value)>;
    
    if constexpr (std::is_same_v<V, T>) {
        return value;
    } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
        double doubleValue;
        if constexpr (std::is_same_v<V, int>) {
            doubleValue = static_cast<double>(value);
        } else if constexpr (std::is_same_v<V, int64_t>) {
            doubleValue = static_cast<double>(value);
        } else if constexpr (std::is_same_v<V, double>) {
            doubleValue = value;
        } else if constexpr (std::is_same_v<V, bool>) {
            doubleValue = value ? 1.0 : 0.0;
        } else {
            doubleValue = 0.0;
        }
        
        // Validate against constraints
        ValidationResult validationResult = ValidationResult();
        for (const auto& constraint : constraints) {
            ValidationResult constraintResult = validateRange(doubleValue, constraint);
            if (!constraintResult.valid) {
                validationResult.valid = false;
                validationResult.errorMessage += constraintResult.errorMessage + "; ";
            } else if (!constraintResult.warningMessage.empty()) {
                validationResult.warningMessage += constraintResult.warningMessage + "; ";
            }
        }
        
        if (!validationResult.valid) {
            // In a real implementation, we might throw or log an error
            // For now, we'll just return the converted value
        }
        
        return static_cast<T>(doubleValue);
    } else {
        return std::nullopt;
    }
}

// Validate range for numeric values
template<typename T>
ValidationResult Config::validateRange(const T& value, const ConfigConstraint& constraint) const {
    double doubleValue;
    if constexpr (std::is_same_v<T, int>) {
        doubleValue = static_cast<double>(value);
    } else if constexpr (std::is_same_v<T, int64_t>) {
        doubleValue = static_cast<double>(value);
    } else if constexpr (std::is_same_v<T, double>) {
        doubleValue = value;
    } else if constexpr (std::is_same_v<T, bool>) {
        doubleValue = value ? 1.0 : 0.0;
    } else {
        doubleValue = 0.0;
    }
    
    ValidationResult result;
    
    // Check minimum
    if (constraint.minInclusive) {
        if (doubleValue < constraint.minValue) {
            result.valid = false;
            result.errorMessage = "Value " + std::to_string(doubleValue) + " is less than minimum " + 
                                 std::to_string(constraint.minValue);
        } else if (std::abs(doubleValue - constraint.minValue) < 1e-9 && constraint.minValue != 0.0) {
            result.warningMessage = "Value is at minimum boundary";
        }
    } else {
        if (doubleValue <= constraint.minValue) {
            result.valid = false;
            result.errorMessage = "Value " + std::to_string(doubleValue) + " is less than or equal to minimum " + 
                                 std::to_string(constraint.minValue);
        }
    }
    
    // Check maximum
    if (constraint.maxInclusive) {
        if (doubleValue > constraint.maxValue) {
            if (result.valid) {
                result.valid = false;
                result.errorMessage = "Value " + std::to_string(doubleValue) + " exceeds maximum " + 
                                     std::to_string(constraint.maxValue);
            } else {
                result.errorMessage += "; Value " + std::to_string(doubleValue) + " exceeds maximum " + 
                                      std::to_string(constraint.maxValue);
            }
        } else if (std::abs(doubleValue - constraint.maxValue) < 1e-9 && constraint.maxValue != 0.0) {
            if (result.warningMessage.empty()) {
                result.warningMessage = "Value is at maximum boundary";
            } else {
                result.warningMessage += "; Value is at maximum boundary";
            }
        }
    } else {
        if (doubleValue >= constraint.maxValue) {
            if (result.valid) {
                result.valid = false;
                result.errorMessage = "Value " + std::to_string(doubleValue) + " is greater than or equal to maximum " + 
                                     std::to_string(constraint.maxValue);
            } else {
                result.errorMessage += "; Value " + std::to_string(doubleValue) + " is greater than or equal to maximum " + 
                                      std::to_string(constraint.maxValue);
            }
        }
    }
    
    return result;
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

bool Config::isNumeric(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), [](char c) {
        return std::isdigit(c) || c == '.' || c == '-' || c == '+';
    });
}

double Config::stringToDouble(const std::string& str) {
    return std::stod(str);
}

// Explicit template instantiations
// (int and int64_t are the same type on this platform, so only one is listed)
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

} // namespace nlm