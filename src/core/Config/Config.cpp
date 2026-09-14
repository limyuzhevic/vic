#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <stdexcept>
#include <system_error>
#include <memory>
#include <cmath>

// Include nlohmann/json for JSON parsing
#include <nlohmann/json.hpp>

// Include yaml-cpp for YAML parsing  
#include <yaml-cpp/yaml.h>

namespace nlm {

namespace {

// Helper function to get current timestamp as string
std::string getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

// Helper function to determine file extension
std::string getFileExtension(const std::string& filename) {
    size_t pos = filename.find_last_of(".");
    if (pos == std::string::npos) return "";
    return filename.substr(pos);
}

// Convert string to lowercase
std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Trim whitespace from both ends
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

// Parse simple key=value format (legacy support)
void parseSimpleFormat(const std::string& content, std::vector<ConfigEntry>& entries) {
    std::istringstream stream(content);
    std::string line;
    
    while (std::getline(stream, line)) {
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
            
            // Parse the value based on its content
            ConfigValue parsedValue;
            bool parsed = false;
            
            // Try to parse as bool
            if (!parsed) {
                if (value == "true" || value == "1" || value == "yes") {
                    parsedValue = true;
                    parsed = true;
                } else if (value == "false" || value == "0" || value == "no") {
                    parsedValue = false;
                    parsed = true;
                }
            }
            
            // Try to parse as int
            if (!parsed) {
                try {
                    size_t pos;
                    long long intVal = std::stoll(value, &pos);
                    if (pos == value.size()) {
                        parsedValue = static_cast<int>(intVal);
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try to parse as int64_t
            if (!parsed) {
                try {
                    size_t pos;
                    long long intVal = std::stoll(value, &pos);
                    if (pos == value.size()) {
                        parsedValue = intVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Try to parse as double
            if (!parsed) {
                try {
                    size_t pos;
                    double doubleVal = std::stod(value, &pos);
                    if (pos == value.size()) {
                        parsedValue = doubleVal;
                        parsed = true;
                    }
                } catch (...) {}
            }
            
            // Use string as fallback
            if (!parsed) {
                parsedValue = value;
            }
            
            entries.emplace_back(key, parsedValue, ConfigSource::File);
        }
    }
}

// Convert ConfigValue to JSON value
nlohmann::json configValueToJson(const ConfigValue& value) {
    nlohmann::json jsonValue;
    
    std::visit([&jsonValue](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        
        if constexpr (std::is_same_v<T, int>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            for (const auto& item : arg) {
                jsonValue.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            for (const auto& item : arg) {
                jsonValue.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            for (const auto& item : arg) {
                jsonValue.push_back(item);
            }
        } else if constexpr (std::is_same_v<T, std::vector<std::vector<int>>>) {
            for (const auto& innerVec : arg) {
                nlohmann::json innerJson;
                for (const auto& item : innerVec) {
                    innerJson.push_back(item);
                }
                jsonValue.push_back(innerJson);
            }
        } else if constexpr (std::is_same_v<T, std::map<std::string, int>>) {
            for (const auto& pair : arg) {
                jsonValue[pair.first] = pair.second;
            }
        } else if constexpr (std::is_same_v<T, std::map<std::string, double>>) {
            for (const auto& pair : arg) {
                jsonValue[pair.first] = pair.second;
            }
        } else if constexpr (std::is_same_v<T, std::map<std::string, bool>>) {
            for (const auto& pair : arg) {
                jsonValue[pair.first] = pair.second;
            }
        } else if constexpr (std::is_same_v<T, std::map<std::string, std::string>>) {
            for (const auto& pair : arg) {
                jsonValue[pair.first] = pair.second;
            }
        }
    }, value);
    
    return jsonValue;
}

// Convert JSON value to ConfigValue
ConfigValue jsonToConfigValue(const nlohmann::json& jsonValue) {
    ConfigValue configValue;
    
    if (jsonValue.is_boolean()) {
        configValue = jsonValue.get<bool>();
    } else if (jsonValue.is_number_integer()) {
        configValue = jsonValue.get<int>();
    } else if (jsonValue.is_number_unsigned()) {
        configValue = jsonValue.get<uint64_t>();
    } else if (jsonValue.is_number()) {
        configValue = jsonValue.get<double>();
    } else if (jsonValue.is_string()) {
        configValue = jsonValue.get<std::string>();
    } else if (jsonValue.is_array()) {
        if (jsonValue.empty()) {
            configValue = std::vector<int>();
        } else {
            // Try to determine the array type
            if (jsonValue[0].is_boolean()) {
                std::vector<bool> vec;
                for (const auto& item : jsonValue) {
                    vec.push_back(item.get<bool>());
                }
                configValue = vec;
            } else if (jsonValue[0].is_number_integer()) {
                std::vector<int> vec;
                for (const auto& item : jsonValue) {
                    vec.push_back(item.get<int>());
                }
                configValue = vec;
            } else if (jsonValue[0].is_number()) {
                std::vector<double> vec;
                for (const auto& item : jsonValue) {
                    vec.push_back(item.get<double>());
                }
                configValue = vec;
            } else if (jsonValue[0].is_string()) {
                std::vector<std::string> vec;
                for (const auto& item : jsonValue) {
                    vec.push_back(item.get<std::string>());
                }
                configValue = vec;
            } else if (jsonValue[0].is_array()) {
                std::vector<std::vector<int>> vec;
                for (const auto& item : jsonValue) {
                    std::vector<int> innerVec;
                    for (const auto& innerItem : item) {
                        innerVec.push_back(innerItem.get<int>());
                    }
                    vec.push_back(innerVec);
                }
                configValue = vec;
            } else {
                configValue = std::vector<int>();
            }
        }
    } else if (jsonValue.is_object()) {
        if (jsonValue.empty()) {
            configValue = std::map<std::string, int>();
        } else {
            // Try to determine the map type
            bool allInt = true;
            bool allDouble = true;
            bool allBool = true;
            bool allString = true;
            
            for (const auto& item : jsonValue.items()) {
                if (!item.value().is_number_integer()) allInt = false;
                if (!item.value().is_number()) allDouble = false;
                if (!item.value().is_boolean()) allBool = false;
                if (!item.value().is_string()) allString = false;
            }
            
            if (allInt) {
                std::map<std::string, int> map;
                for (const auto& item : jsonValue.items()) {
                    map[item.key()] = item.value().get<int>();
                }
                configValue = map;
            } else if (allDouble) {
                std::map<std::string, double> map;
                for (const auto& item : jsonValue.items()) {
                    map[item.key()] = item.value().get<double>();
                }
                configValue = map;
            } else if (allBool) {
                std::map<std::string, bool> map;
                for (const auto& item : jsonValue.items()) {
                    map[item.key()] = item.value().get<bool>();
                }
                configValue = map;
            } else if (allString) {
                std::map<std::string, std::string> map;
                for (const auto& item : jsonValue.items()) {
                    map[item.key()] = item.value().get<std::string>();
                }
                configValue = map;
            } else {
                configValue = std::map<std::string, int>();
            }
        }
    } else {
        configValue = std::string();
    }
    
    return configValue;
}

// Convert Config to JSON object
nlohmann::json configToJson(const Config& config, bool detailed = true) {
    nlohmann::json jsonConfig;
    
    // Add metadata
    jsonConfig["metadata"]["source"] = "config_file";
    jsonConfig["metadata"]["timestamp"] = getCurrentTimestamp();
    jsonConfig["metadata"]["version"] = "1.0";
    
    if (detailed) {
        // Add all entries with descriptions
        for (const auto& entry : config.pImpl->entries) {
            jsonConfig[entry.key]["value"] = configValueToJson(entry.value);
            jsonConfig[entry.key]["source"] = static_cast<int>(entry.source);
            jsonConfig[entry.key]["description"] = entry.description;
        }
    } else {
        // Add summary (just values)
        for (const auto& entry : config.pImpl->entries) {
            jsonConfig[entry.key] = configValueToJson(entry.value);
        }
    }
    
    return jsonConfig;
}

// Parse JSON content into Config
bool parseJsonContent(const std::string& content, Config& config) {
    try {
        auto jsonDoc = nlohmann::json::parse(content);
        
        // Check if this is a config file (has metadata or key-value pairs)
        if (jsonDoc.contains("metadata") || jsonDoc.contains("version")) {
            // Extract entries from JSON
            for (auto it = jsonDoc.begin(); it != jsonDoc.end(); ++it) {
                std::string key = it.key();
                
                // Skip metadata section
                if (key == "metadata") continue;
                
                // Check if entry has value and source
                if (it->contains("value") && it->contains("source")) {
                    ConfigValue value = jsonToConfigValue(it->at("value"));
                    ConfigSource source = static_cast<ConfigSource>(it->at("source").get<int>());
                    std::string description = it->value("description", "");
                    
                    config.set(key, value, source);
                } else {
                    // Just a value
                    ConfigValue value = jsonToConfigValue(*it);
                    config.set(key, value, ConfigSource::File);
                }
            }
        } else {
            // Simple key-value JSON object
            for (auto it = jsonDoc.begin(); it != jsonDoc.end(); ++it) {
                std::string key = it.key();
                ConfigValue value = jsonToConfigValue(*it);
                config.set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        // Fallback to simple format if JSON parsing fails
        parseSimpleFormat(content, config.pImpl->entries);
        return false;
    }
}

// Parse YAML content into Config
bool parseYamlContent(const std::string& content, Config& config) {
    try {
        auto yamlDoc = YAML::Load(content);
        
        // Check if this is a config file (has metadata or key-value pairs)
        if (yamlDoc["metadata"] || yamlDoc["version"] || yamlDoc.IsMap()) {
            // Extract entries from YAML
            if (yamlDoc["metadata"]) {
                auto metadata = yamlDoc["metadata"];
                // Skip metadata section
            }
            
            for (const auto& item : yamlDoc) {
                std::string key = item.first.as<std::string>();
                
                // Skip metadata section
                if (key == "metadata") continue;
                
                // Check if entry has value and source
                if (item.second.IsMap() && item.second["value"] && item.second["source"]) {
                    // Parse value
                    nlohmann::json jsonValue;
                    if (item.second["value"].IsBool()) {
                        jsonValue = item.second["value"].as<bool>();
                    } else if (item.second["value"].IsInt()) {
                        jsonValue = item.second["value"].as<int>();
                    } else if (item.second["value"].IsFloat()) {
                        jsonValue = item.second["value"].as<double>();
                    } else {
                        jsonValue = item.second["value"].as<std::string>();
                    }
                    
                    ConfigValue value = jsonToConfigValue(jsonValue);
                    ConfigSource source = static_cast<ConfigSource>(item.second["source"].as<int>());
                    std::string description = item.second["description"] ? item.second["description"].as<std::string>() : "";
                    
                    config.set(key, value, source);
                } else {
                    // Just a value
                    nlohmann::json jsonValue;
                    if (item.second.IsBool()) {
                        jsonValue = item.second.as<bool>();
                    } else if (item.second.IsInt()) {
                        jsonValue = item.second.as<int>();
                    } else if (item.second.IsFloat()) {
                        jsonValue = item.second.as<double>();
                    } else {
                        jsonValue = item.second.as<std::string>();
                    }
                    
                    ConfigValue value = jsonToConfigValue(jsonValue);
                    config.set(key, value, ConfigSource::File);
                }
            }
        } else {
            // Simple key-value YAML object
            for (const auto& item : yamlDoc) {
                std::string key = item.first.as<std::string>();
                nlohmann::json jsonValue;
                if (item.second.IsBool()) {
                    jsonValue = item.second.as<bool>();
                } else if (item.second.IsInt()) {
                    jsonValue = item.second.as<int>();
                } else if (item.second.IsFloat()) {
                    jsonValue = item.second.as<double>();
                } else {
                    jsonValue = item.second.as<std::string>();
                }
                
                ConfigValue value = jsonToConfigValue(jsonValue);
                config.set(key, value, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const YAML::Exception& e) {
        // Fallback to simple format if YAML parsing fails
        parseSimpleFormat(content, config.pImpl->entries);
        return false;
    }
}

} // anonymous namespace

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::string sourceFile;
    bool isValid;
    std::string validationError;
    std::map<std::string, std::string> requiredFields;
    std::map<std::string, std::function<bool(const ConfigValue&)>> typeValidators;
    std::map<std::string, std::pair<double, double>> rangeValidators;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->isValid = false;
    pImpl->validationError = "";
    
    // Register default type validators
    pImpl->typeValidators["port"] = [](const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) {
            int port = std::get<int>(value);
            return port > 0 && port <= 65535;
        }
        return false;
    };
    
    pImpl->rangeValidators["temperature"] = std::make_pair(-50.0, 150.0);
    pImpl->rangeValidators["pressure"] = std::make_pair(0.0, 1000.0);
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->validationError = "Cannot open file: " + filepath;
        nlm::ErrorHandler::getInstance().logError(
            nlm::ErrorCode::FileIOError,
            "Failed to open config file: " + filepath,
            __FILE__, __LINE__, "Config::loadFromFile",
            nlm::LogLevel::Error,
            -1, -1, -1,
            0, 0.0
        );
        return false;
    }
    
    // Read entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    pImpl->sourceFile = filepath;
    
    // Check file size for potential memory issues
    if (content.size() > 10 * 1024 * 1024) { // 10MB limit
        nlm::ErrorHandler::getInstance().logError(
            nlm::ErrorCode::ResourceExhausted,
            "Config file too large: " + std::to_string(content.size()) + " bytes",
            __FILE__, __LINE__, "Config::loadFromFile",
            nlm::LogLevel::Warning,
            -1, -1, -1,
            0, 0.0
        );
    }
    
    // Clear existing entries
    clear();
    
    // Determine file type and parse accordingly
    std::string ext = getFileExtension(filepath);
    bool success = false;
    
    if (toLower(ext) == ".json" || toLower(ext) == ".json5") {
        success = parseJsonContent(content, *this);
        if (!success) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::ParseError,
                "JSON parsing failed for config file: " + filepath,
                __FILE__, __LINE__, "Config::loadFromFile",
                nlm::LogLevel::Warning,
                -1, -1, -1,
                0, 0.0
            );
        }
    } else if (toLower(ext) == ".yaml" || toLower(ext) == ".yml") {
        success = parseYamlContent(content, *this);
        if (!success) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::ParseError,
                "YAML parsing failed for config file: " + filepath,
                __FILE__, __LINE__, "Config::loadFromFile",
                nlm::LogLevel::Warning,
                -1, -1, -1,
                0, 0.0
            );
        }
    } else {
        // Unknown file type, use simple format (legacy support)
        try {
            parseSimpleFormat(content, pImpl->entries);
            success = false; // Always false for unknown types
        } catch (const std::exception& e) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::ParseError,
                "Failed to parse config file (unknown format): " + std::string(e.what()),
                __FILE__, __LINE__, "Config::loadFromFile",
                nlm::LogLevel::Error,
                -1, -1, -1,
                0, 0.0
            );
            return false;
        }
    }
    
    // Validate configuration
    pImpl->isValid = isValid();
    
    if (!pImpl->isValid) {
        nlm::ErrorHandler::getInstance().logError(
            nlm::ErrorCode::ValidationError,
            "Config validation failed: " + pImpl->validationError,
            __FILE__, __LINE__, "Config::loadFromFile",
            nlm::LogLevel::Warning,
            -1, -1, -1,
            0, 0.0
        );
    }
    
    return success;
}

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        pImpl->validationError = "Cannot open file: " + filepath;
        nlm::ErrorHandler::getInstance().logError(
            nlm::ErrorCode::FileIOError,
            "Failed to open config file: " + filepath,
            __FILE__, __LINE__, "Config::loadFromFile",
            nlm::LogLevel::Error,
            -1, -1, -1,
            0, 0.0
        );
        return false;
    }
    
    // Read entire file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    
    pImpl->sourceFile = filepath;
    
    // Check file size for potential memory issues
    if (content.size() > 10 * 1024 * 1024) { // 10MB limit
        nlm::ErrorHandler::getInstance().logError(
            nlm::ErrorCode::ResourceExhausted,
            "Config file too large: " + std::to_string(content.size()) + " bytes",
            __FILE__, __LINE__, "Config::loadFromFile",
            nlm::LogLevel::Warning,
            -1, -1, -1,
            0, 0.0
        );
    }
    
    // Clear existing entries
    clear();
    
    // Determine file type and parse accordingly
    std::string ext = getFileExtension(filepath);
    bool success = false;
    
    if (toLower(ext) == ".json" || toLower(ext) == ".json5") {
        success = parseJsonContent(content, *this);
        if (!success) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::ParseError,
                "JSON parsing failed for config file: " + filepath,
                __FILE__, __LINE__, "Config::loadFromFile",
                nlm::LogLevel::Warning,
                -1, -1, -1,
                0, 0.0
            );
        }
    } else if (toLower(ext) == ".yaml" || toLower(ext) == ".yml") {
        success = parseYamlContent(content, *this);
        if (!success) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::ParseError,
                "YAML parsing failed for config file: " + filepath,
                __FILE__, __LINE__, "Config::loadFromFile",
                nlm::LogLevel::Warning,
                -1, -1, -1,
                0, 0.0
            );
        }
    } else {
        // Unknown file type, use simple format (legacy support)
        try {
            parseSimpleFormat(content, pImpl->entries);
            success = false; // Always false for unknown types
        } catch (const std::exception& e) {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::ParseError,
                "Failed to parse config file (unknown format): " + std::string(e.what()),
                __FILE__, __LINE__, "Config::loadFromFile",
                nlm::LogLevel::Error,
                -1, -1, -1,
                0, 0.0
            );
            return false;
        }
    }
    
    // Validate configuration
    pImpl->isValid = isValid();
    
    if (!pImpl->isValid) {
        nlm::ErrorHandler::getInstance().logError(
            nlm::ErrorCode::ValidationError,
            "Config validation failed: " + pImpl->validationError,
            __FILE__, __LINE__, "Config::loadFromFile",
            nlm::LogLevel::Warning,
            -1, -1, -1,
            0, 0.0
        );
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    
    // Validate input arguments
    if (!argv) {
        nlm::ErrorHandler::getInstance().logError(
            nlm::ErrorCode::InvalidParameter,
            "Invalid command line arguments: argv is null",
            __FILE__, __LINE__, "Config::loadFromArgs",
            nlm::LogLevel::Error,
            -1, -1, -1,
            0, 0.0
        );
        return false;
    }
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key and value
                if (key.empty()) {
                    nlm::ErrorHandler::getInstance().logError(
                        nlm::ErrorCode::InvalidParameter,
                        "Empty key in command line argument",
                        __FILE__, __LINE__, "Config::loadFromArgs",
                        nlm::LogLevel::Warning,
                        -1, -1, -1,
                        0, 0.0
                    );
                    success = false;
                    continue;
                }
                
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (...) {
                    nlm::ErrorHandler::getInstance().logError(
                        nlm::ErrorCode::ParseError,
                        "Failed to parse command line argument: " + arg,
                        __FILE__, __LINE__, "Config::loadFromArgs",
                        nlm::LogLevel::Warning,
                        -1, -1, -1,
                        0, 0.0
                    );
                    success = false;
                }
            } else {
                nlm::ErrorHandler::getInstance().logError(
                    nlm::ErrorCode::InvalidParameter,
                    "Invalid command line argument format (missing '='): " + arg,
                    __FILE__, __LINE__, "Config::loadFromArgs",
                    nlm::LogLevel::Warning,
                    -1, -1, -1,
                    0, 0.0
                );
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key and value
            if (key.empty()) {
                nlm::ErrorHandler::getInstance().logError(
                    nlm::ErrorCode::InvalidParameter,
                    "Empty key in command line argument",
                    __FILE__, __LINE__, "Config::loadFromArgs",
                    nlm::LogLevel::Warning,
                    -1, -1, -1,
                    0, 0.0
                );
                success = false;
                continue;
            }
            
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (...) {
                nlm::ErrorHandler::getInstance().logError(
                    nlm::ErrorCode::ParseError,
                    "Failed to parse command line argument: -" + key + " " + value,
                    __FILE__, __LINE__, "Config::loadFromArgs",
                    nlm::LogLevel::Warning,
                    -1, -1, -1,
                    0, 0.0
                );
                success = false;
            }
        } else {
            nlm::ErrorHandler::getInstance().logError(
                nlm::ErrorCode::InvalidParameter,
                "Invalid command line argument format: " + arg,
                __FILE__, __LINE__, "Config::loadFromArgs",
                nlm::LogLevel::Warning,
                -1, -1, -1,
                0, 0.0
            );
            success = false;
        }
    }
    
    // Validate configuration
    pImpl->isValid = isValid();
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Create JSON object with detailed configuration
    auto jsonConfig = configToJson(*this, true);
    
    // Write to file with indentation
    file << jsonConfig.dump(4) << std::endl;
    
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
    
    // Update validation status
    pImpl->isValid = isValid();
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
    
    // Update validation status
    pImpl->isValid = isValid();
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
    pImpl->isValid = true; // Empty config is valid
}

bool Config::isValid() const {
    // Check required fields
    for (const auto& required : pImpl->requiredFields) {
        if (!has(required.first)) {
            pImpl->validationError = "Required field not found: " + required.first;
            return false;
        }
    }
    
    // Check all entries
    for (const auto& entry : pImpl->entries) {
        // Check that the key is not empty
        if (entry.key.empty()) {
            pImpl->validationError = "Empty key found";
            return false;
        }
        
        // Check type validation
        auto typeIt = pImpl->typeValidators.find(entry.key);
        if (typeIt != pImpl->typeValidators.end()) {
            if (!typeIt->second(entry.value)) {
                pImpl->validationError = "Type validation failed for key: " + entry.key;
                return false;
            }
        }
        
        // Check range validation
        auto rangeIt = pImpl->rangeValidators.find(entry.key);
        if (rangeIt != pImpl->rangeValidators.end()) {
            if (std::holds_alternative<double>(entry.value)) {
                double val = std::get<double>(entry.value);
                if (val < rangeIt->second.first || val > rangeIt->second.second) {
                    pImpl->validationError = "Range validation failed for key: " + entry.key;
                    return false;
                }
            } else if (std::holds_alternative<int>(entry.value)) {
                double val = static_cast<double>(std::get<int>(entry.value));
                if (val < rangeIt->second.first || val > rangeIt->second.second) {
                    pImpl->validationError = "Range validation failed for key: " + entry.key;
                    return false;
                }
            }
        }
        
        // Check that the value can be accessed (basic validation)
        try {
            std::visit([](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                // Basic type checking
                if constexpr (std::is_same_v<T, int>) {
                    // int is always valid
                } else if constexpr (std::is_same_v<T, double>) {
                    // double is always valid
                } else if constexpr (std::is_same_v<T, bool>) {
                    // bool is always valid
                } else if constexpr (std::is_same_v<T, std::string>) {
                    // string is always valid
                } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                    // vector is always valid
                } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                    // vector is always valid
                } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                    // vector is always valid
                } else if constexpr (std::is_same_v<T, std::vector<std::vector<int>>>) {
                    // vector is always valid
                } else if constexpr (std::is_same_v<T, std::map<std::string, int>>) {
                    // map is always valid
                } else if constexpr (std::is_same_v<T, std::map<std::string, double>>) {
                    // map is always valid
                } else if constexpr (std::is_same_v<T, std::map<std::string, bool>>) {
                    // map is always valid
                } else if constexpr (std::is_same_v<T, std::map<std::string, std::string>>) {
                    // map is always valid
                }
            }, entry.value);
        } catch (...) {
            pImpl->validationError = "Invalid value type for key: " + entry.key;
            return false;
        }
    }
    
    pImpl->validationError = "";
    return true;
}

std::string Config::toString() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
    oss << "Source file: " << pImpl->sourceFile << "\n";
    oss << "Valid: " << (pImpl->isValid ? "Yes" : "No") << "\n";
    if (!pImpl->validationError.empty()) {
        oss << "Error: " << pImpl->validationError << "\n";
    }
    
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

void Config::addRequiredField(const std::string& fieldName) {
    pImpl->requiredFields[fieldName] = "";
    pImpl->isValid = isValid();
}

void Config::addTypeValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator) {
    pImpl->typeValidators[key] = validator;
    pImpl->isValid = isValid();
}

void Config::addRangeValidator(const std::string& key, double min, double max) {
    pImpl->rangeValidators[key] = std::make_pair(min, max);
    pImpl->isValid = isValid();
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
