#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <regex>
#include <iomanip>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
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
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
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
// Config::get<int> template instantiation

template std::optional<int> Config::get<int>(const std::string&) const;

// Config::get<int64_t> template instantiation
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;

// Config::get<double> template instantiation
template std::optional<double> Config::get<double>(const std::string&) const;

// Config::get<bool> template instantiation
template std::optional<bool> Config::get<bool>(const std::string&) const;

// Config::get<std::string> template instantiation
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// Config::getOr<int> template instantiation
template int Config::getOr<int>(const std::string&, const int&) const;

// Config::getOr<int64_t> template instantiation
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;

// Config::getOr<double> template instantiation
template double Config::getOr<double>(const std::string&, const double&) const;

// Config::getOr<bool> template instantiation
template bool Config::getOr<bool>(const std::string&, const bool&) const;

// Config::getOr<std::string> template instantiation
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

// ConfigSchema implementation
namespace nlm {

struct ConfigSchema::Impl {
    std::vector<ConfigSchemaEntry> entries;
};

ConfigSchema::ConfigSchema() : pImpl(std::make_unique<Impl>()) {}

ConfigSchema::~ConfigSchema() = default;

void ConfigSchema::addEntry(const ConfigSchemaEntry& entry) {
    entries_.push_back(entry);
}

void ConfigSchema::validate(const Config& config) const {
    std::vector<std::string> errors;
    
    // Check for missing required entries
    for (const auto& entry : entries_) {
        if (entry.required && !config.has(entry.key)) {
            errors.push_back("Missing required configuration key: '" + entry.key + "'");
        }
    }
    
    // Validate all configured entries
    for (const auto& schemaEntry : entries_) {
        if (config.has(schemaEntry.key)) {
            const auto& configEntry = findConfigEntry(config, schemaEntry.key);
            if (!configEntry) {
                errors.push_back("Configuration entry not found: '" + schemaEntry.key + "'");
                continue;
            }
            
            try {
                validateValueAgainstSchema(*configEntry, schemaEntry);
            } catch (const ConfigValidationException& e) {
                errors.push_back(std::string("Invalid configuration for key '" + 
                                           schemaEntry.key + "': " + e.what()));
            }
        }
    }
    
    // Report all validation errors
    if (!errors.empty()) {
        std::ostringstream oss;
        oss << "Configuration validation failed with " << errors.size() << " error(s):\n";
        for (const auto& error : errors) {
            oss << "  - " << error << "\n";
        }
        throw ConfigValidationException(oss.str());
    }
}

bool ConfigSchema::hasKey(const std::string& key) const {
    return std::any_of(entries_.begin(), entries_.end(),
        [&key](const ConfigSchemaEntry& e) { return e.key == key; });
}

const ConfigSchemaEntry* ConfigSchema::getEntry(const std::string& key) const {
    auto it = std::find_if(entries_.begin(), entries_.end(),
        [&key](const ConfigSchemaEntry& e) { return e.key == key; });
    return (it != entries_.end()) ? &(*it) : nullptr;
}

void ConfigSchema::applyDefaults(Config& config) const {
    for (const auto& entry : entries_) {
        if (!config.has(entry.key)) {
            // Apply default value based on type
            ConfigValue defaultValue;
            
            if (entry.type == std::type_index(typeid(int))) {
                defaultValue = std::stoi(entry.defaultValue);
            } else if (entry.type == std::type_index(typeid(int64_t))) {
                defaultValue = std::stoll(entry.defaultValue);
            } else if (entry.type == std::type_index(typeid(double))) {
                defaultValue = std::stod(entry.defaultValue);
            } else if (entry.type == std::type_index(typeid(bool))) {
                defaultValue = (entry.defaultValue == "true" || entry.defaultValue == "1");
            } else if (entry.type == std::type_index(typeid(std::string))) {
                defaultValue = entry.defaultValue;
            } else if (entry.type == std::type_index(typeid(std::vector<int>))) {
                std::vector<int> vec;
                std::stringstream ss(entry.defaultValue);
                std::string token;
                while (std::getline(ss, token, ',')) {
                    try {
                        vec.push_back(std::stoi(token));
                    } catch (...) {
                        vec.push_back(0); // Default to 0 on parse error
                    }
                }
                defaultValue = vec;
            } else if (entry.type == std::type_index(typeid(std::vector<double>))) {
                std::vector<double> vec;
                std::stringstream ss(entry.defaultValue);
                std::string token;
                while (std::getline(ss, token, ',')) {
                    try {
                        vec.push_back(std::stod(token));
                    } catch (...) {
                        vec.push_back(0.0); // Default to 0.0 on parse error
                    }
                }
                defaultValue = vec;
            } else if (entry.type == std::type_index(typeid(std::vector<std::string>))) {
                std::vector<std::string> vec;
                std::stringstream ss(entry.defaultValue);
                std::string token;
                while (std::getline(ss, token, ',')) {
                    vec.push_back(token);
                }
                defaultValue = vec;
            }
            
            if (!std::holds_alternative<std::monostate>(defaultValue)) {
                config.set(entry.key, defaultValue, entry.defaultSource);
            }
        }
    }
}

std::optional<ConfigValue> ConfigSchema::getDefault(const std::string& key) const {
    auto it = std::find_if(entries_.begin(), entries_.end(),
        [&key](const ConfigSchemaEntry& e) { return e.key == key; });
    if (it == entries_.end()) {
        return std::nullopt;
    }
    
    // Convert default string back to ConfigValue based on type
    ConfigValue defaultValue;
    if (it->type == std::type_index(typeid(int))) {
        defaultValue = std::stoi(it->defaultValue);
    } else if (it->type == std::type_index(typeid(int64_t))) {
        defaultValue = std::stoll(it->defaultValue);
    } else if (it->type == std::type_index(typeid(double))) {
        defaultValue = std::stod(it->defaultValue);
    } else if (it->type == std::type_index(typeid(bool))) {
        defaultValue = (it->defaultValue == "true" || it->defaultValue == "1");
    } else if (it->type == std::type_index(typeid(std::string))) {
        defaultValue = it->defaultValue;
    }
    
    return defaultValue;
}

ConfigSource ConfigSchema::getDefaultSource(const std::string& key) const {
    auto it = std::find_if(entries_.begin(), entries_.end(),
        [&key](const ConfigSchemaEntry& e) { return e.key == key; });
    return (it != entries_.end()) ? it->defaultSource : ConfigSource::Default;
}

std::string ConfigSchema::getDescription(const std::string& key) const {
    auto it = std::find_if(entries_.begin(), entries_.end(),
        [&key](const ConfigSchemaEntry& e) { return e.key == key; });
    return (it != entries_.end()) ? it->description : "";
}

void ConfigSchema::validateValueAgainstSchema(const ConfigEntry& entry, const ConfigSchemaEntry& schema) const {
    // Check type compatibility
    if (!isTypeCompatible(entry.value, schema.type)) {
        std::string actualType = getTypeName(entry.value);
        std::string expectedType = getTypeNameFromTypeIndex(schema.type);
        throw ConfigValidationException(
            "Type mismatch for key '" + entry.key + "': expected " + expectedType + 
            ", got " + actualType, entry.key, expectedType, "");
    }
    
    // Check value range
    if (!isValueInRange(entry.value, schema.minValue, schema.maxValue)) {
        std::ostringstream oss;
        oss << "Value out of range for key '" << entry.key << "'";
        if (schema.minValue.has_value()) {
            oss << ". Minimum value: " << schema.minValue.value();
        }
        if (schema.maxValue.has_value()) {
            oss << ". Maximum value: " << schema.maxValue.value();
        }
        throw ConfigValidationException(oss.str(), entry.key, "", "");
    }
    
    // Apply custom validator if provided
    if (schema.customValidator) {
        if (!schema.customValidator(entry.value)) {
            throw ConfigValidationException(
                "Custom validation failed for key '" + entry.key + "'", 
                entry.key, "", "");
        }
    }
}

bool ConfigSchema::isValueInRange(const ConfigValue& value, const std::optional<double>& min, 
                                 const std::optional<double>& max) const {
    // Only check numeric ranges for numeric types
    if (min.has_value() && max.has_value()) {
        if (value.index() == typeid(double)) {
            double val = std::get<double>(value);
            return val >= min.value() && val <= max.value();
        } else if (value.index() == typeid(int64_t)) {
            double val = static_cast<double>(std::get<int64_t>(value));
            return val >= min.value() && val <= max.value();
        } else if (value.index() == typeid(int)) {
            double val = static_cast<double>(std::get<int>(value));
            return val >= min.value() && val <= max.value();
        }
    } else if (min.has_value()) {
        if (value.index() == typeid(double)) {
            double val = std::get<double>(value);
            return val >= min.value();
        } else if (value.index() == typeid(int64_t)) {
            double val = static_cast<double>(std::get<int64_t>(value));
            return val >= min.value();
        } else if (value.index() == typeid(int)) {
            double val = static_cast<double>(std::get<int>(value));
            return val >= min.value();
        }
    } else if (max.has_value()) {
        if (value.index() == typeid(double)) {
            double val = std::get<double>(value);
            return val <= max.value();
        } else if (value.index() == typeid(int64_t)) {
            double val = static_cast<double>(std::get<int64_t>(value));
            return val <= max.value();
        } else if (value.index() == typeid(int)) {
            double val = static_cast<double>(std::get<int>(value));
            return val <= max.value();
        }
    }
    
    return true; // No range constraints or non-numeric type
}

bool ConfigSchema::isTypeCompatible(const ConfigValue& value, const std::type_index& expectedType) const {
    // Map type_index to actual types
    if (expectedType == std::type_index(typeid(int))) {
        return (value.index() == typeid(int) || value.index() == typeid(int64_t) || 
                (value.index() == typeid(double) && std::floor(std::get<double>(value)) == std::get<double>(value)));
    } else if (expectedType == std::type_index(typeid(int64_t))) {
        return (value.index() == typeid(int64_t) || value.index() == typeid(int) ||
                (value.index() == typeid(double) && std::floor(std::get<double>(value)) == std::get<double>(value)));
    } else if (expectedType == std::type_index(typeid(double))) {
        return (value.index() == typeid(double) || value.index() == typeid(int) || value.index() == typeid(int64_t));
    } else if (expectedType == std::type_index(typeid(bool))) {
        return value.index() == typeid(bool);
    } else if (expectedType == std::type_index(typeid(std::string))) {
        return value.index() == typeid(std::string);
    } else if (expectedType == std::type_index(typeid(std::vector<int>))) {
        return value.index() == typeid(std::vector<int>);
    } else if (expectedType == std::type_index(typeid(std::vector<double>))) {
        return value.index() == typeid(std::vector<double>);
    } else if (expectedType == std::type_index(typeid(std::vector<std::string>))) {
        return value.index() == typeid(std::vector<std::string>);
    }
    
    return false;
}

std::string ConfigSchema::getTypeName(const ConfigValue& value) const {
    if (value.index() == typeid(int)) return "int";
    if (value.index() == typeid(int64_t)) return "int64_t";
    if (value.index() == typeid(double)) return "double";
    if (value.index() == typeid(bool)) return "bool";
    if (value.index() == typeid(std::string)) return "string";
    if (value.index() == typeid(std::vector<int>)) return "vector<int>";
    if (value.index() == typeid(std::vector<double>)) return "vector<double>";
    if (value.index() == typeid(std::vector<std::string>)) return "vector<string>";
    return "unknown";
}

std::string ConfigSchema::getTypeNameFromTypeIndex(const std::type_index& typeIndex) const {
    if (typeIndex == std::type_index(typeid(int))) return "int";
    if (typeIndex == std::type_index(typeid(int64_t))) return "int64_t";
    if (typeIndex == std::type_index(typeid(double))) return "double";
    if (typeIndex == std::type_index(typeid(bool))) return "bool";
    if (typeIndex == std::type_index(typeid(std::string))) return "string";
    if (typeIndex == std::type_index(typeid(std::vector<int>))) return "vector<int>";
    if (typeIndex == std::type_index(typeid(std::vector<double>))) return "vector<double>";
    if (typeIndex == std::type_index(typeid(std::vector<std::string>))) return "vector<string>";
    return "unknown";
}

// Helper function to access ConfigEntry from Config
const ConfigEntry* findConfigEntry(const Config& config, const std::string& key) {
    // This is a helper function that would need to access Config's private implementation
    // In a real implementation, we would need to either:
    // 1. Make findConfigEntry a friend of Config
    // 2. Expose a public API to access ConfigEntry
    // 3. Use a different approach
    //
    // For now, this is a placeholder implementation that returns nullptr.
    // The actual implementation would need to access Config::pImpl->entries
    return nullptr;
}

} // namespace nlm