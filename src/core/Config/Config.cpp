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
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    bool success = false;
    
    if (extension == "json" || extension == "JSON" || extension == "json5") {
        success = loadFromJSON(filepath);
    } else if (extension == "yaml" || extension == "yml" || extension == "YAML" || extension == "YML") {
        success = loadFromYAML(filepath);
    } else {
        // Fallback to simple key=value format for backward compatibility
        success = loadFromSimpleFormat(filepath);
    }
    
    // Validate configuration after loading
    if (success) {
        validateConfiguration();
    }
    
    return success;
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
    
    // Determine file format from extension
    std::string extension = "";
    size_t dotPos = filepath.find_last_of('.');
    if (dotPos != std::string::npos) {
        extension = filepath.substr(dotPos + 1);
    }
    
    if (extension == "json" || extension == "JSON" || extension == "json5") {
        return saveToJSON(filepath);
    } else if (extension == "yaml" || extension == "yml" || extension == "YAML" || extension == "YML") {
        return saveToYAML(filepath);
    } else {
        // Default to simple key=value format for backward compatibility
        return saveToSimpleFormat(filepath);
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
bool Config::loadFromSimpleFormat(const std::string& filepath) {
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
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromJSON(const std::string& filepath) {
#ifdef HAVE_NLOHMANN_JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            parseJSONValue(key, *it, ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error in " + filepath + ": " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_ERROR("JSON parsing not available - nlohmann-json library not found");
    return false;
#endif
}

bool Config::loadFromYAML(const std::string& filepath) {
#ifdef HAVE_YAML_CPP
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        
        for (auto it = yamlData.begin(); it != yamlData.end(); ++it) {
            std::string key = it->first.as<std::string>();
            parseJSONValue(key, nlohmann::json(*it), ConfigSource::File);
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("YAML parsing error in " + filepath + ": " + std::string(e.what()));
        return false;
    }
#else
    NLM_LOG_ERROR("YAML parsing not available - yaml-cpp library not found");
    return false;
#endif
}

bool Config::validateConfiguration() {
    // Validate required keys
    std::vector<std::string> requiredKeys = {
        "random_seed",
        "simulation_timestep",
        "neuron_count",
        "region_count"
    };
    
    if (!validateRequiredKeys(requiredKeys)) {
        NLM_LOG_WARNING("Configuration missing some recommended keys");
    }
    
    // Validate individual keys
    for (const auto& entry : pImpl->entries) {
        if (!validateKeyFormat(entry.key)) {
            NLM_LOG_WARNING("Invalid key format: " + entry.key);
        }
        
        if (!validateValueRanges(entry)) {
            NLM_LOG_WARNING("Invalid value range for key: " + entry.key);
        }
    }
    
    return true;
}

void Config::parseJSONValue(const std::string& key, const nlohmann::json& jsonValue, ConfigSource source) {
    ConfigValue configValue;
    
    if (jsonToConfigValue(jsonValue, configValue)) {
        set(key, configValue, source);
    } else {
        NLM_LOG_WARNING("Could not parse JSON value for key: " + key);
    }
}

bool Config::jsonToConfigValue(const nlohmann::json& jsonValue, ConfigValue& configValue) {
    if (jsonValue.is_number_integer()) {
        // Try int64_t first, then int
        try {
            configValue = jsonValue.get<int64_t>();
            return true;
        } catch (...) {
            try {
                configValue = jsonValue.get<int>();
                return true;
            } catch (...) {
                return false;
            }
        }
    } else if (jsonValue.is_number_float()) {
        configValue = jsonValue.get<double>();
        return true;
    } else if (jsonValue.is_boolean()) {
        configValue = jsonValue.get<bool>();
        return true;
    } else if (jsonValue.is_string()) {
        configValue = jsonValue.get<std::string>();
        return true;
    } else if (jsonValue.is_array()) {
        // Try to infer array type
        if (!jsonValue.empty()) {
            bool allInt = std::all_of(jsonValue.begin(), jsonValue.end(), 
                                    [](const nlohmann::json& v) { return v.is_number_integer(); });
            if (allInt) {
                std::vector<int64_t> intArray;
                for (const auto& v : jsonValue) {
                    intArray.push_back(v.get<int64_t>());
                }
                configValue = intArray;
                return true;
            }
            
            bool allFloat = std::all_of(jsonValue.begin(), jsonValue.end(), 
                                      [](const nlohmann::json& v) { return v.is_number_float(); });
            if (allFloat) {
                std::vector<double> floatArray;
                for (const auto& v : jsonValue) {
                    floatArray.push_back(v.get<double>());
                }
                configValue = floatArray;
                return true;
            }
            
            bool allString = std::all_of(jsonValue.begin(), jsonValue.end(), 
                                        [](const nlohmann::json& v) { return v.is_string(); });
            if (allString) {
                std::vector<std::string> stringArray;
                for (const auto& v : jsonValue) {
                    stringArray.push_back(v.get<std::string>());
                }
                configValue = stringArray;
                return true;
            }
        }
    }
    
    return false;
}

bool Config::yamlToConfigValue(const YAML::Node& yamlValue, ConfigValue& configValue) {
    if (yamlValue.IsScalar()) {
        if (yamlValue.IsInt()) {
            configValue = yamlValue.as<int64_t>();
            return true;
        } else if (yamlValue.IsFloat()) {
            configValue = yamlValue.as<double>();
            return true;
        } else if (yamlValue.IsBool()) {
            configValue = yamlValue.as<bool>();
            return true;
        } else if (yamlValue.IsSequence()) {
            // Check array type
            std::vector<int64_t> intArray;
            std::vector<double> floatArray;
            std::vector<std::string> stringArray;
            
            bool isInt = true, isFloat = true, isString = true;
            
            for (const auto& item : yamlValue) {
                if (item.IsInt()) {
                    intArray.push_back(item.as<int64_t>());
                } else if (item.IsFloat()) {
                    isInt = false;
                    floatArray.push_back(item.as<double>());
                } else if (item.IsScalar()) {
                    std::string str = item.as<std::string>();
                    // Check if it's numeric
                    try {
                        int64_t intVal = std::stoll(str);
                        floatArray.push_back(static_cast<double>(intVal));
                        isInt = false;
                        isString = false;
                    } catch (...) {
                        try {
                            double floatVal = std::stod(str);
                            floatArray.push_back(floatVal);
                            isInt = false;
                            isString = false;
                        } catch (...) {
                            stringArray.push_back(str);
                            isInt = false;
                            isFloat = false;
                        }
                    }
                } else {
                    isInt = isFloat = isString = false;
                    break;
                }
            }
            
            if (isInt && !intArray.empty()) {
                configValue = intArray;
                return true;
            } else if (isFloat && !floatArray.empty()) {
                configValue = floatArray;
                return true;
            } else if (isString && !stringArray.empty()) {
                configValue = stringArray;
                return true;
            }
        }
    }
    
    return false;
}

bool Config::validateRequiredKeys(const std::vector<std::string>& requiredKeys) {
    bool allFound = true;
    for (const std::string& key : requiredKeys) {
        if (!has(key)) {
            NLM_LOG_WARNING("Missing required configuration key: " + key);
            allFound = false;
        }
    }
    return allFound;
}

bool Config::validateKeyFormat(const std::string& key) {
    // Check for valid key characters
    if (key.empty()) {
        return false;
    }
    
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '-' && c != '.') {
            return false;
        }
    }
    
    return true;
}

bool Config::validateValueRanges(const ConfigEntry& entry) {
    // Validate specific ranges for known keys
    if (entry.key == "random_seed") {
        auto value = std::get_if<int64_t>(&entry.value);
        if (value) {
            return *value >= 0 && *value < UINT64_MAX;
        }
    } else if (entry.key == "simulation_timestep") {
        auto value = std::get_if<double>(&entry.value);
        if (value) {
            return *value > 0.0 && *value < 1.0; // Reasonable timestep range
        }
    } else if (entry.key == "neuron_count") {
        auto value = std::get_if<int64_t>(&entry.value);
        if (value) {
            return *value > 0 && *value <= 1000000; // Reasonable neuron count
        }
    } else if (entry.key == "region_count") {
        auto value = std::get_if<int64_t>(&entry.value);
        if (value) {
            return *value > 0 && *value <= 1000; // Reasonable region count
        }
    } else if (entry.key == "connection_probability") {
        auto value = std::get_if<double>(&entry.value);
        if (value) {
            return *value >= 0.0 && *value <= 1.0; // Probability range
        }
    }
    
    return true; // Unknown keys are considered valid
}

std::string Config::jsonEscape(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\b': result += "\\b"; break;
            case '\f': result += "\\f"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            case '\t': result += "\\t"; break;
            default:
                if (static_cast<unsigned char>(c) < 32) {
                    result += "\\u" + std::to_string(static_cast<int>(c));
                } else {
                    result += c;
                }
        }
    }
    return result;
}

std::string Config::doubleToString(double value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string Config::intToString(int64_t value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string Config::boolToString(bool value) {
    return value ? "true" : "false";
}

// Define helper functions for template instantiations
void Config::defineHelperFunctions() {
    // Define helper functions for template instantiations
}