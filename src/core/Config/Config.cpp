#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ValidationConstraints> defaultConstraints;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check if nlohmann/json is available
#ifdef NLOHMANN_JSON_HPP
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann_json j;
        file >> j;
        
        // Clear existing entries
        pImpl->entries.clear();
        
        // Parse JSON object
        if (j.is_object()) {
            for (auto& [key, value] : j.items()) {
                ConfigValue configValue;
                fromJson(value, configValue);
                set(key, configValue, ConfigSource::File);
            }
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        // Fall back to legacy key=value format
        file.clear();
        file.seekg(0);
        
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
        nlohmann_json j;
        toJson(j, entry.value);
        file << entry.key << " = " << j.dump() << "\n\n";
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

bool Config::validate(const std::vector<ValidationRule>& rules) const {
    return getValidationErrors(rules).empty();
}

bool Config::validate(const std::string& key, const std::function<bool(const ConfigValue&)>& validator, 
                      const std::string& errorMessage) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return false;
    }
    
    return validateValue(it->value, validator);
}

std::vector<ValidationError> Config::getValidationErrors(const std::vector<ValidationRule>& rules) const {
    std::vector<ValidationError> errors;
    
    for (const auto& rule : rules) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&rule](const ConfigEntry& e) { return e.key == rule.key; });
        
        if (it == pImpl->entries.end()) {
            if (rule.required) {
                ValidationError error;
                error.key = rule.key;
                error.type = ValidationErrorType::RequiredValueMissing;
                error.message = "Required configuration value is missing: " + rule.key;
                errors.push_back(error);
            }
            continue;
        }
        
        if (!validateValue(it->value, rule.validator)) {
            ValidationError error;
            error.key = rule.key;
            error.type = ValidationErrorType::InvalidFormat;
            error.message = rule.errorMessage;
            error.value = valueToString(it->value);
            errors.push_back(error);
        }
    }
    
    // Apply default constraints
    for (const auto& constraint : pImpl->defaultConstraints) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&constraint](const ConfigEntry& e) { return e.key == constraint.key; });
        
        if (it != pImpl->entries.end()) {
            if (!validateValue(it->value, constraint.validator)) {
                ValidationError error;
                error.key = constraint.key;
                error.type = ValidationErrorType::InvalidType;
                error.message = constraint.errorMessage;
                error.value = valueToString(it->value);
                errors.push_back(error);
            }
        }
    }
    
    return errors;
}

void Config::setDefaultConstraints(const std::vector<ValidationConstraints>& constraints) {
    pImpl->defaultConstraints = constraints;
}

bool Config::validateValue(const ConfigValue& value, const std::function<bool(const ConfigValue&)>& validator) {
    return validator(value);
}

std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

// Internal helper implementations

void Config::fromJson(const nlohmann_json& j, ConfigValue& value) {
    if (j.is_number_integer()) {
        value = j.get<int>();
    } else if (j.is_number_unsigned()) {
        value = j.get<uint64_t>();
    } else if (j.is_number()) {
        value = j.get<double>();
    } else if (j.is_boolean()) {
        value = j.get<bool>();
    } else if (j.is_string()) {
        value = j.get<std::string>();
    } else if (j.is_array()) {
        std::vector<int> intArray;
        std::vector<double> doubleArray;
        std::vector<std::string> stringArray;
        
        for (const auto& item : j) {
            if (item.is_number_integer()) {
                intArray.push_back(item.get<int>());
                doubleArray.push_back(item.get<double>());
            } else if (item.is_number()) {
                doubleArray.push_back(item.get<double>());
            } else if (item.is_string()) {
                stringArray.push_back(item.get<std::string>());
            }
        }
        
        // Choose appropriate type based on content
        if (!stringArray.empty()) {
            value = std::move(stringArray);
        } else if (!doubleArray.empty()) {
            value = std::move(doubleArray);
        } else if (!intArray.empty()) {
            value = std::move(intArray);
        }
    } else {
        // Default to empty string for unknown types
        value = std::string();
    }
}

void Config::toJson(nlohmann_json& j, const ConfigValue& value) {
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        }
    }, value);
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