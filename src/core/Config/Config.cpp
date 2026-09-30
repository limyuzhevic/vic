#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>

namespace nlm {

// ConfigSchema implementation
struct ConfigSchema::Impl {
    std::vector<ConfigEntry> parameters;
    std::unordered_map<std::string, ValidationFunction> validators;
};

ConfigSchema::ConfigSchema() : pImpl(std::make_unique<Impl>()) {}

void ConfigSchema::addParameter(const std::string& key, const std::string& description,
                               const std::string& type, bool required,
                               const std::string& unit, const ValidationFunction& validator) {
    ConfigEntry entry;
    entry.key = key;
    entry.description = description;
    entry.type = type;
    entry.unit = unit;
    entry.required = required;
    
    if (validator) {
        pImpl->validators[key] = validator;
    }
    
    pImpl->parameters.push_back(entry);
}

void ConfigSchema::addParameterWithDefault(const std::string& key, const std::string& description,
                                          const ConfigValue& defaultValue, const std::string& type,
                                          ConfigSource source, const std::string& unit,
                                          const ValidationFunction& validator) {
    addParameter(key, description, type, false, unit, validator);
    // Note: We could store default values separately, but for now we just add the parameter
}

const ConfigEntry* ConfigSchema::getParameter(const std::string& key) const {
    auto it = std::find_if(pImpl->parameters.begin(), pImpl->parameters.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    return (it != pImpl->parameters.end()) ? &(*it) : nullptr;
}

std::vector<std::string> ConfigSchema::getParameterKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->parameters.size());
    for (const auto& param : pImpl->parameters) {
        keys.push_back(param.key);
    }
    return keys;
}

ValidationResult ConfigSchema::validate(const std::string& key, const ConfigValue& value) const {
    ValidationResult result;
    
    const ConfigEntry* param = getParameter(key);
    if (!param) {
        result.isValid = false;
        result.errors.push_back("Unknown configuration parameter: " + key);
        return result;
    }
    
    // Check type compatibility
    if (!std::holds_alternative<std::string>(value)) {
        result.isValid = false;
        result.errors.push_back("Parameter '" + key + "' expects type '" + param->type + 
                               "' but got " + getValueTypeName(value));
    }
    
    // Run custom validator if present
    auto validatorIt = pImpl->validators.find(key);
    if (validatorIt != pImpl->validators.end()) {
        ValidationResult validatorResult = validatorIt->second(key, value);
        if (!validatorResult.isValid) {
            result.isValid = false;
            result.errors.insert(result.errors.end(), validatorResult.errors.begin(), validatorResult.errors.end());
        }
        result.warnings.insert(result.warnings.end(), validatorResult.warnings.begin(), validatorResult.warnings.end());
    }
    
    return result;
}

ValidationResult ConfigSchema::validateAll(const std::vector<ConfigEntry>& entries) const {
    ValidationResult result;
    
    for (const auto& entry : entries) {
        ValidationResult entryResult = validate(entry.key, entry.value);
        if (!entryResult.isValid) {
            result.isValid = false;
            result.errors.insert(result.errors.end(), entryResult.errors.begin(), entryResult.errors.end());
        }
        result.warnings.insert(result.warnings.end(), entryResult.warnings.begin(), entryResult.warnings.end());
    }
    
    return result;
}

std::string ConfigSchema::getHelpText() const {
    std::ostringstream oss;
    oss << "Available configuration parameters:\n";
    for (const auto& param : pImpl->parameters) {
        oss << "  " << param.key;
        if (!param.type.empty()) {
            oss << " (" << param.type << ")";
        }
        if (!param.unit.empty()) {
            oss << " [" << param.unit << "]";
        }
        oss << "\n";
        oss << "    " << param.description << "\n";
        if (param.required) {
            oss << "    [Required]" << std::endl;
        }
        oss << "\n";
    }
    return oss.str();
}

std::string ConfigSchema::getParameterSummary(const std::string& key) const {
    const ConfigEntry* param = getParameter(key);
    if (!param) {
        return "Unknown parameter: " + key;
    }
    
    std::ostringstream oss;
    oss << "Parameter: " << param->key << std::endl;
    oss << "Description: " << param->description << std::endl;
    if (!param->type.empty()) {
        oss << "Type: " << param->type << std::endl;
    }
    if (!param->unit.empty()) {
        oss << "Unit: " << param->unit << std::endl;
    }
    oss << "Source: " << static_cast<int>(param->source) << std::endl;
    oss << "Required: " << (param->required ? "Yes" : "No") << std::endl;
    return oss.str();
}

bool ConfigSchema::hasParameter(const std::string& key) const {
    return getParameter(key) != nullptr;
}

std::optional<ConfigValue> ConfigSchema::getDefaultValue(const std::string& key) const {
    // Note: This is a placeholder - in a full implementation, we would store default values
    return std::nullopt;
}

std::string ConfigSchema::getValueTypeName(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            oss << "int";
        } else if constexpr (std::is_same_v<T, int64_t>) {
            oss << "int64_t";
        } else if constexpr (std::is_same_v<T, double>) {
            oss << "double";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << "bool";
        } else if constexpr (std::is_same_v<T, std::string>) {
            oss << "string";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "std::vector<int>";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "std::vector<double>";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "std::vector<std::string>";
        } else {
            oss << "unknown";
        }
    }, value);
    return oss.str();
}

// Enhanced Config implementation
struct Config::Impl {
    std::vector<ConfigEntry> entries;
    ValidationResult validationResult;
};

Config::Config() : pImpl(std::make_unique<Impl>()), schema(nullptr) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::setSchema(std::unique_ptr<ConfigSchema> schema) {
    this->schema = std::move(schema);
}

bool Config::loadFromFile(const std::string& filepath) {
    // TODO: Implement proper JSON/YAML parser
    // For now, we'll use the simple key=value format
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
            
            if (value.size() >= 2 && ((value.front() == '"' && value.back() == '"') ||
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
        } else if (arg[0] == '-' && i + 1 < argc) {
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
        file << entry.key << " = " << "[" << getValueAsString(entry.value) << "]" << std::endl;
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
        ConfigEntry entry;
        entry.key = key;
        entry.value = value;
        entry.source = source;
        pImpl->entries.push_back(entry);
    }
    
    // Re-validate configuration
    if (schema) {
        pImpl->validationResult = schema->validateAll(pImpl->entries);
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
    
    // Re-validate configuration
    if (schema) {
        pImpl->validationResult = schema->validateAll(pImpl->entries);
    }
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
    if (schema) {
        pImpl->validationResult = ValidationResult();
    }
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [" << getValueAsString(entry.value) << "] (" 
            << static_cast<int>(entry.source) << ")\n";
    }
    
    if (schema && !pImpl->validationResult.isValid) {
        oss << "\nValidation Errors:";
        for (const auto& error : pImpl->validationResult.errors) {
            oss << "\n  " << error;
        }
    }
    
    return oss.str();
}

ValidationResult Config::validate() const {
    if (schema) {
        return schema->validateAll(pImpl->entries);
    }
    return ValidationResult(true); // No schema means validation passes
}

std::string Config::getHelpText() const {
    if (schema) {
        return schema->getHelpText();
    }
    return "No configuration schema available.\n";
}

bool Config::isValid() const {
    return validate().isValid;
}

std::vector<std::string> Config::getValidationErrors() const {
    return validate().errors;
}

std::vector<std::string> Config::getValidationWarnings() const {
    return validate().warnings;
}

std::string Config::getValueAsString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            oss << "["; bool first = true;
            for (const auto& item : arg) {
                if (!first) oss << ", ";
                oss << item;
                first = false;
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            oss << "["; bool first = true;
            for (const auto& item : arg) {
                if (!first) oss << ", ";
                oss << item;
                first = false;
            }
            oss << "]";
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "["; bool first = true;
            for (const auto& item : arg) {
                if (!first) oss << ", ";
                oss << "\"" << item << "\"";
                first = false;
            }
            oss << "]";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

ValidationResult Config::validateValue(const std::string& key, const ConfigValue& value) const {
    return ValidationResult(true); // Placeholder
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