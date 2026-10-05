#include "Config.hpp"
#include "../Logger/Logger.hpp"
#include <nlohmann/json.hpp>
#include <yaml-cpp/yaml.h>
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <map>
#include <mutex>
#include <chrono>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::mutex mutex;
    bool isValid{true};
    std::vector<std::string> validationErrors;
    std::chrono::system_clock::time_point lastModified;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->lastModified = std::chrono::system_clock::now();
}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {
    if (pImpl) {
        pImpl->isValid = true;
    }
}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        if (pImpl) {
            pImpl->isValid = true;
        }
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    // Try to detect file format by extension
    std::string extension = "." + getFileExtension(filepath);
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".json") {
        return loadFromJSON(filepath);
    } else if (extension == ".yaml" || extension == ".yml") {
        return loadFromYAML(filepath);
    } else {
        // Fallback to simple key=value format for backward compatibility
        NLM_LOG_WARNING("Config file " + filepath + " has unknown extension, using simple format");
        return loadFromSimpleFormat(filepath);
    }
}

bool Config::loadFromJSON(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open JSON config file: " + filepath);
        return false;
    }
    
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        parseJSONConfig(jsonData, ConfigSource::File);
        NLM_LOG_INFO("Successfully loaded JSON config from: " + filepath);
        return true;
    } catch (const nlohmann::json::exception& e) {
        NLM_LOG_ERROR("JSON parsing error in file " + filepath + ": " + std::string(e.what()));
        pImpl->isValid = false;
        pImpl->validationErrors.push_back("JSON parsing error: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing JSON config file " + filepath + ": " + std::string(e.what()));
        pImpl->isValid = false;
        pImpl->validationErrors.push_back("General error: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYAML(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open YAML config file: " + filepath);
        return false;
    }
    
    try {
        YAML::Node yamlData = YAML::LoadFile(filepath);
        parseYAMLConfig(yamlData, ConfigSource::File);
        NLM_LOG_INFO("Successfully loaded YAML config from: " + filepath);
        return true;
    } catch (const YAML::Exception& e) {
        NLM_LOG_ERROR("YAML parsing error in file " + filepath + ": " + std::string(e.what()));
        pImpl->isValid = false;
        pImpl->validationErrors.push_back("YAML parsing error: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error parsing YAML config file " + filepath + ": " + std::string(e.what()));
        pImpl->isValid = false;
        pImpl->validationErrors.push_back("General error: " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        ++lineNumber;
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
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_ERROR("Config file " + filepath + ": Empty key at line " + std::to_string(lineNumber));
                success = false;
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate value length
            if (value.size() > 10000) {
                NLM_LOG_WARNING("Config file " + filepath + ": Long value at line " + std::to_string(lineNumber) + 
                              " (" + std::to_string(value.size()) + " characters)");
            }
            
            set(key, value, ConfigSource::File);
        } else {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + " in " + filepath + ": " + line);
            success = false;
        }
    }
    
    if (!success) {
        pImpl->isValid = false;
        NLM_LOG_ERROR("Config file " + filepath + " had parsing errors");
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        NLM_LOG_WARNING("No command line arguments provided for configuration");
        return true;
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
                    NLM_LOG_WARNING("Empty key in command line argument: " + arg);
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_INFO("Loaded command line config: " + key + " = " + (value.size() > 50 ? value.substr(0, 47) + "..." : value));
            } else {
                NLM_LOG_WARNING("Invalid command line argument format: " + arg);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            if (key.empty()) {
                NLM_LOG_WARNING("Empty key in command line argument: " + arg);
                continue;
            }
            
            set(key, value, ConfigSource::CommandLine);
            NLM_LOG_INFO("Loaded command line config: " + key + " = " + (value.size() > 50 ? value.substr(0, 47) + "..." : value));
        } else {
            NLM_LOG_WARNING("Unrecognized command line argument: " + arg);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file for writing: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration File\n";
    file << "# Generated on " << getCurrentTimestamp() << "\n";
    file << "# Valid: " << (isValid ? "Yes" : "No") << "\n\n";
    
    if (!pImpl->validationErrors.empty()) {
        file << "# VALIDATION ERRORS:\n";
        for (const auto& error : pImpl->validationErrors) {
            file << "#   " << error << "\n";
        }
        file << "# END VALIDATION ERRORS\n\n";
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << "# Source: " << static_cast<int>(entry.source) << "\n";
        
        // Write in simple format for compatibility
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << '\"' << arg << '\"';
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
    }
    
    NLM_LOG_INFO("Successfully saved config to: " + filepath);
    return true;
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    if (key.empty()) {
        throw std::invalid_argument("Config key cannot be empty");
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        T value = std::get<T>(it->value);
        
        // Validate the value based on type
        if (!validateValue<T>(value, key)) {
            NLM_LOG_WARNING("Validation failed for config key: " + key);
            if (!pImpl->isValid) {
                pImpl->validationErrors.push_back("Validation failed for key: " + key);
            }
        }
        
        return value;
    } catch (const std::bad_variant_access&) {
        NLM_LOG_WARNING("Type mismatch for config key: " + key);
        if (!pImpl->isValid) {
            pImpl->validationErrors.push_back("Type mismatch for key: " + key);
        }
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    if (key.empty()) {
        throw std::invalid_argument("Config key cannot be empty");
    }
    
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    if (key.empty()) {
        throw std::invalid_argument("Config key cannot be empty");
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    // Validate the value before setting
    if (!validateValue<ConfigValue>(value, key)) {
        throw std::invalid_argument("Invalid value for config key: " + key);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Updated config key: " + key);
    } else {
        pImpl->entries.emplace_back(key, value, source);
        NLM_LOG_DEBUG("Added new config key: " + key);
    }
    
    pImpl->lastModified = std::chrono::system_clock::now();
    pImpl->isValid = true; // Reset validity flag
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
    if (key.empty()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    if (key.empty()) {
        throw std::invalid_argument("Config key cannot be empty");
    }
    
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        pImpl->entries.erase(it);
        NLM_LOG_DEBUG("Removed config key: " + key);
    }
}

std::vector<std::string> Config::getKeys() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

void Config::clear() {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    pImpl->entries.clear();
    pImpl->isValid = true;
    pImpl->validationErrors.clear();
    NLM_LOG_DEBUG("Cleared all config entries");
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "Valid: " << (isValid ? "Yes" : "No") << "\n";
    
    if (!pImpl->validationErrors.empty()) {
        oss << "Validation Errors:\n";
        for (const auto& error : pImpl->validationErrors) {
            oss << "  - " << error << "\n";
        }
        oss << "\n";
    }
    
    oss << "Last Modified: " << getCurrentTimestamp() << "\n\n";
    
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
    
    NLM_LOG_INFO("Config summary generated: " + std::to_string(pImpl->entries.size()) + " entries");
    return oss.str();
}

std::vector<std::string> Config::validate() const {
    std::lock_guard<std::mutex> lock(pImpl->mutex);
    
    std::vector<std::string> errors;
    
    // Validate overall config state
    if (!isValid) {
        errors.push_back("Configuration marked as invalid");
    }
    
    // Check for duplicate keys
    std::map<std::string, int> keyCount;
    for (const auto& entry : pImpl->entries) {
        keyCount[entry.key]++;
    }
    
    for (const auto& [key, count] : keyCount) {
        if (count > 1) {
            errors.push_back("Duplicate config key: " + key);
        }
    }
    
    // Validate each entry
    for (const auto& entry : pImpl->entries) {
        if (entry.key.empty()) {
            errors.push_back("Empty config key");
        }
        
        // Validate vector values
        if (std::holds_alternative<std::vector<int>>(entry.value)) {
            const auto& vec = std::get<std::vector<int>>(entry.value);
            if (vec.empty()) {
                errors.push_back("Empty integer vector for key: " + entry.key);
            }
        } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
            const auto& vec = std::get<std::vector<double>>(entry.value);
            if (vec.empty()) {
                errors.push_back("Empty double vector for key: " + entry.key);
            }
        } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
            const auto& vec = std::get<std::vector<std::string>>(entry.value);
            if (vec.empty()) {
                errors.push_back("Empty string vector for key: " + entry.key);
            }
        }
    }
    
    return errors;
}

void Config::parseJSONConfig(const nlohmann::json& json, ConfigSource source) {
    if (!json.is_object()) {
        NLM_LOG_WARNING("JSON config root is not an object");
        pImpl->isValid = false;
        pImpl->validationErrors.push_back("JSON config root is not an object");
        return;
    }
    
    for (auto it = json.begin(); it != json.end(); ++it) {
        std::string key = it.key();
        const nlohmann::json& value = it.value();
        
        ConfigValue configValue;
        if (value.is_string()) {
            configValue = value.get<std::string>();
        } else if (value.is_number_integer()) {
            configValue = value.get<int>();
        } else if (value.is_number_float()) {
            configValue = value.get<double>();
        } else if (value.is_boolean()) {
            configValue = value.get<bool>();
        } else if (value.is_array()) {
            // Try to infer array type
            if (!value.empty()) {
                if (value[0].is_number_integer()) {
                    std::vector<int> intArray;
                    intArray.reserve(value.size());
                    for (const auto& item : value) {
                        intArray.push_back(item.get<int>());
                    }
                    configValue = intArray;
                } else if (value[0].is_number_float()) {
                    std::vector<double> doubleArray;
                    doubleArray.reserve(value.size());
                    for (const auto& item : value) {
                        doubleArray.push_back(item.get<double>());
                    }
                    configValue = doubleArray;
                } else if (value[0].is_string()) {
                    std::vector<std::string> stringArray;
                    stringArray.reserve(value.size());
                    for (const auto& item : value) {
                        stringArray.push_back(item.get<std::string>());
                    }
                    configValue = stringArray;
                } else {
                    NLM_LOG_WARNING("Unsupported JSON array type for key: " + key);
                    pImpl->isValid = false;
                    pImpl->validationErrors.push_back("Unsupported JSON array type for key: " + key);
                    continue;
                }
            } else {
                // Empty array - default to string vector
                configValue = std::vector<std::string>();
            }
        } else {
            NLM_LOG_WARNING("Unsupported JSON type for key: " + key);
            pImpl->isValid = false;
            pImpl->validationErrors.push_back("Unsupported JSON type for key: " + key);
            continue;
        }
        
        try {
            set(key, configValue, source);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error setting config key " + key + ": " + std::string(e.what()));
            pImpl->isValid = false;
            pImpl->validationErrors.push_back("Error setting key " + key + ": " + std::string(e.what()));
        }
    }
}

void Config::parseYAMLConfig(const YAML::Node& yaml, ConfigSource source) {
    if (!yaml.IsMap()) {
        NLM_LOG_WARNING("YAML config root is not a map");
        pImpl->isValid = false;
        pImpl->validationErrors.push_back("YAML config root is not a map");
        return;
    }
    
    for (const auto& it : yaml) {
        std::string key = it.first.as<std::string>();
        const YAML::Node& value = it.second;
        
        ConfigValue configValue;
        if (value.IsScalar()) {
            // Try to infer scalar type
            if (value.IsInteger()) {
                configValue = value.as<int>();
            } else if (value.IsFloat()) {
                configValue = value.as<double>();
            } else if (value.IsBoolean()) {
                configValue = value.as<bool>();
            } else {
                configValue = value.as<std::string>();
            }
        } else if (value.IsSequence()) {
            // Try to infer sequence type
            if (!value.empty()) {
                if (value[0].IsInteger()) {
                    std::vector<int> intArray;
                    intArray.reserve(value.size());
                    for (const auto& item : value) {
                        intArray.push_back(item.as<int>());
                    }
                    configValue = intArray;
                } else if (value[0].IsFloat()) {
                    std::vector<double> doubleArray;
                    doubleArray.reserve(value.size());
                    for (const auto& item : value) {
                        doubleArray.push_back(item.as<double>());
                    }
                    configValue = doubleArray;
                } else {
                    std::vector<std::string> stringArray;
                    stringArray.reserve(value.size());
                    for (const auto& item : value) {
                        stringArray.push_back(item.as<std::string>());
                    }
                    configValue = stringArray;
                }
            } else {
                // Empty sequence - default to string vector
                configValue = std::vector<std::string>();
            }
        } else {
            NLM_LOG_WARNING("Unsupported YAML type for key: " + key);
            pImpl->isValid = false;
            pImpl->validationErrors.push_back("Unsupported YAML type for key: " + key);
            continue;
        }
        
        try {
            set(key, configValue, source);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Error setting config key " + key + ": " + std::string(e.what()));
            pImpl->isValid = false;
            pImpl->validationErrors.push_back("Error setting key " + key + ": " + std::string(e.what()));
        }
    }
}

void Config::parseSimpleConfig(const std::string& content, ConfigSource source) {
    std::istringstream stream(content);
    std::string line;
    int lineNumber = 0;
    bool success = true;
    
    while (std::getline(stream, line)) {
        ++lineNumber;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs (handles quotes, escapes, etc.)
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_ERROR("Empty config key at line " + std::to_string(lineNumber));
                success = false;
                continue;
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '\"' && value.back() == '\"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Validate value length
            if (value.size() > 10000) {
                NLM_LOG_WARNING("Long config value at line " + std::to_string(lineNumber) + 
                              " (" + std::to_string(value.size()) + " characters)");
            }
            
            // Handle escape sequences
            replaceEscapes(value);
            
            try {
                set(key, value, source);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error parsing config at line " + std::to_string(lineNumber) + ": " + std::string(e.what()));
                success = false;
            }
        } else {
            NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNumber) + ": " + line);
            success = false;
        }
    }
    
    if (!success) {
        pImpl->isValid = false;
        NLM_LOG_ERROR("Config parsing had errors");
    }
}

void Config::replaceEscapes(std::string& str) {
    // Simple escape sequence handling
    size_t pos = 0;
    while ((pos = str.find('\\', pos)) != std::string::npos) {
        if (pos + 1 < str.size()) {
            char escaped = str[pos + 1];
            switch (escaped) {
                case 'n': str.replace(pos, 2, "\n"); break;
                case 't': str.replace(pos, 2, "\t"); break;
                case 'r': str.replace(pos, 2, "\r"); break;
                case '\\': str.replace(pos, 2, "\\"); break;
                case '\"': str.replace(pos, 2, "\""); break;
                default: pos += 2; continue;
            }
        } else {
            break;
        }
    }
}

std::string Config::getFileExtension(const std::string& filename) {
    size_t pos = filename.find_last_of('.');
    if (pos == std::string::npos) {
        return "";
    }
    return filename.substr(pos + 1);
}

// Template specializations for validation
template<typename T>
bool Config::validateValue(const T& value, const std::string& key) {
    // Basic validation for common types
    if (std::is_same_v<T, std::string>) {
        return !std::any_of(value.begin(), value.end(), 
                           [](char c) { return c == '\0' || c == '\n' || c == '\r'; });
    } else if (std::is_same_v<T, int>) {
        return value >= INT_MIN && value <= INT_MAX;
    } else if (std::is_same_v<T, double>) {
        return !std::isnan(value) && !std::isinf(value);
    } else if (std::is_same_v<T, bool>) {
        return true; // Always valid
    }
    return true; // Unknown type - assume valid
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