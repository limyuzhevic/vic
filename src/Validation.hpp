// Validation.hpp
// Input validation and parameter checking for NLM

#ifndef NLM_VALIDATION_HPP
#define NLM_VALIDATION_HPP

#include <string>
#include <vector>
#include <functional>
#include <iostream>

namespace nlm {

// Validation check structure
struct ValidationCheck {
    std::string name;                    ///< Name of the validation check
    std::function<bool()> func;          ///< Function that performs the validation
    std::string errorMessage;            ///< Error message if validation fails
};

// Validation class
class Validation {
private:
    bool isStrict = false;
    bool abortOnError = true;
    std::vector<ValidationCheck> validationChecks;
    
public:
    void setStrictMode(bool strict);
    bool isStrictMode() const;
    
    void setAbortOnError(bool abort);
    bool shouldAbortOnError() const;
    
    // Add various types of validation checks
    void addCheck(const std::string& name, std::function<bool()> checkFunc, const std::string& errorMessage);
    void addRangeCheck(const std::string& name, double value, double min, double max, const std::string& errorMessage);
    void addRangeCheck(const std::string& name, int value, int min, int max, const std::string& errorMessage);
    void addRangeCheck(const std::string& name, size_t value, size_t min, size_t max, const std::string& errorMessage);
    void addFileExistsCheck(const std::string& name, const std::string& filepath, const std::string& errorMessage);
    
    // Run all validation checks
    bool runAllChecks();
    
    // Utility functions
    void clearChecks();
    
private:
    // Helper function to validate ranges
    template<typename T>
    bool validateRange(T value, T min, T max, const std::string& errorMessage);
};

} // namespace nlm

#endif // NLM_VALIDATION_HPP