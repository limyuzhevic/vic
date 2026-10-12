// Error handling utilities implementation

#include "ErrorHandling.hpp"

namespace nlm {

// Implementation of exception classes
const char* NLMException::what() const noexcept {
    return std::runtime_error::what();
}

} // namespace nlm