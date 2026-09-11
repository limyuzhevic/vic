#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <numeric>
#include <unordered_set>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    ConfigSchema schema;
    mutable std::vector<ValidationError> validationErrors;
};

// Constructor and destructor
Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Load from file
bool Config::loadFromFile(const std::string& filepath) {
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
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

// Load from command line arguments
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
        file << entry.key << " = " << configValueToString(entry.value) << "\n";
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
    pImpl->schema.descriptors.clear();
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

void Config::addValidationDescriptor(const ConfigValidationDescriptor& descriptor) {
    pImpl->schema.addDescriptor(descriptor);
}

bool Config::validate() const {
    pImpl->validationErrors.clear();
    
    if (pImpl->schema.descriptors.empty()) {
        return true;
    }
    
    bool valid = true;
    for (const auto& descriptor : pImpl->schema.descriptors) {
        ValidationError error = validateSingleValue(descriptor.key, get<ConfigValue>(descriptor.key), descriptor);
        if (error.getType() != ValidationErrorType::None) {
            pImpl->validationErrors.push_back(error);
            valid = false;
        }
    }
    
    return valid;
}

std::vector<ValidationError> Config::validateWithDetails() const {
    pImpl->validationErrors.clear();
    
    if (pImpl->schema.descriptors.empty()) {
        return pImpl->validationErrors;
    }
    
    for (const auto& descriptor : pImpl->schema.descriptors) {
        ValidationError error = validateSingleValue(descriptor.key, get<ConfigValue>(descriptor.key), descriptor);
        if (error.getType() != ValidationErrorType::None) {
            pImpl->validationErrors.push_back(error);
        }
    }
    
    return pImpl->validationErrors;
}

ValidationError Config::getLastValidationError() const {
    if (pImpl->validationErrors.empty()) {
        return ValidationError();
    }
    return pImpl->validationErrors.back();
}

bool Config::setSchema(const ConfigSchema& schema) {
    pImpl->schema = schema;
    return true;
}

const ConfigSchema& Config::getSchema() const {
    return pImpl->schema;
}

void Config::clearSchema() {
    pImpl->schema.descriptors.clear();
}

template<typename T>
bool Config::isInBounds(const T& value, const std::optional<double>& min, const std::optional<double>& max) {
    if (!min.has_value() && !max.has_value()) {
        return true;
    }
    
    double numValue = 0.0;
    bool isNum = false;
    
    if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
        numValue = static_cast<double>(value);
        isNum = true;
    } else if constexpr (std::is_same_v<T, double>) {
        numValue = value;
        isNum = true;
    }
    
    if (!isNum) {
        return true;
    }
    
    if (min.has_value() && numValue < min.value()) {
        return false;
    }
    
    if (max.has_value() && numValue > max.value()) {
        return false;
    }
    
    return true;
}

std::string Config::configValueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        if constexpr (std::is_same_v<decltype(arg), std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool Config::configValueToBool(const ConfigValue& value) {
    bool result = false;
    std::visit([&result](auto&& arg) {
        if constexpr (std::is_same_v<decltype(arg), bool>) {
            result = arg;
        }
    }, value);
    return result;
}

double Config::configValueToDouble(const ConfigValue& value) {
    double result = 0.0;
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            result = static_cast<double>(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = static_cast<double>(arg);
        } else if constexpr (std::is_same_v<T, double>) {
            result = arg;
        }
    }, value);
    return result;
}

int64_t Config::configValueToInt64(const ConfigValue& value) {
    int64_t result = 0;
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            result = static_cast<int64_t>(arg);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            result = static_cast<int64_t>(arg);
        }
    }, value);
    return result;
}

// Validation helper method implementations
ValidationError Config::validateSingleValue(const std::string& key, const ConfigValue& value, const ConfigValidationDescriptor& descriptor) {
    if (descriptor.required && !has(key)) {
        return ValidationError("Missing required configuration key: " + key, 
                             ValidationErrorType::MissingRequired, key);
    }
    
    if (!has(key)) {
        return ValidationError();
    }
    
    auto actualValue = get<ConfigValue>(key);
    if (!actualValue) {
        return ValidationError("Could not retrieve configuration value for key: " + key,
                             ValidationErrorType::InvalidValue, key);
    }
    
    ValidationError typeError = validateType(key, actualValue.value(), descriptor.type);
    if (typeError.getType() != ValidationErrorType::None) {
        return typeError;
    }
    
    ValidationError rangeError = validateRange(key, actualValue.value(), descriptor.minValue, descriptor.maxValue);
    if (rangeError.getType() != ValidationErrorType::None) {
        return rangeError;
    }
    
    ValidationError allowedError = validateAllowedValues(key, actualValue.value(), descriptor.allowedValues);
    if (allowedError.getType() != ValidationErrorType::None) {
        return allowedError;
    }
    
    return ValidationError();
}

std::string Config::valueTypeToString(const ConfigValue& value) {
    std::string typeName;
    std::visit([&typeName](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) typeName = "int";
        else if constexpr (std::is_same_v<T, int64_t>) typeName = "int64";
        else if constexpr (std::is_same_v<T, double>) typeName = "double";
        else if constexpr (std::is_same_v<T, bool>) typeName = "bool";
        else if constexpr (std::is_same_v<T, std::string>) typeName = "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) typeName = "int_vector";
        else if constexpr (std::is_same_v<T, std::vector<double>>) typeName = "double_vector";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) typeName = "string_vector";
    }, value);
    return typeName;
}

bool Config::isNumber(const ConfigValue& value, double& outValue) {
    bool result = false;
    std::visit([&result, &outValue](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) { outValue = static_cast<double>(arg); result = true; }
        else if constexpr (std::is_same_v<T, int64_t>) { outValue = static_cast<double>(arg); result = true; }
        else if constexpr (std::is_same_v<T, double>) { outValue = arg; result = true; }
        else if constexpr (std::is_same_v<T, std::vector<int>>) { result = false; }
        else if constexpr (std::is_same_v<T, std::vector<double>>) { result = false; }
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) { result = false; }
        else if constexpr (std::is_same_v<T, bool>) { result = false; }
        else if constexpr (std::is_same_v<T, std::string>) { result = false; }
    }, value);
    return result;
}

ValidationError Config::validateType(const std::string& key, const ConfigValue& value, const std::string& expectedType) {
    std::string actualType = valueTypeToString(value);
    if (actualType != expectedType) {
        return ValidationError("Type mismatch for key " + key + ": expected " + expectedType + 
                             " but got " + actualType, ValidationErrorType::InvalidType, key, expectedType);
    }
    return ValidationError();
}

ValidationError Config::validateRange(const std::string& key, const ConfigValue& value, 
                                     const std::optional<double>& min, const std::optional<double>& max) {
    double numValue;
    if (!isNumber(value, numValue)) {
        return ValidationError();
    }
    
    if (min.has_value() && numValue < min.value()) {
        return ValidationError("Value for key " + key + " is below minimum allowed: " + 
                             std::to_string(numValue) + " < " + std::to_string(min.value()),
                             ValidationErrorType::OutOfRange, key, ">=" + std::to_string(min.value()));
    }
    
    if (max.has_value() && numValue > max.value()) {
        return ValidationError("Value for key " + key + " is above maximum allowed: " + 
                             std::to_string(numValue) + " > " + std::to_string(max.value()),
                             ValidationErrorType::OutOfRange, key, "<=" + std::to_string(max.value()));
    }
    
    return ValidationError();
}

ValidationError Config::validateAllowedValues(const std::string& key, const ConfigValue& value,
                                             const std::vector<std::string>& allowed) {
    if (allowed.empty()) {
        return ValidationError();
    }
    
    std::string strValue;
    std::visit([&strValue](auto&& arg) {
        if constexpr (std::is_same_v<decltype(arg), std::string>) {
            strValue = arg;
        } else {
            strValue = std::to_string(arg);
        }
    }, value);
    
    if (std::find(allowed.begin(), allowed.end(), strValue) == allowed.end()) {
        std::string allowedStr;
        for (size_t i = 0; i < allowed.size(); ++i) {
            if (i > 0) allowedStr += ", ";
            allowedStr += "'" + allowed[i] + "'";
        }
        return ValidationError("Value for key " + key + " is not in allowed set: '" + 
                             strValue + "' (allowed: " + allowedStr + ")",
                             ValidationErrorType::InvalidValue, key, allowedStr);
    }
    
    return ValidationError();
}

// Configuration key constants
const std::string Config::BRAIN_NEURON_COUNT = "brain.neuron_count";
const std::string Config::BRAIN_SYNAPSE_DENSITY = "brain.synapse_density";
const std::string Config::BRAIN_CONNECTION_PROBABILITY = "brain.connection_probability";
const std::string Config::PLASTICITY_STDP_ENABLE = "plasticity.stdp.enable";
const std::string Config::PLASTICITY_STDP_LEARNING_RATE = "plasticity.stdp.learning_rate";
const std::string Config::NEUROMOD_DOPAMINE_SCALE = "neuromod.dopamine.scale";

} // namespace nlm
