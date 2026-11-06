#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <cmath>

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
        NLM_LOG_WARN("Configuration file not found: " + filepath);
        return false;
    }
    
    // Determine file format based on extension
    std::string ext = "." + std::filesystem::path(filepath).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    
    bool success = false;
    if (ext == ".json") {
        success = loadFromJson(filepath);
    } else if (ext == ".yaml" || ext == ".yml") {
        success = loadFromYaml(filepath);
    } else {
        // Fallback to simple key=value format for backward compatibility
        success = loadFromSimpleFormat(filepath);
    }
    
    if (success) {
        NLM_LOG_INFO("Loaded configuration from file: " + filepath);
    } else {
        NLM_LOG_WARN("Failed to load configuration from file: " + filepath);
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
    if (!loadFromArgsComplete(argc, argv)) {
        NLM_LOG_WARN("Command line argument parsing failed");
    }
    return true;
}

// Json helpers
static bool parseJsonValue(const nlohmann::json& jsonValue, ConfigValue& configValue) {
    if (jsonValue.is_string()) {
        configValue = jsonValue.get<std::string>();
    } else if (jsonValue.is_number_integer()) {
        configValue = jsonValue.get<int64_t>();
    } else if (jsonValue.is_number_unsigned()) {
        configValue = jsonValue.get<uint64_t>();
    } else if (jsonValue.is_number()) {
        configValue = jsonValue.get<double>();
    } else if (jsonValue.is_boolean()) {
        configValue = jsonValue.get<bool>();
    } else if (jsonValue.is_array()) {
        // Determine array type
        if (!jsonValue.empty()) {
            if (jsonValue[0].is_string()) {
                std::vector<std::string> strVec;
                for (const auto& item : jsonValue) {
                    strVec.push_back(item.get<std::string>());
                }
                configValue = strVec;
            } else if (jsonValue[0].is_number_integer()) {
                std::vector<int64_t> intVec;
                for (const auto& item : jsonValue) {
                    intVec.push_back(item.get<int64_t>());
                }
                configValue = intVec;
            } else if (jsonValue[0].is_number()) {
                std::vector<double> doubleVec;
                for (const auto& item : jsonValue) {
                    doubleVec.push_back(item.get<double>());
                }
                configValue = doubleVec;
            }
        }
    }
    return true;
}

static nlohmann::json configValueToJson(const ConfigValue& configValue) {
    nlohmann::json jsonValue;
    std::visit([&](auto&& value) {
        using T = std::decay_t<decltype(value)>;
        if constexpr (std::is_same_v<T, std::string>) {
            jsonValue = value;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            jsonValue = value;
        } else if constexpr (std::is_same_v<T, uint64_t>) {
            jsonValue = value;
        } else if constexpr (std::is_same_v<T, double>) {
            jsonValue = value;
        } else if constexpr (std::is_same_v<T, bool>) {
            jsonValue = value;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            jsonValue = value;
        } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
            jsonValue = value;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            jsonValue = value;
        }
    }, configValue);
    return jsonValue;
}

// Add new methods for JSON/YAML support
bool Config::loadFromJson(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        if (!jsonConfig.is_object()) {
            NLM_LOG_ERROR("Invalid JSON format in configuration file: " + filepath);
            return false;
        }
        
        clear(); // Clear existing config
        
        for (const auto& [key, value] : jsonConfig.items()) {
            ConfigValue configValue;
            if (parseJsonValue(value, configValue)) {
                set(key, configValue, ConfigSource::File);
            } else {
                NLM_LOG_WARN("Failed to parse JSON value for key: " + key);
            }
        }
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON parsing error in " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::loadFromYaml(const std::string& filepath) {
    // For now, fall back to simple format
    // TODO: Add YAML support with a YAML library
    return loadFromSimpleFormat(filepath);
}

bool Config::saveToJson(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    try {
        nlohmann::json jsonConfig;
        
        for (const auto& entry : pImpl->entries) {
            jsonConfig[entry.key] = configValueToJson(entry.value);
        }
        
        // Write pretty JSON for readability
        file << jsonConfig.dump(2);
        
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("JSON writing error in " + filepath + ": " + std::string(e.what()));
        return false;
    }
}

bool Config::saveToYaml(const std::string& filepath) const {
    // For now, fall back to simple format
    // TODO: Add YAML support with a YAML library
    return saveToSimpleFormat(filepath);
}

bool Config::loadFromSimpleFormat(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse key=value pairs (with support for various value formats)
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
            
            // Try to parse numeric values
            bool parsed = false;
            
            // Try boolean
            if (value == "true" || value == "false") {
                bool boolValue = (value == "true");
                set(key, boolValue, ConfigSource::File);
                parsed = true;
            }
            // Try integer
            else {
                try {
                    // Try int64_t first
                    size_t posEnd;
                    int64_t intValue = std::stoll(value, &posEnd);
                    if (posEnd == value.size()) {
                        set(key, intValue, ConfigSource::File);
                        parsed = true;
                    }
                    // Try double
                    else {
                        double doubleValue = std::stod(value, &posEnd);
                        if (posEnd == value.size()) {
                            set(key, doubleValue, ConfigSource::File);
                            parsed = true;
                        }
                    }
                } catch (const std::exception&) {
                    // Not a number, keep as string
                }
            }
            
            if (!parsed) {
                set(key, value, ConfigSource::File);
            }
        }
    }
    
    return true;
}

bool Config::saveToSimpleFormat(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        
        // Convert value back to string
        std::ostringstream valueStream;
        std::visit([&](auto&& value) {
            using T = std::decay_t<decltype(value)>;
            if constexpr (std::is_same_v<T, std::string>) {
                valueStream << "\"" << value << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                valueStream << (value ? "true" : "false");
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                valueStream << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i) valueStream << ", ";
                    valueStream << "\"" << value[i] << "\"";
                }
                valueStream << "]";
            } else if constexpr (std::is_same_v<T, std::vector<int64_t>>) {
                valueStream << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i) valueStream << ", ";
                    valueStream << value[i];
                }
                valueStream << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                valueStream << "[";
                for (size_t i = 0; i < value.size(); ++i) {
                    if (i) valueStream << ", ";
                    valueStream << value[i];
                }
                valueStream << "]";
            } else {
                valueStream << value;
            }
        }, entry.value);
        
        file << entry.key << " = " << valueStream.str() << "\n\n";
    }
    
    return true;
}

// Complete loadFromArgs implementation
bool Config::loadFromArgsComplete(int argc, char** argv) {
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

// Update set methods to support validation
void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Basic validation - check for empty key
    if (key.empty()) {
        NLM_LOG_WARN("Attempt to set configuration with empty key");
        return;
    }
    
    // Validate the value type
    if (!validateValueType(value)) {
        NLM_LOG_WARN("Invalid configuration value type for key: " + key);
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

// Add validation method
bool Config::validateValueType(const ConfigValue& value) const {
    return std::visit([](auto&& val) -> bool {
        using T = std::decay_t<decltype(val)>;
        return std::is_same_v<T, int> ||
               std::is_same_v<T, int64_t> ||
               std::is_same_v<T, uint64_t> ||
               std::is_same_v<T, double> ||
               std::is_same_v<T, bool> ||
               std::is_same_v<T, std::string> ||
               std::is_same_v<T, std::vector<int64_t>> ||
               std::is_same_v<T, std::vector<double>> ||
               std::is_same_v<T, std::vector<std::string>>;
    }, value);
}

// Add configuration validation
bool Config::validateConfig(const ConfigValidationRules& validationRules) const {
    for (const auto& validation : validationRules) {
        if (!validation.validate(*this)) {
            return false;
        }
    }
    return true;
}

// Add methods for config validation
struct ConfigValidationRules {
    std::string ruleName;
    bool (*validator)(const Config& config);
    std::string description;
};

void Config::addValidationRule(const std::string& name, 
    bool (*validator)(const Config& config), const std::string& description) {
    // TODO: Implement validation rule storage
    // For now, just log the warning
    NLM_LOG_WARN("Validation rule registered: " + name + " - " + description);
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
            } else if constexpr (std::is_same_v<T, std::vector<int>> ||
                                 std::is_same_v<T, std::vector<double>> ||
                                 std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i) oss << ", ";
                    oss << arg[i];
                }
                oss << "]";
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

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    if (it == pImpl->entries.end()) {
        return std::nullopt;
    }
    // Numeric values convert between int/int64/double/float/etc, so a
    // value stored as double can be read as float and vice versa.
    return std::visit([](auto&& v) -> std::optional<T> {
        using V = std::decay_t<decltype(v)>;
        if constexpr (std::is_same_v<V, T>) {
            return v;
        } else if constexpr (std::is_arithmetic_v<V> && std::is_arithmetic_v<T>) {
            return static_cast<T>(v);
        } else {
            return std::nullopt;
        }
    }, it->value);
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

// Explicit template instantiations
// (uint64_t and size_t are the same type on this platform, so only one is listed)
template std::optional<int> Config::get<int>(const std::string&) const;
template std::optional<int64_t> Config::get<int64_t>(const std::string&) const;
template std::optional<uint64_t> Config::get<uint64_t>(const std::string&) const;
template std::optional<float> Config::get<float>(const std::string&) const;
template std::optional<double> Config::get<double>(const std::string&) const;
template std::optional<bool> Config::get<bool>(const std::string&) const;
template std::optional<std::string> Config::get<std::string>(const std::string&) const;

template int Config::getOr<int>(const std::string&, const int&) const;
template int64_t Config::getOr<int64_t>(const std::string&, const int64_t&) const;
template uint64_t Config::getOr<uint64_t>(const std::string&, const uint64_t&) const;
template float Config::getOr<float>(const std::string&, const float&) const;
template double Config::getOr<double>(const std::string&, const double&) const;
template bool Config::getOr<bool>(const std::string&, const bool&) const;
template std::string Config::getOr<std::string>(const std::string&, const std::string&) const;

} // namespace nlm
