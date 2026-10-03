#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <iostream>
#include <unordered_set>
#include <functional>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open configuration file: " << filepath << std::endl;
        return false;
    }
    
    try {
        // Try JSON parsing first (prefered format)
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Clear existing entries before loading
        pImpl->entries.clear();
        
        // Parse JSON and populate entries
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            // Get value based on type
            if (it->is_boolean()) {
                set(key, it->get<bool>(), ConfigSource::File);
            } else if (it->is_number_integer()) {
                set(key, it->get<int64_t>(), ConfigSource::File);
            } else if (it->is_number_float()) {
                set(key, it->get<double>(), ConfigSource::File);
            } else if (it->is_string()) {
                set(key, it->get<std::string>(), ConfigSource::File);
            } else if (it->is_array()) {
                // Handle arrays
                if (it->empty()) {
                    set(key, std::vector<std::string>(), ConfigSource::File);
                } else {
                    // Determine array element type
                    if (it->front().is_boolean()) {
                        std::vector<bool> boolArray;
                        for (const auto& item : *it) {
                            boolArray.push_back(item.get<bool>());
                        }
                        set(key, boolArray, ConfigSource::File);
                    } else if (it->front().is_number_integer()) {
                        std::vector<int64_t> intArray;
                        for (const auto& item : *it) {
                            intArray.push_back(item.get<int64_t>());
                        }
                        set(key, intArray, ConfigSource::File);
                    } else if (it->front().is_number_float()) {
                        std::vector<double> doubleArray;
                        for (const auto& item : *it) {
                            doubleArray.push_back(item.get<double>());
                        }
                        set(key, doubleArray, ConfigSource::File);
                    } else {
                        std::vector<std::string> stringArray;
                        for (const auto& item : *it) {
                            stringArray.push_back(item.get<std::string>());
                        }
                        set(key, stringArray, ConfigSource::File);
                    }
                }
            }
        }
        
        return true;
    } catch (const nlohmann::json::parse_error& e) {
        // Fallback to simple key=value format for backward compatibility
        file.clear();
        file.seekg(0);
        
        std::string line;
        bool hasErrors = false;
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
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            } else {
                std::cerr << "WARNING: Invalid configuration line (no '='): " << line << std::endl;
                hasErrors = true;
            }
        }
        
        // If there were parse errors but we still loaded some valid lines, return true
        return !hasErrors || !pImpl->entries.empty();
    } catch (const std::exception& e) {
        std::cerr << "ERROR: Exception while parsing configuration file: " << e.what() << std::endl;
        return false;
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
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "ERROR: Could not open configuration file for writing: " << filepath << std::endl;
        return false;
    }
    
    file << "# NLM Configuration File (JSON format)\n";
    file << "# Generated by NLM Configuration System\n\n";
    
    // Write configuration in JSON format for better readability and tooling support
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        // Visit the value based on its type and write appropriate JSON
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, double>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                jsonData[entry.key] = arg;
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                jsonData[entry.key] = arg;
            } else {
                // Default fallback - convert to string
                jsonData[entry.key] = std::to_string(arg);
            }
        }, entry.value);
    }
    
    // Write JSON with indentation for readability
    file << jsonData.dump(2);
    file << "\n";
    
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

// Global validator instance
class ConfigValidator {
private:
    std::vector<ValidationRule> validationRules_;
    std::unordered_set<std::string> requiredKeys_;
    bool typeValidationEnabled_ = true;
    
    // Helper function to get value from config for validation
    template<typename T>
    std::optional<T> getConfigValue(const Config& config, const std::string& key) const {
        auto value = config.get<T>(key);
        return value;
    }
    
    // Helper to convert variant to string for comparison
    std::string variantToString(const ConfigValue& value) const {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::fixed << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, std::vector<bool>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << (arg[i] ? "true" : "false");
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << std::fixed << std::setprecision(6) << arg[i];
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    oss << "\"" << arg[i] << "\"";
                    if (i < arg.size() - 1) oss << ", ";
                }
                oss << "]";
            } else {
                oss << "\"" << arg << "\"";
            }
        }, value);
        return oss.str();
    }
public:
    ConfigValidator() = default;
    ~ConfigValidator() = default;
    
    // Add validation rule
    void addValidationRule(const std::string& key, std::function<bool(const std::string&, const ConfigValue&, const std::string&)> callback, const std::string& errorMessage) {
        validationRules_.push_back({key, callback, errorMessage});
    }
    
    // Validate entire configuration
    std::vector<std::string> validate(const Config& config) const {
        std::vector<std::string> errors;
        
        // Check required keys
        for (const auto& requiredKey : requiredKeys_) {
            if (!config.has(requiredKey)) {
                errors.push_back("Missing required key: " + requiredKey);
            }
        }
        
        // Apply custom validation rules
        for (const auto& rule : validationRules_) {
            if (config.has(rule.key)) {
                auto value = config.get<ConfigValue>(rule.key);
                if (value.has_value()) {
                    bool isValid = rule.callback(rule.key, value.value(), rule.errorMessage);
                    if (!isValid) {
                        errors.push_back(rule.errorMessage);
                    }
                }
            }
        }
        
        return errors;
    }
    
    // Clear all validation rules
    void clearValidationRules() {
        validationRules_.clear();
        requiredKeys_.clear();
    }
    
    // Set required keys
    void setRequiredKeys(const std::unordered_set<std::string>& requiredKeys) {
        requiredKeys_ = requiredKeys;
    }
    
    // Get required keys
    const std::unordered_set<std::string>& getRequiredKeys() const {
        return requiredKeys_;
    }
    
    // Set type validation
    void enableTypeValidation(bool enable) {
        typeValidationEnabled_ = enable;
    }
    
    // Check if type validation is enabled
    bool isTypeValidationEnabled() const {
        return typeValidationEnabled_;
    }
    
    // Add range validation
    void addRangeValidation(const std::string& key, double minValue, double maxValue, const std::string& errorMessage) {
        addValidationRule(key, 
            [minValue, maxValue](const std::string&, const ConfigValue& value, const std::string&) {
                double val = 0.0;
                std::visit([&](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, int64_t>) {
                        val = static_cast<double>(arg);
                    } else if constexpr (std::is_same_v<T, double>) {
                        val = arg;
                    }
                }, value);
                return val >= minValue && val <= maxValue;
            }, errorMessage);
    }
    
    // Add string length validation
    void addStringLengthValidation(const std::string& key, size_t minLength, size_t maxLength, const std::string& errorMessage) {
        addValidationRule(key,
            [minLength, maxLength](const std::string&, const ConfigValue& value, const std::string&) {
                std::string val;
                std::visit([&](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        val = arg;
                    } else {
                        val = std::to_string(arg);
                    }
                }, value);
                return val.length() >= minLength && val.length() <= maxLength;
            }, errorMessage);
    }
    
    // Add enum validation
    void addEnumValidation(const std::string& key, const std::vector<std::string>& allowedValues, const std::string& errorMessage) {
        addValidationRule(key,
            [allowedValues](const std::string&, const ConfigValue& value, const std::string&) {
                std::string val;
                std::visit([&](auto&& arg) {
                    using T = std::decay_t<decltype(arg)>;
                    if constexpr (std::is_same_v<T, std::string>) {
                        val = arg;
                    } else {
                        val = std::to_string(arg);
                    }
                }, value);
                
                for (const auto& allowed : allowedValues) {
                    if (val == allowed) {
                        return true;
                    }
                }
                return false;
            }, errorMessage);
    }
};

// Global validator instance
ConfigValidator& getConfigValidator() {
    static ConfigValidator validator;
    return validator;
}

// Convenience function for validation
bool validateConfig(const Config& config) {
    return getConfigValidator().validate(config).empty();
}

std::vector<std::string> getConfigValidationErrors(const Config& config) {
    return getConfigValidator().validate(config);
}

#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <iostream>
#include <unordered_set>
#include <functional>
#include <filesystem>

namespace nlm {

// Global validator instance
ConfigValidator& getConfigValidator() {
    static ConfigValidator validator;
    return validator;
}

// Convenience function for validation
bool validateConfig(const Config& config) {
    return getConfigValidator().validate(config).empty();
}

std::vector<std::string> getConfigValidationErrors(const Config& config) {
    return getConfigValidator().validate(config);
}