#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <regex>
#include <iostream>
#include <unordered_map>

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, ConfigTypeInfo> schema;
    std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    // Initialize with default configuration schema
    initializeDefaultSchema();
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

void Config::initializeDefaultSchema() {
    // Register common configuration keys with validation
    registerKey("random_seed", "Random seed for reproducible results", false,
                std::make_optional<ConfigValue>(42LL),
                [](const ConfigValue& v) {
                    if (std::holds_alternative<int64_t>(v)) {
                        int64_t seed = std::get<int64_t>(v);
                        if (seed < 0 || seed > 2147483647) {
                            return ValidationResult::error("Random seed must be between 0 and 2147483647");
                        }
                    }
                    return ValidationResult::success();
                });
    
    registerKey("simulation_timestep", "Simulation timestep in seconds", true,
                std::make_optional<ConfigValue>(0.001),
                [](const ConfigValue& v) {
                    if (std::holds_alternative<double>(v)) {
                        double dt = std::get<double>(v);
                        if (dt <= 0.0 || dt > 1.0) {
                            return ValidationResult::error("Timestep must be between 0.0001 and 1.0");
                        }
                    }
                    return ValidationResult::success();
                });
    
    registerKey("neuron_count", "Total number of neurons in the network", false,
                std::make_optional<ConfigValue>(1000LL),
                [](const ConfigValue& v) {
                    if (std::holds_alternative<int64_t>(v)) {
                        int64_t count = std::get<int64_t>(v);
                        if (count <= 0 || count > 1000000) {
                            return ValidationResult::error("Neuron count must be between 1 and 1000000");
                        }
                    }
                    return ValidationResult::success();
                });
    
    registerKey("region_count", "Number of neural regions", false,
                std::make_optional<ConfigValue>(1LL),
                [](const ConfigValue& v) {
                    if (std::holds_alternative<int64_t>(v)) {
                        int64_t count = std::get<int64_t>(v);
                        if (count <= 0 || count > 100) {
                            return ValidationResult::error("Region count must be between 1 and 100");
                        }
                    }
                    return ValidationResult::success();
                });
    
    registerKey("connection_probability", "Probability of connection between neurons (0-1)", false,
                std::make_optional<ConfigValue>(0.1),
                [](const ConfigValue& v) {
                    if (std::holds_alternative<double>(v)) {
                        double prob = std::get<double>(v);
                        if (prob < 0.0 || prob > 1.0) {
                            return ValidationResult::error("Connection probability must be between 0.0 and 1.0");
                        }
                    }
                    return ValidationResult::success();
                });
    
    registerKey("stdp_ltp_weight", "STDP LTP learning rate", false,
                std::make_optional<ConfigValue>(0.02),
                [](const ConfigValue& v) {
                    if (std::holds_alternative<double>(v)) {
                        double weight = std::get<double>(v);
                        if (weight < 0.0 || weight > 1.0) {
                            return ValidationResult::error("STDP LTP weight must be between 0.0 and 1.0");
                        }
                    }
                    return ValidationResult::success();
                });
    
    registerKey("stdp_ltd_weight", "STDP LTD learning rate", false,
                std::make_optional<ConfigValue>(0.015),
                [](const ConfigValue& v) {
                    if (std::holds_alternative<double>(v)) {
                        double weight = std::get<double>(v);
                        if (weight < 0.0 || weight > 1.0) {
                            return ValidationResult::error("STDP LTD weight must be between 0.0 and 1.0");
                        }
                    }
                    return ValidationResult::success();
                });
    
    registerKey("stdp_tau", "STDP time constant in milliseconds", false,
                std::make_optional<ConfigValue>(20.0),
                [](const ConfigValue& v) {
                    if (std::holds_alternative<double>(v)) {
                        double tau = std::get<double>(v);
                        if (tau <= 0.0 || tau > 1000.0) {
                            return ValidationResult::error("STDP tau must be between 0.1 and 1000.0");
                        }
                    }
                    return ValidationResult::success();
                });
    
    registerKey("synaptogenesis_rate", "Rate of structural synapse formation", false,
                std::make_optional<ConfigValue>(0.0001),
                [](const ConfigValue& v) {
                    if (std::holds_alternative<double>(v)) {
                        double rate = std::get<double>(v);
                        if (rate < 0.0 || rate > 1.0) {
                            return ValidationResult::error("Synaptogenesis rate must be between 0.0 and 1.0");
                        }
                    }
                    return ValidationResult::success();
                });
    
    registerKey("pruning_rate", "Rate of structural synapse elimination", false,
                std::make_optional<ConfigValue>(0.00001),
                [](const ConfigValue& v) {
                    if (std::holds_alternative<double>(v)) {
                        double rate = std::get<double>(v);
                        if (rate < 0.0 || rate > 1.0) {
                            return ValidationResult::error("Pruning rate must be between 0.0 and 1.0");
                        }
                    }
                    return ValidationResult::success();
                });
}

bool Config::loadFromFile(const std::string& filepath) {
    // TODO PHASE 2: Implement proper JSON/YAML parser
    // PLACEHOLDER - Phase 1 uses a simple key=value format
    
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
            
            // Validate key against schema before setting
            if (pImpl->schema.find(key) != pImpl->schema.end()) {
                // Try to convert and validate
                bool validValue = false;
                ConfigValue configValue;
                
                // Try to convert to appropriate type based on schema
                if (auto schemaVal = pImpl->schema[key].defaultValue) {
                    if (std::holds_alternative<int64_t>(*schemaVal)) {
                        try {
                            int64_t intVal = stringToInt64(value);
                            configValue = intVal;
                            validValue = true;
                        } catch (...) {}
                    } else if (std::holds_alternative<double>(*schemaVal)) {
                        try {
                            double doubleVal = stringToDouble(value);
                            configValue = doubleVal;
                            validValue = true;
                        } catch (...) {}
                    } else if (std::holds_alternative<bool>(*schemaVal)) {
                        try {
                            bool boolVal = stringToBool(value);
                            configValue = boolVal;
                            validValue = true;
                        } catch (...) {}
                    } else if (std::holds_alternative<std::string>(*schemaVal)) {
                        configValue = value;
                        validValue = true;
                    }
                }
                
                if (validValue && pImpl->schema[key].validator) {
                    auto validation = pImpl->schema[key].validator(configValue);
                    if (validation.valid) {
                        set(key, configValue, ConfigSource::File);
                    } else {
                        pImpl->validationErrors.push_back("Invalid value for key '" + key + "': " + validation.errorMessage);
                    }
                } else {
                    // Fallback to simple parsing
                    set(key, value, ConfigSource::File);
                }
            } else {
                // No schema validation, use fallback
                set(key, value, ConfigSource::File);
            }
        }
    }
    
    // Validate all entries after loading
    validateAll();
    
    return true;
}

bool Config::loadFromArgs(int argc, char** argv) {
    // Clear any existing validation errors
    pImpl->validationErrors.clear();
    
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
    
    // Validate all entries after loading
    validateAll();
    
    return true;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    for (const auto& entry : pImpl->entries) {
        file << "# " << entry.description << "\n";
        file << entry.key << " = ";
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << arg[i];
                }
                file << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                file << "[";
                for (size_t i = 0; i < arg.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << "\"" << arg[i] << "\"";
                }
                file << "]";
            } else {
                file << arg;
            }
        }, entry.value);
        file << "\n\n";
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
        // Type mismatch - return nullopt
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    return val.has_value() ? val.value() : defaultValue;
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate against schema if present
    if (pImpl->schema.find(key) != pImpl->schema.end()) {
        const auto& schema = pImpl->schema[key];
        
        // Validate type
        if (!value.index() == typeid(std::decay_t<decltype(schema.defaultValue.value())>).hash_code()) {
            pImpl->validationErrors.push_back("Type mismatch for key '" + key + "': expected " + 
                (schema.defaultValue ? typeName(*schema.defaultValue) : "unknown") + ", got " + typeName(value));
            return;
        }
        
        // Validate against constraints
        if (schema.validator) {
            auto validation = schema.validator(value);
            if (!validation.valid) {
                pImpl->validationErrors.push_back("Validation failed for key '" + key + "': " + validation.errorMessage);
                return;
            }
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

void Config::registerKey(const std::string& key, const std::string& description, bool required,
                         const std::optional<ConfigValue>& defaultValue,
                         const ConfigValidator& validator) {
    pImpl->schema[key] = ConfigTypeInfo(key, description, required, defaultValue, validator);
}

void Config::validateAll() const {
    pImpl->validationErrors.clear();
    
    for (const auto& entry : pImpl->entries) {
        // Check if key is registered in schema
        if (pImpl->schema.find(entry.key) != pImpl->schema.end()) {
            const auto& schema = pImpl->schema.at(entry.key);
            
            // Check if value type matches schema
            if (!schema.defaultValue.has_value()) {
                continue; // No type constraint
            }
            
            if (!typeMatches(entry.value, schema.defaultValue.value())) {
                pImpl->validationErrors.push_back("Type mismatch for key '" + entry.key + "': " +
                    "expected " + typeName(schema.defaultValue.value()) + ", " +
                    "got " + typeName(entry.value));
                continue;
            }
            
            // Validate against schema validator
            if (schema.validator) {
                auto validation = schema.validator(entry.value);
                if (!validation.valid) {
                    pImpl->validationErrors.push_back("Validation failed for key '" + entry.key + "': " +
                        validation.errorMessage);
                }
            }
        }
        
        // Check if required key is present
        if (pImpl->schema.find(entry.key) != pImpl->schema.end() &&
            pImpl->schema.at(entry.key).required &&
            !has(entry.key)) {
            pImpl->validationErrors.push_back("Required key '" + entry.key + "' is missing");
        }
    }
}

bool Config::isValid(const std::string& key) const {
    validateAll();
    return std::find(pImpl->validationErrors.begin(), pImpl->validationErrors.end(),
                    "Validation failed for key '" + key + "'" ) == pImpl->validationErrors.end();
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
    validateAll();
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
    pImpl->validationErrors.clear();
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration (" << pImpl->entries.size() << " entries, " << pImpl->validationErrors.size() << " errors):\n";
    
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
    
    if (!pImpl->validationErrors.empty()) {
        oss << "\nValidation Errors:\n";
        for (const auto& error : pImpl->validationErrors) {
            oss << "  - " << error << "\n";
        }
    }
    
    return oss.str();
}

std::vector<std::string> Config::getValidationErrors() const {
    validateAll();
    return pImpl->validationErrors;
}

// Implementation of helper functions

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

bool Config::stringToBool(const std::string& str) {
    std::string lower = toLower(trim(str));
    if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
        return false;
    }
    throw std::invalid_argument("Invalid boolean string: " + str);
}

double Config::stringToDouble(const std::string& str) {
    double result;
    std::istringstream iss(trim(str));
    iss >> result;
    if (iss.fail() || iss.peek() != EOF) {
        throw std::invalid_argument("Invalid double string: " + str);
    }
    return result;
}

int Config::stringToInt(const std::string& str) {
    int result;
    std::istringstream iss(trim(str));
    iss >> result;
    if (iss.fail() || iss.peek() != EOF) {
        throw std::invalid_argument("Invalid integer string: " + str);
    }
    return result;
}

int64_t Config::stringToInt64(const std::string& str) {
    int64_t result;
    std::istringstream iss(trim(str));
    iss >> result;
    if (iss.fail() || iss.peek() != EOF) {
        throw std::invalid_argument("Invalid integer string: " + str);
    }
    return result;
}

// Helper function to get type name
std::string Config::typeName(const ConfigValue& value) {
    if (std::holds_alternative<int>(value)) return "int";
    if (std::holds_alternative<int64_t>(value)) return "int64_t";
    if (std::holds_alternative<double>(value)) return "double";
    if (std::holds_alternative<bool>(value)) return "bool";
    if (std::holds_alternative<std::string>(value)) return "string";
    if (std::holds_alternative<std::vector<int>>(value)) return "vector<int>";
    if (std::holds_alternative<std::vector<double>>(value)) return "vector<double>";
    if (std::holds_alternative<std::vector<std::string>>(value)) return "vector<string>";
    return "unknown";
}

// Helper function to check type matching
bool Config::typeMatches(const ConfigValue& actual, const ConfigValue& expected) {
    return std::holds_alternative<std::decay_t<decltype(expected)>>(actual);
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

