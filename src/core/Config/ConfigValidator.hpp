#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <variant>
#include <functional>
#include <chrono>
#include <cmath>
#include <limits>

namespace nlm {

class ConfigValidator {
public:
    ConfigValidator();
    ~ConfigValidator();
    
    // Validate individual value
    std::vector<std::string> validate(const std::string& key,
                                     const std::variant<int, int64_t, double, bool, std::string>& value,
                                     const std::string& type = "",
                                     const std::optional<double>& min = std::nullopt,
                                     const std::optional<double>& max = std::nullopt,
                                     const std::string& pattern = "");
    
    // Validate numeric range
    std::vector<std::string> validateRange(const std::string& key,
                                          const std::variant<int, int64_t, double, bool, std::string>& value,
                                          double min, double max);
    
    // Validate pattern (simple regex support)
    std::vector<std::string> validatePattern(const std::string& key,
                                            const std::variant<int, int64_t, double, bool, std::string>& value,
                                            const std::string& pattern);
    
    // Validate cross-parameter constraints
    std::vector<std::string> validateCrossParameters(const std::unordered_map<std::string, 
                                             std::variant<int, int64_t, double, bool, std::string>>& values,
                                            const std::vector<std::function<bool(const std::unordered_map<std::string, 
                                             std::variant<int, int64_t, double, bool, std::string>>&)>>& validators);
    
    // Validate custom condition
    std::vector<std::string> validateCustom(const std::string& key,
                                           const std::variant<int, int64_t, double, bool, std::string>& value,
                                           std::function<bool(const std::variant<int, int64_t, double, bool, std::string>&)> validator,
                                           const std::string& errorMessage = "Custom validation failed");
    
    // Validate required field
    std::vector<std::string> validateRequired(const std::string& key,
                                             bool isProvided,
                                             const std::string& errorMessage = "Field is required");
    
    // Validate type
    std::vector<std::string> validateType(const std::string& key,
                                         const std::variant<int, int64_t, double, bool, std::string>& value,
                                         const std::string& expectedType);
    
    // Format value
    std::string formatValue(const std::string& key,
                           const std::variant<int, int64_t, double, bool, std::string>& value,
                           const std::string& type = "",
                           const std::function<std::string(const std::variant<int, int64_t, double, bool, std::string>&)>& formatter = nullptr);
    
    // Get validation statistics
    size_t getTotalValidations() const { return totalValidations_; }
    size_t getPassedValidations() const { return passedValidations_; }
    size_t getFailedValidations() const { return failedValidations_; }
    double getSuccessRate() const { return totalValidations_ > 0 ? (double)passedValidations_ / totalValidations_ * 100.0 : 0.0; }
    std::chrono::system_clock::time_point getLastValidationTime() const { return lastValidationTime_; }
    
    // Reset statistics
    void resetStatistics();
    
    // Configuration validation presets
    static std::vector<std::string> getDefaultPatternValidaitonErrors(const std::string& key,
                                                                   const std::string& pattern,
                                                                   const std::string& value);
    static std::vector<std::string> getRangeValidationErrors(const std::string& key,
                                                          double value,
                                                          double min,
                                                          double max);
    static std::vector<std::string> getTypeValidationErrors(const std::string& key,
                                                         const std::variant<int, int64_t, double, bool, std::string>& value,
                                                         const std::string& expectedType);
    
    // Parser helpers
    static bool isNumeric(const std::string& str);
    static bool isBoolean(const std::string& str);
    static std::string toLower(const std::string& str);

private:
    std::chrono::system_clock::time_point lastValidationTime_;
    size_t totalValidations_;
    size_t passedValidations_;
    size_t failedValidations_;
    
    bool matchesPattern(const std::string& value, const std::string& pattern);
    double extractNumericValue(const std::variant<int, int64_t, double, bool, std::string>& value) const;
};

} // namespace nlm