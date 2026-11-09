#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <regex>
#include <nlohmann/json.hpp>
#include <regex>

namespace nlm {

using json = nlohmann::json;

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    // Configuration schema for validation
    std::map<std::string, std::function<bool(const ConfigValue&)>> validators;
    // Default values for configuration keys
    std::map<std::string, ConfigValue> defaults;
    // Known configuration keys and their types
    std::map<std::string, std::string> keyTypes;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default known keys
    initializeDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaultSchema() {
    // Define known configuration keys with their types and validators
    pImpl->keyTypes = {
        {"random_seed", "int64_t"},
        {"simulation_timestep", "double"},
        {"neuron_count", "int64_t"},
        {"region_count", "int64_t"},
        {"connection_probability", "float"},
        {"plasticity_learning_rate", "float"},
        {"stdp_ltp_weight", "float"},
        {"stdp_ltd_weight", "float"},
        {"stdp_time_constant", "float"},
        {"development_synaptogenesis_rate", "float"},
        {"development_pruning_rate", "float"},
        {"dopamine_baseline", "float"},
        {"reward_discount_factor", "float"},
        {"environment_name", "string"},
        {"environment_width", "int64_t"},
        {"environment_height", "int64_t"},
        {"log_level", "string"},
        {"log_to_file", "bool"},
        {"log_filename", "string"},
        {"max_simulation_steps", "int64_t"},
        {"simulation_time_limit", "double"},
        {"visualization_enabled", "bool"},
        {"visualization_update_rate", "float"}
    };
    
    pImpl->defaults = {
        {"random_seed", int64_t(42)},
        {"simulation_timestep", 0.001},
        {"neuron_count", int64_t(1000)},
        {"region_count", int64_t(2)},
        {"connection_probability", 0.1f},
        {"plasticity_learning_rate", 0.01f},
        {"stdp_ltp_weight", 0.01f},
        {"stdp_ltd_weight", 0.012f},
        {"stdp_time_constant", 20.0f},
        {"development_synaptogenesis_rate", 0.001f},
        {"development_pruning_rate", 0.0001f},
        {"dopamine_baseline", 0.1f},
        {"reward_discount_factor", 0.99f},
        {"environment_name", "GridWorld"},
        {"environment_width", int64_t(10)},
        {"environment_height", int64_t(10)},
        {"log_level", "INFO"},
        {"log_to_file", false},
        {"log_filename", "nlm.log"},
        {"max_simulation_steps", int64_t(10000)},
        {"simulation_time_limit", 0.0},
        {"visualization_enabled", false},
        {"visualization_update_rate", 30.0f}
    };
    
    // Simple validators - more complex ones could be added
    pImpl->validators = {
        {"random_seed", [](const ConfigValue& v) {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int64_t>) {
                    return arg >= 0 && arg <= 2147483647;
                }
                return false;
            }, v);
        }},
        {"simulation_timestep", [](const ConfigValue& v) {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double>) {
                    return arg > 0.0 && arg <= 1.0;
                }
                return false;
            }, v);
        }},
        {"neuron_count", [](const ConfigValue& v) {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int64_t>) {
                    return arg > 0 && arg <= 100000;
                }
                return false;
            }, v);
        }},
        {"region_count", [](const ConfigValue& v) {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int64_t>) {
                    return arg > 0 && arg <= 100;
                }
                return false;
            }, v);
        }},
        {"connection_probability", [](const ConfigValue& v) {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                    return arg >= 0.0 && arg <= 1.0;
                }
                return false;
            }, v);
        }},
        {"log_level", [](const ConfigValue& v) {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, std::string>) {
                    static const std::vector<std::string> validLevels = {"DEBUG", "INFO", "WARNING", "ERROR", "FATAL"};
                    return std::find(validLevels.begin(), validLevels.end(), arg) != validLevels.end();
                }
                return false;
            }, v);
        }},
        {"log_to_file", [](const ConfigValue& v) {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, bool>) {
                    return true;
                }
                return false;
            }, v);
        }},
        {"max_simulation_steps", [](const ConfigValue& v) {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int64_t>) {
                    return arg > 0 && arg <= 1000000;
                }
                return false;
            }, v);
        }},
        {"visualization_enabled", [](const ConfigValue& v) {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, bool>) {
                    return true;
                }
                return false;
            }, v);
        }},
        {"visualization_update_rate", [](const ConfigValue& v) {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                    return arg > 0.0 && arg <= 1000.0;
                }
                return false;
            }, v);
        }},
        {"environment_width", [](const ConfigValue& v) {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int64_t>) {
                    return arg >= 1 && arg <= 500;
                }
                return false;
            }, v);
        }},
        {"environment_height", [](const ConfigValue& v) {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, int64_t>) {
                    return arg >= 1 && arg <= 500;
                }
                return false;
            }, v);
        }},
        {"reward_discount_factor", [](const ConfigValue& v) {
            return std::visit([](auto&& arg) -> bool {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, double> || std::is_same_v<T, float>) {
                    return arg >= 0.0 && arg <= 1.0;
                }
                return false;
            }, v);
        }}
    };
}

// Load from JSON or simple key=value file
    return loadFromFile(filepath, ConfigSource::File);
}

bool Config::loadFromFile(const std::string& filepath, ConfigSource source) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open config file: " + filepath);
        return false;
    }
    
    // Try to parse as JSON first
    try {
        json j;
        file >> j;
        
        // Clear existing entries before loading
        clear();
        
        // Process JSON entries with validation
        for (auto& [key, value] : j.items()) {
            bool valid = true;
            std::string errorMsg;
            
            // Validate key exists in schema
            if (pImpl->keyTypes.find(key) == pImpl->keyTypes.end()) {
                NLM_LOG_WARNING("Unknown configuration key: " + key);
                // Allow unknown keys for forward compatibility
            }
            
            // Validate value if key is known
            if (pImpl->validators.find(key) != pImpl->validators.end()) {
                valid = pImpl->validators[key](value);
                if (!valid) {
                    errorMsg = "Invalid value for key '" + key + "': ";
                    errorMsg += json(value).dump();
                    NLM_LOG_ERROR(errorMsg);
                }
            }
            
            if (valid) {
                setFromJson(key, value, source);
                if (!errorMsg.empty()) {
                    NLM_LOG_WARNING(errorMsg);
                }
            }
        }
        
        // Fill in missing values with defaults
        for (const auto& [key, defaultValue] : pImpl->defaults) {
            if (!has(key)) {
                set(key, defaultValue, ConfigSource::Default);
            }
        }
        
        NLM_LOG_INFO("Successfully loaded JSON config from: " + filepath);
        return true;
        
    } catch (const json::parse_error& e) {
        // Fall back to simple key=value format
        file.clear();
        file.seekg(0);
        
        NLM_LOG_INFO("JSON parse failed for " + filepath + ", falling back to simple format: " + std::string(e.what()));
        return loadFromSimpleFile(file, source);
    }
}

bool Config::loadFromSimpleFile(std::ifstream& file, ConfigSource source) {
    std::string line;
    size_t lineNumber = 0;
    
    while (std::getline(file, line)) {
        ++lineNumber;
        // Skip empty lines and comments
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == '/') {
            continue;
        }
        
        // Parse simple key=value pairs
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = trim(line.substr(0, pos));
            std::string valueStr = trim(line.substr(pos + 1));
            
            // Remove quotes if present
            if (valueStr.size() >= 2 && 
                ((valueStr.front() == '"' && valueStr.back() == '"') ||
                 (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                valueStr = valueStr.substr(1, valueStr.size() - 2);
            }
            
            // Validate key existence
            if (pImpl->keyTypes.find(key) == pImpl->keyTypes.end()) {
                NLM_LOG_WARNING("Unknown configuration key: " + key + " at line " + std::to_string(lineNumber));
            }
            
            // Convert string to appropriate type based on known schema
            ConfigValue value = convertStringToConfigValue(key, valueStr);
            
            // Validate the value
            bool valid = true;
            if (pImpl->validators.find(key) != pImpl->validators.end()) {
                valid = pImpl->validators[key](value);
            }
            
            if (valid) {
                set(key, value, source);
            } else {
                NLM_LOG_ERROR("Invalid configuration value for key '" + key + "': " + valueStr + " at line " + std::to_string(lineNumber));
            }
        } else {
            NLM_LOG_WARNING("Invalid line format in config file (no '='): " + line + " at line " + std::to_string(lineNumber));
        }
    }
    
    // Fill in missing values with defaults
    for (const auto& [key, defaultValue] : pImpl->defaults) {
        if (!has(key)) {
            set(key, defaultValue, ConfigSource::Default);
        }
    }
    
    return true;
}

ConfigValue Config::convertStringToConfigValue(const std::string& key, const std::string& valueStr) {
    std::string typeName = pImpl->keyTypes[key];
    
    try {
        if (typeName == "int64_t") {
            // Handle both decimal and hex formats
            if (valueStr.size() > 2 && valueStr[0] == '0' && (valueStr[1] == 'x' || valueStr[1] == 'X')) {
                return static_cast<int64_t>(std::stoll(valueStr, nullptr, 16));
            }
            return static_cast<int64_t>(std::stoll(valueStr));
        } else if (typeName == "double") {
            return std::stod(valueStr);
        } else if (typeName == "float") {
            return static_cast<float>(std::stod(valueStr));
        } else if (typeName == "bool") {
            // Handle various boolean representations
            std::string lower = toLower(valueStr);
            if (lower == "true" || lower == "yes" || lower == "1") return true;
            if (lower == "false" || lower == "no" || lower == "0") return false;
            throw std::invalid_argument("Invalid boolean value: " + valueStr);
        } else if (typeName == "string") {
            return valueStr;
        }
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to convert config value for key '" + key + "': " + std::string(e.what()));
        // Return empty value to signal error
    }
    
    return std::string(); // Return empty string as fallback
}

void Config::setFromJson(const std::string& key, const json& value, ConfigSource source) {
    if (value.is_string()) {
        set(key, value.get<std::string>(), source);
    } else if (value.is_number_integer()) {
        if (value.get<int64_t>() >= std::numeric_limits<int>::min() && 
            value.get<int64_t>() <= std::numeric_limits<int>::max()) {
            set(key, value.get<int>(), source);
        } else {
            set(key, value.get<int64_t>(), source);
        }
    } else if (value.is_number_unsigned()) {
        set(key, static_cast<int64_t>(value.get<uint64_t>()), source);
    } else if (value.is_number()) {
        set(key, value.get<double>(), source);
    } else if (value.is_boolean()) {
        set(key, value.get<bool>(), source);
    } else if (value.is_array()) {
        // Handle array types
        if (pImpl->keyTypes.find(key) != pImpl->keyTypes.end()) {
            std::string typeName = pImpl->keyTypes[key];
            if (typeName.find("int") != std::string::npos) {
                std::vector<int> intArray;
                for (const auto& item : value) {
                    intArray.push_back(item.get<int>());
                }
                set(key, intArray, source);
            } else if (typeName.find("double") != std::string::npos) {
                std::vector<double> doubleArray;
                for (const auto& item : value) {
                    doubleArray.push_back(item.get<double>());
                }
                set(key, doubleArray, source);
            } else if (typeName.find("string") != std::string::npos) {
                std::vector<std::string> stringArray;
                for (const auto& item : value) {
                    stringArray.push_back(item.get<std::string>());
                }
                set(key, stringArray, source);
            }
        }
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Initialize with defaults first
    clear();
    for (const auto& [key, defaultValue] : pImpl->defaults) {
        set(key, defaultValue, ConfigSource::Default);
    }
    
    bool success = true;
    // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Validate key
                if (pImpl->keyTypes.find(key) == pImpl->keyTypes.end()) {
                    NLM_LOG_ERROR("Unknown configuration key from command line: " + key);
                    success = false;
                    continue;
                }
                
                // Convert and set with validation
                ConfigValue configValue = convertStringToConfigValue(key, value);
                bool valid = true;
                if (pImpl->validators.find(key) != pImpl->validators.end()) {
                    valid = pImpl->validators[key](configValue);
                }
                
                if (valid) {
                    set(key, configValue, ConfigSource::CommandLine);
                } else {
                    NLM_LOG_ERROR("Invalid command line value for key '" + key + "': " + value);
                    success = false;
                }
            } else {
                NLM_LOG_ERROR("Invalid command line argument format (missing '='): " + arg);
                success = false;
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Validate key
            if (pImpl->keyTypes.find(key) == pImpl->keyTypes.end()) {
                NLM_LOG_ERROR("Unknown configuration key from command line: " + key);
                success = false;
                continue;
            }
            
            // Convert and set with validation
            ConfigValue configValue = convertStringToConfigValue(key, value);
            bool valid = true;
            if (pImpl->validators.find(key) != pImpl->validators.end()) {
                valid = pImpl->validators[key](configValue);
            }
            
            if (valid) {
                set(key, configValue, ConfigSource::CommandLine);
            } else {
                NLM_LOG_ERROR("Invalid command line value for key '" + key + "': " + value);
                success = false;
            }
        } else {
            NLM_LOG_ERROR("Invalid command line argument format: " + arg);
            success = false;
        }
    }
    
    return success;
}
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

void Config::set(const std::string& key, int64_t value, ConfigSource source) {
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

bool Config::validateKey(const std::string& key) const {
    return pImpl->keyTypes.find(key) != pImpl->keyTypes.end();
}

ConfigValue Config::getDefaultValue(const std::string& key) const {
    auto it = pImpl->defaults.find(key);
    if (it != pImpl->defaults.end()) {
        return it->second;
    }
    return std::string(); // Return empty string for unknown keys
}

void Config::set(const std::string& key, const std::vector<int>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<double>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
}

void Config::set(const std::string& key, const std::vector<std::string>& value, ConfigSource source) {
    set(key, ConfigValue(value), source);
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
