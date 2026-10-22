#pragma once

#include "Config/Config.hpp"
#include "Exceptions/Exceptions.hpp"
#include <stdexcept>

namespace nlm {

// Enhanced configuration validation functions
class ConfigValidator {
public:
    static void validateValue(const std::string& key, const ConfigValue& value, 
                             const std::string& description = "");
    
    static void validateRange(const std::string& key, double value, 
                             double min, double max, const std::string& description = "");
    
    static void validatePositive(const std::string& key, double value, 
                                const std::string& description = "");
    
    static void validateNonNegative(const std::string& key, double value, 
                                   const std::string& description = "");
    
    static void validateEnum(const std::string& key, int64_t value, 
                            const std::string& description = "");
    
    static void validateString(const std::string& key, const std::string& value, 
                              size_t minLength, size_t maxLength, const std::string& description = "");
};

// Configuration helper functions
class ConfigHelper {
public:
    static std::string getValueDescription(const std::string& key);
    
    static std::string getErrorPrefix(const ConfigEntry& entry);
    
    static void setValueWithValidation(const std::string& key, const std::string& value,
                                      ConfigSource source = ConfigSource::Runtime);
};

} // namespace nlm
