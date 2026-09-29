// Config.cpp - Enhanced with comprehensive error handling and validation

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <iostream>
#include <unordered_map>

namespace nlm {

// Error handling utilities
struct ConfigError : public std::runtime_error {
    explicit ConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

struct ConfigValidationError : public ConfigError {
    explicit ConfigValidationError(const std::string& msg) : ConfigError(msg) {}
};

// Configuration value validation rules
struct ValidationRule {
    std::string key;
    std::function<bool(const ConfigValue&)> validator;
    std::string errorMessage;
};

// Global validation rules
std::vector<ValidationRule> getDefaultValidationRules() {
    return {
        {"neuron_count", 
         [](const ConfigValue& v) { 
             return std::visit([](auto&& arg) { 
                 return std::is_same_v<decltype(arg), size_t> && arg > 0; 
             }, v); 
         }, 
         "neuron_count must be a positive integer"},
        {"max_steps",
         [](const ConfigValue& v) {
             return std::visit([](auto&& arg) {
                 return std::is_same_v<decltype(arg), uint64_t> && arg > 0;
             }, v);
         },
         "max_steps must be a positive integer"},
        {"simulation_timestep",
         [](const ConfigValue& v) {
             return std::visit([](auto&& arg) {
                 return std::is_same_v<decltype(arg), double> && arg > 0.0 && arg <= 1.0;
             }, v);
         },
         "simulation_timestep must be between 0 and 1"},
        {"region_count",
         [](const ConfigValue& v) {
             return std::visit([](auto&& arg) {
                 return std::is_same_v<decltype(arg), size_t> && arg > 0;
             }, v);
         },
         "region_count must be a positive integer"},
        {"connection_probability",
         [](const ConfigValue& v) {
             return std::visit([](auto&& arg) {
                 return std::is_same_v<decltype(arg), float> && arg >= 0.0f && arg <= 1.0f;
             }, v);
         },
         "connection_probability must be between 0 and 1"},
        {"random_seed",
         [](const ConfigValue& v) {
             return std::visit([](auto&& arg) {
                 return std::is_same_v<decltype(arg), uint64_t>;
             }, v);
         },
         "random_seed must be an unsigned integer"},
    };
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::vector<ValidationRule>> customValidators;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
    
    Impl() : warnings(), errors() {}
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Set up default validation rules
    pImpl->customValidators["global"] = getDefaultValidationRules();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot open configuration file: " << filepath << std::endl;
        return false;
    }
    
    std::cout << "Loading configuration from: " << filepath << std::endl;
    
    std::string line;
    int lineNumber = 0;
    while (std::getline(file, line)) {
        lineNumber++;
        
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
                std::cerr << "WARNING: Error parsing line " << lineNumber 
                         << " in config file: " << e.what() << std::endl;
                pImpl->warnings.push_back("Config parse error at line " + std::to_string(lineNumber) + ": " + e.what());
            }
        } else {
            std::cerr << "WARNING: Invalid line format at line " << lineNumber 
                     << ": " << line << std::endl;
            pImpl->warnings.push_back("Invalid format at line " + std::to_string(lineNumber));
        }
    }
    
    // Validate loaded configuration
    validate();
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    std::cout << "Loading configuration from command line arguments" << std::endl;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                try {
                    set(key, value, ConfigSource::CommandLine);
                } catch (const std::exception& e) {
                    std::cerr << "ERROR: Invalid command line argument: " << arg << std::endl;
                    std::cerr << "  " << e.what() << std::endl;
                    return false;
                }
            } else {
                std::cerr << "ERROR: Invalid command line argument format: " << arg << std::endl;
                std::cerr << "  Expected --key=value format" << std::endl;
                return false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            try {
                set(key, value, ConfigSource::CommandLine);
            } catch (const std::exception& e) {
                std::cerr << "ERROR: Invalid command line argument: -" << key << " " << value << std::endl;
                std::cerr << "  " << e.what() << std::endl;
                return false;
            }
        }
        else {
            std::cerr << "WARNING: Ignoring unrecognized command line argument: " << arg << std::endl;
            pImpl->warnings.push_back("Ignored argument: " + arg);
        }
    }
    
    // Validate loaded configuration
    if (!validate()) {
        std::cerr << "ERROR: Configuration validation failed" << std::endl;
        return false;
    }
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Cannot save configuration to file: " << filepath << std::endl;
        return false;
    }
    
    std::cout << "Saving configuration to: " << filepath << std::endl;
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = " << "[PROTECTED_VALUE]\n";
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
        std::cerr << "ERROR: Type mismatch for key \"" << key << "\"" << std::endl;
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    } else {
        std::cout << "WARNING: Using default value for \"" << key 
                 << "\": " << defaultValue << std::endl;
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate key
    if (key.empty()) {
        throw ConfigError("Key cannot be empty");
    }
    
    if (key.find(' ') != std::string::npos) {
        throw ConfigError("Key cannot contain spaces: \"" + key + "\"");
    }
    
    // Validate value if we have validation rules
    auto globalRules = pImpl->customValidators["global"];
    for (const auto& rule : globalRules) {
        if (rule.key == key) {
            if (!rule.validator(value)) {
                throw ConfigValidationError(rule.errorMessage);
            }
            break;
        }
    }
    
    // Also check custom validators
    auto customIt = pImpl->customValidators.find(key);
    if (customIt != pImpl->customValidators.end()) {
        for (const auto& rule : customIt->second) {
            if (!rule.validator(value)) {
                throw ConfigValidationError("Key \"" + key + "\": " + rule.errorMessage);
            }
        }
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
        std::cout << "Updated configuration: " << key << " = " << valueToString(value) << std::endl;
    } else {
        pImpl->entries.emplace_back(key, value, source);
        std::cout << "Set configuration: " << key << " = " << valueToString(value) << std::endl;
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

bool Config::validate() {
    bool isValid = true;
    
    // Get all validation rules
    std::vector<ValidationRule> allRules;
    
    // Add global rules
    auto globalIt = pImpl->customValidators.find("global");
    if (globalIt != pImpl->customValidators.end()) {
        allRules.insert(allRules.end(), globalIt->second.begin(), globalIt->second.end());
    }
    
    // Add custom rules for each entry
    for (const auto& entry : pImpl->entries) {
        auto customIt = pImpl->customValidators.find(entry.key);
        if (customIt != pImpl->customValidators.end()) {
            allRules.insert(allRules.end(), customIt->second.begin(), customIt->second.end());
        }
    }
    
    // Apply validation rules
    for (const auto& rule : allRules) {
        auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
            [&rule](const ConfigEntry& e) { return e.key == rule.key; });
        
        if (it != pImpl->entries.end()) {
            if (!rule.validator(it->value)) {
                std::cerr << "VALIDATION ERROR: " << rule.errorMessage << std::endl;
                pImpl->errors.push_back(rule.errorMessage);
                isValid = false;
            }
        }
    }
    
    // Check for duplicate keys
    std::vector<std::string> seenKeys;
    for (const auto& entry : pImpl->entries) {
        if (std::find(seenKeys.begin(), seenKeys.end(), entry.key) != seenKeys.end()) {
            std::cerr << "VALIDATION ERROR: Duplicate key: \"" << entry.key << "\"" << std::endl;
            pImpl->errors.push_back("Duplicate key: " + entry.key);
            isValid = false;
        }
        seenKeys.push_back(entry.key);
    }
    
    // Show warnings if any
    if (!pImpl->warnings.empty()) {
        std::cout << "=== Configuration Warnings ===" << std::endl;
        for (const auto& warning : pImpl->warnings) {
            std::cout << "  WARNING: " << warning << std::endl;
        }
        std::cout << "================================" << std::endl << std::endl;
    }
    
    // Show errors if any
    if (!pImpl->errors.empty()) {
        std::cout << "=== Configuration Errors ===" << std::endl;
        for (const auto& error : pImpl->errors) {
            std::cout << "  ERROR: " << error << std::endl;
        }
        std::cout << "===========================" << std::endl << std::endl;
    }
    
    return isValid;
}

void Config::addValidationRule(const std::string& key, std::function<bool(const ConfigValue&)> validator,
                               const std::string& errorMessage) {
    ValidationRule rule;
    rule.key = key;
    rule.validator = validator;
    rule.errorMessage = errorMessage;
    
    pImpl->customValidators[key].push_back(rule);
}

void Config::removeValidationRule(const std::string& key, const std::string& errorMessage) {
    auto it = pImpl->customValidators.find(key);
    if (it != pImpl->customValidators.end()) {
        auto& rules = it->second;
        rules.erase(std::remove_if(rules.begin(), rules.end(),
            [&errorMessage](const ValidationRule& r) { return r.errorMessage == errorMessage; }),
            rules.end());
    }
}

std::string Config::valueToString(const ConfigValue& value) {
    std::ostringstream oss;
    std::visit([&oss](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            oss << "\"" << arg << "\"";
        } else {
            oss << arg;
        }
    }, value);
    return oss.str();
}

bool Config::hasValidationErrors() const {
    return !pImpl->errors.empty();
}

bool Config::hasWarnings() const {
    return !pImpl->warnings.empty();
}

std::vector<std::string> Config::getValidationErrors() const {
    return pImpl->errors;
}

std::vector<std::string> Config::getWarnings() const {
    return pImpl->warnings;
}

void Config::clearValidationErrors() {
    pImpl->errors.clear();
}

void Config::clearWarnings() {
    pImpl->warnings.clear();
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