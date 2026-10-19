#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <numeric>
#include <limits>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigMetadata> metadata;
    std::map<std::string, std::vector<ValidatorFunc>> validators;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath, bool validate) {
    // First, try to parse as simple key=value format
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_WARNING("Config file not found: " + filepath);
        return false;
    }
    
    std::string line;
    bool success = true;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        std::string key, value;
        if (parseFileLine(line, key, value)) {
            auto parsedValue = parseValue(value, key);
            if (parsedValue.has_value()) {
                set(key, parsedValue.value(), ConfigSource::File, validate);
            } else {
                NLM_LOG_WARNING("Failed to parse config value for key: " + key);
                success = false;
            }
        }
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv, bool validate) {
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                set(key, value, ConfigSource::CommandLine, validate);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine, validate);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath, bool pretty) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Sort entries by category for better organization
    std::vector<std::pair<std::string, const ConfigEntry*>> sortedEntries;
    for (const auto& entry : pImpl->entries) {
        sortedEntries.emplace_back(entry.metadata.category, &entry);
    }
    
    std::sort(sortedEntries.begin(), sortedEntries.end(),
        [](const auto& a, const auto& b) {
            return a.first < b.first;
        });
    
    for (const auto& entryPair : sortedEntries) {
        const auto& entry = *entryPair.second;
        
        // Add section header for category
        if (!entry.metadata.category.empty()) {
            file << "# " << entry.metadata.category << "\n";
            file << "# " << std::string(entry.metadata.category.length(), '=') << "\n";
        }
        
        file << "# " << entry.metadata.description << "\n";
        file << entry.key << " = ";
        
        // Properly serialize different value types
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (val ? "true" : "false");
            } else {
                file << val;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key, bool validate) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    // Validate if requested
    if (validate) {
        auto validationResult = validate(key);
        if (!validationResult.valid) {
            NLM_LOG_WARNING("Validation failed for key '" + key + "': " + validationResult.errorMessage);
            return std::nullopt;
        }
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue, bool validate) const {
    auto val = get<T>(key, validate);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, bool validate) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    // Validate if requested and metadata exists
    if (validate && pImpl->metadata.find(key) != pImpl->metadata.end()) {
        auto validationResult = validate(key);
        if (!validationResult.valid) {
            NLM_LOG_ERROR("Invalid configuration value for key '" + key + "': " + validationResult.errorMessage);
            // Remove invalid entry
            remove(key);
        }
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source, bool validate) {
    auto parsedValue = parseValue(value, key);
    if (parsedValue.has_value()) {
        set(key, parsedValue.value(), source, validate);
    } else {
        NLM_LOG_ERROR("Failed to parse string value for key: " + key + " -> '" + value + "'");
    }
}

void Config::set(const std::string& key, int value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, double value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

void Config::set(const std::string& key, bool value, ConfigSource source, bool validate) {
    set(key, ConfigValue(value), source, validate);
}

bool Config::addValidator(const std::string& key, const ValidatorFunc& validator) {
    if (pImpl->validators.find(key) == pImpl->validators.end()) {
        pImpl->validators[key] = std::vector<ValidatorFunc>();
    }
    pImpl->validators[key].push_back(validator);
    return true;
}

ValidationResult Config::validate(const std::string& key) const {
    // Check if key exists
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ValidationResult::failure("Key does not exist");
    }
    
    // Apply registered validators
    if (pImpl->validators.find(key) != pImpl->validators.end()) {
        for (const auto& validator : pImpl->validators[key]) {
            ValidationResult result = validator(key, it->value);
            if (!result.valid) {
                return result;
            }
        }
    }
    
    // Apply metadata validation if available
    if (pImpl->metadata.find(key) != pImpl->metadata.end()) {
        const auto& metadata = pImpl->metadata[key];
        
        // Check if required
        if (metadata.required && std::holds_alternative<std::monostate>(it->value)) {
            return ValidationResult::failure("Required key has no value");
        }
        
        // Check value type
        if (!std::holds_alternative<int>(it->value) && 
            !std::holds_alternative<int64_t>(it->value) &&
            !std::holds_alternative<double>(it->value) &&
            !std::holds_alternative<bool>(it->value) &&
            !std::holds_alternative<std::string>(it->value)) {
            return ValidationResult::failure("Invalid value type");
        }
        
        // Check range constraints
        if (std::holds_alternative<double>(it->value) || 
            std::holds_alternative<int>(it->value) ||
            std::holds_alternative<int64_t>(it->value)) {
            
            double val = 0.0;
            if (std::holds_alternative<double>(it->value)) {
                val = std::get<double>(it->value);
            } else if (std::holds_alternative<int>(it->value)) {
                val = static_cast<double>(std::get<int>(it->value));
            } else {
                val = static_cast<double>(std::get<int64_t>(it->value));
            }
            
            return validateNumeric(key, it->value, metadata.minValue, metadata.maxValue);
        }
        
        // Check enum values
        if (!metadata.allowedValues.empty() && std::holds_alternative<std::string>(it->value)) {
            const std::string& val = std::get<std::string>(it->value);
            auto itAllowed = std::find(metadata.allowedValues.begin(), metadata.allowedValues.end(), val);
            if (itAllowed == metadata.allowedValues.end()) {
                return ValidationResult::failure("Value not in allowed list");
            }
        }
        
        // Check string length
        if (std::holds_alternative<std::string>(it->value)) {
            const std::string& val = std::get<std::string>(it->value);
            if (val.length() < metadata.minLength) {
                return ValidationResult::failure("String too short (minimum " + 
                    std::to_string(metadata.minLength) + " characters)");
            }
            if (val.length() > metadata.maxLength) {
                return ValidationResult::failure("String too long (maximum " + 
                    std::to_string(metadata.maxLength) + " characters)");
            }
        }
        
        // Use custom validator if provided
        if (metadata.validator) {
            return metadata.validator(key, it->value);
        }
    }
    
    return ValidationResult::success();
}

std::vector<std::string> Config::validateAll() const {
    std::vector<std::string> errors;
    for (const auto& entry : pImpl->entries) {
        ValidationResult result = validate(entry.key);
        if (!result.valid) {
            errors.push_back("Key '" + entry.key + "': " + result.errorMessage);
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
    pImpl->validators.erase(key);
    pImpl->metadata.erase(key);
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
    for (const auto& entry : pImpl->entries) {
        if (entry.metadata.category == category) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->validators.clear();
    pImpl->metadata.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << val << "\"";
            } else {
                oss << val;
            }
        }, entry.value);
        oss << "] (source: " << static_cast<int>(entry.source) << ")";
        if (!entry.metadata.description.empty()) {
            oss << " - " << entry.metadata.description;
        }
        oss << "\n";
    }
    return oss.str();
}

bool Config::registerConfigOption(const std::string& key, const ConfigMetadata& metadata) {
    pImpl->metadata[key] = metadata;
    return true;
}

ConfigMetadata Config::getMetadata(const std::string& key) const {
    auto it = pImpl->metadata.find(key);
    if (it != pImpl->metadata.end()) {
        return it->second;
    }
    return ConfigMetadata();
}

bool Config::isRegistered(const std::string& key) const {
    return pImpl->metadata.find(key) != pImpl->metadata.end();
}

// Implementation helpers
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

bool Config::parseFileLine(const std::string& line, std::string& key, std::string& value) {
    size_t pos = line.find('=');
    if (pos != std::string::npos) {
        key = trim(line.substr(0, pos));
        value = trim(line.substr(pos + 1));
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '\"' && value.back() == '\"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        return true;
    }
    return false;
}

std::string Config::serializeValue(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << val << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (val ? "true" : "false");
        } else {
            oss << val;
        }
    }, value);
    return oss.str();
}

std::optional<ConfigValue> Config::parseValue(const std::string& str, const std::string& key) {
    // Try to determine type
    if (str.empty()) {
        return std::nullopt;
    }
    
    // Check for boolean
    std::string lowerStr = toLower(str);
    if (lowerStr == "true" || lowerStr == "false") {
        return lowerStr == "true";
    }
    
    // Check for numeric values
    try {
        if (str.find('.') != std::string::npos) {
            return std::stod(str);
        } else {
            size_t pos;
            int64_t intVal = std::stoll(str, &pos);
            if (pos == str.size()) {
                return intVal;
            }
        }
    } catch (const std::exception&) {}
    
    // Check for integer
    try {
        if (str.find('.') == std::string::npos) {
            return std::stoi(str);
        }
    } catch (const std::exception&) {}
    
    // Default to string
    return str;
}

ValidationResult Config::validateNumeric(const std::string& key, const ConfigValue& value, double min, double max) {
    double val = 0.0;
    if (std::holds_alternative<double>(value)) {
        val = std::get<double>(value);
    } else if (std::holds_alternative<int>(value)) {
        val = static_cast<double>(std::get<int>(value));
    } else if (std::holds_alternative<int64_t>(value)) {
        val = static_cast<double>(std::get<int64_t>(value));
    } else {
        return ValidationResult::failure("Value is not numeric");
    }
    
    if (val < min) {
        return ValidationResult::failure("Value " + std::to_string(val) + " is less than minimum " + std::to_string(min));
    }
    
    if (val > max) {
        return ValidationResult::failure("Value " + std::to_string(val) + " is greater than maximum " + std::to_string(max));
    }
    
    return ValidationResult::success();
}

ValidationResult Config::validateEnum(const std::string& key, const ConfigValue& value, const std::vector<std::string>& allowed) {
    if (!std::holds_alternative<std::string>(value)) {
        return ValidationResult::failure("Value must be a string for enum validation");
    }
    
    const std::string& val = std::get<std::string>(value);
    auto it = std::find(allowed.begin(), allowed.end(), val);
    if (it == allowed.end()) {
        return ValidationResult::failure("Value '" + val + "' not in allowed list");
    }
    
    return ValidationResult::success();
}

ValidationResult Config::validateString(const std::string& key, const ConfigValue& value, size_t minLength, size_t maxLength) {
    if (!std::holds_alternative<std::string>(value)) {
        return ValidationResult::failure("Value must be a string for string validation");
    }
    
    const std::string& val = std::get<std::string>(value);
    if (val.length() < minLength) {
        return ValidationResult::failure("String too short (minimum " + 
            std::to_string(minLength) + " characters)");
    }
    if (val.length() > maxLength) {
        return ValidationResult::failure("String too long (maximum " + 
            std::to_string(maxLength) + " characters)");
    }
    
    return ValidationResult::success();
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