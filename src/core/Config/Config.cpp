// NLM Configuration Implementation
// Enhanced configuration management with better error handling and validation

#include "core/Config/Config.hpp"
#include <algorithm>
#include <fstream>
#include <sstream>
#include <cstring>
#include <ctime>

namespace nlm {

struct Config::Impl {
    std::map<std::string, ConfigEntry> entries;
    std::map<std::string, std::function<bool(const ConfigValue&)>> validators;
    
    Impl() {
        // Register built-in validators
        validators["int_range"] = [](const ConfigValue& v) {
            return std::get<int>(v) >= -1000000 && std::get<int>(v) <= 1000000;
        };
        validators["int64_range"] = [](const ConfigValue& v) {
            return std::get<int64_t>(v) >= -1000000 && std::get<int64_t>(v) <= 1000000;
        };
        validators["double_range"] = [](const ConfigValue& v) {
            double d = std::get<double>(v);
            return d >= -1000000.0 && d <= 1000000.0;
        };
        validators["float_range"] = [](const ConfigValue& v) {
            float f = std::get<float>(v);
            return f >= -1000000.0f && f <= 1000000.0f;
        };
        validators["positive"] = [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v)) {
                return std::get<double>(v) >= 0.0;
            } else if (std::holds_alternative<float>(v)) {
                return std::get<float>(v) >= 0.0f;
            } else if (std::holds_alternative<int>(v)) {
                return std::get<int>(v) >= 0;
            }
            return false;
        };
        validators["probability"] = [](const ConfigValue& v) {
            float f = std::get<float>(v);
            return f >= 0.0f && f <= 1.0f;
        };
    }
};

Config::Config() : pImpl(new Impl) {}

Config::~Config() = default;

Config::Config(Config&& other) noexcept : pImpl(std::move(other.pImpl)) {}

Config& Config::operator=(Config&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }
        
        json configJson;
        file >> configJson;
        
        // Clear existing entries (except defaults)
        std::map<std::string, ConfigEntry> defaults;
        for (const auto& [key, entry] : pImpl->entries) {
            if (entry.source == ConfigSource::Default) {
                defaults[key] = entry;
            }
        }
        pImpl->entries = std::move(defaults);
        
        // Load from JSON
        for (auto& [key, value] : configJson.items()) {
            if (value.is_number_integer()) {
                set(key, value.get<int>(), ConfigSource::File);
            } else if (value.is_number_unsigned()) {
                set(key, static_cast<int64_t>(value.get<uint64_t>()), ConfigSource::File);
            } else if (value.is_number_float()) {
                set(key, value.get<double>(), ConfigSource::File);
            } else if (value.is_boolean()) {
                set(key, value.get<bool>(), ConfigSource::File);
            } else if (value.is_string()) {
                set(key, value.get<std::string>(), ConfigSource::File);
            } else if (value.is_array()) {
                // Handle array types
                std::vector<double> doubleArray;
                for (auto& item : value) {
                    doubleArray.push_back(item.get<double>());
                }
                set(key, doubleArray, ConfigSource::File);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool foundConfig = false;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--config" && i + 1 < argc) {
            std::string configFile = argv[++i];
            if (loadFromFile(configFile)) {
                foundConfig = true;
            }
        } else if (arg.find("--") == 0) {
            // Parse command line argument: --key=value or --key value
            size_t equalsPos = arg.find('=');
            std::string key, valueStr;
            
            if (equalsPos != std::string::npos) {
                key = arg.substr(2, equalsPos - 2);
                valueStr = arg.substr(equalsPos + 1);
            } else {
                key = arg.substr(2);
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    valueStr = argv[++i];
                }
            }
            
            if (!key.empty() && !valueStr.empty()) {
                parseAndSet(key, valueStr, ConfigSource::CommandLine);
            }
        }
    }
    
    return foundConfig;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    validateKey(key);
    
    ConfigEntry entry(key, value, source);
    validateValue(key, entry.value);
    
    pImpl->entries[key] = std::move(entry);
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    validateKey(key);
    
    ConfigEntry entry(key, ConfigValue(value), source);
    validateValue(key, entry.value);
    
    pImpl->entries[key] = std::move(entry);
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    validateKey(key);
    
    ConfigEntry entry(key, ConfigValue(value), source);
    validateValue(key, entry.value);
    
    pImpl->entries[key] = std::move(entry);
}

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    validateKey(key);
    
    ConfigEntry entry(key, ConfigValue(value), source);
    validateValue(key, entry.value);
    
    pImpl->entries[key] = std::move(entry);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    validateKey(key);
    
    ConfigEntry entry(key, ConfigValue(value), source);
    validateValue(key, entry.value);
    
    pImpl->entries[key] = std::move(entry);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    validateKey(key);
    
    ConfigEntry entry(key, ConfigValue(value), source);
    validateValue(key, entry.value);
    
    pImpl->entries[key] = std::move(entry);
}

std::optional<int> Config::get(const std::string& key) const {
    validateKey(key);
    
    auto it = pImpl->entries.find(key);
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    
    const ConfigValue& value = it->second.value;
    if (std::holds_alternative<int>(value)) {
        return std::get<int>(value);
    }
    return std::nullopt;
}

int Config::getOr(const std::string& key, const int& defaultValue) const {
    auto value = get<int>(key);
    if (value.has_value()) {
        return *value;
    }
    return defaultValue;
}

bool Config::has(const std::string& key) const {
    validateKey(key);
    return pImpl->entries.find(key) != pImpl->entries.end();
}

void Config::remove(const std::string& key) {
    validateKey(key);
    pImpl->entries.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    
    for (const auto& [key, entry] : pImpl->entries) {
        keys.push_back(key);
    }
    
    return keys;
}

void Config::clear() {
    std::map<std::string, ConfigEntry> defaults;
    
    for (const auto& [key, entry] : pImpl->entries) {
        if (entry.source == ConfigSource::Default) {
            defaults[key] = entry;
        }
    }
    
    pImpl->entries = std::move(defaults);
}

std::string Config::summary() const {
    std::stringstream ss;
    ss << "Configuration Summary:" << std::endl;
    
    for (const auto& [key, entry] : pImpl->entries) {
        ss << "  " << key << ": ";
        
        const ConfigValue& value = entry.value;
        std::visit([&ss](const auto& val) {
            if constexpr (std::is_same_v<decltype(val), int>) {
                ss << val;
            } else if constexpr (std::is_same_v<decltype(val), int64_t>) {
                ss << val;
            } else if constexpr (std::is_same_v<decltype(val), double>) {
                ss << val;
            } else if constexpr (std::is_same_v<decltype(val), float>) {
                ss << val;
            } else if constexpr (std::is_same_v<decltype(val), bool>) {
                ss << (val ? "true" : "false");
            } else if constexpr (std::is_same_v<decltype(val), std::string>) {
                ss << "\"" << val << "\"";
            } else if constexpr (std::is_same_v<decltype(val), std::vector<int>>) {
                ss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << val[i];
                }
                ss << "]";
            } else if constexpr (std::is_same_v<decltype(val), std::vector<double>>) {
                ss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << val[i];
                }
                ss << "]";
            } else if constexpr (std::is_same_v<decltype(val), std::vector<std::string>>) {
                ss << "[";
                for (size_t i = 0; i < val.size(); ++i) {
                    if (i > 0) ss << ", ";
                    ss << "\"" << val[i] << "\"";
                }
                ss << "]";
            }
        }, value);
        
        ss << " (source: " << static_cast<int>(entry.source) << ")" << std::endl;
    }
    
    return ss.str();
}

void Config::validateKey(const std::string& key) const {
    if (key.empty()) {
        throw ConfigurationError("Configuration key cannot be empty");
    }
    if (key.find('.') != std::string::npos) {
        throw ConfigurationError("Configuration key cannot contain dots");
    }
}

void Config::validateValue(const std::string& key, const ConfigValue& value) {
    std::string typeName;
    
    if (std::holds_alternative<int>(value)) {
        typeName = "int_range";
    } else if (std::holds_alternative<int64_t>(value)) {
        typeName = "int64_range";
    } else if (std::holds_alternative<double>(value)) {
        typeName = "double_range";
    } else if (std::holds_alternative<float>(value)) {
        typeName = "float_range";
    } else if (std::holds_alternative<bool>(value)) {
        typeName = "bool_range";
    } else if (std::holds_alternative<std::string>(value)) {
        typeName = "string_range";
    }
    
    auto it = pImpl->validators.find(typeName);
    if (it != pImpl->validators.end() && !it->second(value)) {
        throw ConfigurationError("Validation failed for key " + key);
    }
}

void Config::parseAndSet(const std::string& key, const std::string& valueStr, ConfigSource source) {
    // Try to parse as int
    try {
        size_t pos;
        int intValue = std::stoi(valueStr, &pos);
        if (pos == valueStr.length()) {
            set(key, intValue, source);
            return;
        }
    } catch (...) {}
    
    // Try to parse as int64
    try {
        size_t pos;
        int64_t int64Value = std::stoll(valueStr, &pos);
        if (pos == valueStr.length()) {
            set(key, int64Value, source);
            return;
        }
    } catch (...) {}
    
    // Try to parse as double
    try {
        size_t pos;
        double doubleValue = std::stod(valueStr, &pos);
        if (pos == valueStr.length()) {
            set(key, doubleValue, source);
            return;
        }
    } catch (...) {}
    
    // Try to parse as bool
    if (valueStr == "true" || valueStr == "false") {
        bool boolValue = (valueStr == "true");
        set(key, boolValue, source);
        return;
    }
    
    // Otherwise treat as string
    set(key, valueStr, source);
}

} // namespace nlm
