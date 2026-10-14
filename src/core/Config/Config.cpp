#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// Implementation of validation functions
namespace ConfigValidation {

bool isValidKey(const std::string& key) {
    if (key.empty() || key.length() > 100) {
        return false;
    }
    
    // Check for invalid characters
    for (char c : key) {
        if (!(std::isalnum(c) || c == '_' || c == '-' || c == '.')) {
            return false;
        }
    }
    
    return true;
}

bool isValidValue(const std::string& key, const std::string& value) {
    // Basic string validation
    if (value.length() > 10000) {
        return false;
    }
    
    // Check for problematic escape sequences
    if (value.find("\\0") != std::string::npos) {
        return false;
    }
    
    return true;
}

bool isValidValue(const std::string& key, int value) {
    // Validate integer ranges based on key
    if (key == "random_seed" && (value < 0 || value > 2147483647)) {
        return false;
    }
    if (key == "neuron_count" && (value < 1 || value > 1000000)) {
        return false;
    }
    if (key == "region_count" && (value < 1 || value > 1000)) {
        return false;
    }
    
    return true;
}

bool isValidValue(const std::string& key, double value) {
    // Validate floating point ranges
    if (std::isnan(value) || std::isinf(value)) {
        return false;
    }
    
    if (key == "simulation_timestep" && (value <= 0.0 || value > 1.0)) {
        return false;
    }
    
    if (key == "connection_probability" && (value < 0.0 || value > 1.0)) {
        return false;
    }
    
    return true;
}

bool isValidVector(const std::string& key, const std::vector<std::string>& values) {
    if (values.size() > 1000000) {
        return false;
    }
    
    for (const auto& value : values) {
        if (!isValidValue(key, value)) {
            return false;
        }
    }
    
    return true;
}

std::string getValidationError(const std::string& key, const std::string& value) {
    std::ostringstream oss;
    oss << "Invalid value '" << value << "' for configuration key '" << key << "'";
    return oss.str();
}

} // namespace ConfigValidation

// Utility functions for error handling
namespace {

std::string getFileError(const std::string& filepath, ConfigError error) {
    switch (error) {
        case ConfigError::FileNotFound:
            return "File not found: " + filepath;
        case ConfigError::PermissionDenied:
            return "Permission denied: " + filepath;
        case ConfigError::InvalidFormat:
            return "Invalid file format: " + filepath;
        default:
            return "File error: " + filepath;
    }
}

} // namespace

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Validate filepath
        if (filepath.empty()) {
            throw ConfigException(ConfigError::InvalidFormat, "Empty filepath");
        }
        
        if (!ConfigValidation::isValidKey(filepath)) {
            throw ConfigException(ConfigError::InvalidKey, "Invalid filepath format: " + filepath);
        }
        
        std::ifstream file(filepath, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            // Check if file exists and we have permission
            if (std::filesystem::exists(filepath)) {
                throw ConfigException(ConfigError::PermissionDenied, "Cannot read file: " + filepath);
            } else {
                throw ConfigException(ConfigError::FileNotFound, "File not found: " + filepath);
            }
        }
        
        // Check file size before reading
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        if (fileSize > 10 * 1024 * 1024) { // 10MB limit
            throw ConfigException(ConfigError::ValueOutOfRange, "File too large: " + filepath);
        }
        
        std::string line;
        size_t lineNumber = 0;
        
        while (std::getline(file, line)) {
            ++lineNumber;
            
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse key=value pairs with better validation
            size_t pos = line.find('=');
            if (pos == std::string::npos) {
                throw ConfigException(ConfigError::InvalidFormat, 
                    "Invalid line format at " + filepath + ":" + std::to_string(lineNumber) + ": " + line);
            }
            
            std::string key = trim(line.substr(0, pos));
            std::string value = trim(line.substr(pos + 1));
            
            // Validate key
            if (!ConfigValidation::isValidKey(key)) {
                throw ConfigException(ConfigError::InvalidKey, 
                    "Invalid key at " + filepath + ":" + std::to_string(lineNumber) + ": " + key);
            }
            
            // Validate value
            if (!ConfigValidation::isValidValue(key, value)) {
                throw ConfigException(ConfigError::InvalidValue,
                    "Invalid value for key '" + key + "' at " + filepath + ":" + std::to_string(lineNumber) + ": " + value);
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            // Set the configuration value with validation
            try {
                set(key, value, ConfigSource::File);
            } catch (const ConfigException& e) {
                // Log validation error but continue processing
                pImpl->validationErrors.push_back(e.what());
            }
        }
        
        if (!pImpl->validationErrors.empty()) {
            std::ostringstream warning;
            warning << "Configuration loaded with " << pImpl->validationErrors.size() << " warnings:\n";
            for (const auto& error : pImpl->validationErrors) {
                warning << "  WARNING: " << error << "\n";
            }
            // TODO: Add proper logging here
            // NLM_LOG_WARNING(warning.str());
        }
        
        return true;
        
    } catch (const ConfigException& e) {
        // Log error and return false for backward compatibility
        // TODO: Add proper error logging
        // NLM_LOG_ERROR("Failed to load configuration from " + filepath + ": " + e.what());
        return false;
    } catch (const std::exception& e) {
        // Catch any other unexpected exceptions
        // TODO: Add proper error logging
        // NLM_LOG_ERROR("Unexpected error loading configuration from " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        if (argc <= 1 || argv == nullptr) {
            throw ConfigException(ConfigError::InvalidFormat, "Invalid command line arguments");
        }
        
        for (int i = 1; i < argc; ++i) {
            if (argv[i] == nullptr) {
                continue; // Skip null pointers
            }
            
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    // Validate key
                    if (!ConfigValidation::isValidKey(key)) {
                        throw ConfigException(ConfigError::InvalidKey, "Invalid command line key: " + key);
                    }
                    
                    // Validate value
                    if (!ConfigValidation::isValidValue(key, value)) {
                        throw ConfigException(ConfigError::InvalidValue, 
                            "Invalid value for key '" + key + "': " + value);
                    }
                    
                    set(key, value, ConfigSource::CommandLine);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc && argv[i + 1] != nullptr) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                // Validate key
                if (!ConfigValidation::isValidKey(key)) {
                    throw ConfigException(ConfigError::InvalidKey, "Invalid command line key: " + key);
                }
                
                // Validate value
                if (!ConfigValidation::isValidValue(key, value)) {
                    throw ConfigException(ConfigError::InvalidValue, 
                        "Invalid value for key '" + key + "': " + value);
                }
                
                set(key, value, ConfigSource::CommandLine);
            }
        }
        
        return true;
        
    } catch (const ConfigException& e) {
        // Log error and return false for backward compatibility
        // TODO: Add proper error logging
        // NLM_LOG_ERROR("Failed to load configuration from command line: " + std::string(e.what()));
        return false;
    } catch (const std::exception& e) {
        // Catch any other unexpected exceptions
        // TODO: Add proper error logging
        // NLM_LOG_ERROR("Unexpected error loading configuration from command line: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Validate filepath
        if (filepath.empty()) {
            throw ConfigException(ConfigError::InvalidFormat, "Empty filepath for saving");
        }
        
        if (!ConfigValidation::isValidKey(filepath)) {
            throw ConfigException(ConfigError::InvalidKey, "Invalid filepath format: " + filepath);
        }
        
        // Create output file
        std::ofstream file(filepath, std::ios::out | std::ios::trunc);
        if (!file.is_open()) {
            // Check if directory exists and we have permission
            std::filesystem::path filepath_path(filepath);
            if (std::filesystem::exists(filepath_path.parent_path())) {
                throw ConfigException(ConfigError::PermissionDenied, "Cannot write to file: " + filepath);
            } else {
                throw ConfigException(ConfigError::FileNotFound, "Directory does not exist: " + filepath_path.parent_path().string());
            }
        }
        
        // Write configuration entries
        for (const auto& entry : pImpl->entries) {
            // Write comment if present
            if (!entry.description.empty()) {
                file << "# " << entry.description << "\n";
            }
            
            // Write key=value pair
            file << entry.key << " = ";
            
            // Handle different value types
            std::visit([&file](auto&& arg) {
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
        
        // Flush to ensure all data is written
        file.flush();
        
        // Check if file was actually written
        if (file.fail()) {
            throw ConfigException(ConfigError::MemoryAllocationFailed, "Failed to write configuration to file: " + filepath);
        }
        
        return true;
        
    } catch (const ConfigException& e) {
        // Log error and return false for backward compatibility
        // TODO: Add proper error logging
        // NLM_LOG_ERROR("Failed to save configuration to " + filepath + ": " + e.what());
        return false;
    } catch (const std::exception& e) {
        // Catch any other unexpected exceptions
        // TODO: Add proper error logging
        // NLM_LOG_ERROR("Unexpected error saving configuration to " + filepath + ": " + std::string(e.what()));
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
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key
    if (!ConfigValidation::isValidKey(key)) {
        throw ConfigException(ConfigError::InvalidKey, "Invalid configuration key: " + key);
    }
    
    // Validate value based on its type
    if (std::holds_alternative<int>(value)) {
        if (!ConfigValidation::isValidValue(key, std::get<int>(value))) {
            throw ConfigException(ConfigError::InvalidValue, 
                "Invalid integer value for key '" + key + "': " + std::to_string(std::get<int>(value)));
        }
    } else if (std::holds_alternative<double>(value)) {
        if (!ConfigValidation::isValidValue(key, std::get<double>(value))) {
            throw ConfigException(ConfigError::InvalidValue,
                "Invalid double value for key '" + key + "': " + std::to_string(std::get<double>(value)));
        }
    } else if (std::holds_alternative<std::string>(value)) {
        if (!ConfigValidation::isValidValue(key, std::get<std::string>(value))) {
            throw ConfigException(ConfigError::InvalidValue,
                "Invalid string value for key '" + key + "': " + std::get<std::string>(value));
        }
    }
    
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
    // Use the base ConfigValue setter which includes validation
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    // Use the base ConfigValue setter which includes validation
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    // Use the base ConfigValue setter which includes validation
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    // Use the base ConfigValue setter which includes validation
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
