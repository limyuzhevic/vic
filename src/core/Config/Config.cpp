#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>

// JSON/YAML support - include when available
#if defined(HAS_NLOHMANN_JSON)
#include "nlohmann/json.hpp"
#endif
#if defined(HAS_YAML_CPP)
#include "yaml-cpp/yaml.h"
#endif

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // Check file existence first
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_WARNING("Configuration file not found: " + filepath);
        return false;
    }
    
    FileFormat format = detectFileFormat(filepath);
    
    switch (format) {
        case FileFormat::JSON:
#if defined(HAS_NLOHMANN_JSON)
            return parseJsonFile(filepath, *this);
#else
            NLM_LOG_WARNING("JSON parsing requested but nlohmann/json library not available");
            return false;
#endif
        
        case FileFormat::YAML:
#if defined(HAS_YAML_CPP)
            return parseYamlFile(filepath, *this);
#else
            NLM_LOG_WARNING("YAML parsing requested but yaml-cpp library not available");
            return false;
#endif
        
        case FileFormat::KeyValue:
            return parseKeyValueFile(filepath, *this);
        
        case FileFormat::Unknown:
            NLM_LOG_WARNING("Unknown file format for configuration: " + filepath);
            return false;
    }
    
    return false;
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
    FileFormat format = detectFileFormat(filepath);
    
    switch (format) {
        case FileFormat::JSON:
            return saveToJsonFile(filepath, *this);
        
        case FileFormat::YAML:
            return saveToYamlFile(filepath, *this);
        
        case FileFormat::KeyValue:
            return saveToKeyValueFile(filepath, *this);
        
        case FileFormat::Unknown:
            NLM_LOG_WARNING("Unknown file format for saving configuration: " + filepath);
            return false;
    }
    
    return false;
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
#include <iostream>

namespace nlm {

// Helper function to validate configuration keys
bool isValidKey(const std::string& key) {
    if (key.empty()) {
        std::cerr << "Error: Empty configuration key is not allowed" << std::endl;
        return false;
    }
    
    // Check for valid characters (alphanumeric, underscore, dot, dash)
    for (char c : key) {
        if (!std::isalnum(c) && c != '_' && c != '.' && c != '-') {
            std::cerr << "Error: Invalid character in key '" << key 
                      << "'. Keys can only contain alphanumeric characters, underscores, dots, and dashes." << std::endl;
            return false;
        }
    }
    
    return true;
}

// Helper function to validate configuration values
bool isValidValue(const ConfigValue& value) {
    std::visit([](const auto& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            if (arg.empty()) {
                throw std::invalid_argument("Empty string values are not allowed");
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                            std::is_same_v<T, std::vector<double>> ||
                            std::is_same_v<T, std::vector<std::string>>) {
            if (arg.empty()) {
                throw std::invalid_argument("Empty array values are not allowed");
            }
        }
    }, value);
    return true;
}

// Helper function to get file extension in lowercase
std::string toLowerExtension(const std::string& filepath) {
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

// Enhanced saveToFile with better error handling and formatting
bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot write to configuration file: " << filepath << std::endl;
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Generated on " << __DATE__ << " " << __TIME__ << "\n";
    file << "# Source entries from " << pImpl->entries.size() << " configuration items\n\n";
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        
        // Convert value to string with proper formatting
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << '\"' << arg << '\"';
            } else if constexpr (std::is_same_v<T, bool>) {
                file << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int> ||
                               std::is_same_v<T, int64_t>) {
                file << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                // Format to avoid scientific notation
                file.precision(6);
                file << std::fixed << arg;
            } else {
                file << arg; // Fallback
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    std::cout << "Configuration saved successfully to: " << filepath << std::endl;
    return true;
}

// Enhanced loadFromFile with better error handling and validation
bool Config::loadFromFile(const std::string& filepath) {
    // Check file existence first
    if (!std::filesystem::exists(filepath)) {
        std::cerr << "Error: Configuration file not found: " << filepath << std::endl;
        return false;
    }
    
    // Determine file format based on extension
    std::string ext = toLowerExtension(filepath);
    
    // For now, only support .cfg and .txt files with key=value format
    if (ext != ".cfg" && ext != ".txt") {
        std::cerr << "Error: Unsupported configuration file format: " << ext << std::endl;
        std::cerr << "Supported formats: .cfg, .txt" << std::endl;
        return false;
    }
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open configuration file: " << filepath << std::endl;
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    size_t validEntries = 0;
    size_t errorCount = 0;
    
    while (std::getline(file, line)) {
        lineNumber++;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs with enhanced validation
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
            
            // Validate key
            if (!isValidKey(key)) {
                errorCount++;
                continue;
            }
            
            // Validate value
            ConfigValue configValue(value);
            try {
                if (!isValidValue(configValue)) {
                    std::cerr << "Error: Invalid value for key '" << key 
                              << "' at line " << lineNumber << std::endl;
                    errorCount++;
                    continue;
                }
            } catch (const std::exception& e) {
                std::cerr << "Error: Exception validating key '" << key 
                          << "' at line " << lineNumber << ": " << e.what() << std::endl;
                errorCount++;
                continue;
            }
            
            // Successfully parsed and validated
            set(key, value, ConfigSource::File);
            validEntries++;
            
        } else {
            std::cerr << "Error: No '=' found in line " << lineNumber << ": " << line << std::endl;
            errorCount++;
        }
    }
    
    std::cout << "Configuration loaded: " << validEntries << " entries successfully parsed" << std::endl;
    if (errorCount > 0) {
        std::cout << "Configuration loaded with " << errorCount << " errors (see above for details)" << std::endl;
    }
    
    return validEntries > 0;
}

} // namespace nlm
