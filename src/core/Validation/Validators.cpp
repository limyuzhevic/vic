#include "Validators.hpp"
#include <cmath>

namespace nlm {

// Simple implementation for validators

bool RangeValidator::validate(const ValidationContext& context) const {
    try {
        double value = context.getValueAs<double>();
        return validateRange(value);
    } catch (const InvalidConfigurationValue&) {
        return false;
    }
}

std::string RangeValidator::getErrorMessage(const ValidationContext& context) const {
    try {
        double value = context.getValueAs<double>();
        
        std::stringstream ss;
        ss << "Value '" << value << "' is out of range. ";
        
        bool minValid = (minType_ == BoundType::Inclusive) ? (value >= minValue_) : (value > minValue_);
        bool maxValid = (maxType_ == BoundType::Inclusive) ? (value <= maxValue_) : (value < maxValue_);
        
        if (!minValid) {
            ss << "Must be ";
            if (minType_ == BoundType::Inclusive) ss << ">= ";
            else ss << "> ";
            ss << minValue_;
        }
        
        if (!maxValid) {
            if (!minValid) ss << " and ";
            ss << "must be ";
            if (maxType_ == BoundType::Inclusive) ss << "<= ";
            else ss << "< ";
            ss << maxValue_;
        }
        
        return ss.str();
    } catch (const InvalidConfigurationValue&) {
        return "Value has incorrect type for range validation";
    }
}

bool StringValidator::validate(const ValidationContext& context) const {
    try {
        std::string value = context.getValueAs<std::string>();
        return validateString(value);
    } catch (const InvalidConfigurationValue&) {
        return false;
    }
}

std::string StringValidator::getErrorMessage(const ValidationContext& context) const {
    try {
        std::string value = context.getValueAs<std::string>();
        
        std::stringstream ss;
        ss << "String value '" << value << "' is invalid. ";
        
        if (value.length() < minLength_ || value.length() > maxLength_) {
            ss << "Length must be between " << minLength_ << " and " << maxLength_ << ".";
            return ss.str();
        }
        
        return ss.str() + "Pattern validation failed.";
    } catch (const InvalidConfigurationValue&) {
        return "String validation failed - value is not a string";
    }
}

bool StringValidator::validateString(const std::string& value) const {
    // Check length constraints
    if (value.length() < minLength_ || value.length() > maxLength_) {
        return false;
    }
    
    // Pattern validation - simplified implementation
    if (patternType_ == PatternType::Regex) {
        try {
            return std::regex_match(value, std::regex(pattern_));
        } catch (const std::regex_error&) {
            return false;
        }
    }
    
    return true;
}

bool ListValidator::validate(const ValidationContext& context) const {
    try {
        // Try common vector types
        auto value = context.getValueAs<std::vector<double>>();
        return value.size() >= minSize_ && value.size() <= maxSize_;
    } catch (const InvalidConfigurationValue&) {
        try {
            auto value = context.getValueAs<std::vector<int>>();
            return value.size() >= minSize_ && value.size() <= maxSize_;
        } catch (const InvalidConfigurationValue&) {
            try {
                auto value = context.getValueAs<std::vector<std::string>>();
                return value.size() >= minSize_ && value.size() <= maxSize_;
            } catch (const InvalidConfigurationValue&) {
                return false;
            }
        }
    }
}

std::string ListValidator::getErrorMessage(const ValidationContext& context) const {
    try {
        std::stringstream ss;
        
        try {
            auto value = context.getValueAs<std::vector<double>>();
            ss << "List size is " << value.size() << ", but must be between " << minSize_ << " and " << maxSize_ << ".";
            return ss.str();
        } catch (const InvalidConfigurationValue&) {}
        
        try {
            auto value = context.getValueAs<std::vector<int>>();
            ss << "List size is " << value.size() << ", but must be between " << minSize_ << " and " << maxSize_ << ".";
            return ss.str();
        } catch (const InvalidConfigurationValue&) {}
        
        try {
            auto value = context.getValueAs<std::vector<std::string>>();
            ss << "List size is " << value.size() << ", but must be between " << minSize_ << " and " << maxSize_ << ".";
            return ss.str();
        } catch (const InvalidConfigurationValue&) {}
        
        return "List validation failed - value is not a list";
    } catch (const InvalidConfigurationValue&) {
        return "List validation failed";
    }
}

} // namespace nlm