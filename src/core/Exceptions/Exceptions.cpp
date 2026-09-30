#include "Exceptions.hpp"

namespace nlm {

// ExceptionFactory implementation
std::unique_ptr<NLMException> ExceptionFactory::createValidationError(const std::string& component, const std::string& message) {
    return std::make_unique<NLMException>(component + ": Validation error - " + message);
}
    
std::unique_ptr<NLMException> ExceptionFactory::createNullPointerError(const std::string& component, const std::string& operation) {
    return std::make_unique<NLMException>(component + ": Null pointer error in " + operation);
}
    
std::unique_ptr<NLMException> ExceptionFactory::createRuntimeError(const std::string& component, const std::string& message) {
    return std::make_unique<NLMException>(component + ": " + message);
}

} // namespace nlm
