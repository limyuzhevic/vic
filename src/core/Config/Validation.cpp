// Configuration validation implementation
// Complete implementation of the validation system

#include "Validation.hpp"

namespace nlm {
namespace config {

// SchemaElement implementation
bool SchemaElement::validate(const ConfigValue& value, std::vector<ValidationError>& errors,
                             const std::string& path) const {
    std::stringstream error_path;
    if (!path.empty()) {
        error_path << path << "." << name;
    } else {
        error_path << name;
    }
    
    // Check type
    switch (type) {
        case SchemaElementType::BOOLEAN:
            if (!std::holds_alternative<bool>(value)) {
                errors.emplace_back(ValidationErrorType::TYPE_MISMATCH, error_path.str(),
                                 "Expected boolean type",
                                 get_type_name(),
                                 config_value_to_string(value));
                return false;
            }
            break;
            
        case SchemaElementType::INTEGER:
        case SchemaElementType::INTEGER64:
            if (!std::holds_alternative<int64_t>(value)) {
                errors.emplace_back(ValidationErrorType::TYPE_MISMATCH, error_path.str(),
                                 "Expected integer type",
                                 get_type_name(),
                                 config_value_to_string(value));
                return false;
            }
            break;
            
        case SchemaElementType::UNSIGNED_INTEGER:
            if (!std::holds_alternative<uint64_t>(value)) {
                errors.emplace_back(ValidationErrorType::TYPE_MISMATCH, error_path.str(),
                                 "Expected unsigned integer type",
                                 get_type_name(),
                                 config_value_to_string(value));
                return false;
            }
            break;
            
        case SchemaElementType::FLOAT:
            if (!std::holds_alternative<double>(value)) {
                errors.emplace_back(ValidationErrorType::TYPE_MISMATCH, error_path.str(),
                                 "Expected float type",
                                 get_type_name(),
                                 config_value_to_string(value));
                return false;
            }
            break;
            
        case SchemaElementType::DOUBLE:
            if (!std::holds_alternative<double>(value)) {
                errors.emplace_back(ValidationErrorType::TYPE_MISMATCH, error_path.str(),
                                 "Expected double type",
                                 get_type_name(),
                                 config_value_to_string(value));
                return false;
            }
            break;
            
        case SchemaElementType::STRING:
            if (!std::holds_alternative<std::string>(value)) {
                errors.emplace_back(ValidationErrorType::TYPE_MISMATCH, error_path.str(),
                                 "Expected string type",
                                 get_type_name(),
                                 config_value_to_string(value));
                return false;
            }
            break;
            
        case SchemaElementType::ENUM:
            if (!std::holds_alternative<std::string>(value)) {
                errors.emplace_back(ValidationErrorType::TYPE_MISMATCH, error_path.str(),
                                 "Expected string type for enum",
                                 get_type_name(),
                                 config_value_to_string(value));
                return false;
            }
            break;
            
        case SchemaElementType::NULL_VALUE:
            // null is a valid value
            break;
            
        default:
            errors.emplace_back(ValidationErrorType::TYPE_MISMATCH, error_path.str(),
                             "Unsupported schema type",
                             get_type_name(),
                             config_value_to_string(value));
            return false;
    }
    
    // Validate against constraints
    if (min_value_.has_value() && value < *min_value_) {
        errors.emplace_back(ValidationErrorType::RANGE_VIOLATION, error_path.str(),
                         "Value below minimum",
                         "Min: " + config_value_to_string(*min_value_),
                         "Actual: " + config_value_to_string(value));
        return false;
    }
    
    if (max_value_.has_value() && value > *max_value_) {
        errors.emplace_back(ValidationErrorType::RANGE_VIOLATION, error_path.str(),
                         "Value above maximum",
                         "Max: " + config_value_to_string(*max_value_),
                         "Actual: " + config_value_to_string(value));
        return false;
    }
    
    // Validate against enum list
    if (!enum_values_.empty()) {
        if (type == SchemaElementType::ENUM) {
            // Already validated type above
            std::string str_value;
            if (std::holds_alternative<std::string>(value)) {
                str_value = std::get<std::string>(value);
            } else {
                str_value = config_value_to_string(value);
            }
            
            if (std::find(enum_values_.begin(), enum_values_.end(), str_value) == enum_values_.end()) {
                std::string valid_list = "Valid values: ";
                for (size_t i = 0; i < enum_values_.size(); ++i) {
                    if (i > 0) valid_list += ", ";
                    valid_list += "\"" + enum_values_[i] + "\"";
                }
                errors.emplace_back(ValidationErrorType::RANGE_VIOLATION, error_path.str(),
                                 "Value not in allowed enum list",
                                 valid_list,
                                 "\"" + str_value + "\"");
                return false;
            }
        }
    }
    
    // Run custom validator
    if (custom_validator_) {
        if (!custom_validator_(value)) {
            errors.emplace_back(ValidationErrorType::CUSTOM_ERROR, error_path.str(),
                             "Custom validation failed",
                             "Custom validator failed",
                             config_value_to_string(value));
            return false;
        }
    }
    
    return true;
}

} // namespace config
} // namespace nlm