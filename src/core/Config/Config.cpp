#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <functional>
#include <numeric>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Configuration validation rules
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validationRules;
    
    Impl() : entries() {
        // Initialize validation rules with common configuration patterns
        initializeValidationRules();
    }
    
    void initializeValidationRules() {
        // Define common configuration validation rules
        validationRules["neuron_count"] = [](const ConfigValue& v) {
            if (const auto* val = std::get_if<size_t>(&v)) {
                return *val > 0 && *val <= 100000;  // Reasonable limits
            }
            return false;
        };
        
        validationRules["region_count"] = [](const ConfigValue& v) {
            if (const auto* val = std::get_if<size_t>(&v)) {
                return *val > 0 && *val <= 1000;
            }
            return false;
        };
        
        validationRules["connection_probability"] = [](const ConfigValue& v) {
            if (const auto* val = std::get_if<double>(&v)) {
                return *val >= 0.0 && *val <= 1.0;
            }
            return false;
        };
        
        validationRules["simulation_timestep"] = [](const ConfigValue& v) {
            if (const auto* val = std::get_if<double>(&v)) {
                return *val > 0.0 && *val < 1.0;
            }
            return false;
        };
        
        validationRules["random_seed"] = [](const ConfigValue& v) {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_integral_v<T>) {
                    return arg >= 0;
                } else if constexpr (std::is_same_v<T, std::string>) {
                    return !arg.empty();
                }
                return false;
            }, v);
        };
    }
    
    bool validateConfigEntry(const ConfigEntry& entry) const {
        auto it = validationRules.find(entry.key);
        if (it != validationRules.end()) {
            return it->second(entry.value);
        }
        return true;  // No validation rule defined, assume valid
    }
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open config file: " << filepath << std::endl;
        return false;
    }
    
    // Clear existing entries
    clear();
    
    std::string line;
    int lineNum = 0;
    bool hasValidEntries = false;
    
    while (std::getline(file, line)) {
        ++lineNum;
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Enhanced parsing for key=value format
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
            
            // Parse value based on type
            ConfigValue configValue = parseConfigValue(value);
            if (configValue.has_value()) {
                ConfigEntry entry(key, configValue, ConfigSource::File);
                
                // Validate the configuration entry
                if (pImpl->validateConfigEntry(entry)) {
                    pImpl->entries.push_back(entry);
                    hasValidEntries = true;
                } else {
                    std::cerr << "Warning: Invalid configuration value for key '" << key 
                              << "' in file " << filepath << " line " << lineNum 
                              << ". Entry skipped." << std::endl;
                }
            } else {
                std::cerr << "Warning: Cannot parse configuration value for key '" << key 
                          << "' in file " << filepath << " line " << lineNum 
                          << ". Entry skipped." << std::endl;
            }
        } else {
            std::cerr << "Warning: Invalid format in config file " << filepath 
                      << " line " << lineNum << ". Line must be in format key=value" << std::endl;
        }
    }
    
    if (!hasValidEntries && !pImpl->entries.empty()) {
        std::cerr << "Warning: No valid configuration entries found in file " << filepath << std::endl;
    }
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (argc <= 1) return true;  // No arguments
    
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
                std::cerr << "Warning: Invalid command line argument format: " << arg 
                          << ". Use --key=value format." << std::endl;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            set(key, value, ConfigSource::CommandLine);
        } else {
            std::cerr << "Warning: Invalid command line argument: " << arg << std::endl;
        }
    }
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open config file for writing: " << filepath << std::endl;
        return false;
    }
    
    file << "# NLM Configuration File (Generated)" << std::endl;
    file << "# Source: " << static_cast<int>(ConfigSource::File) << std::endl;
    file << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << std::endl;
        file << entry.key << " = " << serializeConfigValue(entry.value) << std::endl;
    }
    
    return true;
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
    if (key.empty() || key.find(' ') != std::string::npos) {
        std::cerr << "Warning: Invalid configuration key: '" << key << "'. Key cannot be empty or contain spaces." << std::endl;
        return;
    }
    
    // Create new entry
    ConfigEntry entry(key, value, source);
    
    // Validate the entry if validation rules exist
    if (!pImpl->validateConfigEntry(entry)) {
        std::cerr << "Warning: Invalid configuration value for key '" << key 
                  << "'. Value will not be set." << std::endl;
        return;
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
    oss << "NLM Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    oss << "===============================\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "Key: " << entry.key << "\n";
        oss << "  Value: [";
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
        oss << "] (Source: " << static_cast<int>(entry.source) << ")\n";
        oss << "  Description: " << entry.description << "\n";
    }
    
    return oss.str();
}

ConfigValue Config::parseConfigValue(const std::string& str) {
    // Try to parse as boolean first
    if (str == "true" || str == "1" || str == "yes" || str == "on") {
        return true;
    } else if (str == "false" || str == "0" || str == "no" || str == "off") {
        return false;
    }
    
    // Try to parse as integer
    try {
        size_t pos;
        int intVal = std::stoi(str, &pos);
        if (pos == str.size()) {
            return intVal;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleVal = std::stod(str, &pos);
        if (pos == str.size()) {
            return doubleVal;
        }
    } catch (...) {}
    
    // Try to parse as int64_t for larger integers
    try {
        size_t pos;
        int64_t int64Val = std::stoll(str, &pos);
        if (pos == str.size()) {
            return int64Val;
        }
    } catch (...) {}
    
    // Try to parse as vector (comma-separated)
    if (str.find(',') != std::string::npos) {
        std::vector<double> vec;
        std::stringstream ss(str);
        std::string item;
        while (std::getline(ss, item, ',')) {
            item = trim(item);
            try {
                double val = std::stod(item);
                vec.push_back(val);
            } catch (...) {
                // Skip invalid items
            }
        }
        if (!vec.empty()) {
            return vec;
        }
    }
    
    // Return as string (fallback)
    return str;
}

std::string Config::serializeConfigValue(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else if constexpr (std::is_same_v<T, bool>) {
            oss << (arg ? "true" : "false");
        } else if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
            oss << arg;
        } else if constexpr (std::is_floating_point_v<T>) {
            oss << arg;
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
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            oss << "[";
            for (size_t i = 0; i < arg.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << arg[i] << "\"";
            }
            oss << "]";
        } else {
            oss << "\"unknown type\"";
        }
    }, value);
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