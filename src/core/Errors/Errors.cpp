#include "Errors.hpp"

namespace nlm {

// Exception class implementations (inline)

NLMError::NLMError(const std::string& message) 
    : std::runtime_error("NLM: " + message) {}

// All other exception classes are simple and inline

} // namespace nlm
