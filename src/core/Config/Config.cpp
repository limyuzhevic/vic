#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iomanip>
#include <ctime>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check if file exists
    if (!checkFileExists(filepath)) {
        throw ConfigError("Configuration file not found: " + filepath, 
                         std::filesystem_error("File not found: " + filepath, 
                                              std::error_code()));
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError("Failed to open configuration file for reading: " + filepath, 
                         std::filesystem_error("File open failed", 
                                              std::error_code()));
    }
    
    try {
        // Parse as JSON
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries
        clear();
        
        // Parse JSON entries
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            std::string description = "Configuration parameter: " + key;
            
            // Validate key before processing
            if (!isValidKey(key)) {
                throw ConfigError("Invalid configuration key format: " + key);
            }
            
            // Try to convert to appropriate ConfigValue type
            auto configValue = jsonToConfigValue(*it);
            if (configValue.has_value()) {
                // Validate schema for the key-value pair
                if (!validateSchema(key, configValue.value())) {
                    throw ConfigError("Schema validation failed for key: " + key);
                }
                set(key, configValue.value(), ConfigSource::File, description);
            } else {
                throw ConfigError("Failed to convert JSON value to configuration type for key: " + key);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        throw ConfigError("JSON parse error in configuration file " + filepath + ": " + e.what(), 
                         e);
    } catch (const nlohmann::json::type_error& e) {
        throw ConfigError("JSON type error in configuration file " + filepath + ": " + e.what(), 
                         e);
    } catch (const std::exception& e) {
        throw ConfigError("Unexpected error loading configuration from file " + filepath + ": " + e.what(), 
                         e);
    }
}

bool Config::loadFromFileSimple(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError("Failed to open configuration file for reading: " + filepath, 
                         std::ifstream::failure("Failed to open file"));
    }
    
    clear();
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key format
            if (!isValidKey(key)) {
                throw ConfigError("Invalid configuration key format on line " + 
                                 std::to_string(lineNum) + ": " + key);
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            set(key, value, ConfigSource::File);
        } else {
            // If no = found, treat entire line as key with default value
            std::string key = line;
            if (!isValidKey(key)) {
                throw ConfigError("Invalid configuration key format on line " + 
                                 std::to_string(lineNum) + ": " + key);
            }
            set(key, std::string("1"), ConfigSource::File);
        }
    }
    
    return true;
}

bool Config::loadFromJsonString(const std::string& jsonString) {
    try {
        nlohmann::json jsonData = nlohmann::json::parse(jsonString);
        clear();
        
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            std::string description = "Configuration parameter: " + key;
            
            // Validate key format
            if (!isValidKey(key)) {
                throw ConfigError("Invalid configuration key format: " + key);
            }
            
            auto configValue = jsonToConfigValue(*it);
            if (configValue.has_value()) {
                // Validate schema for the key-value pair
                if (!validateSchema(key, configValue.value())) {
                    throw ConfigError("Schema validation failed for key: " + key);
                }
                set(key, configValue.value(), ConfigSource::File, description);
            } else {
                throw ConfigError("Failed to convert JSON value to configuration type for key: " + key);
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        throw ConfigError("JSON parse error: " + e.what(), e);
    } catch (const nlohmann::json::type_error& e) {
        throw ConfigError("JSON type error: " + e.what(), e);
    } catch (const std::exception& e) {
        throw ConfigError("Unexpected error parsing JSON configuration: " + e.what(), e);
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
    // Ensure directory exists with error handling
    if (!ensureDirectoryExists(filepath)) {
        throw ConfigError("Failed to create directory for configuration file: " + filepath, 
                         std::filesystem_error("Directory creation failed", 
                                              std::error_code()));
    }
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigError("Failed to open configuration file for writing: " + filepath, 
                         std::ofstream::failure("Failed to open file for writing"));
    }
    
    // Write JSON with pretty formatting
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        try {
            jsonData[entry.key] = configValueToJson(entry.value);
        } catch (const std::exception& e) {
            throw ConfigError("Failed to serialize configuration value for key " + entry.key + ": " + e.what(), 
                             e);
        }
    }
    
    // Add metadata
    jsonData["__metadata"]["created"] = getCurrentTimestamp();
    jsonData["__metadata"]["version"] = "1.0";
    
    // Write to file with indentation
    try {
        file << std::setw(2) << jsonData << std::endl;
    } catch (const std::exception& e) {
        throw ConfigError("Failed to write configuration to file: " + filepath + ": " + e.what(), 
                         e);
    }
    
    return true;
}

std::string Config::saveToJsonString() const {
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        jsonData[entry.key] = configValueToJson(entry.value);
    }
    
    return jsonData.dump(2);
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
    set(key, value, source, "");
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

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source, const std::string& description) {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        if (!description.empty()) {
            it->description = description;
        }
    } else {
        pImpl->entries.emplace_back(key, value, source, description);
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

bool Config::validate() const {
    std::vector<std::string> errors = getValidationErrors();
    
    // Log validation errors if any
    if (!errors.empty()) {
        // Note: In a real implementation, we would use the Logger class
        for (const auto& error : errors) {
            // LogError(error); // Would use Logger when available
        }
    }
    
    return errors.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    std::vector<std::string> errors;
    
    // Validate required keys
    const std::vector<std::string> requiredKeys = {"app_name", "version"};
    for (const auto& key : requiredKeys) {
        if (!has(key)) {
            errors.push_back("Missing required configuration key: " + key);
        }
    }
    
    // Validate each configuration entry using schema validation
    for (const auto& entry : pImpl->entries) {
        std::vector<std::string> entryErrors;
        
        // Validate key format
        if (!isValidKey(entry.key)) {
            entryErrors.push_back("Invalid key format: '" + entry.key + "'");
        }
        
        // Validate value using schema
        if (!validateSchema(entry.key, entry.value)) {
            entryErrors.push_back("Schema validation failed for key: '" + entry.key + "'");
        }
        
        // Validate value range if applicable
        if (!isValueWithinRange(entry.key, entry.value)) {
            entryErrors.push_back("Value out of range for key: '" + entry.key + "'");
        }
        
        if (!entryErrors.empty()) {
            for (const auto& error : entryErrors) {
                errors.push_back(entry.key + ": " + error);
            }
        }
    }
    
    return errors;
}

nlohmann::json Config::configValueToJson(const ConfigValue& value) {
    nlohmann::json j;
    
    std::visit([&j](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, int>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, double>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::string>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            j = arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            j = arg;
        }
    }, value);
    
    return j;
}

std::optional<ConfigValue> Config::jsonToConfigValue(const nlohmann::json& j) {
    try {
        if (j.is_number_integer()) {
            return j.get<int>();
        } else if (j.is_number_unsigned()) {
            return j.get<int>();
        } else if (j.is_number_float()) {
            return j.get<double>();
        } else if (j.is_boolean()) {
            return j.get<bool>();
        } else if (j.is_string()) {
            return j.get<std::string>();
        } else if (j.is_array()) {
            // Try to infer array type
            if (!j.empty()) {
                if (j[0].is_number_integer()) {
                    std::vector<int> vec = j.get<std::vector<int>>();
                    return vec;
                } else if (j[0].is_number_float()) {
                    std::vector<double> vec = j.get<std::vector<double>>();
                    return vec;
                } else if (j[0].is_string()) {
                    std::vector<std::string> vec = j.get<std::vector<std::string>>();
                    return vec;
                }
            }
            return std::vector<int>{};
        }
        
        // Fallback to empty string
        return std::string("");
    } catch (...) {
        return std::nullopt;
    }
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

bool Config::checkFileExists(const std::string& filepath) {
    return std::filesystem::exists(filepath);
}

bool Config::ensureDirectoryExists(const std::string& filepath) {
    std::filesystem::path path(filepath);
    std::filesystem::path parent = path.parent_path();
    
    if (parent.empty() || parent == ".") {
        return true; // Current directory
    }
    
    return std::filesystem::create_directories(parent);
}

std::string Config::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    
    std::tm tm_buf;
    gmtime_r(&time_t_now, &tm_buf);
    
    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%Y-%m-%dT%H:%M:%SZ");
    
    return oss.str();
}

// Explicit template instantiations
namespace nlm {
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
}

} // namespace nlm
