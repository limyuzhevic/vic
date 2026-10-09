#include "Config.hpp"
#include "Errors/Errors.hpp"
#include "Logger/Logger.hpp"
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

bool Config::loadFromFile(const std::string& filepath) throw(ConfigLoadException) {
    validateFilePath(filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw ConfigLoadException("Failed to open configuration file: " + filepath, __FILE__, __LINE__);
    }
    
    std::string line;
    int lineNum = 0;
    while (std::getline(file, line)) {
        lineNum++;
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            throw ConfigParseException("Invalid configuration format: missing '=' in line " + std::to_string(lineNum), __FILE__, __LINE__);
        }
        
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
        } catch (const ConfigValidationException& e) {
            throw ConfigLoadException("Configuration validation error in line " + std::to_string(lineNum) + ": " + e.what(), __FILE__, __LINE__);
        }
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) throw(ConfigLoadException) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    set(key, value, ConfigSource::CommandLine);
                } else {
                    throw ConfigValidationException("Invalid command line argument format: missing '='", __FILE__, __LINE__);
                }
            }
            // Handle -key value format
            else if (arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                set(key, value, ConfigSource::CommandLine);
            } else {
                throw ConfigLoadException("Invalid command line argument: " + arg, __FILE__, __LINE__);
            }
        }
        return true;
    } catch (const ConfigValidationException& e) {
        throw ConfigLoadException("Configuration error from command line arguments: " + std::string(e.what()), __FILE__, __LINE__);
    }
}

bool Config::saveToFile(const std::string& filepath) const throw(ConfigLoadException) {
    validateFilePath(filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw ConfigLoadException("Failed to open configuration file for writing: " + filepath, __FILE__, __LINE__);
    }
    
    try {
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
        }
    } catch (const std::exception& e) {
        throw ConfigLoadException("Error writing configuration to file: " + std::string(e.what()), __FILE__, __LINE__);
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const throw(ConfigValidationException) {
    validateKey(key);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        throw ConfigValidationException("Type mismatch for key: " + key, __FILE__, __LINE__);
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) throw(ConfigValidationException) {
    validateKey(key);
    validateValue(key, value);
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) throw(ConfigValidationException) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) throw(ConfigValidationException) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) throw(ConfigValidationException) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) throw(ConfigValidationException) {
    set(key, ConfigValue(value), source);
}

void Config::validateKey(const std::string& key) const throw(ConfigValidationException) {
    if (!isValidKey(key)) {
        throw ConfigValidationException("Invalid configuration key: " + key, __FILE__, __LINE__);
    }
}

void Config::validateValue(const std::string& key, const ConfigValue& value) const throw(ConfigValidationException) {
    if (!isValidValue(value)) {
        throw ConfigValidationException("Invalid configuration value for key: " + key, __FILE__, __LINE__);
    }
}

void Config::validateFilePath(const std::string& filepath) const throw(ConfigValidationException) {
    if (!isValidFilePath(filepath)) {
        throw ConfigValidationException("Invalid file path: " + filepath, __FILE__, __LINE__);
    }
}

bool Config::isValidFilePath(const std::string& filepath) {
    return !filepath.empty() && filepath.find('\0') == std::string::npos;
}

bool Config::isValidKey(const std::string& key) {
    return !key.empty() && key.find('=') == std::string::npos && key.find('\0') == std::string::npos;
}

bool Config::isValidValue(const ConfigValue& value) {
    return !std::holds_alternative<std::monostate>(value);
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
// All templates are now defined in the separate Config.cpp.templates file

} // namespace nlm
