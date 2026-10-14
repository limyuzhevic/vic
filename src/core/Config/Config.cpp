#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <cmath>
#include <cstdint>
#include <string>
#include <map>
#include "../Logger/Logger.hpp"

namespace nlm {

class ConfigValidationError : public std::runtime_error {
public:
    ConfigValidationError(const std::string& key, const std::string& value, const std::string& message)
        : std::runtime_error("Configuration error for key \"" + key + "\" (value: \"" + value + "\"): " + message),
          key_(key), value_(value), message_(message) {}
    
    const std::string& getKey() const { return key_; }
    const std::string& getValue() const { return value_; }
    const std::string& getMessage() const { return message_; }
    
private:
    std::string key_;
    std::string value_;
    std::string message_;
};

// Type validation and conversion functions
namespace ConfigValidation {
    bool validateIntRange(int64_t value, int64_t min, int64_t max, const std::string& key) {
        if (value < min || value > max) {
            NLM_LOG_ERROR("Configuration validation failed for key '" + key + "': " +
                         "value " + std::to_string(value) + " is outside valid range [" +
                         std::to_string(min) + ", " + std::to_string(max) + "]");
            return false;
        }
        return true;
    }
    
    bool validateFloatRange(double value, double min, double max, const std::string& key) {
        if (std::isnan(value) || value < min || value > max) {
            NLM_LOG_ERROR("Configuration validation failed for key '" + key + "': " +
                         "value " + std::to_string(value) + " is outside valid range [" +
                         std::to_string(min) + ", " + std::to_string(max) + "]");
            return false;
        }
        return true;
    }
    
    bool validateProbability(float value, const std::string& key) {
        if (value < 0.0f || value > 1.0f) {
            NLM_LOG_ERROR("Configuration validation failed for key '" + key + "': " +
                         "probability " + std::to_string(value) + " must be between 0.0 and 1.0");
            return false;
        }
        return true;
    }
    
    bool validatePositive(int64_t value, const std::string& key) {
        if (value <= 0) {
            NLM_LOG_ERROR("Configuration validation failed for key '" + key + "': " +
                         "positive value required, got " + std::to_string(value));
            return false;
        }
        return true;
    }
    
    bool validatePositive(double value, const std::string& key) {
        if (value <= 0.0) {
            NLM_LOG_ERROR("Configuration validation failed for key '" + key + "': " +
                         "positive value required, got " + std::to_string(value));
            return false;
        }
        return true;
    }
    
    bool validateLogLevel(const std::string& value, const std::string& key) {
        static const std::map<std::string, bool> validLevels = {
            {"debug", true}, {"info", true}, {"warning", true}, {"error", true}, {"critical", true}
        };
        
        if (validLevels.find(value) == validLevels.end()) {
            NLM_LOG_ERROR("Configuration validation failed for key '" + key + "': " +
                         "invalid log level '" + value + "' (valid: debug, info, warning, error, critical)");
            return false;
        }
        return true;
    }
    
    // Default validation constraints
    const std::map<std::string, std::function<bool(const std::string&, const std::string&)>> getDefaultValidators() {
        return {
            {"random_seed", [](const std::string& value, const std::string& key) {
                try {
                    int64_t val = std::stoll(value);
                    return val >= 0;  // Allow 0 for random seed
                } catch (...) { return false; }
            }},
            {"simulation_timestep", [](const std::string& value, const std::string& key) {
                try {
                    double val = std::stod(value);
                    return val > 0.0;
                } catch (...) { return false; }
            }},
            {"neuron_count", [](const std::string& value, const std::string& key) {
                try {
                    int64_t val = std::stoll(value);
                    return val >= 10 && val <= 10000;
                } catch (...) { return false; }
            }},
            {"region_count", [](const std::string& value, const std::string& key) {
                try {
                    int64_t val = std::stoll(value);
                    return val >= 1 && val <= 100;
                } catch (...) { return false; }
            }},
            {"connection_probability", [](const std::string& value, const std::string& key) {
                try {
                    float val = std::stof(value);
                    return val >= 0.0f && val <= 1.0f;
                } catch (...) { return false; }
            }},
            {"log_level", validateLogLevel},
            {"max_simulation_steps", [](const std::string& value, const std::string& key) {
                try {
                    int64_t val = std::stoll(value);
                    return val >= 0;
                } catch (...) { return false; }
            }},
            {"environment_width", [](const std::string& value, const std::string& key) {
                try {
                    int64_t val = std::stoll(value);
                    return val > 0;
                } catch (...) { return false; }
            }},
            {"environment_height", [](const std::string& value, const std::string& key) {
                try {
                    int64_t val = std::stoll(value);
                    return val > 0;
                } catch (...) { return false; }
            }},
            {"plasticity_learning_rate", [](const std::string& value, const std::string& key) {
                try {
                    double val = std::stod(value);
                    return val > 0.0 && val <= 1.0;
                } catch (...) { return false; }
            }},
            {"stdp_ltp_weight", [](const std::string& value, const std::string& key) {
                try {
                    float val = std::stof(value);
                    return val >= 0.0f && val <= 1.0f;
                } catch (...) { return false; }
            }},
            {"stdp_ltd_weight", [](const std::string& value, const std::string& key) {
                try {
                    float val = std::stof(value);
                    return val >= 0.0f && val <= 1.0f;
                } catch (...) { return false; }
            }},
            {"synaptogenesis_rate", [](const std::string& value, const std::string& key) {
                try {
                    float val = std::stof(value);
                    return val >= 0.0f && val <= 1.0f;
                } catch (...) { return false; }
            }},
            {"pruning_rate", [](const std::string& value, const std::string& key) {
                try {
                    float val = std::stof(value);
                    return val >= 0.0f && val <= 1.0f;
                } catch (...) { return false; }
            }},
            {"reward_discount_factor", [](const std::string& value, const std::string& key) {
                try {
                    double val = std::stod(value);
                    return val > 0.0 && val <= 1.0;
                } catch (...) { return false; }
            }},
            {"visualization_update_rate", [](const std::string& value, const std::string& key) {
                try {
                    double val = std::stod(value);
                    return val > 0.0;
                } catch (...) { return false; }
            }},
            {"num_worker_threads", [](const std::string& value, const std::string& key) {
                try {
                    int64_t val = std::stoll(value);
                    return val >= 1 && val <= 32;
                } catch (...) { return false; }
            }},
            {"memory_pool_size", [](const std::string& value, const std::string& key) {
                try {
                    int64_t val = std::stoll(value);
                    return val >= 1 && val <= 10000;
                } catch (...) { return false; }
            }}
        };
    }
    
    bool validateWithDefaultConstraints(const std::string& key, const std::string& value) {
        const auto& validators = getDefaultValidators();
        auto it = validators.find(key);
        if (it != validators.end()) {
            return it->second(value, key);
        }
        return true;  // No validation constraints defined for this key
    }
}

struct Config::Impl {
    std::vector<ConfigEntry> entries;
    std::map<std::string, std::string> descriptions;
};

Config::Config() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("Config initialized");
}

Config::~Config() = default;

Config::Config(Config&&) noexcept = default;

Config& Config::operator=(Config&&) noexcept = default;

bool Config::loadFromFile(const std::string& filepath) {
    NLM_LOG_INFO("Loading configuration from file: " + filepath);
    
    std::ifstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file: " + filepath);
        return false;
    }
    
    std::string line;
    int lineNum = 0;
    bool success = true;
    
    while (std::getline(file, line)) {
        lineNum++;
        
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
                // Validate the key-value pair
                if (!ConfigValidation::validateWithDefaultConstraints(key, value)) {
                    NLM_LOG_ERROR("Invalid configuration at line " + std::to_string(lineNum) + 
                                 " in file " + filepath + ": key '" + key + "' has invalid value '" + value + "'");
                    success = false;
                    continue;  // Skip this invalid entry
                }
                
                set(key, value, ConfigSource::File);
                if (!descriptions.count(key)) {
                    descriptions[key] = "Loaded from configuration file";
                }
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error parsing configuration at line " + std::to_string(lineNum) + 
                             " in file " + filepath + ": " + std::string(e.what()) +
                             " (line: '" + line + "')");
                success = false;
            }
        } else {
            NLM_LOG_WARNING("Skipping malformed configuration line " + std::to_string(lineNum) +
                           " in file " + filepath + ": '" + line + "'");
        }
    }
    
    if (success) {
        NLM_LOG_INFO("Successfully loaded configuration from file: " + filepath);
    } else {
        NLM_LOG_ERROR("Partial success loading configuration from file: " + filepath);
    }
    
    return success;
}

bool Config::loadFromArgs(int argc, char** argv) {
    NLM_LOG_INFO("Processing command line arguments for configuration");
    
    bool success = true;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        
        // Handle --key=value format
        if (arg.substr(0, 2) == "--") {
            size_t pos = arg.find('=');
            if (pos != std::string::npos) {
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                
                try {
                    // Validate the key-value pair
                    if (!ConfigValidation::validateWithDefaultConstraints(key, value)) {
                        NLM_LOG_ERROR("Invalid command line argument: key '" + key + 
                                     "' has invalid value '" + value + "'");
                        success = false;
                        continue;
                    }
                    
                    set(key, value, ConfigSource::CommandLine);
                    NLM_LOG_INFO("Command line configuration: " + key + " = " + value);
                    
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error processing command line argument --" + key + "=" + value + ": " + 
                                 std::string(e.what()));
                    success = false;
                }
            }
        }
        // Handle -key value format
        else if (arg[0] == '-' && i + 1 < argc) {
            std::string key = arg.substr(1);
            std::string value = argv[++i];
            
            try {
                // Validate the key-value pair
                if (!ConfigValidation::validateWithDefaultConstraints(key, value)) {
                    NLM_LOG_ERROR("Invalid command line argument: key '" + key + 
                                 "' has invalid value '" + value + "'");
                    success = false;
                    continue;
                }
                
                set(key, value, ConfigSource::CommandLine);
                NLM_LOG_INFO("Command line configuration: " + key + " = " + value);
                
            } catch (const std::exception& e) {
                NLM_LOG_ERROR("Error processing command line argument -" + key + " " + value + ": " + 
                             std::string(e.what()));
                success = false;
            }
        }
    }
    
    if (success) {
        NLM_LOG_INFO("Command line arguments processed successfully");
    } else {
        NLM_LOG_ERROR("Errors occurred while processing command line arguments");
    }
    
    return success;
}

bool Config::saveToFile(const std::string& filepath) const {
    NLM_LOG_INFO("Saving configuration to file: " + filepath);
    
    std::ofstream file(filepath);
    if (!file.is_open()) {
        NLM_LOG_ERROR("Failed to open configuration file for writing: " + filepath);
        return false;
    }
    
    // Write header
    file << "# NLM Configuration File\n";
    file << "# Generated on " << std::put_time(std::localtime(&std::time(nullptr)), "%Y-%m-%d %H:%M:%S") << "\n";
    file << "# Do not edit manually - use NLM configuration interface instead\n\n";
    
    for (const auto& entry : pImpl->entries) {
        if (descriptions.count(entry.key)) {
            file << "# " << descriptions[entry.key] << "\n";
        }
        file << entry.key << " = ";
        
        // Format value appropriately
        std::visit([&file](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                file << "\"" << arg << "\"";
            } else {
                file << arg;
            }
        }, entry.value);
        
        file << "\n\n";
    }
    
    file.flush();
    
    if (file.good()) {
        NLM_LOG_INFO("Successfully saved configuration to file: " + filepath);
        return true;
    } else {
        NLM_LOG_ERROR("Failed to write configuration to file: " + filepath);
        return false;
    }
}

template<typename T>
std::optional<T> Config::get(const std::string& key) const {
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it == pImpl->entries.end()) {
        NLM_LOG_DEBUG("Configuration key not found: '" + key + "'");
        return std::nullopt;
    }
    
    try {
        return std::get<T>(it->value);
    } catch (const std::bad_variant_access&) {
        NLM_LOG_ERROR("Type mismatch for configuration key '" + key + "': expected " +
                     typeid(T).name() + " but stored different type");
        return std::nullopt;
    }
}

template<typename T>
T Config::getOr(const std::string& key, const T& defaultValue) const {
    auto val = get<T>(key);
    if (val.has_value()) {
        return val.value();
    } else {
        NLM_LOG_DEBUG("Using default value for configuration key '" + key + "': " +
                     (std::is_same_v<T, std::string> ? ("\"" + std::to_string(defaultValue) + "\"") : std::to_string(defaultValue)));
        return defaultValue;
    }
}

void Config::set(const std::string& key, const ConfigValue& value, ConfigSource source) {
    // Validate configuration key format
    if (key.empty()) {
        throw std::invalid_argument("Configuration key cannot be empty");
    }
    
    if (key.find('=') != std::string::npos || key.find(' ') != std::string::npos) {
        throw std::invalid_argument("Configuration key cannot contain '=' or spaces: '" + key + "'");
    }
    
    auto it = std::find_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (it != pImpl->entries.end()) {
        // Check if value actually changed
        if (it->value == value && it->source == source) {
            NLM_LOG_DEBUG("Configuration key '" + key + "' unchanged (same value, same source)");
            return;
        }
        
        // Log change if source has higher priority than existing
        if (static_cast<int>(source) > static_cast<int>(it->source)) {
            NLM_LOG_INFO("Updating configuration key '" + key + "' from source " +
                        std::to_string(static_cast<int>(it->source)) + " to " +
                        std::to_string(static_cast<int>(source)));
        }
        
        it->value = value;
        it->source = source;
    } else {
        NLM_LOG_INFO("Setting new configuration key: '" + key + "' = [value]");
        pImpl->entries.emplace_back(key, value, source);
    }
}

void Config::set(const std::string& key, const std::string& value, ConfigSource source) {
    try {
        set(key, ConfigValue(value), source);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to set configuration key '" + key + "': " + std::string(e.what()));
        throw;
    }
}

void Config::set(const std::string& key, int value, ConfigSource source) {
    try {
        set(key, ConfigValue(value), source);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to set configuration key '" + key + "' to int value " + 
                     std::to_string(value) + ": " + std::string(e.what()));
        throw;
    }
}

void Config::set(const std::string& key, double value, ConfigSource source) {
    try {
        set(key, ConfigValue(value), source);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to set configuration key '" + key + "' to double value " + 
                     std::to_string(value) + ": " + std::string(e.what()));
        throw;
    }
}

void Config::set(const std::string& key, bool value, ConfigSource source) {
    try {
        set(key, ConfigValue(value), source);
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to set configuration key '" + key + "' to bool value " + 
                     (value ? "true" : "false") + ": " + std::string(e.what()));
        throw;
    }
}

bool Config::has(const std::string& key) const {
    bool exists = std::any_of(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (!exists) {
        NLM_LOG_DEBUG("Configuration key not found: '" + key + "'");
    }
    
    return exists;
}

void Config::remove(const std::string& key) {
    auto count = std::remove_if(pImpl->entries.begin(), pImpl->entries.end(),
        [&key](const ConfigEntry& e) { return e.key == key; });
    
    if (count != 0) {
        NLM_LOG_INFO("Removed configuration key: '" + key + "'");
        pImpl->entries.erase(count, pImpl->entries.end());
    } else {
        NLM_LOG_DEBUG("Configuration key not found for removal: '" + key + "'");
    }
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
    int count = pImpl->entries.size();
    pImpl->entries.clear();
    NLM_LOG_INFO("Cleared " + std::to_string(count) + " configuration entries");
}

std::string Config::summary() const {
    std::ostringstream oss;
    oss << "Configuration Summary (" << pImpl->entries.size() << " entries):\n";
    
    for (const auto& entry : pImpl->entries) {
        oss << "  Key: " << std::left << std::setw(25) << entry.key;
        
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << " = \"" << arg << "\"";
            } else {
                oss << " = " << arg;
            }
        }, entry.value);
        
        oss << " (source: " << static_cast<int>(entry.source) << ")\n";
    }
    
    oss << "\nTotal configuration size: ~" << pImpl->entries.size() * 50 << " bytes\n";
    
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

} // namespace nlm
