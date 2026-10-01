#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include "../core/Logger/Logger.hpp"
#include "../core/ErrorHandling/ErrorHandling.hpp"

namespace nlm {

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::unordered_map<std::string, std::vector<ValidationFunction>> validators;
    mutable std::vector<std::string> validationErrors;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    try {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            NLM_THROW_CONFIGURATION_ERROR("Failed to open configuration file: " + filepath);
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
                
                set(key, value, ConfigSource::File);
            }
        }
        
        // Validate all loaded configuration
        validateAll();
        
        return true;
    } catch (const ConfigurationError& e) {
        NLM_LOG_ERROR("Configuration error loading file " + filepath + ": " + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error loading configuration file " + filepath + ": " + e.what());
        return false;
    }
}

bool Config::loadFromArgs(int argc, char** argv) {
    try {
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
        
        // Validate all loaded configuration
        validateAll();
        
        return true;
    } catch (const ConfigurationError& e) {
        NLM_LOG_ERROR("Configuration error loading from arguments: " + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error loading configuration from arguments: " + e.what());
        return false;
    }
}

bool Config::saveToFile(const std::string& filepath) const {
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_THROW_CONFIGURATION_ERROR("Failed to open configuration file for writing: " + filepath);
        }
        
        for (const auto& entry : pImpl->entries) {
            file << "# " << entry.description << "\n";
            file << entry.key << " = " << "PLACEHOLDER_VALUE\n";
        }
        
        return true;
    } catch (const ConfigurationError& e) {
        NLM_LOG_ERROR("Configuration error saving file " + filepath + ": " + e.what());
        return false;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Unexpected error saving configuration file " + filepath + ": " + e.what());
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
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        it->value = value;
        it->source = source;
    } else {
        ConfigEntry entry;
        entry.key = key;
        entry.value = value;
        entry.source = source;
        pImpl->entries.push_back(std::move(entry));
    }
    
    // Validate the new value
    validateAll();
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

bool Config::addValidation(const std::string& key, const ValidationFunction& validator) {
    if (!has(key)) {
        NLM_THROW_CONFIGURATION_ERROR("Cannot add validation for non-existent key: " + key);
    }
    
    pImpl->validators[key].push_back(validator);
    return validateAll();
}

bool Config::removeValidation(const std::string& key) {
    auto it = pImpl->validators.find(key);
    if (it != pImpl->validators.end()) {
        pImpl->validators.erase(it);
    }
    return validateAll();
}

bool Config::validateAll() const {
    pImpl->validationErrors.clear();
    bool allValid = true;
    
    for (const auto& entry : pImpl->entries) {
        auto validatorsIt = pImpl->validators.find(entry.key);
        if (validatorsIt != pImpl->validators.end()) {
            for (const auto& validator : validatorsIt->second) {
                if (!validator(entry.value)) {
                    allValid = false;
                    pImpl->validationErrors.push_back(
                        "Validation failed for key '" + entry.key + "': " + 
                        (entry.description.empty() ? "invalid value" : entry.description)
                    );
                }
            }
        }
    }
    
    if (!allValid) {
        NLM_LOG_WARNING("Configuration validation failed with " + 
                       std::to_string(pImpl->validationErrors.size()) + " errors");
        for (const auto& error : pImpl->validationErrors) {
            NLM_LOG_WARNING("  " + error);
        }
    }
    
    return allValid;
}

std::vector<std::string> Config::getValidationErrors() const {
    validateAll(); // Refresh validation errors
    return pImpl->validationErrors;
}

bool Config::addRangeValidation(const std::string& key, double min, double max, const std::string& description) {
    if (!has(key)) {
        NLM_THROW_CONFIGURATION_ERROR("Cannot add range validation for non-existent key: " + key);
    }
    
    bool isDouble = std::holds_alternative<double>(getValue(key));
    bool isInt = std::holds_alternative<int>(getValue(key)) || std::holds_alternative<int64_t>(getValue(key));
    
    if (!isDouble && !isInt) {
        NLM_THROW_CONFIGURATION_ERROR("Range validation requires numeric key: " + key);
    }
    
    ValidationFunction validator;
    if (isDouble) {
        validator = [min, max](const ConfigValue& val) {
            return std::get<double>(val) >= min && std::get<double>(val) <= max;
        };
    } else {
        validator = [min, max](const ConfigValue& val) {
            double val = (std::holds_alternative<int>(val) ? std::get<int>(val) : std::get<int64_t>(val));
            return val >= min && val <= max;
        };
    }
    
    return addValidation(key, validator);
}

bool Config::addIntegerRangeValidation(const std::string& key, int64_t min, int64_t max, const std::string& description) {
    if (!has(key)) {
        NLM_THROW_CONFIGURATION_ERROR("Cannot add integer range validation for non-existent key: " + key);
    }
    
    bool isInt = std::holds_alternative<int>(getValue(key)) || std::holds_alternative<int64_t>(getValue(key));
    
    if (!isInt) {
        NLM_THROW_CONFIGURATION_ERROR("Integer range validation requires integer key: " + key);
    }
    
    ValidationFunction validator = [min, max](const ConfigValue& val) {
        int64_t val = (std::holds_alternative<int>(val) ? std::get<int>(val) : std::get<int64_t>(val));
        return val >= min && val <= max;
    };
    
    return addValidation(key, validator);
}

bool Config::addPositiveValidation(const std::string& key, const std::string& description) {
    if (!has(key)) {
        NLM_THROW_CONFIGURATION_ERROR("Cannot add positive validation for non-existent key: " + key);
    }
    
    bool isDouble = std::holds_alternative<double>(getValue(key));
    bool isInt = std::holds_alternative<int>(getValue(key)) || std::holds_alternative<int64_t>(getValue(key));
    
    if (!isDouble && !isInt) {
        NLM_THROW_CONFIGURATION_ERROR("Positive validation requires numeric key: " + key);
    }
    
    ValidationFunction validator;
    if (isDouble) {
        validator = [](const ConfigValue& val) {
            return std::get<double>(val) > 0.0;
        };
    } else {
        validator = [](const ConfigValue& val) {
            int64_t val = (std::holds_alternative<int>(val) ? std::get<int>(val) : std::get<int64_t>(val));
            return val > 0;
        };
    }
    
    return addValidation(key, validator);
}

bool Config::addNonNegativeValidation(const std::string& key, const std::string& description) {
    if (!has(key)) {
        NLM_THROW_CONFIGURATION_ERROR("Cannot add non-negative validation for non-existent key: " + key);
    }
    
    bool isDouble = std::holds_alternative<double>(getValue(key));
    bool isInt = std::holds_alternative<int>(getValue(key)) || std::holds_alternative<int64_t>(getValue(key));
    
    if (!isDouble && !isInt) {
        NLM_THROW_CONFIGURATION_ERROR("Non-negative validation requires numeric key: " + key);
    }
    
    ValidationFunction validator;
    if (isDouble) {
        validator = [](const ConfigValue& val) {
            return std::get<double>(val) >= 0.0;
        };
    } else {
        validator = [](const ConfigValue& val) {
            int64_t val = (std::holds_alternative<int>(val) ? std::get<int>(val) : std::get<int64_t>(val));
            return val >= 0;
        };
    }
    
    return addValidation(key, validator);
}

bool Config::addNeuronCountValidation(size_t maxNeurons) {
    ValidationFunction validator = [maxNeurons](const ConfigValue& val) {
        if (const auto* intPtr = std::get_if<int>(&val)) {
            return *intPtr >= 0 && static_cast<size_t>(*intPtr) <= maxNeurons;
        } else if (const auto* int64Ptr = std::get_if<int64_t>(&val)) {
            return *int64Ptr >= 0 && static_cast<size_t>(*int64Ptr) <= maxNeurons;
        } else if (const auto* doublePtr = std::get_if<double>(&val)) {
            return *doublePtr >= 0.0 && static_cast<size_t>(*doublePtr) <= maxNeurons;
        }
        return false;
    };
    
    return addValidation("neuron_count", validator);
}

bool Config::addConnectionProbabilityValidation(double min, double max) {
    ValidationFunction validator = [min, max](const ConfigValue& val) {
        if (const auto* doublePtr = std::get_if<double>(&val)) {
            return *doublePtr >= min && *doublePtr <= max;
        }
        return false;
    };
    
    return addValidation("connection_probability", validator);
}

bool Config::addMemoryCapacityValidation(size_t maxWorkingMemory) {
    ValidationFunction validator = [maxWorkingMemory](const ConfigValue& val) {
        if (const auto* intPtr = std::get_if<int>(&val)) {
            return *intPtr >= 0 && static_cast<size_t>(*intPtr) <= maxWorkingMemory;
        } else if (const auto* int64Ptr = std::get_if<int64_t>(&val)) {
            return *int64Ptr >= 0 && static_cast<size_t>(*int64Ptr) <= maxWorkingMemory;
        } else if (const auto* doublePtr = std::get_if<double>(&val)) {
            return *doublePtr >= 0.0 && static_cast<size_t>(*doublePtr) <= maxWorkingMemory;
        }
        return false;
    };
    
    return addValidation("working_memory_capacity", validator);
}

bool Config::addTimeStepValidation(double min, double max) {
    ValidationFunction validator = [min, max](const ConfigValue& val) {
        if (const auto* doublePtr = std::get_if<double>(&val)) {
            return *doublePtr >= min && *doublePtr <= max;
        }
        return false;
    };
    
    return addValidation("simulation_timestep", validator);
}

bool Config::addRegionValidation(size_t maxRegions) {
    ValidationFunction validator = [maxRegions](const ConfigValue& val) {
        if (const auto* intPtr = std::get_if<int>(&val)) {
            return *intPtr >= 1 && static_cast<size_t>(*intPtr) <= maxRegions;
        } else if (const auto* int64Ptr = std::get_if<int64_t>(&val)) {
            return *int64Ptr >= 1 && static_cast<size_t>(*int64Ptr) <= maxRegions;
        }
        return false;
    };
    
    return addValidation("region_count", validator);
}

ConfigValue Config::getValue(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        return std::string();
    }
    
    return it->value;
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
    
    // Remove associated validators
    pImpl->validators.erase(key);
}

std::vector<std::string> Config::getKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->entries.size());
    for (const auto& entry : pImpl->entries) {
        keys.push_back(entry.key);
    }
    return keys;
}

std::vector<std::string> Config::getValidationKeys() const {
    std::vector<std::string> keys;
    keys.reserve(pImpl->validators.size());
    for (const auto& pair : pImpl->validators) {
        keys.push_back(pair.first);
    }
    return keys;
}

void Config::clear() {
    pImpl->entries.clear();
    pImpl->validators.clear();
    pImpl->validationErrors.clear();
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

std::string Config::validationSummary() const {
    std::ostringstream oss;
    oss << "Validation Summary (" << pImpl->validationErrors.size() << " errors):\n";
    
    if (pImpl->validationErrors.empty()) {
        oss << "  All valid!\n";
    } else {
        for (const auto& error : pImpl->validationErrors) {
            oss << "  - " << error << "\n";
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

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

bool Config::isNumeric(const std::string& str) {
    return !str.empty() && std::find_if(str.begin(), str.end(), [](char c) { return !std::isdigit(c) && c != '.' && c != '-'; }) == str.end();
}

ConfigValue Config::stringToConfigValue(const std::string& str) {
    if (isNumeric(str)) {
        // Check for integer
        try {
            size_t pos;
            long long_val = std::stoll(str);
            if (str.find('.') == std::string::npos) {
                if (long_val >= std::numeric_limits<int>::min() && long_val <= std::numeric_limits<int>::max()) {
                    return static_cast<int>(long_val);
                }
                return long_val;
            }
        } catch (const std::exception&) {}
        
        // Check for double
        try {
            double double_val = std::stod(str);
            return double_val;
        } catch (const std::exception&) {}
    }
    
    if (str == "true") return true;
    if (str == "false") return false;
    
    return str;
}

// Explicit template instantiations

// Explicit instantiations

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

