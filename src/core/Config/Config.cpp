// Add includes for configuration validation
#include <unordered_map>
#include <vector>
#include <functional>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <limits>

namespace nlm {

// Configuration validation namespace
namespace config_validation {

// Helper to validate numeric values against ranges
bool validateRange(const ConfigValue& value, double min, double max) {
    if (std::holds_alternative<int>(value) && 
        (std::get<int>(value) < min || std::get<int>(value) > max)) {
        return false;
    } else if (std::holds_alternative<int64_t>(value) && 
               (std::get<int64_t>(value) < min || std::get<int64_t>(value) > max)) {
        return false;
    } else if (std::holds_alternative<double>(value) && 
               (std::get<double>(value) < min || std::get<double>(value) > max)) {
        return false;
    } else if (std::holds_alternative<float>(value) && 
               (std::get<float>(value) < min || std::get<float>(value) > max)) {
        return false;
    }
    return true;
}

// Helper to validate string against allowed values
bool validateAllowedValues(const ConfigValue& value, const std::vector<std::string>& allowed) {
    if (std::holds_alternative<std::string>(value)) {
        const std::string& val = std::get<std::string>(value);
        return std::find(allowed.begin(), allowed.end(), val) != allowed.end();
    }
    return false; // Non-string values are not allowed for string parameters
}

// Helper to validate that the value matches the expected type
bool validateType(const ConfigValue& value, const std::string& key, ConfigSource source) {
    // Type validation is already handled by the Config class template methods
    return true;
}

// Helper to validate configuration parameter
bool validateParameter(const std::string& key, const ConfigValue& value,
                      const ConfigSource& source,
                      const std::unordered_map<std::string, std::vector<std::function<bool(const ConfigValue&)>>>& validators,
                      const std::unordered_map<std::string, ConfigValue>& defaults,
                      const std::unordered_map<std::string, std::pair<double, double>>& valueRanges,
                      const std::unordered_map<std::string, std::vector<std::string>>& allowedValues) {
    
    // Always pass validation for basic types
    if (std::holds_alternative<int>(value) || std::holds_alternative<int64_t>(value) ||
        std::holds_alternative<double>(value) || std::holds_alternative<float>(value) ||
        std::holds_alternative<bool>(value) || std::holds_alternative<std::string>(value) ||
        std::holds_alternative<std::vector<int>>(value) ||
        std::holds_alternative<std::vector<double>>(value) ||
        std::holds_alternative<std::vector<std::string>>(value)) {
        return true;
    }
    
    return false;
}

// Validate all configuration entries
bool validateConfig(const Config::Impl& impl) {
    // Basic validation - in a full implementation, this would check against
    // schema, validate types, ranges, allowed values, etc.
    return true;
}

// Get default value for a key
bool getDefault(const std::string& key, ConfigValue& value,
                const std::unordered_map<std::string, ConfigValue>& defaults) {
    auto it = defaults.find(key);
    if (it != defaults.end()) {
        value = it->second;
        return true;
    }
    return false;
}

// Post-process configuration after loading
void postProcessConfig(Config::Impl& impl,
                      const std::unordered_map<std::string, std::function<void(ConfigValue&)>>& processors,
                      const std::unordered_map<std::string, ConfigValue>& defaults) {
    for (auto& entry : impl.entries) {
        // Apply post-processing if available
        auto it = processors.find(entry.key);
        if (it != processors.end()) {
            it->second(entry.value);
        }
        
        // Ensure value is not empty for required keys
        if (defaults.find(entry.key) != defaults.end() && 
            std::holds_alternative<std::monostate>(entry.value)) {
            entry.value = defaults.at(entry.key);
        }
    }
}

// Validate configuration parameter against schema
bool validateConfigParameter(const std::string& key, const ConfigValue& value,
                            const std::unordered_map<std::string, std::function<bool(const ConfigValue&)>>& validators,
                            const std::unordered_map<std::string, std::pair<double, double>>& valueRanges,
                            const std::unordered_map<std::string, std::vector<std::string>>& allowedValues) {
    
    // Check custom validators
    auto validatorIt = validators.find(key);
    if (validatorIt != validators.end() && !validatorIt->second(value)) {
        return false;
    }
    
    // Check value ranges
    auto rangeIt = valueRanges.find(key);
    if (rangeIt != valueRanges.end()) {
        if (!validateRange(value, rangeIt->second.first, rangeIt->second.second)) {
            return false;
        }
    }
    
    // Check allowed values
    auto allowedIt = allowedValues.find(key);
    if (allowedIt != allowedValues.end()) {
        if (!validateAllowedValues(value, allowedIt->second)) {
            return false;
        }
    }
    
    return true;
}

} // namespace config_validation

