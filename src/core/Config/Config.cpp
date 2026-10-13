// ==============================================================================
// Improved Configuration System - Enhanced Error Handling and Validation
// ==============================================================================

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <stdexcept>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    bool validateValues_;  // Enable/disable value validation
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->validateValues_ = true;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_WARNING("Could not open configuration file: " + filepath);
            return false;
        }
        
        std::string line;
        int lineNumber = 0;
        while (std::getline(file, line)) {
            lineNumber++;
            // Skip empty lines and comments
            line = trim(line);
            if (line.empty() || line[0] == '#' || line[0] == '/') {
                continue;
            }
            
            // Parse simple key=value pairs with improved error handling
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = trim(line.substr(0, pos));
                std::string value = trim(line.substr(pos + 1));
                
                if (key.empty()) {
                    NLM_LOG_WARNING("Empty key in configuration file: " + filepath + " at line " + std::to_string(lineNumber));
                    continue;
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
                    NLM_LOG_ERROR("Error parsing configuration line " + std::to_string(lineNumber) + 
                                " in file " + filepath + ": " + std::string(e.what()));
                    return false;
                }
            } else {
                NLM_LOG_WARNING("Invalid configuration line (no '=') in file " + filepath + 
                            " at line " + std::to_string(lineNumber));
            }
        }
        
        NLM_LOG_INFO("Loaded configuration from file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading configuration file " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Handle --key=value format with better error handling
            if (arg.size() >= 3 && arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != std::string::npos) {
                    std::string key = arg.substr(2, pos - 2);
                    std::string value = arg.substr(pos + 1);
                    
                    if (key.empty()) {
                        NLM_LOG_WARNING("Empty key in command line argument");
                        continue;
                    }
                    
                    set(key, value, ConfigSource::CommandLine);
                }
            }
            // Handle -key value format with better error handling
            else if (arg.size() >= 2 && arg[0] == '-' && i + 1 < argc) {
                std::string key = arg.substr(1);
                std::string value = argv[++i];
                
                if (key.empty()) {
                    NLM_LOG_WARNING("Empty key in command line argument");
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
            }
        }
        
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception loading configuration from command line: " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Could not open configuration file for writing: " + filepath);
            return false;
        }
        
        file << "# NLM Configuration File" << std::endl;
        file << "# Generated on " << std::time(nullptr) << std::endl;
        file << std::endl;
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = ";
            
            // Write value with proper formatting based on type
            std::visit([&file](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    file << '"' << arg << '"';
                } else if constexpr (std::is_same_v<T, bool>) {
                    file << (arg ? "true" : "false");
                } else {
                    file << arg;
                }
            }, entry.value);
            
            file << "\n\n";
        }
        
        NLM_LOG_INFO("Saved configuration to file: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Exception saving configuration to file " + filepath + ": " + std::string(e.what()));
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
        T value = std::get<T>(it->value);
        
        // Validate value if validation is enabled
        if (pImpl->validateValues_) {
            validateValue(key, value);
        }
        
        return value;
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Type mismatch for configuration key: " + key);
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    try {
        // Validate key format
        if (key.empty()) {
            throw std::invalid_argument("Configuration key cannot be empty");
        }
        
        // Check if key is valid (no spaces, special characters)
        if (std::any_of(key.begin(), key.end(), ::isspace)) {
            throw std::invalid_argument("Configuration key cannot contain spaces");
        }
        
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&key](const ConfigEntry& e) { return e.key == key; });
        
        if (it != pImpl->entries.end()) {
            // Key exists, update value
            it->value = value;
            it->source = source;
        } else {
            // New key, add entry
            std::string description = "Configuration parameter: " + key;
            pImpl->entries.emplace_back(key, value, source, description);
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error setting configuration key '" + key + "': " + std::string(e.what()));
        throw; // Re-throw to let caller handle it
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
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
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
        oss << "] (Source: " << static_cast<int>(entry.source) << ")\n";
    }
    return oss.str();
}

void Config::validateValue(const std::string& key, const ConfigValue& value) const {
    // Validate configuration values based on key
    try {
        if (key == "neuron_count") {
            auto val = std::get<int64_t>(value);
            if (val <= 0 || val > 100000) {
                throw std::invalid_argument("neuron_count must be between 1 and 100000");
            }
        } else if (key == "connection_probability") {
            auto val = std::get<double>(value);
            if (val < 0.0 || val > 1.0) {
                throw std::invalid_argument("connection_probability must be between 0.0 and 1.0");
            }
        } else if (key == "simulation_timestep") {
            auto val = std::get<double>(value);
            if (val <= 0.0 || val > 1.0) {
                throw std::invalid_argument("simulation_timestep must be between 0.0 and 1.0");
            }
        } else if (key == "random_seed") {
            auto val = std::get<int64_t>(value);
            if (val < 0) {
                throw std::invalid_argument("random_seed must be non-negative");
            }
        }
    } catch (const std::bad_variant_access&) {
        // Type validation error - key type doesn't match value type
        throw std::invalid_argument("Type mismatch for configuration key: " + key);
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

// Explicit template instantiations
// Basic types
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

// Container types
template std::optional<std::vector<int>> Config::get<std::vector<int>>(const std::string&) const;
template std::optional<std::vector<double>> Config::get<std::vector<double>>(const std::string&) const;
template std::optional<std::vector<std::string>> Config::get<std::vector<std::string>>(const std::string&) const;

// getOr methods

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

// Container getOr methods

template std::vector<int> Config::getOr<std::vector<int>>(const std::string&, const std::vector<int>&) const;
template std::vector<double> Config::getOr<std::vector<double>>(const std::string&, const std::vector<double>&) const;
template std::vector<std::string> Config::getOr<std::vector<std::string>>(const std::string&, const std::vector<std::string>&) const;

} // namespace nlm