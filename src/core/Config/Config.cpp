// Configuration Parser Implementation
// Includes JSON support, enhanced error handling, and better validation

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <json.hpp> // nlohmann/json library

using json = nlohmann::json;

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool validateRequiredKeys;  // Whether to validate required configuration keys
    std::set<std::string> requiredKeys;  // Keys that must be present
    bool jsonEnabled;  // Whether JSON parsing is enabled
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->validateRequiredKeys = false;
    pImpl->jsonEnabled = true;  // Default to JSON parsing
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

// Enhanced configuration loading with JSON support
bool Config::loadFromFile(const std::string& filepath) {
    // Check if file exists
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_WARNING("Configuration file not found: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open configuration file: " + filepath);
        return false;
    }
    
    try {
        // Try JSON parsing first
        if (pImpl->jsonEnabled) {
            return loadFromJSONFile(filepath);
        }
        
        // Fall back to simple key=value parsing for backward compatibility
        return loadFromLegacyFile(filepath);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error loading configuration from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromJSONFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        json j;
        file >> j;
        
        // Clear existing entries
        clear();
        
        // Parse JSON object
        if (j.is_object()) {
            for (auto& [key, value] : j.items()) {
                ConfigValue configValue;
                ConfigSource source = ConfigSource::File;
                
                // Convert JSON value to ConfigValue
                if (value.is_string()) {
                    configValue = value.get<std::string>();
                } else if (value.is_number_integer()) {
                    configValue = value.get<int64_t>();
                } else if (value.is_number_unsigned()) {
                    configValue = value.get<uint64_t>();
                } else if (value.is_number_float()) {
                    configValue = value.get<double>();
                } else if (value.is_boolean()) {
                    configValue = value.get<bool>();
                } else if (value.is_array()) {
                    // Handle arrays - convert to appropriate type
                    auto arr = value.get<std::vector<json>>();
                    if (!arr.empty()) {
                        if (arr[0].is_string()) {
                            std::vector<std::string> strArray;
                            for (const auto& item : arr) {
                                strArray.push_back(item.get<std::string>());
                            }
                            configValue = strArray;
                        } else if (arr[0].is_number_integer()) {
                            std::vector<int64_t> intArray;
                            for (const auto& item : arr) {
                                intArray.push_back(item.get<int64_t>());
                            }
                            configValue = intArray;
                        } else if (arr[0].is_number_float()) {
                            std::vector<double> doubleArray;
                            for (const auto& item : arr) {
                                doubleArray.push_back(item.get<double>());
                            }
                            configValue = doubleArray;
                        } else {
                            NLM_LOG_WARNING("Skipping array with unsupported element type in key: " + key);
                            continue;
                        }
                    }
                } else {
                    NLM_LOG_WARNING("Skipping configuration key with unsupported type: " + key);
                    continue;
                }
                
                set(key, configValue, source);
            }
        }
        
        NLM_LOG_INFO("Successfully loaded JSON configuration from: " + filepath);
        return true;
        
    } catch (const json::exception& e) {
        NLM_LOG_ERROR("JSON parsing error in " + filepath + ": " + std::string(e.what()));
        // Fall back to legacy parsing
        return loadFromLegacyFile(filepath);
    }
}

bool Config::loadFromLegacyFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    int lineNumber = 0;
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
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const std::exception& e) {
                NLM_LOG_WARNING("Skipping invalid configuration line " + std::to_string(lineNumber) + ": " + e.what());
                continue;
            }
        } else {
            NLM_LOG_WARNING("Skipping malformed configuration line " + std::to_string(lineNumber) + ": " + line);
        }
    }
    
    NLM_LOG_INFO("Loaded legacy configuration from: " + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) {
        NLM_LOG_DEBUG("No command line arguments provided");
        return true;  // No arguments is valid
    }
    
    bool success = true;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                if (!setFromString(key, value, ConfigSource::CommandLine)) {
                    NLM_LOG_ERROR("Failed to parse command line argument: " + arg);
                    success = false;
                }
            } else {
                NLM_LOG_ERROR("Malformed command line argument (missing =): " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            if (!setFromString(key, value, ConfigSource::CommandLine)) {
                NLM_LOG_ERROR("Failed to parse command line argument: " + arg + " " + value);
                success = false;
            }
        } else {
            NLM_LOG_WARNING("Unrecognized command line argument: " + arg);
            // Continue processing other arguments
        }
    }
    
    if (success) {
        NLM_LOG_INFO("Successfully parsed command line arguments");
    } else {
        NLM_LOG_WARNING("Some command line arguments were invalid or could not be parsed");
    }
    
    return success;
}

// Enhanced configuration saving with JSON support
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Cannot open configuration file for writing: " + filepath);
        return false;
    }
    
    try {
        // Create JSON object
        json j;
        
        for (const auto& entry : pImpl->entries) {
            // Convert ConfigValue to JSON
            if (std::holds_alternative<int>(entry.value)) {
                j[entry.key] = std::get<int>(entry.value);
            } else if (std::holds_alternative<int64_t>(entry.value)) {
                j[entry.key] = std::get<int64_t>(entry.value);
            } else if (std::holds_alternative<double>(entry.value)) {
                j[entry.key] = std::get<double>(entry.value);
            } else if (std::holds_alternative<bool>(entry.value)) {
                j[entry.key] = std::get<bool>(entry.value);
            } else if (std::holds_alternative<std::string>(entry.value)) {
                j[entry.key] = std::get<std::string>(entry.value);
            } else if (std::holds_alternative<std::vector<int>>(entry.value)) {
                j[entry.key] = std::get<std::vector<int>>(entry.value);
            } else if (std::holds_alternative<std::vector<double>>(entry.value)) {
                j[entry.key] = std::get<std::vector<double>>(entry.value);
            } else if (std::holds_alternative<std::vector<std::string>>(entry.value)) {
                j[entry.key] = std::get<std::vector<std::string>>(entry.value);
            } else {
                NLM_LOG_WARNING("Skipping configuration key with unsupported type for saving: " + entry.key);
            }
        }
        
        // Write JSON to file with pretty formatting
        file << j.dump(2);
        
        NLM_LOG_INFO("Configuration saved to: " + filepath + " (" + std::to_string(pImpl->entries.size()) + " entries)");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error saving configuration to " + filepath + ": " + std::string(e.what()));
        return false;
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
        NLM_LOG_WARNING("Type mismatch for configuration key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    } else {
        // Validate if this is a required key
        if (pImpl->validateRequiredKeys && pImpl->requiredKeys.count(key) > 0) {
            NLM_LOG_WARNING("Required configuration key missing: " + key + ", using default value");
        }
        return defaultValue;
    }
}

// Enhanced setter with validation and error handling
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    // Remove any leading/trailing whitespace from key
    std::string trimmedKey = trim(key);
    if (trimmedKey.empty()) {
        throw std::invalid_argument("Configuration key cannot be only whitespace");
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&trimmedKey](const ConfigEntry& e) { return e.key == trimmedKey; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        NLM_LOG_DEBUG("Updated configuration key: " + trimmedKey);
    } else {
        pImpl->entries.emplace_back(trimmedKey, value, source);
        NLM_LOG_DEBUG("Added new configuration key: " + trimmedKey);
    }
}

// Convenience setters with validation
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

// Enhanced setter from string with parsing
bool Config::setFromString(const std::string& key, const std::string& valueStr, ConfigSource source) {
    try {
        // Try to determine type based on content
        if (valueStr.empty()) {
            set(key, std::string(), source);
            return true;
        }
        
        // Check if it's a boolean
        std::string lowerValue = toLower(valueStr);
        if (lowerValue == "true" || lowerValue == "false") {
            set(key, lowerValue == "true", source);
            return true;
        }
        
        // Check if it's an integer
        try {
            size_t pos;
            int intValue = std::stoi(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, intValue, source);
                return true;
            }
        } catch (...) {}
        
        // Check if it's a double
        try {
            size_t pos;
            double doubleValue = std::stod(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, doubleValue, source);
                return true;
            }
        } catch (...) {}
        
        // Check if it's an integer64
        try {
            size_t pos;
            int64_t int64Value = std::stoll(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, int64Value, source);
                return true;
            }
        } catch (...) {}
        
        // Check if it's a double
        try {
            size_t pos;
            uint64_t uint64Value = std::stoull(valueStr, &pos);
            if (pos == valueStr.size()) {
                set(key, uint64Value, source);
                return true;
            }
        } catch (...) {}
        
        // Default to string
        set(key, valueStr, source);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to parse configuration value for key '" + key + "': " + std::string(e.what()));
        return false;
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
    NLM_LOG_DEBUG("Removed configuration key: " + key);
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
    NLM_LOG_DEBUG("Cleared all configuration entries");
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  " << entry.key << " = [";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << arg[i] << "\"";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "["; 
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
            } else {
                oss << arg;
            }
        }, entry.value);
        oss << "] (" << static_cast<int>(entry.source) << ")\n";
    }
    
    // Add validation status if required keys are configured
    if (pImpl->validateRequiredKeys && !pImpl->requiredKeys.empty()) {
        oss << "\nRequired keys validation: " << (pImpl->validateRequiredKeys ? "ENABLED" : "DISABLED") << "\n";
        oss << "Required keys: ";
        for (const auto& key : pImpl->requiredKeys) {
            oss << key << " ";
        }
        oss << "\n";
        
        // Check which required keys are present
        oss << "Missing required keys: ";
        bool first = true;
        for (const auto& key : pImpl->requiredKeys) {
            if (!has(key)) {
                if (!first) oss << ", ";
                oss << key;
                first = false;
            }
        }
        if (first) {
            oss << "none";
        }
        oss << "\n";
    }
    
    return oss.str();
}

// Configuration validation methods
void Config::setRequiredKeys(const std::set<std::string>& keys) {
    pImpl->validateRequiredKeys = true;
    pImpl->requiredKeys = keys;
    NLM_LOG_DEBUG("Set " + std::to_string(pImpl->requiredKeys.size()) + " required configuration keys");
}

void Config::setRequiredKeys(const std::vector<std::string>& keys) {
    pImpl->validateRequiredKeys = true;
    pImpl->requiredKeys = std::set<std::string>(keys.begin(), keys.end());
    NLM_LOG_DEBUG("Set " + std::to_string(pImpl->requiredKeys.size()) + " required configuration keys");
}

void Config::enableJSONParsing(bool enable) {
    pImpl->jsonEnabled = enable;
    if (enable) {
        NLM_LOG_DEBUG("Enabled JSON configuration file parsing");
    } else {
        NLM_LOG_DEBUG("Disabled JSON configuration file parsing (legacy mode)");
    }
}

bool Config::validate() const {
    if (!pImpl->validateRequiredKeys) {
        return true;  // No validation required
    }
    
    std::vector<std::string> missingKeys;
    for (const auto& key : pImpl->requiredKeys) {
        if (!has(key)) {
            missingKeys.push_back(key);
        }
    }
    
    if (!missingKeys.empty()) {
        NLM_LOG_ERROR("Configuration validation failed: missing required keys: ");
        for (const auto& key : missingKeys) {
            NLM_LOG_ERROR("  - " + key);
        }
        return false;
    }
    
    return true;
}

std::string Config::getValidationError() const {
    if (!validate()) {
        std::string error = "Configuration validation failed: missing required keys: ";
        bool first = true;
        for (const auto& key : pImpl->requiredKeys) {
            if (!has(key)) {
                if (!first) error += ", ";
                error += key;
                first = false;
            }
        }
        return error;
    }
    return "";
}

// Explicit template instantiations
// These ensure the compiler generates the code for these template functions
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

// getOr specializations
template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;
template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

} // namespace nlm