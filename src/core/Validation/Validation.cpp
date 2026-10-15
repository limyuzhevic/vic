#include "Validation.hpp"

namespace nlm {

// Global schema instance definition
Phase6ConfigSchema phase6Schema;

// Validation error implementation
std::string ConfigValidationError::formatMessage(const std::string& message, 
                                                 const std::string& key,
                                                 const std::string& value,
                                                 const std::string& expected) {
    std::ostringstream oss;
    oss << message;
    if (!key.empty()) oss << " (key: '" << key << "')";
    if (!value.empty()) oss << " (value: '" << value << "')";
    if (!expected.empty()) oss << " (expected: '" << expected << "')";
    return oss.str();
}

} // namespace nlm
