#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // Validation cache
    mutable bool validationDirty;
    mutable std::vector<ValidationError> validationErrors;
    
    Impl() : entries(), validationDirty(true), validationErrors() {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
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

bool Config::loadFromArgs(const std::vector<std::string>& args) {
    for (const std::string& arg : args) {
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
        else if (!arg.empty() && arg[0] == '-') {
            // This is a simplified implementation - in a full version we'd need to parse the next arg
            // For now, treat the entire arg as a key with empty value
            std::string key = arg.substr(1);
            set(key, "", ConfigSource::CommandLine);
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

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
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

void Config::set(const std::string& key, const std::vector<int64_t>& value, ConfigSource source) {
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

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](const auto& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>> ||
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

// Main implementation of the set method that updates entries
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        // Mark validation as dirty when a value is modified
        pImpl->validationDirty = true;
    } else {
        pImpl->entries.emplace_back(key, value, source);
        // Mark validation as dirty when a new entry is added
        pImpl->validationDirty = true;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Template method implementations for type-safe getters

template<typename T>
bool Config::get(const std::string& key, T& value) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return false;
    }
    return getValueFromVariant(it->value, value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    T value;
    if (get<T>(key, value)) {
        return value;
    }
    return defaultValue;
}

// Validation methods implementation

bool Config::validate() const {
    std::vector<ValidationError> errors = validateDetailed();
    return errors.empty();
}

std::string Config::getValidationErrors() const {
    std::vector<ValidationError> errors = validateDetailed();
    if (errors.empty()) {
        return "Configuration is valid.";
    }
    
    std::ostringstream oss;
    oss << "Configuration validation failed with " << errors.size() << " error(s):\n";
    for (const auto& error : errors) {
        oss << "  " << error.message << " (key: '" << error.key << "')\n";
    }
    return oss.str();
}

std::vector<ValidationError> Config::validateDetailed() const {
    std::vector<ValidationError> errors;
    
    for (const auto& entry : pImpl->entries) {
        // Validate key
        if (!isValidKey(entry.key)) {
            errors.emplace_back(ConfigValidationError::InvalidKey, entry.key, 
                "Invalid configuration key format");
            continue;
        }
        
        // Validate type based on key name and expected type
        errors.insert(errors.end(), validateEntryType(entry).begin(), validateEntryType(entry).end());
        
        // Validate required keys
        if (isRequiredKey(entry.key)) {
            if (entry.value.index() == 0 || // Empty variant
                (std::get_if<double>(&entry.value) != nullptr && std::get<double>(entry.value) == 0.0) ||
                (std::get_if<bool>(&entry.value) != nullptr && !std::get<bool>(entry.value)) ||
                (std::get_if<std::string>(&entry.value) != nullptr && std::get<std::string>(entry.value).empty()) ||
                (std::get_if<std::vector<int64_t>>(&entry.value) != nullptr && std::get<std::vector<int64_t>>(entry.value).empty()) ||
                (std::get_if<std::vector<double>>(&entry.value) != nullptr && std::get<std::vector<double>>(entry.value).empty()) ||
                (std::get_if<std::vector<std::string>>(&entry.value) != nullptr && std::get<std::vector<std::string>>(entry.value).empty())) {
                errors.emplace_back(ConfigValidationError::MissingRequired, entry.key,
                    "Required configuration key is missing or has empty value");
            }
        }
        
        // Validate range for numeric values
        errors.insert(errors.end(), validateNumericRange(entry).begin(), validateNumericRange(entry).end());
    }
    
    return errors;
}

std::vector<ValidationError> Config::validateEntryType(const ConfigEntry& entry) const {
    std::vector<ValidationError> errors;
    
    // Type validation based on key name and expected type
    const std::string& key = entry.key;
    const ConfigValue& value = entry.value;
    
    // Check type compatibility with key name
    if (key == "random_seed") {
        if (const auto* ptr = std::get_if<int64_t>(&value)) {
            // Valid
        } else {
            errors.emplace_back(ConfigValidationError::InvalidType, key,
                "Expected int64 for random_seed");
        }
    } else if (key == "simulation_timestep") {
        if (const auto* ptr = std::get_if<double>(&value)) {
            if (*ptr <= 0.0) {
                errors.emplace_back(ConfigValidationError::OutOfRange, key,
                    "simulation_timestep must be positive");
            }
        } else {
            errors.emplace_back(ConfigValidationError::InvalidType, key,
                "Expected double for simulation_timestep");
        }
    } else if (key == "neuron_count") {
        if (const auto* ptr = std::get_if<int64_t>(&value)) {
            if (*ptr <= 0) {
                errors.emplace_back(ConfigValidationError::OutOfRange, key,
                    "neuron_count must be positive");
            } else if (*ptr > 10000) {
                errors.emplace_back(ConfigValidationError::OutOfRange, key,
                    "neuron_count must not exceed 10000");
            }
        } else {
            errors.emplace_back(ConfigValidationError::InvalidType, key,
                "Expected int64 for neuron_count");
        }
    } else if (key == "region_count") {
        if (const auto* ptr = std::get_if<int64_t>(&value)) {
            if (*ptr <= 0) {
                errors.emplace_back(ConfigValidationError::OutOfRange, key,
                    "region_count must be positive");
            }
        } else {
            errors.emplace_back(ConfigValidationError::InvalidType, key,
                "Expected int64 for region_count");
        }
    } else if (key == "connection_probability") {
        if (const auto* ptr = std::get_if<double>(&value)) {
            if (*ptr < 0.0 || *ptr > 1.0) {
                errors.emplace_back(ConfigValidationError::OutOfRange, key,
                    "connection_probability must be between 0.0 and 1.0");
            }
        } else {
            errors.emplace_back(ConfigValidationError::InvalidType, key,
                "Expected double for connection_probability");
        }
    } else if (key == "plasticity_learning_rate") {
        if (const auto* ptr = std::get_if<double>(&value)) {
            if (*ptr < 0.0) {
                errors.emplace_back(ConfigValidationError::OutOfRange, key,
                    "plasticity_learning_rate must be non-negative");
            }
        } else {
            errors.emplace_back(ConfigValidationError::InvalidType, key,
                "Expected double for plasticity_learning_rate");
        }
    }
    
    return errors;
}

std::vector<ValidationError> Config::validateNumericRange(const ConfigEntry& entry) const {
    std::vector<ValidationError> errors;
    
    const std::string& key = entry.key;
    const ConfigValue& value = entry.value;
    
    // Additional range validation based on key name
    if (key == "environment_width" || key == "environment_height") {
        if (const auto* ptr = std::get_if<int64_t>(&value)) {
            if (*ptr <= 0 || *ptr > 100) {
                errors.emplace_back(ConfigValidationError::OutOfRange, key,
                    "Environment dimensions must be between 1 and 100");
            }
        } else if (const auto* ptr = std::get_if<double>(&value)) {
            if (*ptr <= 0.0 || *ptr > 100.0) {
                errors.emplace_back(ConfigValidationError::OutOfRange, key,
                    "Environment dimensions must be between 1.0 and 100.0");
            }
        }
    }
    
    return errors;
}

bool Config::isRequiredKey(const std::string& key) const {
    // List of required configuration keys
    static const std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count"
    };
    
    return std::find(requiredKeys.begin(), requiredKeys.end(), key) != requiredKeys.end();
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
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

// Type-safe getter instantiations
template bool Config::get<int64_t>(const std::string&, int64_t&) const;
template bool Config::get<double>(const std::string&, double&) const;
template bool Config::get<float>(const std::string&, float&) const;
template bool Config::get<bool>(const std::string&, bool&) const;
template bool Config::get<std::string>(const std::string&, std::string&) const;
template bool Config::get<std::vector<int64_t>>(const std::string&, std::vector<int64_t>&) const;
template bool Config::get<std::vector<double>>(const std::string&, std::vector<double>&) const;
template bool Config::get<std::vector<std::string>>(const std::string&, std::vector<std::string>&) const;

template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
