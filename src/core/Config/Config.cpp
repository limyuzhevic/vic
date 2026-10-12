#include "Config.hpp"
#include "ErrorHandling.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cstring>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
    // Validate input parameters
    if (!Validation::isFilePathValid(filepath)) {
        ExceptionManager::throwError<ConfigError>(
            "Invalid file path: " + filepath, ConfigError::Type::InvalidFile, filepath);
    }
    
    // Try to open file with error handling
    std::ifstream file(filepath, std::ios::in);
    if (!file.is_open()) {
        // Check if file exists and is accessible
        if (!std::filesystem::exists(filepath)) {
            ExceptionManager::throwError<ConfigError>(
                "Configuration file not found: " + filepath, ConfigError::Type::FileNotFound, filepath);
        }
        if (!std::filesystem::is_regular_file(filepath)) {
            ExceptionManager::throwError<ConfigError>(
                "Path is not a regular file: " + filepath, ConfigError::Type::InvalidFile, filepath);
        }
        if (!std::filesystem::is_readable(filepath)) {
            ExceptionManager::throwError<ConfigError>(
                "Configuration file not readable: " + filepath, ConfigError::Type::PermissionDenied, filepath);
        }
        // If file exists but cannot be opened, return false for backward compatibility
        return false;
    }
    
    // Check file size to prevent loading extremely large files
    try {
        auto fileSize = std::filesystem::file_size(filepath);
        if (fileSize > 10 * 1024 * 1024) {  // 10MB limit
            ExceptionManager::throwError<ConfigError>(
                "Configuration file too large: " + std::to_string(fileSize) + " bytes", ConfigError::Type::CorruptedData, filepath);
        }
    } catch (const std::filesystem::filesystem_error& e) {
        ExceptionManager::throwError<ConfigError>(
            "Failed to get file size: " + std::string(e.what()), ConfigError::Type::InvalidFile, filepath);
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
            
            // Validate key
            if (!Validation::isFilePathValid(key) || key.empty()) {
                ExceptionManager::throwError<ConfigError>(
                    "Invalid configuration key: " + key, ConfigError::Type::InvalidFormat);
            }
            
            // Remove quotes if present
            if (value.size() >= 2 && 
                ((value.front() == '"' && value.back() == '"') ||
                 (value.front() == '\'' && value.back() == '\''))) {
                value = value.substr(1, value.size() - 2);
            }
            
            try {
                set(key, value, ConfigSource::File);
            } catch (const std::exception& e) {
                ExceptionManager::throwError<ConfigError>(
                    "Failed to set configuration key-value: " + key + "=" + value + " (" + e.what() + ")", 
                    ConfigError::Type::InvalidValue);
            }
        } else {
            // Log warning for malformed lines but continue parsing
            Logger::getGlobal().warning("Malformed configuration line (skipping): " + line, filepath.c_str());
        }
    }
    
    // Verify file is properly closed
    if (!file.eof() && file.bad()) {
        ExceptionManager::throwError<ConfigError>(
            "Error reading configuration file: I/O error", ConfigError::Type::CorruptedData, filepath);
    }
    
    return true;
}
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Validate input parameters
    if (argc <= 0 || argv == nullptr) {
        ExceptionManager::throwError<ConfigError>(
            "Invalid arguments for configuration loading", ConfigError::Type::InvalidFormat);
    }
    
    // Validate each argument
    for (int i = 1; i < argc; ++i) {
        if (argv[i] == nullptr) {
            ExceptionManager::throwError<ConfigError>(
                "Null argument at index " + std::to_string(i), ConfigError::Type::InvalidFormat);
        }
        
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key and value
                if (!Validation::isFilePathValid(key) || key.empty()) {
                    ExceptionManager::throwError<ConfigError>(
                        "Invalid configuration key from command line: " + key, ConfigError::Type::InvalidFormat);
                }
                
                set(key, value, ConfigSource::CommandLine);
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            
            // Validate key
            if (!Validation::isFilePathValid(key) || key.empty()) {
                ExceptionManager::throwError<ConfigError>(
                    "Invalid configuration key from command line: " + key, ConfigError::Type::InvalidFormat);
            }
            
            std::string value = argv[++i];
            
            set(key, value, ConfigSource::CommandLine);
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    // Validate input parameters
    if (!Validation::isFilePathValid(filepath)) {
        ExceptionManager::throwError<ConfigError>(
            "Invalid file path for saving: " + filepath, ConfigError::Type::InvalidFile, filepath);
    }
    
    // Try to open file for writing with error handling
    std::ofstream file(filepath, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        // Check if we can write to the directory
        std::filesystem::path path(filepath);
        if (std::filesystem::exists(path.parent_path()) && 
            !std::filesystem::is_directory(path.parent_path())) {
            ExceptionManager::throwError<ConfigError>(
                "Parent path is not a directory: " + path.parent_path().string(), 
                ConfigError::Type::InvalidFile, filepath);
        }
        
        // Check if we have write permissions
        if (std::filesystem::exists(path) && !std::filesystem::is_writable(path)) {
            ExceptionManager::throwError<ConfigError>(
                "Configuration file is not writable: " + filepath, 
                ConfigError::Type::PermissionDenied, filepath);
        }
        
        // If we can't write, return false for backward compatibility
        return false;
    }
    
    // Set file permissions (optional, platform-dependent)
    try {
        // Ensure we can write to the file
        file << "# Configuration saved by NLM" << std::endl;
        file << "# " << pImpl->entries.size() << " entries" << std::endl;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<ConfigError>(
            "Failed to write header to configuration file: " + std::string(e.what()), 
            ConfigError::Type::WriteError, filepath);
    }
    
    for (const auto& entry : pImpl->entries) {
        // Validate entry data before writing
        if (!Validation::isFilePathValid(entry.key) || entry.key.empty()) {
            ExceptionManager::throwError<ConfigError>(
                "Invalid configuration key: " + entry.key, ConfigError::Type::InvalidFormat);
        }
        
        try {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Write the value based on its type
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << "\"" << arg << "\"";
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << "\n\n";
        } catch (const std::exception& e) {
            ExceptionManager::throwError<ConfigError>(
                "Failed to write configuration entry " + entry.key + ": " + e.what(), 
                ConfigError::Type::WriteError, filepath);
        }
    }
    
    // Verify file is properly closed
    if (file.bad()) {
        ExceptionManager::throwError<ConfigError>(
            "Error writing to configuration file: I/O error", ConfigError::Type::CorruptedData, filepath);
    }
    
    // Flush and check for errors
    try {
        file.flush();
        if (file.fail()) {
            ExceptionManager::throwError<ConfigError>(
                "Failed to flush configuration file", ConfigError::Type::WriteError, filepath);
        }
    } catch (const std::exception& e) {
        ExceptionManager::throwError<ConfigError>(
            "Failed to flush configuration file: " + std::string(e.what()), 
            ConfigError::Type::WriteError, filepath);
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Validate input parameters
    if (!Validation::isFilePathValid(key) || key.empty()) {
        ExceptionManager::throwError<ConfigError>(
            "Invalid configuration key: " + key, ConfigError::Type::InvalidFormat);
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        // Log error but return nullopt for backward compatibility
        Logger::getGlobal().warning(
            "Failed to get value of type " + std::to_string(typeid(T).hash_code()) + 
            " for key: " + key);
        return std::nullopt;
    } catch (const std::exception& e) {
        ExceptionManager::throwError<ConfigError>(
            "Exception getting configuration value for key " + key + ": " + e.what(),
            ConfigError::Type::InvalidValue);
    }
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    try {
        auto val = get<T>(key);
        return val.has_value() ? val.value() : defaultValue;
    } catch (const std::exception& e) {
        // Log error and return default value
        Logger::getGlobal().error(
            "Error in getOr for key " + key + ": " + e.what());
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate input parameters
    if (!Validation::isFilePathValid(key) || key.empty()) {
        ExceptionManager::throwError<ConfigError>(
            "Invalid configuration key: " + key, ConfigError::Type::InvalidFormat);
    }
    
    // Validate that the value is not null (variant can hold null variants but std::variant doesn't)
    // In C++, std::variant doesn't have a null type by default, so we don't need to check for null
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        try {
            it->value = value;
            it->source = source;
        } catch (const std::exception& e) {
            ExceptionManager::throwError<ConfigError>(
                "Failed to set configuration value for key " + key + ": " + e.what(),
                ConfigError::Type::InvalidValue);
        }
    } else {
        try {
            pImpl->entries.emplace_back(key, value, source);
        } catch (const std::exception& e) {
            ExceptionManager::throwError<ConfigError>(
                "Failed to add new configuration entry for key " + key + ": " + e.what(),
                ConfigError::Type::InvalidValue);
        }
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
    // Validate input parameters
    if (!Validation::isFilePathValid(key) || key.empty()) {
        ExceptionManager::throwError<ConfigError>(
            "Invalid configuration key: " + key, ConfigError::Type::InvalidFormat);
    }
    
    return std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
}

void Config::remove(const std::string& key) {
    // Validate input parameters
    if (!Validation::isFilePathValid(key) || key.empty()) {
        ExceptionManager::throwError<ConfigError>(
            "Invalid configuration key: " + key, ConfigError::Type::InvalidFormat);
    }
    
    try {
        pImpl->entries.erase(
            std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
                [&key](const ConfigEntry& e) { return e.key == key; }),
            pImpl->entries.end()
        );
    } catch (const std::exception& e) {
        ExceptionManager::throwError<ConfigError>(
            "Failed to remove configuration key " + key + ": " + e.what(),
            ConfigError::Type::InvalidValue);
    }
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    
    try {
        keys.reserve(pImpl->entries.size());
        for (const auto& entry : pImpl->entries) {
            if (!Validation::isFilePathValid(entry.key) || entry.key.empty()) {
                // Log warning but continue processing
                Logger::getGlobal().warning("Invalid configuration key found, skipping: " + entry.key);
                continue;
            }
            keys.push_back(entry.key);
        }
    } catch (const std::exception& e) {
        ExceptionManager::throwError<ConfigError>(
            "Failed to get configuration keys: " + std::string(e.what()),
            ConfigError::Type::CorruptedData);
    }
    
    return keys;
}

void Config::clear() {
    try {
        pImpl->entries.clear();
    } catch (const std::exception& e) {
        ExceptionManager::throwError<ConfigError>(
            "Failed to clear configuration: " + std::string(e.what()),
            ConfigError::Type::CorruptedData);
    }
}

std::string Config::summary() const {
    std::ostringstream oss;
    try {
        oss << "Configuration (" << pImpl->entries.size() << " entries):\n";
        for (const auto& entry : pImpl->entries) {
            if (!Validation::isFilePathValid(entry.key) || entry.key.empty()) {
                // Log warning but continue processing
                Logger::getGlobal().warning("Invalid configuration entry found, skipping: " + entry.key);
                continue;
            }
            
            oss << "  " << entry.key << " = [";
            try {
                std::visit([&oss](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        oss << "\"" << arg << "\"";
                    } else {
                        oss << arg;
                    }
                }, entry.value);
            } catch (const std::exception& e) {
                oss << "[ERROR: " << e.what() << "]";
            }
            oss << "] (" << static_cast<int>(entry.source) << ")\n";
        }
    } catch (const std::exception& e) {
        ExceptionManager::throwError<ConfigError>(
            "Failed to generate configuration summary: " + std::string(e.what()),
            ConfigError::Type::CorruptedData);
    }
    
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    // Validate input
    if (str.empty()) {
        return "";
    }
    
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    
    try {
        return str.substr(start, end - start + 1);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<ConfigError>(
            "Failed to trim string: " + std::string(e.what()),
            ConfigError::Type::InvalidFormat);
    }
    
    return str;
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    
    try {
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    } catch (const std::exception& e) {
        ExceptionManager::throwError<ConfigError>(
            "Failed to convert string to lowercase: " + std::string(e.what()),
            ConfigError::Type::InvalidFormat);
    }
    
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
