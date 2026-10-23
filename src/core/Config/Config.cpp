#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::string> jsonDefaults;
    bool jsonEnabled;
    
    // Validation configuration
    std::unordered_map<std::string, std::function<bool(const ConfigValue&)>> validators;
    std::unordered_map<std::string, std::string> paramDescriptions;
    bool validationEnabled;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    pImpl->jsonEnabled = false;
    pImpl->validationEnabled = true;
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    // First try to parse as JSON
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    // Try to parse as JSON
    try {
        nlohmann::json jsonData;
        file >> jsonData;
        
        // Successfully parsed as JSON
        return loadFromJSON(jsonData);
    } catch (const nlohmann::json::parse_error&) {
        // Not valid JSON, try legacy format
        file.clear();
        file.seekg(0);
        
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
                
                // Remove quotes if present
                if (value.size() >= 2 && 
                    ((value.front() == '"' && value.back() == '"') ||
                     (value.front() == '\'' && value.back() == '\''))) {
                    value = value.substr(1, value.size() - 2);
                }
                
                set(key, value, ConfigSource::File);
            }
        }
        return true;
    }
}

bool Config::loadFromJSON(const nlohmann::json& jsonData) {
    pImpl->jsonEnabled = true;
    
    // Clear existing entries
    clear();
    
    // Store JSON data for quick access
    if (jsonData.is_object()) {
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            pImpl->jsonDefaults[key] = it.value().dump();
        }
    }
    
    // Convert to our internal format for consistency
    if (jsonData.is_object()) {
        for (auto it = jsonData.begin(); it != jsonData.end(); ++it) {
            std::string key = it.key();
            
            // Handle different JSON value types
            if (it.value().is_string()) {
                set(key, it.value().get<std::string>(), ConfigSource::File);
            } else if (it.value().is_number_integer()) {
                set(key, it.value().get<int64_t>(), ConfigSource::File);
            } else if (it.value().is_number_unsigned()) {
                set(key, it.value().get<uint64_t>(), ConfigSource::File);
            } else if (it.value().is_number()) {
                set(key, it.value().get<double>(), ConfigSource::File);
            } else if (it.value().is_boolean()) {
                set(key, it.value().get<bool>(), ConfigSource::File);
            } else if (it.value().is_array()) {
                // Handle arrays (store as JSON string for now)
                std::string arrayStr = "[";
                for (size_t i = 0; i < it.value().size(); ++i) {
                    if (i > 0) arrayStr += ", ";
                    arrayStr += it.value()[i].dump();
                }
                arrayStr += "]";
                set(key, arrayStr, ConfigSource::File);
            }
        }
    }
    
    return true;
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
        return false;
    }
    
    // Write in JSON format for better compatibility
    nlohmann::json jsonData;
    
    for (const auto& entry : pImpl->entries) {
        // Convert ConfigValue to appropriate JSON type
        jsonData[entry.key] = std::visit([&](auto&& arg) -> nlohmann::json {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                return arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                return arg;
            } else if constexpr (std::is_same_v<T, double>) {
                return arg;
            } else if constexpr (std::is_same_v<T, bool>) {
                return arg;
            } else {
                return arg; // Default to string representation
            }
        }, entry.value);
    }
    
    file << jsonData.dump(2); // Pretty print with indentation
    
    return true;
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    // First check our internal entries
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        try {
            return std::get<T>(it->value);
        } catch (const std::bad_variant_access&) {
            return std::nullopt;
        }
    }
    
    // If JSON enabled, check jsonDefaults
    if (pImpl->jsonEnabled) {
        auto jsonIt = pImpl->jsonDefaults.find(key);
        if (jsonIt != pImpl->jsonDefaults.end()) {
            try {
                nlohmann::json jsonValue = nlohmann::json::parse(jsonIt->second);
                return jsonValue.get<T>();
            } catch (...) {
                // JSON parsing failed, continue to try other sources
            }
        }
    }
    
    return std::nullopt;
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate the value if validation is enabled
    if (pImpl->validationEnabled && !validate(key, value)) {
        std::string errorMsg = "Invalid configuration value for key '" + key + "'.";
        if (auto desc = getParameterDescription(key); !desc.empty()) {
            errorMsg += " Parameter: " + desc;
        }
        NLM_LOG_ERROR(errorMsg);
        // Continue anyway, but log the error
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
    // Check internal entries first
    if (std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; })) {
        return true;
    }
    
    // Check JSON defaults if enabled
    if (pImpl->jsonEnabled) {
        return pImpl->jsonDefaults.find(key) != pImpl->jsonDefaults.end();
    }
    
    return false;
}

void Config::remove(const std::string& key) {
    pImpl->entries.erase(
        std::remove_if(pImpl->entries.begin(),
                      pImpl->entries.end(),
                      [&key](const ConfigEntry& e) { return e.key == key; }),
        pImpl->entries.end()
    );
    
    if (pImpl->jsonEnabled) {
        pImpl->jsonDefaults.erase(key);
    }
    
    // Also remove validation rules
    pImpl->validators.erase(key);
    pImpl->paramDescriptions.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size() + (pImpl->jsonEnabled ? pImpl->jsonDefaults.size() : 0));
    
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    
    if (pImpl->jsonEnabled) {
        for (const auto& pair : pImpl->jsonDefaults) {
            keys.push_back(pair.first);
        }
    }
    
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->jsonDefaults.clear();
    pImpl->jsonEnabled = false;
    pImpl->validators.clear();
    pImpl->paramDescriptions.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " << 
        (pImpl->jsonEnabled ? pImpl->jsonDefaults.size() : 0) << " JSON defaults):\n";
    
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
    
    if (pImpl->jsonEnabled && !pImpl->jsonDefaults.empty()) {
        oss << "\nJSON Defaults:\n";
        for (const auto& pair : pImpl->jsonDefaults) {
            oss << "  " << pair.first << " = " << pair.second << "\n";
        }
    }
    
    return oss.str();
}

std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
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

// Validation implementation
bool Config::validate(const std::string& key, const ConfigValue& value) const {
    if (!pImpl->validationEnabled) return true;
    
    auto it = pImpl->validators.find(key);
    if (it != pImpl->validators.end()) {
        return it->second(value);
    }
    
    // Default validation based on type
    return validateDefault(key, value);
}

bool Config::validateDefault(const std::string& key, const ConfigValue& value) const {
    try {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, int64_t>) {
                // Validate integer values
                int64_t intVal = arg;
                if (intVal < -1000000 || intVal > 1000000) {
                    throw std::runtime_error("Integer value out of reasonable range");
                }
            } else if constexpr (std::is_same_v<T, double>) {
                // Validate float values
                double floatVal = arg;
                if (std::isnan(floatVal) || std::isinf(floatVal)) {
                    throw std::runtime_error("Float value is NaN or infinity");
                }
                if (floatVal < -1e6 || floatVal > 1e6) {
                    throw std::runtime_error("Float value out of reasonable range");
                }
            } else if constexpr (std::is_same_v<T, std::string>) {
                // Validate string values
                if (arg.size() > 1024) {
                    throw std::runtime_error("String value too long (max 1024 chars)");
                }
            }
        }, value);
        return true;
    } catch (...) {
        return false;
    }
}

bool Config::validateAll() const {
    if (!pImpl->validationEnabled) return true;
    
    for (const auto& entry : pImpl->entries) {
        if (!validateDefault(entry.key, entry.value)) {
            NLM_LOG_ERROR("Config validation failed for key: " + entry.key);
            return false;
        }
    }
    
    return true;
}

void Config::setParameterDescription(const std::string& key, const std::string& description) {
    pImpl->paramDescriptions[key] = description;
}

std::string Config::getParameterDescription(const std::string& key) const {
    auto it = pImpl->paramDescriptions.find(key);
    return it != pImpl->paramDescriptions.end() ? it->second : "";
}

void Config::enableValidation(bool enable) {
    pImpl->validationEnabled = enable;
}

bool Config::isValidationEnabled() const {
    return pImpl->validationEnabled;
}

void Config::setValidator(const std::string& key, std::function<bool(const ConfigValue&)> validator) {
    pImpl->validators[key] = validator;
}

void Config::setRangeValidation(const std::string& key, double min, double max) {
    setValidator(key, [min, max](const ConfigValue& value) {
        try {
            return std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double>) {
                    return arg >= min && arg <= max;
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    return static_cast<double>(arg) >= min && static_cast<double>(arg) <= max;
                } else {
                    return false; // Type doesn't match
                }
            }, value);
        } catch (...) {
            return false;
        }
    });
}

void Config::setAllowedValuesValidation(const std::string& key, const std::vector<double>& allowedValues) {
    setValidator(key, [allowedValues](const ConfigValue& value) {
        try {
            return std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double>) {
                    return std::find(allowedValues.begin(), allowedValues.end(), arg) != allowedValues.end();
                } else if constexpr (std::is_same_v<T, int64_t>) {
                    double val = static_cast<double>(arg);
                    return std::find(allowedValues.begin(), allowedValues.end(), val) != allowedValues.end();
                } else {
                    return false;
                }
            }, value);
        } catch (...) {
            return false;
        }
    });
}

void Config::setIntRangeValidation(const std::string& key, int64_t min, int64_t max) {
    setValidator(key, [min, max](const ConfigValue& value) {
        try {
            return std::get<int64_t>(value) >= min && std::get<int64_t>(value) <= max;
        } catch (...) {
            return false;
        }
    });
}

void Config::setStringLengthValidation(const std::string& key, size_t minLength, size_t maxLength) {
    setValidator(key, [minLength, maxLength](const ConfigValue& value) {
        try {
            std::string str = std::get<std::string>(value);
            return str.size() >= minLength && str.size() <= maxLength;
        } catch (...) {
            return false;
        }
    });
}

} // namespace nlm