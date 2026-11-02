#ifndef PYNLN_BINDINGS_UTILS_H
#define PYNLN_BINDINGS_UTILS_H

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

namespace py = pybind11;

// Common utility functions and error handling for Python bindings
namespace nlm::bindings {

// Custom exception for Python bindings
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

// Helper function to validate non-negative values
inline void validateNonNegative(double value, const std::string& name) {
    if (value < 0) {
        throw NLMException(name + " must be non-negative, got " + std::to_string(value));
    }
}

// Helper function to validate ranges
inline void validateRange(double value, double min, double max, const std::string& name) {
    if (value < min || value > max) {
        throw NLMException(name + " must be between " + std::to_string(min) + 
                          " and " + std::to_string(max) + ", got " + std::to_string(value));
    }
}

// Helper function to convert Python string to lowercase
inline std::string toLowercase(const std::string& str) {
    std::string result = str;
    for (char& c : result) {
        c = std::tolower(c);
    }
    return result;
}

} // namespace nlm::bindings

#endif // PYNLN_BINDINGS_UTILS_H