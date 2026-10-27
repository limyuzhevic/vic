#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <cctype>

// Include JSON/YAML/TOML parsers if available
#ifdef HAS_JSON
#include "nlohmann/json.hpp"
#endif

#ifdef HAS_YAML
#include "yaml-cpp/yaml.h"
#endif

#ifdef HAS_TOML
#include "toml.hpp"
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    ConfigSchema schema;
};

Config::Config() : pImpl(std::make_unique<Impl>()), schema_() {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::string ext = getFileExtension(filepath);
    
    if (ext == ".json") {
        return loadFromJSON(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        return loadFromYAML(filepath);
    } else if (ext == ".toml") {
        return loadFromTOML(filepath);
    } else {
        // Fallback to simple key=value format
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
#ifdef HAS_JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError(ConfigError::ErrorCode::FILE_NOT_FOUND, 
                         "Cannot open JSON file: " + filepath);
    }
    
    try {
        auto json = nlohmann::json::parse(file);
        
        for (auto it = json.begin(); it != json.end(); ++it) {
            std::string key = it.key();
            
            // Convert JSON value to ConfigValue
            ConfigValue value;
            if (it->is_string()) {
                value = it->get<std::string>();
            } else if (it->is_number_integer()) {
                value = it->get<int64_t>();
            } else if (it->is_number_unsigned()) {
                value = static_cast<int64_t>(it->get<uint64_t>());
            } else if (it->is_number_float()) {
                value = it->get<double>();
            } else if (it->is_boolean()) {
                value = it->get<bool>();
            } else if (it->is_array()) {
                // Handle arrays
                if (it->front().is_string()) {
                    std::vector<std::string> strVec;
                    for (const auto& item : *it) {
                        strVec.push_back(item.get<std::string>());
                    }
                    value = strVec;
                } else if (it->front().is_number_integer()) {
                    std::vector<int64_t> intVec;
                    for (const auto& item : *it) {
                        intVec.push_back(item.get<int64_t>());
                    }
                    value = intVec;
                } else if (it->front().is_number_float()) {
                    std::vector<double> doubleVec;
                    for (const auto& item : *it) {
                        doubleVec.push_back(item.get<double>());
                    }
                    value = doubleVec;
                } else {
                    throw ConfigError(ConfigError::ErrorCode::INVALID_FORMAT,
                                     "Unsupported array type in JSON for key: " + key);
                }
            } else {
                throw ConfigError(ConfigError::ErrorCode::INVALID_FORMAT,
                                 "Unsupported JSON value type for key: " + key);
            }
            
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        throw ConfigError(ConfigError::ErrorCode::PARSING_ERROR,
                         "JSON parsing error: " + std::string(e.what()));
    }
#else
    throw ConfigError(ConfigError::ErrorCode::INVALID_FORMAT,
                     "JSON support not compiled in. Rebuild with nlohmann/json.");
#endif
}

bool Config::loadFromYAML(const std::string& filepath) {
#ifdef HAS_YAML
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError(ConfigError::ErrorCode::FILE_NOT_FOUND,
                         "Cannot open YAML file: " + filepath);
    }
    
    try {
        YAML::Node yaml = YAML::LoadFile(filepath);
        
        for (const auto& pair : yaml) {
            std::string key = pair.first.as<std::string>();
            
            // Convert YAML value to ConfigValue
            ConfigValue value;
            YAML::Node node = pair.second;
            
            if (node.IsScalar()) {
                if (node.Tag() == "!!bool") {
                    value = node.as<bool>();
                } else if (node.Tag() == "!!int" || node.Tag() == "!!integer") {
                    value = node.as<int64_t>();
                } else if (node.Tag() == "!!float" || node.Tag() == "!!double") {
                    value = node.as<double>();
                } else {
                    value = node.as<std::string>();
                }
            } else if (node.IsSequence()) {
                // Handle sequences (arrays)
                if (!node.empty() && node[0].IsScalar()) {
                    if (node[0].Tag() == "!!bool") {
                        std::vector<bool> boolVec;
                        for (const auto& item : node) {
                            boolVec.push_back(item.as<bool>());
                        }
                        value = boolVec;
                    } else if (node[0].Tag() == "!!int" || node[0].Tag() == "!!integer") {
                        std::vector<int64_t> intVec;
                        for (const auto& item : node) {
                            intVec.push_back(item.as<int64_t>());
                        }
                        value = intVec;
                    } else if (node[0].Tag() == "!!float" || node[0].Tag() == "!!double") {
                        std::vector<double> doubleVec;
                        for (const auto& item : node) {
                            doubleVec.push_back(item.as<double>());
                        }
                        value = doubleVec;
                    } else {
                        std::vector<std::string> strVec;
                        for (const auto& item : node) {
                            strVec.push_back(item.as<std::string>());
                        }
                        value = strVec;
                    }
                }
            }
            
            set(key, value, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        throw ConfigError(ConfigError::ErrorCode::PARSING_ERROR,
                         "YAML parsing error: " + std::string(e.what()));
    }
#else
    throw ConfigError(ConfigError::ErrorCode::INVALID_FORMAT,
                     "YAML support not compiled in. Rebuild with yaml-cpp.");
#endif
}

bool Config::loadFromTOML(const std::string& filepath) {
#ifdef HAS_TOML
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError(ConfigError::ErrorCode::FILE_NOT_FOUND,
                         "Cannot open TOML file: " + filepath);
    }
    
    try {
        auto toml = toml::parse_file(filepath);
        
        for (auto& [key, value] : toml) {
            ConfigValue configValue;
            
            if (std::holds_alternative<toml::value<int>>(value)) {
                configValue = value.as<int64_t>();
            } else if (std::holds_alternative<toml::value<double>>(value)) {
                configValue = value.as<double>();
            } else if (std::holds_alternative<toml::value<bool>>(value)) {
                configValue = value.as<bool>();
            } else if (std::holds_alternative<toml::value<std::string>>(value)) {
                configValue = value.as<std::string>();
            } else if (std::holds_alternative<toml::value<toml::array>>(value)) {
                // Handle arrays
                auto& arr = std::get<toml::value<toml::array>>(value).get();
                std::vector<std::string> strVec;
                bool allStrings = true;
                bool allIntegers = true;
                bool allFloats = true;
                
                for (size_t i = 0; i < arr.size(); ++i) {
                    if (!arr[i].is<std::string>()) {
                        allStrings = false;
                    }
                    if (!arr[i].is<int64_t>()) {
                        allIntegers = false;
                    }
                    if (!arr[i].is<double>()) {
                        allFloats = false;
                    }
                }
                
                if (allStrings) {
                    std::vector<std::string> vec;
                    for (size_t i = 0; i < arr.size(); ++i) {
                        vec.push_back(arr[i].as<std::string>());
                    }
                    configValue = vec;
                } else if (allIntegers) {
                    std::vector<int64_t> vec;
                    for (size_t i = 0; i < arr.size(); ++i) {
                        vec.push_back(arr[i].as<int64_t>());
                    }
                    configValue = vec;
                } else if (allFloats) {
                    std::vector<double> vec;
                    for (size_t i = 0; i < arr.size(); ++i) {
                        vec.push_back(arr[i].as<double>());
                    }
                    configValue = vec;
                } else {
                    throw ConfigError(ConfigError::ErrorCode::INVALID_FORMAT,
                                     "Unsupported TOML array type for key: " + key.str());
                }
            }
            
            set(key.str(), configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        throw ConfigError(ConfigError::ErrorCode::PARSING_ERROR,
                         "TOML parsing error: " + std::string(e.what()));
    }
#else
    throw ConfigError(ConfigError::ErrorCode::INVALID_FORMAT,
                     "TOML support not compiled in. Rebuild with toml11.");
#endif
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        
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
        } else {
            throw ConfigError(ConfigError::ErrorCode::PARSING_ERROR,
                             "Invalid line in config file " + filepath + ": " +
                             std::to_string(lineNumber) + ": \"" + line + "\"");
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
                
                try {
                    setFromString(key, value, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    throw ConfigError(ConfigError::ErrorCode::PARSING_ERROR,
                                     "Failed to parse command line argument --" + key + "=" + value + ": " +
                                     std::string(e.what()));
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            try {
                setFromString(key, value, ConfigSource::CommandLine);
            } catch (const std::exception& e) {
                throw ConfigError(ConfigError::ErrorCode::PARSING_ERROR,
                                 "Failed to parse command line argument -" + key + " " + value + ": " +
                                 std::string(e.what()));
            }
        }
    }
    return true;
}

void Config::loadFromEnvironment(const std::unordered_map<std::string, std::string>& envVars) {
    // Get all environment variables if not provided
    std::unordered_map<std::string, std::string> actualEnvVars = envVars;
    if (actualEnvVars.empty()) {
        // Get all environment variables
        for (char** env = environ; *env != nullptr; ++env) {
            std::string var(*env);
            size_t pos = var.find('=');
            if (pos != std::string::npos && pos > 0) {
                std::string key = var.substr(0, pos);
                std::string value = var.substr(pos + 1);
                actualEnvVars[key] = value;
            }
        }
    }
    
    // Process environment variables
    for (const auto& pair : actualEnvVars) {
        std::string key = pair.first;
        std::string value = pair.second;
        
        // Convert environment variable name to config key (e.g., NLM_NEURON_COUNT -> neuron_count)
        std::transform(key.begin(), key.end(), key.begin(), ::tolower);
        std::replace(key.begin(), key.end(), '_', '-');
        
        // Parse and set the value
        try {
            setFromString(key, value, ConfigSource::EnvironmentVariable);
        } catch (const std::exception& e) {
            // Environment variables may contain invalid values, warn but continue
            NLM_LOG_WARNING("Invalid environment variable " + pair.first + ": " + e.what());
        }
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File" << std::endl;
    file << "# Generated on " << getCurrentTimestamp() << std::endl;
    file << "# Format: key = value" << std::endl;
    file << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert ConfigValue to string
        std::visit([&file](const auto& val) {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << val[i] << "\"";
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << val[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << val[i];
                }
                file << "]";
            } else {
                file << val;
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
        if constexpr (std::is_same_v<T, int>) {
            return std::get<int>(it->value);
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return std::get<int64_t>(it->value);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::get<double>(it->value);
        } else if constexpr (std::is_same_v<T, bool>) {
            return std::get<bool>(it->value);
        } else if constexpr (std::is_same_v<T, std::string>) {
            return std::get<std::string>(it->value);
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            return std::get<std::vector<int64_t>>(it->value);
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return std::get<std::vector<double>>(it->value);
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return std::get<std::vector<std::string>>(it->value);
        } else {
            static_assert(!std::is_same_v<T, T>, "Unsupported type for get()");
            return std::nullopt;
        }
    } catch (const std::bad_variant_access&) {
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

template<typename T>
void Config::set(const std::string& key, const T& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Try to determine the appropriate type by attempting conversions
    bool success = false;
    
    // Try int first (more specific)
    if (!success) {
        int intVal;
        if (parseStringToValue(valueStr, intVal)) {
            set(key, intVal, source);
            success = true;
        }
    }
    
    // Try int64_t
    if (!success) {
        int64_t int64Val;
        if (parseStringToValue(valueStr, int64Val)) {
            set(key, int64Val, source);
            success = true;
        }
    }
    
    // Try double
    if (!success) {
        double doubleVal;
        if (parseStringToValue(valueStr, doubleVal)) {
            set(key, doubleVal, source);
            success = true;
        }
    }
    
    // Try bool
    if (!success) {
        bool boolVal;
        if (parseStringToValue(valueStr, boolVal)) {
            set(key, boolVal, source);
            success = true;
        }
    }
    
    // Try string (fallback)
    if (!success) {
        set(key, valueStr, source);
        success = true;
    }
    
    if (!success) {
        throw std::runtime_error("Cannot parse value: \"" + valueStr + "\"");
    }
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

std::optional<ConfigValue> Config::getAny(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    return it->value;
}

void Config::clear() {
    pImpl->entries.clear();
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
    return oss.str();
}

std::vector<std::string> Config::validate(const ConfigSchema& schema) const {
    return schema.validate(*this);
}

template<typename T>
bool Config::parseStringToValue(const std::string& str, T& outValue) {
    std::istringstream iss(str);
    iss >> std::skipws >> outValue;
    
    // Check if entire string was consumed
    char remaining;
    if (iss >> remaining) {
        return false;  // Extra characters
    }
    
    return !iss.fail();
}

template<typename T>
std::string Config::valueToString(const T& value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string Config::getFileExtension(const std::string& filepath) {
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos == std::string::npos) return "";
    
    std::string ext = filepath.substr(dotPos + 1);
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

void Config::setSchema(const ConfigSchema& schema) {
    pImpl->schema = schema;
}

bool Config::hasSchema() const {
    return !pImpl->schema.entries.empty();
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
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
#define REGISTER_TYPE(T) \
    template std::optional<T> Config::get<T>(const std::string&) const; \
    template T Config::getOr<T>(const std::string&, const T&) const; \
    template void Config::set<T>(const std::string&, const T&, ConfigSource);

REGISTER_TYPE(int)
REGISTER_TYPE(int64_t)
REGISTER_TYPE(double)
REGISTER_TYPE(bool)
REGISTER_TYPE(std::string)
REGISTER_TYPE(std::vector<int64_t>)
REGISTER_TYPE(std::vector<double>)
REGISTER_TYPE(std::vector<std::string>)

#undef REGISTER_TYPE

} // namespace nlm
