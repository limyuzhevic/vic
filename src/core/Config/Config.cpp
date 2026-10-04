#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <nlohmann/json.hpp>
#include <iostream>
#include <stdexcept>
#include <chrono>
#include <any>

namespace nlm {

class ValidationError : public std::runtime_error {
public:
    ValidationError(const std::string& message, const std::string& field = "", const std::string& value = "")
        : std::runtime_error("Configuration Error"), 
          message_(message), field_(field), value_(value) {}
    
    const std::string& getField() const { return field_; }
    const std::string& getValue() const { return value_; }
    const std::string& getMessage() const { return message_; }
    
private:
    std::string message_;
    std::string field_;
    std::string value_;
};

struct ConfigValidationSchema {
    std::string name;
    std::string type;
    std::any min;
    std::any max;
    std::string unit;
    std::string description;
    std::function<bool(const ConfigValue&)> customValidator;
};

// Standard configuration parameter validation schemas
static std::vector<ConfigValidationSchema> getValidationSchemas() {
    return {
        {"neuron_count", "int", 1, 1000000, "neurons", 
         "Number of neurons in the network. Range: 1-1,000,000", 
         [](const ConfigValue& v) { auto val = std::get<int>(v); return val >= 1 && val <= 1000000; }},
        
        {"region_count", "int", 1, 100, "regions",
         "Number of brain regions. Range: 1-100",
         [](const ConfigValue& v) { auto val = std::get<int>(v); return val >= 1 && val <= 100; }},
        
        {"connection_probability", "double", 0.0, 1.0, "probability",
         "Probability of connection between neurons. Range: 0.0-1.0",
         [](const ConfigValue& v) { auto val = std::get<double>(v); return val >= 0.0 && val <= 1.0; }},
        
        {"simulation_timestep", "double", 0.0001, 1.0, "seconds",
         "Simulation timestep. Range: 0.0001-1.0 seconds",
         [](const ConfigValue& v) { auto val = std::get<double>(v); return val >= 0.0001 && val <= 1.0; }},
        
        {"stdp_ltp_weight", "double", -10.0, 10.0, "weight",
         "STDP LTP weight. Range: -10.0-10.0",
         [](const ConfigValue& v) { auto val = std::get<double>(v); return val >= -10.0 && val <= 10.0; }},
        
        {"stdp_ltd_weight", "double", -10.0, 10.0, "weight",
         "STDP LTD weight. Range: -10.0-10.0",
         [](const ConfigValue& v) { auto val = std::get<double>(v); return val >= -10.0 && val <= 10.0; }},
        
        {"synaptogenesis_rate", "double", 0.0, 0.1, "per_step",
         "Synaptogenesis rate. Range: 0.0-0.1 per step",
         [](const ConfigValue& v) { auto val = std::get<double>(v); return val >= 0.0 && val <= 0.1; }},
        
        {"pruning_rate", "double", 0.0, 0.01, "per_step",
         "Pruning rate. Range: 0.0-0.01 per step",
         [](const ConfigValue& v) { auto val = std::get<double>(v); return val >= 0.0 && val <= 0.01; }},
        
        {"working_memory_capacity", "int", 10, 100000, "neurons",
         "Working memory capacity. Range: 10-100,000 neurons",
         [](const ConfigValue& v) { auto val = std::get<int>(v); return val >= 10 && val <= 100000; }},
        
        {"episodic_memory_max_episodes", "int", 1, 10000, "episodes",
         "Episodic memory max episodes. Range: 1-10,000",
         [](const ConfigValue& v) { auto val = std::get<int>(v); return val >= 1 && val <= 10000; }},
        
        {"checkpoint_interval", "int", 100, 1000000, "steps",
         "Checkpoint interval. Range: 100-1,000,000 steps",
         [](const ConfigValue& v) { auto val = std::get<int>(v); return val >= 100 && val <= 1000000; }},
        
        {"max_checkpoints", "int", 1, 100, "checkpoints",
         "Maximum checkpoints to store. Range: 1-100",
         [](const ConfigValue& v) { auto val = std::get<int>(v); return val >= 1 && val <= 100; }},
        
        {"development_threshold", "double", 0.0, 1.0, "factor",
         "Development threshold. Range: 0.0-1.0",
         [](const ConfigValue& v) { auto val = std::get<double>(v); return val >= 0.0 && val <= 1.0; }},
        
        {"dopamine_level", "double", 0.0, 1.0, "level",
         "Dopamine level. Range: 0.0-1.0",
         [](const ConfigValue& v) { auto val = std::get<double>(v); return val >= 0.0 && val <= 1.0; }},
        
        {"curiosity_level", "double", 0.0, 1.0, "level",
         "Curiosity level. Range: 0.0-1.0",
         [](const ConfigValue& v) { auto val = std::get<double>(v); return val >= 0.0 && val <= 1.0; }},
        
        {"novelty_threshold", "double", 0.0, 1.0, "threshold",
         "Novelty threshold. Range: 0.0-1.0",
         [](const ConfigValue& v) { auto val = std::get<double>(v); return val >= 0.0 && val <= 1.0; }},
        
        {"prediction_horizon", "int", 1, 100, "steps",
         "Prediction horizon. Range: 1-100 steps",
         [](const ConfigValue& v) { auto val = std::get<int>(v); return val >= 1 && val <= 100; }},
        
        {"prediction_confidence_threshold", "double", 0.0, 1.0, "confidence",
         "Prediction confidence threshold. Range: 0.0-1.0",
         [](const ConfigValue& v) { auto val = std::get<double>(v); return val >= 0.0 && val <= 1.0; }},
    };
}

// Validation utility functions
namespace {
    const std::vector<std::string> VALID_ENV_PREFIXES = {"NLM_", "BRAIN_"};
    
    bool validateFileFormat(const std::string& filepath) {
        if (!std::filesystem::exists(filepath)) return false;
        if (!std::filesystem::is_regular_file(filepath)) return false;
        
        try {
            std::ifstream file(filepath);
            if (!file.is_open()) return false;
            
            nlohmann::json jsonConfig;
            file >> jsonConfig;
            return true;
        } catch (const std::exception&) {
            return false;
        }
    }
    
    bool validateEnvVarName(const std::string& name) {
        for (const auto& prefix : VALID_ENV_PREFIXES) {
            if (name.find(prefix) == 0) {
                return true;
            }
        }
        return false;
    }
    
    template<typename T>
    bool convertAndValidate(const std::string& key, const std::string& valueStr, ConfigValue& target) {
        try {
            const auto& schemas = getValidationSchemas();
            const auto& schemaIt = std::find_if(schemas.begin(), schemas.end(),
                [&key](const ConfigValidationSchema& schema) { return schema.name == key; });
            
            if (schemaIt != schemas.end()) {
                const auto& schema = *schemaIt;
                
                if constexpr (std::is_same_v<T, int>) {
                    size_t pos;
                    int intVal = std::stoi(valueStr, &pos);
                    if (pos != valueStr.size()) return false;
                    
                    if (schema.min.has_value() && std::any_cast<int>(schema.min) > intVal) return false;
                    if (schema.max.has_value() && std::any_cast<int>(schema.max) < intVal) return false;
                    
                    target = intVal;
                    return true;
                }
                else if constexpr (std::is_same_v<T, double>) {
                    size_t pos;
                    double doubleVal = std::stod(valueStr, &pos);
                    if (pos != valueStr.size()) return false;
                    
                    if (schema.min.has_value() && std::any_cast<double>(schema.min) > doubleVal) return false;
                    if (schema.max.has_value() && std::any_cast<double>(schema.max) < doubleVal) return false;
                    
                    target = doubleVal;
                    return true;
                }
                else if constexpr (std::is_same_v<T, bool>) {
                    if (valueStr == "true" || valueStr == "false") {
                        bool boolVal = (valueStr == "true");
                        target = boolVal;
                        return true;
                    }
                    return false;
                }
                else if constexpr (std::is_same_v<T, std::string>) {
                    target = valueStr;
                    return true;
                }
                else if constexpr (std::is_same_v<T, int64_t>) {
                    size_t pos;
                    int64_t intVal = std::stoll(valueStr, &pos);
                    if (pos != valueStr.size()) return false;
                    
                    if (schema.min.has_value()) {
                        int64_t minVal = std::any_cast<int64_t>(schema.min);
                        if (minVal > intVal) return false;
                    }
                    if (schema.max.has_value()) {
                        int64_t maxVal = std::any_cast<int64_t>(schema.max);
                        if (maxVal < intVal) return false;
                    }
                    
                    target = intVal;
                    return true;
                }
            }
        } catch (const std::exception&) {}
        return false;
    }
}

static ConfigValue jsonToConfigValue(const nlohmann::json& j) {
    if (j.is_string()) {
        return j.get<std::string>();
    } else if (j.is_number_integer()) {
        return j.get<int>();
    } else if (j.is_number_unsigned()) {
        return static_cast<int>(j.get<std::uint64_t>());
    } else if (j.is_number()) {
        if (j.is_number_integer()) {
            return j.get<int>();
        } else {
            double val = j.get<double>();
            if (std::floor(val) == val) {
                return static_cast<int>(val);
            }
            return val;
        }
    } else if (j.is_boolean()) {
        return j.get<bool>();
    } else if (j.is_array()) {
        if (j.empty()) {
            return std::vector<int>();
        }
        
        bool isIntArray = std::all_of(j.begin(), j.end(), [](const nlohmann::json& item) {
            return item.is_number_integer();
        });
        if (isIntArray) {
            return j.get<std::vector<int>>();
        }
        
        bool isDoubleArray = std::all_of(j.begin(), j.end(), [](const nlohmann::json& item) {
            return item.is_number();
        });
        if (isDoubleArray) {
            return j.get<std::vector<double>>();
        }
        
        bool isStringArray = std::all_of(j.begin(), j.end(), [](const nlohmann::json& item) {
            return item.is_string();
        });
        if (isStringArray) {
            return j.get<std::vector<std::string>>();
        }
        
        return j.get<std::vector<double>>();
    }
    
    return j.get<std::string>();
}

static nlohmann::json configValueToJson(const ConfigValue& value) {
    return std::visit([](auto&& arg) -> nlohmann::json {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int>) {
            return arg;
        } else if constexpr (std::is_same_v<T, int64_t>) {
            return arg;
        } else if constexpr (std::is_same_v<T, double>) {
            return arg;
        } else if constexpr (std::is_same_v<T, bool>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            return arg;
        } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
            return arg;
        } else {
            return std::string("unknown_type");
        }
    }, value);
}

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw ValidationError("Configuration file not found or cannot be opened", "filepath", filepath);
        }
        
        // Validate file format first
        if (!validateFileFormat(filepath)) {
            throw ValidationError("Invalid configuration file format - must be valid JSON", "filepath", filepath);
        }
        
        nlohmann::json jsonConfig;
        file >> jsonConfig;
        
        if (!jsonConfig.is_object()) {
            throw ValidationError("Configuration file must contain a JSON object", "format", "");
        }
        
        clear();
        
        // Process and validate entries
        for (const auto& [key, value] : jsonConfig.items()) {
            ConfigValue configValue = jsonToConfigValue(value);
            
            // Validate against schema
            const auto& schemas = getValidationSchemas();
            const auto& schemaIt = std::find_if(schemas.begin(), schemas.end(),
                [&key](const ConfigValidationSchema& schema) { return schema.name == key; });
            
            if (schemaIt != schemas.end()) {
                const auto& schema = *schemaIt;
                if (schema.customValidator && !schema.customValidator(configValue)) {
                    throw ValidationError("Configuration value out of valid range", key, value.dump());
                }
            }
            
            set(key, configValue, ConfigSource::File);
        }
        
        return true;
    } catch (const ValidationError& e) {
        std::cerr << "[CONFIG ERROR] " << e.what() << " (Field: " << e.getField() << ")" << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "[CONFIG ERROR] Failed to load configuration file: " << e.what() << std::endl;
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    bool success = true;
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                // Convert to appropriate type with validation
                bool converted = false;
                
                // Try boolean
                if (value == "true" || value == "false") {
                    set(key, value == "true", ConfigSource::CommandLine);
                    converted = true;
                }
                // Try integer
                else {
                    // Check if it's a valid integer
                    bool isValidInt = true;
                    size_t posCheck;
                    try {
                        std::stoi(value, &posCheck);
                        if (posCheck != value.size()) isValidInt = false;
                    } catch (...) {
                        isValidInt = false;
                    }
                    
                    if (isValidInt) {
                        set(key, std::stoi(value), ConfigSource::CommandLine);
                        converted = true;
                    } else {
                        // Check if it's a valid float/double
                        bool isValidFloat = true;
                        try {
                            std::stod(value, &posCheck);
                            if (posCheck != value.size()) isValidFloat = false;
                        } catch (...) {
                            isValidFloat = false;
                        }
                        
                        if (isValidFloat) {
                            set(key, std::stod(value), ConfigSource::CommandLine);
                            converted = true;
                        }
                    }
                }
                
                if (!converted) {
                    set(key, value, ConfigSource::CommandLine);
                }
            }
        }
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            // Similar conversion logic for single-character args
            bool converted = false;
            
            if (value == "true" || value == "false") {
                set(key, value == "true", ConfigSource::CommandLine);
                converted = true;
            } else {
                size_t pos;
                try {
                    int intVal = std::stoi(value, &pos);
                    if (pos == value.size()) {
                        set(key, intVal, ConfigSource::CommandLine);
                        converted = true;
                    } else {
                        double doubleVal = std::stod(value, &pos);
                        if (pos == value.size()) {
                            set(key, doubleVal, ConfigSource::CommandLine);
                            converted = true;
                        }
                    }
                } catch (const std::exception&) {}
            }
            
            if (!converted) {
                set(key, value, ConfigSource::CommandLine);
            }
        }
    }
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw ValidationError("Cannot create configuration file", "filepath", filepath);
        }
        
        nlohmann::json jsonConfig;
        
        for (const auto& entry : pImpl->entries) {
            jsonConfig[entry.key] = configValueToJson(entry.value);
        }
        
        file << jsonConfig.dump(2);
        file.flush();
        
        if (!file.good()) {
            throw ValidationError("Failed to write configuration data", "file", filepath);
        }
        
        return true;
    } catch (const ValidationError& e) {
        std::cerr << "[CONFIG ERROR] " << e.what() << " (Field: " << e.getField() << ")" << std::endl;
        return false;
    } catch (const std::exception& e) {
        std::cerr << "[CONFIG ERROR] Failed to save configuration file: " << e.what() << std::endl;
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
    // Validate key name
    if (key.empty()) {
        throw ValidationError("Configuration key cannot be empty", "key", "");
    }
    
    if (key.find(' ') != std::string::npos) {
        throw ValidationError("Configuration key cannot contain spaces", "key", key);
    }
    
    // Validate value based on type
    const auto& schemas = getValidationSchemas();
    const auto& schemaIt = std::find_if(schemas.begin(), schemas.end(),
        [&key](const ConfigValidationSchema& schema) { return schema.name == key; });
    
    if (schemaIt != schemas.end()) {
        const auto& schema = *schemaIt;
        if (schema.customValidator && !schema.customValidator(value)) {
            throw ValidationError("Configuration value validation failed", key, std::to_string(std::visit([](auto&& v) { 
                if constexpr (std::is_same_v<decltype(v), int>) return v;
                else if constexpr (std::is_same_v<decltype(v), double>) return v;
                else if constexpr (std::is_same_v<decltype(v), bool>) return v;
                else if constexpr (std::is_same_v<decltype(v), std::string>) return 0;
                else return 0;
            }, value)));
        }
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

bool Config::validateEntry(const std::string& key, const ConfigValue& value) {
    auto it = pImpl->validators.find(key);
    if (it != pImpl->validators.end()) {
        return it->second(value);
    }
    return true;
}

void Config::registerValidator(const std::string& key, 
                               std::function<bool(const ConfigValue&)> validator) {
    pImpl->validators[key] = validator;
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
