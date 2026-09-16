#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <numeric>
#include <regex>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, KeyValidationRules> key_rules;
    std::unordered_map<std::string, ValueValidationRules> value_rules;
    std::unordered_map<std::string, ValidationCallback> callbacks;
    ValidationMode validation_mode = ValidationMode::Strict;
    bool strict_validation_on_set = true;
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

// Include missing headers
#include <numeric>
#include <regex>

// After the setters and getters, add these new validation functions

// Private validation helper functions
ValidationResult Config::validateKey(const std::string& key) const {
    const auto& rules = pImpl->key_rules.count(key) ? pImpl->key_rules.at(key) : KeyValidationRules();
    
    if (rules.allow_empty && key.empty()) {
        return ValidationResult::KeyFormatInvalid;
    }
    
    if (rules.max_length > 0 && key.length() > rules.max_length) {
        return ValidationResult::KeyFormatInvalid;
    }
    
    if (!rules.allowed_pattern.empty()) {
        try {
            if (!std::regex_match(key, std::regex(rules.allowed_pattern))) {
                return ValidationResult::KeyFormatInvalid;
            }
        } catch (const std::regex_error&) {
            return ValidationResult::KeyFormatInvalid;
        }
    }
    
    if (!rules.allow_special_chars) {
        for (char c : key) {
            if (!std::isalnum(c) && c != '_') {
                return ValidationResult::KeyFormatInvalid;
            }
        }
    }
    
    for (const auto& prefix : rules.reserved_prefixes) {
        if (key.rfind(prefix, 0) == 0) {
            return ValidationResult::KeyFormatInvalid;
        }
    }
    
    return ValidationResult::Valid;
}

ValidationResult Config::validateValue(const std::string& key, const ConfigValue& value) const {
    auto it = pImpl->entries.begin();
    for (; it != pImpl->entries.end(); ++it) {
        if (it->key == key) {
            break;
        }
    }
    if (it == pImpl->entries.end()) {
        return ValidationResult::Valid;
    }
    
    const auto& rules = pImpl->value_rules.count(key) ? pImpl->value_rules.at(key) : ValueValidationRules();
    
    return validateValueInternal(key, value, rules);
}

ValidationResult Config::validateValueInternal(const std::string& key, const ConfigValue& value, const ValueValidationRules& rules) const {
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || std::is_same_v<T, double>) {
            double num = val;
            if (!rules.allow_negative_numbers && num < 0) {
                return ValidationResult::ValueOutOfRange;
            }
            if (!rules.allow_zero && num == 0) {
                return ValidationResult::ValueOutOfRange;
            }
            
            const auto& bounds = rules.numeric_bounds.find(key);
            if (bounds != rules.numeric_bounds.end()) {
                const auto& b = bounds->second;
                if (b.has_min && num < b.min) {
                    return ValidationResult::ValueOutOfRange;
                }
                if (b.has_max && num > b.max) {
                    return ValidationResult::ValueOutOfRange;
                }
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (rules.require_non_empty_string && val.empty()) {
                return ValidationResult::DefaultInvalid;
            }
            
            if (rules.validate_file_paths && !isValidFilePath(val)) {
                return ValidationResult::PathInvalid;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            // No validation for boolean
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                           std::is_same_v<T, std::vector<double>> ||
                           std::is_same_v<T, std::vector<std::string>>) {
            if (rules.validate_array_non_empty && val.empty()) {
                return ValidationResult::DefaultInvalid;
            }
            
            if (rules.validate_file_paths) {
                for (const auto& item : val) {
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        if (!isValidFilePath(item)) {
                            return ValidationResult::PathInvalid;
                        }
                    }
                }
            }
        }
        
        const auto& enums = rules.allowed_enums.find(key);
        if (enums != rules.allowed_enums.end()) {
            std::string enum_val;
            if constexpr (std::is_same_v<T, int>) {
                enum_val = std::to_string(val);
            } else if constexpr (std::is_same_v<T, std::string>) {
                enum_val = val;
            } else {
                return ValidationResult::EnumValueInvalid;
            }
            
            if (std::find(enums->second.begin(), enums->second.end(), enum_val) == enums->second.end()) {
                return ValidationResult::EnumValueInvalid;
            }
        }
    }, value);
    
    return ValidationResult::Valid;
}

bool Config::isValidFilePath(const std::string& path) const {
    std::error_code ec;
    if (std::filesystem::exists(path, ec) && std::filesystem::is_regular_file(path, ec)) {
        return true;
    }
    return false;
}

void Config::setValidationMode(ValidationMode mode) {
    pImpl->validation_mode = mode;
}

ValidationMode Config::getValidationMode() const {
    return pImpl->validation_mode;
}

void Config::setKeyValidationRules(const std::string& key, const KeyValidationRules& rules) {
    pImpl->key_rules[key] = rules;
}

void Config::setValueValidationRules(const std::string& key, const ValueValidationRules& rules) {
    pImpl->value_rules[key] = rules;
}

void Config::addValidationCallback(const std::string& key, const ValidationCallback& callback) {
    pImpl->callbacks[key] = callback;
}

void Config::setStrictValidation(bool strict) {
    pImpl->strict_validation_on_set = strict;
}

bool Config::hasStrictValidation() const {
    return pImpl->strict_validation_on_set;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (pImpl->strict_validation_on_set) {
        ValidationResult keyResult = validateKey(key);
        if (keyResult != ValidationResult::Valid) {
            std::string errorMsg = "Key validation failed: ";
            switch (keyResult) {
                case ValidationResult::KeyFormatInvalid:
                    errorMsg += "Key format is invalid";
                    break;
                case ValidationResult::EnumValueInvalid:
                    errorMsg += "Enum value is invalid";
                    break;
                case ValidationResult::PathInvalid:
                    errorMsg += "Path is invalid";
                    break;
                case ValidationResult::DefaultInvalid:
                    errorMsg += "Default value is invalid";
                    break;
                default:
                    errorMsg += "Unknown validation error";
            }
            throw ValidationError(errorMsg, key, "");
        }
        
        ValidationResult valueResult = validateValue(key, value);
        if (valueResult != ValidationResult::Valid) {
            std::string errorMsg = "Value validation failed: ";
            switch (valueResult) {
                case ValidationResult::TypeMismatch:
                    errorMsg += "Type mismatch";
                    break;
                case ValidationResult::ValueOutOfRange:
                    errorMsg += "Value out of range";
                    break;
                case ValidationResult::EnumValueInvalid:
                    errorMsg += "Enum value is invalid";
                    break;
                case ValidationResult::PathInvalid:
                    errorMsg += "Path is invalid";
                    break;
                case ValidationResult::DefaultInvalid:
                    errorMsg += "Default value is invalid";
                    break;
                default:
                    errorMsg += "Unknown validation error";
            }
            throw ValidationError(errorMsg, key, "");
        }
        
        for (const auto& pair : pImpl->callbacks) {
            if (pair.first.empty() || pair.first == key) {
                ValidationResult callbackResult = pair.second(key, value, "set");
                if (callbackResult != ValidationResult::Valid) {
                    throw ValidationError("Callback validation failed", key, "");
                }
            }
        }
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

ValidationResult Config::validateValue(const std::string& key, const ConfigValue& value) const {
    auto it = pImpl->entries.begin();
    for (; it != pImpl->entries.end(); ++it) {
        if (it->key == key) {
            break;
        }
    }
    if (it == pImpl->entries.end()) {
        return ValidationResult::Valid;
    }
    
    const auto& rules = pImpl->value_rules.count(key) ? pImpl->value_rules.at(key) : ValueValidationRules();n    
    return validateValueInternal(key, value, rules);
}

ValidationResult Config::validateValueInternal(const std::string& key, const ConfigValue& value, const ValueValidationRules& rules) const {
    std::visit([&](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        
        if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t> || std::is_same_v<T, double>) {
            double num = val;
            if (!rules.allow_negative_numbers && num < 0) {
                return ValidationResult::ValueOutOfRange;
            }
            if (!rules.allow_zero && num == 0) {
                return ValidationResult::ValueOutOfRange;
            }
            
            const auto& bounds = rules.numeric_bounds.find(key);
            if (bounds != rules.numeric_bounds.end()) {
                const auto& b = bounds->second;
                if (b.has_min && num < b.min) {
                    return ValidationResult::ValueOutOfRange;
                }
                if (b.has_max && num > b.max) {
                    return ValidationResult::ValueOutOfRange;
                }
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (rules.require_non_empty_string && val.empty()) {
                return ValidationResult::DefaultInvalid;
            }
            
            if (rules.validate_file_paths && !isValidFilePath(val)) {
                return ValidationResult::PathInvalid;
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            // No validation for boolean
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                           std::is_same_v<T, std::vector<double>> ||
                           std::is_same_v<T, std::vector<std::string>>) {
            if (rules.validate_array_non_empty && val.empty()) {
                return ValidationResult::DefaultInvalid;
            }
            
            if (rules.validate_file_paths) {
                for (const auto& item : val) {
                    if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                        if (!isValidFilePath(item)) {
                            return ValidationResult::PathInvalid;
                        }
                    }
                }
            }
        }
        
        const auto& enums = rules.allowed_enums.find(key);
        if (enums != rules.allowed_enums.end()) {
            std::string enum_val;
            if constexpr (std::is_same_v<T, int>) {
                enum_val = std::to_string(val);
            } else if constexpr (std::is_same_v<T, std::string>) {
                enum_val = val;
            } else {
                return ValidationResult::EnumValueInvalid;
            }
            
            if (std::find(enums->second.begin(), enums->second.end(), enum_val) == enums->second.end()) {
                return ValidationResult::EnumValueInvalid;
            }
        }
    }, value);
    
    return ValidationResult::Valid;
}

std::string Config::isValidFilePath(const std::string& path) const {
    std::error_code ec;
    if (std::filesystem::exists(path, ec) && std::filesystem::is_regular_file(path, ec)) {
        return path;
    }
    return "";
}

void Config::setValidationMode(ValidationMode mode) {
    pImpl->validation_mode = mode;
}

ValidationMode Config::getValidationMode() const {
    return pImpl->validation_mode;
}

void Config::setKeyValidationRules(const std::string& key, const KeyValidationRules& rules) {
    pImpl->key_rules[key] = rules;
}

void Config::setValueValidationRules(const std::string& key, const ValueValidationRules& rules) {
    pImpl->value_rules[key] = rules;
}

void Config::addValidationCallback(const std::string& key, const ValidationCallback& callback) {
    pImpl->callbacks[key] = callback;
}

void Config::setStrictValidation(bool strict) {
    pImpl->strict_validation_on_set = strict;
}

bool Config::hasStrictValidation() const {
    return pImpl->strict_validation_on_set;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (pImpl->strict_validation_on_set) {
        ValidationResult keyResult = validateKey(key);
        if (keyResult != ValidationResult::Valid) {
            std::string errorMsg = "Key validation failed: ";
            switch (keyResult) {
                case ValidationResult::KeyFormatInvalid:
                    errorMsg += "Key format is invalid";
                    break;
                case ValidationResult::EnumValueInvalid:
                    errorMsg += "Enum value is invalid";
                    break;
                case ValidationResult::PathInvalid:
                    errorMsg += "Path is invalid";
                    break;
                case ValidationResult::DefaultInvalid:
                    errorMsg += "Default value is invalid";
                    break;
                default:
                    errorMsg += "Unknown validation error";
            }
            throw ValidationError(errorMsg, key, "");
        }
        
        ValidationResult valueResult = validateValue(key, value);
        if (valueResult != ValidationResult::Valid) {
            std::string errorMsg = "Value validation failed: ";
            switch (valueResult) {
                case ValidationResult::TypeMismatch:
                    errorMsg += "Type mismatch";
                    break;
                case ValidationResult::ValueOutOfRange:
                    errorMsg += "Value out of range";
                    break;
                case ValidationResult::EnumValueInvalid:
                    errorMsg += "Enum value is invalid";
                    break;
                case ValidationResult::PathInvalid:
                    errorMsg += "Path is invalid";
                    break;
                case ValidationResult::DefaultInvalid:
                    errorMsg += "Default value is invalid";
                    break;
                default:
                    errorMsg += "Unknown validation error";
            }
            throw ValidationError(errorMsg, key, "");
        }
        
        for (const auto& pair : pImpl->callbacks) {
            if (pair.first.empty() || pair.first == key) {
                ValidationResult callbackResult = pair.second(key, value, "set");
                if (callbackResult != ValidationResult::Valid) {
                    throw ValidationError("Callback validation failed", key, "");
                }
            }
        }
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
