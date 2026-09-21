// Implementation file for Config class with improved validation and type safety

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <numeric>
#include <stdexcept>
#include <limits>

namespace nlm {

// Forward declaration for static utility functions
namespace {
    std::string trim(const std::string& str);
    std::string toLower(const std::string& str);
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    
    // Validation rules for configuration entries
    struct ValidationRule {
        std::string key;
        std::function<bool(const ConfigValue&)> validator;
        std::string errorMessage;
    };
    std::vector<ValidationRule> validationRules;
    
    // Schema definitions for configuration keys
    struct Schema {
        std::string key;
        std::string type;
        std::string description;
        std::string validationRule;
    };
    std::vector<Schema> schemas;
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

bool Config::loadFromArgs(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine);
            }
        }
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
    
    if (std::holds_alternative<T>(it->value)) {
        return std::get<T>(it->value);
    }
    
    return tryConvertType<T>(it->value);
}

template<typename T>
std::optional<T> Config::tryConvertType(const ConfigValue& value) const {
    if constexpr (std::is_same_v<T, int64_t>) {
        if (std::holds_alternative<int>(value)) {
            return static_cast<int64_t>(std::get<int>(value));
        }
    } else if constexpr (std::is_same_v<T, float>) {
        if (std::holds_alternative<double>(value)) {
            return static_cast<float>(std::get<double>(value));
        }
    } else if constexpr (std::is_same_v<T, bool>) {
        if (std::holds_alternative<int>(value)) {
            return static_cast<bool>(std::get<int>(value));
        } else if (std::holds_alternative<double>(value)) {
            return static_cast<bool>(std::get<double>(value));
        }
    }
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
void Config::set(const std::string& key, const T& value, ConfigSource source) {
    if (!validateKey(key) || !validateValue(value)) {
        return;
    }
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (!validateKey(key) || !validateValue(value)) {
        return;
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
    if (!validateKey(key) || !validateValue(value)) {
        return;
    }
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    if (!validateKey(key) || !validateValue(value)) {
        return;
    }
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    if (!validateKey(key) || !validateValue(value)) {
        return;
    }
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    if (!validateKey(key) || !validateValue(value)) {
        return;
    }
    set(key, ConfigValue(value), source);
}

template<typename T>
std::optional<T> Config::getValidated(const std::string& key) const {
    if (!validateKey(key)) {
        return std::nullopt;
    }
    
    auto value = get<T>(key);
    if (!value) {
        return std::nullopt;
    }
    
    if (!validateValue(*value)) {
        return std::nullopt;
    }
    
    return value;
}

bool Config::validateKey(const std::string& key) const {
    if (key.empty()) {
        return false;
    }
    
    if (key.find('=') != std::string::npos) {
        return false;
    }
    
    if (key.find(' ') != std::string::npos || key.find('\t') != std::string::npos) {
        return false;
    }
    
    return true;
}

bool Config::validateValue(const ConfigValue& value) const {
    return true;
}

std::string Config::getValidationErrorMessage(const std::string& key, const ConfigValue& value) const {
    return "Invalid configuration: key='" + key + "', value type not supported";
}

template<typename T>
bool Config::validateValue(const T& value) const {
    if constexpr (std::is_same_v<T, std::string>) {
        return !value.empty();
    } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
        return true;
    } else if constexpr (std::is_same_v<T, double>) {
        return std::isfinite(value);
    } else if constexpr (std::is_same_v<T, bool>) {
        return true;
    } else if constexpr (std::is_same_v<T, std::vector<int>>) {
        return true;
    } else if constexpr (std::is_same_v<T, std::vector<double>>) {
        for (const double& val : value) {
            if (!std::isfinite(val)) return false;
        }
        return true;
    } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
        for (const std::string& val : value) {
            if (val.empty()) return false;
        }
        return true;
    }
    return true;
}

template<typename From, typename To>
std::optional<To> Config::convertType(const From& value) const {
    if constexpr (std::is_same_v<From, To>) {
        return value;
    } else if constexpr (std::is_same_v<From, int> && std::is_same_v<To, int64_t>) {
        return static_cast<To>(value);
    } else if constexpr (std::is_same_v<From, double> && std::is_same_v<To, float>) {
        return static_cast<To>(value);
    } else if constexpr (std::is_same_v<From, int> && std::is_same_v<To, bool>) {
        return static_cast<To>(value);
    } else if constexpr (std::is_same_v<From, double> && std::is_same_v<To, bool>) {
        return static_cast<To>(value);
    }
    return std::nullopt;
}

template<typename T>
bool Config::isValidValue(const std::string& key, const T& value) const {
    return validateValue(value);
}

template<typename T>
void Config::setSchema(const std::string& key, const std::string& type,
                       const std::string& description,
                       const std::string& validationRule) {
    if (!validateKey(key)) {
        return;
    }
    
    T defaultValue;
    if constexpr (std::is_same_v<T, std::string>) {
        defaultValue = "";
    } else if constexpr (std::is_same_v<T, int>) {
        defaultValue = 0;
    } else if constexpr (std::is_same_v<T, int64_t>) {
        defaultValue = 0;
    } else if constexpr (std::is_same_v<T, double>) {
        defaultValue = 0.0;
    } else if constexpr (std::is_same_v<T, bool>) {
        defaultValue = false;
    }
    
    set(key, defaultValue, ConfigSource::Default);
}

bool Config::hasSchema(const std::string& key) const {
    return std::any_of(pImpl->schemas.begin(), pImpl->schemas.end(),
                      [&key](const Schema& schema) { return schema.key == key; });
}

std::string Config::getSchema(const std::string& key) const {
    auto it = std::find_if(pImpl->schemas.begin(), pImpl->schemas.end(),
                          [&key](const Schema& schema) { return schema.key == key; });
    return it != pImpl->schemas.end() ? it->type : "";
}

void Config::addValidationRule(const std::string& key,
                               std::function<bool(const ConfigValue&)> validator,
                               const std::string& errorMessage) {
    if (!validateKey(key)) {
        return;
    }
    
    pImpl->validationRules.push_back({key, validator, errorMessage});
}

bool Config::validateAll() const {
    for (const auto& rule : pImpl->validationRules) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&rule](const ConfigEntry& e) { return e.key == rule.key; });
        
        if (it != pImpl->entries.end() && !rule.validator(it->value)) {
            return false;
        }
    }
    return true;
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    for (const auto& rule : pImpl->validationRules) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
                              [&rule](const ConfigEntry& e) { return e.key == rule.key; });
        
        if (it != pImpl->entries.end() && !rule.validator(it->value)) {
            errors.push_back(rule.errorMessage);
        }
    }
    
    return errors;
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

// Static utility functions
namespace {
    std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    }
    
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
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

template bool Config::validateValue<int>(const int&) const;
template bool Config::validateValue<int64_t>(const int64_t&) const;
template bool Config::validateValue<double>(const double&) const;
template bool Config::validateValue<bool>(const bool&) const;
template bool Config::validateValue<std::string>(const std::string&) const;
template bool Config::validateValue<std::vector<int>>(const std::vector<int>&) const;
template bool Config::validateValue<std::vector<double>>(const std::vector<double>&) const;
template bool Config::validateValue<std::vector<std::string>>(const std::vector<std::string>&) const;

template bool Config::isValidValue<int>(const std::string&, const int&) const;
template bool Config::isValidValue<int64_t>(const std::string&, const int64_t&) const;
template bool Config::isValidValue<double>(const std::string&, const double&) const;
template bool Config::isValidValue<bool>(const std::string&, const bool&) const;
template bool Config::isValidValue<std::string>(const std::string&, const std::string&) const;

template std::optional<int64_t> Config::convertType<int, int64_t>(const int&) const;
template std::optional<float> Config::convertType<double, float>(const double&) const;
template std::optional<bool> Config::convertType<int, bool>(const int&) const;
template std::optional<bool> Config::convertType<double, bool>(const double&) const;

template void Config::set(const std::string& key, const int& value, ConfigSource source);
template void Config::set(const std::string& key, const double& value, ConfigSource source);
template void Config::set(const std::string& key, const bool& value, ConfigSource source);
template void Config::set(const std::string& key, const std::string& value, ConfigSource source);

template void Config::setSchema<int>(const std::string& key, const std::string& type,
                                    const std::string& description,
                                    const std::string& validationRule);
template void Config::setSchema<double>(const std::string& key, const std::string& type,
                                       const std::string& description,
                                       const std::string& validationRule);
template void Config::setSchema<bool>(const std::string& key, const std::string& type,
                                     const std::string& description,
                                     const std::string& validationRule);
template void Config::setSchema<std::string>(const std::string& key, const std::string& type,
                                            const std::string& description,
                                            const std::string& validationRule);

template void Config::addValidationRule(const std::string& key,
                                       std::function<bool(const ConfigValue&)> validator,
                                       const std::string& errorMessage);

} // namespace nlm