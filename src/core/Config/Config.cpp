#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <map>
#include <set>
#include <stdexcept>
#include <vector>
#include <memory>
#include <typeinfo>
#include <functional>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::string> fileHeaders;  // Comments and metadata
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default configuration presets
    ConfigurationManager::createDefaultPreset();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Detect file type and load accordingly
    std::string fileType = detectFileType(filepath);
    
    if (fileType == "json") {
        return loadFromJSONFile(filepath);
    } else if (fileType == "yaml" || fileType == "yml") {
        return loadFromYAMLFile(filepath);
    } else {
        // Fall back to simple key=value format
        return loadFromSimpleFile(filepath);
    }
}

bool Config::loadFromJSON(const std::string& jsonStr) {
    try {
        auto jsonValue = nlohmann::json::parse(jsonStr);
        return jsonToConfig(jsonValue);
    } catch (const nlohmann::json::parse_error& e) {
        throw ConfigError("JSON parse error: " + std::string(e.what()), ErrorCode::PARSE_ERROR);
    }
}

bool Config::loadFromYAML(const std::string& yamlStr) {
    try {
        auto yamlValue = YAML::Load(yamlStr);
        return yamlToConfig(yamlValue);
    } catch (const YAML::Exception& e) {
        throw ConfigError("YAML parse error: " + std::string(e.what()), ErrorCode::PARSE_ERROR);
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

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Save in JSON format for consistency
    auto jsonValue = configToJSON();
    file << jsonValue.dump(2);
    
    return true;
}

std::string Config::saveToJSON() const {
    auto jsonValue = configToJSON();
    return jsonValue.dump(2);
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

void Config::set(const std::string& key, int value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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
    if (start == std::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool Config::isEmpty(const std::string& str) {
    return str.empty();
}

std::string Config::detectFileType(const std::string& filepath) {
    if (filepath.empty()) {
        return "unknown";
    }
    
    std::string ext = "." + filepath.substr(filepath.find_last_of(".") + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    if (ext == ".json") {
        return "json";
    } else if (ext == ".yaml" || ext == ".yml") {
        return "yaml";
    } else if (ext == ".cfg" || ext == ".ini") {
        return "simple";
    } else {
        return "unknown";
    }
}

bool Config::loadFromSimpleFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError("Could not open file: " + filepath, ErrorCode::FILE_NOT_FOUND);
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

bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError("Could not open JSON file: " + filepath, ErrorCode::FILE_NOT_FOUND);
    }
    
    try {
        auto jsonValue = nlohmann::json::parse(file);
        return jsonToConfig(jsonValue);
    } catch (const nlohmann::json::parse_error& e) {
        throw ConfigError("JSON parse error in file " + filepath + ": " + std::string(e.what()),
                         ErrorCode::PARSE_ERROR);
    }
}

bool Config::loadFromYAMLFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError("Could not open YAML file: " + filepath, ErrorCode::FILE_NOT_FOUND);
    }
    
    try {
        auto yamlValue = YAML::Load(file);
        return yamlToConfig(yamlValue);
    } catch (const YAML::Exception& e) {
        throw ConfigError("YAML parse error in file " + filepath + ": " + std::string(e.what()),
                         ErrorCode::PARSE_ERROR);
    }
}

bool Config::validate(const std::string& schemaName) const {
    try {
        const auto& schema = ConfigurationManager::getInstance().getSchema(schemaName);
        return schema.validate(*this);
    } catch (const ConfigError& e) {
        throw ConfigError("Validation failed for schema '" + schemaName + "': " + e.what(),
                         ErrorCode::VALIDATION_ERROR);
    }
}

std::vector<std::string> Config::getAvailableSchemas() const {
    return ConfigurationManager::getInstance().getSchemaNames();
}

std::string Config::getValidationError(const std::string& schemaName) const {
    try {
        const auto& schema = ConfigurationManager::getInstance().getSchema(schemaName);
        return schema.getValidationErrorMessage(*this);
    } catch (const ConfigError&) {
        return "Schema not found: " + schemaName;
    }
}

std::string Config::applyPreset(const std::string& name) {
    applyConfigurationPreset(*this, name);
    return "Applied preset: " + name;
}

nlohmann::json Config::configToJSON() const {
    nlohmann::json jsonValue = nlohmann::json::object();
    
    for (const auto& entry : pImpl->entries) {
        jsonValue[entry.key] = convertConfigValueToJSON(entry.value);
    }
    
    return jsonValue;
}

bool Config::jsonToConfig(const nlohmann::json& jsonValue) {
    try {
        if (!jsonValue.is_object()) {
            throw ConfigError("JSON root must be an object", ErrorCode::PARSE_ERROR);
        }
        
        for (auto it = jsonValue.begin(); it != jsonValue.end(); ++it) {
            std::string key = it.key();
            ConfigValue configValue = convertJSONToConfigValue(*it);
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const ConfigError&) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigError("Error parsing JSON config: " + std::string(e.what()),
                         ErrorCode::PARSE_ERROR);
    }
}

YAML::Node Config::configToYAML() const {
    YAML::Node yamlNode = YAML::Node(YAML::NodeType::Map);
    
    for (const auto& entry : pImpl->entries) {
        yamlNode[entry.key] = convertConfigValueToYAML(entry.value);
    }
    
    return yamlNode;
}

bool Config::yamlToConfig(const YAML::Node& yamlValue) {
    try {
        if (!yamlValue.IsMap()) {
            throw ConfigError("YAML root must be a map", ErrorCode::PARSE_ERROR);
        }
        
        for (auto it = yamlValue.begin(); it != yamlValue.end(); ++it) {
            std::string key = it->first.as<std::string>();
            ConfigValue configValue = convertYAMLToConfigValue(*it);
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const ConfigError&) {
        throw;
    } catch (const std::exception& e) {
        throw ConfigError("Error parsing YAML config: " + std::string(e.what()),
                         ErrorCode::PARSE_ERROR);
    }
}

nlohmann::json Config::convertConfigValueToJSON(const ConfigValue& value) {
    nlohmann::json result;
    
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            result = arg;
        }
    }, value);
    
    return result;
}

ConfigValue Config::convertJSONToConfigValue(const nlohmann::json& jsonValue) {
    ConfigValue result;
    
    if (jsonValue.is_number_integer()) {
        // Try int64_t first (covers int), then int
        try {
            result = jsonValue.get<int64_t>();
        } catch (...) {
            result = jsonValue.get<int>();
        }
    } else if (jsonValue.is_number_float()) {
        result = jsonValue.get<double>();
    } else if (jsonValue.is_boolean()) {
        result = jsonValue.get<bool>();
    } else if (jsonValue.is_string()) {
        result = jsonValue.get<std::string>();
    } else if (jsonValue.is_array()) {
        // Try to determine array type
        if (!jsonValue.empty()) {
            bool allInts = true;
            bool allDoubles = true;
            bool allStrings = true;
            
            for (const auto& item : jsonValue) {
                if (!item.is_number_integer()) allInts = false;
                if (!item.is_number_float()) allDoubles = false;
                if (!item.is_string()) allStrings = false;
            }
            
            if (allInts) {
                std::vector<int> intList;
                for (const auto& item : jsonValue) {
                    intList.push_back(item.get<int>());
                }
                result = intList;
            } else if (allDoubles) {
                std::vector<double> doubleList;
                for (const auto& item : jsonValue) {
                    doubleList.push_back(item.get<double>());
                }
                result = doubleList;
            } else if (allStrings) {
                std::vector<std::string> stringList;
                for (const auto& item : jsonValue) {
                    stringList.push_back(item.get<std::string>());
                }
                result = stringList;
            } else {
                // Mixed array - store as vector of doubles
                std::vector<double> mixedList;
                for (const auto& item : jsonValue) {
                    if (item.is_number()) {
                        mixedList.push_back(item.get<double>());
                    } else {
                        mixedList.push_back(std::stod(item.get<std::string>()));
                    }
                }
                result = mixedList;
            }
        }
    }
    
    return result;
}

YAML::Node Config::convertConfigValueToYAML(const ConfigValue& value) {
    YAML::Node result;
    
    std::visit([&result](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            result = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& item : arg) {
                result.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : arg) {
                result.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                result.push_back(item);
            }
        }
    }, value);
    
    return result;
}

ConfigValue Config::convertYAMLToConfigValue(const YAML::Node& yamlValue) {
    ConfigValue result;
    
    if (yamlValue.IsScalar()) {
        std::string scalarType = yamlValue.Scalar();
        
        // Try to determine type based on content
        try {
            // Try int64_t first
            result = yamlValue.as<int64_t>();
        } catch (...) {
            try {
                // Try double
                result = yamlValue.as<double>();
            } catch (...) {
                try {
                    // Try bool
                    result = yamlValue.as<bool>();
                } catch (...) {
                    // Fall back to string
                    result = yamlValue.as<std::string>();
                }
            }
        }
    } else if (yamlValue.IsSequence()) {
        // Try to determine sequence type
        if (!yamlValue.empty()) {
            bool allInts = true;
            bool allDoubles = true;
            bool allStrings = true;
            
            for (const auto& item : yamlValue) {
                try {
                    auto temp = item.as<int64_t>();
                } catch (...) { allInts = false; }
                try {
                    auto temp = item.as<double>();
                } catch (...) { allDoubles = false; }
                try {
                    auto temp = item.as<std::string>();
                } catch (...) { allStrings = false; }
            }
            
            if (allInts) {
                std::vector<int64_t> intList;
                for (const auto& item : yamlValue) {
                    intList.push_back(item.as<int64_t>());
                }
                // Convert to appropriate type
                bool allWithinIntRange = true;
                for (int64_t val : intList) {
                    if (val < std::numeric_limits<int>::min() || 
                        val > std::numeric_limits<int>::max()) {
                        allWithinIntRange = false;
                        break;
                    }
                }
                if (allWithinIntRange) {
                    std::vector<int> finalList;
                    for (int64_t val : intList) {
                        finalList.push_back(static_cast<int>(val));
                    }
                    result = finalList;
                } else {
                    result = intList;
                }
            } else if (allDoubles) {
                std::vector<double> doubleList;
                for (const auto& item : yamlValue) {
                    doubleList.push_back(item.as<double>());
                }
                result = doubleList;
            } else if (allStrings) {
                std::vector<std::string> stringList;
                for (const auto& item : yamlValue) {
                    stringList.push_back(item.as<std::string>());
                }
                result = stringList;
            } else {
                // Mixed sequence - store as vector of doubles
                std::vector<double> mixedList;
                for (const auto& item : yamlValue) {
                    try {
                        mixedList.push_back(item.as<double>());
                    } catch (...) {
                        mixedList.push_back(std::stod(item.as<std::string>()));
                    }
                }
                result = mixedList;
            }
        }
    }
    
    return result;
}

void Config::createPreset(const std::string& name, const ConfigSchema& schema) {
    ConfigurationManager::getInstance().registerSchema(name, schema);
}

std::string Config::errorCodeToString(ErrorCode code) {
    switch (code) {
        case ErrorCode::UNKNOWN: return "UNKNOWN";
        case ErrorCode::FILE_NOT_FOUND: return "FILE_NOT_FOUND";
        case ErrorCode::PARSE_ERROR: return "PARSE_ERROR";
        case ErrorCode::VALIDATION_ERROR: return "VALIDATION_ERROR";
        case ErrorCode::INVALID_TYPE: return "INVALID_TYPE";
        case ErrorCode::MISSING_KEY: return "MISSING_KEY";
        case ErrorCode::INVALID_VALUE: return "INVALID_VALUE";
        default: return "UNKNOWN";
    }
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