#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <iomanip>
#include <ctime>
#include <cstring>
#include <regex>
#include <yaml-cpp/yaml.h>
#include <nlohmann/json.hpp>
#include "ErrorHandling.hpp"

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()), schema() {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Parser helper implementations
bool Config::parseJSON(const std::string& content) {
    try {
        auto json = nlohmann::json::parse(content);
        
        if (json.is_object()) {
            for (const auto& item : json.items()) {
                const std::string& key = item.key();
                
                // Handle nested objects recursively
                if (item.value().is_object()) {
                    set(key, ConfigValue(std::string("{" + item.value().dump() + "}")), ConfigSource::File);
                } else {
                    // Convert JSON value to ConfigValue based on type
                    std::string valueStr;
                    if (item.value().is_string()) {
                        valueStr = item.value().get<std::string>();
                        set(key, valueStr, ConfigSource::File);
                    } else if (item.value().is_number_integer()) {
                        int64_t intVal = item.value().get<int64_t>();
                        set(key, intVal, ConfigSource::File);
                    } else if (item.value().is_number_float()) {
                        double doubleVal = item.value().get<double>();
                        set(key, doubleVal, ConfigSource::File);
                    } else if (item.value().is_boolean()) {
                        bool boolVal = item.value().get<bool>();
                        set(key, boolVal, ConfigSource::File);
                    } else if (item.value().is_array()) {
                        const auto& arr = item.value();
                        std::vector<std::string> strArr;
                        for (const auto& elem : arr) {
                            strArr.push_back(elem.get<std::string>());
                        }
                        set(key, strArr, ConfigSource::File);
                    }
                }
            }
            return true;
        }
    } catch (const std::exception& e) {
        auto error = Errors::configInvalid("json_content", "valid_json", "invalid_json", 
                                          nlm::ErrorContextUtils::createContext("Config", "parseJSON", "", 0));
        error->getContext().exceptionMessage = e.what();
        nlm::getGlobalErrorManager().recordError(error);
        setError(std::string("JSON parsing error: ") + e.what());
        return false;
    }
    return false;
}

bool Config::parseYAML(const std::string& content) {
    try {
        YAML::Node doc = YAML::Load(content);
        
        if (doc.IsMap()) {
            for (const auto& item : doc) {
                const std::string& key = item.first.as<std::string>();
                
                if (item.second.IsMap()) {
                    // Handle nested objects by converting to string representation
                    std::string yamlStr = item.second.Write();
                    set(key, ConfigValue("{" + yamlStr + "}"), ConfigSource::File);
                } else if (item.second.IsScalar()) {
                    std::string valueStr = item.second.as<std::string>();
                    set(key, valueStr, ConfigSource::File);
                } else if (item.second.IsSequence()) {
                    std::vector<std::string> strArr;
                    for (const auto& elem : item.second) {
                        strArr.push_back(elem.as<std::string>());
                    }
                    set(key, strArr, ConfigSource::File);
                }
            }
            return true;
        }
    } catch (const std::exception& e) {
        auto error = Errors::configInvalid("yaml_content", "valid_yaml", "invalid_yaml", 
                                          nlm::ErrorContextUtils::createContext("Config", "parseYAML", "", 0));
        error->getContext().exceptionMessage = e.what();
        nlm::getGlobalErrorManager().recordError(error);
        setError(std::string("YAML parsing error: ") + e.what());
        return false;
    }
    return false;
}

bool Config::parseKeyValue(const std::string& content) {
    // Original simple parser for backward compatibility
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == \'\' && value.back() == \'\'))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    return true;
}

bool Config::loadFromFile(const std::string& filepath, bool validate) {
    clear();
    setError("");
    
    // Create error context
    nlm::ErrorContext ctx;
    ctx.file = "Config.hpp";
    ctx.line = __LINE__;
    ctx.function = "loadFromFile";
    ctx.module = "Config";
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        auto error = Errors::ioError(ErrorCode::FILE_NOT_FOUND, filepath, "Failed to open file");
        error->getContext() = ctx;
        nlm::getGlobalErrorManager().recordError(error);
        setError("Failed to open file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    // Determine file type based on extension
    std::string ext = "." + filepath.substr(filepath.find_last_of('.') + 1);
    bool success = false;
    
    if (ext == ".json") {
        success = parseJSON(content);
    } else if (ext == ".yaml" || ext == ".yml") {
        success = parseYAML(content);
    } else {
        // Default to key=value format for backward compatibility
        success = parseKeyValue(content);
    }
    
    if (success && validate && !schema.empty()) {
        ValidationResult result = validateAgainstSchema(schema);
        if (!result.valid) {
            auto error = Errors::configInvalid("schema_validation", "valid_schema", 
                                              "Schema validation failed: " + result.getErrors());
            error->getContext() = ctx;
            nlm::getGlobalErrorManager().recordError(error);
            setError("Schema validation failed: " + result.getErrors());
            return false;
        }
    }
    
    return success;
}

bool Config::loadFromJSON(const std::string& filepath, bool validate) {
    clear();
    setError("");
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        setError("Failed to open JSON file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    bool success = parseJSON(content);
    
    if (success && validate && !schema.empty()) {
        ValidationResult result = validateAgainstSchema(schema);
        if (!result.valid) {
            setError("Schema validation failed: " + result.getErrors());
            return false;
        }
    }
    
    return success;
}

bool Config::loadFromYAML(const std::string& filepath, bool validate) {
    clear();
    setError("");
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        setError("Failed to open YAML file: " + filepath);
        return false;
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    
    bool success = parseYAML(content);
    
    if (success && validate && !schema.empty()) {
        ValidationResult result = validateAgainstSchema(schema);
        if (!result.valid) {
            setError("Schema validation failed: " + result.getErrors());
            return false;
        }
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv, bool validate) {
    clear();
    setError("");
    
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
    
    if (validate && !schema.empty()) {
        ValidationResult result = validateAgainstSchema(schema);
        if (!result.valid) {
            setError("Schema validation failed: " + result.getErrors());
            return false;
        }
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& jsonString, bool validate) {
    clear();
    setError("");
    
    bool success = parseJSON(jsonString);
    
    if (success && validate && !schema.empty()) {
        ValidationResult result = validateAgainstSchema(schema);
        if (!result.valid) {
            setError("Schema validation failed: " + result.getErrors());
            return false;
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        auto error = Errors::ioError(ErrorCode::FILE_ACCESS_DENIED, filepath, "Failed to open file for writing");
        nlm::getGlobalErrorManager().recordError(error);
        return false;
    }
    
    // Save in JSON format for better compatibility
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigValue to nlohmann::json
        std::string key = entry.key;
        
        // Handle different ConfigValue types
        if (std::holds_alternative<int>(entry.value)) {
            jsonData[key] = std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            jsonData[key] = std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            jsonData[key] = std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            jsonData[key] = std::get<bool>(entry.value);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            jsonData[key] = std::get<std::string>(entry.value);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            auto intVec = std::get<std::vector<int>>(entry.value);
            jsonData[key] = intVec;
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            auto doubleVec = std::get<std::vector<double>>(entry.value);
            jsonData[key] = doubleVec;
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            auto stringVec = std::get<std::vector<std::string>>(entry.value);
            jsonData[key] = stringVec;
        }
    }
    
    file << jsonData.dump(4);
    return true;
}

std::string Config::toJSON() const {
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        
        if (std::holds_alternative<int>(entry.value)) {
            jsonData[key] = std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            jsonData[key] = std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            jsonData[key] = std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            jsonData[key] = std::get<bool>(entry.value);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            jsonData[key] = std::get<std::string>(entry.value);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            auto intVec = std::get<std::vector<int>>(entry.value);
            jsonData[key] = intVec;
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            auto doubleVec = std::get<std::vector<double>>(entry.value);
            jsonData[key] = doubleVec;
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            auto stringVec = std::get<std::vector<std::string>>(entry.value);
            jsonData[key] = stringVec;
        }
    }
    
    return jsonData.dump(4);
}

std::string Config::toYAML() const {
    YAML::Emitter emitter;
    emitter.SetIndent(2);
    emitter.SetMapFormat(YAML::EmitterStyle::Block);
    
    emitter << YAML::BeginMap;
    
    for (const auto& entry : pImpl->entries) {
        std::string key = entry.key;
        emitter << YAML::Key << key;
        
        if (std::holds_alternative<int>(entry.value)) {
            emitter << std::get<int>(entry.value);
        } else if (std::holds_alternative<int64_t>(entry.value)) {
            emitter << std::get<int64_t>(entry.value);
        } else if (std::holds_alternative<double>(entry.value)) {
            emitter << std::get<double>(entry.value);
        } else if (std::holds_alternative<bool>(entry.value)) {
            emitter << std::get<bool>(entry.value);
        } else if (std::holds_alternative<std::string>(entry.value)) {
            emitter << std::get<std::string>(entry.value);
        } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
            emitter << YAML::BeginSeq;
            auto intVec = std::get<std::vector<int>>(entry.value);
            for (int val : intVec) {
                emitter << val;
            }
            emitter << YAML::EndSeq;
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            emitter << YAML::BeginSeq;
            auto doubleVec = std::get<std::vector<double>>(entry.value);
            for (double val : doubleVec) {
                emitter << val;
            }
            emitter << YAML::EndSeq;
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            emitter << YAML::BeginSeq;
            auto stringVec = std::get<std::vector<std::string>>(entry.value);
            for (const auto& val : stringVec) {
                emitter << val;
            }
            emitter << YAML::EndSeq;
        }
    }
    
    emitter << YAML::EndMap;
    
    return emitter.c_str();
}

ValidationResult Config::validate(const ConfigSchema& schema) const {
    ValidationResult result;
    
    // Check for missing required fields
    if (schema.required && !has(schema.name)) {
        result.addError("Required configuration key missing: '" + schema.name + "'");
        return result;
    }
    
    // Validate type if value exists
    if (has(schema.name)) {
        auto val = get<std::string>(schema.name);
        if (val.has_value()) {
            // Validate against schema type
            if (!schema.isValidType(ConfigValue(val.value()))) {
                result.addError("Invalid type for configuration key '" + schema.name + "'. Expected: " + 
                              schema.getTypeName() + ", Got: " + getTypeNameFromValue(ConfigValue(val.value())));
            }
        }
    }
    
    return result;
}

bool Config::validateAgainstSchema(const std::vector<ConfigSchema>& schema) {
    ValidationResult result;
    
    for (const auto& schemaItem : schema) {
        ValidationResult itemResult = validate(schemaItem);
        if (!itemResult.valid) {
            result.errors.insert(result.errors.end(), itemResult.errors.begin(), itemResult.errors.end());
        }
        if (itemResult.hasWarnings()) {
            result.warnings.insert(result.warnings.end(), itemResult.warnings.begin(), itemResult.warnings.end());
        }
    }
    
    if (!result.valid) {
        setError("Validation failed: " + result.getErrors());
    } else if (result.hasWarnings()) {
        setError("Validation warnings: " + result.getWarnings());
    }
    
    return result.valid;
}

void Config::setSchema(const std::vector<ConfigSchema>& newSchema) {
    schema = newSchema;
}

void Config::addSchema(const ConfigSchema& schemaItem) {
    schema.push_back(schemaItem);
}

void Config::clearSchema() {
    schema.clear();
}

std::string Config::getSchemaHelp() const {
    std::ostringstream oss;
    oss << "Configuration Schema:\n";
    oss << "===================\n\n";
    
    for (const auto& schemaItem : schema) {
        oss << "Key: " << schemaItem.name << "\n";
        oss << "  Type: " << schemaItem.getTypeName() << "\n";
        oss << "  Description: " << schemaItem.description << "\n";
        if (!schemaItem.defaultValue.empty()) {
            oss << "  Default: " << schemaItem.defaultValue << "\n";
        }
        if (schemaItem.required) {
            oss << "  Required: Yes\n";
        }
        oss << "\n";
    }
    
    return oss.str();
}

ValidationResult Config::validateKey(const std::string& key) const {
    ValidationResult result;
    
    for (const auto& schemaItem : schema) {
        if (schemaItem.name == key) {
            if (!schemaItem.isValidType(ConfigValue("placeholder"))) {
                result.addError("Invalid type for configuration key: '" + key + "'");
            }
            return result;
        }
    }
    
    result.addError("Unknown configuration key: '" + key + "'");
    return result;
}

ValidationResult Config::validateValue(const ConfigEntry& entry) const {
    ValidationResult result;
    
    for (const auto& schemaItem : schema) {
        if (schemaItem.name == entry.key) {
            // Validate value against schema
            if (!schemaItem.isValidType(entry.value)) {
                result.addError("Invalid value for configuration key '" + entry.key + "'. Expected type: " + 
                              schemaItem.getTypeName() + ". Actual type: " + getTypeNameFromValue(entry.value));
            }
            
            // Check if value matches expected format
            if (std::holds_alternative<std::string>(entry.value) && schemaItem.validators.size() > 0) {
                const std::string& value = std::get<std::string>(entry.value);
                for (const auto& validator : schemaItem.validators) {
                    if (!validateConfigValue(value, validator)) {
                        result.addError("Value '" + value + "' for key '" + entry.key + "' failed validation: " + validator);
                    }
                }
            }
            
            break;
        }
    }
    
    return result;
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

bool ConfigSchema::isValidType(const ConfigValue& value) const {
    switch (type) {
        case ConfigType::INTEGER:
            return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
        case ConfigType::DOUBLE:
            return std::holds_alternative<double>(value);
        case ConfigType::STRING:
            return std::holds_alternative<std::string>(value);
        case ConfigType::BOOLEAN:
            return std::holds_alternative<bool>(value);
        case ConfigType::INTEGER_ARRAY:
            return std::holds_alternative<std::vector<int>>(value);
        case ConfigType::DOUBLE_ARRAY:
            return std::holds_alternative<std::vector<double>>(value);
        case ConfigType::STRING_ARRAY:
            return std::holds_alternative<std::vector<std::string>>(value);
        case ConfigType::OBJECT:
            return std::holds_alternative<std::string>(value); // Objects stored as JSON string
        default:
            return false;
    }
}

std::string ConfigSchema::getTypeName() const {
    switch (type) {
        case ConfigType::INTEGER: return "integer";
        case ConfigType::DOUBLE: return "double";
        case ConfigType::STRING: return "string";
        case ConfigType::BOOLEAN: return "boolean";
        case ConfigType::INTEGER_ARRAY: return "integer_array";
        case ConfigType::DOUBLE_ARRAY: return "double_array";
        case ConfigType::STRING_ARRAY: return "string_array";
        case ConfigType::OBJECT: return "object";
        default: return "unknown";
    }
}

std::string Config::getTypeNameFromValue(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "integer";
    if (std::holds_alternative<int64_t>(value)) return "int64";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "boolean";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "integer_array";
    if (std::holds_alternative<std::vector<double>>(value)) return "double_array";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "string_array";
    return "unknown";
}

bool Config::validateConfigValue(const std::string& value, const std::string& validator) {
    // Implement common validators
    if (validator == "positive") {
        try {
            double val = std::stod(value);
            return val > 0;
        } catch (...) {
            return false;
        }
    } else if (validator == "non-empty") {
        return !value.empty();
    } else if (validator == "alphanumeric") {
        std::regex pattern("^[a-zA-Z0-9_]+$");
        return std::regex_match(value, pattern);
    }
    return true;
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

std::string Config::escapeJSON(const std::string& str) {
    std::ostringstream oss;
    oss << std::quoted(str);
    return oss.str();
}

std::string Config::escapeYAML(const std::string& str) {
    // Escape YAML special characters
    std::string result = str;
    // Replace control characters with \u escapes
    for (char& c : result) {
        if (c == '\n') {
            c = '\\n';
        } else if (c == '\r') {
            c = '\\r';
        } else if (c == '\t') {
            c = '\\t';
        } else if (c == '\"') {
            c = '\\"';
        } else if (c == '\\') {
            c = '\\\\';
        }
    }
    return result;
}

// Explicit template instantiations

// Value getters
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// Default value getters
template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm

// Template implementations for array types
template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (std::holds_alternative<std::vector<int>>(it->value)) {
                return std::get<std::vector<int>>(it->value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (std::holds_alternative<std::vector<double>>(it->value)) {
                return std::get<std::vector<double>>(it->value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            if (std::holds_alternative<std::vector<std::string>>(it->value)) {
                return std::get<std::vector<std::string>>(it->value);
            }
        }
    } catch (...) {}
    
    return std::nullopt;
}

// Array getOr implementations

template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;
