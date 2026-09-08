#include "Config.hpp"
#include "../Logger/Logger.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <cassert>
#include <cstring>
#include <stdexcept>
#include <cmath>
#include <memory>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    NLM_LOG_DEBUG("Attempting to load config from file: " + filepath);
    
    // Validate filepath is not empty
    if (filepath.empty()) {
        NLM_LOG_ERROR("Config file path is empty");
        return false;
    }
    
    // Check if file exists before attempting to open
    if (!std::filesystem::exists(filepath)) {
        NLM_LOG_ERROR("Config file does not exist: " + filepath);
        return false;
    }
    
    std::ifstream file(filepath, std::ios::in);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Check file size to avoid reading huge files
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);
    
    const size_t MAX_CONFIG_SIZE = 10 * 1024 * 1024; // 10MB limit
    if (fileSize > MAX_CONFIG_SIZE) {
        NLM_LOG_ERROR("Config file too large: " + filepath + " (" + std::to_string(fileSize) + " bytes)");
        return false;
    }
    
    std::string line;
    size_t lineNumber = 0;
    bool hasErrors = false;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos == std::string::npos) {
            NLM_LOG_WARNING("Invalid config line (no '='): line " + std::to_string(lineNumber) + ": " + line);
            hasErrors = true;
            continue;
        }
        
        std::string key = trim(line.substr(0, pos));
        std::string value = trim(line.substr(pos + 1));
        
        // Validate key
        if (key.empty()) {
            NLM_LOG_ERROR("Empty key found at line " + std::to_string(lineNumber));
            hasErrors = true;
            continue;
        }
        
        if (key.length() > 256) {
            NLM_LOG_ERROR("Key too long at line " + std::to_string(lineNumber) + ": " + key);
            hasErrors = true;
            continue;
        }
        
        // Remove quotes if present
        if (value.size() >= 2 && 
            ((value.front() == '"' && value.back() == '"') ||
             (value.front() == '\'' && value.back() == '\''))) {
            value = value.substr(1, value.size() - 2);
        }
        
        // Validate value
        if (value.empty()) {
            NLM_LOG_WARNING("Empty value for key: " + key + " at line " + std::to_string(lineNumber));
        }
        
        try {
            set(key, value, ConfigSource::File);
            NLM_LOG_DEBUG("Config loaded: " + key + " = " + value);
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Failed to set config key '" + key + "' at line " + std::to_string(lineNumber) + ": " + e.what());
            hasErrors = true;
        }
    }
    
    file.close();
    
    if (hasErrors) {
        NLM_LOG_WARNING("Config file loaded with errors: " + filepath);
    } else {
        NLM_LOG_INFO("Successfully loaded config from: " + filepath);
    }
    
    return !hasErrors;
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_DEBUG("Loading config from command line arguments (argc=" + std::to_string(argc) + ")");
    
    if (argc <= 1) {
        NLM_LOG_DEBUG("No command line arguments to process");
        return true;
    }
    
    bool hasErrors = false;
    
    for (int i = 1; i < argc; ++i) {
        // Validate argv pointer before dereferencing
        if (!argv || !argv[i]) {
            NLM_LOG_ERROR("Null argument at index " + std::to_string(i));
            hasErrors = true;
            continue;
        }
        
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos == std::string::npos) {
                NLM_LOG_WARNING("Invalid command line argument format (no '='): " + arg);
                hasErrors = true;
                continue;
            }
            
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            
            // Validate key and value
            if (key.empty()) {
                NLM_LOG_ERROR("Empty key in command line argument: " + arg);
                hasErrors = true;
                continue;
            }
            
            if (value.empty()) {
                NLM_LOG_WARNING("Empty value for key in command line argument: " + arg);
            }
            
            try {
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_DEBUG("Config from command line: " + key + " = " + value);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to set config from command line '" + key + "': " + e.what());
                hasErrors = true;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            
            // Validate key
            if (key.empty()) {
                NLM_LOG_ERROR("Empty key in command line argument: " + arg);
                hasErrors = true;
                continue;
            }
            
            // Check next argument is not another flag
            if (argv[i + 1][0] == '-') {
                NLM_LOG_ERROR("Missing value for key in command line argument: " + arg);
                hasErrors = true;
                continue;
            }
            
            std::string value = argv[++i];
            
            if (value.empty()) {
                NLM_LOG_WARNING("Empty value for key in command line argument: " + arg);
            }
            
            try {
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_DEBUG("Config from command line: " + key + " = " + value);
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Failed to set config from command line '" + key + "': " + e.what());
                hasErrors = true;
            }
        } else {
            NLM_LOG_WARNING("Ignoring unrecognized command line argument: " + arg);
        }
    }
    
    if (hasErrors) {
        NLM_LOG_WARNING("Config loaded from command line with errors");
    } else {
        NLM_LOG_INFO("Successfully loaded config from command line arguments");
    }
    
    return !hasErrors;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
    }
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // Validate input parameters
    if (key.empty()) {
        NLM_LOG_ERROR("Attempt to get config with empty key");
        return std::nullopt;
    }
    
    if (key.length() > 256) {
        NLM_LOG_ERROR("Config key too long for retrieval: " + key);
        return std::nullopt;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Config key not found: " + key);
        return std::nullopt;
    }
    
    try {
        T result = std::get<T>(it->value);
        NLM_LOG_DEBUG("Successfully retrieved config key: " + key);
        return result;
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Config type mismatch for key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    // Validate input parameters
    if (key.empty()) {
        NLM_LOG_ERROR("Attempt to getOr config with empty key");
        return defaultValue;
    }
    
    auto val = get<T>(key);
    if (val.has_value()) {
        NLM_LOG_DEBUG("Successfully retrieved config key (getOr): " + key);
        return val.value();
    }
    
    NLM_LOG_DEBUG("Using default value for config key (getOr): " + key + " = " + std::to_string(defaultValue));
    return defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate input parameters
    if (key.empty()) {
        NLM_LOG_ERROR("Attempt to set config with empty key");
        return;
    }
    
    if (key.length() > 256) {
        NLM_LOG_ERROR("Config key too long for setting: " + key);
        return;
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    try {
        if (it != pImpl->entries.end()) {
            it->value = value;
            it->source = source;
            NLM_LOG_DEBUG("Config updated: " + key + " (source: " + std::to_string(static_cast<int>(source)) + ")");
        } else {
            pImpl->entries.emplace_back(key, value, source);
            NLM_LOG_DEBUG("Config added: " + key + " (source: " + std::to_string(static_cast<int>(source)) + ")");
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to set config key '" + key + "': " + e.what());
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    // Delegate to the main set method
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    // Delegate to the main set method
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    // Delegate to the main set method
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
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
