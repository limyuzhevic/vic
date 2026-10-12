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
    std::vector<ConfigSchema> schemas;
    std::map<ConfigSource, size_t> sourceCounts;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->sourceCounts[ConfigSource::Default] = 0;
    pImpl->sourceCounts[ConfigSource::File] = 0;
    pImpl->sourceCounts[ConfigSource::CommandLine] = 0;
    pImpl->sourceCounts[ConfigSource::Runtime] = 0;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::loadFromFile(const std::string& filepath) {
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        throw ConfigFileError(filepath, "File does not exist");
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigIOError(filepath, "open", "Failed to open file");
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
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
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const ConfigParseError& e) {
                throw ConfigParseError(key, value, "Line " + std::to_string(lineNum) + " - " + e.what());
            }
        } else {
            throw ConfigParseError("", line, "Invalid format - missing '='");
        }
    }
}

void Config::loadFromArgs(int argc, char** argv) {
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
}

void Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigIOError(filepath, "save", "Failed to open file for writing");
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        std::visit([&file](const auto& val) {
            file << val;
        }, entry.value);
        file << "\n\n";
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    validateKey(key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        throw ConfigParseError(key, "", "Type mismatch - expected " + typeid(T).name() + ".");
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    validateKeyUniqueness(key);
    validateValueType(key, value);
    validateValue(key, value);
    validateAgainstSchema(key, value);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
    
    // Update source counts
    pImpl->sourceCounts[source]++;
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
    validateKey(key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Update source count before removal
        pImpl->sourceCounts[it->source]--;
        pImpl->entries.erase(it);
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
    // Reset source counts
    for (auto& pair : pImpl->sourceCounts) {
        pair.second = 0;
    }
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
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    oss << "\nSchema Summary (" << pImpl->schemas.size() << " schemas):\n";
    for (const auto& schema : pImpl->schemas) {
        oss << "  " << schema.key << " : " << schema.type << " (required: " << (schema.required ? "yes" : "no") << ") - " << schema.description << "\n";
    }
    
    return oss.str();
}

bool Config::validateKey(const std::string& key) const {
    if (key.empty()) {
        throw ConfigValidationError(key, "Key cannot be empty");
    }
    if (key.find('=') != std::string::npos) {
        throw ConfigValidationError(key, "Key cannot contain '=' character");
    }
    if (std::any_of(key.begin(), key.end(), ::isspace)) {
        throw ConfigValidationError(key, "Key cannot contain whitespace");
    }
    return true;
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Basic value validation
    std::visit([this, &key](const auto& val) {
        using T = std::decay_t<decltype(val)>;
        if constexpr (std::is_same_v<T, std::string>) {
            // String validation
            if (val.empty()) {
                throw ConfigValidationError(key, "String value cannot be empty");
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            // Boolean validation - always valid
        } else if constexpr (std::is_integral_v<T>) {
            // Integer validation
            if (val < std::numeric_limits<T>::min()) {
                throw ConfigValidationError(key, "Value " + std::to_string(val) + " below minimum");
            }
        } else if constexpr (std::is_floating_point_v<T>) {
            // Float validation
            if (std::isnan(val) || std::isinf(val)) {
                throw ConfigValidationError(key, "Value " + std::to_string(val) + " is not a valid number");
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                             std::is_same_v<T, std::vector<double>> ||
                             std::is_same_v<T, std::vector<std::string>>) {
            // Vector validation
            if (val.empty()) {
                throw ConfigValidationError(key, "Vector value cannot be empty");
            }
        }
    }, value);
    
    return true;
}

bool Config::validateAgainstSchema(const std::string& key, const ConfigValue& value) const {
    // Find schema for this key
    auto schemaIt = std::find_if(pImpl->schemas.begin(), pImpl->schemas.end(),
        [&key](const ConfigSchema& s) { return s.key == key; });
    
    if (schemaIt == pImpl->schemas.end()) {
        // No schema found for this key - if required, throw error
        auto entryIt = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        if (entryIt != pImpl->entries.end() && entryIt->source == ConfigSource::Runtime) {
            // Runtime entries are always allowed even without schema
            return true;
        }
        return false;
    }
    
    const auto& schema = *schemaIt;
    
    // Check if value matches schema type
    ConfigType expectedType = getConfigType(key);
    
    if (expectedType == ConfigType::UNKNOWN && schema.required) {
        throw ConfigValidationError(key, "Schema requires key '" + key + "' but type is unknown");
    }
    
    if (expectedType != schema.type) {
        throw ConfigValidationError(key, "Type mismatch: expected '" + schema.type + "', got " + configValueToString(value));
    }
    
    return true;
}

void Config::validateSchema() const {
    // Validate all schemas
    for (const auto& schema : pImpl->schemas) {
        validateSchema(schema);
    }
}

void Config::addSchema(const ConfigSchema& schema) {
    validateSchema(schema);
    
    // Check if schema already exists
    auto it = std::find_if(pImpl->schemas.begin(), pImpl->schemas.end(),
        [&schema](const ConfigSchema& s) { return s.key == schema.key; });
    
    if (it != pImpl->schemas.end()) {
        throw ConfigValidationError(schema.key, "Schema already exists for key '" + schema.key + "'");
    }
    
    pImpl->schemas.push_back(schema);
}

void Config::removeSchema(const std::string& key) {
    auto it = std::find_if(pImpl->schemas.begin(), pImpl->schemas.end(),
        [&key](const ConfigSchema& s) { return s.key == key; });
    
    if (it != pImpl->schemas.end()) {
        pImpl->schemas.erase(it);
    }
}

void Config::clearSchemas() {
    pImpl->schemas.clear();
}

bool Config::hasSchema(const std::string& key) const {
    return std::any_of(pImpl->schemas.begin(), pImpl->schemas.end(),
        [&key](const ConfigSchema& s) { return s.key == key; });
}

std::vector<ConfigSchema> Config::getSchemas() const {
    return pImpl->schemas;
}

std::string Config::getSchemaSummary() const {
    std::ostringstream oss;
    oss << "Configuration Schema Summary (" << pImpl->schemas.size() << " schemas):\n";
    for (const auto& schema : pImpl->schemas) {
        oss << "  Key: " << schema.key << "\n";
        oss << "    Type: " << schema.type << "\n";
        oss << "    Description: " << schema.description << "\n";
        oss << "    Required: " << (schema.required ? "yes" : "no") << "\n";
        oss << "    Default: " << schema.defaultValue << "\n";
        oss << "\n";
    }
    return oss.str();
}

Config::ConfigType Config::getConfigType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ConfigType::UNKNOWN;
    }
    
    const auto& value = it->value;
    
    if (std::holds_alternative<int>(value)) {
        return ConfigType::INTEGER;
    } else if (std::holds_alternative<int64_t>(value)) {
        return ConfigType::INTEGER;
    } else if (std::holds_alternative<double>(value)) {
        return ConfigType::FLOAT;
    } else if (std::holds_alternative<bool>(value)) {
        return ConfigType::BOOLEAN;
    } else if (std::holds_alternative<std::string>(value)) {
        return ConfigType::STRING;
    } else if (std::holds_alternative<std::vector<int>>(value)) {
        return ConfigType::INT_ARRAY;
    } else if (std::holds_alternative<std::vector<double>>(value)) {
        return ConfigType::FLOAT_ARRAY;
    } else if (std::holds_alternative<std::vector<std::string>>(value)) {
        return ConfigType::STRING_ARRAY;
    }
    
    return ConfigType::UNKNOWN;
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getEntryCount(ConfigSource source) const {
    return pImpl->sourceCounts[source];
}

std::map<ConfigSource, size_t> Config::getSourceCounts() const {
    return pImpl->sourceCounts;
}

void Config::validateSchema(const ConfigSchema& schema) const {
    if (schema.key.empty()) {
        throw ConfigValidationError(schema.key, "Schema key cannot be empty");
    }
    
    if (schema.type.empty()) {
        throw ConfigValidationError(schema.key, "Schema type cannot be empty");
    }
    
    // Validate type string
    const std::vector<std::string> validTypes = {"int", "float", "string", "bool", "array", "object"};
    if (std::find(validTypes.begin(), validTypes.end(), schema.type) == validTypes.end()) {
        throw ConfigValidationError(schema.key, "Invalid schema type: '" + schema.type + "'. Valid types: " + 
            "int, float, string, bool, array, object");
    }
}

std::string Config::configValueToString(const ConfigValue& value) const {
    std::ostringstream oss;
    std::visit([&oss](const auto& val) {
        oss << val;
    }, value);
    return oss.str();
}

bool Config::isNumeric(const std::string& str) {
    // Check if string represents a number (int or float)
    if (str.empty()) return false;
    
    char* end;
    std::strtod(str.c_str(), &end);
    
    // Check if entire string was parsed as a number
    return (*end == '\0');
}

bool Config::isBoolean(const std::string& str) {
    // Check if string represents a boolean
    std::string lower = toLower(str);
    return (lower == "true" || lower == "false" || lower == "1" || lower == "0");
}

Config::ConfigType Config::inferType(const std::string& value) {
    if (isBoolean(value)) {
        return ConfigType::BOOLEAN;
    } else if (isNumeric(value)) {
        // Check if it's an integer
        try {
            size_t pos;
            long int_val = std::stol(value, &pos);
            if (pos == value.size()) {
                return ConfigType::INTEGER;
            }
        } catch (...) {
            // Fall through to float
        }
        
        try {
            size_t pos;
            double float_val = std::stod(value, &pos);
            if (pos == value.size()) {
                return ConfigType::FLOAT;
            }
        } catch (...) {
            // Fall through to string
        }
    }
    
    return ConfigType::STRING;
}

void Config::validateKeyUniqueness(const std::string& key) const {
    if (has(key)) {
        throw ConfigValidationError(key, "Key already exists in configuration");
    }
}

void Config::validateValueType(const std::string& key, const ConfigValue& value) const {
    // This is a placeholder for type-specific validation
    // In a real implementation, you might want to validate specific type combinations
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
