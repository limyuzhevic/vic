#include "ValidationExceptions.hpp"

namespace nlm {

// Simple implementation for validation exceptions
ValidationResult::ValidationResult() : isValid(true) {}

std::string ValidationResult::toString() const {
    std::stringstream ss;
    if (!errors.empty()) {
        ss << "Validation Errors:\n";
        for (const auto& error : errors) {
            ss << "  - " << error << "\n";
        }
    }
    if (!warnings.empty()) {
        if (!errors.empty()) ss << "\n";
        ss << "Warnings:\n";
        for (const auto& warning : warnings) {
            ss << "  - " << warning << "\n";
        }
    }
    if (isValid && !errors.empty() && !warnings.empty()) {
        ss << "Valid (with warnings)\n";
    } else if (isValid) {
        ss << "Valid\n";
    } else {
        ss << "Invalid\n";
    }
    return ss.str();
}

} // namespace nlm