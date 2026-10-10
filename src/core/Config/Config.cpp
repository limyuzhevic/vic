#include "Config.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace nlm {

// Configuration validation and parsing improvements
namespace {
    // Configuration schema definitions for validation
    struct ConfigSchema {
        std::string key;
        std::function<bool(const ConfigValue&)> validator;
        std::string description;
        bool required;
        ConfigValue defaultValue;
        
        ConfigSchema(const std::string& k, std::function<bool(const ConfigValue&)> val, 
                    const std::string& desc, bool req = false, const ConfigValue& def = ConfigValue())
            : key(k), validator(val), description(desc), required(req), defaultValue(def) {}
    };
    
    // Type-specific validators
    bool validateInt(const ConfigValue& value) {
        return std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value);
    }
    
    bool validateFloat(const ConfigValue& value) {
        return std::holds_alternative<double>(value) || std::holds_alternative<float>(value);
    }
    
    bool validateString(const ConfigValue& value) {
        return std::holds_alternative<std::string>(value);
    }
    
    bool validateBool(const ConfigValue& value) {
        return std::holds_alternative<bool>(value);
    }
    
    bool validatePositiveInt(const ConfigValue& value) {
        if (std::holds_alternative<int>(value)) {
            return std::get<int>(value) >= 0;
        } else if (std::holds_alternative<int64_t>(value)) {
            return std::get<int64_t>(value) >= 0;
        }
        return false;
    }
    
    bool validateRangeFloat(const ConfigValue& value, float min, float max) {
        if (std::holds_alternative<double>(value)) {
            float val = static_cast<float>(std::get<double>(value));
            return val >= min && val <= max;
        } else if (std::holds_alternative<float>(value)) {
            return std::get<float>(value) >= min && std::get<float>(value) <= max;
        }
        return false;
    }
    
    // Configuration validation errors
    class ConfigValidationError : public std::runtime_error {
    public:
        ConfigValidationError(const std::string& message, const std::string& key)
            : std::runtime_error("Config error: " + message + " (key: " + key + ")"), key_(key) {}
        
        const std::string& getKey() const { return key_; }
    
    private:
        std::string key_;
    };
    
    // Enhanced configuration loading with better error handling
    class ConfigLoader {
    public:
        static bool loadFromFileEnhanced(Config& config, const std::string& filepath) {
            std::ifstream file(filepath);
            if (!file.is_open()) {
                NLM_LOG_WARNING("Configuration file not found: " + filepath);
                return false;
            }
            
            std::string line;
            size_t lineNum = 0;
            while (std::getline(file, line)) {
                ++lineNum;
                
                // Skip comments and empty lines
                line = Config::trim(line);
                if (line.empty() || line[0] == '#' || line[0] == '/') {
                    continue;
                }
                
                try {
                    // Parse line with enhanced error handling
                    size_t pos = line.find('=');
                    if (pos == std::string::npos) {
                        NLM_LOG_WARNING("Invalid config line " + std::to_string(lineNum) + ": no '=' found: " + line);
                        continue;
                    }
                    
                    std::string key = Config::trim(line.substr(0, pos));
                    std::string valueStr = Config::trim(line.substr(pos + 1));
                    
                    // Remove quotes
                    if (valueStr.size() >= 2 && 
                        ((valueStr.front() == '"' && valueStr.back() == '"') ||
                         (valueStr.front() == '\'' && valueStr.back() == '\''))) {
                        valueStr = valueStr.substr(1, valueStr.size() - 2);
                    }
                    
                    config.set(key, valueStr, ConfigSource::File);
                    
                } catch (const std::exception& e) {
                    NLM_LOG_ERROR("Error parsing config line " + std::to_string(lineNum) + ": " + std::string(e.what()));
                }
            }
            
            return true;
        }
        
        static bool validateConfig(const Config& config, const std::vector<ConfigSchema>& schema) {
            bool valid = true;
            
            for (const auto& entry : schema) {
                if (entry.required && !config.has(entry.key)) {
                    NLM_LOG_WARNING("Required configuration key missing: " + entry.key + " (" + entry.description + ")");
                    valid = false;
                    continue;
                }
                
                if (config.has(entry.key)) {
                    auto valueOpt = config.get<std::string>(entry.key);
                    if (valueOpt) {
                        // For now, just basic string validation
                        // In a full implementation, would parse based on type
                        ConfigValue testValue = *valueOpt;
                        if (entry.validator && !entry.validator(testValue)) {
                            NLM_LOG_ERROR("Configuration validation failed for key: " + entry.key + 
                                        " value: " + *valueOpt + " (" + entry.description + ")");
                            valid = false;
                        }
                    }
                }
            }
            
            return valid;
        }
    };
    
    // Performance optimization helpers
    class ConfigPerformanceOptimizer {
    public:
        static void optimizeMemoryUsage(Config& config) {
            // Remove unused entries to save memory
            auto keys = config.getKeys();
            for (const auto& key : keys) {
                // In a real implementation, would check if key is actually used
                // For now, keep all entries
            }
        }
        
        static void optimizeLookupSpeed(Config& config) {
            // Build index for faster lookups
            // Implementation would maintain reverse lookup maps
        }
    };

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
