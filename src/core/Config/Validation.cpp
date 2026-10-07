// Implementation of configuration validation system

#include "Validation.hpp"

namespace nlm {

// RangeValidator implementation
bool RangeValidator::validate(const ConfigValue& value) const {
    try {
        double d = std::get<double>(value);
        if (minType == BoundType::Inclusive && maxType == BoundType::Inclusive) {
            return minValue <= d && d <= maxValue;
        } else if (minType == BoundType::Inclusive && maxType == BoundType::Exclusive) {
            return minValue <= d && d < maxValue;
        } else if (minType == BoundType::Exclusive && maxType == BoundType::Inclusive) {
            return d < minValue && d <= maxValue;
        } else {
            return d < minValue && d < maxValue;
        }
    } catch (const std::bad_variant_access&) {
        return false;
    }
}

// ConfigSchemaEntry::validate implementation
bool ConfigSchemaEntry::validate(const ConfigValue& value) const {
    // Check type
    if (typeValidator && !typeValidator(value)) {
        return false;
    }
    
    // Check range if available
    if (rangeValidator && !rangeValidator->validate(value)) {
        return false;
    }
    
    // Note: customValidator is not called here as it's meant to be called during ConfigSchema::validate
    // which has access to the full Config object for context
    
    return true;
}
