#pragma once

#include <string>
#include <stdexcept>

namespace nlm {

// Configuration validation error classes
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& message) 
        : std::runtime_error(message) {}
};

class ConfigValidationError : public ConfigError {
public:
    ConfigValidationError(const std::string& message, const std::string& key = "")
        : ConfigError(message), key_(key) {}
    
    const std::string& getKey() const { return key_; }
    
private:
    std::string key_;
};

class ConfigRangeError : public ConfigValidationError {
public:
    ConfigRangeError(const std::string& message, const std::string& key = "", 
                    double value = 0.0, double min = 0.0, double max = 0.0)
        : ConfigValidationError(message, key),
          value_(value), min_(min), max_(max) {}
    
    double getValue() const { return value_; }
    double getMin() const { return min_; }
    double getMax() const { return max_; }
    
private:
    double value_;
    double min_;
    double max_;
};

class ConfigTypeError : public ConfigValidationError {
public:
    ConfigTypeError(const std::string& message, const std::string& key = "",
                   const std::string& expectedType = "", const std::string& actualType = "")
        : ConfigValidationError(message, key),
          expectedType_(expectedType), actualType_(actualType) {}
    
    const std::string& getExpectedType() const { return expectedType_; }
    const std::string& getActualType() const { return actualType_; }
    
private:
    std::string expectedType_;
    std::string actualType_;
};

} // namespace nlm