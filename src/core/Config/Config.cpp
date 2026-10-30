#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <limits>
#include <regex>

namespace nlm {

// Implementation of ConfigValidator
namespace {
    
    // Helper function to convert ConfigValue to string for comparison
    std::string configValueToString(const ConfigValue& value) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, double>) {
                return std::to_string(arg);
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg ? "true" : "false";
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                std::is_same_v<T, std::vector<double>> ||
                                std::is_same_v<T, std::vector<std::string>>) {
                // For vectors, join elements with commas
                std::ostringstream oss;
                oss << "[";
                bool first = true;
                std::visit([&](const auto& vec) {
                    for (const auto& item : vec) {
                        if (!first) oss << ",";
                        oss << item;
                        first = false;
                    }
                }, value);
                oss << "]";
                return oss.str();
            }
            return std::string();
        }, value);
        return "";
    }
    
    // Helper function to check if a string matches a pattern
    bool stringMatchesPattern(const std::string& str, const std::string& pattern) {
        if (pattern.empty()) return true;
        
        // Convert glob pattern to regex
        std::string regexPattern = std::regex_escape(pattern);
        regexPattern = std::regex_replace(regexPattern, std::regex(R"(\*)"), ".*");
        regexPattern = std::regex_replace(regexPattern, std::regex(R"(\?)"), ".");
        
        try {
            std::regex regex(regexPattern);
            return std::regex_match(str, regex);
        } catch (...) {
            return str == pattern; // Fallback to exact match
        }
    }
    
    // Helper function to convert string to lowercase
    std::string toLowercase(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    // Helper function to parse type string
    std::string normalizeType(const std::string& type) {
        std::string normalized = toLowercase(type);
        if (normalized == "int" || normalized == "integer" || normalized == "number") {
            return "int";
        } else if (normalized == "float" || normalized == "double" || normalized == "real") {
            return "double";
        } else if (normalized == "bool" || normalized == "boolean") {
            return "bool";
        } else if (normalized == "string" || normalized == "text") {
            return "string";
        } else if (normalized == "vector" || normalized == "array") {
            return "vector";
        }
        return type; // Return original if unknown
    }
    
    // Helper function to check if a value is within range constraints
    bool checkRangeConstraint(const ConfigValue& value, const std::string& type, const std::map<std::string, std::string>& constraints) {
        auto itMin = constraints.find("min");
        auto itMax = constraints.find("max");
        auto itMinInc = constraints.find("min_including");
        auto itMaxInc = constraints.find("max_including");
        
        std::function<double(const ConfigValue&)> toDouble = [](const ConfigValue& val) -> double {
            return std::visit([](auto&& arg) -> double {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int64_t>) {
                    return static_cast<double>(arg);
                } else if constexpr (std::is_same_v<T, double>) {
                    return arg;
                } else if constexpr (std::is_same_v<T, bool>) {
                    return arg ? 1.0 : 0.0;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    try {
                        return std::stod(arg);
                    } catch (...) {
                        return std::numeric_limits<double>::quiet_NaN();
                    }
                }
                return std::numeric_limits<double>::quiet_NaN();
            }, val);
        };
        
        double valDouble = toDouble(value);
        if (std::isnan(valDouble)) return false; // Can't convert to number
        
        // Check minimum constraints
        double minVal = std::numeric_limits<double>::lowest();
        if (itMin != constraints.end()) {
            try {
                minVal = std::stod(itMin->second);
            } catch (...) { return false; }
        }
        if (itMinInc != constraints.end()) {
            try {
                minVal = std::stod(itMinInc->second);
            } catch (...) { return false; }
        }
        
        // Check maximum constraints
        double maxVal = std::numeric_limits<double>::max();
        if (itMax != constraints.end()) {
            try {
                maxVal = std::stod(itMax->second);
            } catch (...) { return false; }
        }
        if (itMaxInc != constraints.end()) {
            try {
                maxVal = std::stod(itMaxInc->second);
            } catch (...) { return false; }
        }
        
        // Handle inclusive/exclusive ranges
        bool minInclusive = (itMinInc != constraints.end());
        bool maxInclusive = (itMaxInc != constraints.end());
        
        if (!minInclusive && valDouble <= minVal) return false;
        if (minInclusive && valDouble < minVal) return false;
        if (!maxInclusive && valDouble >= maxVal) return false;
        if (maxInclusive && valDouble > maxVal) return false;
        
        return true;
    }
    
    // Helper function to check if a string matches a pattern constraint
    bool checkPatternConstraint(const std::string& strValue, const std::map<std::string, std::string>& constraints) {
        auto itPattern = constraints.find("pattern");
        if (itPattern == constraints.end()) return true;
        
        return stringMatchesPattern(strValue, itPattern->second);
    }
    
    // Helper function to check if a vector has length constraints
    bool checkVectorConstraint(const ConfigValue& value, const std::map<std::string, std::string>& constraints) {
        if (!std::holds_alternative<std::vector<int>>(value) && 
            !std::holds_alternative<std::vector<double>>(value) &&
            !std::holds_alternative<std::vector<std::string>>(value)) {
            return false; // Not a vector type
        }
        
        auto itMinLen = constraints.find("min_length");
        auto itMaxLen = constraints.find("max_length");
        auto itMinLenInc = constraints.find("min_length_including");
        auto itMaxLenInc = constraints.find("max_length_including");
        
        size_t length = 0;
        std::visit([&](const auto& vec) {
            length = vec.size();
        }, value);
        
        // Check minimum length constraints
        size_t minLen = 0;
        if (itMinLen != constraints.end()) {
            try {
                minLen = static_cast<size_t>(std::stoul(itMinLen->second));
            } catch (...) { return false; }
        }
        if (itMinLenInc != constraints.end()) {
            try {
                minLen = static_cast<size_t>(std::stoul(itMinLenInc->second));
            } catch (...) { return false; }
        }
        
        // Check maximum length constraints
        size_t maxLen = std::numeric_limits<size_t>::max();
        if (itMaxLen != constraints.end()) {
            try {
                maxLen = static_cast<size_t>(std::stoul(itMaxLen->second));
            } catch (...) { return false; }
        }
        if (itMaxLenInc != constraints.end()) {
            try {
                maxLen = static_cast<size_t>(std::stoul(itMaxLenInc->second));
            } catch (...) { return false; }
        }
        
        // Handle inclusive/exclusive ranges
        bool minInclusive = (itMinLenInc != constraints.end());
        bool maxInclusive = (itMaxLenInc != constraints.end());
        
        if (!minInclusive && length <= minLen) return false;
        if (minInclusive && length < minLen) return false;
        if (!maxInclusive && length >= maxLen) return false;
        if (maxInclusive && length > maxLen) return false;
        
        return true;
    }
    
    // Helper function to check if a value matches a specific value constraint
    bool checkEnumConstraint(const ConfigValue& value, const std::map<std::string, std::string>& constraints) {
        auto itEnum = constraints.find("enum");
        if (itEnum == constraints.end()) return true;
        
        std::string enumStr = itEnum->second;
        std::vector<std::string> enumValues;
        size_t start = 0;
        size_t end = enumStr.find(',');
        while (end != std::string::npos) {
            enumValues.push_back(enumStr.substr(start, end - start));
            start = end + 1;
            end = enumStr.find(',', start);
        }
        enumValues.push_back(enumStr.substr(start));
        
        // Convert current value to string for comparison
        std::string currentStr;
        bool found = false;
        
        if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value)) {
            currentStr = std::to_string(std::visit([](auto&& arg) -> int64_t { return arg; }, value));
            for (const auto& enumVal : enumValues) {
                if (enumVal == currentStr) {
                    found = true;
                    break;
                }
            }
        } else if (std::holds_alternative<double>(value)) {
            currentStr = std::to_string(std::visit([](auto&& arg) -> double { return arg; }, value));
            for (const auto& enumVal : enumValues) {
                if (enumVal == currentStr) {
                    found = true;
                    break;
                }
            }
        } else if (std::holds_alternative<bool>(value)) {
            currentStr = std::visit([](auto&& arg) -> std::string { return arg ? "true" : "false"; }, value);
            for (const auto& enumVal : enumValues) {
                if (enumVal == currentStr) {
                    found = true;
                    break;
                }
            }
        } else if (std::holds_alternative<std::string>(value)) {
            currentStr = std::visit([](auto&& arg) -> std::string { return arg; }, value);
            for (const auto& enumVal : enumValues) {
                if (enumVal == currentStr) {
                    found = true;
                    break;
                }
            }
        }
        
        return found;
    }
    
    // Helper function to parse constraints from string
    std::map<std::string, std::string> parseConstraints(const std::string& constraintsStr) {
        std::map<std::string, std::string> constraints;
        if (constraintsStr.empty()) return constraints;
        
        size_t pos = 0;
        size_t end = constraintsStr.find(';');
        while (end != std::string::npos) {
            std::string pair = constraintsStr.substr(pos, end - pos);
            size_t equalPos = pair.find('=');
            if (equalPos != std::string::npos) {
                std::string key = pair.substr(0, equalPos);
                std::string value = pair.substr(equalPos + 1);
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t\n\r"));
                key.erase(key.find_last_not_of(" \t\n\r") + 1);
                value.erase(0, value.find_first_not_of(" \t\n\r"));
                value.erase(value.find_last_not_of(" \t\n\r") + 1);
                constraints[key] = value;
            }
            pos = end + 1;
            end = constraintsStr.find(';', pos);
        }
        
        // Process last pair
        if (pos < constraintsStr.length()) {
            std::string pair = constraintsStr.substr(pos);
            size_t equalPos = pair.find('=');
            if (equalPos != std::string::npos) {
                std::string key = pair.substr(0, equalPos);
                std::string value = pair.substr(equalPos + 1);
                // Trim whitespace
                key.erase(0, key.find_first_not_of(" \t\n\r"));
                key.erase(key.find_last_not_of(" \t\n\r") + 1);
                value.erase(0, value.find_first_not_of(" \t\n\r"));
                value.erase(value.find_last_not_of(" \t\n\r") + 1);
                constraints[key] = value;
            }
        }
        
        return constraints;
    }
    
} // namespace

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, ConfigSchema> schemas;
    std::map<std::string, Config> presets;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO: Implement proper JSON/YAML parser
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
        file << entry.key << " = ";
        
        // Convert value to string based on type
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
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

// ConfigValidator implementation
std::vector<ValidationError> ConfigValidator::validate(const Config& config, const ConfigSchema& schema) {
    std::vector<ValidationError> errors;
    
    // Check if key exists
    if (schema.required && !config.has(schema.key)) {
        errors.emplace_back(schema.key, "Required key is missing", "missing_required");
        return errors; // Can't validate value if key doesn't exist
    }
    
    // If key doesn't exist but isn't required, skip validation
    if (!config.has(schema.key)) {
        return errors;
    }
    
    // Get the value from config
    ConfigValue value;
    auto it = std::find_if(config.pImpl->entries.begin(), config.pImpl->entries.end(),
        [&schema](const ConfigEntry& e) { return e.key == schema.key; });
    
    if (it != config.pImpl->entries.end()) {
        value = it->value;
    }
    
    // Validate type
    std::string normalizedType = normalizeType(schema.type);
    std::string actualType = configValueToString(value);
    
    // Type validation (simplified - in practice would be more sophisticated)
    bool typeValid = false;
    if (normalizedType == "int" && (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value))) {
        typeValid = true;
    } else if (normalizedType == "double" && std::holds_alternative<double>(value)) {
        typeValid = true;
    } else if (normalizedType == "bool" && std::holds_alternative<bool>(value)) {
        typeValid = true;
    } else if (normalizedType == "string" && std::holds_alternative<std::string>(value)) {
        typeValid = true;
    } else if (normalizedType == "vector" && 
               (std::holds_alternative<std::vector<int>>(value) ||
                std::holds_alternative<std::vector<double>>(value) ||
                std::holds_alternative<std::vector<std::string>>(value))) {
        typeValid = true;
    }
    
    if (!typeValid) {
        errors.emplace_back(schema.key, "Invalid type: expected " + schema.type + ", got " + 
                           (actualType.empty() ? "unknown" : actualType), "type_mismatch");
    }
    
    // Validate constraints
    if (typeValid) {
        if (!validateValue(schema.key, value, schema.type, schema.constraints)) {
            errors.emplace_back(schema.key, "Value does not meet constraints", "constraint_violation");
        }
    }
    
    // Run custom validator if provided
    if (schema.validator && typeValid) {
        if (!schema.validator(value)) {
            errors.emplace_back(schema.key, "Custom validation failed", "custom_validator");
        }
    }
    
    return errors;
}

std::vector<ValidationError> ConfigValidator::validateAll(const Config& config, const std::vector<ConfigSchema>& schemas) {
    std::vector<ValidationError> allErrors;
    for (const auto& schema : schemas) {
        auto errors = validate(config, schema);
        allErrors.insert(allErrors.end(), errors.begin(), errors.end());
    }
    return allErrors;
}

bool ConfigValidator::validateValue(const std::string& key, const ConfigValue& value, const std::string& type, const std::map<std::string, std::string>& constraints) {
    std::string normalizedType = normalizeType(type);
    
    // Check type-specific constraints
    if (normalizedType == "int" || normalizedType == "double") {
        if (!checkRangeConstraint(value, type, constraints)) {
            return false;
        }
        if (!checkPatternConstraint(configValueToString(value), constraints)) {
            return false;
        }
        return true;
    } else if (normalizedType == "string") {
        if (!checkPatternConstraint(configValueToString(value), constraints)) {
            return false;
        }
        if (!checkEnumConstraint(value, constraints)) {
            return false;
        }
        return true;
    } else if (normalizedType == "vector") {
        if (!checkVectorConstraint(value, constraints)) {
            return false;
        }
        return true;
    }
    
    return false; // Unknown type
}

std::vector<std::string> ConfigValidator::getAvailableTypes() {
    return {"int", "double", "bool", "string", "vector"};
}

std::string ConfigValidator::valueToString(const ConfigValue& value) {
    return configValueToString(value);
}

// Config validation implementation
std::vector<ValidationError> Config::validate(const std::vector<ConfigSchema>& schemas) const {
    return ConfigValidator::validateAll(*this, schemas);
}

std::vector<ValidationError> Config::validate(const std::string& schemaName) const {
    auto it = pImpl->schemas.find(schemaName);
    if (it == pImpl->schemas.end()) {
        return {ValidationError(schemaName, "Schema not found", "schema_not_found")};
    }
    return validate(std::vector<ConfigSchema>{it->second});
}

bool Config::isValid(const std::vector<ConfigSchema>& schemas) const {
    return validate(schemas).empty();
}

bool Config::isValid(const std::string& schemaName) const {
    return validate(schemaName).empty();
}

void Config::addSchema(const ConfigSchema& schema) {
    pImpl->schemas[schema.key] = schema;
}

void Config::addSchema(const std::string& name, const ConfigSchema& schema) {
    pImpl->schemas[name] = schema;
}

bool Config::hasSchema(const std::string& name) const {
    return pImpl->schemas.find(name) != pImpl->schemas.end();
}

ConfigSchema Config::getSchema(const std::string& name) const {
    auto it = pImpl->schemas.find(name);
    if (it == pImpl->schemas.end()) {
        throw std::runtime_error("Schema not found: " + name);
    }
    return it->second;
}

std::vector<std::string> Config::getSchemaNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->schemas.size());
    for (const auto& pair : pImpl->schemas) {
        names.push_back(pair.first);
    }
    return names;
}

void Config::addPreset(const std::string& name, const Config& presetConfig) {
    pImpl->presets[name] = presetConfig;
}

bool Config::hasPreset(const std::string& name) const {
    return pImpl->presets.find(name) != pImpl->presets.end();
}

Config Config::getPreset(const std::string& name) const {
    auto it = pImpl->presets.find(name);
    if (it == pImpl->presets.end()) {
        throw std::runtime_error("Preset not found: " + name);
    }
    return it->second;
}

std::vector<std::string> Config::getPresetNames() const {
    std::vector<std::string> names;
    names.reserve(pImpl->presets.size());
    for (const auto& pair : pImpl->presets) {
        names.push_back(pair.first);
    }
    return names;
}

std::string Config::getValueType(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "unknown";
    }
    
    if (std::holds_alternative<int>(it->value)) {
        return "int";
    } else if (std::holds_alternative<int64_t>(it->value)) {
        return "int64";
    } else if (std::holds_alternative<double>(it->value)) {
        return "double";
    } else if (std::holds_alternative<bool>(it->value)) {
        return "bool";
    } else if (std::holds_alternative<std::string>(it->value)) {
        return "string";
    } else if (std::holds_alternative<std::vector<int>>(it->value)) {
        return "vector<int>";
    } else if (std::holds_alternative<std::vector<double>>(it->value)) {
        return "vector<double>";
    } else if (std::holds_alternative<std::vector<std::string>>(it->value)) {
        return "vector<string>";
    }
    
    return "unknown";
}

std::string Config::getDescription(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return "";
    }
    
    return it->description;
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return ConfigSource::Default;
    }
    
    return it->source;
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool Config::isNumeric(const std::string& str) {
    if (str.empty()) return false;
    size_t pos = 0;
    if (str[0] == '-') pos = 1;
    bool hasDecimal = false;
    for (; pos < str.length(); ++pos) {
        if (str[pos] == '.') {
            if (hasDecimal) return false;
            hasDecimal = true;
        } else if (!std::isdigit(str[pos])) {
            return false;
        }
    }
    return true;
}

// Initialize predefined schemas
namespace ConfigSchemas {
    const std::vector<ConfigSchema> Phase2Defaults = {
        ConfigSchema("random_seed", "int", "Random seed for reproducible simulation", true, static_cast<int64_t>(42)),
        ConfigSchema("simulation_timestep", "double", "Simulation timestep in seconds", true, 0.001),
        ConfigSchema("neuron_count", "int", "Number of neurons in the brain", true, static_cast<int64_t>(500)),
        ConfigSchema("region_count", "int", "Number of neural regions", true, static_cast<int64_t>(1)),
        ConfigSchema("connection_probability", "double", "Probability of synapse formation", true, 0.15),
        ConfigSchema("stdp_ltp_weight", "double", "STDP long-term potentiation weight", true, 0.02),
        ConfigSchema("stdp_ltd_weight", "double", "STDP long-term depression weight", true, 0.015),
        ConfigSchema("stdp_tau", "double", "STDP time constant", true, 20.0),
        ConfigSchema("synaptogenesis_rate", "double", "Structural synaptogenesis rate", true, 0.0001),
        ConfigSchema("pruning_rate", "double", "Structural pruning rate", true, 0.00001),
        ConfigSchema("max_synapses_per_neuron", "int", "Maximum synapses per neuron", false, static_cast<int64_t>(100)),
        ConfigSchema("learning_rate", "double", "Global learning rate", false, 0.01),
        ConfigSchema("excitatory_ratio", "double", "Ratio of excitatory neurons", false, 0.8),
        ConfigSchema("inhibitory_ratio", "double", "Ratio of inhibitory neurons", false, 0.2)
    };
    
    const std::vector<ConfigSchema> Phase6Defaults = {
        ConfigSchema("random_seed", "int", "Random seed for reproducible simulation", true, static_cast<int64_t>(42)),
        ConfigSchema("simulation_timestep", "double", "Simulation timestep in seconds", true, 0.001),
        ConfigSchema("neuron_count", "int", "Number of neurons in the brain", true, static_cast<int64_t>(2000)),
        ConfigSchema("region_count", "int", "Number of neural regions", true, static_cast<int64_t>(3)),
        ConfigSchema("connection_probability", "double", "Probability of synapse formation", true, 0.2),
        ConfigSchema("stdp_ltp_weight", "double", "STDP long-term potentiation weight", true, 0.05),
        ConfigSchema("stdp_ltd_weight", "double", "STDP long-term depression weight", true, 0.03),
        ConfigSchema("stdp_tau", "double", "STDP time constant", true, 50.0),
        ConfigSchema("synaptogenesis_rate", "double", "Structural synaptogenesis rate", true, 0.001),
        ConfigSchema("pruning_rate", "double", "Structural pruning rate", true, 0.0005),
        ConfigSchema("max_synapses_per_neuron", "int", "Maximum synapses per neuron", false, static_cast<int64_t>(200)),
        ConfigSchema("working_memory_capacity", "int", "Working memory capacity in items", false, static_cast<int64_t>(50)),
        ConfigSchema("episodic_memory_capacity", "int", "Episodic memory capacity in episodes", false, static_cast<int64_t>(100)),
        ConfigSchema("novelty_threshold", "double", "Novelty detection threshold", false, 0.5),
        ConfigSchema("curiosity_strength", "double", "Curiosity drive strength", false, 0.3),
        ConfigSchema("reward_scaling", "double", "Reward prediction error scaling factor", false, 1.0),
        ConfigSchema("prediction_horizon", "int", "Prediction system time horizon", false, static_cast<int64_t>(10)),
        ConfigSchema("concept_formation_rate", "double", "Concept formation learning rate", false, 0.02)
    };
    
    const std::vector<ConfigSchema> LearningPresets = {
        ConfigSchema("random_seed", "int", "Random seed for reproducible learning", true, static_cast<int64_t>(42)),
        ConfigSchema("simulation_timestep", "double", "Simulation timestep in seconds", true, 0.001),
        ConfigSchema("neuron_count", "int", "Number of neurons for learning experiments", false, static_cast<int64_t>(1000)),
        ConfigSchema("region_count", "int", "Number of neural regions", false, static_cast<int64_t>(2)),
        ConfigSchema("connection_probability", "double", "Probability of synapse formation", false, 0.1),
        ConfigSchema("stdp_ltp_weight", "double", "STDP long-term potentiation weight", false, 0.05),
        ConfigSchema("stdp_ltd_weight", "double", "STDP long-term depression weight", false, 0.03),
        ConfigSchema("stdp_tau", "double", "STDP time constant", false, 50.0),
        ConfigSchema("synaptogenesis_rate", "double", "Structural synaptogenesis rate", false, 0.001),
        ConfigSchema("pruning_rate", "double", "Structural pruning rate", false, 0.0005),
        ConfigSchema("learning_rate", "double", "Global learning rate", false, 0.02),
        ConfigSchema("excitatory_ratio", "double", "Ratio of excitatory neurons", false, 0.7),
        ConfigSchema("inhibitory_ratio", "double", "Ratio of inhibitory neurons", false, 0.3)
    };
    
    const std::vector<ConfigSchema> DevelopmentPresets = {
        ConfigSchema("random_seed", "int", "Random seed for developmental simulation", true, static_cast<int64_t>(42)),
        ConfigSchema("simulation_timestep", "double", "Simulation timestep in seconds", true, 0.001),
        ConfigSchema("neuron_count", "int", "Number of neurons", true, static_cast<int64_t>(500)),
        ConfigSchema("region_count", "int", "Number of neural regions", true, static_cast<int64_t>(1)),
        ConfigSchema("connection_probability", "double", "Probability of synapse formation", true, 0.15),
        ConfigSchema("initial_plasticity_rate", "double", "Initial plasticity rate (0.0-1.0)", false, 1.0),
        ConfigSchema("plasticity_decay_rate", "double", "Plasticity decay rate per timestep", false, 0.95),
        ConfigSchema("excitability_growth_rate", "double", "Excitability growth rate", false, 0.9),
        ConfigSchema("critical_period_duration", "double", "Critical period duration in steps", false, 1000.0),
        ConfigSchema("maturation_threshold", "double", "Maturation threshold", false, 0.8),
        ConfigSchema("aging_rate", "double", "Aging rate (0.0-1.0)", false, 0.01)
    };
    
    const std::vector<ConfigSchema> StructuralPresets = {
        ConfigSchema("random_seed", "int", "Random seed for structural plasticity", true, static_cast<int64_t>(42)),
        ConfigSchema("simulation_timestep", "double", "Simulation timestep in seconds", true, 0.001),
        ConfigSchema("neuron_count", "int", "Number of neurons", true, static_cast<int64_t>(500)),
        ConfigSchema("region_count", "int", "Number of neural regions", true, static_cast<int64_t>(1)),
        ConfigSchema("connection_probability", "double", "Probability of synapse formation", true, 0.15),
        ConfigSchema("synaptogenesis_rate", "double", "Synaptogenesis rate (synapses per neuron per step)", true, 0.001),
        ConfigSchema("pruning_rate", "double", "Pruning rate (synapses removed per neuron per step)", true, 0.0005),
        ConfigSchema("max_synapses_per_neuron", "int", "Maximum synapses per neuron", false, static_cast<int64_t>(200)),
        ConfigSchema("structural_plasticity_threshold", "double", "Structural plasticity activation threshold", false, 0.5),
        ConfigSchema("activity_dependent_synaptogenesis", "bool", "Whether synaptogenesis is activity-dependent", false, true),
        ConfigSchema("activity_dependent_pruning", "bool", "Whether pruning is activity-dependent", false, true)
    };
}

// Initialize utility functions
namespace ConfigUtils {
    Config createDefaultConfig() {
        Config config;
        const auto& schemas = ConfigSchemas::Phase2Defaults;
        for (const auto& schema : schemas) {
            config.set(schema.key, schema.defaultValue, ConfigSource::Default);
        }
        return config;
    }
    
    Config createLearningConfig() {
        Config config;
        const auto& schemas = ConfigSchemas::LearningPresets;
        for (const auto& schema : schemas) {
            config.set(schema.key, schema.defaultValue, ConfigSource::Default);
        }
        return config;
    }
    
    Config createDevelopmentConfig() {
        Config config;
        const auto& schemas = ConfigSchemas::DevelopmentPresets;
        for (const auto& schema : schemas) {
            config.set(schema.key, schema.defaultValue, ConfigSource::Default);
        }
        return config;
    }
    
    Config createStructuralConfig() {
        Config config;
        const auto& schemas = ConfigSchemas::StructuralPresets;
        for (const auto& schema : schemas) {
            config.set(schema.key, schema.defaultValue, ConfigSource::Default);
        }
        return config;
    }
    
    Config applyPreset(const Config& baseConfig, const std::string& presetName) {
        Config result = baseConfig;
        if (presetName == "phase2") {
            const auto& schemas = ConfigSchemas::Phase2Defaults;
            for (const auto& schema : schemas) {
                if (baseConfig.has(schema.key) || schema.required) {
                    result.set(schema.key, schema.defaultValue, ConfigSource::Runtime);
                }
            }
        } else if (presetName == "phase6") {
            const auto& schemas = ConfigSchemas::Phase6Defaults;
            for (const auto& schema : schemas) {
                if (baseConfig.has(schema.key) || schema.required) {
                    result.set(schema.key, schema.defaultValue, ConfigSource::Runtime);
                }
            }
        } else if (presetName == "learning") {
            const auto& schemas = ConfigSchemas::LearningPresets;
            for (const auto& schema : schemas) {
                if (baseConfig.has(schema.key) || schema.required) {
                    result.set(schema.key, schema.defaultValue, ConfigSource::Runtime);
                }
            }
        } else if (presetName == "development") {
            const auto& schemas = ConfigSchemas::DevelopmentPresets;
            for (const auto& schema : schemas) {
                if (baseConfig.has(schema.key) || schema.required) {
                    result.set(schema.key, schema.defaultValue, ConfigSource::Runtime);
                }
            }
        } else if (presetName == "structural") {
            const auto& schemas = ConfigSchemas::StructuralPresets;
            for (const auto& schema : schemas) {
                if (baseConfig.has(schema.key) || schema.required) {
                    result.set(schema.key, schema.defaultValue, ConfigSource::Runtime);
                }
            }
        }
        return result;
    }
    
    std::vector<ValidationError> validateConfig(const Config& config, const std::string& presetName) {
        if (presetName == "phase2") {
            return config.validate(ConfigSchemas::Phase2Defaults);
        } else if (presetName == "phase6") {
            return config.validate(ConfigSchemas::Phase6Defaults);
        } else if (presetName == "learning") {
            return config.validate(ConfigSchemas::LearningPresets);
        } else if (presetName == "development") {
            return config.validate(ConfigSchemas::DevelopmentPresets);
        } else if (presetName == "structural") {
            return config.validate(ConfigSchemas::StructuralPresets);
        } else {
            return config.validate(std::vector<ConfigSchema>{}); // No validation for unknown preset
        }
    }
}

} // namespace nlm