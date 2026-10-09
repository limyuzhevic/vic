#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
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

// Configuration validation implementation
ConfigValidationResult Config::validate() const {
    ConfigValidationResult result;
    
    for (const auto& entry : pImpl->entries) {
        // Check if required field is set
        if (entry.required && entry.value.index() == static_cast<size_t>(ConfigValueIndex::Null)) {
            result.addError("Required configuration key \"" + entry.key + "\" is not set");
            continue;
        }
        
        // Validate value against custom validator if present
        auto metaIt = configMetadata.find(entry.key);
        if (metaIt != configMetadata.end()) {
            const auto& meta = metaIt->second;
            if (!meta.validator(entry.value)) {
                result.addError("Validation failed for configuration key \"" + entry.key + "\": invalid value");
            }
        }
        
        // Perform basic type validation
        if (!validateValue(entry)) {
            result.addError("Invalid value type for configuration key \"" + entry.key + "\"");
        }
    }
    
    return result;
}

ConfigValidationResult Config::validate(const std::map<std::string, ConfigValidator>& customValidators) const {
    ConfigValidationResult result = validate();
    
    // Apply custom validators
    for (const auto& [key, validator] : customValidators) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            if (!validator(it->value)) {
                result.addError("Custom validation failed for configuration key \"" + key + "\"");
            }
        }
    }
    
    return result;
}

void Config::setRequired(const std::string& key, bool required) {
    configMetadata[key].required = required;
}

void Config::setValidator(const std::string& key, const ConfigValidator& validator) {
    configMetadata[key].validator = validator;
}

void Config::setDescription(const std::string& key, const std::string& description) {
    configMetadata[key].description = description;
}

std::string Config::getDescription(const std::string& key) const {
    auto it = configMetadata.find(key);
    if (it != configMetadata.end()) {
        return it->second.description;
    }
    return "";
}

// Helper function implementations
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

bool Config::isNumber(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

std::string Config::parseValueType(const std::string& value) {
    if (value.empty()) return "string";
    
    if (value[0] == '"' && value.back() == '"') {
        return "string";
    }
    
    if (value == "true" || value == "false") {
        return "bool";
    }
    
    if (isNumber(value)) {
        return "int";
    }
    
    try {
        std::stod(value);
        return "float";
    } catch (...) {
        return "string";
    }
}

ConfigValue Config::parseValue(const std::string& value, const std::string& typeHint) {
    if (value.empty()) {
        return "";
    }
    
    // Parse based on type hint
    if (typeHint == "int" || typeHint == "bool") {
        // Try to parse as int
        try {
            return std::stoi(value);
        } catch (...) {}
    }
    
    if (typeHint == "float") {
        // Try to parse as double
        try {
            return std::stod(value);
        } catch (...) {}
    }
    
    if (value == "true") {
        return true;
    } else if (value == "false") {
        return false;
    }
    
    // Try to parse as int
    try {
        return std::stoi(value);
    } catch (...) {}
    
    // Try to parse as float
    try {
        return std::stod(value);
    } catch (...) {}
    
    // Return as string
    return value;
}

// Validation helper implementations
ConfigValidationResult Config::validateSingleEntry(const ConfigEntry& entry) const {
    ConfigValidationResult result;
    
    if (!validateTypeMatch(entry, getTypeName(entry.value))) {
        result.addError("Type mismatch for configuration key \"" + entry.key + "\"");
    }
    
    if (!validateValue(entry)) {
        result.addError("Invalid value for configuration key \"" + entry.key + "\"");
    }
    
    return result;
}

bool Config::validateTypeMatch(const ConfigEntry& entry, const std::string& expectedType) const {
    // Basic type matching - this can be extended with more sophisticated validation
    auto metaIt = configMetadata.find(entry.key);
    if (metaIt != configMetadata.end()) {
        const auto& meta = metaIt->second;
        if (!meta.typeName.empty() && meta.typeName != expectedType) {
            return false;
        }
    }
    return true;
}

bool Config::validateValue(const ConfigEntry& entry) const {
    // Basic validation - check if value is set and valid
    if (entry.required && entry.value.index() == static_cast<size_t>(ConfigValueIndex::Null)) {
        return false;
    }
    
    // Additional validation can be added here
    return true;
}

std::string Config::getTypeName(const ConfigValue& value) {
    if (const auto* v = std::get_if<int>(&value)) {
        return "int";
    } else if (const auto* v = std::get_if<int64_t>(&value)) {
        return "int64_t";
    } else if (const auto* v = std::get_if<double>(&value)) {
        return "double";
    } else if (const auto* v = std::get_if<bool>(&value)) {
        return "bool";
    } else if (const auto* v = std::get_if<std::string>(&value)) {
        return "string";
    } else if (const auto* v = std::get_if<std::vector<int>>(&value)) {
        return "vector<int>";
    } else if (const auto* v = std::get_if<std::vector<double>>(&value)) {
        return "vector<double>";
    } else if (const auto* v = std::get_if<std::vector<std::string>>(&value)) {
        return "vector<string>";
    }
    
    return "unknown";
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