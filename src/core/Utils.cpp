// Utility functions implementation for common patterns

#include "core/Utils.hpp"
#include "core/Constants.hpp"

namespace nlm {

namespace Utils {

// Input validation implementations
void validateNotNull(const void* ptr, const std::string& context) {
    if (!ptr) {
        throw Error(Error::Code::NullPointer, "Null pointer provided" + 
                   (context.empty() ? "" : " in " + context));
    }
}

void validateRange(double value, double min, double max, const std::string& context) {
    if (value < min || value > max) {
        throw Error(Error::Code::OutOfRange, 
                   "Value " + std::to_string(value) + " outside range [" + 
                   std::to_string(min) + ", " + std::to_string(max) + "]" +
                   (context.empty() ? "" : " in " + context));
    }
}

void validateGreaterThan(double value, double min, const std::string& context) {
    if (value <= min) {
        throw Error(Error::Code::OutOfRange,
                   "Value " + std::to_string(value) + " must be > " + 
                   std::to_string(min) + (context.empty() ? "" : " in " + context));
    }
}

void validateLessThan(double value, double max, const std::string& context) {
    if (value >= max) {
        throw Error(Error::Code::OutOfRange,
                   "Value " + std::to_string(value) + " must be < " + 
                   std::to_string(max) + (context.empty() ? "" : " in " + context));
    }
}

void validateNonNegative(double value, const std::string& context) {
    if (value < 0.0) {
        throw Error(Error::Code::OutOfRange,
                   "Value " + std::to_string(value) + " must be non-negative" +
                   (context.empty() ? "" : " in " + context));
    }
}

void validatePositive(double value, const std::string& context) {
    if (value <= 0.0) {
        throw Error(Error::Code::OutOfRange,
                   "Value " + std::to_string(value) + " must be positive" +
                   (context.empty() ? "" : " in " + context));
    }
}

void validateArraysNotEmpty(const std::vector<float>& arr, const std::string& context) {
    if (arr.empty()) {
        throw Error(Error::Code::ValidationError,
                   "Array is empty" + (context.empty() ? "" : " in " + context));
    }
}

void validateArraysSameSize(const std::vector<float>& arr1, const std::vector<float>& arr2, const std::string& context) {
    if (arr1.size() != arr2.size()) {
        throw Error(Error::Code::ValidationError,
                   "Array size mismatch: " + std::to_string(arr1.size()) + " vs " + 
                   std::to_string(arr2.size()) + (context.empty() ? "" : " in " + context));
    }
}

} // namespace Utils

} // namespace nlm
