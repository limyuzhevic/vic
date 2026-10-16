#include "AdvancedConfig.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<ConfigSchema> schemas;
    std::map<std::string, std::string> validationErrors;
    
    // JSON parsing helpers
    static nlohmann::json parseJSONFile(const std::string& filepath);
    static nlohmann::json parseYAMLFile(const std::string& filepath);
    static std::string detectFormat(const std::string& filepath);
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    std::string format = Impl::detectFormat(filepath);
    return loadFromString(filepath, format);
}

bool Config::loadFromYAML(const std::string& filepath) {
    try {
        auto jsonData = Impl::parseYAMLFile(filepath);
        return loadFromJSONInternal(jsonData, ConfigSource::File, filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load YAML config from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    try {
        auto jsonData = Impl::parseJSONFile(filepath);
        return loadFromJSONInternal(jsonData, ConfigSource::File, filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to load JSON config from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromString(const std::string& content, const std::string& format) {
    ConfigSource source = ConfigSource::File;
    
    if (format == "json") {
        try {
            auto jsonData = nlohmann::json::parse(content);
            return loadFromJSONInternal(jsonData, source, "");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to parse JSON content: " + std::string(e.what()));
            return false;
        }
    } else if (format == "yaml" || format == "yml") {
        try {
            auto jsonData = Impl::parseYAMLString(content);
            return loadFromJSONInternal(jsonData, source, "");
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to parse YAML content: " + std::string(e.what()));
            return false;
        }
    } else {
        NLM_LOG_ERROR("Unsupported format: " + format);
        return false;
    }
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

bool Config::loadFromEnv(const std::map<std::string, std::string>& envVars) {
    bool success = true;
    
    for (const auto& pair : envVars) {
        const std::string& key = pair.first;
        const std::string& value = pair.second;
        
        // Convert environment variable to config key (lowercase, replace hyphens with underscores)
        std::string configKey = key;
        std::transform(configKey.begin(), configKey.end(), configKey.begin(), ::tolower);
        std::replace(configKey.begin(), configKey.end(), '-', '_');
        
        // Parse value based on type hints from key name
        bool set = false;
        if (key.find("enabled") != std::string::npos || key.find("flag") != std::string::npos) {
            try {
                bool boolVal = (value == "1" || value == "true" || value == "yes" || value == "on");
                set(key, boolVal, ConfigSource::Environment);
                set = true;
            } catch (...) {}
        } else if (key.find("count") != std::string::npos || key.find("num") != std::string::npos) {
            try {
                int intVal = std::stoi(value);
                set(key, intVal, ConfigSource::Environment);
                set = true;
            } catch (...) {}
        } else if (key.find("rate") != std::string::npos || key.find("ratio") != std::string::npos || 
                   key.find("value") != std::string::npos || key.find("scale") != std::string::npos) {
            try {
                double floatVal = std::stod(value);
                set(key, floatVal, ConfigSource::Environment);
                set = true;
            } catch (...) {}
        }
        
        if (!set) {
            // Default to string type
            set(key, value, ConfigSource::Environment);
        }
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath, const std::string& format) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string content = saveToString(format);
    file << content;
    
    if (!file.good()) {
        return false;
    }
    
    return true;
}

std::string Config::saveToString(const std::string& format) const {
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::string>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonData[entry.key] = arg;
            }
        }, entry.value);
    }
    
    if (format == "json") {
        return jsonData.dump(2);
    } else if (format == "yaml" || format == "yml") {
        // For simplicity, we'll use JSON for YAML output for now
        // In a real implementation, you'd use a YAML library like yaml-cpp
        return jsonData.dump(2);
    } else {
        NLM_LOG_ERROR("Unsupported format for saving: " + format);
        return "";
    }
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
    if (validateValue(key, value)) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
        } else {
            pImpl->entries.emplace_back(key, value, source);
        }
        
        pImpl->validationErrors.erase(key);
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

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

bool Config::validate() const {
    pImpl->validationErrors.clear();
    
    for (const auto& schema : pImpl->schemas) {
        if (!validateSchema(schema)) {
            return false;
        }
    }
    
    return pImpl->validationErrors.empty();
}

bool Config::validate(const std::string& key) const {
    pImpl->validationErrors.erase(key);
    
    for (const auto& schema : pImpl->schemas) {
        if (schema.key == key) {
            if (!validateSchema(schema)) {
                return false;
            }
            break;
        }
    }
    
    return pImpl->validationErrors.find(key) == pImpl->validationErrors.end();
}

bool Config::validateValue(const std::string& key, const ConfigValue& value) const {
    for (const auto& schema : pImpl->schemas) {
        if (schema.key == key) {
            if (schema.validator && !schema.validator(key, value)) {
                return false;
            }
        }
    }
    return true;
}

bool Config::validateSchema(const ConfigSchema& schema) const {
    bool isValid = true;
    
    // Check if key exists
    bool keyExists = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&schema](const ConfigEntry& e) { return e.key == schema.key; });
    
    if (schema.required && !keyExists) {
        pImpl->validationErrors[schema.key] = "Required configuration key is missing";
        isValid = false;
    }
    
    if (keyExists) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&schema](const ConfigEntry& e) { return e.key == schema.key; });
        
        if (it != pImpl->entries.end()) {
            if (!validateValueType(schema.type, it->value)) {
                pImpl->validationErrors[schema.key] = "Value type does not match expected type: " + schema.type;
                isValid = false;
            }
            
            if (schema.validator && !schema.validator(schema.key, it->value)) {
                pImpl->validationErrors[schema.key] = "Value validation failed";
                isValid = false;
            }
        }
    }
    
    return isValid;
}

void Config::addSchema(const ConfigSchema& schema) {
    // Check if schema with same key already exists
    auto it = std::find_if(pImpl->schemas.begin(), pImpl->schemas.end(),
        [&schema](const ConfigSchema& s) { return s.key == schema.key; });
    
    if (it != pImpl->schemas.end()) {
        *it = schema;
    } else {
        pImpl->schemas.push_back(schema);
    }
}

bool Config::removeSchema(const std::string& key) {
    auto it = std::remove_if(pImpl->schemas.begin(), pImpl->schemas.end(),
        [&key](const ConfigSchema& s) { return s.key == key; });
    
    if (it != pImpl->schemas.end()) {
        pImpl->schemas.erase(it, pImpl->schemas.end());
        return true;
    }
    
    return false;
}

void Config::clearSchemas() {
    pImpl->schemas.clear();
}

bool Config::hasSchema(const std::string& key) const {
    return std::any_of(pImpl->schemas.begin(), pImpl->schemas.end(),
        [&key](const ConfigSchema& s) { return s.key == key; });
}

bool Config::has(const std::string& key) const {
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getKeysBySource(ConfigSource source) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (entry.source == source) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

std::vector<std::string> Config::getKeysByType(const std::string& type) const {
    std::vector<std::string> keys;
    for (const auto& entry : pImpl->entries) {
        if (typeToString(entry.value) == type) {
            keys.push_back(entry.key);
        }
    }
    return keys;
}

ConfigSource Config::getSource(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->source;
    }
    
    return ConfigSource::Default;
}

std::string Config::getFilePath(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        return it->filePath;
    }
    
    return "";
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    pImpl->validationErrors.erase(key);
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->validationErrors.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Advanced Configuration (" << pImpl->entries.size() << " entries, " 
               << pImpl->schemas.size() << " schemas):\n";
    
    if (!pImpl->validationErrors.empty()) {
        oss << "VALIDATION ERRORS:\n";
        for (const auto& error : pImpl->validationErrors) {
            oss << "  " << error.first << ": " << error.second << "\n";
        }
        oss << "\n";
    }
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = ";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << " (" << static_cast<int>(entry.source) << ")";
        if (!entry.filePath.empty()) {
            oss << " [from " << entry.filePath << "]";
        }
        oss << "\n";
    }
    
    if (!pImpl->schemas.empty()) {
        oss << "\nSCHEMAS:\n";
        for (const auto& schema : pImpl->schemas) {
            oss << "  " << schema.key << " (" << schema.type << ")";
            if (schema.required) oss << " [required]";
            if (schema.defaultValue.has_value()) {
                oss << " [default: ";
                auto& defVal = schema.defaultValue.value();
                std::visit([&oss](auto&& arg) {
                    if constexpr (std::is_same_v<T, std::string>) {
                        oss << "\"" << arg << "\"";
                    } else {
                        oss << arg;
                    }
                }, defVal);
                oss << "]";
            }
            if (!schema.description.empty()) {
                oss << " - " << schema.description;
            }
            oss << "\n";
        }
    }
    
    return oss.str();
}

std::string Config::toString(const std::string& format) const {
    return saveToString(format);
}

size_t Config::getEntryCount() const {
    return pImpl->entries.size();
}

size_t Config::getSchemaCount() const {
    return pImpl->schemas.size();
}

bool Config::isValid(const std::string& key) const {
    return pImpl->validationErrors.find(key) == pImpl->validationErrors.end();
}

std::string Config::getValidationError(const std::string& key) const {
    auto it = pImpl->validationErrors.find(key);
    if (it != pImpl->validationErrors.end()) {
        return it->second;
    }
    return "";
}

void Config::setDefault(const std::string& key, const ConfigValue& defaultValue) {
    for (auto& schema : pImpl->schemas) {
        if (schema.key == key) {
            schema.defaultValue = defaultValue;
            break;
        }
    }
}

void Config::setRequired(const std::string& key, bool required) {
    for (auto& schema : pImpl->schemas) {
        if (schema.key == key) {
            schema.required = required;
            break;
        }
    }
}

void Config::setValidator(const std::string& key, const ValidationFunction& validator) {
    for (auto& schema : pImpl->schemas) {
        if (schema.key == key) {
            schema.validator = validator;
            break;
        }
    }
}

std::string Config::typeToString(const ConfigValue& value) {
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) return "int";
        else if constexpr (std::is_same_v<T, int64_t>) return "int64_t";
        else if constexpr (std::is_same_v<T, double>) return "float";
        else if constexpr (std::is_same_v<T, bool>) return "bool";
        else if constexpr (std::is_same_v<T, std::string>) return "string";
        else if constexpr (std::is_same_v<T, std::vector<int>>) return "list<int>";
        else if constexpr (std::is_same_v<T, std::vector<double>>) return "list<float>";
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return "list<string>";
        else return "unknown";
    }, value);
}

std::string Config::detectType(const ConfigValue& value) {
    return typeToString(value);
}

bool Config::validateValueType(const std::string& expectedType, const ConfigValue& value) const {
    std::string actualType = typeToString(value);
    return expectedType == actualType;
}

nlohmann::json Config::toJSON(const ConfigValue& value) {
    std::visit([](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) return nlohmann::json(arg);
        else if constexpr (std::is_same_v<T, int64_t>) return nlohmann::json(arg);
        else if constexpr (std::is_same_v<T, double>) return nlohmann::json(arg);
        else if constexpr (std::is_same_v<T, bool>) return nlohmann::json(arg);
        else if constexpr (std::is_same_v<T, std::string>) return nlohmann::json(arg);
        else if constexpr (std::is_same_v<T, std::vector<int>>) return nlohmann::json(arg);
        else if constexpr (std::is_same_v<T, std::vector<double>>) return nlohmann::json(arg);
        else if constexpr (std::is_same_v<T, std::vector<std::string>>) return nlohmann::json(arg);
        else return nlohmann::json();
    }, value);
}

ConfigValue Config::parseJSONValue(const nlohmann::json& jsonValue) {
    if (jsonValue.is_number_integer()) {
        return jsonValue.get<int>();
    } else if (jsonValue.is_number_float()) {
        return jsonValue.get<double>();
    } else if (jsonValue.is_boolean()) {
        return jsonValue.get<bool>();
    } else if (jsonValue.is_string()) {
        return jsonValue.get<std::string>();
    } else if (jsonValue.is_array()) {
        // Check array element type
        if (!jsonValue.empty()) {
            if (jsonValue[0].is_number_integer()) {
                std::vector<int> intArray;
                for (const auto& item : jsonValue) {
                    intArray.push_back(item.get<int>());
                }
                return intArray;
            } else if (jsonValue[0].is_number_float()) {
                std::vector<double> floatArray;
                for (const auto& item : jsonValue) {
                    floatArray.push_back(item.get<double>());
                }
                return floatArray;
            } else if (jsonValue[0].is_string()) {
                std::vector<std::string> stringArray;
                for (const auto& item : jsonValue) {
                    stringArray.push_back(item.get<std::string>());
                }
                return stringArray;
            }
        }
        // Default to empty int array
        return std::vector<int>{};
    }
    return std::string(); // Empty string for unknown types
}

bool Config::loadFromJSONInternal(const nlohmann::json& jsonData, ConfigSource source, const std::string& filepath) {
    if (!jsonData.is_object()) {
        NLM_LOG_ERROR("JSON data is not an object");
        return false;
    }
    
    bool success = true;
    
    for (const auto& item : jsonData.items()) {
        const std::string& key = item.key();
        const nlohmann::json& jsonValue = item.value();
        
        ConfigValue configValue = parseJSONValue(jsonValue);
        set(key, configValue, source);
        
        // Track file path
        if (!filepath.empty()) {
            for (auto& entry : pImpl->entries) {
                if (entry.key == key) {
                    entry.filePath = filepath;
                    break;
                }
            }
        }
    }
    
    return success;
}

nlohmann::json Config::parseJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filepath);
    }
    
    nlohmann::json jsonData;
    file >> jsonData;
    return jsonData;
}

nlohmann::json Config::parseYAMLFile(const std::string& filepath) {
    // For now, delegate to JSON parser
    // In a full implementation, you would use a YAML library like yaml-cpp
    return parseJSONFile(filepath);
}

std::string Config::detectFormat(const std::string& filepath) {
    std::string ext = filepath.substr(filepath.find_last_of(".") + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == "json") return "json";
    if (ext == "yaml" || ext == "yml") return "yaml";
    
    // Try to detect by content
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return "json"; // Default
    }
    
    std::string firstLine;
    std::getline(file, firstLine);
    file.close();
    
    if (firstLine.find("{") != std::string::npos || firstLine.find("[") != std::string::npos) {
        return "json";
    }
    
    return "yaml"; // Default assumption
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