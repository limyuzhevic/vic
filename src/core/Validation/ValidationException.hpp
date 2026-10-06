#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>

namespace nlm {

/**
 * @enum ValidationErrorCategory
 * @brief Categories of validation errors for better error handling
 */
enum class ValidationErrorCategory {
    Type,          // Type validation errors
    Range,         // Range/value constraint errors
    Dependency,    // Dependency relationship errors
    Consistency,   // Consistency/conflict errors
    Schema,        // Schema validation errors
    Custom        // Custom validation errors
};

/**
 * @class ValidationException
 * @brief Exception thrown during configuration validation failures
 */
class ValidationException : public std::runtime_error {
public:
    ValidationException(const std::string& message, 
                       ValidationErrorCategory category = ValidationErrorCategory::Custom,
                       const std::string& key = "",
                       const std::string& context = "")
        : std::runtime_error(message),
          category_(category),
          key_(key),
          context_(context) {}

    ValidationErrorCategory getCategory() const { return category_; }
    const std::string& getKey() const { return key_; }
    const std::string& getContext() const { return context_; }

private:
    ValidationErrorCategory category_;
    std::string key_;
    std::string context_;
};

/**
 * @struct ValidationResult
 * @brief Structure holding the result of a validation operation
 */
struct ValidationResult {
    bool valid = true;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::vector<std::string> info;

    ValidationResult() = default;
    ValidationResult(bool isValid, const std::vector<std::string>& errs,
                     const std::vector<std::string>& warns = {},
                     const std::vector<std::string>& infos = {})
        : valid(isValid), errors(errs), warnings(warns), info(infos) {}

    bool isValid() const { return valid; }
    size_t errorCount() const { return errors.size(); }
    size_t warningCount() const { return warnings.size(); }
    size_t infoCount() const { return info.size(); }
};

} // namespace nlm
