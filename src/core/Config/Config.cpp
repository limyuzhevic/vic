#include "Config.hpp"
#include "../Errors/Errors.hpp"
#include "../Errors/ErrorHandling.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <filesystem>
#include <system_error>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

#include "Config.hpp"
#include "../Errors/Errors.hpp"
#include "../Errors/ErrorHandling.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <filesystem>
#include <system_error>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        // Validate file path
        if (!validateFilePath(filepath)) {
            throw ConfigError("Invalid file path: " + filepath, ConfigError::ErrorType::InvalidFileFormat);
        }
        
        // Check if file exists
        if (!fileExists(filepath)) {
            throw ConfigError("Configuration file not found: " + filepath, ConfigError::ErrorType::MissingRequiredParameter);
        }
        
        // Create backup before loading
        if (fileExists(filepath)) {
            try {
                SafeFile::createBackup(filepath);
            } catch (const FileError& e) {
                logWarning("Failed to create config backup: " + e.what());
            }
        }
        
        std::ifstream file(filepath, std::ios::in | std::ios::binary);
        if (!file.is_open()) {
            throw ConfigError("Failed to open configuration file: " + filepath, ConfigError::ErrorType::FileNotFound);
        }
        
        // Check file size
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);
        
        // Limit file size to prevent memory exhaustion
        constexpr size_t MAX_CONFIG_FILE_SIZE = 10 * 1024 * 1024; // 10MB
        if (fileSize > MAX_CONFIG_FILE_SIZE) {
            throw ConfigError("Configuration file too large: " + filepath, ConfigError::ErrorType::InvalidFileFormat);
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
            
            // Validate line length
            if (line.length() > 1024) {
                throw ConfigError("Line " + std::to_string(lineNumber) + " too long in config file", 
                                 ConfigError::ErrorType::InvalidFileFormat);
            }
            
            // Parse key=value pairs with validation
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                // Validate key and value
                validateConfigKey(key);
                validateConfigValue(key, value);
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            } else {
                throw ConfigError("Invalid format on line " + std::to_string(lineNumber) + ": missing '='", 
                                 ConfigError::ErrorType::InvalidFileFormat);
            }
        }
        
        if (!file.good() && !file.eof()) {
            throw ConfigError("Error reading configuration file: " + filepath, 
                             ConfigError::ErrorType::ReadFailed);
        }
        
        return true;
        
    } catch (const ConfigError& e) {
        throw;  // Re-throw config errors as-is
    } catch (const ValidationError& e) {
        throw ConfigError("Config validation error: " + e.getMessage(), 
                         ConfigError::ErrorType::ValidationFailed);
    } catch (const FileError& e) {
        throw ConfigError("File error in config: " + e.getMessage(), 
                         ConfigError::ErrorType::FileNotFound);
    } catch (const std::exception& e) {
        throw ConfigError("Unexpected error loading config file " + filepath + ": " + std::string(e.what()),
                         ConfigError::ErrorType::InvalidFileFormat);
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        if (argc <= 1) {
            return true;  // No arguments, nothing to do
        }
        
        for (int i = 1; i < argc; ++i) {
            try {
                if (argv[i] == nullptr) {
                    throw ValidationError("Null argument pointer", ValidationError::ErrorType::InvalidFormat);
                }
                
                std::string arg(argv[i]);
                
                // Handle --key=value format
                if (arg.substr(0, 2) == "--") {
                    size_t pos = arg.find('=');
                    if (pos != std::string::npos) {
                        std::string key = arg.substr(2, pos - 2);
                        std::string value = arg.substr(pos + 1);
                        
                        // Validate key and value
                        validateConfigKey(key);
                        validateConfigValue(key, value);
                        
                        set(key, value, ConfigSource::CommandLine);
                    } else {
                        throw ValidationError("Invalid command line argument format: " + arg, 
                                           ValidationError::ErrorType::InvalidFormat);
                    }
                }
                // Handle -key value format
                else if (arg[0] == '-' && i + 1 < argc) {
                    std::string key = arg.substr(1);
                    std::string value = std::string(argv[++i]);
                    
                    // Validate key and value
                    validateConfigKey(key);
                    validateConfigValue(key, value);
                    
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    throw ValidationError("Invalid command line argument: " + arg, 
                                       ValidationError::ErrorType::InvalidFormat);
                }
            } catch (const ValidationError& e) {
                throw ConfigError("Error parsing argument " + std::to_string(i) + ": " + e.getMessage(),
                                 ConfigError::ErrorType::InvalidValue);
            }
        }
        
        return true;
        
    } catch (const ConfigError& e) {
        throw;  // Re-throw config errors
    } catch (const std::exception& e) {
        throw ConfigError("Unexpected error loading from arguments: " + std::string(e.what()),
                         ConfigError::ErrorType::InvalidFileFormat);
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        // Validate file path
        if (!validateFilePath(filepath)) {
            throw FileError("Invalid file path: " + filepath, FileError::ErrorType::InvalidPath);
        }
        
        // Check if directory exists and is writable
        std::filesystem::path dir = std::filesystem::path(filepath).parent_path();
        if (!dir.empty() && !std::filesystem::exists(dir)) {
            throw FileError("Directory does not exist: " + dir.string(), FileError::ErrorType::DirectoryExpected);
        }
        
        // Create backup before saving
        if (fileExists(filepath)) {
            try {
                SafeFile::createBackup(filepath);
            } catch (const FileError& e) {
                logWarning("Failed to create config backup before save: " + e.what());
            }
        }
        
        // Open file for writing with error handling
        std::ofstream file(filepath, std::ios::out | std::ios::trunc | std::ios::binary);
        if (!file.is_open()) {
            throw FileError("Failed to open configuration file for writing: " + filepath, 
                           FileError::ErrorType::WriteFailed);
        }
        
        // Write configuration with error handling
        for (const auto& entry : pImpl->entries) {
            // Write description as comment
            file << "# " << entry.description << "\n";
            
            // Write key=value pair
            file << entry.key << " = ";
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            }, entry.value);
            file << "\n";
            
            // Check for write errors
            if (!file.good()) {
                throw FileError("Write error in config file: " + filepath, FileError::ErrorType::WriteFailed);
            }
        }
        
        // Flush to ensure data is written
        file.flush();
        
        if (!file.good()) {
            throw FileError("Error flushing config file: " + filepath, FileError::ErrorType::WriteFailed);
        }
        
        // Check if file was actually written
        if (pImpl->entries.empty()) {
            logWarning("Saving empty configuration to file: " + filepath);
        }
        
        return true;
        
    } catch (const FileError& e) {
        throw;  // Re-throw file errors as-is
    } catch (const ValidationError& e) {
        throw FileError("File error in saveToFile: " + e.getMessage(), FileError::ErrorType::InvalidPath);
    } catch (const std::exception& e) {
        throw FileError("Unexpected error saving config to " + filepath + ": " + std::string(e.what()),
                       FileError::ErrorType::WriteFailed);
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    try {
        // Validate key
        validateConfigKey(key);
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it == pImpl->entries.end()) {
            return std::nullopt;
        }
        
        try {
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            throw ConfigError("Type mismatch for config key: " + key, 
                             ConfigError::ErrorType::ConversionFailed);
        }
        
    } catch (const ConfigError& e) {
        throw;  // Re-throw config errors
    } catch (const ValidationError& e) {
        throw ConfigError("Config error retrieving key " + key + ": " + e.getMessage(),
                         ConfigError::ErrorType::ValidationFailed);
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    try {
        auto val = get<T>(key);
        if (val.has_value()) {
            return val.value();
        }
        return defaultValue;
    } catch (const ConfigError& e) {
        logWarning("Config error in getOr for key " + key + ": " + e.getMessage());
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    try {
        // Validate key and value
        validateConfigKey(key);
        
        // For non-string values, we need to validate the string representation
        if (source == ConfigSource::Runtime) {
            // Runtime values need special validation
            if (std::holds_alternative<std::string>(value)) {
                validateConfigValue(key, std::get<std::string>(value));
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
        
    } catch (const ConfigError& e) {
        throw;  // Re-throw config errors
    } catch (const ValidationError& e) {
        throw ConfigError("Config error setting key " + key + ": " + e.getMessage(),
                         ConfigError::ErrorType::ValidationFailed);
    } catch (const std::exception& e) {
        throw ConfigError("Unexpected error setting config key " + key + ": " + std::string(e.what()),
                         ConfigError::ErrorType::InvalidFileFormat);
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
    try {
        validateConfigKey(key);
        return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
    } catch (const ValidationError& e) {
        logWarning("Config error checking key " + key + ": " + e.getMessage());
        return false;
    }
}

void Config::remove(const std::string& key) {
    try {
        validateConfigKey(key);
        
        pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
            pImpl->entries.end()
        );
    } catch (const ValidationError& e) {
        logWarning("Config error removing key " + key + ": " + e.getMessage());
    }
}

std::vector<std::string> Config::getKeys() const {
    try {
        std::vector<std::string> keys;
        keys.reserve(pImpl->entries.size());
        for (const auto& entry : pImpl->entries) {
            keys.push_back(entry.key);
        }
        return keys;
    } catch (const std::exception& e) {
        logError(ConfigError("Error getting config keys: " + std::string(e.what()), 
                           ConfigError::ErrorType::InvalidFileFormat));
        return std::vector<std::string>();
    }
}

void Config::clear() {
    try {
        pImpl->entries.clear();
    } catch (const std::exception& e) {
        logError(ConfigError("Error clearing config: " + std::string(e.what()), 
                           ConfigError::ErrorType::InvalidFileFormat));
    }
}

std::string Config::summary() const {
    try {
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
    } catch (const std::exception& e) {
        logError(ConfigError("Error generating config summary: " + std::string(e.what()), 
                           ConfigError::ErrorType::InvalidFileFormat));
        return "Error generating configuration summary";
    }
}

std::string Config::trim(const std::string& str) {
    try {
        size_t start = str.find_first_not_of(" \t\r\n");
        if (start == std::string::npos) return "";
        size_t end = str.find_last_not_of(" \t\r\n");
        return str.substr(start, end - start + 1);
    } catch (const std::exception& e) {
        logError(ConfigError("Error trimming string: " + std::string(e.what()), 
                           ConfigError::ErrorType::InvalidFileFormat));
        return str;  // Return original string on error
    }
}

std::string Config::toLower(const std::string& str) {
    try {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    } catch (const std::exception& e) {
        logError(ConfigError("Error converting to lowercase: " + std::string(e.what()), 
                           ConfigError::ErrorType::InvalidFileFormat));
        return str;  // Return original string on error
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
